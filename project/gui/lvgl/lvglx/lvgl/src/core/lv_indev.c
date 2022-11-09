#include "lv_obj.h"
#include "lv_disp.h"
#include "lv_refr.h"
#include "lv_indev.h"
#include "lv_group.h"
#include "lv_indev_scroll.h"
#include "../misc/lv_math.h"
#include "../misc/lv_timer.h"
#include "../hal/lv_hal_tick.h"

#if (LV_INDEV_DEF_SCROLL_THROW <= 0)
#warning "LV_INDEV_DRAG_THROW must be greater than 0"
#endif

static lv_indev_t *indev_act;
static lv_obj_t *indev_obj_act = NULL;

static void indev_gesture(_lv_indev_proc_t *proc);
static void indev_proc_press(_lv_indev_proc_t *proc);
static void indev_click_focus(_lv_indev_proc_t *proc);
static bool indev_reset_check(_lv_indev_proc_t *proc);
static void indev_proc_release(_lv_indev_proc_t *proc);
static void indev_proc_reset_query_handler(lv_indev_t *indev);

static void indev_keypad_proc(lv_indev_t *i, lv_indev_data_t *data);
static void indev_button_proc(lv_indev_t *i, lv_indev_data_t *data);
static void indev_encoder_proc(lv_indev_t *i, lv_indev_data_t *data);
static void indev_pointer_proc(lv_indev_t *i, lv_indev_data_t *data);

#if LV_LOG_TRACE_INDEV
#define INDEV_TRACE(...)        LV_LOG_TRACE(__VA_ARGS__)
#else
#define INDEV_TRACE(...)
#endif

void lv_indev_read_timer_cb(lv_timer_t *timer)
{
    INDEV_TRACE("begin");

    lv_indev_data_t data;
    indev_act = (lv_indev_t *)timer->user_data;

    // 读取和处理所有indevs
    if (indev_act->driver->disp == NULL) {
        // 未分配给任何显示器
        return;
    }

    // 在处理点之前处理重置查询
    indev_proc_reset_query_handler(indev_act);

    if ((indev_act->proc.disabled) || (indev_act->driver->disp->prev_scr != NULL)) {
        return;
    }

    bool continue_reading;
    do {
        // 读取数据
        _lv_indev_read(indev_act, &data);
        continue_reading = data.continue_reading;

        // 即使在读取功能中，活动对象也可能被删除
        indev_proc_reset_query_handler(indev_act);
        indev_obj_act = NULL;

        indev_act->proc.state = data.state;

        // 保存上次活动时间
        if (indev_act->proc.state == LV_INDEV_STATE_PRESSED) {
            indev_act->driver->disp->last_activity_time = lv_tick_get();
        } else if ((indev_act->driver->type == LV_INDEV_TYPE_ENCODER) && data.enc_diff) {
            indev_act->driver->disp->last_activity_time = lv_tick_get();
        }

        if (indev_act->driver->type == LV_INDEV_TYPE_POINTER) {
            indev_pointer_proc(indev_act, &data);
        } else if (indev_act->driver->type == LV_INDEV_TYPE_KEYPAD) {
            indev_keypad_proc(indev_act, &data);
        } else if (indev_act->driver->type == LV_INDEV_TYPE_ENCODER) {
            indev_encoder_proc(indev_act, &data);
        } else if (indev_act->driver->type == LV_INDEV_TYPE_BUTTON) {
            indev_button_proc(indev_act, &data);
        }

        // 如果它发生在处理过程中，则处理重置查询
        indev_proc_reset_query_handler(indev_act);
    } while (continue_reading);

    // indev处理结束，所以没有行为indev
    indev_act = NULL;
    indev_obj_act = NULL;

    INDEV_TRACE("finished");
}

void lv_indev_enable(lv_indev_t *indev, bool en)
{
    uint8_t enable = en ? 0 : 1;

    if (indev) {
        indev->proc.disabled = enable;
    } else {
        lv_indev_t *i = lv_indev_get_next(NULL);
        while (i) {
            i->proc.disabled = enable;
            i = lv_indev_get_next(i);
        }
    }
}

lv_indev_t *lv_indev_get_act(void)
{
    return indev_act;
}

lv_indev_type_t lv_indev_get_type(const lv_indev_t *indev)
{
    if (indev == NULL) {
        return LV_INDEV_TYPE_NONE;
    }

    return indev->driver->type;
}

void lv_indev_reset(lv_indev_t *indev, lv_obj_t *obj)
{
    if (indev) {
        indev->proc.reset_query = 1;
        if (indev_act == indev) {
            indev_obj_act = NULL;
        }

        if ((indev->driver->type == LV_INDEV_TYPE_POINTER) || (indev->driver->type == LV_INDEV_TYPE_KEYPAD)) {
            if ((obj == NULL) || (indev->proc.types.pointer.last_pressed == obj)) {
                indev->proc.types.pointer.last_pressed = NULL;
            }

            if ((obj == NULL) || (indev->proc.types.pointer.act_obj == obj)) {
                indev->proc.types.pointer.act_obj = NULL;
            }

            if ((obj == NULL) || (indev->proc.types.pointer.last_obj == obj)) {
                indev->proc.types.pointer.last_obj = NULL;
            }
        }
    } else {
        lv_indev_t *i = lv_indev_get_next(NULL);
        while (i) {
            i->proc.reset_query = 1;
            if ((i->driver->type == LV_INDEV_TYPE_POINTER) || (i->driver->type == LV_INDEV_TYPE_KEYPAD)) {
                if ((obj == NULL) || (i->proc.types.pointer.last_pressed == obj)) {
                    i->proc.types.pointer.last_pressed = NULL;
                }

                if ((obj == NULL) || (i->proc.types.pointer.act_obj == obj)) {
                    i->proc.types.pointer.act_obj = NULL;
                }

                if ((obj == NULL) || (i->proc.types.pointer.last_obj == obj)) {
                    i->proc.types.pointer.last_obj = NULL;
                }
            }
            i = lv_indev_get_next(i);
        }

        indev_obj_act = NULL;
    }
}

void lv_indev_reset_long_press(lv_indev_t *indev)
{
    indev->proc.long_pr_sent = 0;
    indev->proc.longpr_rep_timestamp = lv_tick_get();
    indev->proc.pr_timestamp = lv_tick_get();
}

void lv_indev_set_cursor(lv_indev_t *indev, lv_obj_t *cur_obj)
{
    if (indev->driver->type != LV_INDEV_TYPE_POINTER) {
        return;
    }

    indev->cursor = cur_obj;
    lv_obj_set_parent(indev->cursor, lv_disp_get_layer_sys(indev->driver->disp));
    lv_obj_set_pos(indev->cursor, indev->proc.types.pointer.act_point.x, indev->proc.types.pointer.act_point.y);
    lv_obj_clear_flag(indev->cursor, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(indev->cursor, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING);
}

void lv_indev_set_group(lv_indev_t *indev, lv_group_t *group)
{
    if ((indev->driver->type == LV_INDEV_TYPE_KEYPAD) || (indev->driver->type == LV_INDEV_TYPE_ENCODER)) {
        indev->group = group;
    }
}

void lv_indev_set_button_points(lv_indev_t *indev, const lv_point_t points[])
{
    if (indev->driver->type == LV_INDEV_TYPE_BUTTON) {
        indev->btn_points = points;
    }
}

void lv_indev_get_point(const lv_indev_t *indev, lv_point_t *point)
{
    if (indev == NULL) {
        point->x = 0;
        point->y = 0;
        return;
    }

    if ((indev->driver->type != LV_INDEV_TYPE_POINTER) && (indev->driver->type != LV_INDEV_TYPE_BUTTON)) {
        point->x = -1;
        point->y = -1;
    } else {
        point->x = indev->proc.types.pointer.act_point.x;
        point->y = indev->proc.types.pointer.act_point.y;
    }
}

lv_dir_t lv_indev_get_gesture_dir(const lv_indev_t *indev)
{
    return indev->proc.types.pointer.gesture_dir;
}

uint32_t lv_indev_get_key(const lv_indev_t *indev)
{
    if (indev->driver->type != LV_INDEV_TYPE_KEYPAD) {
        return 0;
    } else {
        return indev->proc.types.keypad.last_key;
    }
}

lv_dir_t lv_indev_get_scroll_dir(const lv_indev_t *indev)
{
    if (indev == NULL) {
        return false;
    }

    if ((indev->driver->type != LV_INDEV_TYPE_POINTER) && (indev->driver->type != LV_INDEV_TYPE_BUTTON)) {
        return false;
    }

    return indev->proc.types.pointer.scroll_dir;
}

lv_obj_t *lv_indev_get_scroll_obj(const lv_indev_t *indev)
{
    if (indev == NULL) {
        return NULL;
    }

    if ((indev->driver->type != LV_INDEV_TYPE_POINTER) && (indev->driver->type != LV_INDEV_TYPE_BUTTON)) {
        return NULL;
    }

    return indev->proc.types.pointer.scroll_obj;
}

void lv_indev_get_vect(const lv_indev_t *indev, lv_point_t *point)
{
    point->x = 0;
    point->y = 0;

    if (indev == NULL) {
        return;
    }

    if ((indev->driver->type == LV_INDEV_TYPE_POINTER) || (indev->driver->type == LV_INDEV_TYPE_BUTTON)) {
        point->x = indev->proc.types.pointer.vect.x;
        point->y = indev->proc.types.pointer.vect.y;
    }
}

void lv_indev_wait_release(lv_indev_t *indev)
{
    if (indev == NULL) {
        return;
    }

    indev->proc.wait_until_release = 1;
}

lv_obj_t *lv_indev_get_obj_act(void)
{
    return indev_obj_act;
}

lv_timer_t *lv_indev_get_read_timer(lv_indev_t *indev)
{
    if (!indev) {
        LV_LOG_WARN("lv_indev_get_read_timer: indev was NULL");
        return NULL;
    }

    return indev->driver->read_timer;
}

lv_obj_t *lv_indev_search_obj(lv_obj_t *obj, lv_point_t *point)
{
    lv_obj_t *found_p = NULL;

    // 如果这个obj被隐藏，那么子对象也被隐藏，所以立即返回
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        return NULL;
    }

    lv_point_t p_trans = *point;
    lv_obj_transform_point(obj, &p_trans, false, true);

    bool hit_test_ok = lv_obj_hit_test(obj, &p_trans);

    // 如果这个点在这个对象上或者已经溢出可见，也检查它的子对象
    if (_lv_area_is_point_on(&obj->coords, &p_trans, 0) || lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
        int32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);

        // 如果子匹配使用它
        for (i = child_cnt - 1; i >= 0; i--) {
            lv_obj_t *child = obj->spec_attr->children[i];
            found_p = lv_indev_search_obj(child, &p_trans);
            if (found_p) {
                return found_p;
            }
        }
    }

    // 如果没有返回更早的被点击的子对象并且这个obj的hittest是ok使用它否则返回NULL
    if (hit_test_ok) {
        return obj;
    } else {
        return NULL;
    }
}

static void indev_pointer_proc(lv_indev_t *i, lv_indev_data_t *data)
{
    lv_disp_t *disp = i->driver->disp;

    // 保存原始点，以便它们可以在_lv_indev_read中再次使用
    i->proc.types.pointer.last_raw_point.x = data->point.x;
    i->proc.types.pointer.last_raw_point.y = data->point.y;

    if ((disp->driver->rotated == LV_DISP_ROT_180) || (disp->driver->rotated == LV_DISP_ROT_270)) {
        data->point.x = disp->driver->hor_res - data->point.x - 1;
        data->point.y = disp->driver->ver_res - data->point.y - 1;
    }

    if ((disp->driver->rotated == LV_DISP_ROT_90) || (disp->driver->rotated == LV_DISP_ROT_270)) {
        lv_coord_t tmp = data->point.y;
        data->point.y = data->point.x;
        data->point.x = disp->driver->ver_res - tmp - 1;
    }

    // 简单的健全性检查
    if(data->point.x < 0) {
        LV_LOG_WARN("X is %d which is smaller than zero", (int)data->point.x);
    }

    if(data->point.x >= lv_disp_get_hor_res(i->driver->disp)) {
        LV_LOG_WARN("X is %d which is greater than hor. res", (int)data->point.x);
    }

    if(data->point.y < 0) {
        LV_LOG_WARN("Y is %d which is smaller than zero", (int)data->point.y);
    }

    if(data->point.y >= lv_disp_get_ver_res(i->driver->disp)) {
        LV_LOG_WARN("Y is %d which is greater than ver. res", (int)data->point.y);
    }

    // 如果设置和移动，则移动光标
    if ((i->cursor != NULL) && ((i->proc.types.pointer.last_point.x != data->point.x) || (i->proc.types.pointer.last_point.y != data->point.y))) {
        lv_obj_set_pos(i->cursor, data->point.x, data->point.y);
    }

    i->proc.types.pointer.act_point.x = data->point.x;
    i->proc.types.pointer.act_point.y = data->point.y;

    if (i->proc.state == LV_INDEV_STATE_PRESSED) {
        indev_proc_press(&i->proc);
    } else {
        indev_proc_release(&i->proc);
    }

    i->proc.types.pointer.last_point.x = i->proc.types.pointer.act_point.x;
    i->proc.types.pointer.last_point.y = i->proc.types.pointer.act_point.y;
}

static void indev_keypad_proc(lv_indev_t *i, lv_indev_data_t *data)
{
    if ((data->state == LV_INDEV_STATE_PRESSED) && i->proc.wait_until_release) {
        return;
    }

    if (i->proc.wait_until_release) {
        i->proc.wait_until_release = 0;
        i->proc.pr_timestamp = 0;
        i->proc.long_pr_sent = 0;

        // 跳过发布的处理
        i->proc.types.keypad.last_state = LV_INDEV_STATE_RELEASED;
    }

    lv_group_t *g = i->group;
    if (g == NULL) {
        return;
    }

    indev_obj_act = lv_group_get_focused(g);
    if (indev_obj_act == NULL) {
        return;
    }

    bool dis = lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);

    // 保存最后一个键以将其与RELEASE上的当前后者进行比较
    uint32_t prev_key = i->proc.types.keypad.last_key;

    // 保存最后一个键。必须在这里完成，否则`lv_indev_get_key`将返回事件中的最后一个键
    i->proc.types.keypad.last_key = data->key;

    // 保存之前的状态，这样我们就可以检测下面的状态变化，并且现在也设置最后一个状态，所以如果途中有任何事件处理程序返回`LV_RES_INV`，下一次记住最后一个状态
    uint32_t prev_state = i->proc.types.keypad.last_state;
    i->proc.types.keypad.last_state = data->state;

    // 按键发生
    if ((data->state == LV_INDEV_STATE_PRESSED) && (prev_state == LV_INDEV_STATE_RELEASED)) {
        LV_LOG_INFO("%" LV_PRIu32 " key is pressed", data->key);
        i->proc.pr_timestamp = lv_tick_get();

        if (data->key == LV_KEY_NEXT) {      // 将注意力转移到NEXT
            lv_group_set_editing(g, false);         // KEYPAD不使用编辑确保它已禁用
            lv_group_focus_next(g);
            if (indev_reset_check(&i->proc)) {
                return;
            }
        } else if (data->key == LV_KEY_PREV) {      // 将注意力转移到PREV
            lv_group_set_editing(g, false);         // KEYPAD不使用编辑确保它已禁用
            lv_group_focus_prev(g);
            if (indev_reset_check(&i->proc)) {
                return;
            }
        } else if (!dis) {
            if (data->key == LV_KEY_ENTER) {
                lv_group_send_data(g, LV_KEY_ENTER);
                if (indev_reset_check(&i->proc)) {
                    return;
                }

                if (!dis) {
                    lv_event_send(indev_obj_act, LV_EVENT_PRESSED, indev_act);
                }

                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else if (data->key == LV_KEY_ESC) {
                lv_group_send_data(g, LV_KEY_ESC);
                if (indev_reset_check(&i->proc)) {
                    return;
                }

                lv_event_send(indev_obj_act, LV_EVENT_CANCEL, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else {
                lv_group_send_data(g, data->key);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            }
        }
    } else if (!dis && (data->state == LV_INDEV_STATE_PRESSED) && (prev_state == LV_INDEV_STATE_PRESSED)) {
        if (data->key == LV_KEY_ENTER) {
            lv_event_send(indev_obj_act, LV_EVENT_PRESSING, indev_act);
            if (indev_reset_check(&i->proc)) {
                return;
            }
        }

        // 长按时间已过?
        if ((i->proc.long_pr_sent == 0) && (lv_tick_elaps(i->proc.pr_timestamp) > i->driver->long_press_time)) {
            i->proc.long_pr_sent = 1;
            if (data->key == LV_KEY_ENTER) {
                i->proc.longpr_rep_timestamp = lv_tick_get();
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            }
        } else if ((i->proc.long_pr_sent != 0) && (lv_tick_elaps(i->proc.longpr_rep_timestamp) > i->driver->long_press_repeat_time)) {
            i->proc.longpr_rep_timestamp = lv_tick_get();

            // 按ENTER发送LONG_PRESS_REP
            if (data->key == LV_KEY_ENTER) {
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else if (data->key == LV_KEY_NEXT) {      // 再次将注意力转移到NEXT上
                lv_group_set_editing(g, false);         // KEYPAD不使用编辑确保它已禁用
                lv_group_focus_next(g);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else if (data->key == LV_KEY_PREV) {      // 再次将焦点转移到PREV上
                lv_group_set_editing(g, false);         // KEYPAD不使用编辑确保它已禁用
                lv_group_focus_prev(g);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else {
                lv_group_send_data(g, data->key);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            }
        }
    } else if (!dis && (data->state == LV_INDEV_STATE_RELEASED) && (prev_state == LV_INDEV_STATE_PRESSED)) {
        LV_LOG_INFO("%" LV_PRIu32 " key is released", data->key);

        // 当它被释放时，用户可能会清除该键。始终松开按键
        data->key = prev_key;
        if (data->key == LV_KEY_ENTER) {
            lv_event_send(indev_obj_act, LV_EVENT_RELEASED, indev_act);
            if (indev_reset_check(&i->proc)) {
                return;
            }

            if (i->proc.long_pr_sent == 0) {
                lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            }

            lv_event_send(indev_obj_act, LV_EVENT_CLICKED, indev_act);
            if (indev_reset_check(&i->proc)) {
                return;
            }
        }

        i->proc.pr_timestamp = 0;
        i->proc.long_pr_sent = 0;
    }

    indev_obj_act = NULL;
}

static void indev_encoder_proc(lv_indev_t *i, lv_indev_data_t *data)
{
    if ((data->state == LV_INDEV_STATE_PRESSED) && i->proc.wait_until_release) {
        return;
    }

    if (i->proc.wait_until_release) {
        i->proc.wait_until_release = 0;
        i->proc.pr_timestamp = 0;
        i->proc.long_pr_sent = 0;

        // 跳过释放的处理
        i->proc.types.keypad.last_state = LV_INDEV_STATE_RELEASED;
    }

    // 在其他任何事情之前保存最后的按键。如果函数因任何原因返回，它们需要已经保存
    lv_indev_state_t last_state = i->proc.types.keypad.last_state;
    i->proc.types.keypad.last_state = data->state;
    i->proc.types.keypad.last_key = data->key;

    lv_group_t *g = i->group;
    if (g == NULL) {
        return;
    }

    indev_obj_act = lv_group_get_focused(g);
    if (indev_obj_act == NULL) {
        return;
    }

    // 处理只有松开按钮才有效的步骤
    if (data->state != LV_INDEV_STATE_RELEASED) {
        data->enc_diff = 0;
    }

    // 刷新焦点对象。它可能会因lv_group_focus_prev/next而改变
    indev_obj_act = lv_group_get_focused(g);
    if (indev_obj_act == NULL) {
        return;
    }

    // 发生按下按钮
    if ((data->state == LV_INDEV_STATE_PRESSED) && (last_state == LV_INDEV_STATE_RELEASED)) {
        LV_LOG_INFO("pressed");

        i->proc.pr_timestamp = lv_tick_get();

        if (data->key == LV_KEY_ENTER) {
            bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) || lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);
            if ((lv_group_get_editing(g) == true) || (editable_or_scrollable == false)) {
                lv_event_send(indev_obj_act, LV_EVENT_PRESSED, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            }
        } else if (data->key == LV_KEY_LEFT) {
            // 左侧模拟编码器
            data->enc_diff--;
        } else if (data->key == LV_KEY_RIGHT) {
            // 右模拟编码器
            data->enc_diff++;
        } else if (data->key == LV_KEY_ESC) {
            // 将ESC作为普通KEY发送
            lv_group_send_data(g, LV_KEY_ESC);
            if (indev_reset_check(&i->proc)) {
                return;
            }

            lv_event_send(indev_obj_act, LV_EVENT_CANCEL, indev_act);
            if (indev_reset_check(&i->proc)) {
                return;
            }
        } else {
            lv_group_send_data(g, data->key);
            if (indev_reset_check(&i->proc)) {
                return;
            }
        }
    } else if ((data->state == LV_INDEV_STATE_PRESSED) && (last_state == LV_INDEV_STATE_PRESSED)) {
        // 长按
        if ((i->proc.long_pr_sent == 0) && (lv_tick_elaps(i->proc.pr_timestamp) > i->driver->long_press_time)) {
            i->proc.long_pr_sent = 1;
            i->proc.longpr_rep_timestamp = lv_tick_get();

            if (data->key == LV_KEY_ENTER) {
                bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) || lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);

                // 进入长按切换编辑模式
                if (editable_or_scrollable) {
                    // 如果只有一个对象(无处可导航)，请不要离开编辑模式
                    if(lv_group_get_obj_count(g) > 1) {
                        LV_LOG_INFO("toggling edit mode");
                        lv_group_set_editing(g, lv_group_get_editing(g) ? false : true);    // 长按切换编辑模式
                        lv_obj_clear_state(indev_obj_act, LV_STATE_PRESSED);                // 手动移除按下状态
                    }
                } else {    // 如果不可编辑，则只需发送一个长按调用祖先的事件处理程序
                    lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }
                }
            }

            i->proc.long_pr_sent = 1;
        } else if ((i->proc.long_pr_sent != 0) && (lv_tick_elaps(i->proc.longpr_rep_timestamp) > i->driver->long_press_repeat_time)) {
            i->proc.longpr_rep_timestamp = lv_tick_get();

            if (data->key == LV_KEY_ENTER) {
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else if (data->key == LV_KEY_LEFT) {
                // 左侧模拟编码器
                data->enc_diff--;
            } else if (data->key == LV_KEY_RIGHT) {
                // 右模拟编码器
                data->enc_diff++;
            } else {
                lv_group_send_data(g, data->key);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            }
        }
    } else if ((data->state == LV_INDEV_STATE_RELEASED) && (last_state == LV_INDEV_STATE_PRESSED)) {
        LV_LOG_INFO("released");

        if (data->key == LV_KEY_ENTER) {
            bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) || lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);

            // 在不可编辑的对象上释放按钮。只需发送输入
            if (editable_or_scrollable == false) {
                lv_event_send(indev_obj_act, LV_EVENT_RELEASED, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }

                if (i->proc.long_pr_sent == 0) {
                    lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                }

                if (indev_reset_check(&i->proc)) {
                    return;
                }

                lv_event_send(indev_obj_act, LV_EVENT_CLICKED, indev_act);
                if (indev_reset_check(&i->proc)) {
                    return;
                }
            } else if (lv_group_get_editing(g)) {   // 正在编辑对象并释放按钮
                // 忽略长按输入释放，因为它来自模式切换
                if (!i->proc.long_pr_sent || lv_group_get_obj_count(g) <= 1) {
                    lv_event_send(indev_obj_act, LV_EVENT_RELEASED, indev_act);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }

                    lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }

                    lv_event_send(indev_obj_act, LV_EVENT_CLICKED, indev_act);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }

                    lv_group_send_data(g, LV_KEY_ENTER);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }
                } else {
                    // 手动移除按下状态
                    lv_obj_clear_state(indev_obj_act, LV_STATE_PRESSED);
                }
            } else if (!i->proc.long_pr_sent) {
                LV_LOG_INFO("entering edit mode");

                // 设置编辑模式
                lv_group_set_editing(g, true);
            }
        }

        i->proc.pr_timestamp = 0;
        i->proc.long_pr_sent = 0;
    }

    indev_obj_act = NULL;

    // 如果编码器步进或通过左/右键模拟步进
    if (data->enc_diff != 0) {
        // 在编辑模式下发送左/右键
        if (lv_group_get_editing(g)) {
            LV_LOG_INFO("rotated by %+d (edit)", data->enc_diff);

            int32_t s;
            if (data->enc_diff < 0) {
                for (s = 0; s < -data->enc_diff; s++) {
                    lv_group_send_data(g, LV_KEY_LEFT);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }
                }
            } else if (data->enc_diff > 0) {
                for (s = 0; s < data->enc_diff; s++) {
                    lv_group_send_data(g, LV_KEY_RIGHT);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }
                }
            }
        } else {
            LV_LOG_INFO("rotated by %+d (nav)", data->enc_diff);

            int32_t s;
            if (data->enc_diff < 0) {
                for (s = 0; s < -data->enc_diff; s++) {
                    lv_group_focus_prev(g);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }
                }
            } else if (data->enc_diff > 0) {
                for (s = 0; s < data->enc_diff; s++) {
                    lv_group_focus_next(g);
                    if (indev_reset_check(&i->proc)) {
                        return;
                    }
                }
            }
        }
    }
}

static void indev_button_proc(lv_indev_t *i, lv_indev_data_t *data)
{
    // 如果i->btn_points为NULL，则优雅地死去
    if (i->btn_points == NULL) {
        LV_LOG_WARN("btn_points is NULL");
        return;
    }

    lv_coord_t x = i->btn_points[data->btn_id].x;
    lv_coord_t y = i->btn_points[data->btn_id].y;

    static lv_indev_state_t prev_state = LV_INDEV_STATE_RELEASED;
    if (prev_state != data->state) {
        if (data->state == LV_INDEV_STATE_PRESSED) {
            LV_LOG_INFO("button %" LV_PRIu32 " is pressed (x:%d y:%d)", data->btn_id, (int)x, (int)y);
        } else {
            LV_LOG_INFO("button %" LV_PRIu32 " is released (x:%d y:%d)", data->btn_id, (int)x, (int)y);
        }
    }

    // 如果有新的点，总是发布
    if (data->state == LV_INDEV_STATE_PRESSED) {
        if ((i->proc.types.pointer.last_point.x != x) || (i->proc.types.pointer.last_point.y != y)) {
            indev_proc_release(&i->proc);
        }
    }

    if (indev_reset_check(&i->proc)) {
        return;
    }

    // 保存新点
    i->proc.types.pointer.act_point.x = x;
    i->proc.types.pointer.act_point.y = y;

    if (data->state == LV_INDEV_STATE_PRESSED) {
        indev_proc_press(&i->proc);
    } else {
        indev_proc_release(&i->proc);
    }

    if (indev_reset_check(&i->proc)) {
        return;
    }

    i->proc.types.pointer.last_point.x = i->proc.types.pointer.act_point.x;
    i->proc.types.pointer.last_point.y = i->proc.types.pointer.act_point.y;
}

static void indev_proc_press(_lv_indev_proc_t *proc)
{
    LV_LOG_INFO("pressed at x:%d y:%d", (int)proc->types.pointer.act_point.x, (int)proc->types.pointer.act_point.y);

    indev_obj_act = proc->types.pointer.act_obj;

    if (proc->wait_until_release != 0) {
        return;
    }

    bool new_obj_searched = false;
    lv_disp_t *disp = indev_act->driver->disp;

    // 如果没有最后一个对象，则搜索
    if (indev_obj_act == NULL) {
        indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_sys(disp), &proc->types.pointer.act_point);
        if (indev_obj_act == NULL) {
            indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_top(disp), &proc->types.pointer.act_point);
        }

        if (indev_obj_act == NULL) {
            indev_obj_act = lv_indev_search_obj(lv_disp_get_scr_act(disp), &proc->types.pointer.act_point);
        }

        new_obj_searched = true;
    } else if ((proc->types.pointer.scroll_obj == NULL) && (lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_PRESS_LOCK) == false)) {
        indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_sys(disp), &proc->types.pointer.act_point);
        if (indev_obj_act == NULL) {
            indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_top(disp), &proc->types.pointer.act_point);
        }

        if (indev_obj_act == NULL) {
            indev_obj_act = lv_indev_search_obj(lv_disp_get_scr_act(disp), &proc->types.pointer.act_point);
        }

        new_obj_searched = true;
    }

    // 最后一个对象可能有滚动抛出。手动停止
    if (new_obj_searched && proc->types.pointer.last_obj) {
        proc->types.pointer.scroll_throw_vect.x = 0;
        proc->types.pointer.scroll_throw_vect.y = 0;
        
        _lv_indev_scroll_throw_handler(proc);
        if (indev_reset_check(proc)) {
            return;
        }
    }

    lv_obj_transform_point(indev_obj_act, &proc->types.pointer.act_point, true, true);

    // 如果发现一个新对象，则重置一些变量并发送一个按下的调用祖先的事件处理程序
    if (indev_obj_act != proc->types.pointer.act_obj) {
        proc->types.pointer.last_point.x = proc->types.pointer.act_point.x;
        proc->types.pointer.last_point.y = proc->types.pointer.act_point.y;

        // 如果一个新对象发现前一个丢失了，那么发送一个调用祖先的事件处理程序
        if (proc->types.pointer.act_obj != NULL) {
            // 保存obj因为在特殊情况下`act_obj`可以在调用祖先的事件处理函数中改变
            lv_obj_t *last_obj = proc->types.pointer.act_obj;

            lv_event_send(last_obj, LV_EVENT_PRESS_LOST, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }
        }

        // 保存按下的对象
        proc->types.pointer.act_obj = indev_obj_act;
        proc->types.pointer.last_obj = indev_obj_act;

        if (indev_obj_act != NULL) {
            // 保存obj按下时计算长按时间的时间
            proc->pr_timestamp = lv_tick_get();
            proc->long_pr_sent = 0;
            proc->types.pointer.scroll_sum.x = 0;
            proc->types.pointer.scroll_sum.y = 0;
            proc->types.pointer.scroll_dir = LV_DIR_NONE;
            proc->types.pointer.gesture_dir = LV_DIR_NONE;
            proc->types.pointer.gesture_sent = 0;
            proc->types.pointer.gesture_sum.x = 0;
            proc->types.pointer.gesture_sum.y = 0;
            proc->types.pointer.vect.x = 0;
            proc->types.pointer.vect.y = 0;

            // 调用有关新闻的祖先的事件处理程序
            lv_event_send(indev_obj_act, LV_EVENT_PRESSED, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }

            if (indev_act->proc.wait_until_release) {
                return;
            }

            // 处理焦点
            indev_click_focus(&indev_act->proc);
            if (indev_reset_check(proc)) {
                return;
            }
        }
    }

    // 计算向量并应用低通滤波器：新值 = 0.5 * old_value + 0.5 * new_value
    proc->types.pointer.vect.x = proc->types.pointer.act_point.x - proc->types.pointer.last_point.x;
    proc->types.pointer.vect.y = proc->types.pointer.act_point.y - proc->types.pointer.last_point.y;

    proc->types.pointer.scroll_throw_vect.x = (proc->types.pointer.scroll_throw_vect.x + proc->types.pointer.vect.x) / 2;
    proc->types.pointer.scroll_throw_vect.y = (proc->types.pointer.scroll_throw_vect.y + proc->types.pointer.vect.y) / 2;

    proc->types.pointer.scroll_throw_vect_ori = proc->types.pointer.scroll_throw_vect;

    if (indev_obj_act) {
        lv_event_send(indev_obj_act, LV_EVENT_PRESSING, indev_act);
        if (indev_reset_check(proc)) {
            return;
        }

        if (indev_act->proc.wait_until_release) {
            return;
        }

        _lv_indev_scroll_handler(proc);
        if (indev_reset_check(proc)) {
            return;
        }

        indev_gesture(proc);
        if (indev_reset_check(proc)) {
            return;
        }

        // 如果没有滚动，则检查长按时间
        if ((proc->types.pointer.scroll_obj == NULL) && (proc->long_pr_sent == 0)) {
            // 如果经过了足够的时间，则调用祖先的事件处理程序关于长按
            if (lv_tick_elaps(proc->pr_timestamp) > indev_act->driver->long_press_time) {
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                if (indev_reset_check(proc)) {
                    return;
                }

                // 将调用祖先的事件处理程序发送标记为不再发送
                proc->long_pr_sent = 1;

                // 保存长按重复处理程序的长按时间戳
                proc->longpr_rep_timestamp = lv_tick_get();
            }
        }

        // 发送长按重复调用祖先的事件处理程序
        if ((proc->types.pointer.scroll_obj == NULL) && (proc->long_pr_sent == 1)) {
            // 如果经过足够的时间，则调用祖先的事件处理程序关于长按重复
            if (lv_tick_elaps(proc->longpr_rep_timestamp) > indev_act->driver->long_press_repeat_time) {
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, indev_act);
                if (indev_reset_check(proc)) {
                    return;
                }

                proc->longpr_rep_timestamp = lv_tick_get();
            }
        }
    }
}

static void indev_proc_release(_lv_indev_proc_t *proc)
{
    if (proc->wait_until_release != 0) {
        lv_event_send(proc->types.pointer.act_obj, LV_EVENT_PRESS_LOST, indev_act);
        if (indev_reset_check(proc)) {
            return;
        }

        proc->types.pointer.act_obj  = NULL;
        proc->types.pointer.last_obj = NULL;
        proc->pr_timestamp = 0;
        proc->longpr_rep_timestamp = 0;
        proc->wait_until_release = 0;
    }

    indev_obj_act = proc->types.pointer.act_obj;
    lv_obj_t *scroll_obj = proc->types.pointer.scroll_obj;

    // 忘记行为obj并发送一个释放的调用祖先的事件处理程序
    if (indev_obj_act) {
        LV_LOG_INFO("released");

        // 发送RELEASE调用祖先的事件处理程序和事件
        lv_event_send(indev_obj_act, LV_EVENT_RELEASED, indev_act);
        if (indev_reset_check(proc)) {
            return;
        }

        // 如果没有滚动，则发送CLICK
        if (scroll_obj == NULL) {
            if (proc->long_pr_sent == 0) {
                lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                if (indev_reset_check(proc)) {
                    return;
                }
            }

            lv_event_send(indev_obj_act, LV_EVENT_CLICKED, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }
        } else {
            lv_event_send(scroll_obj, LV_EVENT_SCROLL_THROW_BEGIN, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }
        }

        proc->types.pointer.act_obj = NULL;
        proc->pr_timestamp = 0;
        proc->longpr_rep_timestamp = 0;
    }

    if (scroll_obj) {
        _lv_indev_scroll_throw_handler(proc);
        if (indev_reset_check(proc)) {
            return;
        }
    }
}

static void indev_proc_reset_query_handler(lv_indev_t *indev)
{
    if (indev->proc.reset_query) {
        indev->proc.types.pointer.act_obj = NULL;
        indev->proc.types.pointer.last_obj = NULL;
        indev->proc.types.pointer.scroll_obj = NULL;
        indev->proc.long_pr_sent = 0;
        indev->proc.pr_timestamp = 0;
        indev->proc.longpr_rep_timestamp = 0;
        indev->proc.types.pointer.scroll_sum.x = 0;
        indev->proc.types.pointer.scroll_sum.y = 0;
        indev->proc.types.pointer.scroll_dir = LV_DIR_NONE;
        indev->proc.types.pointer.scroll_throw_vect.x = 0;
        indev->proc.types.pointer.scroll_throw_vect.y = 0;
        indev->proc.types.pointer.gesture_sum.x = 0;
        indev->proc.types.pointer.gesture_sum.y = 0;
        indev->proc.reset_query = 0;
        indev_obj_act = NULL;
    }
}

static void indev_click_focus(_lv_indev_proc_t *proc)
{
    if ((lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_CLICK_FOCUSABLE) == false) || (proc->types.pointer.last_pressed == indev_obj_act)) {
        return;
    }

    lv_group_t *g_act = (lv_group_t *)lv_obj_get_group(indev_obj_act);
    lv_group_t *g_prev = proc->types.pointer.last_pressed ? (lv_group_t *)lv_obj_get_group(proc->types.pointer.last_pressed) : NULL;

    // 如果既是最后又是行为。对象。在同一个组中(或没有组)
    if (g_act == g_prev) {
        // 对象在一个组中
        if (g_act) {
            lv_group_focus_obj(indev_obj_act);
            if (indev_reset_check(proc)) {
                return;
            }
        } else {
            if (proc->types.pointer.last_pressed) {
                lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
                if (indev_reset_check(proc)) {
                    return;
                }
            }

            lv_event_send(indev_obj_act, LV_EVENT_FOCUSED, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }
        }
    } else {
        if ((g_prev == NULL) && (proc->types.pointer.last_pressed)) {
            lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }
        } else {
            if (proc->types.pointer.last_pressed) {
                if (g_prev == NULL) {
                    lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
                    if (indev_reset_check(proc)) {
                        return;
                    }
                } else {
                    lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_LEAVE, indev_act);
                    if (indev_reset_check(proc)) {
                        return;
                    }
                }
            }
        }

        if (g_act) {
            lv_group_focus_obj(indev_obj_act);
            if (indev_reset_check(proc)) {
                return;
            }
        } else {
            lv_event_send(indev_obj_act, LV_EVENT_FOCUSED, indev_act);
            if (indev_reset_check(proc)) {
                return;
            }
        }
    }

    proc->types.pointer.last_pressed = indev_obj_act;
}

void indev_gesture(_lv_indev_proc_t *proc)
{
    if (proc->types.pointer.scroll_obj) {
        return;
    }

    if (proc->types.pointer.gesture_sent) {
        return;
    }

    lv_obj_t *gesture_obj = proc->types.pointer.act_obj;

    // 如果手势父项处于活动状态，则递归检查手势属性
    while (gesture_obj && lv_obj_has_flag(gesture_obj, LV_OBJ_FLAG_GESTURE_BUBBLE)) {
        gesture_obj = lv_obj_get_parent(gesture_obj);
    }

    if (gesture_obj == NULL) {
        return;
    }

    if ((LV_ABS(proc->types.pointer.vect.x) < indev_act->driver->gesture_min_velocity) && (LV_ABS(proc->types.pointer.vect.y) < indev_act->driver->gesture_min_velocity)) {
        proc->types.pointer.gesture_sum.x = 0;
        proc->types.pointer.gesture_sum.y = 0;
    }

    // 通过手势计算运动
    proc->types.pointer.gesture_sum.x += proc->types.pointer.vect.x;
    proc->types.pointer.gesture_sum.y += proc->types.pointer.vect.y;

    if ((LV_ABS(proc->types.pointer.gesture_sum.x) > indev_act->driver->gesture_limit) || (LV_ABS(proc->types.pointer.gesture_sum.y) > indev_act->driver->gesture_limit)) {
        proc->types.pointer.gesture_sent = 1;

        if (LV_ABS(proc->types.pointer.gesture_sum.x) > LV_ABS(proc->types.pointer.gesture_sum.y)) {
            if (proc->types.pointer.gesture_sum.x > 0) {
                proc->types.pointer.gesture_dir = LV_DIR_RIGHT;
            } else {
                proc->types.pointer.gesture_dir = LV_DIR_LEFT;
            }
        } else {
            if (proc->types.pointer.gesture_sum.y > 0) {
                proc->types.pointer.gesture_dir = LV_DIR_BOTTOM;
            } else {
                proc->types.pointer.gesture_dir = LV_DIR_TOP;
            }
        }

        lv_event_send(gesture_obj, LV_EVENT_GESTURE, indev_act);
        if (indev_reset_check(proc)) {
            return;
        }
    }
}

static bool indev_reset_check(_lv_indev_proc_t *proc)
{
    if (proc->reset_query) {
        indev_obj_act = NULL;
    }

    return proc->reset_query ? true : false;
}
