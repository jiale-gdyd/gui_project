#ifndef LIBDRM_DISPLAY_CONTROL_HPP
#define LIBDRM_DISPLAY_CONTROL_HPP

#include <stdint.h>
#include "image.hpp"

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

    S_ALLOW_THROUGH_COUNT = 10400,
};
}

#endif
