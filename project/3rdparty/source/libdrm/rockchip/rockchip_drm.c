#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/stddef.h>

#include <libdrm/xf86drm.h>
#include <libdrm/drm/rockchip_drm.h>
#include <libdrm/drm/rockchip_drmif.h>

struct rockchip_device *rockchip_device_create(int fd)
{
    struct rockchip_device *dev;

    dev = calloc(1, sizeof(*dev));
    if (!dev) {
        fprintf(stderr, "failed to create device[%s].\n", strerror(errno));
        return NULL;
    }
    dev->fd = fd;

    return dev;
}

void rockchip_device_destroy(struct rockchip_device *dev)
{
    free(dev);
}

struct rockchip_bo *rockchip_bo_create(struct rockchip_device *dev, size_t size, uint32_t flags)
{
    struct rockchip_bo *bo;
    struct drm_rockchip_gem_create req = {
        .size = size,
        .flags = flags,
    };

    if (size == 0) {
        fprintf(stderr, "invalid size.\n");
        return NULL;
    }

    bo = calloc(1, sizeof(*bo));
    if (!bo) {
        fprintf(stderr, "failed to create bo[%s].\n", strerror(errno));
        goto fail;
    }

    bo->dev = dev;

    if (drmIoctl(dev->fd, DRM_IOCTL_ROCKCHIP_GEM_CREATE, &req)){
        fprintf(stderr, "failed to create gem object[%s].\n", strerror(errno));
        goto err_free_bo;
    }

    bo->handle = req.handle;
    bo->size = size;
    bo->flags = flags;

    return bo;

err_free_bo:
    free(bo);

fail:
    return NULL;
}

struct rockchip_bo *rockchip_bo_from_handle(struct rockchip_device *dev, uint32_t handle, uint32_t flags, uint32_t size)
{
    struct rockchip_bo *bo;

    if (size == 0) {
        fprintf(stderr, "invalid size.\n");
        return NULL;
    }

    bo = calloc(1, sizeof(*bo));
    if (!bo) {
        fprintf(stderr, "failed to create bo[%s].\n", strerror(errno));
        return NULL;
    }

    bo->dev = dev;
    bo->handle = handle;
    bo->size = size;
    bo->flags = flags;

    return bo;
}

void rockchip_bo_destroy(struct rockchip_bo *bo)
{
    if (!bo) {
        return;
    }

    if (bo->vaddr) {
        munmap(bo->vaddr, bo->size);
    }

    if (bo->handle) {
        struct drm_gem_close req = {
            .handle = bo->handle,
        };

        drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_CLOSE, &req);
    }

    free(bo);
}

struct rockchip_bo *rockchip_bo_from_name(struct rockchip_device *dev, uint32_t name)
{
    struct rockchip_bo *bo;
    struct drm_gem_open req = {
        .name = name,
    };

    bo = calloc(1, sizeof(*bo));
    if (!bo) {
        fprintf(stderr, "failed to allocate bo[%s].\n", strerror(errno));
        return NULL;
    }

    if (drmIoctl(dev->fd, DRM_IOCTL_GEM_OPEN, &req)) {
        fprintf(stderr, "failed to open gem object[%s].\n", strerror(errno));
        goto err_free_bo;
    }

    bo->dev = dev;
    bo->name = name;
    bo->handle = req.handle;

    return bo;

err_free_bo:
    free(bo);
    return NULL;
}

int rockchip_bo_get_name(struct rockchip_bo *bo, uint32_t *name)
{
    if (!bo->name) {
        int ret;
        struct drm_gem_flink req = {
            .handle = bo->handle,
        };

        ret = drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_FLINK, &req);
        if (ret) {
            fprintf(stderr, "failed to get gem global name[%s].\n", strerror(errno));
            return ret;
        }

        bo->name = req.name;
    }

    *name = bo->name;

    return 0;
}

uint32_t rockchip_bo_handle(struct rockchip_bo *bo)
{
    return bo->handle;
}

void *rockchip_bo_map(struct rockchip_bo *bo)
{
    if (!bo->vaddr) {
        int ret;
        struct rockchip_device *dev = bo->dev;
        struct drm_rockchip_gem_map_off req = {
            .handle = bo->handle,
        };

        ret = drmIoctl(dev->fd, DRM_IOCTL_ROCKCHIP_GEM_MAP_OFFSET, &req);
        if (ret) {
            fprintf(stderr, "failed to ioctl gem map offset[%s].\n", strerror(errno));
            return NULL;
        }

        bo->vaddr = mmap64(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED, dev->fd, req.offset);
        if (bo->vaddr == MAP_FAILED) {
            fprintf(stderr, "failed to mmap buffer[%s].\n", strerror(errno));
            return NULL;
        }
    }

    return bo->vaddr;
}
