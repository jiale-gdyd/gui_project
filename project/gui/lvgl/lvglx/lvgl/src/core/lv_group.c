#include <stddef.h>

#include "lv_group.h"
#include "../misc/lv_gc.h"
#include "../core/lv_obj.h"
#include "../core/lv_indev.h"

static lv_group_t *default_group;

static void lv_group_refocus(lv_group_t *g);
static lv_indev_t *get_indev(const lv_group_t *g);
static bool focus_next_core(lv_group_t *group, void *(*begin)(const lv_ll_t *), void *(*move)(const lv_ll_t *, const void *));

void _lv_group_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_group_ll), sizeof(lv_group_t));
}

lv_group_t *lv_group_create(void)
{
    lv_group_t *group = (lv_group_t *)_lv_ll_ins_head(&LV_GC_ROOT(_lv_group_ll));
    LV_ASSERT_MALLOC(group);
    if (group == NULL) {
        return NULL;
    }

    _lv_ll_init(&group->obj_ll, sizeof(lv_obj_t *));

    group->obj_focus = NULL;
    group->frozen = 0;
    group->focus_cb = NULL;
    group->edge_cb = NULL;
    group->editing = 0;
    group->refocus_policy = LV_GROUP_REFOCUS_POLICY_PREV;
    group->wrap = 1;

#if LV_USE_USER_DATA
    group->user_data = NULL;
#endif

    return group;
}

void lv_group_del(lv_group_t *group)
{
    // 散焦当前聚焦的对象
    if (group->obj_focus != NULL) {
        lv_event_send(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        lv_obj_invalidate(*group->obj_focus);
    }

    // 从组中删除对象
    lv_obj_t **obj;
    // _LV_LL_READ(&group->obj_ll, obj) {
    for (obj = (lv_obj_t **)_lv_ll_get_head(&group->obj_ll); obj != NULL; obj = (lv_obj_t **)_lv_ll_get_next(&group->obj_ll, obj)) {
        if ((*obj)->spec_attr) {
            (*obj)->spec_attr->group_p = NULL;
        }
    }

    // 从任何indev设备中删除组
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev) {
        if (indev->group == group) {
            lv_indev_set_group(indev, NULL);
        }

        indev = lv_indev_get_next(indev);
    }

    if (group == lv_group_get_default()) {
        lv_group_set_default(NULL);
    }

    _lv_ll_clear(&(group->obj_ll));
    _lv_ll_remove(&LV_GC_ROOT(_lv_group_ll), group);
    lv_free(group);
}

void lv_group_set_default(lv_group_t *group)
{
    default_group = group;
}

lv_group_t *lv_group_get_default(void)
{
    return default_group;
}

void lv_group_add_obj(lv_group_t *group, lv_obj_t *obj)
{
    if (group == NULL) {
        return;
    }

    LV_LOG_TRACE("begin");

    lv_group_remove_obj(obj);

    // 不要两次添加对象
    lv_obj_t **obj_i;
    // _LV_LL_READ(&group->obj_ll, obj_i) {
    for (obj_i = (lv_obj_t **)_lv_ll_get_head(&group->obj_ll); obj_i != NULL; obj_i = (lv_obj_t **)_lv_ll_get_next(&group->obj_ll, obj_i)) {
        if ((*obj_i) == obj) {
            LV_LOG_INFO("the object is already added to this group");
            return;
        }
    }

    // 如果对象已经在一个组中并且被聚焦，则重新聚焦它
    lv_group_t *group_cur = (lv_group_t *)lv_obj_get_group(obj);
    if (group_cur) {
        if (obj->spec_attr->group_p && (*(obj->spec_attr->group_p->obj_focus) == obj)) {
            lv_group_refocus(group_cur);
            LV_LOG_INFO("changing object's group");
        }
    }

    if (obj->spec_attr == NULL) {
        lv_obj_allocate_spec_attr(obj);
    }
    obj->spec_attr->group_p = group;

    lv_obj_t **next = (lv_obj_t **)_lv_ll_ins_tail(&group->obj_ll);
    LV_ASSERT_MALLOC(next);
    if (next == NULL) {
        return;
    }
    *next = obj;

    // 如果头尾相等，则链表中只有一个对象。在这种情况下自动激活它
    if (_lv_ll_get_head(&group->obj_ll) == next) {
        lv_group_refocus(group);
    }

    LV_LOG_TRACE("finished");
}

void lv_group_swap_obj(lv_obj_t *obj1, lv_obj_t *obj2)
{
    lv_group_t *g1 = (lv_group_t *)lv_obj_get_group(obj1);
    lv_group_t *g2 = (lv_group_t *)lv_obj_get_group(obj2);

    if (g1 != g2) {
        return;
    }

    if (g1 == NULL) {
        return;
    }

    // 不要两次添加对象
    lv_obj_t **obj_i;
    //_LV_LL_READ(&g1->obj_ll, obj_i) {
    for (obj_i = (lv_obj_t **)_lv_ll_get_head(&g1->obj_ll); obj_i != NULL; obj_i = (lv_obj_t **)_lv_ll_get_next(&g1->obj_ll, obj_i)) {
        if ((*obj_i) == obj1) {
            (*obj_i) =  obj2;
        } else if ((*obj_i) == obj2) {
            (*obj_i) =  obj1;
        }
    }

    if (*g1->obj_focus == obj1) {
        lv_group_focus_obj(obj2);
    } else if (*g1->obj_focus == obj2) {
        lv_group_focus_obj(obj1);
    }
}

void lv_group_remove_obj(lv_obj_t *obj)
{
    lv_group_t *g = (lv_group_t *)lv_obj_get_group(obj);
    if (g == NULL) {
        return;
    }

    LV_LOG_TRACE("begin");

    // 专注于下一个对象
    if (g->obj_focus && (*g->obj_focus == obj)) {
        if (g->frozen) {
            g->frozen = 0;
        }

        // 如果这是组中唯一的对象，则不关注任何内容
        if ((_lv_ll_get_head(&g->obj_ll) == g->obj_focus) && (_lv_ll_get_tail(&g->obj_ll) == g->obj_focus)) {
            lv_event_send(*g->obj_focus, LV_EVENT_DEFOCUSED, get_indev(g));
        } else {    // 如果组中有更多对象，则关注下一个/上一个对象
            lv_group_refocus(g);
        }
    }

    // 如果焦点对象仍然相同，则它是组中唯一的对象，但将被删除。将`obj_focus`设置为NULL以返回具有零个对象的组的初始状态
    if (g->obj_focus && (*g->obj_focus == obj)) {
        g->obj_focus = NULL;
    }

    // 搜索对象并将其从其组中删除
    lv_obj_t **i;
    // _LV_LL_READ(&g->obj_ll, i) {
    for (i = (lv_obj_t **)_lv_ll_get_head(&g->obj_ll); i != NULL; i = (lv_obj_t **)_lv_ll_get_next(&g->obj_ll, i)) {
        if (*i == obj) {
            _lv_ll_remove(&g->obj_ll, i);
            lv_free(i);

            if (obj->spec_attr) {
                obj->spec_attr->group_p = NULL;
            }

            break;
        }
    }

    LV_LOG_TRACE("finished");
}

void lv_group_remove_all_objs(lv_group_t *group)
{
    // 散焦当前聚焦的对象
    if (group->obj_focus != NULL) {
        lv_event_send(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        lv_obj_invalidate(*group->obj_focus);
        group->obj_focus = NULL;
    }

    // 从组中删除对象
    lv_obj_t **obj;
    // _LV_LL_READ(&group->obj_ll, obj) {
    for (obj = (lv_obj_t **)_lv_ll_get_head(&group->obj_ll); obj != NULL; obj = (lv_obj_t **)_lv_ll_get_next(&group->obj_ll, obj)) {
        if ((*obj)->spec_attr) {
            (*obj)->spec_attr->group_p = NULL;
        }
    }

    _lv_ll_clear(&(group->obj_ll));
}

void lv_group_focus_obj(lv_obj_t *obj)
{
    if (obj == NULL) {
        return;
    }

    lv_group_t *g = (lv_group_t *)lv_obj_get_group(obj);
    if (g == NULL) {
        return;
    }

    if (g->frozen != 0) {
        return;
    }

    // 必须离开散焦编辑模式
    lv_group_set_editing(g, false);

    lv_obj_t **i;
    // _LV_LL_READ(&g->obj_ll, i) {
    for (i = (lv_obj_t **)_lv_ll_get_head(&g->obj_ll); i != NULL; i = (lv_obj_t **)_lv_ll_get_next(&g->obj_ll, i)) {
        if(*i == obj) {
            // 如果需要再次聚焦同一物体，请不要散焦
            if ((g->obj_focus != NULL) && (obj != *g->obj_focus)) {
                lv_res_t res = lv_event_send(*g->obj_focus, LV_EVENT_DEFOCUSED, get_indev(g));
                if(res != LV_RES_OK) {
                    return;
                }

                lv_obj_invalidate(*g->obj_focus);
            }

            g->obj_focus = i;

            if (g->obj_focus != NULL) {
                if (g->focus_cb) {
                    g->focus_cb(g);
                }

                lv_res_t res = lv_event_send(*g->obj_focus, LV_EVENT_FOCUSED, get_indev(g));
                if (res != LV_RES_OK) {
                    return;
                }

                lv_obj_invalidate(*g->obj_focus);
            }

            break;
        }
    }
}

void lv_group_focus_next(lv_group_t *group)
{
    bool focus_changed = focus_next_core(group, _lv_ll_get_head, _lv_ll_get_next);
    if (group->edge_cb) {
        if (!focus_changed) {
            group->edge_cb(group, true);
        }
    }
}

void lv_group_focus_prev(lv_group_t *group)
{
    bool focus_changed = focus_next_core(group, _lv_ll_get_tail, _lv_ll_get_prev);
    if (group->edge_cb) {
        if (!focus_changed) {
            group->edge_cb(group, false);
        }
    }
}

void lv_group_focus_freeze(lv_group_t *group, bool en)
{
    if (en == false) {
        group->frozen = 0;
    } else {
        group->frozen = 1;
    }
}

lv_res_t lv_group_send_data(lv_group_t *group, uint32_t c)
{
    lv_obj_t *act = lv_group_get_focused(group);
    if (act == NULL) {
        return LV_RES_OK;
    }

    if (lv_obj_has_state(act, LV_STATE_DISABLED)) {
        return LV_RES_OK;
    }

    return lv_event_send(act, LV_EVENT_KEY, &c);
}

void lv_group_set_focus_cb(lv_group_t *group, lv_group_focus_cb_t focus_cb)
{
    group->focus_cb = focus_cb;
}

void lv_group_set_edge_cb(lv_group_t *group, lv_group_edge_cb_t edge_cb)
{
    group->edge_cb = edge_cb;
}

void lv_group_set_editing(lv_group_t *group, bool edit)
{
    if (group == NULL) {
        return;
    }

    uint8_t en_val = edit ? 1 : 0;

    // 不要再次设置相同的模式
    if (en_val == group->editing) {
        return;
    }

    group->editing = en_val;
    lv_obj_t *focused = lv_group_get_focused(group);

    if (focused) {
        lv_res_t res = lv_event_send(*group->obj_focus, LV_EVENT_FOCUSED, get_indev(group));
        if (res != LV_RES_OK) {
            return;
        }

        lv_obj_invalidate(focused);
    }
}

void lv_group_set_refocus_policy(lv_group_t *group, lv_group_refocus_policy_t policy)
{
    group->refocus_policy = policy & 0x01;
}

void lv_group_set_wrap(lv_group_t *group, bool en)
{
    group->wrap = en ? 1 : 0;
}

lv_obj_t *lv_group_get_focused(const lv_group_t *group)
{
    if (!group) {
        return NULL;
    }

    if (group->obj_focus == NULL) {
        return NULL;
    }

    return *group->obj_focus;
}

lv_group_focus_cb_t lv_group_get_focus_cb(const lv_group_t *group)
{
    if (!group) {
        return NULL;
    }

    return group->focus_cb;
}

lv_group_edge_cb_t lv_group_get_edge_cb(const lv_group_t *group)
{
    if (!group) {
        return NULL;
    }

    return group->edge_cb;
}

bool lv_group_get_editing(const lv_group_t *group)
{
    if (!group) {
        return false;
    }

    return group->editing ? true : false;
}

bool lv_group_get_wrap(lv_group_t *group)
{
    if (!group) {
        return false;
    }

    return group->wrap ? true : false;
}

uint32_t lv_group_get_obj_count(lv_group_t *group)
{
    return _lv_ll_get_len(&group->obj_ll);
}

static void lv_group_refocus(lv_group_t *g)
{
    // 重新聚焦必须暂时允许包装正常工作
    uint8_t temp_wrap = g->wrap;

    g->wrap = 1;

    if (g->refocus_policy == LV_GROUP_REFOCUS_POLICY_NEXT) {
        lv_group_focus_next(g);
    } else if (g->refocus_policy == LV_GROUP_REFOCUS_POLICY_PREV) {
        lv_group_focus_prev(g);
    }

    // 恢复包装属性
    g->wrap = temp_wrap;
}

static bool focus_next_core(lv_group_t *group, void *(*begin)(const lv_ll_t *), void *(*move)(const lv_ll_t *, const void *))
{
    bool focus_changed = false;
    if (group->frozen) {
        return focus_changed;
    }

    bool can_move = true;
    bool can_begin = true;
    lv_obj_t **obj_sentinel = NULL;
    lv_obj_t **obj_next = group->obj_focus;

    for (; ;) {
        if (obj_next == NULL) {
            if (group->wrap || (obj_sentinel == NULL)) {
                if (!can_begin) {
                    return focus_changed;
                }

                obj_next = (lv_obj_t **)begin(&group->obj_ll);
                can_move = false;
                can_begin = false;
            } else {
                // 当前聚焦的对象是组中的最后一个/第一个，保持这种状态
                return focus_changed;
            }
        }

        if (obj_sentinel == NULL) {
            obj_sentinel = obj_next;
            if (obj_sentinel == NULL) {
                // 组为空
                return focus_changed;
            }
        }

        if (can_move) {
            obj_next = (lv_obj_t **)move(&group->obj_ll, obj_next);

            // 如果我们遍历整个列表并且没有找到另一个可见对象，则放弃
            if (obj_next == obj_sentinel) {
                return focus_changed;
            }
        }

        can_move = true;

        if (obj_next == NULL) {
            continue;
        }

        if (lv_obj_get_state(*obj_next) & LV_STATE_DISABLED) {
            continue;
        }

        lv_obj_t *parent = *obj_next;
        while (parent) {
            if (lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) {
                break;
            }

            parent = lv_obj_get_parent(parent);
        }

        if (parent && lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) {
            continue;
        }

        break;
    }

    if (obj_next == group->obj_focus) {
        return focus_changed;
    }

    if (group->obj_focus) {
        lv_res_t res = lv_event_send(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        if (res != LV_RES_OK) {
            return focus_changed;
        }

        lv_obj_invalidate(*group->obj_focus);
    }

    group->obj_focus = obj_next;

    lv_res_t res = lv_event_send(*group->obj_focus, LV_EVENT_FOCUSED, get_indev(group));
    if (res != LV_RES_OK) {
        return focus_changed;
    }

    lv_obj_invalidate(*group->obj_focus);
    if (group->focus_cb) {
        group->focus_cb(group);
    }

    focus_changed = true;
    return focus_changed;
}

static lv_indev_t *get_indev(const lv_group_t *g)
{
    lv_indev_t *indev_group = NULL;
    lv_indev_t *indev_encoder = NULL;
    lv_indev_t *indev = lv_indev_get_next(NULL);

    while (indev) {
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if (indev->group == g) {
            // 首选键盘
            if (indev_type == LV_INDEV_TYPE_KEYPAD) {
                return indev;
            }

            if (indev_type == LV_INDEV_TYPE_ENCODER) {
                indev_encoder = indev;
            }

            indev_group = indev;
        }

        indev = lv_indev_get_next(indev);
    }

    if (indev_encoder) {
        return indev_encoder;
    }

    if (indev_group) {
        return indev_group;
    }

    // 如果没有更好的选择，请使用第一个输入设备。(如果没有输入设备可以为NULL)
    return lv_indev_get_next(NULL);
}
