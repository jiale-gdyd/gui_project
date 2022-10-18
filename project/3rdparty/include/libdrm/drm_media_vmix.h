#ifndef LIBDRM_MEDIA_VMIX_H
#define LIBDRM_MEDIA_VMIX_H

#include "drm_media_rga.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VMIX_MAX_LINE_NUM           64

enum DRM_VMIX_CHANNELS {
    DRM_VMIX_CHANNEL_0,
    DRM_VMIX_CHANNEL_1,
    DRM_VMIX_CHANNEL_BUTT
};

enum DRM_VMIX_DEVICES {
    DRM_VMIX_DEVICE_0,
    DRM_VMIX_DEVICE_1,
    DRM_VMIX_DEVICE_BUTT
};

typedef struct VMIX_CHN_INFO_S {
    drm_image_type_e enImgInType;
    drm_image_type_e enImgOutType;
    drm_rect_t       stInRect;
    drm_rect_t       stOutRect;
    uint16_t         u16Rotaion;
    drm_rga_flip_e   enFlip;
} drm_vmix_chn_info_t;

typedef struct VMIX_DEV_INFO_S {
    uint16_t         u16ChnCnt;
    uint16_t         u16Fps;
    uint32_t         u32ImgWidth;
    uint32_t         u32ImgHeight;
    drm_image_type_e enImgType;
    bool             bEnBufPool;
    uint16_t         u16BufPoolCnt;
    VMIX_CHN_INFO_S  stChnInfo[DRM_VMIX_CHANNEL_BUTT];
} drm_vmix_dev_info_t;

typedef struct rkVMIX_LINE_INFO_S {
    uint32_t   u32LineCnt;
    uint32_t   u32Color;
    drm_rect_t stLines[VMIX_MAX_LINE_NUM];
    uint8_t    u8Enable[VMIX_MAX_LINE_NUM];
} drm_vmix_line_info_t;

#ifdef __cplusplus
}
#endif

#endif