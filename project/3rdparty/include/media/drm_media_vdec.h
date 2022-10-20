#ifndef LIBDRM_MEDIA_VDEC_H
#define LIBDRM_MEDIA_VDEC_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DRM_VDEC_CHANNELS {
    DRM_VDEC_CHANNEL_00 = 0,
    DRM_VDEC_CHANNEL_02,
    DRM_VDEC_CHANNEL_03,
    DRM_VDEC_CHANNEL_04,
    DRM_VDEC_CHANNEL_05,
    DRM_VDEC_CHANNEL_06,
    DRM_VDEC_CHANNEL_07,
    DRM_VDEC_CHANNEL_08,
    DRM_VDEC_CHANNEL_09,
    DRM_VDEC_CHANNEL_10,
    DRM_VDEC_CHANNEL_11,
    DRM_VDEC_CHANNEL_12,
    DRM_VDEC_CHANNEL_13,
    DRM_VDEC_CHANNEL_14,
    DRM_VDEC_CHANNEL_15,
    DRM_VDEC_CHANNEL_BUTT
};

typedef enum VIDEO_MODE_E {
    VIDEO_MODE_STREAM = 0,
    VIDEO_MODE_FRAME,
    VIDEO_MODE_COMPAT,
    VIDEO_MODE_BUTT
} drm_video_mode_e;

typedef enum VIDEO_DECODEC_MODE_E {
    VIDEO_DECODEC_SOFTWARE = 0,
    VIDEO_DECODEC_HADRWARE,
} drm_video_decodec_mode_e;

typedef struct VDEC_ATTR_VIDEO_S {

} drm_video_vdec_attr_t;

typedef struct VDEC_CHN_ATTR_S {
    drm_codec_type_e          enCodecType;
    drm_video_mode_e          enMode;
    drm_video_decodec_mode_e  enDecodecMode;
    union {
        drm_video_vdec_attr_t stVdecVideoAttr;
    };
} drm_vdec_chn_attr_t;

#ifdef __cplusplus
}
#endif

#endif
