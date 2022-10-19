#ifndef LIBDRM_MEDIA_VI_H
#define LIBDRM_MEDIA_VI_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DRM_VI_CHANNELS {
    DRM_VI_CHANNEL_00 = 0,
    DRM_VI_CHANNEL_01,
    DRM_VI_CHANNEL_02,
    DRM_VI_CHANNEL_03,
    DRM_VI_CHANNEL_04,
    DRM_VI_CHANNEL_05,
    DRM_VI_CHANNEL_06,
    DRM_VI_CHANNEL_07,
    DRM_VI_CHANNEL_BUTT
};

typedef enum DRM_VI_CHN_WORK_MODE {
    DRM_VI_WORK_MODE_NORMAL = 0,
    DRM_VI_WORK_MODE_LUMA_ONLY,
    DRM_VI_WORK_MODE_BUTT
} drm_vi_chn_work_mode_e;

typedef enum DRM_VI_CHN_BUF_TYPE {
    DRM_VI_CHN_BUF_TYPE_DMA = 0,
    DRM_VI_CHN_BUF_TYPE_MMAP,
} drm_vi_chn_buf_type_e;

typedef struct DRM_VI_CHN_ATTR {
    const char             *pcVideoNode;
    uint32_t               u32Width;
    uint32_t               u32Height;
    drm_image_type_e       enPixFmt;
    uint32_t               u32BufCnt;
    drm_vi_chn_buf_type_e  enBufType;
    drm_vi_chn_work_mode_e enWorkMode;
} drm_vi_chn_attr_t;

typedef struct DRM_VIDEO_REGION_INFO {
    uint32_t   u32RegionNum;
    drm_rect_t *pstRegion;
} drm_video_region_info_t;

typedef struct DRM_VI_USERPOC_ATTR {
    drm_image_type_e enPixFmt;
    uint32_t         u32Width;
    uint32_t         u32Height;
    void             *pvPicPtr;
} drm_vi_userpic_attr_t;

#ifdef __cplusplus
}
#endif

#endif