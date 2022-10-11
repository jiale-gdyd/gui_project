#ifndef LIBDRM_UTIL_BUFFER_H
#define LIBDRM_UTIL_BUFFER_H

#include "pattern.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct bo;

void bo_destroy(struct bo *bo);
struct bo *bo_create(int fd, unsigned int format, unsigned int width, unsigned int height, unsigned int handles[4], unsigned int pitches[4], unsigned int offsets[4], enum util_fill_pattern pattern);

#if defined(__cplusplus)
}
#endif

#endif
