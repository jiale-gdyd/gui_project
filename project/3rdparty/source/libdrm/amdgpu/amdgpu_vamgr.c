#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <libdrm/util_math.h>
#include <libdrm/drm/amdgpu.h>
#include <libdrm/drm/amdgpu_drm.h>

#include "amdgpu_internal.h"

drm_public int amdgpu_va_range_query(amdgpu_device_handle dev, enum amdgpu_gpu_va_range type, uint64_t *start, uint64_t *end)
{
    if (type != amdgpu_gpu_va_range_general) {
        return -EINVAL;
    }

    *start = dev->dev_info.virtual_address_offset;
    *end = dev->dev_info.virtual_address_max;
    return 0;
}

drm_private void amdgpu_vamgr_init(struct amdgpu_bo_va_mgr *mgr, uint64_t start, uint64_t max, uint64_t alignment)
{
    struct amdgpu_bo_va_hole *n;

    mgr->va_max = max;
    mgr->va_alignment = alignment;

    list_inithead(&mgr->va_holes);
    pthread_mutex_init(&mgr->bo_va_mutex, NULL);

    pthread_mutex_lock(&mgr->bo_va_mutex);
    n = calloc(1, sizeof(struct amdgpu_bo_va_hole));
    n->size = mgr->va_max - start;
    n->offset = start;
    list_add(&n->list, &mgr->va_holes);
    pthread_mutex_unlock(&mgr->bo_va_mutex);
}

drm_private void amdgpu_vamgr_deinit(struct amdgpu_bo_va_mgr *mgr)
{
    struct amdgpu_bo_va_hole *hole, *tmp;
    LIST_FOR_EACH_ENTRY_SAFE(hole, tmp, &mgr->va_holes, list) {
        list_del(&hole->list);
        free(hole);
    }

    pthread_mutex_destroy(&mgr->bo_va_mutex);
}

static int amdgpu_vamgr_subtract_hole(struct amdgpu_bo_va_hole *hole, uint64_t start_va, uint64_t end_va)
{
    if ((start_va > hole->offset) && ((end_va - hole->offset) < hole->size)) {
        struct amdgpu_bo_va_hole *n = calloc(1, sizeof(struct amdgpu_bo_va_hole));
        if (!n) {
            return -ENOMEM;
        }

        n->size = start_va - hole->offset;
        n->offset = hole->offset;
        list_add(&n->list, &hole->list);

        hole->size -= (end_va - hole->offset);
        hole->offset = end_va;
    } else if (start_va > hole->offset) {
        hole->size = start_va - hole->offset;
    } else if (end_va - hole->offset < hole->size) {
        hole->size -= (end_va - hole->offset);
        hole->offset = end_va;
    } else {
        list_del(&hole->list);
        free(hole);
    }

    return 0;
}

static int amdgpu_vamgr_find_va(struct amdgpu_bo_va_mgr *mgr, uint64_t size, uint64_t alignment, uint64_t base_required, bool search_from_top, uint64_t *va_out)
{
    int ret;
    uint64_t offset = 0;
    struct amdgpu_bo_va_hole *hole, *n;

    alignment = MAX2(alignment, mgr->va_alignment);
    size = ALIGN(size, mgr->va_alignment);

    if (base_required % alignment) {
        return -EINVAL;
    }

    pthread_mutex_lock(&mgr->bo_va_mutex);
    if (!search_from_top) {
        LIST_FOR_EACH_ENTRY_SAFE_REV(hole, n, &mgr->va_holes, list) {
            if (base_required) {
                if ((hole->offset > base_required) || ((hole->offset + hole->size) < (base_required + size))) {
                    continue;
                }

                offset = base_required;
            } else {
                uint64_t waste = hole->offset % alignment;
                waste = waste ? alignment - waste : 0;
                offset = hole->offset + waste;
                if ((offset >= (hole->offset + hole->size)) || (size > ((hole->offset + hole->size) - offset))) {
                    continue;
                }
            }

            ret = amdgpu_vamgr_subtract_hole(hole, offset, offset + size);
            pthread_mutex_unlock(&mgr->bo_va_mutex);
            *va_out = offset;
            return ret;
        }
    } else {
        LIST_FOR_EACH_ENTRY_SAFE(hole, n, &mgr->va_holes, list) {
            if (base_required) {
                if ((hole->offset > base_required) || ((hole->offset + hole->size) < (base_required + size))) {
                    continue;
                }

                offset = base_required;
            } else {
                if (size > hole->size) {
                    continue;
                }

                offset = hole->offset + hole->size - size;
                offset -= offset % alignment;
                if (offset < hole->offset) {
                    continue;
                }
            }

            ret = amdgpu_vamgr_subtract_hole(hole, offset, offset + size);
            pthread_mutex_unlock(&mgr->bo_va_mutex);
            *va_out = offset;
            return ret;
        }
    }

    pthread_mutex_unlock(&mgr->bo_va_mutex);
    return -ENOMEM;
}

static void amdgpu_vamgr_free_va(struct amdgpu_bo_va_mgr *mgr, uint64_t va, uint64_t size)
{
    struct amdgpu_bo_va_hole *hole, *next;

    if (va == AMDGPU_INVALID_VA_ADDRESS) {
        return;
    }

    size = ALIGN(size, mgr->va_alignment);

    pthread_mutex_lock(&mgr->bo_va_mutex);
    hole = container_of(&mgr->va_holes, hole, list);
    LIST_FOR_EACH_ENTRY(next, &mgr->va_holes, list) {
        if (next->offset < va) {
            break;
        }

        hole = next;
    }

    if (&hole->list != &mgr->va_holes) {
        if (hole->offset == (va + size)) {
            hole->offset = va;
            hole->size += size;

            if ((next != hole) && (&next->list != &mgr->va_holes) && ((next->offset + next->size) == va)) {
                next->size += hole->size;
                list_del(&hole->list);
                free(hole);
            }

            goto out;
        }
    }

    if ((next != hole) && (&next->list != &mgr->va_holes) && ((next->offset + next->size) == va)) {
        next->size += size;
        goto out;
    }

    next = calloc(1, sizeof(struct amdgpu_bo_va_hole));
    if (next) {
        next->size = size;
        next->offset = va;
        list_add(&next->list, &hole->list);
    }

out:
    pthread_mutex_unlock(&mgr->bo_va_mutex);
}

drm_public int amdgpu_va_range_alloc(amdgpu_device_handle dev, enum amdgpu_gpu_va_range va_range_type, uint64_t size, uint64_t va_base_alignment, uint64_t va_base_required, uint64_t *va_base_allocated, amdgpu_va_handle *va_range_handle, uint64_t flags)
{
    int ret;
    struct amdgpu_bo_va_mgr *vamgr;
    bool search_from_top = !!(flags & AMDGPU_VA_RANGE_REPLAYABLE);

    if (flags & AMDGPU_VA_RANGE_HIGH && !dev->vamgr_high_32.va_max) {
        flags &= ~AMDGPU_VA_RANGE_HIGH;
    }

    if (flags & AMDGPU_VA_RANGE_HIGH) {
        if (flags & AMDGPU_VA_RANGE_32_BIT) {
            vamgr = &dev->vamgr_high_32;
        } else {
            vamgr = &dev->vamgr_high;
        }
    } else {
        if (flags & AMDGPU_VA_RANGE_32_BIT) {
            vamgr = &dev->vamgr_32;
        } else {
            vamgr = &dev->vamgr;
        }
    }

    va_base_alignment = MAX2(va_base_alignment, vamgr->va_alignment);
    size = ALIGN(size, vamgr->va_alignment);

    ret = amdgpu_vamgr_find_va(vamgr, size, va_base_alignment, va_base_required, search_from_top, va_base_allocated);
    if (!(flags & AMDGPU_VA_RANGE_32_BIT) && ret) {
        if (flags & AMDGPU_VA_RANGE_HIGH) {
            vamgr = &dev->vamgr_high_32;
        } else {
            vamgr = &dev->vamgr_32;
        }

        ret = amdgpu_vamgr_find_va(vamgr, size, va_base_alignment, va_base_required, search_from_top, va_base_allocated);
    }

    if (!ret) {
        struct amdgpu_va *va = calloc(1, sizeof(struct amdgpu_va));
        if (!va) {
            amdgpu_vamgr_free_va(vamgr, *va_base_allocated, size);
            return -ENOMEM;
        }

        va->dev = dev;
        va->address = *va_base_allocated;
        va->size = size;
        va->range = va_range_type;
        va->vamgr = vamgr;
        *va_range_handle = va;
    }

    return ret;
}

drm_public int amdgpu_va_range_free(amdgpu_va_handle va_range_handle)
{
    if (!va_range_handle || !va_range_handle->address) {
        return 0;
    }

    amdgpu_vamgr_free_va(va_range_handle->vamgr, va_range_handle->address, va_range_handle->size);
    free(va_range_handle);

    return 0;
}
