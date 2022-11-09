#ifndef LV_HAL_INDEV_H
#define LV_HAL_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../lv_conf_internal.h"
#include "../misc/lv_area.h"
#include "../misc/lv_timer.h"

// 以像素为单位的拖动阈值
#define LV_INDEV_DEF_SCROLL_LIMIT               10

// 拖投减速[%]。更大的值 -> 更快的减速
#define LV_INDEV_DEF_SCROLL_THROW               10

// 长按时间以毫秒为单位。是时候发送`LV_EVENT_LONG_PRESSSED`
#define LV_INDEV_DEF_LONG_PRESS_TIME            400

// 长按重复触发周期[ms], LV_EVENT_LONG_PRESSED_REPEAT之间的时间
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME        100

// 以像素为单位的手势阈值
#define LV_INDEV_DEF_GESTURE_LIMIT              50

// 滑动前释放手势的最小速度(像素)
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY       3

struct _lv_obj_t;
struct _lv_disp_t;
struct _lv_group_t;
struct _lv_indev_t;
struct _lv_indev_drv_t;

// 可能的输入设备类型
typedef enum {
    LV_INDEV_TYPE_NONE,                         // 未初始化状态
    LV_INDEV_TYPE_POINTER,                      // 触摸板、鼠标、外部按钮
    LV_INDEV_TYPE_KEYPAD,                       // 键盘或键盘
    LV_INDEV_TYPE_BUTTON,                       // 分配给屏幕特定点的外部(硬件按钮)
    LV_INDEV_TYPE_ENCODER,                      // 只有左转、右转和一个按钮的编码器
} lv_indev_type_t;

// 输入设备的状态
typedef enum {
    LV_INDEV_STATE_RELEASED = 0,
    LV_INDEV_STATE_PRESSED
} lv_indev_state_t;

// 传递给输入驱动程序以填充的数据结构
typedef struct {
    lv_point_t       point;                     // 对于LV_INDEV_TYPE_POINTER当前按下的点
    uint32_t         key;                       // 对于LV_INDEV_TYPE_KEYPAD当前按下的键
    uint32_t         btn_id;                    // 对于LV_INDEV_TYPE_BUTTON当前按下的按钮
    int16_t          enc_diff;                  // 对于LV_INDEV_TYPE_ENCODER自上次读取后的步数
    lv_indev_state_t state;                     // LV_INDEV_STATE_REL或LV_INDEV_STATE_PR
    bool             continue_reading;          // 如果设置为true，则再次调用读取回调
} lv_indev_data_t;

// 由用户初始化并由'lv_indev_add()'注册
typedef struct _lv_indev_drv_t {
    lv_indev_type_t   type;                     // 输入设备类型

    // 读取输入设备数据的函数指针
    void (*read_cb)(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

    // 在输入设备上发生操作时调用。第二个参数是来自`lv_event_t`的事件
    void (*feedback_cb)(struct _lv_indev_drv_t *, uint8_t);

#if LV_USE_USER_DATA
    void              *user_data;               // 用户数据
#endif
    struct _lv_disp_t *disp;                    // 指向指定显示器的指针
    lv_timer_t        *read_timer;              // 定时读取输入设备的定时器
    uint8_t           scroll_limit;             // 在实际拖动对象之前要滑动的像素数
    uint8_t           scroll_throw;             // 拖投减速[%]。更大的价值意味着更快的减速
    uint8_t           gesture_min_velocity;     // 至少这种差异应该在两点之间评估为手势
    uint8_t           gesture_limit;            // 至少这个区别应该是发送手势
    uint16_t          long_press_time;          // 长按时间(毫秒)
    uint16_t          long_press_repeat_time;   // 长按重复触发周期[ms]
} lv_indev_drv_t;

// 输入设备的运行时数据。库内部使用，您应该不需要触摸它
typedef struct _lv_indev_proc_t {
    lv_indev_state_t         state;                     // Current state of the input device
    uint8_t                  long_pr_sent : 1;
    uint8_t                  reset_query : 1;
    uint8_t                  disabled : 1;
    uint8_t                  wait_until_release : 1;

    union {
        struct {
            // 指针和按钮数据
            lv_point_t       act_point;                 // 输入设备当前点
            lv_point_t       last_point;                // 输入设备的最后一点
            lv_point_t       last_raw_point;            // 从read_cb读取的最后一点
            lv_point_t       vect;                      // `act_point`和`last_point`之间的区别
            lv_point_t       scroll_sum;                // 计算拖动的像素以检查LV_INDEV_DEF_SCROLL_LIMIT
            lv_point_t       scroll_throw_vect;
            lv_point_t       scroll_throw_vect_ori;
            struct _lv_obj_t *act_obj;                  // 被按压的对象
            struct _lv_obj_t *last_obj;                 // 最后按下的对象
            struct _lv_obj_t *scroll_obj;               // 被滚动的对象
            struct _lv_obj_t *last_pressed;             // 最后按下的对象
            lv_area_t        scroll_area;

            lv_point_t       gesture_sum;               // 计算手势像素以检查LV_INDEV_DEF_GESTURE_LIMIT
            lv_dir_t         scroll_dir : 4;
            lv_dir_t         gesture_dir : 4;
            uint8_t          gesture_sent : 1;
        } pointer;
        struct {
            // 键盘数据
            lv_indev_state_t last_state;
            uint32_t         last_key;
        } keypad;
    } types;

    uint32_t                 pr_timestamp;              // 按下的时间戳
    uint32_t                 longpr_rep_timestamp;      // 长按重复时间戳
} _lv_indev_proc_t;

// 带有驱动程序、运行时数据('proc') 和一些附加信息的主要输入设备描述符
typedef struct _lv_indev_t {
    struct _lv_indev_drv_t *driver;
    _lv_indev_proc_t       proc;
    struct _lv_obj_t       *cursor;                     // LV_INPUT_TYPE_POINTER的光标
    struct _lv_group_t     *group;                      // 键盘目标组
    const lv_point_t       *btn_points;                 // 分配给按钮屏幕的阵列点将在此处被按钮按下
} lv_indev_t;

/**
 * 函数名称: lv_indev_drv_init
 * 功能描述: 使用默认值初始化输入设备驱动程序。它用于在字段ant中具有已知值而不是内存垃圾。之后，您可以设置字段
 * 输入参数: 无
 * 输出参数: driver --> 指向要初始化的驱动程序变量的指针
 * 返回说明: 无
 */
void lv_indev_drv_init(struct _lv_indev_drv_t *driver);

/**
 * 函数名称: lv_indev_drv_register
 * 功能描述: 注册一个初始化的输入设备驱动程序
 * 输入参数: driver --> 指向初始化的`lv_indev_drv_t`变量的指针(可以是局部变量)
 * 输出参数: 无
 * 返回说明: 指向新输入设备的指针或出现错误时为NULL
 */
lv_indev_t *lv_indev_drv_register(struct _lv_indev_drv_t *driver);

/**
 * 函数名称: lv_indev_drv_update
 * 功能描述: 在运行时更新驱动程序
 * 输入参数: indev   --> 指向输入设备的指针。(`lv_indev_drv_register`的返回值)
 *           new_drv --> 指向新驱动程序的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_indev_drv_update(lv_indev_t *indev, struct _lv_indev_drv_t *new_drv);

void lv_indev_delete(lv_indev_t *indev);

/**
 * 函数名称: lv_indev_get_next
 * 功能描述: 获取下一个输入设备
 * 输入参数: indev --> 指向当前输入设备的指针。NULL则初始化
 * 输出参数: 无
 * 返回说明: 下一个输入设备或NULL如果没有更多。参数为NULL时给出第一个输入设备
 */
lv_indev_t *lv_indev_get_next(lv_indev_t *indev);

/**
 * 函数名称: _lv_indev_read
 * 功能描述: 从输入设备读取数据
 * 输入参数: indev --> 指向输入设备的指针
 * 输出参数: data  --> 输入设备将在此处写入其数据
 * 返回说明: 无
 */
void _lv_indev_read(lv_indev_t *indev, lv_indev_data_t *data);

#ifdef __cplusplus
}
#endif

#endif
