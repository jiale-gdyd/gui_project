#ifndef LIBDRM_MEDIA_VP_H
#define LIBDRM_MEDIA_VP_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DRM_VP_CHANNELS {
    DRM_VP_CHANNEL_00 = 0,
    DRM_VP_CHANNEL_01,
    DRM_VP_CHANNEL_02,
    DRM_VP_CHANNEL_03,
    DRM_VP_CHANNEL_04,
    DRM_VP_CHANNEL_05,
    DRM_VP_CHANNEL_06,
    DRM_VP_CHANNEL_07,
    DRM_VP_CHANNEL_BUTT
};

typedef enum VP_CHN_WORK_MODE {
    VP_WORK_MODE_NORMAL = 0,
    VP_WORK_MODE_BUTT
} drm_vp_chn_work_mode_e;

typedef enum VP_CHN_BUF_TYPE {
    VP_CHN_BUF_TYPE_DMA = 0,
    VP_CHN_BUF_TYPE_MMAP,
} drm_vp_chn_buf_type_e;

typedef struct VP_CHN_ATTR_S {
    const char             *pcVideoNode;
    uint32_t               u32Width;
    uint32_t               u32Height;
    drm_image_type_e       enPixFmt;
    uint32_t               u32BufCnt;
    drm_vp_chn_buf_type_e  enBufType;
    drm_vp_chn_work_mode_e enWorkMode;
} drm_vp_chn_attr_t;

#ifdef __cplusplus
}
#endif

#endif