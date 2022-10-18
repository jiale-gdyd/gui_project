#ifndef RKRGA_DRMRGA_H
#define RKRGA_DRMRGA_H

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/cdefs.h>

#define HAL_TRANSFORM_FLIP_H                    0x01
#define HAL_TRANSFORM_FLIP_V                    0x02
#define HAL_TRANSFORM_ROT_90                    0x04
#define HAL_TRANSFORM_ROT_180                   0x03
#define HAL_TRANSFORM_ROT_270                   0x07
#define HAL_TRANSFORM_FLIP_H_V                  0x08

#define DRM_RGA_MODULE_API_VERSION              HWC_MODULE_API_VERSION_0_1
#define DRM_RGA_DEVICE_API_VERSION              HWC_DEVICE_API_VERSION_0_1
#define DRM_RGA_API_VERSION                     HWC_DEVICE_API_VERSION

#define DRM_RGA_TRANSFORM_ROT_MASK              0x0000000F
#define DRM_RGA_TRANSFORM_ROT_0                 0x00000000
#define DRM_RGA_TRANSFORM_ROT_90                HAL_TRANSFORM_ROT_90
#define DRM_RGA_TRANSFORM_ROT_180               HAL_TRANSFORM_ROT_180
#define DRM_RGA_TRANSFORM_ROT_270               HAL_TRANSFORM_ROT_270

#define DRM_RGA_TRANSFORM_FLIP_MASK             0x00000003
#define DRM_RGA_TRANSFORM_FLIP_H                HAL_TRANSFORM_FLIP_H
#define DRM_RGA_TRANSFORM_FLIP_V                HAL_TRANSFORM_FLIP_V

enum {
    AWIDTH = 0,
    AHEIGHT,
    ASTRIDE,
    AFORMAT,
    ASIZE,
    ATYPE,
};

enum drm_rockchip_gem_mem_type {
    ROCKCHIP_BO_CONTIG   = 1 << 0,
    ROCKCHIP_BO_CACHABLE = 1 << 1,
    ROCKCHIP_BO_WC       = 1 << 2,
    ROCKCHIP_BO_SECURE   = 1 << 3,
    ROCKCHIP_BO_MASK     = ROCKCHIP_BO_CONTIG | ROCKCHIP_BO_CACHABLE | ROCKCHIP_BO_WC | ROCKCHIP_BO_SECURE
};

typedef struct bo {
    int      fd;
    void     *ptr;
    size_t   size;
    size_t   offset;
    size_t   pitch;
    unsigned handle;
} bo_t;

typedef struct rga_rect {
    int xoffset;
    int yoffset;
    int width;
    int height;
    int wstride;
    int hstride;
    int format;
    int size;
} rga_rect_t;

typedef struct rga_nn {
    int nn_flag;
    int scale_r;
    int scale_g;
    int scale_b;
    int offset_r;
    int offset_g;
    int offset_b;
} rga_nn_t;

typedef struct rga_dither {
    int enable;
    int mode;
    int lut0_l;
    int lut0_h;
    int lut1_l;
    int lut1_h;
} rga_dither_t;

typedef struct rga_info {
    int          fd;
    void         *virAddr;
    void         *phyAddr;
    unsigned     hnd;
    int          format;
    rga_rect_t   rect;
    unsigned int blend;
    int          bufferSize;
    int          rotation;
    int          color;
    int          testLog;
    int          mmuFlag;
    int          colorkey_en;
    int          colorkey_mode;
    int          colorkey_max;
    int          colorkey_min;
    int          scale_mode;
    int          color_space_mode;
    int          sync_mode;
    rga_nn_t     nn;
    rga_dither_t dither;
    int          rop_code;
    int          reserve[127];
} rga_info_t;

typedef struct drm_rga {
    rga_rect_t src;
    rga_rect_t dst;
} drm_rga_t;

static inline int rga_set_rect(rga_rect_t *rect, int x, int y, int w, int h, int sw, int sh, int f)
{
    if (!rect) {
        return -EINVAL;
    }

    rect->xoffset = x;
    rect->yoffset = y;
    rect->width = w;
    rect->height = h;
    rect->wstride = sw;
    rect->hstride = sh;
    rect->format = f;

    return 0;
}

static inline void rga_set_rotation(rga_info_t *info, int angle)
{
    if (angle == 90) {
        info->rotation = HAL_TRANSFORM_ROT_90;
    } else if (angle == 180) {
        info->rotation = HAL_TRANSFORM_ROT_180;
    } else if (angle == 270) {
        info->rotation = HAL_TRANSFORM_ROT_270;
    }
}

#endif
