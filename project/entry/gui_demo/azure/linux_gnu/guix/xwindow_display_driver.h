#ifndef _GUIX_XWINDOW_DISPLAY_DRIVER_H_
#define _GUIX_XWINDOW_DISPLAY_DRIVER_H_

#include <gx_api.h>

#define CONFIG_X11_SERVER

#ifdef __cplusplus
extern "C" {
#endif

UINT gx_x11_graphics_driver_setup_24xrgb(GX_DISPLAY *display);

#ifdef __cplusplus
}
#endif

#endif
