#ifndef _GUIX_XWINDOW_DISPLAY_DRIVER_H_
#define _GUIX_XWINDOW_DISPLAY_DRIVER_H_

#include <gx_api.h>

#ifdef __cplusplus
extern "C" {
#endif

UINT gx_x11_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
UINT gx_x11_graphics_driver_setup_565rgb(GX_DISPLAY *display);
UINT gx_x11_graphics_driver_setup_332rgb(GX_DISPLAY *display);
UINT gx_x11_graphics_driver_setup_4444argb(GX_DISPLAY *display);
UINT gx_x11_graphics_driver_setup_monochrome(GX_DISPLAY *display);
UINT gx_x11_graphics_driver_setup_8bit_palette(GX_DISPLAY *display);
UINT gx_x11_graphics_driver_setup_4bpp_grayscale(GX_DISPLAY *display);

#ifdef __cplusplus
}
#endif

#endif
