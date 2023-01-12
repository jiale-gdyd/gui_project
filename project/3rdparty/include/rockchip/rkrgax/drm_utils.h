#ifndef RKRGA_DRM_UTILS_H
#define RKRGA_DRM_UTILS_H

#include <stdint.h>

#define is_drm_fourcc(format)   (((format >> 24) & 0xff) && ((format >> 16) & 0xff) && ((format >> 8) & 0xff) && ((format) & 0xff))

uint32_t get_format_from_drm_fourcc(uint32_t drm_fourcc);

#endif
