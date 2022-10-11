#ifndef LIBDRM_UTIL_PATTERN_H
#define LIBDRM_UTIL_PATTERN_H

#include "../drm/drm_mode.h"

#if defined(__cplusplus)
extern "C" {
#endif

enum util_fill_pattern {
    UTIL_PATTERN_TILES,
    UTIL_PATTERN_PLAIN,
    UTIL_PATTERN_SMPTE,
    UTIL_PATTERN_GRADIENT,
};

enum util_fill_pattern util_pattern_enum(const char *name);
void util_smpte_c8_gamma(unsigned size, struct drm_color_lut *lut);

void util_fill_pattern(uint32_t format, enum util_fill_pattern pattern, void *planes[3], unsigned int width, unsigned int height, unsigned int stride);

#if defined(__cplusplus)
}
#endif

#endif
