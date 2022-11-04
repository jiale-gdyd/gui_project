#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/stddef.h>

#include <libdrm/xf86drm.h>
#include <libdrm/libdrm_macros.h>
#include <libdrm/drm/exynos_drm.h>
#include <libdrm/drm/exynos_drmif.h>

#define U642VOID(x)                 ((void *)(unsigned long)(x))

drm_public struct exynos_device * exynos_device_create(int fd)
{
    struct exynos_device *dev;

    dev = calloc(sizeof(*dev), 1);
    if (!dev) {
        fprintf(stderr, "failed to create device[%s].\n", strerror(errno));
        return NULL;
    }
    dev->fd = fd;

    return dev;
}

drm_public void exynos_device_destroy(struct exynos_device *dev)
{
    free(dev);
}

drm_public struct exynos_bo *exynos_bo_create(struct exynos_device *dev, size_t size, uint32_t flags)
{
    struct exynos_bo *bo;
    struct drm_exynos_gem_create req = {
        .size = size,
        .flags = flags,
    };

    if (size == 0) {
        fprintf(stderr, "invalid size.\n");
        goto fail;
    }

    bo = calloc(sizeof(*bo), 1);
    if (!bo) {
        fprintf(stderr, "failed to create bo[%s].\n", strerror(errno));
        goto err_free_bo;
    }

    bo->dev = dev;

    if (drmIoctl(dev->fd, DRM_IOCTL_EXYNOS_GEM_CREATE, &req)){
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

drm_public int exynos_bo_get_info(struct exynos_device *dev, uint32_t handle, size_t *size, uint32_t *flags)
{
    int ret;
    struct drm_exynos_gem_info req = {
        .handle = handle,
    };

    ret = drmIoctl(dev->fd, DRM_IOCTL_EXYNOS_GEM_GET, &req);
    if (ret < 0) {
        fprintf(stderr, "failed to get gem object information[%s].\n", strerror(errno));
        return ret;
    }

    *size = req.size;
    *flags = req.flags;

    return 0;
}

drm_public void exynos_bo_destroy(struct exynos_bo *bo)
{
    if (!bo) {
        return;
    }

    if (bo->vaddr) {
        munmap(bo->vaddr, bo->size);
    }

    if (bo->handle) {
        drmCloseBufferHandle(bo->dev->fd, bo->handle);
    }

    free(bo);
}

drm_public struct exynos_bo *exynos_bo_from_name(struct exynos_device *dev, uint32_t name)
{
    struct exynos_bo *bo;
    struct drm_gem_open req = {
        .name = name,
    };

    bo = calloc(sizeof(*bo), 1);
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

drm_public int exynos_bo_get_name(struct exynos_bo *bo, uint32_t *name)
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

drm_public uint32_t exynos_bo_handle(struct exynos_bo *bo)
{
    return bo->handle;
}

drm_public void *exynos_bo_map(struct exynos_bo *bo)
{
    if (!bo->vaddr) {
        int ret;
        void *map = NULL;
        struct drm_mode_map_dumb arg;
        struct exynos_device *dev = bo->dev;

        memset(&arg, 0, sizeof(arg));
        arg.handle = bo->handle;

        ret = drmIoctl(dev->fd, DRM_IOCTL_MODE_MAP_DUMB, &arg);
        if (ret) {
            fprintf(stderr, "failed to map dumb buffer[%s].\n", strerror(errno));
            return NULL;
        }

        map = drm_mmap(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED, dev->fd, arg.offset);
        if (map != MAP_FAILED) {
            bo->vaddr = map;
        }
    }

    return bo->vaddr;
}

drm_public int exynos_prime_handle_to_fd(struct exynos_device *dev, uint32_t handle, int *fd)
{
    return drmPrimeHandleToFD(dev->fd, handle, 0, fd);
}

drm_public int exynos_prime_fd_to_handle(struct exynos_device *dev, int fd, uint32_t *handle)
{
    return drmPrimeFDToHandle(dev->fd, fd, handle);
}

drm_public int exynos_vidi_connection(struct exynos_device *dev, uint32_t connect, uint32_t ext, void *edid)
{
    int ret;
    struct drm_exynos_vidi_connection req = {
        .connection = connect,
        .extensions = ext,
        .edid       = (uint64_t)(uintptr_t)edid,
    };

    ret = drmIoctl(dev->fd, DRM_IOCTL_EXYNOS_VIDI_CONNECTION, &req);
    if (ret) {
        fprintf(stderr, "failed to request vidi connection[%s].\n", strerror(errno));
        return ret;
    }

    return 0;
}

static void exynos_handle_vendor(int fd, struct drm_event *e, void *ctx)
{
    struct drm_exynos_g2d_event *g2d;
    struct exynos_event_context *ectx = ctx;

    switch (e->type) {
        case DRM_EXYNOS_G2D_EVENT:
            if ((ectx->version < 1) || (ectx->g2d_event_handler == NULL)) {
                break;
            }

            g2d = (struct drm_exynos_g2d_event *)e;
            ectx->g2d_event_handler(fd, g2d->cmdlist_no, g2d->tv_sec, g2d->tv_usec, U642VOID(g2d->user_data));
            break;

        default:
            break;
    }
}

drm_public int exynos_handle_event(struct exynos_device *dev, struct exynos_event_context *ctx)
{
    int len, i;
    char buffer[1024];
    struct drm_event *e;
    struct drm_event_vblank *vblank;
    drmEventContextPtr evctx = &ctx->base;

    len = read(dev->fd, buffer, sizeof buffer);
    if (len == 0) {
        return 0;
    }

    if (len < (int)sizeof *e) {
        return -1;
    }

    i = 0;
    while (i < len) {
        e = (struct drm_event *)(buffer + i);
        switch (e->type) {
            case DRM_EVENT_VBLANK:
                if ((evctx->version < 1) || (evctx->vblank_handler == NULL)) {
                    break;
                }

                vblank = (struct drm_event_vblank *)e;
                evctx->vblank_handler(dev->fd, vblank->sequence, vblank->tv_sec, vblank->tv_usec, U642VOID(vblank->user_data));
                break;

            case DRM_EVENT_FLIP_COMPLETE:
                if ((evctx->version < 2) || (evctx->page_flip_handler == NULL)) {
                    break;
                }

                vblank = (struct drm_event_vblank *) e;
                evctx->page_flip_handler(dev->fd, vblank->sequence, vblank->tv_sec, vblank->tv_usec, U642VOID(vblank->user_data));
                break;

            default:
                exynos_handle_vendor(dev->fd, e, evctx);
                break;
        }

        i += e->length;
    }

    return 0;
}
