#ifndef LIBDRM_MEDIA_VO_H
#define LIBDRM_MEDIA_VO_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DRM_VO_CHANNELS {
    DRM_VO_CHANNEL_00 = 0,
    DRM_VO_CHANNEL_01,
    DRM_VO_CHANNEL_BUTT
};

typedef enum VO_PLANE_TYPE_E {
    VO_PLANE_PRIMARY = 0,
    VO_PLANE_OVERLAY,
    VO_PLANE_CURSOR,
    VO_PLANE_BUTT
} drm_plane_type_e;

typedef struct VO_CHN_ATTR_S {
    const char       *pcDevNode;
    uint16_t         u16ConIdx;
    uint16_t         u16EncIdx;
    uint16_t         u16CrtcIdx;
    drm_plane_type_e emPlaneType;
    drm_image_type_e enImgType;
    uint32_t         u32Width;
    uint32_t         u32Height;
    uint16_t         u16Fps;
    uint16_t         u16Zpos;
    drm_rect_t       stImgRect;
    drm_rect_t       stDispRect;
} drm_vo_chn_attr_t;

#ifdef __cplusplus
}
#endif

#endif
