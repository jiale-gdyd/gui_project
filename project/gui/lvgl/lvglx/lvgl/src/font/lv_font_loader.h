#ifndef LV_FONT_LOADER_H
#define LV_FONT_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_font.h"

void lv_font_free(lv_font_t *font);
lv_font_t *lv_font_load(const char *fontName);

#ifdef __cplusplus
}
#endif

#endif
