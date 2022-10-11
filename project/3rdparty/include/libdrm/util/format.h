#ifndef LIBDRM_UTIL_FORMAT_H
#define LIBDRM_UTIL_FORMAT_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct util_color_component {
    unsigned int length;
    unsigned int offset;
};

struct util_rgb_info {
    struct util_color_component red;
    struct util_color_component green;
    struct util_color_component blue;
    struct util_color_component alpha;
};

enum util_yuv_order {
    YUV_YCbCr = 1,
    YUV_YCrCb = 2,
    YUV_YC    = 4,
    YUV_CY    = 8,
};

struct util_yuv_info {
    enum util_yuv_order order;
    unsigned int        xsub;
    unsigned int        ysub;
    unsigned int        chroma_stride;
};

struct util_format_info {
    uint32_t                   format;
    const char                 *name;
    const struct util_rgb_info rgb;
    const struct util_yuv_info yuv;
};

uint32_t util_format_fourcc(const char *name);
const struct util_format_info *util_format_info_find(uint32_t format);

#if defined(__cplusplus)
}
#endif

#endif
