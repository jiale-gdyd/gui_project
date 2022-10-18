#ifndef LIBDRM_MEDIA_RGA_H
#define LIBDRM_MEDIA_RGA_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DRM_RGA_CHANNELS {
    DRM_RGA_CHANNEL_00 = 0,
    DRM_RGA_CHANNEL_02,
    DRM_RGA_CHANNEL_03,
    DRM_RGA_CHANNEL_04,
    DRM_RGA_CHANNEL_05,
    DRM_RGA_CHANNEL_06,
    DRM_RGA_CHANNEL_07,
    DRM_RGA_CHANNEL_08,
    DRM_RGA_CHANNEL_09,
    DRM_RGA_CHANNEL_10,
    DRM_RGA_CHANNEL_11,
    DRM_RGA_CHANNEL_12,
    DRM_RGA_CHANNEL_13,
    DRM_RGA_CHANNEL_14,
    DRM_RGA_CHANNEL_15,
    DRM_RGA_CHANNEL_BUTT
};

typedef struct RGA_INFO_S {
    drm_image_type_e imgType;
    uint32_t         u32X;
    uint32_t         u32Y;
    uint32_t         u32Width;
    uint32_t         u32Height;
    uint32_t         u32HorStride;
    uint32_t         u32VirStride;
} drm_rga_info_t;

typedef enum RGA_FLIP_E {
    RGA_FLIP_NULL,
    RGA_FLIP_H,
    RGA_FLIP_V,
    RGA_FLIP_HV,
} drm_rga_flip_e;

typedef struct RGA_ATTR_S {
    drm_rga_info_t stImgIn;
    drm_rga_info_t stImgOut;
    uint16_t       u16Rotaion;
    bool           bEnBufPool;
    uint16_t       u16BufPoolCnt;
    drm_rga_flip_e enFlip;
} drm_rga_attr_t;

#ifdef __cplusplus
}
#endif

#endif