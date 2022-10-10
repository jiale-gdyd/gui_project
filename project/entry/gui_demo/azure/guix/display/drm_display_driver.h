#ifndef _GUIX_DRM_DISPLAY_DRIVER_H_
#define _GUIX_DRM_DISPLAY_DRIVER_H_

#include <gx_api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum displayPlaneTypeEnum {
    DISP_PLANE_PRIMARY = 0,
    DISP_PLANE_OVERLAY,
    DISP_PLANE_CURSOR,
    DISP_PLANE_BUTT
} disp_plane_type_e;

typedef enum displayImageTypeEnum {
    DISP_IMAGE_TYPE_UNKNOW = 0,
    DISP_IMAGE_TYPE_GRAY8,
    DISP_IMAGE_TYPE_GRAY16,
    DISP_IMAGE_TYPE_YUV420P,
    DISP_IMAGE_TYPE_NV12,
    DISP_IMAGE_TYPE_NV21,
    DISP_IMAGE_TYPE_YV12,
    DISP_IMAGE_TYPE_FBC2,
    DISP_IMAGE_TYPE_FBC0,
    DISP_IMAGE_TYPE_YUV422P,
    DISP_IMAGE_TYPE_NV16,
    DISP_IMAGE_TYPE_NV61,
    DISP_IMAGE_TYPE_YV16,
    DISP_IMAGE_TYPE_YUYV422,
    DISP_IMAGE_TYPE_UYVY422,
    DISP_IMAGE_TYPE_YUV444SP,
    DISP_IMAGE_TYPE_RGB332,
    DISP_IMAGE_TYPE_RGB565,
    DISP_IMAGE_TYPE_BGR565,
    DISP_IMAGE_TYPE_RGB888,
    DISP_IMAGE_TYPE_BGR888,
    DISP_IMAGE_TYPE_ARGB8888,
    DISP_IMAGE_TYPE_ABGR8888,
    DISP_IMAGE_TYPE_RGBA8888,
    DISP_IMAGE_TYPE_BGRA8888,
    DISP_IMAGE_TYPE_JPEG,
    DISP_IMAGE_TYPE_BUTT
} disp_image_type_e;

int gx_drm_graphics_driver_exit(int channel);
int gx_drm_graphics_driver_setup(int channel, size_t width, size_t height, size_t xoffset, size_t yoffset, disp_image_type_e type, disp_plane_type_e dispLayer, int zpos);

UINT gx_drm_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
UINT gx_drm_graphics_driver_setup_565rgb(GX_DISPLAY *display);
UINT gx_drm_graphics_driver_setup_332rgb(GX_DISPLAY *display);
UINT gx_drm_graphics_driver_setup_1555xrgb(GX_DISPLAY *display);
UINT gx_drm_graphics_driver_setup_4444argb(GX_DISPLAY *display);

#ifdef __cplusplus
}
#endif

#endif
