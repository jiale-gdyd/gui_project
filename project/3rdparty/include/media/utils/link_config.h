#ifndef LIBDRM_DISPLAY_LINK_CONFIG_H
#define LIBDRM_DISPLAY_LINK_CONFIG_H

#include <time.h>

namespace libdrm {
enum LinkType {
    LINK_NONE,
    LINK_VIDEO,
    LINK_AUDIO,
    LINK_PICTURE
};

typedef struct linkvideo {
    void         *buffer_ptr;
    unsigned int buffer_size;
    time_t       timestamp;
    int          nat_type;
} linkvideo_s;

typedef struct linkaudio {
    void         *buffer_ptr;
    unsigned int buffer_size;
    time_t       timestamp;
} linkaudio_s;

typedef struct linkpicture {
    void         *buffer_ptr;
    unsigned int buffer_size;
    int          type;
    const char   *id;
} linkpicture_s;

typedef struct linkcommon {
    int             linktype;
    union {
        linkvideo_s video;
        linkaudio_s audio;
        linkpicture picture;
    };
} linkcommon_s;
}

#endif
