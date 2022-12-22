#ifndef LV_EVENT_H
#define LV_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../misc/lv_area.h"
#include "lv_obj_draw.h"
#include "lv_obj_class.h"
#include "../hal/lv_hal_indev.h"

struct _lv_obj_t;
struct _lv_event_dsc_t;

// 发送到对象的事件类型
typedef enum {
    LV_EVENT_ALL = 0,

    // 输入设备事件
    LV_EVENT_PRESSED,                   // 对象已被按下
    LV_EVENT_PRESSING,                  // 对象被按下(按下时连续调用)
    LV_EVENT_PRESS_LOST,                // 对象仍被按下但已将光标/手指滑离对象
    LV_EVENT_SHORT_CLICKED,             // 物体被按压了一小段时间，然后松开。滚动时不调用
    LV_EVENT_LONG_PRESSED,              // 对象已被按下至少`long_press_time`。滚动时不调用
    LV_EVENT_LONG_PRESSED_REPEAT,       // 在每个`long_press_repeat_time`毫秒的`long_press_time`之后调用。滚动时不调用
    LV_EVENT_CLICKED,                   // 如果不滚动则在发布时调用(无论是否长按)
    LV_EVENT_RELEASED,                  // 当对象被释放时，在每种情况下调用
    LV_EVENT_SCROLL_BEGIN,              // 滚动开始。.event参数是一个指向滚动动画的指针。可以修改
    LV_EVENT_SCROLL_THROW_BEGIN,        // 滚动开始
    LV_EVENT_SCROLL_END,                // 滚动结束
    LV_EVENT_SCROLL,                    // 正在滚动
    LV_EVENT_GESTURE,                   // 检测到手势。使用`lv_indev_get_gesture_dir(lv_indev_get_act());`获取手势
    LV_EVENT_KEY,                       // 按键被发送到对象。使用`lv_indev_get_key(lv_indev_get_act());`获取按键
    LV_EVENT_FOCUSED,                   // 对象是重点
    LV_EVENT_DEFOCUSED,                 // 物体散焦
    LV_EVENT_LEAVE,                     // 对象散焦但仍被选中
    LV_EVENT_HIT_TEST,                  // 执行高级命中测试

    // 绘图事件
    LV_EVENT_COVER_CHECK,               // 检查物体是否完全覆盖了一个区域。事件参数是`lv_cover_check_info_t *`
    LV_EVENT_REFR_EXT_DRAW_SIZE,        // 获取对象周围所需的额外绘制区域(例如用于阴影)。事件参数是`lv_coord_t *`来存储大小
    LV_EVENT_DRAW_MAIN_BEGIN,           // 开始主要绘图阶段
    LV_EVENT_DRAW_MAIN,                 // 执行主绘图
    LV_EVENT_DRAW_MAIN_END,             // 完成主绘图阶段
    LV_EVENT_DRAW_POST_BEGIN,           // 开始绘制后阶段(当所有孩子都被绘制时)
    LV_EVENT_DRAW_POST,                 // 执行后绘制阶段(当所有孩子都被绘制时)
    LV_EVENT_DRAW_POST_END,             // 完成后抽签阶段(当所有孩子都被抽中时)
    LV_EVENT_DRAW_PART_BEGIN,           // 开始画一部分。事件参数是`lv_obj_draw_dsc_t *`
    LV_EVENT_DRAW_PART_END,             // 完成绘制零件。事件参数是`lv_obj_draw_dsc_t *`

    // 特殊事件
    LV_EVENT_VALUE_CHANGED,             // 对象的值已更改(即滑块移动)
    LV_EVENT_INSERT,                    // 文本被插入到对象中。事件数据是`char *`被插入
    LV_EVENT_REFRESH,                   // 通知对象刷新其上的某些内容(对于用户)
    LV_EVENT_READY,                     // 一个过程已经完成
    LV_EVENT_CANCEL,                    // 一个进程被取消

    // 其他事件
    LV_EVENT_DELETE,                    // 正在删除对象
    LV_EVENT_CHILD_CHANGED,             // 子项已被移除、添加或更改其大小、位置
    LV_EVENT_CHILD_CREATED,             // 孩子被创造了，总是冒泡给所有父母
    LV_EVENT_CHILD_DELETED,             // 孩子被删除了，总是冒泡给所有的父母
    LV_EVENT_SCREEN_UNLOAD_START,       // 屏幕卸载开始，在调用scr_load时立即触发
    LV_EVENT_SCREEN_LOAD_START,         // 屏幕加载开始，当屏幕更改延迟到期时触发
    LV_EVENT_SCREEN_LOADED,             // 加载了一个屏幕
    LV_EVENT_SCREEN_UNLOADED,           // 一个屏幕被卸载了

    LV_EVENT_SIZE_CHANGED,              // 对象坐标/大小已更改
    LV_EVENT_STYLE_CHANGED,             // 对象的样式已更改
    LV_EVENT_LAYOUT_CHANGED,            // 由于重新计算布局，子项位置已更改
    LV_EVENT_GET_SELF_SIZE,             // 获取小部件的内部大小

#if LV_USE_MSG
    LV_EVENT_MSG_RECEIVED,              // 可选LVGL组件的事件
#endif

    _LV_EVENT_LAST,                     // 默认事件数

    LV_EVENT_PREPROCESS = 0x80,         // 这是一个可以用事件设置的标志，以便在类默认事件处理之前处理它
} lv_event_code_t;

typedef struct _lv_event_t {
    struct _lv_obj_t   *target;
    struct _lv_obj_t   *current_target;
    lv_event_code_t    code;
    void               *user_data;
    void               *param;
    struct _lv_event_t *prev;
    uint8_t            deleted : 1;
    uint8_t            stop_processing : 1;
    uint8_t            stop_bubbling : 1;
} lv_event_t;

// 事件回调。事件用于通知用户对对象采取的某些操作。有关详细信息，请参阅::lv_event_t
typedef void (*lv_event_cb_t)(lv_event_t *e);

// 用作::LV_EVENT_HIT_TEST的事件参数来检查`point`是否可以点击对象
typedef struct {
    const lv_point_t *point;            // 一个相对于屏幕的点来检查它是否可以点击对象
    bool             res;               // true: `point`可以点击对象；false: 它不能
} lv_hit_test_info_t;

// 用作::LV_EVENT_COVER_CHECK 的事件参数来检查一个区域是否被对象覆盖。在事件中使用`const lv_area_t *area = lv_event_get_cover_area(e)`获取要检查的区域，并使用`lv_event_set_cover_res(e, res)`设置结果
typedef struct {
    lv_cover_res_t  res;
    const lv_area_t *area;
} lv_cover_check_info_t;

/**
 * 函数名称: lv_event_send
 * 功能描述: 向对象发送事件
 * 输入参数: obj        --> 指向对象的指针
 *           event_code --> 来自`lv_event_t`的事件类型
 *           param      --> 取决于小部件类型和事件的任意数据。(通常为`NULL`)
 * 输出参数: 无
 * 返回说明: LV_RES_OK: 事件中没有删除`obj`；LV_RES_INV: 在event_code中删除了`obj`
 */
lv_res_t lv_event_send(struct _lv_obj_t *obj, lv_event_code_t event_code, void *param);

/**
 * 函数名称: lv_obj_event_base
 * 功能描述: 由小部件内部使用以调用祖先小部件类型的事件处理程序
 * 输入参数: class_p --> 指向小部件类的指针(不是祖先类)
 *           e       --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: LV_RES_OK: 事件中没有删除目标对象；LV_RES_INV: 在event_code中被删除
 */
lv_res_t lv_obj_event_base(const lv_obj_class_t *class_p, lv_event_t *e);

/**
 * 函数名称: lv_event_get_target
 * 功能描述: 获取事件最初针对的对象。即使事件冒泡也是一样的
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: event_code的目标
 */
struct _lv_obj_t *lv_event_get_target(lv_event_t *e);

/**
 * 函数名称: lv_event_get_current_target
 * 功能描述: 获取事件的当前目标。它是被调用事件处理程序的对象。如果事件没有冒泡，则它与正常目标相同
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: 指向event_code当前目标的指针
 */
struct _lv_obj_t *lv_event_get_current_target(lv_event_t *e);

/**
 * 函数名称: lv_event_get_code
 * 功能描述: 获取事件的事件代码
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: 事件代码。(例如`LV_EVENT_CLICKED`、`LV_EVENT_FOCUSED`等)
 */
lv_event_code_t lv_event_get_code(lv_event_t *e);

/**
 * 函数名称: lv_event_get_param
 * 功能描述: 获取发送事件时传递的参数
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: 指向参数的指针
 */
void *lv_event_get_param(lv_event_t *e);

/**
 * 函数名称: lv_event_get_user_data
 * 功能描述: 获取在对象上注册事件时传递的 user_data
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: 指向user_data的指针
 */
void *lv_event_get_user_data(lv_event_t *e);

/**
 * 函数名称: lv_event_stop_bubbling
 * 功能描述: 阻止事件冒泡。这只在事件处理链中间调用时有效
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_event_stop_bubbling(lv_event_t *e);

/**
 * 函数名称: lv_event_stop_processing
 * 功能描述: 停止处理此事件。这只在事件处理链中间调用时有效。
 * 输入参数: e --> 指向事件描述符的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_event_stop_processing(lv_event_t *e);

/**
 * 函数名称: lv_event_register_id
 * 功能描述: 注册一个新的自定义事件ID。它可以以与例如相同的方式使用`LV_EVENT_CLICKED`发送自定义事件
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 新的事件ID
 */
uint32_t lv_event_register_id(void);

/**
 * 函数名称: _lv_event_mark_deleted
 * 功能描述: 可以调用嵌套事件，其中一个可能属于正在删除的对象。将此对象的`event_temp_data`标记为已删除以知道它的`lv_event_send`应该返回`LV_RES_INV`
 * 输入参数: obj --> 指向要标记为已删除的对象的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void _lv_event_mark_deleted(struct _lv_obj_t *obj);

/**
 * 函数名称: lv_obj_add_event_cb
 * 功能描述: 为对象添加事件处理函数。用户使用它对对象发生的事件做出反应。一个对象可以有多个事件处理程序。它们的调用顺序与添加顺序相同
 * 输入参数: obj       --> 指向对象的指针
 *           event_cb  --> 新的事件函数
 *           filter    --> 和事件代码(例如`LV_EVENT_CLICKED`)应该在其上调用事件。`LV_EVENT_ALL`可以被起诉接收所有事件
 *           user_data --> 自定义数据数据将在`event_cb`中可用
 * 输出参数: 无
 * 返回说明: 事件描述符的指针。可以在::lv_obj_remove_event_dsc中使用
 */
struct _lv_event_dsc_t *lv_obj_add_event_cb(struct _lv_obj_t *obj, lv_event_cb_t event_cb, lv_event_code_t filter, void *user_data);

/**
 * 函数名称: lv_obj_remove_event_cb
 * 功能描述: 删除对象的事件处理函数
 * 输入参数: obj      --> 指向对象的指针
 *           event_cb --> 要删除的事件函数, 或`NULL`删除第一个添加的事件回调
 * 输出参数: 无
 * 返回说明: 如果删除了任何事件处理程序，则为true
 */
bool lv_obj_remove_event_cb(struct _lv_obj_t *obj, lv_event_cb_t event_cb);

/**
 * 函数名称: lv_obj_remove_event_cb_with_user_data
 * 功能描述: 从对象中删除具有特定user_data的事件处理函数
 * 输入参数: obj             --> 指向对象的指针
 *           event_cb        --> 要删除的事件函数，或者只有`user_data`的`NULL`很重要
 *           event_user_data --> ::lv_obj_add_event_cb中指定的user_data
 * 输出参数: 无
 * 返回说明: 如果删除了任何事件处理程序，则为true
 */
bool lv_obj_remove_event_cb_with_user_data(struct _lv_obj_t *obj, lv_event_cb_t event_cb, const void *event_user_data);

/**
 * 函数名称: lv_obj_remove_event_dsc
 * 功能描述: 删除对象的事件处理函数
 * 输入参数: obj       --> 指向对象的指针
 *           event_dsc --> 指向要移除的事件描述符的指针(由::lv_obj_add_event_cb返回)
 * 输出参数: 无
 * 返回说明: 如果删除了任何事件处理程序，则为true
 */
bool lv_obj_remove_event_dsc(struct _lv_obj_t *obj, struct _lv_event_dsc_t *event_dsc);

void *lv_obj_get_event_user_data(struct _lv_obj_t *obj, lv_event_cb_t event_cb);

/**
 * 函数名称: lv_event_get_indev
 * 功能描述: 获取作为参数传递给indev相关事件的输入设备
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 触发事件的indev或NULL如果在与indev无关的事件上调用
 */
lv_indev_t *lv_event_get_indev(lv_event_t *e);

/**
 * 函数名称: lv_event_get_draw_part_dsc
 * 功能描述: 获取作为参数传递给`LV_EVENT_DRAW_PART_BEGIN/END`的零件绘制描述符
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 如果在不相关的事件上调用，则用于挂钩绘图的部件绘图描述符或NULL
 */
lv_obj_draw_part_dsc_t *lv_event_get_draw_part_dsc(lv_event_t *e);

/**
 * 函数名称: lv_event_get_draw_ctx
 * 功能描述: 获取绘制上下文，它应该是绘制函数的第一个参数
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 如果在不相关的事件上调用，则指向绘制上下文或NULL
 */
lv_draw_ctx_t *lv_event_get_draw_ctx(lv_event_t * e);

/**
 * 函数名称: lv_event_get_old_size
 * 功能描述: 获取对象大小改变前的旧区域。可以在`LV_EVENT_SIZE_CHANGED`中使用
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 对象的旧绝对区域或NULL(如果在无关事件上调用)
 */
const lv_area_t *lv_event_get_old_size(lv_event_t *e);

/**
 * 函数名称: lv_event_get_key
 * 功能描述: 获取作为参数传递给事件的键。可以在`LV_EVENT_KEY`中使用
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 如果在不相关的事件上调用，则为触发键或NULL
 */
uint32_t lv_event_get_key(lv_event_t *e);

/**
 * 函数名称: lv_event_get_scroll_anim
 * 功能描述: 获取滚动的动画描述符。可以在`LV_EVENT_SCROLL_BEGIN`中使用
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 将滚动对象的动画。(可根据需要修改)
 */
lv_anim_t *lv_event_get_scroll_anim(lv_event_t *e);

/**
 * 函数名称: lv_event_set_ext_draw_size
 * 功能描述: 设置新的额外绘制尺寸。可以在`LV_EVENT_REFR_EXT_DRAW_SIZE`中使用
 * 输入参数: e    --> 指向事件的指针
 *           size --> 新的额外绘制尺寸
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_event_set_ext_draw_size(lv_event_t *e, lv_coord_t size);

/**
 * 函数名称: lv_event_get_self_size_info
 * 功能描述: 获取指向应保存自身大小的`lv_point_t`变量的指针(宽度为`point->x`，高度为`point->y`)。可以在`LV_EVENT_GET_SELF_SIZE`中使用
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 如果在不相关的事件上调用，则指向`lv_point_t`或NULL的指针
 */
lv_point_t *lv_event_get_self_size_info(lv_event_t *e);

/**
 * 函数名称: lv_event_get_hit_test_info
 * 功能描述: 获取指向应保存命中测试结果的`lv_hit_test_info_t`变量的指针。可以在`LV_EVENT_HIT_TEST`中使用
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 如果在无关事件上调用，则指向`lv_hit_test_info_t`或NULL的指针
 */
lv_hit_test_info_t *lv_event_get_hit_test_info(lv_event_t *e);

/**
 * 函数名称: lv_event_get_cover_area
 * 功能描述: 获取指向应该检查对象是否完全覆盖它的区域的指针。可以在`LV_EVENT_HIT_TEST`中使用
 * 输入参数: e --> 指向事件的指针
 * 输出参数: 无
 * 返回说明: 具有要检查的绝对坐标的区域
 */
const lv_area_t *lv_event_get_cover_area(lv_event_t *e);

/**
 * 函数名称: lv_event_set_cover_res
 * 功能描述: 设置封面检查的结果。可以在`LV_EVENT_COVER_CHECK`中使用
 * 输入参数: e   --> 指向事件的指针
 *           res --> ::lv_cover_check_info_t的一个元素
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_event_set_cover_res(lv_event_t *e, lv_cover_res_t res);

#ifdef __cplusplus
}
#endif

#endif
