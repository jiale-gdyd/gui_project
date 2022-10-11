#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>

#include <libdrm/drm/drm.h>
#include <libdrm/xf86drm.h>
#include <libdrm/util/buffer.h>
#include <libdrm/libdrm_macros.h>
#include <libdrm/drm/drm_fourcc.h>

struct bo {
    int      fd;
    void     *ptr;
    size_t   size;
    size_t   offset;
    size_t   pitch;
    unsigned handle;
};

static struct bo *bo_create_dumb(int fd, unsigned int width, unsigned int height, unsigned int bpp)
{
    int ret;
    struct bo *bo;
    struct drm_mode_create_dumb arg;

    bo = calloc(1, sizeof(*bo));
    if (bo == NULL) {
        fprintf(stderr, "failed to allocate buffer object\n");
        return NULL;
    }

    memset(&arg, 0, sizeof(arg));
    arg.bpp = bpp;
    arg.width = width;
    arg.height = height;

    ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &arg);
    if (ret) {
        fprintf(stderr, "failed to create dumb buffer: %s\n", strerror(errno));
        free(bo);
        return NULL;
    }

    bo->fd = fd;
    bo->handle = arg.handle;
    bo->size = arg.size;
    bo->pitch = arg.pitch;

    return bo;
}

static int bo_map(struct bo *bo, void **out)
{
    int ret;
    void *map;
    struct drm_mode_map_dumb arg;

    memset(&arg, 0, sizeof(arg));
    arg.handle = bo->handle;

    ret = drmIoctl(bo->fd, DRM_IOCTL_MODE_MAP_DUMB, &arg);
    if (ret) {
        return ret;
    }

    map = drm_mmap(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED, bo->fd, arg.offset);
    if (map == MAP_FAILED) {
        return -EINVAL;
    }

    bo->ptr = map;
    *out = map;

    return 0;
}

static void bo_unmap(struct bo *bo)
{
    if (!bo->ptr) {
        return;
    }

    drm_munmap(bo->ptr, bo->size);
    bo->ptr = NULL;
}

struct bo *bo_create(int fd, unsigned int format, unsigned int width, unsigned int height, unsigned int handles[4], unsigned int pitches[4], unsigned int offsets[4], enum util_fill_pattern pattern)
{
    int ret;
    struct bo *bo;
    void *virtual;
    unsigned int bpp;
    void *planes[3] = { 0, };
    unsigned int virtual_height;

    switch (format) {
        case DRM_FORMAT_C8:
        case DRM_FORMAT_NV12:
        case DRM_FORMAT_NV21:
        case DRM_FORMAT_NV16:
        case DRM_FORMAT_NV61:
        case DRM_FORMAT_YUV420:
        case DRM_FORMAT_YVU420:
            bpp = 8;
            break;

        case DRM_FORMAT_ARGB4444:
        case DRM_FORMAT_XRGB4444:
        case DRM_FORMAT_ABGR4444:
        case DRM_FORMAT_XBGR4444:
        case DRM_FORMAT_RGBA4444:
        case DRM_FORMAT_RGBX4444:
        case DRM_FORMAT_BGRA4444:
        case DRM_FORMAT_BGRX4444:
        case DRM_FORMAT_ARGB1555:
        case DRM_FORMAT_XRGB1555:
        case DRM_FORMAT_ABGR1555:
        case DRM_FORMAT_XBGR1555:
        case DRM_FORMAT_RGBA5551:
        case DRM_FORMAT_RGBX5551:
        case DRM_FORMAT_BGRA5551:
        case DRM_FORMAT_BGRX5551:
        case DRM_FORMAT_RGB565:
        case DRM_FORMAT_BGR565:
        case DRM_FORMAT_UYVY:
        case DRM_FORMAT_VYUY:
        case DRM_FORMAT_YUYV:
        case DRM_FORMAT_YVYU:
            bpp = 16;
            break;

        case DRM_FORMAT_BGR888:
        case DRM_FORMAT_RGB888:
            bpp = 24;
            break;

        case DRM_FORMAT_ARGB8888:
        case DRM_FORMAT_XRGB8888:
        case DRM_FORMAT_ABGR8888:
        case DRM_FORMAT_XBGR8888:
        case DRM_FORMAT_RGBA8888:
        case DRM_FORMAT_RGBX8888:
        case DRM_FORMAT_BGRA8888:
        case DRM_FORMAT_BGRX8888:
        case DRM_FORMAT_ARGB2101010:
        case DRM_FORMAT_XRGB2101010:
        case DRM_FORMAT_ABGR2101010:
        case DRM_FORMAT_XBGR2101010:
        case DRM_FORMAT_RGBA1010102:
        case DRM_FORMAT_RGBX1010102:
        case DRM_FORMAT_BGRA1010102:
        case DRM_FORMAT_BGRX1010102:
            bpp = 32;
            break;

        case DRM_FORMAT_XRGB16161616F:
        case DRM_FORMAT_XBGR16161616F:
        case DRM_FORMAT_ARGB16161616F:
        case DRM_FORMAT_ABGR16161616F:
            bpp = 64;
            break;

        default:
            fprintf(stderr, "unsupported format 0x%08x\n",  format);
            return NULL;
    }

    switch (format) {
        case DRM_FORMAT_NV12:
        case DRM_FORMAT_NV21:
        case DRM_FORMAT_YUV420:
        case DRM_FORMAT_YVU420:
            virtual_height = height * 3 / 2;
            break;

        case DRM_FORMAT_NV16:
        case DRM_FORMAT_NV61:
            virtual_height = height * 2;
            break;

        default:
            virtual_height = height;
            break;
    }

    bo = bo_create_dumb(fd, width, virtual_height, bpp);
    if (!bo) {
        return NULL;
    }

    ret = bo_map(bo, &virtual);
    if (ret) {
        fprintf(stderr, "failed to map buffer: %s\n", strerror(-errno));
        bo_destroy(bo);
        return NULL;
    }

    switch (format) {
        case DRM_FORMAT_UYVY:
        case DRM_FORMAT_VYUY:
        case DRM_FORMAT_YUYV:
        case DRM_FORMAT_YVYU:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;

            planes[0] = virtual;
            break;

        case DRM_FORMAT_NV12:
        case DRM_FORMAT_NV21:
        case DRM_FORMAT_NV16:
        case DRM_FORMAT_NV61:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;
            pitches[1] = pitches[0];
            offsets[1] = pitches[0] * height;
            handles[1] = bo->handle;

            planes[0] = virtual;
            planes[1] = virtual + offsets[1];
            break;

        case DRM_FORMAT_YUV420:
        case DRM_FORMAT_YVU420:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;
            pitches[1] = pitches[0] / 2;
            offsets[1] = pitches[0] * height;
            handles[1] = bo->handle;
            pitches[2] = pitches[1];
            offsets[2] = offsets[1] + pitches[1] * height / 2;
            handles[2] = bo->handle;

            planes[0] = virtual;
            planes[1] = virtual + offsets[1];
            planes[2] = virtual + offsets[2];
            break;

        case DRM_FORMAT_C8:
        case DRM_FORMAT_ARGB4444:
        case DRM_FORMAT_XRGB4444:
        case DRM_FORMAT_ABGR4444:
        case DRM_FORMAT_XBGR4444:
        case DRM_FORMAT_RGBA4444:
        case DRM_FORMAT_RGBX4444:
        case DRM_FORMAT_BGRA4444:
        case DRM_FORMAT_BGRX4444:
        case DRM_FORMAT_ARGB1555:
        case DRM_FORMAT_XRGB1555:
        case DRM_FORMAT_ABGR1555:
        case DRM_FORMAT_XBGR1555:
        case DRM_FORMAT_RGBA5551:
        case DRM_FORMAT_RGBX5551:
        case DRM_FORMAT_BGRA5551:
        case DRM_FORMAT_BGRX5551:
        case DRM_FORMAT_RGB565:
        case DRM_FORMAT_BGR565:
        case DRM_FORMAT_BGR888:
        case DRM_FORMAT_RGB888:
        case DRM_FORMAT_ARGB8888:
        case DRM_FORMAT_XRGB8888:
        case DRM_FORMAT_ABGR8888:
        case DRM_FORMAT_XBGR8888:
        case DRM_FORMAT_RGBA8888:
        case DRM_FORMAT_RGBX8888:
        case DRM_FORMAT_BGRA8888:
        case DRM_FORMAT_BGRX8888:
        case DRM_FORMAT_ARGB2101010:
        case DRM_FORMAT_XRGB2101010:
        case DRM_FORMAT_ABGR2101010:
        case DRM_FORMAT_XBGR2101010:
        case DRM_FORMAT_RGBA1010102:
        case DRM_FORMAT_RGBX1010102:
        case DRM_FORMAT_BGRA1010102:
        case DRM_FORMAT_BGRX1010102:
        case DRM_FORMAT_XRGB16161616F:
        case DRM_FORMAT_XBGR16161616F:
        case DRM_FORMAT_ARGB16161616F:
        case DRM_FORMAT_ABGR16161616F:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;

            planes[0] = virtual;
            break;
    }

    util_fill_pattern(format, pattern, planes, width, height, pitches[0]);
    bo_unmap(bo);

    return bo;
}

void bo_destroy(struct bo *bo)
{
    int ret;
    struct drm_mode_destroy_dumb arg;

    memset(&arg, 0, sizeof(arg));
    arg.handle = bo->handle;

    ret = drmIoctl(bo->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);
    if (ret) {
        fprintf(stderr, "failed to destroy dumb buffer: %s\n", strerror(errno));
    }

    free(bo);
}
