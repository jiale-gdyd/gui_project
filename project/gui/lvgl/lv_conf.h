#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

#define LV_USE_DEV_VERSION

// 色深: 1(每像素1字节)、8(RGB332)、16(RGB565)、32(ARGB8888)
#define LV_COLOR_DEPTH                              32

// 如果使用色度键控，则不会绘制具有此颜色的图像像素
#define LV_COLOR_CHROMA_KEY                         lv_color_hex(0x00ff00)

// 启用和配置内置内存管理器
#define LV_USE_BUILTIN_MALLOC                       1

#if LV_USE_BUILTIN_MALLOC
// `lv_malloc()`可用的内存大小(>= 2kB)
#define LV_MEM_SIZE                                 (128U * 1024U)

// lv_malloc()的内存扩展大小(以字节为单位)
#define LV_MEM_POOL_EXPAND_SIZE                     0

// 为内存池设置地址，而不是将其分配为普通数组。也可以在外部SRAM中
#define LV_MEM_ADR                                  0     // 0: 未使用

// 给出一个内存分配器而不是一个地址，该分配器将被调用以获取LVGL的内存池。例如my_malloc
#if (LV_MEM_ADR == 0)
#undef LV_MEM_POOL_INCLUDE
#undef LV_MEM_POOL_ALLOC
#endif
#endif

// 使能lv_memcpy_builtin, lv_memset_builtin, lv_strlen_builtin, lv_strncpy_builtin
#define LV_USE_BUILTIN_MEMCPY                       1

// 启用和配置内置(v)snprintf
#define LV_USE_BUILTIN_SNPRINTF                     1

#if LV_USE_BUILTIN_SNPRINTF
#define LV_SPRINTF_USE_FLOAT                        0
#endif

#define LV_STDIO_INCLUDE                            <stdio.h>
#define LV_STDLIB_INCLUDE                           <stdlib.h>
#define LV_STRING_INCLUDE                           <string.h>

#define LV_MALLOC                                   lv_malloc_builtin
#define LV_REALLOC                                  lv_realloc_builtin
#define LV_FREE                                     lv_free_builtin
#define LV_MEMSET                                   lv_memset_builtin
#define LV_MEMCPY                                   lv_memcpy_builtin
#define LV_SNPRINTF                                 lv_snprintf_builtin
#define LV_VSNPRINTF                                lv_vsnprintf_builtin
#define LV_STRLEN                                   lv_strlen_builtin
#define LV_STRNCPY                                  lv_strncpy_builtin

#define LV_COLOR_EXTERN_INCLUDE                     <stdint.h>
#define LV_COLOR_MIX                                lv_color_mix
#define LV_COLOR_PREMULT                            lv_color_premult
#define LV_COLOR_MIX_PREMULT                        lv_color_mix_premult

// 默认显示刷新，输入设备读取和动画步骤周期
#define LV_DEF_REFR_PERIOD                          33 /*[ms]*/

// 使用自定义滴答源，以毫秒为单位告诉经过的时间。它消除了使用`lv_tick_inc()`手动更新刻度的需要
#define LV_TICK_CUSTOM                              0

#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE                      "Arduino.h"         // 系统时间函数的标题
#define LV_TICK_CUSTOM_SYS_TIME_EXPR                (millis())          // 以毫秒为单位计算当前系统时间的表达式
#endif

// 默认每英寸点数。用于初始化默认大小，例如小部件大小、样式填充。(不是很重要，你可以调整它来修改默认大小和空间)
#define LV_DPI_DEF                                  130

// 启用内置遮罩引擎。需要绘制阴影、圆角、圆、弧、斜线或任何其他蒙版
#define LV_USE_DRAW_MASKS                           1

#define LV_USE_DRAW_SW                              1

#if LV_USE_DRAW_SW
// 启用复杂的绘图引擎。需要绘制阴影、渐变、圆角、圆、弧、斜线、图像变换或任何蒙版
#define LV_DRAW_SW_COMPLEX                          1

// 如果小部件具有`style_opa < 255`(不是`bg_opa`、`text_opa`等)或不是NORMAL混合模式，它会在渲染前缓冲到“简单”层。小部件可以以较小的块缓冲。"变换层"(如果设置了`transform_angle/zoom`)使用更大的缓冲区并且不能以块的形式绘制

// 简单层块的目标缓冲区大小
#define LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE            (24 * 1024)

// 在无法分配`LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE`时使用
#define LV_DRAW_SW_LAYER_SIMPLE_FALLBACK_BUF_SIZE   (3 * 1024)

// 允许缓冲一些阴影计算。LV_DRAW_SW_SHADOW_CACHE_SIZE是最大值。缓冲区的阴影大小，其中阴影大小为`shadow_width + radius`缓存具有LV_DRAW_SW_SHADOW_CACHE_SIZE^2 RAM成本
#define LV_DRAW_SW_SHADOW_CACHE_SIZE                0

// 设置最大缓存的圆形数据的数量。为抗锯齿半径保存1/4圆的周长 * 每个圆使用4个字节(保存最常用的半径)0：禁用缓存
#define LV_DRAW_SW_CIRCLE_CACHE_SIZE                4

// 默认渐变缓冲区大小。当LVGL计算梯度"映射"时，它可以将它们保存到缓存中以避免再次计算它们。LV_DRAW_SW_GRADIENT_CACHE_DEF_SIZE设置此缓存的大小(以字节为单位)。如果缓存太小，则仅在绘图需要时分配地图。0表示不缓存
#define LV_DRAW_SW_GRADIENT_CACHE_DEF_SIZE          0

// 允许对渐变进行抖动处理(以在有限的颜色深度显示上实现视觉上的平滑颜色渐变)LV_DRAW_SW_GRADIENT_DITHER意味着分配一到两行对象的渲染表面。如果使用误差扩散，内存消耗的增加是(32位 * 对象宽度)加上24位 * 对象宽度
#define LV_DRAW_SW_GRADIENT_DITHER                  0

#if LV_DRAW_SW_GRADIENT_DITHER
// 添加对误差扩散抖动的支持。误差扩散抖动获得了更好的视觉效果，但在绘图时意味着更多的CPU消耗和内存。内存消耗的增加是(24位 * 对象的宽度)
#define LV_DRAW_SW_GRADIENT_DITHER_ERROR_DIFFUSION  0
#endif

// 启用子像素渲染
#define LV_DRAW_SW_FONT_SUBPX                       1

#if LV_DRAW_SW_FONT_SUBPX
// 设置显示的像素顺序。RGB通道的物理顺序。与"普通"字体无关
#define LV_DRAW_SW_FONT_SUBPX_BGR                   0           /* 0: RGB; 1:BGR */
#endif
#endif

// 使用SDL渲染API
#define LV_USE_DRAW_SDL                             0

#if LV_USE_DRAW_SDL
#define LV_DRAW_SDL_INCLUDE_PATH                    <SDL2/SDL.h>

// 纹理缓存大小，默认8MB
#define LV_DRAW_SDL_LRU_SIZE                        (1024 * 1024 * 8)

// 蒙版绘制的自定义混合模式，如果您需要与旧的SDL2库链接，请禁用
#define LV_DRAW_SDL_CUSTOM_BLEND_MODE               (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

// 使用Arm的2D加速库Arm-2D
#define LV_USE_GPU_ARM2D                            0

// 使用STM32的DMA2D(又名Chrom Art)GPU
#define LV_USE_GPU_STM32_DMA2D                      0

#if LV_USE_GPU_STM32_DMA2D
// 必须定义为包括目标处理器的 CMSIS 头的路径。例如"stm32f769xx.h"或"stm32f429xx.h"
#define LV_GPU_DMA2D_CMSIS_INCLUDE
#endif

#define LV_USE_GPU_GD32_IPA                         0

// 使用NXP的PXP GPU iMX RTxxx平台
#define LV_USE_GPU_NXP_PXP                          0

#if LV_USE_GPU_NXP_PXP
// 1: 为PXP(lv_gpu_nxp_pxp_osa.c)添加默认的裸机和FreeRTOS中断处理例程，并在lv_init()期间自动调用lv_gpu_nxp_pxp_init()。请注意，必须定义符号SDK_OS_FREE_RTOS才能使用FreeRTOS OSA，否则选择裸机实现。0: lv_gpu_nxp_pxp_init()必须在lv_init()之前手动调用
#define LV_USE_GPU_NXP_PXP_AUTO_INIT                0
#endif

// 使用NXP的VG-Lite GPU iMX RTxxx平台
#define LV_USE_GPU_NXP_VG_LITE                      0

// 使用SWM341的DMA2D GPU
#define LV_USE_GPU_SWM341_DMA2D                     0

#if LV_USE_GPU_SWM341_DMA2D
#define LV_GPU_SWM341_DMA2D_INCLUDE                 "SWM341.h"
#endif

// 启用日志模块
#define LV_USE_LOG                                  1

#if LV_USE_LOG
/**
 * 应该添加重要的日志:
 *  - LV_LOG_LEVEL_TRACE       大量日志提供详细信息
 *  - LV_LOG_LEVEL_INFO        记录重要事件
 *  - LV_LOG_LEVEL_WARN        记录是否发生了不需要的事情但没有引起问题
 *  - LV_LOG_LEVEL_ERROR       只有关键问题，当系统可能会失败
 *  - LV_LOG_LEVEL_USER        仅用户添加的日志
 *  - LV_LOG_LEVEL_NONE        不要记录任何东西
 */
#define LV_LOG_LEVEL                                LV_LOG_LEVEL_WARN

// 1: 用'printf'打印日志；0: 用户需要使用`lv_log_register_print_cb()`注册回调
#define LV_LOG_PRINTF                               1

// 1: 使能打印时间戳; 0: 禁止打印时间戳
#define LV_LOG_USE_TIMESTAMP                        1

/*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
#define LV_LOG_TRACE_MEM                            1
#define LV_LOG_TRACE_TIMER                          1
#define LV_LOG_TRACE_INDEV                          1
#define LV_LOG_TRACE_DISP_REFR                      1
#define LV_LOG_TRACE_EVENT                          1
#define LV_LOG_TRACE_OBJ_CREATE                     1
#define LV_LOG_TRACE_LAYOUT                         1
#define LV_LOG_TRACE_ANIM                           1
#define LV_LOG_TRACE_MSG                            1
#endif

// 如果操作失败或发现无效数据，则启用断言。如果启用LV_USE_LOG，将在失败时打印错误消息
#define LV_USE_ASSERT_NULL                          1   // 检查参数是否为NULL。(非常快，推荐)
#define LV_USE_ASSERT_MALLOC                        1   // 检查内存是否成功分配。(非常快，推荐)
#define LV_USE_ASSERT_STYLE                         0   // 检查样式是否正确初始化。(非常快，推荐)
#define LV_USE_ASSERT_MEM_INTEGRITY                 0   // 在关键操作后检查`lv_mem`的完整性。(慢)
#define LV_USE_ASSERT_OBJ                           0   // 检查对象的类型和存在(例如未删除)。(慢)

// 当断言发生时添加一个自定义处理程序，例如 重新启动MCU
#define LV_ASSERT_HANDLER_INCLUDE                   <stdint.h>
#define LV_ASSERT_HANDLER                           while (1);

// 1：显示CPU使用率和FPS计数
#define LV_USE_PERF_MONITOR                         1

#if LV_USE_PERF_MONITOR
#define LV_USE_PERF_MONITOR_POS                     LV_ALIGN_BOTTOM_RIGHT
#endif

// 1：显示已用内存和内存碎片。需要`LV_USE_BUILTIN_MALLOC = 1`
#define LV_USE_MEM_MONITOR                          0

#if LV_USE_MEM_MONITOR
#define LV_USE_MEM_MONITOR_POS                      LV_ALIGN_BOTTOM_LEFT
#endif

// 1: 在重绘区域上绘制随机彩色矩形
#define LV_USE_REFR_DEBUG                           0

// 为旋转分配的最大缓冲区大小。仅在显示驱动程序中启用软件旋转时使用
#define LV_DISP_ROT_MAX_BUF                         (10*1024)

// 使用用户数据
#define LV_USE_USER_DATA                            1

// 垃圾收集器设置。如果lvgl绑定到更高级别的语言并且内存由该语言管理，则使用
#define LV_ENABLE_GC                                0

#if (LV_ENABLE_GC != 0)
#define LV_GC_INCLUDE                               "gc.h"
#endif

// 默认图像缓存大小。图像缓存使一些图像保持打开状态。如果仅使用内置图像格式，则缓存没有真正的优势。使用其他图像解码器(例如PNG或JPG)缓存保存图像的连续打开/解码。但是，打开的图像会消耗额外的RAM。0：禁用缓存
#define LV_IMG_CACHE_DEF_SIZE                       0

// 每个渐变允许的停靠点数。增加它以允许更多停止。这将增加(sizeof(lv_color_t) + 1)字节每个额外的停止
#define LV_GRADIENT_MAX_STOPS                       2

// 调整颜色混合功能四舍五入。GPU可能会以不同的方式计算颜色混合(混合)。0：向下舍入，64：从x.75向上舍入，128：从一半向上舍入，192：从x.25向上舍入，254：向上舍入
#define LV_COLOR_MIX_ROUND_OFS                      0

// 对于设置为1的大端系统
#define LV_BIG_ENDIAN_SYSTEM                        0

// 为`lv_tick_inc`函数定义自定义属性
#define LV_ATTRIBUTE_TICK_INC

// 为`lv_timer_handler`函数定义自定义属性
#define LV_ATTRIBUTE_TIMER_HANDLER

// 为`lv_disp_flush_ready`函数定义自定义属性
#define LV_ATTRIBUTE_FLUSH_READY

// 缓冲区所需的对齐大小
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE                 1

// 将添加到内存需要对齐的地方(默认情况下，-Os数据可能不会与边界对齐)。 例如__attribute__((aligned(4)))
#define LV_ATTRIBUTE_MEM_ALIGN

// 标记大型常量数组的属性，例如字体的位图
#define LV_ATTRIBUTE_LARGE_CONST

// RAM中大数组声明的编译器前缀
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

// 将性能关键功能放入更快的内存(例如RAM)
#define LV_ATTRIBUTE_FAST_MEM

// 将整数常量导出到绑定。该宏与LV_<CONST>形式的常量一起使用，这些常量也应该出现在LVGL绑定API上，例如Micropython
#define LV_EXPORT_CONST_INT(int_value)              struct _silence_gcc_warning

// 通过使用int32_t而不是int16_t作为坐标，将默认的-32k..32k坐标范围扩展到-4M..4M
#define LV_USE_LARGE_COORD                          0

// 带有ASCII范围的蒙特塞拉特字体和一些使用bpp = 4的符号
#define LV_FONT_MONTSERRAT_8                        1
#define LV_FONT_MONTSERRAT_10                       1
#define LV_FONT_MONTSERRAT_12                       1
#define LV_FONT_MONTSERRAT_14                       1
#define LV_FONT_MONTSERRAT_16                       1
#define LV_FONT_MONTSERRAT_18                       1
#define LV_FONT_MONTSERRAT_20                       1
#define LV_FONT_MONTSERRAT_22                       1
#define LV_FONT_MONTSERRAT_24                       1
#define LV_FONT_MONTSERRAT_26                       1
#define LV_FONT_MONTSERRAT_28                       1
#define LV_FONT_MONTSERRAT_30                       1
#define LV_FONT_MONTSERRAT_32                       1
#define LV_FONT_MONTSERRAT_34                       1
#define LV_FONT_MONTSERRAT_36                       1
#define LV_FONT_MONTSERRAT_38                       1
#define LV_FONT_MONTSERRAT_40                       1
#define LV_FONT_MONTSERRAT_42                       1
#define LV_FONT_MONTSERRAT_44                       1
#define LV_FONT_MONTSERRAT_46                       1
#define LV_FONT_MONTSERRAT_48                       1

// 展示特殊功能
#define LV_FONT_MONTSERRAT_12_SUBPX                 1
#define LV_FONT_MONTSERRAT_28_COMPRESSED            1     // bpp = 3
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW            1     // 希伯来文、阿拉伯文、波斯文字母及其所有形式
#define LV_FONT_SIMSUN_16_CJK                       1     // 1000个最常见的CJK部首

// 像素完美的等宽字体
#define LV_FONT_UNSCII_8                            1
#define LV_FONT_UNSCII_16                           1

// 可选择在此处声明自定义字体。您也可以将这些字体用作默认字体，它们将在全球范围内可用。例如。#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)
#define LV_FONT_CUSTOM_DECLARE

// 始终设置默认字体
#define LV_FONT_DEFAULT                             &lv_font_montserrat_24

// 启用处理大字体和/或包含大量字符的字体。限制取决于字体大小、字体和bpp。如果字体需要它，将触发编译器错误
#define LV_FONT_FMT_TXT_LARGE                       1

// 启用/禁用对压缩字体的支持
#define LV_USE_FONT_COMPRESSED                      1

// 未找到字形dsc时启用绘图占位符
#define LV_USE_FONT_PLACEHOLDER                     1

// 选择字符串的字符编码。您的IDE或编辑器应该具有相同的字符编码LV_TXT_ENC_UTF8/LV_TXT_ENC_ASCII
#define LV_TXT_ENC                                  LV_TXT_ENC_UTF8

// 可以在这些字符上打断(换行)文本
#define LV_TXT_BREAK_CHARS                          " ,.;:-_)]}"

// 如果一个单词至少有这么长，就会在"prettiest"的地方中断，要禁用，设置一个值<=0
#define LV_TXT_LINE_BREAK_LONG_LEN                  0

// 长单词中在断点前放在一行中的最小字符数。 取决于LV_TXT_LINE_BREAK_LONG_LEN
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN          3

// 长单词中在断点后放在一行中的最少字符数。取决于LV_TXT_LINE_BREAK_LONG_LEN
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN         3

// 用于信号文本重新着色的控制字符
#define LV_TXT_COLOR_CMD                            "#"

// 支持双向文本。允许混合从左到右和从右到左的文本
#define LV_USE_BIDI                                 0

#if LV_USE_BIDI
// 设置默认方向
#define LV_BIDI_BASE_DIR_DEF                        LV_BASE_DIR_AUTO
#endif

// 启用阿拉伯语/波斯语处理。在这些语言中，字符应根据其在文本中的位置替换为其他形式
#define LV_USE_ARABIC_PERSIAN_CHARS                 0

#define LV_USE_ANIMIMG                              1
#define LV_USE_ARC                                  1
#define LV_USE_BAR                                  1
#define LV_USE_BTN                                  1
#define LV_USE_BTNMATRIX                            1

#define LV_USE_CALENDAR                             1

#if LV_USE_CALENDAR
#define LV_CALENDAR_WEEK_STARTS_MONDAY              0
#if LV_CALENDAR_WEEK_STARTS_MONDAY
#define LV_CALENDAR_DEFAULT_DAY_NAMES               {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
#else
#define LV_CALENDAR_DEFAULT_DAY_NAMES               {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
#endif

#define LV_CALENDAR_DEFAULT_MONTH_NAMES             {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
#define LV_USE_CALENDAR_HEADER_ARROW                1
#define LV_USE_CALENDAR_HEADER_DROPDOWN             1
#endif

#define LV_USE_CANVAS                               1
#define LV_USE_CHART                                1
#define LV_USE_CHECKBOX                             1
#define LV_USE_COLORWHEEL                           1
#define LV_USE_DROPDOWN                             1
#define LV_USE_IMG                                  1
#define LV_USE_IMGBTN                               1
#define LV_USE_KEYBOARD                             1

#define LV_USE_LABEL                                1
#if LV_USE_LABEL
// 启用选择标签的文本
#define LV_LABEL_TEXT_SELECTION                     1

// 在标签中存储一些额外的信息以加快超长文本的绘制速度
#define LV_LABEL_LONG_TXT_HINT                      1
#endif

#define LV_USE_LED                                  1
#define LV_USE_LINE                                 1
#define LV_USE_LIST                                 1
#define LV_USE_MENU                                 1
#define LV_USE_METER                                1
#define LV_USE_MSGBOX                               1

#define LV_USE_ROLLER                               1
#define LV_USE_SLIDER                               1

#define LV_USE_SPAN                                 1
#if LV_USE_SPAN
// 一行文本可以包含最大数量的跨度描述符
#define LV_SPAN_SNIPPET_STACK_SIZE                  64
#endif

#define LV_USE_SPINBOX                              1
#define LV_USE_SPINNER                              1
#define LV_USE_SWITCH                               1

#define LV_USE_TEXTAREA                             1
#if LV_USE_TEXTAREA != 0
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME               1500
#endif

#define LV_USE_TABLE                                1
#define LV_USE_TABVIEW                              1
#define LV_USE_TILEVIEW                             1
#define LV_USE_WIN                                  1

// 一个简单，令人印象深刻且非常完整的主题
#define LV_USE_THEME_DEFAULT                        1

#if LV_USE_THEME_DEFAULT
// 0: 亮模式；1: 暗模式
#define LV_THEME_DEFAULT_DARK                       0

// 1: 在媒体上启用增长
#define LV_THEME_DEFAULT_GROW                       1

// 默认转换时间[ms]
#define LV_THEME_DEFAULT_TRANSITION_TIME            80
#endif

// 一个非常简单的它们，是自定义主题的良好起点
#define LV_USE_THEME_BASIC                          1

// 专为单色显示器设计的主题
#define LV_USE_THEME_MONO                           1

// 一种类似于CSS中Flexbox的布局
#define LV_USE_FLEX                                 1

// 类似于CSS中的Grid的布局
#define LV_USE_GRID                                 1

// 1: 启用API对对象进行快照
#define LV_USE_SNAPSHOT                             1

// 1:使能Monkey测试
#define LV_USE_MONKEY                               1

// 1:启用网格导航
#define LV_USE_GRIDNAV                              1

// 1:启用lv_obj片段
#define LV_USE_FRAGMENT                             1

// 1:支持在标签或跨度小部件中使用图像作为字体
#define LV_USE_IMGFONT                              1

#if LV_USE_IMGFONT
// imgfont图片文件路径最大长度
#define LV_IMGFONT_PATH_MAX_LEN                     64

// 1：使用img缓存缓存头部信息
#define LV_IMGFONT_USE_IMG_CACHE_HEADER             0
#endif

// 1:启用基于已发布订阅者的消息系统
#define LV_USE_MSG                                  1

// 内置TTF解码器
#define LV_USE_TINY_TTF                             1
#if LV_USE_TINY_TTF
// 启用从文件加载 TTF 数据
#define LV_TINY_TTF_FILE_SUPPORT                    1
#endif

#define LV_USE_PNG                                  1
#define LV_USE_BMP                                  1
#define LV_USE_GIF                                  1
#define LV_USE_SJPG                                 1
#define LV_USE_QRCODE                               1
#define LV_USE_BARCODE                              1
#define LV_USE_RLOTTIE                              1

// 1启用拼音输入法。需要：lv_keyboard
#define LV_USE_IME_PINYIN                           1
#if LV_USE_IME_PINYIN
// 1：使用默认词库。如果不使用默认词库，请务必在设置词库后使用`lv_ime_pinyin`
#define LV_IME_PINYIN_USE_DEFAULT_DICT              1

// 设置可以显示的候选面板的最大数量。这个需要根据屏幕的大小进行调整
#define LV_IME_PINYIN_CAND_TEXT_NUM                 6

// 使用9键输入(k9)
#define LV_IME_PINYIN_USE_K9_MODE                   1
#if (LV_IME_PINYIN_USE_K9_MODE == 1)
#define LV_IME_PINYIN_K9_CAND_TEXT_NUM              3
#endif
#endif

// 1：启用文件资源管理器。要求：lv_table
#define LV_USE_FILE_EXPLORER                        1
#if LV_USE_FILE_EXPLORER
#define LV_FILE_EXPLORER_PATH_MAX_LEN               (128)
#define LV_FILE_EXPLORER_QUICK_ACCESS               1
#endif

// 使用SDL在PC上打开窗口，处理鼠标和键盘
#define LV_USE_SDL                                  0
#if LV_USE_SDL
#define LV_SDL_INCLUDE_PATH                         <SDL2/SDL.h>
#endif

// /dev/fb的驱动
#define LV_USE_LINUX_FBDEV                          0
#if LV_USE_LINUX_FBDEV
#define LV_LINUX_FBDEV_BSD                          0
#endif

#define LV_USE_TFT_ESPI                             0

#define LV_USE_FS_STDIO                             0
#if LV_USE_FS_STDIO
#define LV_FS_STDIO_LETTER                          '\0'        // 设置一个可访问驱动器的大写字母(例如。'A')
#define LV_FS_STDIO_PATH                            ""          // 设置工作目录。文件/目录路径将被追加到它
#define LV_FS_STDIO_CACHE_SIZE                      0           // >0表示在lv_fs_read()中缓存这个字节数
#endif

// FATFS(需要单独添加). f_open, f_read, etc
#define LV_USE_FS_FATFS                             0
#if LV_USE_FS_FATFS
#define LV_FS_FATFS_LETTER                          '\0'        // 设置一个可访问驱动器的大写字母(例如。'A')
#define LV_FS_FATFS_CACHE_SIZE                      0           // >0表示在lv_fs_read()中缓存这个字节数
#endif

// open, read, etc
#define LV_USE_FS_POSIX                             1
#if LV_USE_FS_POSIX
#define LV_FS_POSIX_LETTER                          'B'         // 设置一个可访问驱动器的大写字母(例如。'A')
#define LV_FS_POSIX_PATH                            "/root"     // 设置工作目录。文件/目录路径将被追加到它
#define LV_FS_POSIX_CACHE_SIZE                      0           // >0表示在lv_fs_read()中缓存这个字节数
#endif

#define LV_USE_FFMPEG                               0
#if LV_USE_FFMPEG
#define LV_FFMPEG_DUMP_FORMAT                       0
#endif

#define LV_USE_FREETYPE                             0

#if LV_USE_FREETYPE
#define LV_FREETYPE_CACHE_SIZE                      (64 * 1024)
#define LV_FREETYPE_USE_LVGL_PORT                   0
#define LV_FREETYPE_SBIT_CACHE                      0
#define LV_FREETYPE_CACHE_FT_FACES                  4
#define LV_FREETYPE_CACHE_FT_SIZES                  4
#endif

#endif
