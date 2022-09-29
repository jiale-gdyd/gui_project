#ifndef GUI_LVGL_LVGL_H
#define GUI_LVGL_LVGL_H

#include <linux/kconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_LVGL_V7)
#include "lvgl7/lvgl.h"
#endif

#if defined(CONFIG_LVGL_V8)
#include "lvgl8/lvgl.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
