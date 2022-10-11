#ifndef LIBDRM_UTIL_CURSOR_H
#define LIBDRM_UTIL_CURSOR_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

int cursor_stop(void);
int cursor_start(void);
int cursor_init(int fd, uint32_t bo_handle, uint32_t crtc_id, uint32_t crtc_w, uint32_t crtc_h, uint32_t w, uint32_t h);

#if defined(__cplusplus)
}
#endif

#endif
