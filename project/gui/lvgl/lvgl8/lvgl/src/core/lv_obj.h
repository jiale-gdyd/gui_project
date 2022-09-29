#ifndef LV_OBJ_H
#define LV_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

#include "../lv_conf_internal.h"

#include "../hal/lv_hal.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"
#include "../misc/lv_types.h"
#include "../misc/lv_color.h"
#include "../misc/lv_assert.h"

struct _lv_obj_t;

// 小部件的可能状态。OR-ed值是可以的
enum {
    LV_STATE_DEFAULT   =  0x0000,
    LV_STATE_CHECKED   =  0x0001,
    LV_STATE_FOCUSED   =  0x0002,
    LV_STATE_FOCUS_KEY =  0x0004,
    LV_STATE_EDITED    =  0x0008,
    LV_STATE_HOVERED   =  0x0010,
    LV_STATE_PRESSED   =  0x0020,
    LV_STATE_SCROLLED  =  0x0040,
    LV_STATE_DISABLED  =  0x0080,

    LV_STATE_USER_1    =  0x1000,
    LV_STATE_USER_2    =  0x2000,
    LV_STATE_USER_3    =  0x4000,
    LV_STATE_USER_4    =  0x8000,

    LV_STATE_ANY       = 0xFFFF,                // 可以在某些函数中使用特殊值来针对所有状态
};

typedef uint16_t lv_state_t;

// 小部件的可能部分。这些部件可以被视为小部件的内部构建块。例如，滑块 = 背景 + 指示器 + 旋钮, 注意每个部件都被每个小部件使用
enum {
    LV_PART_MAIN         = 0x000000,            // 像矩形的背景
    LV_PART_SCROLLBAR    = 0x010000,            // 滚动条
    LV_PART_INDICATOR    = 0x020000,            // 指标，例如, 用于滑块、条、开关或复选框的勾选框
    LV_PART_KNOB         = 0x030000,            // 像手柄一样抓取调整值
    LV_PART_SELECTED     = 0x040000,            // 指示当前选择的选项或部分
    LV_PART_ITEMS        = 0x050000,            // 如果小部件具有多个相似元素(即表格单元格)，则使用
    LV_PART_TICKS        = 0x060000,            // 刻度上的刻度，例如, 对于图表或仪表
    LV_PART_CURSOR       = 0x070000,            // 标记一个特定的地方，例如, 用于文本区域的光标或图表

    LV_PART_CUSTOM_FIRST = 0x080000,            // 自定义小部件的扩展点

    LV_PART_ANY          = 0x0F0000,            // 在某些函数中可以使用特殊值来针对所有部分
};

typedef uint32_t lv_part_t;

// 开/关功能控制对象的行为。OR-ed值是可以的
enum {
    LV_OBJ_FLAG_HIDDEN             = (1L << 0),     // 使对象隐藏。(就像它根本不存在一样)
    LV_OBJ_FLAG_CLICKABLE          = (1L << 1),     // 使对象可被输入设备点击
    LV_OBJ_FLAG_CLICK_FOCUSABLE    = (1L << 2),     // 单击时将焦点状态添加到对象
    LV_OBJ_FLAG_CHECKABLE          = (1L << 3),     // 单击对象时切换选中状态
    LV_OBJ_FLAG_SCROLLABLE         = (1L << 4),     // 使对象可滚动
    LV_OBJ_FLAG_SCROLL_ELASTIC     = (1L << 5),     // 允许在内部滚动但速度较慢
    LV_OBJ_FLAG_SCROLL_MOMENTUM    = (1L << 6),     // 抛出时使对象进一步滚动
    LV_OBJ_FLAG_SCROLL_ONE         = (1L << 7),     // 只允许滚动一个可捕捉的孩子
    LV_OBJ_FLAG_SCROLL_CHAIN_HOR   = (1L << 8),     // 允许将水平滚动传播到父级
    LV_OBJ_FLAG_SCROLL_CHAIN_VER   = (1L << 9),     // 允许将垂直滚动传播到父节点
    LV_OBJ_FLAG_SCROLL_CHAIN       = (LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER),
    LV_OBJ_FLAG_SCROLL_ON_FOCUS    = (1L << 10),    // 自动滚动对象，使其可见时聚焦
    LV_OBJ_FLAG_SCROLL_WITH_ARROW  = (1L << 11),    // 允许用方向键滚动聚焦的对象
    LV_OBJ_FLAG_SNAPPABLE          = (1L << 12),    // 如果在父对象上启用了scroll snap，它可以对这个对象进行snap
    LV_OBJ_FLAG_PRESS_LOCK         = (1L << 13),    // 保持按压的对象，即使压力从对象滑出
    LV_OBJ_FLAG_EVENT_BUBBLE       = (1L << 14),    // 也将事件传播到父节点
    LV_OBJ_FLAG_GESTURE_BUBBLE     = (1L << 15),    // 将手势传播给父对象
    LV_OBJ_FLAG_ADV_HITTEST        = (1L << 16),    // 允许进行更精确的点击测试。例如考虑圆角
    LV_OBJ_FLAG_IGNORE_LAYOUT      = (1L << 17),    // 通过布局*/使对象可定位
    LV_OBJ_FLAG_FLOATING           = (1L << 18),    // 不滚动对象时，父卷轴和忽略布局
    LV_OBJ_FLAG_OVERFLOW_VISIBLE   = (1L << 19),    // 不要剪辑孩子的内容到父母的边界

    LV_OBJ_FLAG_LAYOUT_1           = (1L << 23),    // 自定义标志，可供布局免费使用
    LV_OBJ_FLAG_LAYOUT_2           = (1L << 24),    // 自定义标志，可供布局免费使用

    LV_OBJ_FLAG_WIDGET_1           = (1L << 25),    // 自定义标志，小部件免费使用
    LV_OBJ_FLAG_WIDGET_2           = (1L << 26),    // 自定义标志，小部件免费使用

    LV_OBJ_FLAG_USER_1             = (1L << 27),    // 自定义标志，用户免费使用
    LV_OBJ_FLAG_USER_2             = (1L << 28),    // 自定义标志，用户免费使用
    LV_OBJ_FLAG_USER_3             = (1L << 29),    // 自定义标志，用户免费使用
    LV_OBJ_FLAG_USER_4             = (1L << 30),    // 自定义标志，用户免费使用
};

typedef uint32_t lv_obj_flag_t;

// 如果`class_p = lv_obj_class`，则为`lv_obj_draw_part_dsc_t`中的`type`字段。在`LV_EVENT_DRAW_PART_BEGIN`和`LV_EVENT_DRAW_PART_END`中使用
typedef enum {
    LV_OBJ_DRAW_PART_RECTANGLE,                 // 主矩形
    LV_OBJ_DRAW_PART_BORDER_POST,               // 如果style_border_post = true的边框
    LV_OBJ_DRAW_PART_SCROLLBAR,                 // 滚动条
} lv_obj_draw_part_type_t;

#include "lv_obj_pos.h"
#include "lv_obj_tree.h"
#include "lv_obj_draw.h"
#include "lv_obj_class.h"
#include "lv_obj_style.h"
#include "lv_obj_scroll.h"

#include "lv_event.h"
#include "lv_group.h"

// 使基对象的类公开可用
extern const lv_obj_class_t lv_obj_class;

// 特殊的，很少使用的属性。如果设置了任何元素，它们将自动分配
typedef struct {
    struct _lv_obj_t       **children;              // 将孩子的指针存储在数组中
    uint32_t               child_cnt;               // 孩子个数
    lv_group_t             *group_p;

    struct _lv_event_dsc_t *event_dsc;              // 动态分配的事件回调和用户数据数组
    lv_point_t             scroll;                  // 当前X/Y滚动偏移

    lv_coord_t             ext_click_pad;           // 所有方向的额外点击填充
    lv_coord_t             ext_draw_size;           // EXTend各个方向的尺寸进行绘制

    lv_scrollbar_mode_t    scrollbar_mode : 2;      // 如何显示滚动条
    lv_scroll_snap_t       scroll_snap_x : 2;       // 在哪里水平对齐可捕捉的孩子
    lv_scroll_snap_t       scroll_snap_y : 2;       // 在哪里垂直对齐可捕捉的孩子
    lv_dir_t               scroll_dir : 4;          // 允许的滚动方向
    uint8_t                event_dsc_cnt : 6;       // 存储在`event_dsc`数组中的事件回调数
    uint8_t                layer_type : 2;          // 在此处缓存图层类型。lv_intermediate_layer_type_t的元素
} _lv_obj_spec_attr_t;

typedef struct _lv_obj_t {
    const lv_obj_class_t *class_p;
    struct _lv_obj_t     *parent;
    _lv_obj_spec_attr_t  *spec_attr;
    _lv_obj_style_t      *styles;
#if LV_USE_USER_DATA
    void                 *user_data;
#endif
    lv_area_t            coords;
    lv_obj_flag_t        flags;
    lv_state_t           state;
    uint16_t             layout_inv : 1;
    uint16_t             scr_layout_inv : 1;
    uint16_t             skip_trans : 1;
    uint16_t             style_cnt : 6;
    uint16_t             h_layout : 1;
    uint16_t             w_layout : 1;
} lv_obj_t;

/**
 * 函数名称: lv_init
 * 功能描述: 初始化LVGL库。应该在任何其他LVGL相关函数之前调用
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_init(void);

#if LV_ENABLE_GC || LV_USE_BUILTIN_MALLOC
/**
 * 函数名称: lv_deinit
 * 功能描述: 取消LVGL库。当前仅在不使用自定义分配器或启用GC时实现
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_deinit(void);
#endif

/**
 * 函数名称: lv_is_initialized
 * 功能描述: 获取'lv'库当前是否已初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 已经初始化返回true，否则返回false
 */
bool lv_is_initialized(void);

/**
 * 函数名称: lv_obj_create
 * 功能描述: 创建基础对象(矩形)
 * 输入参数: parent --> 指向父对象的指针。如果为NULL，则将创建一个屏幕
 * 输出参数: 无
 * 返回说明: 指向新对象的指针
 */
lv_obj_t *lv_obj_create(lv_obj_t *parent);

/**
 * 函数名称: lv_obj_add_flag
 * 功能描述: 设置一个或多个标志
 * 输入参数: obj --> 指向对象的指针
 *           f   --> 来自`lv_obj_flag_t`的R-ed值要设置
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_obj_add_flag(lv_obj_t *obj, lv_obj_flag_t f);

/**
 * 函数名称: lv_obj_clear_flag
 * 功能描述: 清除一个或多个标志
 * 输入参数: obj --> 指向对象的指针
 *           f   --> 来自`lv_obj_flag_t`的OR-ed值进行设置
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_obj_clear_flag(lv_obj_t *obj, lv_obj_flag_t f);

/**
 * 函数名称: lv_obj_add_state
 * 功能描述: 向对象添加一种或多种状态。其他状态位将保持不变。如果在样式中指定，过渡动画将从前一个状态开始到当前状态
 * 输入参数: obj   --> 指向对象的指针
 *           state --> 要添加的状态。例如`LV_STATE_PRESSED | LV_STATE_FOCUSED`
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_obj_add_state(lv_obj_t *obj, lv_state_t state);

/**
 * 函数名称: lv_obj_clear_state
 * 功能描述: 删除对象的一种或多种状态。其他状态位将保持不变。如果在样式中指定，过渡动画将从前一个状态开始到当前状态
 * 输入参数: obj   --> 指向对象的指针
 *           state --> 要移除的状态。例如`LV_STATE_PRESSED | LV_STATE_FOCUSED`
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_obj_clear_state(lv_obj_t *obj, lv_state_t state);

#if LV_USE_USER_DATA
/**
 * 函数名称: lv_obj_set_user_data
 * 功能描述: 设置对象的user_data字段
 * 输入参数: obj       --> 指向对象的指针
 *           user_data --> 指向新user_data的指针
 * 输出参数: 无
 * 返回说明: 无
 */
static inline void lv_obj_set_user_data(lv_obj_t *obj, void *user_data)
{
    obj->user_data = user_data;
}
#endif

/**
 * 函数名称: lv_obj_has_flag
 * 功能描述: 检查是否在对象上设置了给定标志或所有给定标志
 * 输入参数: obj --> 指向对象的指针
 *           f   --> 要检查的标志(可以使用OR值)
 * 输出参数: 无
 * 返回说明: true: 设置所有标志；false: 并非所有标志都已设置
 */
bool lv_obj_has_flag(const lv_obj_t *obj, lv_obj_flag_t f);

/**
 * 函数名称: lv_obj_has_flag_any
 * 功能描述: 检查是否在对象上设置了给定标志或任何标志
 * 输入参数: obj --> 指向对象的指针
 *           f   --> 要检查的标志(可以使用OR值)
 * 输出参数: 无
 * 返回说明: true: 至少设置了一个标志标志；false: 未设置任何标志
 */
bool lv_obj_has_flag_any(const lv_obj_t *obj, lv_obj_flag_t f);

/**
 * 函数名称: lv_obj_get_state
 * 功能描述: 获取对象的状态
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 状态(来自`lv_state_t`的OR-ed值)
 */
lv_state_t lv_obj_get_state(const lv_obj_t *obj);

/**
 * 函数名称: lv_obj_has_state
 * 功能描述: 检查对象是否处于给定状态
 * 输入参数: obj   --> 指向对象的指针
 *           state --> 要检查的状态或状态组合
 * 输出参数: 无
 * 返回说明: true: `obj`处于`state`状态；false: `obj`不在`state`
 */
bool lv_obj_has_state(const lv_obj_t *obj, lv_state_t state);

/**
 * 函数名称: lv_obj_get_group
 * 功能描述: 获取对象的组
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 指向对象组的指针
 */
lv_group_t *lv_obj_get_group(const lv_obj_t *obj);

#if LV_USE_USER_DATA
/**
 * 函数名称: lv_obj_get_user_data
 * 功能描述: 获取对象的user_data字段
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 指向对象的user_data的指针
 */
static inline void *lv_obj_get_user_data(lv_obj_t * obj)
{
    return obj->user_data;
}
#endif

/**
 * 函数名称: lv_obj_allocate_spec_attr
 * 功能描述: 如果尚未分配，则为对象分配特殊数据
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_obj_allocate_spec_attr(lv_obj_t *obj);

/**
 * 函数名称: lv_obj_check_type
 * 功能描述: 检查obj的类型
 * 输入参数: obj     --> 指向对象的指针
 *           class_p --> 要检查的类(例如`lv_slider_class`)
 * 输出参数: 无
 * 返回说明: true: `class_p`是`obj`类
 */
bool lv_obj_check_type(const lv_obj_t *obj, const lv_obj_class_t *class_p);

/**
 * 函数名称: lv_obj_has_class
 * 功能描述: 检查任何对象是否具有给定的类(类型)。它也检查祖先类
 * 输入参数: obj     --> 指向对象的指针
 *           class_p --> 要检查的类(例如`lv_slider_class`)
 * 输出参数: 无
 * 返回说明: true: `obj`具有给定的类
 */
bool lv_obj_has_class(const lv_obj_t *obj, const lv_obj_class_t *class_p);

/**
 * 函数名称: lv_obj_get_class
 * 功能描述: 获取对象的类(类型)
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 对象的类(类型)
 */
const lv_obj_class_t *lv_obj_get_class(const lv_obj_t *obj);

/**
 * 函数名称: lv_obj_is_valid
 * 功能描述: 检查是否有任何对象仍然'alive'
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: true: 有效
 */
bool lv_obj_is_valid(const lv_obj_t *obj);

/**
 * 函数名称: lv_obj_dpx
 * 功能描述: 考虑到`obj`显示器的DPI，相对于160 DPI显示器缩放给定数量的像素(距离或大小)。它确保例如 无论显示器的DPI如何，`lv_dpx(100)`都将具有相同的物理尺寸
 * 输入参数: obj --> 应考虑其显示dpi的对象
 *           n   --> 要缩放的像素数
 * 输出参数: 无
 * 返回说明: n x current_dpi / 160
 */
static inline lv_coord_t lv_obj_dpx(const lv_obj_t *obj, lv_coord_t n)
{
    return _LV_DPX_CALC(lv_disp_get_dpi(lv_obj_get_disp(obj)), n);
}

#if LV_USE_ASSERT_OBJ
#define LV_ASSERT_OBJ(obj_p, obj_class)                                                                 \
    do {                                                                                                \
        LV_ASSERT_MSG(obj_p != NULL, "The object is NULL");                                             \
        LV_ASSERT_MSG(lv_obj_has_class(obj_p, obj_class) == true, "Incompatible object type.");         \
        LV_ASSERT_MSG(lv_obj_is_valid(obj_p)  == true, "The object is invalid, deleted or corrupted?"); \
    } while (0)
#else
#  define LV_ASSERT_OBJ(obj_p, obj_class)   do {} while (0)
#endif

#if LV_USE_LOG && LV_LOG_TRACE_OBJ_CREATE
#define LV_TRACE_OBJ_CREATE(...)            LV_LOG_TRACE(__VA_ARGS__)
#else
#define LV_TRACE_OBJ_CREATE(...)
#endif

#ifdef __cplusplus
}
#endif

#endif
