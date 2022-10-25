#ifndef TK_LCD_LINUX_DRM_H
#define TK_LCD_LINUX_DRM_H

#include "base/lcd.h"

BEGIN_C_DECLS

lcd_t *lcd_linux_drm_create(const char *card);

END_C_DECLS

#endif
