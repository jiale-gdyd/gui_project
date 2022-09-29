#ifndef LVGL_FRIVERS_H
#define LVGL_FRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "display/drm.h"
#include "display/fbdev.h"
#include "display/ST7565.h"
#include "display/UC1610.h"
#include "display/GC9A01.h"
#include "display/R61581.h"
#include "display/ILI9341.h"
#include "display/monitor.h"
#include "display/SSD1963.h"
#include "display/SHARP_MIP.h"

#include "gtkdrv/gtkdrv.h"

#include "indev/mouse.h"
#include "indev/evdev.h"
#include "indev/AD_touch.h"
#include "indev/keyboard.h"
#include "indev/XPT2046.h"
#include "indev/FT5406EE8.h"
#include "indev/mousewheel.h"
#include "indev/libinput_drv.h"

#include "wayland/wayland.h"

#ifdef __cplusplus
}
#endif

#endif
