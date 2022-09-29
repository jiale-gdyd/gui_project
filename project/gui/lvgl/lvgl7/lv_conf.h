#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

// 库支持的最大水平和垂直分辨率
#define LV_HOR_RES_MAX                          (1440)
#define LV_VER_RES_MAX                          (810)

// 颜色深度。1: 每像素1个字节；8: RGB332；16: RGB565；32: ARGB8888
#define LV_COLOR_DEPTH                          32

// 交换RGB565颜色的2个字节。如果显示器具有8位接口(例如 SPI)，则很有用
#define LV_COLOR_16_SWAP                        0

// 1: 启用屏幕透明度。对OSD或其他重叠GUI很有用。需要`LV_COLOR_DEPTH = 32`颜色，屏幕样式需要修改: `style.body.opa = ...`
#define LV_COLOR_SCREEN_TRANSP                  1

// 不会绘制具有此颜色的图像像素(使用色度键控)
#define LV_COLOR_TRANSP                         LV_COLOR_LIME

// 启用抗锯齿(线条和半径将被平滑)
#define LV_ANTIALIAS                            1

// 默认显示刷新周期。可以在显示驱动程序(`lv_disp_drv_t`)中更改
#define LV_DISP_DEF_REFR_PERIOD                 10

// 每英寸点数：用于初始化默认大小。例如，宽度为LV_DPI/2 -> 半英寸宽的按钮(不那么重要，您可以调整它以修改默认大小和空间)
#define LV_DPI                                  130

// 显示的实际宽度会更改一些默认值：默认对象大小、示例布局等。根据显示的宽度(hor. res. / dpi)，显示分为4类。第4个是特大号，没有上限所以不在这里列出。类别的上限在下面以0.1英寸为单位设置
#define LV_DISP_SMALL_LIMIT                     30
#define LV_DISP_MEDIUM_LIMIT                    50
#define LV_DISP_LARGE_LIMIT                     70

// 坐标类型。应该是`int16_t`(或者极端情况下的`int32_t`)
typedef int16_t lv_coord_t;

// 为轮换分配的最大缓冲区大小。仅在启用软件旋转时使用
#define LV_DISP_ROT_MAX_BUF                     (10U * 1024U)

// 1: 使用自定义malloc/free, 0: 使用内置的`lv_mem_alloc`和`lv_mem_free`
#define LV_MEM_CUSTOM                           1

#if (LV_MEM_CUSTOM == 0)
// `lv_mem_alloc`使用的内存大小，以字节为单位(>= 2kB)
#define LV_MEM_SIZE                             (32U * 1024U)

// 大数组声明的编译器前缀
#define LV_MEM_ATTR

// 为内存池设置地址，而不是将其分配为数组。也可以在外部SRAM中
#define LV_MEM_ADR                              0

// 自动碎片整理。碎片整理。表示加入相邻的空闲单元格
#define LV_MEM_AUTO_DEFRAG                      1
#else
#define LV_MEM_CUSTOM_INCLUDE                   <stdlib.h>
#define LV_MEM_CUSTOM_ALLOC                     malloc
#define LV_MEM_CUSTOM_FREE                      free
#endif

// 使用标准的memcpy和memset代替LVGL自己的函数。标准函数可能会更快，也可能不会更快，具体取决于它们的实现
#define LV_MEMCPY_MEMSET_STD                    1

// 垃圾收集器设置。如果lvgl绑定到更高级别的语言并且内存由该语言管理，则使用
#define LV_ENABLE_GC                            0

#if (LV_ENABLE_GC != 0)
#define LV_GC_INCLUDE                           "gc.h"
#define LV_MEM_CUSTOM_REALLOC                   your_realloc
#define LV_MEM_CUSTOM_GET_SIZE                  your_mem_get_size
#endif

// 以毫秒为单位的输入设备读取周期
#define LV_INDEV_DEF_READ_PERIOD                10

// 以像素为单位的拖动阈值
#define LV_INDEV_DEF_DRAG_LIMIT                 10

// 拖投减速[%]。更大的价值 -> 更快的减速
#define LV_INDEV_DEF_DRAG_THROW                 10

// 长按时间以毫秒为单位。是时候发送`LV_EVENT_LONG_PRESSED`
#define LV_INDEV_DEF_LONG_PRESS_TIME            400

// 长按重复触发周期 [ms]。`LV_EVENT_LONG_PRESSED_REPEAT 之间的时间
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME        100

// 以像素为单位的手势阈值
#define LV_INDEV_DEF_GESTURE_LIMIT              50

// 滑动前释放手势的最小速度(像素)
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY       3

// 1：启用动画
#define LV_USE_ANIMATION                        1

#if LV_USE_ANIMATION
// 声明动画的用户数据类型(例如可以是`void *`、`int`、`struct`)
typedef void *lv_anim_user_data_t;
#endif

// 1：在矩形上启用阴影绘制
#define LV_USE_SHADOW                           1

#if LV_USE_SHADOW
// 允许缓冲一些阴影计算LV_SHADOW_CACHE_SIZE是最大值。要缓冲的阴影大小，其中阴影大小为`shadow_width + radius`缓存具有LV_SHADOW_CACHE_SIZE^2 RAM成本
#define LV_SHADOW_CACHE_SIZE                    0
#endif

// 1：在矩形上启用轮廓绘制
#define LV_USE_OUTLINE                          1

// 1：启用矩形上的图案绘制
#define LV_USE_PATTERN                          1

// 1：在矩形上启用值字符串绘制
#define LV_USE_VALUE_STR                        1

// 1：使用其他混合模式而不是正常模式(`LV_BLEND_MODE_...`)
#define LV_USE_BLEND_MODES                      1

// 1：使用`opa_scale`样式属性一次设置对象及其子对象的不透明度
#define LV_USE_OPA_SCALE                        1

// 1：使用图像缩放和旋转
#define LV_USE_IMG_TRANSFORM                    1

// 1：启用对象组(用于键盘/编码器导航)
#define LV_USE_GROUP                            1

#if LV_USE_GROUP
typedef void *lv_group_user_data_t;
#endif

// 1：启用GPU接口
#define LV_USE_GPU                              0   // 仅在显示中启用`gpu_fill_cb`和`gpu_blend_cb`。drv-
#define LV_USE_GPU_STM32_DMA2D                  0

// 如果启用 LV_USE_GPU_STM32_DMA2D，则必须定义LV_GPU_DMA2D_CMSIS_INCLUDE以包含目标处理器的CMSIS标头的路径，例如"stm32f769xx.h"或"stm32f429xx.h"
#define LV_GPU_DMA2D_CMSIS_INCLUDE

// 1：在NXP RTxxx平台上使用PXP进行CPU卸载
#define LV_USE_GPU_NXP_PXP                      0

// 1：为PXP(lv_gpu_nxp_pxp_osa.c)添加默认的裸机和FreeRTOS中断处理例程，并在lv_init()期间自动调用lv_gpu_nxp_pxp_init()。请注意，必须定义符号FSL_RTOS_FREE_RTOS才能使用FreeRTOS OSA，否则选择裸机实现。0：lv_gpu_nxp_pxp_init()必须在lv_init()之前手动调用
#define LV_USE_GPU_NXP_PXP_AUTO_INIT            0

// 1：在NXP RTxxx平台上使用VG-Lite进行CPU卸载
#define LV_USE_GPU_NXP_VG_LITE                  0

// 1：向驱动程序和对象添加`user_data`
#define LV_USE_USER_DATA                        1

// 1：在右下角显示CPU使用率和FPS计数
#define LV_USE_PERF_MONITOR                     1

// 1：如果可能，使用旧API中的函数和类型
#define LV_USE_API_EXTENSION_V6                 1
#define LV_USE_API_EXTENSION_V7                 1

// 1：启用索引(调色板)图像
#define LV_IMG_CF_INDEXED                       1

// 1：启用alpha索引图像
#define LV_IMG_CF_ALPHA                         1

// 默认图像缓存大小。图像缓存使图像保持打开状态。如果仅使用内置图像格式，则缓存没有真正的优势。(即没有添加新的图像解码器)使用复杂的图像解码器(例如PNG或JPG)缓存可以保存图像的连续打开/解码。但是，打开的图像可能会消耗额外的 RAM。将其设置为0以禁用缓存
#define LV_IMG_CACHE_DEF_SIZE                   1

// 声明图像解码器的用户数据类型(例如可以是`void *`、`int`、`struct`)
typedef void *lv_img_decoder_user_data_t;

// 对于设置为1的大端系统
#define LV_BIG_ENDIAN_SYSTEM                    0

// 为`lv_tick_inc`函数定义自定义属性
#define LV_ATTRIBUTE_TICK_INC

// 为`lv_task_handler`函数定义自定义属性
#define LV_ATTRIBUTE_TASK_HANDLER

// 为`lv_disp_flush_ready`函数定义自定义属性
#define LV_ATTRIBUTE_FLUSH_READY

// 缓冲区所需的对齐大小
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE

// 使用大小优化(-Os)时，编译器可能不会将数据与4或8字节边界对齐。有些硬件甚至可能需要32或64个字节。这种对齐将在需要的地方明确应用。LV_ATTRIBUTE_MEM_ALIGN_SIZE应该用于指定所需的对齐大小。例如，__attribute__((aligned(LV_ATTRIBUTE_MEM_ALIGN_SIZE)))
#define LV_ATTRIBUTE_MEM_ALIGN

// 标记大型常量数组的属性，例如字体的位图
#define LV_ATTRIBUTE_LARGE_CONST

// 为性能关键函数添加前缀以将它们放入更快的内存(例如RAM)中。使用15-20 kB额外内存
#define LV_ATTRIBUTE_FAST_MEM

// 将整数常量导出到绑定。该宏与LV_<CONST>形式的常量一起使用，这些常量也应该出现在lvgl绑定API上，例如Micropython。默认值只是防止GCC警告
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning

// 在GPU加速操作中使用的前缀变量，通常需要放置在DMA可访问的RAM部分中
#define LV_ATTRIBUTE_DMA

// 1：使用自定义刻度源。它消除了使用`lv_tick_inc`手动更新刻度的需要
#define LV_TICK_CUSTOM                          0

#if (LV_TICK_CUSTOM == 1)
#define LV_TICK_CUSTOM_INCLUDE                  "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR            (millis())
#endif

// 显示驱动程序中的用户数据类型
typedef void *lv_disp_drv_user_data_t;

// 输入设备驱动程序中的用户数据类型
typedef void *lv_indev_drv_user_data_t;

// 1：启用日志模块
#define LV_USE_LOG                              0

#if LV_USE_LOG
/** 
 * 应该添加重要的日志：
 *   - LV_LOG_LEVEL_TRACE       大量日志提供详细信息
 *   - LV_LOG_LEVEL_INFO        记录重要事件
 *   - LV_LOG_LEVEL_WARN        记录是否发生了不需要的事情但没有引起问题
 *   - LV_LOG_LEVEL_ERROR       只有关键问题，当系统可能会失败
 *   - LV_LOG_LEVEL_NONE        不要记录任何东西
 */
#define LV_LOG_LEVEL                            LV_LOG_LEVEL_WARN

// 1：用'printf'打印日志；0：用户需要使用`lv_log_register_print_cb`注册回调
#define LV_LOG_PRINTF                           1
#endif

// 如果启用调试，LittelvGL会验证函数的参数。如果发现无效参数，则会打印错误日志消息，并且MCU会在出现错误时停止。(应该启用`LV_USE_LOG`)。如果您正在调试MCU，您可以暂停调试器以准确查看问题所在。可以通过在此处重新定义断言来覆盖断言的行为。例如，#define LV_ASSERT_MEM(p) <my_assert_code>
#define LV_USE_DEBUG                            1

#if LV_USE_DEBUG

// 检查参数是否为NULL。(蛮快)
#define LV_USE_ASSERT_NULL                      1

// 检查内存是否成功分配。(蛮快)
#define LV_USE_ASSERT_MEM                       1

// 在关键操作后检查`lv_mem`的完整性。(慢)
#define LV_USE_ASSERT_MEM_INTEGRITY             0

// 检查字符串。搜索 NULL、很长的字符串、无效字符和不自然的重复。(慢)。如果禁用`LV_USE_ASSERT_NULL`将改为执行(如果启用)
#define LV_USE_ASSERT_STR                       0

// 检查NULL、对象的类型和存在(例如未删除)。(相当慢)。如果禁用`LV_USE_ASSERT_NULL`将改为执行(如果启用)
#define LV_USE_ASSERT_OBJ                       0

// 检查样式是否正确初始化。(快速地)
#define LV_USE_ASSERT_STYLE                     1
#endif

// 蒙特塞拉特字体bpp = 4
#define LV_FONT_MONTSERRAT_8                    1
#define LV_FONT_MONTSERRAT_10                   1
#define LV_FONT_MONTSERRAT_12                   1
#define LV_FONT_MONTSERRAT_14                   1
#define LV_FONT_MONTSERRAT_16                   1
#define LV_FONT_MONTSERRAT_18                   1
#define LV_FONT_MONTSERRAT_20                   1
#define LV_FONT_MONTSERRAT_22                   1
#define LV_FONT_MONTSERRAT_24                   1
#define LV_FONT_MONTSERRAT_26                   1
#define LV_FONT_MONTSERRAT_28                   1
#define LV_FONT_MONTSERRAT_30                   1
#define LV_FONT_MONTSERRAT_32                   1
#define LV_FONT_MONTSERRAT_34                   1
#define LV_FONT_MONTSERRAT_36                   1
#define LV_FONT_MONTSERRAT_38                   1
#define LV_FONT_MONTSERRAT_40                   1
#define LV_FONT_MONTSERRAT_42                   1
#define LV_FONT_MONTSERRAT_44                   1
#define LV_FONT_MONTSERRAT_46                   1
#define LV_FONT_MONTSERRAT_48                   1

// 展示特殊功能
#define LV_FONT_MONTSERRAT_12_SUBPX             1
#define LV_FONT_MONTSERRAT_28_COMPRESSED        1  // bpp = 3
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW        1  // 希伯来语、阿拉伯语、PErisan字母及其所有形式
#define LV_FONT_SIMSUN_16_CJK                   1  // 1000个最常见的CJK部首

// 像素完美等宽字体
#define LV_FONT_UNSCII_8                        1
#define LV_FONT_UNSCII_16                       1

// 可选择在此处声明您的自定义字体。您也可以将这些字体用作默认字体，它们将在全球范围内可用。例如，#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)
#define LV_FONT_CUSTOM_DECLARE

// 如果您的字体包含大量字符，请启用它。限制取决于字体大小、字体和bpp，但如果您看到可能需要启用它的问题，则超过10,000个字符
#define LV_FONT_FMT_TXT_LARGE                   1

// 启用/禁用对压缩字体的支持。如果它被禁用，压缩的字形不能被库处理，也不会被渲染
#define LV_USE_FONT_COMPRESSED                  1

// 启用子像素渲染
#define LV_USE_FONT_SUBPX                       1

#if LV_USE_FONT_SUBPX
// 设置显示器的像素顺序。仅当使用subpx字体时才重要。使用普通字体没关系
#define LV_FONT_SUBPX_BGR                       0
#endif

// 声明字体的用户数据的类型(例如可以是`void *`、`int`、`struct`)
typedef void *lv_font_user_data_t;

// 没有主题，您可以根据需要应用您的样式。没有标志。设置LV_THEME_DEFAULT_FLAG 0
#define LV_USE_THEME_EMPTY                      1

// 简单地基于它创建您的主题。没有旗帜。设置LV_THEME_DEFAULT_FLAG 0
#define LV_USE_THEME_TEMPLATE                   1

/**
 * 一个快速而令人印象深刻的主题
 * Flags:
 *   - LV_THEME_MATERIAL_FLAG_LIGHT:         轻主题
 *   - LV_THEME_MATERIAL_FLAG_DARK:          黑暗主题
 *   - LV_THEME_MATERIAL_FLAG_NO_TRANSITION: 禁用转换(状态更改动画)
 *   - LV_THEME_MATERIAL_FLAG_NO_FOCUS:      禁用聚焦状态的指示
 */
#define LV_USE_THEME_MATERIAL                   1

// 单色显示的单色主题。如果LV_THEME_DEFAULT_COLOR_PRIMARY是LV_COLOR_BLACK，则文本和边框将为黑色，背景将为白色。否则颜色反转。没有旗帜。设置LV_THEME_DEFAULT_FLAG 0
#define LV_USE_THEME_MONO                       1

#define LV_THEME_DEFAULT_INCLUDE                <stdint.h>
#define LV_THEME_DEFAULT_INIT                   lv_theme_material_init

#define LV_THEME_DEFAULT_COLOR_PRIMARY          lv_color_hex(0x01A2B1)
#define LV_THEME_DEFAULT_COLOR_SECONDARY        lv_color_hex(0x44D1B6)

#define LV_THEME_DEFAULT_FLAG                   LV_THEME_MATERIAL_FLAG_LIGHT

#define LV_THEME_DEFAULT_FONT_SMALL             &lv_font_montserrat_24
#define LV_THEME_DEFAULT_FONT_NORMAL            &lv_font_montserrat_24
#define LV_THEME_DEFAULT_FONT_SUBTITLE          &lv_font_montserrat_24
#define LV_THEME_DEFAULT_FONT_TITLE             &lv_font_montserrat_24

// 选择字符串的字符编码。您的IDE或编辑器应该具有相同的字符编码。LV_TXT_ENC_UTF8、LV_TXT_ENC_ASCII
#define LV_TXT_ENC                              LV_TXT_ENC_UTF8

// 可以在这些字符上打断(换行)文本
#define LV_TXT_BREAK_CHARS                      " ,.;:-_"

// 如果一个词至少有这么长，就会在最漂亮的地方断掉。要禁用，请设置为<=0的值
#define LV_TXT_LINE_BREAK_LONG_LEN              0

// 长单词中在断点前放在一行中的最小字符数。取决于LV_TXT_LINE_BREAK_LONG_LEN
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN      3

// 长单词中在断点后放在一行中的最少字符数。取决于LV_TXT_LINE_BREAK_LONG_LEN
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN     3

// 用于信号文本重新着色的控制字符
#define LV_TXT_COLOR_CMD                        "#"

// 支持双向文本。允许混合从左到右和从右到左的文本
#define LV_USE_BIDI                             1

#if LV_USE_BIDI
/** 
 * 设置默认方向。支持的值：
 *   - `LV_BIDI_DIR_LTR`  左到右
 *   - `LV_BIDI_DIR_RTL`  右到左
 *   - `LV_BIDI_DIR_AUTO` 检测文本基本方向
 */
#define LV_BIDI_BASE_DIR_DEF                    LV_BIDI_DIR_AUTO
#endif

// 启用阿拉伯语/波斯语处理。在这些语言中，字符应根据其在文本中的位置替换为其他形式
#define LV_USE_ARABIC_PERSIAN_CHARS             1

// 0: 更改内置(v)snprintf函数
#define LV_SPRINTF_CUSTOM                       1

#if LV_SPRINTF_CUSTOM
#define LV_SPRINTF_INCLUDE                      <stdio.h>
#define lv_snprintf                             snprintf
#define lv_vsnprintf                            vsnprintf
#else
#define LV_SPRINTF_DISABLE_FLOAT                1
#endif

#if LV_USE_USER_DATA
// 声明对象的用户数据的类型(例如可以是`void *`、`int`、`struct`)
typedef void *lv_obj_user_data_t;

// 提供释放用户数据的功能
#define LV_USE_USER_DATA_FREE                   0

#if LV_USE_USER_DATA_FREE
#define LV_USER_DATA_FREE_INCLUDE               "something.h"

// 函数原型：void user_data_free(lv_obj_t *obj);
#define LV_USER_DATA_FREE                       (user_data_free)
#endif
#endif

// 1：基于`lv_obj_align()`参数启用`lv_obj_realign()`
#define LV_USE_OBJ_REALIGN                      1

/*
 * 启用以使对象可在更大的区域上单击。
 *   - LV_EXT_CLICK_AREA_OFF或0  : 禁用此功能
 *   - LV_EXT_CLICK_AREA_TINY    : 额外区域可以水平和垂直调整(0..255 px)
 *   - LV_EXT_CLICK_AREA_FULL    : 额外的区域可以在所有4个方向上进行调整(-32k..+32k px)
 */
#define LV_USE_EXT_CLICK_AREA                   LV_EXT_CLICK_AREA_TINY

#define LV_USE_ARC                              1
#define LV_USE_BAR                              1
#define LV_USE_BTN                              1
#define LV_USE_BTNMATRIX                        1

#define LV_USE_CALENDAR                         1

#if LV_USE_CALENDAR
#define LV_CALENDAR_WEEK_STARTS_MONDAY          0
#endif

#define LV_USE_CANVAS                           1
#define LV_USE_CHECKBOX                         1

#define LV_USE_CHART                            1

#if LV_USE_CHART
#define LV_CHART_AXIS_TICK_LABEL_MAX_LEN        256
#endif

#define LV_USE_CONT                             1
#define LV_USE_CPICKER                          1

#define LV_USE_DROPDOWN                         1

#if (LV_USE_DROPDOWN != 0)
// 打开和关闭默认动画时间 [ms](0：无动画)
#define LV_DROPDOWN_DEF_ANIM_TIME               200
#endif

#define LV_USE_GAUGE                            1
#define LV_USE_IMG                              1

#define LV_USE_IMGBTN                           1

#if LV_USE_IMGBTN
// 1：imgbtn需要左、中、右三个部分，宽度可以自由设置
#define LV_IMGBTN_TILED                         0
#endif

#define LV_USE_KEYBOARD                         1

#define LV_USE_LABEL                            1

#if (LV_USE_LABEL != 0)
// 水平或垂直。'LV_LABEL_LONG_ROLL/ROLL_CIRC'模式下的滚动速度[px/sec]
#define LV_LABEL_DEF_SCROLL_SPEED               25

// 动画周期开始/结束时的等待期
#define LV_LABEL_WAIT_CHAR_COUNT                3

// 启用选择标签文本
#define LV_LABEL_TEXT_SEL                       1

// 将额外的一些信息存储在标签(12字节)中以加快绘制超长文本的速度
#define LV_LABEL_LONG_TXT_HINT                  1
#endif

#define LV_USE_LED                              1

#if LV_USE_LED
// 最低亮度
#define LV_LED_BRIGHT_MIN                       120

// 最大亮度
#define LV_LED_BRIGHT_MAX                       255
#endif

#define LV_USE_LINE                             1

#define LV_USE_LIST                             1

#if (LV_USE_LIST != 0)
// 聚焦到列表元素的默认动画时间 [ms](0：无动画)
#define LV_LIST_DEF_ANIM_TIME                   100
#endif

#define LV_USE_LINEMETER                        1

#if LV_USE_LINEMETER
// 以牺牲性能为代价更精确地画线。如果有很多行，任何次要都可见，则很有用。0: 没有额外的精度；1: 一些额外的精度；2: 最佳精度
#define LV_LINEMETER_PRECISE                    1
#endif

#define LV_USE_OBJMASK                          1

#define LV_USE_MSGBOX                           1

#define LV_USE_PAGE                             1

#if (LV_USE_PAGE != 0)
// 焦点默认动画时间 [ms](0：无动画)
#define LV_PAGE_DEF_ANIM_TIME                   400
#endif

#define LV_USE_SPINNER                          1

#if (LV_USE_SPINNER != 0)
#define LV_SPINNER_DEF_ARC_LENGTH               60
#define LV_SPINNER_DEF_SPIN_TIME                1000
#define LV_SPINNER_DEF_ANIM                     LV_SPINNER_TYPE_SPINNING_ARC
#endif

#define LV_USE_ROLLER                           1

#if (LV_USE_ROLLER != 0)
// 焦点动画时间 [ms](0：无动画)
#define LV_ROLLER_DEF_ANIM_TIME                 200

// 滚轮无限时的额外页面数
#define LV_ROLLER_INF_PAGES                     7
#endif

#define LV_USE_SLIDER                           1
#define LV_USE_SPINBOX                          1
#define LV_USE_SWITCH                           1

#define LV_USE_TEXTAREA                         1

#if (LV_USE_TEXTAREA != 0)
#define LV_TEXTAREA_DEF_CURSOR_BLINK_TIME       400
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME           1500
#endif

#define LV_USE_TABLE                            1

#if LV_USE_TABLE
#define LV_TABLE_COL_MAX                        12
#define LV_TABLE_CELL_STYLE_CNT                 4
#endif

#define LV_USE_TABVIEW                          1

#if (LV_USE_TABVIEW != 0)
// 幻灯片动画时间 [ms](0：无动画)
#define LV_TABVIEW_DEF_ANIM_TIME                300
#endif

#define LV_USE_TILEVIEW                         1

#if LV_USE_TILEVIEW
// 幻灯片动画时间 [ms](0：无动画)
#define LV_TILEVIEW_DEF_ANIM_TIME               300
#endif

#define LV_USE_WIN                              1

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

// 1：启用文件系统(可能需要图像)
#define LV_USE_FILESYSTEM                       1

#if LV_USE_FILESYSTEM
// 声明文件系统驱动程序的用户数据类型(例如可以是`void *`、`int`、`struct`)
typedef void *lv_fs_drv_user_data_t;
#endif

// 文件系统接口
#define LV_USE_FS_IF	                        1

#if LV_USE_FS_IF
#define LV_FS_IF_PC                             1
#endif

#endif
