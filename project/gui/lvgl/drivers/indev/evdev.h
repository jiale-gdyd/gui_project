#ifndef EVDEV_H
#define EVDEV_H

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

#if USE_EVDEV || USE_BSD_EVDEV

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../../lvgl.h"
#include "../../src/indev/lv_indev_private.h"
#include "../../src/indev/lv_indev.h"
#endif

typedef struct {
    /*Device*/
    int fd;
    /*Config*/
    bool swap_axes;
    int ver_min;
    int hor_min;
    int ver_max;
    int hor_max;
    /*State*/
    int root_x;
    int root_y;
    int key;
    lv_indev_state_t state;
} evdev_device_t;

void evdev_init(void);

void evdev_device_init(evdev_device_t *dsc);
bool evdev_set_file(const char * dev_path);
bool evdev_device_set_file(evdev_device_t * dsc, const char * dev_path);
void evdev_device_set_swap_axes(evdev_device_t * dsc, bool swap_axes);
void evdev_device_set_calibration(evdev_device_t * dsc, int hor_min, int ver_min, int hor_max, int ver_max);
void evdev_read(lv_indev_drv_t * drv, lv_indev_data_t * data);

#endif

#ifdef __cplusplus
}
#endif

#endif
