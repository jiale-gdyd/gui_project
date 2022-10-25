#ifndef TK_LCD_LINUX_FB_H
#define TK_LCD_LINUX_FB_H

#include "base/lcd.h"

BEGIN_C_DECLS

lcd_t *lcd_linux_fb_create(const char *filename);

END_C_DECLS

#endif
