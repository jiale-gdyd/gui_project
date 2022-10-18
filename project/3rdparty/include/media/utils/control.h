#ifndef LIBDRM_DISPLAY_CONTROL_H
#define LIBDRM_DISPLAY_CONTROL_H

#include <stdint.h>

#include "image.h"

namespace libdrm {
typedef struct {
    const char *name;
    uint64_t   value;
} DRMPropertyArg;

typedef struct {
    unsigned long int sub_request;
    int               size;
    void              *arg;
} SubRequest;

typedef struct {
    size_t size;
    void   *data;
} UserPicArg;

enum {
    S_FIRST_CONTROL = 10000,
    S_SUB_REQUEST,
    S_SOURCE_RECT,
    G_SOURCE_RECT,
    S_DESTINATION_RECT,
    G_DESTINATION_RECT,
    S_SRC_DST_RECT,
    G_SRC_DST_RECT,
    S_SET_MUTE,

    G_PLANE_IMAGE_INFO = 10100,
    G_PLANE_SUPPORT_SCALE,
    S_CRTC_PROPERTY,
    S_CONNECTOR_PROPERTY,

    S_STREAM_OFF = 10200,
    S_INSERT_USER_PICTURE,
    S_ENABLE_USER_PICTURE,
    S_DISABLE_USER_PICTURE,
    G_STREAM_RECENT_TIME,

    S_ALSA_VOLUME = 10300,
    G_ALSA_VOLUME,
    S_VQE_ENABLE,
    S_VQE_ATTR,
    G_VQE_ATTR,

    S_ALLOW_THROUGH_COUNT = 10400,

    S_ANR_ON = 10500,
    G_ANR_ON,

    S_NN_CALLBACK = 10600,
    G_NN_CALLBACK,
    S_NN_HANDLER,
    G_NN_HANDLER,
    S_NN_DRAW_HANDLER,
    G_NN_DRAW_HANDLER,
    S_NN_INFO,
    G_NN_INFO,

    S_MD_ROI_ENABLE = 10700,
    G_MD_ROI_ENABLE,
    S_MD_ROI_RECTS,
    G_MD_ROI_RECTS,
    S_MD_SENSITIVITY,
    G_MD_SENSITIVITY,

    S_START_SRTEAM = 10800,
    S_STOP_SRTEAM,
    S_MANUAL_SPLIT_STREAM,
    G_MUXER_GET_STATUS,
    S_MUXER_FILE_DURATION,
    S_MUXER_FILE_PATH,
    S_MUXER_FILE_PREFIX,
    S_MUXER_FILE_NAME_CB,
    S_MUXER_SET_FPS,

    S_OD_ROI_ENABLE = 10900,
    G_OD_ROI_ENABLE,
    S_OD_ROI_RECTS,
    G_OD_ROI_RECTS,
    S_OD_SENSITIVITY,
    G_OD_SENSITIVITY,

    S_RGA_CFG = 11000,
    G_RGA_CFG,
    S_RGA_LINE_INFO,
    G_RGA_LINE_INFO,
    S_RGA_SHOW,
    S_RGA_HIDE,
    S_RGA_OSD_INFO,
    G_RGA_OSD_INFO,
    G_RGA_REGION_LUMA,
};
}

#endif
