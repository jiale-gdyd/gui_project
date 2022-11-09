#include "lv_indev.h"
#include "lv_indev_scroll.h"

// 由于这个因素，在弹性部件上滚动速度较慢
#define ELASTIC_SLOWNESS_FACTOR         (4)

static void init_scroll_limits(_lv_indev_proc_t *proc);
static lv_obj_t *find_scroll_obj(_lv_indev_proc_t *proc);
static lv_coord_t scroll_throw_predict_y(_lv_indev_proc_t *proc);
static lv_coord_t scroll_throw_predict_x(_lv_indev_proc_t *proc);
static void scroll_limit_diff(_lv_indev_proc_t *proc, lv_coord_t *diff_x, lv_coord_t *diff_y);
static lv_coord_t find_snap_point_x(const lv_obj_t *obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs);
static lv_coord_t find_snap_point_y(const lv_obj_t *obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs);
static lv_coord_t elastic_diff(lv_obj_t *scroll_obj, lv_coord_t diff, lv_coord_t scroll_start, lv_coord_t scroll_end, lv_dir_t dir);

void _lv_indev_scroll_handler(_lv_indev_proc_t *proc)
{
    lv_obj_t *scroll_obj = proc->types.pointer.scroll_obj;

    // 如果还没有滚动对象，请尝试找到一个
    if (scroll_obj == NULL) {
        proc->types.pointer.scroll_sum.x += proc->types.pointer.vect.x;
        proc->types.pointer.scroll_sum.y += proc->types.pointer.vect.y;

        scroll_obj = find_scroll_obj(proc);
        if (scroll_obj == NULL) {
            return;
        }

        init_scroll_limits(proc);
        lv_event_send(scroll_obj, LV_EVENT_SCROLL_BEGIN, NULL);

        if (proc->reset_query) {
            return;
        }
    }

    // 如果向量不为零，则设置新位置或滚动
    if ((proc->types.pointer.vect.x != 0) || (proc->types.pointer.vect.y != 0)) {
        lv_coord_t diff_x = 0;
        lv_coord_t diff_y = 0;

        if (proc->types.pointer.scroll_dir == LV_DIR_HOR) {
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);
            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            diff_x = elastic_diff(scroll_obj, proc->types.pointer.vect.x, sl, sr, LV_DIR_HOR);
        } else {
            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);
            diff_y = elastic_diff(scroll_obj, proc->types.pointer.vect.y, st, sb, LV_DIR_VER);
        }

        lv_dir_t scroll_dir = lv_obj_get_scroll_dir(scroll_obj);
        if (((scroll_dir & LV_DIR_LEFT) == 0) && (diff_x > 0)) {
            diff_x = 0;
        }

        if (((scroll_dir & LV_DIR_RIGHT) == 0) && (diff_x < 0)) {
            diff_x = 0;
        }

        if (((scroll_dir & LV_DIR_TOP) == 0) && (diff_y > 0)) {
            diff_y = 0;
        }

        if (((scroll_dir & LV_DIR_BOTTOM) == 0) && (diff_y < 0)) {
            diff_y = 0;
        }

        // 尊重滚动限制区域
        scroll_limit_diff(proc, &diff_x, &diff_y);

        _lv_obj_scroll_by_raw(scroll_obj, diff_x, diff_y);
        if (proc->reset_query) {
            return;
        }
        proc->types.pointer.scroll_sum.x += diff_x;
        proc->types.pointer.scroll_sum.y += diff_y;
    }
}

void _lv_indev_scroll_throw_handler(_lv_indev_proc_t *proc)
{
    lv_obj_t *scroll_obj = proc->types.pointer.scroll_obj;
    if (scroll_obj == NULL) {
        return;
    }

    if (proc->types.pointer.scroll_dir == LV_DIR_NONE) {
        return;
    }

    lv_indev_t *indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw = indev_act->driver->scroll_throw;

    if (lv_obj_has_flag(scroll_obj, LV_OBJ_FLAG_SCROLL_MOMENTUM) == false) {
        proc->types.pointer.scroll_throw_vect.y = 0;
        proc->types.pointer.scroll_throw_vect.x = 0;
    }

    lv_scroll_snap_t align_x = lv_obj_get_scroll_snap_x(scroll_obj);
    lv_scroll_snap_t align_y = lv_obj_get_scroll_snap_y(scroll_obj);

    if (proc->types.pointer.scroll_dir == LV_DIR_VER) {
        proc->types.pointer.scroll_throw_vect.x = 0;
        // 如果没有捕捉抛出
        if (align_y == LV_SCROLL_SNAP_NONE) {
            proc->types.pointer.scroll_throw_vect.y = proc->types.pointer.scroll_throw_vect.y * (100 - scroll_throw) / 100;

            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);
            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);

            proc->types.pointer.scroll_throw_vect.y = elastic_diff(scroll_obj, proc->types.pointer.scroll_throw_vect.y, st, sb, LV_DIR_VER);

            _lv_obj_scroll_by_raw(scroll_obj, 0,  proc->types.pointer.scroll_throw_vect.y);
            if (proc->reset_query) {
                return;
            }
        } else {    // 通过捕捉找到最近的捕捉点并在那里滚动
            lv_coord_t diff_y = scroll_throw_predict_y(proc);
            proc->types.pointer.scroll_throw_vect.y = 0;
            scroll_limit_diff(proc, NULL, &diff_y);
            lv_coord_t y = find_snap_point_y(scroll_obj, LV_COORD_MIN, LV_COORD_MAX, diff_y);
            lv_obj_scroll_by(scroll_obj, 0, diff_y + y, LV_ANIM_ON);
            if (proc->reset_query) {
                return;
            }
        }
    } else if (proc->types.pointer.scroll_dir == LV_DIR_HOR) {
        proc->types.pointer.scroll_throw_vect.y = 0;
        // 如果没有捕捉抛出
        if (align_x == LV_SCROLL_SNAP_NONE) {
            proc->types.pointer.scroll_throw_vect.x = proc->types.pointer.scroll_throw_vect.x * (100 - scroll_throw) / 100;

            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);

            proc->types.pointer.scroll_throw_vect.x = elastic_diff(scroll_obj, proc->types.pointer.scroll_throw_vect.x, sl ,sr, LV_DIR_HOR);

            _lv_obj_scroll_by_raw(scroll_obj, proc->types.pointer.scroll_throw_vect.x, 0);
            if (proc->reset_query) {
                return;
            }
        } else {    // 通过捕捉找到最近的捕捉点并在那里滚动
            lv_coord_t diff_x = scroll_throw_predict_x(proc);
            proc->types.pointer.scroll_throw_vect.x = 0;
            scroll_limit_diff(proc, &diff_x, NULL);
            lv_coord_t x = find_snap_point_x(scroll_obj, LV_COORD_MIN, LV_COORD_MAX, diff_x);
            lv_obj_scroll_by(scroll_obj, x + diff_x, 0, LV_ANIM_ON);
            if (proc->reset_query) {
                return;
            }
        }
    }

    // 检查滚动是否完成
    if ((proc->types.pointer.scroll_throw_vect.x == 0) && (proc->types.pointer.scroll_throw_vect.y == 0)) {
        // 如果滚动则还原。如果垂直滚动且不受snap控制
        if (align_y == LV_SCROLL_SNAP_NONE) {
            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);

            if ((st > 0) || (sb > 0)) {
                if (st < 0) {
                    lv_obj_scroll_by(scroll_obj, 0, st, LV_ANIM_ON);
                    if (proc->reset_query) {
                        return;
                    }
                } else if (sb < 0) {
                    lv_obj_scroll_by(scroll_obj, 0, -sb, LV_ANIM_ON);
                    if (proc->reset_query) {
                        return;
                    }
                }
            }
        }

        // 如果水平滚动且不受snap控制
        if (align_x == LV_SCROLL_SNAP_NONE) {
            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);

            if ((sl > 0) || (sr > 0)) {
                if (sl < 0) {
                    lv_obj_scroll_by(scroll_obj, sl, 0, LV_ANIM_ON);
                    if (proc->reset_query) {
                        return;
                    }
                } else if (sr < 0) {
                    lv_obj_scroll_by(scroll_obj, -sr, 0, LV_ANIM_ON);
                    if (proc->reset_query) {
                        return;
                    }
                }
            }
        }

        lv_event_send(scroll_obj, LV_EVENT_SCROLL_END, indev_act);
        if (proc->reset_query) {
            return;
        }

        proc->types.pointer.scroll_dir = LV_DIR_NONE;
        proc->types.pointer.scroll_obj = NULL;
    }
}

lv_coord_t lv_indev_scroll_throw_predict(lv_indev_t *indev, lv_dir_t dir)
{
    if (indev == NULL) {
        return 0;
    }

    lv_coord_t v;

    switch (dir) {
        case LV_DIR_VER:
            v = indev->proc.types.pointer.scroll_throw_vect_ori.y;
            break;

        case LV_DIR_HOR:
            v = indev->proc.types.pointer.scroll_throw_vect_ori.x;
            break;

        default:
            return 0;
    }

    lv_coord_t sum = 0;
    lv_coord_t scroll_throw = indev->driver->scroll_throw;

    while (v) {
        sum += v;
        v = v * (100 - scroll_throw) / 100;
    }

    return sum;
}

void lv_indev_scroll_get_snap_dist(lv_obj_t *obj, lv_point_t *p)
{
    p->x = find_snap_point_x(obj, obj->coords.x1, obj->coords.x2, 0);
    p->y = find_snap_point_y(obj, obj->coords.y1, obj->coords.y2, 0);
}

static lv_obj_t *find_scroll_obj(_lv_indev_proc_t *proc)
{
    bool hor_en = false;
    bool ver_en = false;
    lv_obj_t *obj_candidate = NULL;
    lv_dir_t dir_candidate = LV_DIR_NONE;
    lv_indev_t *indev_act = lv_indev_get_act();
    lv_coord_t scroll_limit =  indev_act->driver->scroll_limit;

    lv_obj_t *obj_act = proc->types.pointer.act_obj;

    if (LV_ABS(proc->types.pointer.scroll_sum.x) > LV_ABS(proc->types.pointer.scroll_sum.y)) {
        hor_en = true;
    } else {
        ver_en = true;
    }

    while (obj_act) {
        if (lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLLABLE) == false) {
            if ((lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_HOR) == false) && hor_en) {
                break;
            }

            if ((lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_VER) == false) && ver_en) {
                break;
            }

            obj_act = lv_obj_get_parent(obj_act);
            continue;
        }

        // 根据当前方向考虑上下或左右滚动
        bool up_en = ver_en;
        bool down_en = ver_en;
        bool left_en = hor_en;
        bool right_en = hor_en;

        // 该对象可能已禁用某些方向
        lv_dir_t scroll_dir = lv_obj_get_scroll_dir(obj_act);
        if ((scroll_dir & LV_DIR_LEFT) == 0) {
            left_en = false;
        }

        if ((scroll_dir & LV_DIR_RIGHT) == 0) {
            right_en = false;
        }

        if ((scroll_dir & LV_DIR_TOP) == 0) {
            up_en = false;
        }

        if ((scroll_dir & LV_DIR_BOTTOM) == 0) {
            down_en = false;
        }

        // 如果对象的内容在该方向溢出，则该对象可滚动到该方向
        lv_coord_t st = lv_obj_get_scroll_top(obj_act);
        lv_coord_t sb = lv_obj_get_scroll_bottom(obj_act);
        lv_coord_t sl = lv_obj_get_scroll_left(obj_act);
        lv_coord_t sr = lv_obj_get_scroll_right(obj_act);

        if (((st > 0) || (sb > 0))
            && ((up_en && (proc->types.pointer.scroll_sum.y >= scroll_limit))
            || (down_en && (proc->types.pointer.scroll_sum.y <= -scroll_limit))))
        {
            obj_candidate = obj_act;
            dir_candidate = LV_DIR_VER;
        }

        if (((sl > 0) || (sr > 0)) 
            && ((left_en && (proc->types.pointer.scroll_sum.x >= scroll_limit))
            || (right_en && (proc->types.pointer.scroll_sum.x <= -scroll_limit))))
        {
            obj_candidate = obj_act;
            dir_candidate = LV_DIR_HOR;
        }

        if (st <= 0) {
            up_en = false;
        }

        if (sb <= 0) {
            down_en = false;
        }

        if (sl <= 0) {
            left_en = false;
        }

        if (sr <= 0) {
            right_en = false;
        }

        // 如果对象确实可以滚动到当前方向，请使用它
        if ((left_en && (proc->types.pointer.scroll_sum.x >= scroll_limit))
            || (right_en && (proc->types.pointer.scroll_sum.x <= -scroll_limit))
            || (up_en && (proc->types.pointer.scroll_sum.y >= scroll_limit))
            || (down_en && (proc->types.pointer.scroll_sum.y <= -scroll_limit)))
        {
            proc->types.pointer.scroll_dir = hor_en ? LV_DIR_HOR : LV_DIR_VER;
            break;
        }

        // 如果此对象不想链接父级的滚动，则停止搜索
        if ((lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_HOR) == false) && hor_en) {
            break;
        }

        if ((lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_VER) == false) && ver_en) {
            break;
        }

        obj_act = lv_obj_get_parent(obj_act);
    }

    // 使用最后一个候选
    if (obj_candidate) {
        proc->types.pointer.scroll_dir = dir_candidate;
        proc->types.pointer.scroll_obj = obj_candidate;
        proc->types.pointer.scroll_sum.x = 0;
        proc->types.pointer.scroll_sum.y = 0;
    }

    return obj_candidate;
}

static void init_scroll_limits(_lv_indev_proc_t *proc)
{
    lv_obj_t *obj = proc->types.pointer.scroll_obj;

    // 如果没有STOP允许滚动到任何地方
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ONE) == false) {
        lv_area_set(&proc->types.pointer.scroll_area, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MAX, LV_COORD_MAX);
    } else {    // 使用STOP限制滚动到perv和next快照点
        switch (lv_obj_get_scroll_snap_y(obj)) {
            case LV_SCROLL_SNAP_START:
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, obj->coords.y1 + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, obj->coords.y1 - 1, 0);
                break;

            case LV_SCROLL_SNAP_END:
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, obj->coords.y2, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, obj->coords.y2, 0);
                break;

            case LV_SCROLL_SNAP_CENTER: {
                lv_coord_t y_mid = obj->coords.y1 + lv_area_get_height(&obj->coords) / 2;
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, y_mid + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, y_mid - 1, 0);
                break;
            }

            default:
                proc->types.pointer.scroll_area.y1 = LV_COORD_MIN;
                proc->types.pointer.scroll_area.y2 = LV_COORD_MAX;
                break;
        }

        switch (lv_obj_get_scroll_snap_x(obj)) {
            case LV_SCROLL_SNAP_START:
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, obj->coords.x1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, obj->coords.x1, 0);
                break;

            case LV_SCROLL_SNAP_END:
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, obj->coords.x2, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, obj->coords.x2, 0);
                break;

            case LV_SCROLL_SNAP_CENTER: {
                lv_coord_t x_mid = obj->coords.x1 + lv_area_get_width(&obj->coords) / 2;
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, x_mid + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, x_mid - 1, 0);
                break;
            }

            default:
                proc->types.pointer.scroll_area.x1 = LV_COORD_MIN;
                proc->types.pointer.scroll_area.x2 = LV_COORD_MAX;
                break;
        }
    }

    // 允许在边缘滚动。无论如何它都会因为捕捉而恢复到边缘
    if (proc->types.pointer.scroll_area.x1 == 0) {
        proc->types.pointer.scroll_area.x1 = LV_COORD_MIN;
    }

    if (proc->types.pointer.scroll_area.x2 == 0) {
        proc->types.pointer.scroll_area.x2 = LV_COORD_MAX;
    }

    if (proc->types.pointer.scroll_area.y1 == 0) {
        proc->types.pointer.scroll_area.y1 = LV_COORD_MIN;
    }

    if (proc->types.pointer.scroll_area.y2 == 0) {
        proc->types.pointer.scroll_area.y2 = LV_COORD_MAX;
    }
}

static lv_coord_t find_snap_point_x(const lv_obj_t *obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs)
{
    lv_scroll_snap_t align = lv_obj_get_scroll_snap_x(obj);
    if (align == LV_SCROLL_SNAP_NONE) {
        return 0;
    }

    lv_coord_t dist = LV_COORD_MAX;
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

    uint32_t i;
    for (i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t *child = obj->spec_attr->children[i];
        if (lv_obj_has_flag_any(child, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) {
            continue;
        }

        if (lv_obj_has_flag(child, LV_OBJ_FLAG_SNAPPABLE)) {
            lv_coord_t x_child = 0;
            lv_coord_t x_parent = 0;

            switch (align) {
                case LV_SCROLL_SNAP_START:
                    x_child = child->coords.x1;
                    x_parent = obj->coords.x1 + pad_left;
                    break;
        
                case LV_SCROLL_SNAP_END:
                    x_child = child->coords.x2;
                    x_parent = obj->coords.x2 - pad_right;
                    break;

                case LV_SCROLL_SNAP_CENTER:
                    x_child = child->coords.x1 + lv_area_get_width(&child->coords) / 2;
                    x_parent = obj->coords.x1 + pad_left + (lv_area_get_width(&obj->coords) - pad_left - pad_right) / 2;
                    break;

                default:
                    continue;
            }

            x_child += ofs;
            if ((x_child >= min) && (x_child <= max)) {
                lv_coord_t x = x_child - x_parent;
                if (LV_ABS(x) < LV_ABS(dist)) {
                    dist = x;
                }
            }
        }
    }

    return dist == LV_COORD_MAX ? 0: -dist;
}

static lv_coord_t find_snap_point_y(const lv_obj_t *obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs)
{
    lv_scroll_snap_t align = lv_obj_get_scroll_snap_y(obj);
    if (align == LV_SCROLL_SNAP_NONE) {
        return 0;
    }

    lv_coord_t dist = LV_COORD_MAX;
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    uint32_t i;
    for (i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t *child = obj->spec_attr->children[i];
        if (lv_obj_has_flag_any(child, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) {
            continue;
        }

        if (lv_obj_has_flag(child, LV_OBJ_FLAG_SNAPPABLE)) {
            lv_coord_t y_child = 0;
            lv_coord_t y_parent = 0;

            switch (align) {
                case LV_SCROLL_SNAP_START:
                    y_child = child->coords.y1;
                    y_parent = obj->coords.y1 + pad_top;
                    break;

                case LV_SCROLL_SNAP_END:
                    y_child = child->coords.y2;
                    y_parent = obj->coords.y2 - pad_bottom;
                    break;

                case LV_SCROLL_SNAP_CENTER:
                    y_child = child->coords.y1 + lv_area_get_height(&child->coords) / 2;
                    y_parent = obj->coords.y1 + pad_top + (lv_area_get_height(&obj->coords) - pad_top - pad_bottom) / 2;
                    break;

                default:
                    continue;
            }

            y_child += ofs;
            if ((y_child >= min) && (y_child <= max)) {
                lv_coord_t y = y_child - y_parent;
                if (LV_ABS(y) < LV_ABS(dist)) {
                    dist = y;
                }
            }
        }
    }

    return dist == LV_COORD_MAX ? 0 : -dist;
}

static void scroll_limit_diff(_lv_indev_proc_t *proc, lv_coord_t *diff_x, lv_coord_t *diff_y)
{
    if (diff_y) {
        if ((proc->types.pointer.scroll_sum.y + *diff_y) < proc->types.pointer.scroll_area.y1) {
           *diff_y = proc->types.pointer.scroll_area.y1 - proc->types.pointer.scroll_sum.y;
        }

        if ((proc->types.pointer.scroll_sum.y + *diff_y) > proc->types.pointer.scroll_area.y2) {
            *diff_y = proc->types.pointer.scroll_area.y2 - proc->types.pointer.scroll_sum.y;
        }
    }

    if (diff_x) {
        if ((proc->types.pointer.scroll_sum.x + *diff_x) < proc->types.pointer.scroll_area.x1) {
            *diff_x = proc->types.pointer.scroll_area.x1 - proc->types.pointer.scroll_sum.x;
        }

        if ((proc->types.pointer.scroll_sum.x + *diff_x) > proc->types.pointer.scroll_area.x2) {
            *diff_x = proc->types.pointer.scroll_area.x2 - proc->types.pointer.scroll_sum.x;
        }
    }
}

static lv_coord_t scroll_throw_predict_y(_lv_indev_proc_t *proc)
{
    lv_coord_t move = 0;
    lv_coord_t y = proc->types.pointer.scroll_throw_vect.y;

    lv_indev_t *indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw = indev_act->driver->scroll_throw;

    while (y) {
        move += y;
        y = y * (100 - scroll_throw) / 100;
    }

    return move;
}

static lv_coord_t scroll_throw_predict_x(_lv_indev_proc_t *proc)
{
    lv_coord_t move = 0;
    lv_coord_t x =  proc->types.pointer.scroll_throw_vect.x;

    lv_indev_t *indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw =  indev_act->driver->scroll_throw;

    while (x) {
        move += x;
        x = x * (100 - scroll_throw) / 100;
    }

    return move;
}

static lv_coord_t elastic_diff(lv_obj_t *scroll_obj, lv_coord_t diff, lv_coord_t scroll_start, lv_coord_t scroll_end, lv_dir_t dir)
{
    if (lv_obj_has_flag(scroll_obj, LV_OBJ_FLAG_SCROLL_ELASTIC)) {
        // 如果在当前方向有对齐，请不要使用弹性系数，因为第一个和最后一个项目很自然地滚动(对齐)
        lv_scroll_snap_t snap;
        snap = dir == LV_DIR_HOR ? lv_obj_get_scroll_snap_x(scroll_obj) : lv_obj_get_scroll_snap_y(scroll_obj);

        lv_coord_t snap_point = 0;
        lv_coord_t act_obj_point = 0;
        lv_obj_t *act_obj = lv_indev_get_obj_act();

        if (dir == LV_DIR_HOR) {
            lv_coord_t pad_left = lv_obj_get_style_pad_left(scroll_obj, LV_PART_MAIN);
            lv_coord_t pad_right = lv_obj_get_style_pad_right(scroll_obj, LV_PART_MAIN);

            switch (snap) {
                case LV_SCROLL_SNAP_CENTER:
                    snap_point = pad_left + (lv_area_get_width(&scroll_obj->coords) - pad_left - pad_right) / 2 + scroll_obj->coords.x1;
                    act_obj_point = lv_area_get_width(&act_obj->coords) / 2 + act_obj->coords.x1;
                    break;

                case LV_SCROLL_SNAP_START:
                    snap_point = scroll_obj->coords.x1 + pad_left;
                    act_obj_point = act_obj->coords.x1;
                    break;

                case LV_SCROLL_SNAP_END:
                    snap_point = scroll_obj->coords.x2 - pad_right;
                    act_obj_point = act_obj->coords.x2;
                    break;
            }
        } else {
            lv_coord_t pad_top = lv_obj_get_style_pad_top(scroll_obj, LV_PART_MAIN);
            lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(scroll_obj, LV_PART_MAIN);

            switch (snap) {
                case LV_SCROLL_SNAP_CENTER:
                    snap_point = pad_top + (lv_area_get_height(&scroll_obj->coords) - pad_top - pad_bottom) / 2 + scroll_obj->coords.y1;
                    act_obj_point = lv_area_get_height(&act_obj->coords) / 2 + act_obj->coords.y1;
                    break;

                case LV_SCROLL_SNAP_START:
                    snap_point = scroll_obj->coords.y1 + pad_top;
                    act_obj_point = act_obj->coords.y1;
                    break;

                case LV_SCROLL_SNAP_END:
                    snap_point = scroll_obj->coords.y2 - pad_bottom;
                    act_obj_point = act_obj->coords.y2;
                    break;
            }
        }

        if (scroll_end < 0) {
            if ((snap != LV_SCROLL_SNAP_NONE) && (act_obj_point > snap_point)) {
                return diff;
            }

            // 四舍五入
            if (diff < 0) {
                diff -= ELASTIC_SLOWNESS_FACTOR / 2;
            }

            if (diff > 0) {
                diff += ELASTIC_SLOWNESS_FACTOR / 2;
            }

            return diff / ELASTIC_SLOWNESS_FACTOR;
        } else if (scroll_start < 0) {
            if ((snap != LV_SCROLL_SNAP_NONE) && (act_obj_point < snap_point)) {
                return diff;
            }

            // 四舍五入
            if (diff < 0) {
                diff -= ELASTIC_SLOWNESS_FACTOR / 2;
            }

            if (diff > 0) {
                diff += ELASTIC_SLOWNESS_FACTOR / 2;
            }

            return diff / ELASTIC_SLOWNESS_FACTOR;
        }
    } else {
        // 如果需要，滚动回边界
        if ((scroll_end + diff) < 0) {
            diff = -scroll_end;
        }

        if ((scroll_start - diff) < 0) {
            diff = scroll_start;
        }
    }

    return diff;
}
