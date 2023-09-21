#ifndef DRM_H
#define DRM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_DRM

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../../lvgl.h"
#include "../../src/display/lv_display_private.h"
#include "../../src/display/lv_display.h"
#endif

void drm_init(void);
void drm_exit(void);

void drm_wait_vsync(lv_display_t *drv);
void drm_flush(lv_display_t *drv, const lv_area_t *area, uint8_t *color_p);
void drm_get_sizes(lv_coord_t *width, lv_coord_t *height, uint32_t *dpi);

#endif

#ifdef __cplusplus
}
#endif

#endif
