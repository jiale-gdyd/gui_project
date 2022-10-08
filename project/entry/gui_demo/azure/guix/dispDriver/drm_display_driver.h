#ifndef _GUIX_DRM_DISPLAY_DRIVER_H_
#define _GUIX_DRM_DISPLAY_DRIVER_H_

#include <gx_api.h>

#ifdef __cplusplus
extern "C" {
#endif

UINT gx_drm_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
UINT gx_drm_graphics_driver_setup_565rgb(GX_DISPLAY *display);
UINT gx_drm_graphics_driver_setup_332rgb(GX_DISPLAY *display);

#ifdef __cplusplus
}
#endif

#endif
