#ifndef LVGL_DEMO_PRINTER_THEME_H
#define LVGL_DEMO_PRINTER_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "printer.h"

#if LV_USE_DEMO_PRINTER

#define LV_DEMO_PRINTER_WHITE           lv_color_hex(0xFFFFFF)
#define LV_DEMO_PRINTER_LIGHT           lv_color_hex(0xF3F8FE)
#define LV_DEMO_PRINTER_GRAY            lv_color_hex(0x8A8A8A)
#define LV_DEMO_PRINTER_LIGHT_GRAY      lv_color_hex(0xC4C4C4)
#define LV_DEMO_PRINTER_BLUE            lv_color_hex(0x2F3243)
#define LV_DEMO_PRINTER_GREEN           lv_color_hex(0x4CB242)
#define LV_DEMO_PRINTER_RED             lv_color_hex(0xD51732)

typedef enum {
    LV_DEMO_PRINTER_THEME_TITLE = _LV_THEME_BUILTIN_LAST,
    LV_DEMO_PRINTER_THEME_LABEL_WHITE,
    LV_DEMO_PRINTER_THEME_ICON,
    LV_DEMO_PRINTER_THEME_BTN_BORDER,
    LV_DEMO_PRINTER_THEME_BTN_CIRCLE,
    LV_DEMO_PRINTER_THEME_BOX_BORDER,
    LV_DEMO_PRINTER_THEME_BTN_BACK
} lv_demo_printer_theme_t;

lv_theme_t *lv_demo_printer_theme_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags, const lv_font_t *font_small, const lv_font_t *font_normal, const lv_font_t *font_subtitle, const lv_font_t *font_title);

#endif

#ifdef __cplusplus
}
#endif

#endif
