#ifndef _GUIX_SIMPLE_MAIN_DISPLAY_RESOURCES_H_
#define _GUIX_SIMPLE_MAIN_DISPLAY_RESOURCES_H_

#include <gx_api.h>

// 显示和主题定义
#define MAIN_DISPLAY                        0
#define MAIN_DISPLAY_COLOR_FORMAT           GX_COLOR_FORMAT_24XRGB
#define MAIN_DISPLAY_X_RESOLUTION           640
#define MAIN_DISPLAY_Y_RESOLUTION           480
#define MAIN_DISPLAY_THEME_1                0
#define MAIN_DISPLAY_THEME_TABLE_SIZE       1

// 语言的定义
#define LANGUAGE_ENGLISH                    0
#define MAIN_DISPLAY_LANGUAGE_TABLE_SIZE    1

// 颜色标识定义
#define GX_COLOR_ID_BLUE                    29
#define MAIN_DISPLAY_COLOR_TABLE_SIZE       30

// 字体ID定义
#define MAIN_DISPLAY_FONT_TABLE_SIZE        4

// Pixelmap ID定义
#define GX_PIXELMAP_ID_BUTTON               5
#define GX_PIXELMAP_ID_BUTTON_ACTIVE        6
#define GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL  7
#define GX_PIXELMAP_ID_PROMPT_BORDER        8
#define MAIN_DISPLAY_PIXELMAP_TABLE_SIZE    9

// 字符串id
#define GX_STRING_ID_STRING_2               1
#define GX_STRING_ID_STRING_1               2
#define GX_STRING_ID_STRING_3               3
#define GX_STRING_ID_STRING_4               4
#define GX_STRING_ID_STRING_5               5
#define MAIN_DISPLAY_STRING_TABLE_SIZE      6

#endif
