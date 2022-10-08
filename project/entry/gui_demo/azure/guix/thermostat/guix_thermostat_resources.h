#ifndef _GUIX_THERMOSTAT_MAIN_DISPLAY_RESOURCES_H_
#define _GUIX_THERMOSTAT_MAIN_DISPLAY_RESOURCES_H_

#include <gx_api.h>

/* Display and theme definitions                                               */

#define MAIN_DISPLAY 0
#define MAIN_DISPLAY_COLOR_FORMAT GX_COLOR_FORMAT_565RGB
#define MAIN_DISPLAY_X_RESOLUTION 640
#define MAIN_DISPLAY_Y_RESOLUTION 480
#define MAIN_DISPLAY_DEFAULT_THEME 0
#define MAIN_DISPLAY_THEME_TABLE_SIZE 1

/* Language definitions                                                        */

#define LANGUAGE_ENGLISH 0
#define MAIN_DISPLAY_LANGUAGE_TABLE_SIZE 1

/* Color ID definitions                                                        */

#define GX_COLOR_ID_DARK_BLUE 29
#define GX_COLOR_ID_GREEN 30
#define MAIN_DISPLAY_COLOR_TABLE_SIZE 31

/* Font ID definitions                                                         */

#define GX_FONT_ID_LARGE_BOLD 4
#define GX_FONT_ID_MIDDLE 5
#define GX_FONT_ID_BOLD 6
#define MAIN_DISPLAY_FONT_TABLE_SIZE 7

/* Pixelmap ID definitions                                                     */

#define GX_PIXELMAP_ID_BACKGROUND 5
#define GX_PIXELMAP_ID_BUTTON 6
#define GX_PIXELMAP_ID_BUTTON_ACTIVE 7
#define GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL 8
#define GX_PIXELMAP_ID_NEEDLE 9
#define MAIN_DISPLAY_PIXELMAP_TABLE_SIZE 10

/* String Ids                                                                  */

#define GX_STRING_ID_STRING_2 1
#define GX_STRING_ID_STRING_5 2
#define GX_STRING_ID_STRING_7 3
#define MAIN_DISPLAY_STRING_TABLE_SIZE 4

#endif                                       /* sentry                         */
