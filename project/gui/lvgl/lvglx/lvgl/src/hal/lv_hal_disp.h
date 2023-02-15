#ifndef LV_HAL_DISP_H
#define LV_HAL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "lv_hal.h"
#include "../misc/lv_ll.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_area.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_color.h"
#include "../misc/lv_types.h"

#if LV_USE_ATOMICS == 1
#include <stdatomic.h>
#endif

#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE                 (32)
#endif

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

// 默认显示刷新、输入设备读取和动画步进周期
#define LV_DEF_REFR_PERIOD              33

struct _lv_obj_t;
struct _lv_disp_t;
struct _lv_theme_t;
struct _lv_disp_drv_t;

#if LV_USE_ATOMICS == 1
#define FLUSHING_TYPE                   atomic_int
#else
#define FLUSHING_TYPE                   volatile int
#endif

// 用于保存显示缓冲区信息的结构
typedef struct _lv_disp_draw_buf_t {
    void              *buf1;                // 第一个显示缓冲区
    void              *buf2;                // 第二个显示缓冲区

    void              *buf_act;
    uint32_t          size;                 // 像素数

    FLUSHING_TYPE     flushing;             // 1: 正在冲洗。(它不能是位域，因为当它从IRQ中清除时可能会发生读-修改-写问题)
    FLUSHING_TYPE     flushing_last;        // 1：这是最后一个要刷新的块。(它不能是位域，因为当它从IRQ中清除时可能会发生读-修改-写问题)
    uint32_t          last_area : 1;        // 1: 正在渲染最后一个区域
    uint32_t          last_part : 1;        // 1: 当前区域的最后一部分正在渲染
} lv_disp_draw_buf_t;
#undef FLUSHING_TYPE

typedef enum {
    LV_DISP_ROT_NONE = 0,
    LV_DISP_ROT_90,
    LV_DISP_ROT_180,
    LV_DISP_ROT_270
} lv_disp_rot_t;

// 由HAL注册的显示驱动程序结构。只有它的指针会保存在`lv_disp_t`中，所以它应该声明为`static lv_disp_drv_t my_drv`或动态分配
typedef struct _lv_disp_drv_t {
    lv_coord_t         hor_res;                 // 水平分辨率
    lv_coord_t         ver_res;                 // 垂直分辨率

    lv_coord_t         physical_hor_res;        // 完整/物理显示的水平分辨率。全屏模式设置为-1
    lv_coord_t         physical_ver_res;        // 完整/物理显示的垂直分辨率。全屏模式设置为-1
    lv_coord_t         offset_x;                // 与完整/物理显示的水平偏移。全屏模式设置为0
    lv_coord_t         offset_y;                // 与完整/物理显示的垂直偏移。全屏模式设置为0

    lv_disp_draw_buf_t *draw_buf;               // 指向用`lv_disp_draw_buf_init()`初始化的缓冲区的指针。LVGL将使用这个缓冲区来绘制屏幕内容

    uint32_t           direct_mode : 1;         // 1: 使用屏幕大小的缓冲区并绘制到绝对坐标
    uint32_t           full_refresh : 1;        // 1: 总是让整个屏幕重绘
    uint32_t           sw_rotate : 1;           // 1: 使用软件轮换(较慢)
    uint32_t           antialiasing : 1;        // 1: 在此显示器上启用了抗锯齿
    uint32_t           rotated : 2;             // 1: 将显示屏旋转90度。不会更新坐标
    uint32_t           screen_transp : 1;       // 如果屏幕没有纯色(opa == LV_OPA_COVER)背景，则处理。仅在需要时使用，因为它较慢

    uint32_t           dpi : 10;                // 显示器的DPI(每英寸点数)。默认值为`LV_DPI_DEF`

    lv_color_format_t  color_format;

    // 强制的: 将内部缓冲区(draw_buf)写入显示器。完成后必须调用`lv_disp_flush_ready()`
    void (*flush_cb)(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

    // 可选的: 扩展无效区域以符合显示驱动程序要求, 在单色显示器上将`y`舍入到, 8, 16 ..
    void (*rounder_cb)(struct _lv_disp_drv_t *disp_drv, lv_area_t *area);

    void (*clear_cb)(struct _lv_disp_drv_t *disp_drv, uint8_t *buf, uint32_t size);

    // 可选的: 每个刷新周期后调用，告诉渲染和刷新时间 + 刷新像素数
    void (*monitor_cb)(struct _lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px);

    // 可选的: 在lvgl等待操作完成时定期调用。例如冲洗或GPU。用户可以在这里执行非常简单的任务或产生任务
    void (*wait_cb)(struct _lv_disp_drv_t *disp_drv);

    // 可选的: 当lvgl需要清理影响渲染的任何CPU缓存时调用
    void (*clean_dcache_cb)(struct _lv_disp_drv_t *disp_drv);

    // 可选的: 更新驱动程序参数时调用
    void (*drv_update_cb)(struct _lv_disp_drv_t *disp_drv);

    // 可选的: 开始渲染时调用
    void (*render_start_cb)(struct _lv_disp_drv_t *disp_drv);

    lv_color_t color_chroma_key;    // 在CHROMA_KEYED图像上，此颜色将是透明的。`LV_COLOR_CHROMA_KEY`默认情况下。(lv_conf.h)

    lv_draw_ctx_t *draw_ctx;
    void (*draw_ctx_init)(struct _lv_disp_drv_t *disp_drv, lv_draw_ctx_t *draw_ctx);
    void (*draw_ctx_deinit)(struct _lv_disp_drv_t *disp_drv, lv_draw_ctx_t *draw_ctx);
    size_t draw_ctx_size;

#if LV_USE_USER_DATA
    void       *user_data;          // 自定义显示驱动程序用户数据
#endif
} lv_disp_drv_t;

// 显示结构。`lv_disp_drv_t`应该是结构的第一个成员
typedef struct _lv_disp_t {
    struct _lv_disp_drv_t *driver;                  // 驱动程序到显示器
    lv_timer_t            *refr_timer;              // 定期检查脏区并刷新它们的计时器
    struct _lv_theme_t    *theme;                   // 分配给屏幕的主题

    // 显示器的屏幕
    struct _lv_obj_t      **screens;                // 屏幕对象数组
    struct _lv_obj_t      *act_scr;                 // 此显示器上的当前活动屏幕
    struct _lv_obj_t      *prev_scr;                // 上一个画面。在屏幕动画期间使用
    struct _lv_obj_t      *scr_to_load;             // lv_scr_load_anim中准备加载的画面
    struct _lv_obj_t      *top_layer;               // 参阅lv_disp_get_layer_top
    struct _lv_obj_t      *sys_layer;               // 参阅lv_disp_get_layer_sys
    uint32_t              screen_cnt;               // 显示器屏幕数
    uint8_t               draw_prev_over_act : 1;   // 是否绘制上一个活动屏幕的后面
    uint8_t               del_prev : 1;             // 1: 当屏幕加载动画准备好时自动删除上一个屏幕
    uint8_t               rendering_in_progress : 1;// 1：当前屏幕渲染正在进行中

    lv_opa_t              bg_opa;                   // 背景颜色或墙纸的不透明度
    lv_color_t            bg_color;                 // 屏幕透明时的默认显示颜色
    const void            *bg_img;                  // 显示为墙纸的图像源

    // 无效(标记为重绘)区域
    lv_area_t             inv_areas[LV_INV_BUF_SIZE];
    uint8_t               inv_area_joined[LV_INV_BUF_SIZE];
    uint16_t              inv_p;
    int32_t               inv_en_cnt;

    uint32_t              last_activity_time;       // 上次在此显示器上进行活动的时间
    uint32_t              last_render_start_time;   // 此显示器上的上次渲染开始时间(毫秒)
} lv_disp_t;

/**
 * 函数名称: lv_disp_drv_init
 * 功能描述: 使用默认值初始化显示驱动程序。它用于在字段中具有已知值而不是内存中的垃圾。之后，您可以安全地只设置您需要的字段
 * 输入参数: 无
 * 输出参数: driver --> 指向要初始化的驱动程序变量的指针
 * 返回说明: 无
 */
void lv_disp_drv_init(lv_disp_drv_t *driver);

/**
 * 函数名称: lv_disp_draw_buf_init
 * 功能描述: 初始化显示缓冲区
 * 输入参数: buf1           --> LVGL 用于绘制图像的缓冲区。始终必须指定且不能为NULL。可以是用户分配的数组。例如`static lv_color_t disp_buf1[1024 * 10]`或者一个内存地址，例如外部SRAM
 *           buf2           --> (可选)指定第二个缓冲区以使图像渲染和图像刷新(发送到显示器)并行。在`disp_drv->flush`中，您应该使用DMA或类似硬件将图像发送到后台的显示器。它允许LVGL在发送前一帧时将下一帧渲染到另一个缓冲区中。如果未使用，则设置为NULL
 *           size_in_px_cnt --> `buf1`和`buf2`的像素数大小
 * 输出参数: draw_buf       --> 要初始化的指针`lv_disp_draw_buf_t`变量
 * 返回说明: 无
 */
void lv_disp_draw_buf_init(lv_disp_draw_buf_t *draw_buf, void *buf1, void *buf2, uint32_t size_in_px_cnt);

/**
 * 函数名称: lv_disp_drv_register
 * 功能描述: 注册一个初始化的显示驱动程序。自动将第一个显示设置为活动状态
 * 输入参数: driver --> 指向初始化的`lv_disp_drv_t`变量的指针。只保存它的指针
 * 输出参数: 无
 * 返回说明: 指向新显示的指针或出现错误时为NULL
 */
lv_disp_t *lv_disp_drv_register(lv_disp_drv_t *driver);

/**
 * 函数名称: lv_disp_drv_update
 * 功能描述: 在运行时更新驱动程序
 * 输入参数: disp    --> 指向显示器的指针。(`lv_disp_drv_register`的返回值)
 *           new_drv --> 指向新驱动程序的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_drv_update(lv_disp_t *disp, lv_disp_drv_t *new_drv);

/**
 * 函数名称: lv_disp_remove
 * 功能描述: 移除显示器
 * 输入参数: disp --> 指向显示器的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_remove(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_set_default
 * 功能描述: 设置默认显示。默认情况下，将在其上创建新屏幕
 * 输入参数: disp --> 指向显示器的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_set_default(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_default
 * 功能描述: 获取默认显示
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 指向默认显示的指针
 */
lv_disp_t *lv_disp_get_default(void);

/**
 * 函数名称: lv_disp_get_hor_res
 * 功能描述: 获取显示器的水平分辨率
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: 显示器的水平分辨率
 */
lv_coord_t lv_disp_get_hor_res(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_ver_res
 * 功能描述: 获取显示器的垂直分辨率
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: 显示器的垂直分辨率
 */
lv_coord_t lv_disp_get_ver_res(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_offset_x
 * 功能描述: 获取完整/物理显示的水平偏移量
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: 与完整/物理显示的水平偏移
 */
lv_coord_t lv_disp_get_offset_x(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_offset_y
 * 功能描述: 完整/物理显示获取垂直偏移
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: 与完整/物理显示的水平偏移
 */
lv_coord_t lv_disp_get_offset_y(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_physical_hor_res
 * 功能描述: 获取显示器的完整/物理水平分辨率
 * 输入参数: disp --> 指向显示的指针(NULL使用默认显示)
 * 输出参数: 无
 * 返回说明: 显示器的完整/物理水平分辨率
 */
lv_coord_t lv_disp_get_physical_hor_res(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_physical_ver_res
 * 功能描述: 获取显示器的完整/物理垂直分辨率
 * 输入参数: disp --> 指向显示的指针(NULL使用默认显示)
 * 输出参数: 无
 * 返回说明: 显示器的完整/物理垂直分辨率
 */
lv_coord_t lv_disp_get_physical_ver_res(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_antialiasing
 * 功能描述: 获取是否为显示器启用抗锯齿
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: true: 启用抗锯齿；false: 禁用
 */
bool lv_disp_get_antialiasing(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_dpi
 * 功能描述: 获取显示器的 DPI
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: 显示器的dpi
 */
lv_coord_t lv_disp_get_dpi(const lv_disp_t *disp);

/**
 * 函数名称: lv_disp_set_rotation
 * 功能描述: 设置此显示器的旋转
 * 输入参数: disp     --> 指向显示的指针(NULL则使用默认显示)
 *           rotation --> 旋转角度
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_set_rotation(lv_disp_t *disp, lv_disp_rot_t rotation);

/**
 * 函数名称: lv_disp_get_rotation
 * 功能描述: 获取此显示器的当前旋转角度
 * 输入参数: disp --> 指向显示的指针(NULL则使用默认显示)
 * 输出参数: 无
 * 返回说明: 显示器的当前旋转角度
 */
lv_disp_rot_t lv_disp_get_rotation(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_flush_ready
 * 功能描述: 刷新完成后调用显示驱动的`flush_cb`函数
 * 输入参数: disp_drv --> 指向调用此函数的`flush_cb`中的显示驱动程序的指针
 * 输出参数: 无
 * 返回说明: 无
 */
LV_ATTRIBUTE_FLUSH_READY void lv_disp_flush_ready(lv_disp_drv_t *disp_drv);

/**
 * 函数名称: lv_disp_flush_is_last
 * 功能描述: 判断它是否是刷新过程的最后一个区域。可以从`flush_cb`调用以在所有区域刷新时根据需要执行一些特殊的显示刷新
 * 输入参数: disp_drv --> 指向显示驱动程序的指针
 * 输出参数: 无
 * 返回说明: true: 它是最后一个要冲洗的区域；false: 还有其他区域会很快更新
 */
LV_ATTRIBUTE_FLUSH_READY bool lv_disp_flush_is_last(lv_disp_drv_t *disp_drv);

/**
 * 函数名称: lv_disp_get_next
 * 功能描述: 获取下一个显示
 * 输入参数: disp --> 指向当前显示的指针。NULL则进行初始化
 * 输出参数: 无
 * 返回说明: 下一个显示或NULL如果没有更多。参数为NULL时给出第一个显示
 */
lv_disp_t *lv_disp_get_next(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_draw_buf
 * 功能描述: 获取显示器的内部缓冲区
 * 输入参数: disp --> 指向显示器的指针
 * 输出参数: 无
 * 返回说明: 指向内部缓冲区的指针
 */
lv_disp_draw_buf_t *lv_disp_get_draw_buf(lv_disp_t *disp);

#ifdef __cplusplus
}
#endif

#endif
