#ifndef RKRGA_UTILS_H
#define RKRGA_UTILS_H

#include "drm_utils.h"

#define u64_to_ptr(var)             ((void *)(uintptr_t)(var))
#define ptr_to_u64(ptr)             ((uint64_t)(uintptr_t)(ptr))

#define is_rga_format(format)       ((format) & 0xff00 || (format) == 0)

int convert_to_rga_format(int ex_format);

#endif
