#ifndef LVGL_CONF_H
#define LVGL_CONF_H

#include <stdint.h>

// Color depth: 8 (A8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)
#define LV_COLOR_DEPTH                      32

#define LV_USE_STDLIB_MALLOC                LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING                LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF               LV_STDLIB_BUILTIN

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
#define LV_MEM_SIZE                         (256 * 1024U)

#define LV_MEM_POOL_EXPAND_SIZE             0

#define LV_MEM_ADR                          0

#if LV_MEM_ADR == 0
#undef LV_MEM_POOL_INCLUDE
#undef LV_MEM_POOL_ALLOC
#endif
#endif

#if LV_USE_STDLIB_SPRINTF == LV_STDLIB_BUILTIN
#define LV_SPRINTF_USE_FLOAT                0
#endif

#define LV_DEF_REFR_PERIOD                  33

#define LV_TICK_CUSTOM                      0
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE              "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR        (millis())
#endif

#define LV_DPI_DEF                          130

#define LV_LAYER_MAX_MEMORY_USAGE           150

#define LV_USE_DRAW_SW                      1

#if LV_USE_DRAW_SW == 1
#define LV_DRAW_SW_DRAW_UNIT_CNT            1

#define LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)

#define LV_DRAW_SW_COMPLEX                  1

#if LV_DRAW_SW_COMPLEX == 1
#define LV_DRAW_SW_SHADOW_CACHE_SIZE        0
#define LV_DRAW_SW_CIRCLE_CACHE_SIZE        4
#endif
#endif

#define LV_USE_OS                           LV_OS_NONE

#if LV_USE_OS == LV_OS_CUSTOM
#define LV_OS_CUSTOM_INCLUDE                <stdint.h>
#endif

#define LV_USE_LOG                          0
#if LV_USE_LOG

#define LV_LOG_LEVEL                        LV_LOG_LEVEL_WARN

#define LV_LOG_PRINTF                       0

#define LV_LOG_USE_TIMESTAMP                1

#define LV_LOG_TRACE_MEM                    1
#define LV_LOG_TRACE_TIMER                  1
#define LV_LOG_TRACE_INDEV                  1
#define LV_LOG_TRACE_DISP_REFR              1
#define LV_LOG_TRACE_EVENT                  1
#define LV_LOG_TRACE_OBJ_CREATE             1
#define LV_LOG_TRACE_LAYOUT                 1
#define LV_LOG_TRACE_ANIM                   1
#define LV_LOG_TRACE_MSG                    1
#endif

#define LV_USE_ASSERT_NULL                  1
#define LV_USE_ASSERT_MALLOC                1
#define LV_USE_ASSERT_STYLE                 0
#define LV_USE_ASSERT_MEM_INTEGRITY         0
#define LV_USE_ASSERT_OBJ                   0

#define LV_ASSERT_HANDLER_INCLUDE           <stdint.h>
#define LV_ASSERT_HANDLER                   while (1);

#define LV_USE_REFR_DEBUG                   0
#define LV_USE_LAYER_DEBUG                  0
#define LV_USE_PARALLEL_DRAW_DEBUG          0

#define LV_USE_PERF_MONITOR                 0

#if LV_USE_PERF_MONITOR
#define LV_USE_PERF_MONITOR_POS             LV_ALIGN_BOTTOM_RIGHT
#define LV_USE_PERF_MONITOR_LOG_MODE        0
#endif

#define LV_USE_MEM_MONITOR                  0

#if LV_USE_MEM_MONITOR
#define LV_USE_MEM_MONITOR_POS              LV_ALIGN_BOTTOM_LEFT
#endif

#define LV_DISP_ROT_MAX_BUF                 (10*1024)

#define LV_ENABLE_GC                        0

#if LV_ENABLE_GC != 0
#define LV_GC_INCLUDE                       "gc.h"
#endif

#define LV_IMG_CACHE_DEF_SIZE               0
#define LV_GRADIENT_MAX_STOPS               2
#define lv_color_mix_ROUND_OFS              0

#define LV_BIG_ENDIAN_SYSTEM                0

#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER

#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE         1

#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

#define LV_ATTRIBUTE_FAST_MEM

#define LV_EXPORT_CONST_INT(int_value)      struct _silence_gcc_warning

#define LV_USE_LARGE_COORD                  0

#define LV_FONT_MONTSERRAT_8                1
#define LV_FONT_MONTSERRAT_10               1
#define LV_FONT_MONTSERRAT_12               1
#define LV_FONT_MONTSERRAT_14               1
#define LV_FONT_MONTSERRAT_16               1
#define LV_FONT_MONTSERRAT_18               1
#define LV_FONT_MONTSERRAT_20               1
#define LV_FONT_MONTSERRAT_22               1
#define LV_FONT_MONTSERRAT_24               1
#define LV_FONT_MONTSERRAT_26               1
#define LV_FONT_MONTSERRAT_28               1
#define LV_FONT_MONTSERRAT_30               1
#define LV_FONT_MONTSERRAT_32               1
#define LV_FONT_MONTSERRAT_34               1
#define LV_FONT_MONTSERRAT_36               1
#define LV_FONT_MONTSERRAT_38               1
#define LV_FONT_MONTSERRAT_40               1
#define LV_FONT_MONTSERRAT_42               1
#define LV_FONT_MONTSERRAT_44               1
#define LV_FONT_MONTSERRAT_46               1
#define LV_FONT_MONTSERRAT_48               1

#define LV_FONT_MONTSERRAT_28_COMPRESSED    1
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    1
#define LV_FONT_SIMSUN_16_CJK               1

#define LV_FONT_UNSCII_8                    1
#define LV_FONT_UNSCII_16                   1

#define LV_FONT_CUSTOM_DECLARE

#define LV_FONT_DEFAULT                     &lv_font_montserrat_14

#define LV_FONT_FMT_TXT_LARGE               0

#define LV_USE_FONT_COMPRESSED              0
#define LV_USE_FONT_PLACEHOLDER             1

#define LV_TXT_ENC                          LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS                  " ,.;:-_)]}"

#define LV_TXT_LINE_BREAK_LONG_LEN          0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

#define LV_USE_BIDI                         0

#if LV_USE_BIDI
#define LV_BIDI_BASE_DIR_DEF                LV_BASE_DIR_AUTO
#endif

#define LV_USE_ARABIC_PERSIAN_CHARS         0

#define LV_USE_ANIMIMG                      1
#define LV_USE_ARC                          1
#define LV_USE_BAR                          1
#define LV_USE_BTN                          1
#define LV_USE_BTNMATRIX                    1
#define LV_USE_CALENDAR                     1

#if LV_USE_CALENDAR
#define LV_CALENDAR_WEEK_STARTS_MONDAY      0
#if LV_CALENDAR_WEEK_STARTS_MONDAY
#define LV_CALENDAR_DEFAULT_DAY_NAMES       {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
#else
#define LV_CALENDAR_DEFAULT_DAY_NAMES       {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
#endif

#define LV_CALENDAR_DEFAULT_MONTH_NAMES     {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
#define LV_USE_CALENDAR_HEADER_ARROW        1
#define LV_USE_CALENDAR_HEADER_DROPDOWN     1
#endif

#define LV_USE_CANVAS                       1
#define LV_USE_CHART                        1
#define LV_USE_CHECKBOX                     1
#define LV_USE_DROPDOWN                     1
#define LV_USE_IMG                          1
#define LV_USE_IMGBTN                       1
#define LV_USE_KEYBOARD                     1
#define LV_USE_LABEL                        1

#if LV_USE_LABEL
#define LV_LABEL_TEXT_SELECTION             1
#define LV_LABEL_LONG_TXT_HINT              1
#endif

#define LV_USE_LED                          1
#define LV_USE_LINE                         1
#define LV_USE_LIST                         1
#define LV_USE_MENU                         1
#define LV_USE_METER                        1
#define LV_USE_MSGBOX                       1
#define LV_USE_ROLLER                       1
#define LV_USE_SLIDER                       1
#define LV_USE_SPAN                         1

#if LV_USE_SPAN
#define LV_SPAN_SNIPPET_STACK_SIZE          64
#endif

#define LV_USE_SPINBOX                      1
#define LV_USE_SPINNER                      1
#define LV_USE_SWITCH                       1
#define LV_USE_TEXTAREA                     1

#if LV_USE_TEXTAREA != 0
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME       1500
#endif

#define LV_USE_TABLE                        1
#define LV_USE_TABVIEW                      1
#define LV_USE_TILEVIEW                     1
#define LV_USE_WIN                          1
#define LV_USE_THEME_DEFAULT                1

#if LV_USE_THEME_DEFAULT
#define LV_THEME_DEFAULT_DARK               0
#define LV_THEME_DEFAULT_GROW               1
#define LV_THEME_DEFAULT_TRANSITION_TIME    80
#endif

#define LV_USE_THEME_BASIC                  1
#define LV_USE_THEME_MONO                   1

#define LV_USE_FLEX                         1
#define LV_USE_GRID                         1

#define LV_USE_FS_STDIO                     0
#if LV_USE_FS_STDIO
#define LV_FS_STDIO_LETTER                  '\0'
#define LV_FS_STDIO_PATH                    ""
#define LV_FS_STDIO_CACHE_SIZE              0
#endif

#define LV_USE_FS_POSIX                     0
#if LV_USE_FS_POSIX
#define LV_FS_POSIX_LETTER                  '\0'
#define LV_FS_POSIX_PATH                    ""
#define LV_FS_POSIX_CACHE_SIZE              0
#endif

#define LV_USE_FS_WIN32                     0
#if LV_USE_FS_WIN32
#define LV_FS_WIN32_LETTER                  '\0'
#define LV_FS_WIN32_PATH                    ""
#define LV_FS_WIN32_CACHE_SIZE              0
#endif

#define LV_USE_FS_FATFS                     0
#if LV_USE_FS_FATFS
#define LV_FS_FATFS_LETTER                  '\0'
#define LV_FS_FATFS_CACHE_SIZE              0
#endif

#define LV_USE_PNG                          1
#define LV_USE_BMP                          1
#define LV_USE_SJPG                         1
#define LV_USE_GIF                          1
#define LV_USE_QRCODE                       1
#define LV_USE_BARCODE                      1

#define LV_USE_FREETYPE                     0
#if LV_USE_FREETYPE
#define LV_FREETYPE_CACHE_SIZE              (64 * 1024)
#define LV_FREETYPE_USE_LVGL_PORT           0
#define LV_FREETYPE_SBIT_CACHE              0
#define LV_FREETYPE_CACHE_FT_FACES          4
#define LV_FREETYPE_CACHE_FT_SIZES          4
#endif

#define LV_USE_TINY_TTF                     0
#if LV_USE_TINY_TTF
#define LV_TINY_TTF_FILE_SUPPORT            0
#endif

#define LV_USE_RLOTTIE                      1

#define LV_USE_FFMPEG                       0
#if LV_USE_FFMPEG
#define LV_FFMPEG_DUMP_FORMAT               0
#endif

#define LV_USE_SNAPSHOT                     0
#define LV_USE_SYSMON                       0

#define LV_USE_PROFILER                     0
#if LV_USE_PROFILER
#define LV_USE_PROFILER_BUILTIN             1
#if LV_USE_PROFILER_BUILTIN
#define LV_PROFILER_BUILTIN_BUF_SIZE         (16 * 1024)
#endif

#define LV_PROFILER_INCLUDE                 "lvgl/src/misc/lv_profiler_builtin.h"

#define LV_PROFILER_BEGIN                   LV_PROFILER_BUILTIN_BEGIN
#define LV_PROFILER_END                     LV_PROFILER_BUILTIN_END
#endif

#define LV_USE_MONKEY                       0
#define LV_USE_GRIDNAV                      0
#define LV_USE_FRAGMENT                     0

#define LV_USE_IMGFONT                      1
#if LV_USE_IMGFONT
#define LV_IMGFONT_PATH_MAX_LEN             64
#define LV_IMGFONT_USE_IMG_CACHE_HEADER     0
#endif

#define LV_USE_MSG                          0

#define LV_USE_IME_PINYIN                   0
#if LV_USE_IME_PINYIN
#define LV_IME_PINYIN_USE_DEFAULT_DICT      1
#define LV_IME_PINYIN_CAND_TEXT_NUM         6

#define LV_IME_PINYIN_USE_K9_MODE           1
#if LV_IME_PINYIN_USE_K9_MODE == 1
#define LV_IME_PINYIN_K9_CAND_TEXT_NUM      3
#endif
#endif

#define LV_USE_FILE_EXPLORER                0
#if LV_USE_FILE_EXPLORER
#define LV_FILE_EXPLORER_PATH_MAX_LEN       (128)
#define LV_FILE_EXPLORER_QUICK_ACCESS       1
#endif

#define LV_USE_SDL                          0
#if LV_USE_SDL
#define LV_SDL_INCLUDE_PATH                 <SDL2/SDL.h>
#define LV_SDL_PARTIAL_MODE                 0
#define LV_SDL_FULLSCREEN                   0
#define LV_SDL_DIRECT_EXIT                  1
#endif

#define LV_USE_TFT_ESPI                     0

#define LV_USE_LINUX_FBDEV                  0
#if LV_USE_LINUX_FBDEV
#define LV_LINUX_FBDEV_BSD                  0
#endif

#define LV_USE_LINUX_DRM                    0

#endif
