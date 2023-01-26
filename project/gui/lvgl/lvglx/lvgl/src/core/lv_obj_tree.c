#include <stdlib.h>

#include "lv_obj.h"
#include "lv_indev.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_anim.h"
#include "../misc/lv_async.h"

#define MY_CLASS        &lv_obj_class

static void obj_del_core(lv_obj_t *obj);
static void lv_obj_del_async_cb(void *obj);
static lv_obj_tree_walk_res_t walk_core(lv_obj_t *obj, lv_obj_tree_walk_cb_t cb, void *user_data);

void lv_obj_del(lv_obj_t *obj)
{
    LV_LOG_TRACE("begin (delete %p)", (void *)obj);

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_invalidate(obj);

    lv_obj_t *par = lv_obj_get_parent(obj);
    if (par) {
        lv_obj_scrollbar_invalidate(par);
    }

    lv_disp_t *disp = NULL;
    bool act_scr_del = false;
    if (par == NULL) {
        disp = lv_obj_get_disp(obj);
        if (!disp) {
            return;
        }

        if (disp->act_scr == obj) {
            act_scr_del = true;
        }
    }

    obj_del_core(obj);

    // 将祖先的事件处理程序调用给父级以通知它有关子项删除的信息
    if (par) {
        lv_obj_update_layout(par);
        lv_obj_readjust_scroll(par, LV_ANIM_OFF);
        lv_obj_scrollbar_invalidate(par);
        lv_event_send(par, LV_EVENT_CHILD_CHANGED, NULL);
        lv_event_send(par, LV_EVENT_CHILD_DELETED, NULL);
    }

    // 处理活动屏幕是否被删除
    if (act_scr_del)  {
        LV_LOG_WARN("the active screen was deleted");
        disp->act_scr = NULL;
    }

    LV_ASSERT_MEM_INTEGRITY();
    LV_LOG_TRACE("finished (delete %p)", (void *)obj);
}

void lv_obj_clean(lv_obj_t *obj)
{
    LV_LOG_TRACE("begin (delete %p)", (void *)obj);

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);

    lv_obj_t *child = lv_obj_get_child(obj, 0);
    while (child) {
        obj_del_core(child);
        child = lv_obj_get_child(obj, 0);
    }

    // 只是为了删除滚动动画(如果有)
    lv_obj_scroll_to(obj, 0, 0, LV_ANIM_OFF);
    if (obj->spec_attr) {
        obj->spec_attr->scroll.x = 0;
        obj->spec_attr->scroll.y = 0;
    }

    LV_ASSERT_MEM_INTEGRITY();

    LV_LOG_TRACE("finished (delete %p)", (void *)obj);
}

void lv_obj_del_anim_ready_cb(lv_anim_t *a)
{
    lv_obj_del((lv_obj_t *)a->var);
}

void lv_obj_del_async(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_async_call(lv_obj_del_async_cb, obj);
}

void lv_obj_set_parent(lv_obj_t *obj, lv_obj_t *parent)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_OBJ(parent, MY_CLASS);

    if (obj->parent == NULL) {
        LV_LOG_WARN("Can't set the parent of a screen");
        return;
    }

    if (parent == NULL) {
        LV_LOG_WARN("Can't set parent == NULL to an object");
        return;
    }

    lv_obj_invalidate(obj);
    lv_obj_allocate_spec_attr(parent);

    // 从旧父级的子级列表中删除对象
    int32_t i;
    lv_obj_t *old_parent = obj->parent;

    for (i = lv_obj_get_index(obj); i <= ((int32_t)lv_obj_get_child_cnt(old_parent) - 2); i++) {
        old_parent->spec_attr->children[i] = old_parent->spec_attr->children[i + 1];
    }

    old_parent->spec_attr->child_cnt--;
    if (old_parent->spec_attr->child_cnt) {
        old_parent->spec_attr->children = (struct _lv_obj_t **)lv_realloc(old_parent->spec_attr->children, old_parent->spec_attr->child_cnt * (sizeof(lv_obj_t *)));
    } else {
        lv_free(old_parent->spec_attr->children);
        old_parent->spec_attr->children = NULL;
    }

    // 将孩子作为最后一个(最新的孩子)添加到新的父母
    parent->spec_attr->child_cnt++;
    parent->spec_attr->children = (struct _lv_obj_t **)lv_realloc(parent->spec_attr->children, parent->spec_attr->child_cnt * (sizeof(lv_obj_t *)));
    parent->spec_attr->children[lv_obj_get_child_cnt(parent) - 1] = obj;

    obj->parent = parent;

    // 通知原来的父级，因为它的一个子级丢失了
    lv_obj_readjust_scroll(old_parent, LV_ANIM_OFF);
    lv_obj_scrollbar_invalidate(old_parent);
    lv_event_send(old_parent, LV_EVENT_CHILD_CHANGED, obj);
    lv_event_send(old_parent, LV_EVENT_CHILD_DELETED, NULL);

    // 通知新家长关于孩子的信息
    lv_event_send(parent, LV_EVENT_CHILD_CHANGED, obj);
    lv_event_send(parent, LV_EVENT_CHILD_CREATED, NULL);

    lv_obj_mark_layout_as_dirty(obj);
    lv_obj_invalidate(obj);
}

void lv_obj_move_to_index(lv_obj_t *obj, int32_t index)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t *parent = lv_obj_get_parent(obj);
    if (!parent) {
        LV_LOG_WARN("parent is NULL");
        return;
    }

    if (index < 0) {
        index = lv_obj_get_child_cnt(parent) + index;
    }

    const int32_t old_index = lv_obj_get_index(obj);
    if (index < 0) {
        return;
    }

    if (index >= (int32_t)lv_obj_get_child_cnt(parent)) {
        return;
    }

    if (index == old_index) {
        return;
    }

    int32_t i = old_index;
    if (index < old_index) {
        while (i > index) {
            parent->spec_attr->children[i] = parent->spec_attr->children[i - 1];
            i--;
        }
    } else {
        while (i < index) {
            parent->spec_attr->children[i] = parent->spec_attr->children[i + 1];
            i++;
        }
    }

    parent->spec_attr->children[index] = obj;
    lv_event_send(parent, LV_EVENT_CHILD_CHANGED, NULL);

    lv_obj_invalidate(parent);
}

void lv_obj_swap(lv_obj_t *obj1, lv_obj_t *obj2)
{
    LV_ASSERT_OBJ(obj1, MY_CLASS);
    LV_ASSERT_OBJ(obj2, MY_CLASS);

    lv_obj_t *parent = lv_obj_get_parent((const lv_obj_t *)obj1);
    lv_obj_t *parent2 = lv_obj_get_parent((const lv_obj_t *)obj2);

    uint_fast32_t index1 = (uint_fast32_t)lv_obj_get_index((const lv_obj_t *)obj1);
    uint_fast32_t index2 = (uint_fast32_t)lv_obj_get_index((const lv_obj_t *)obj2);

    lv_event_send(parent2, LV_EVENT_CHILD_DELETED, obj2);
    lv_event_send(parent, LV_EVENT_CHILD_DELETED, obj1);

    parent->spec_attr->children[index1] = obj2;
    parent2->spec_attr->children[index2] = obj1;

    lv_event_send(parent, LV_EVENT_CHILD_CHANGED, obj2);
    lv_event_send(parent, LV_EVENT_CHILD_CREATED, obj2);
    lv_event_send(parent2, LV_EVENT_CHILD_CHANGED, obj1);
    lv_event_send(parent2, LV_EVENT_CHILD_CREATED, obj1);

    lv_obj_invalidate(parent);

    if (parent != parent2) {
        lv_obj_invalidate(parent2);
    }

    lv_group_swap_obj(obj1, obj2);
}

lv_obj_t *lv_obj_get_screen(const lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const lv_obj_t *act_par;
    const lv_obj_t *par = obj;

    do {
        act_par = par;
        par = lv_obj_get_parent(act_par);
    } while (par != NULL);

    return (lv_obj_t *)act_par;
}

lv_disp_t *lv_obj_get_disp(const lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const lv_obj_t *scr;

    if (obj->parent == NULL) {
        scr = obj;                      // `obj`是一个屏幕
    } else {
        scr = lv_obj_get_screen(obj);   // 获取`obj`的屏幕
    }

    lv_disp_t *d;
    // _LV_LL_READ(&LV_GC_ROOT(_lv_disp_ll), d) {
    for (d = (lv_disp_t *)_lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll)); d != NULL; d = (lv_disp_t *)_lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), d)) {
        uint32_t i;
        for (i = 0; i < d->screen_cnt; i++) {
            if (d->screens[i] == scr) {
                return d;
            }
        }
    }

    LV_LOG_WARN("No screen found");
    return NULL;
}

lv_obj_t *lv_obj_get_parent(const lv_obj_t *obj)
{
    if (obj == NULL) {
        return NULL;
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    return obj->parent;
}

lv_obj_t *lv_obj_get_child(const lv_obj_t *obj, int32_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if (obj->spec_attr == NULL) {
        return NULL;
    }

    uint32_t idu;
    if (id < 0) {
        id = obj->spec_attr->child_cnt + id;
        if (id < 0) {
            return NULL;
        }

        idu = (uint32_t) id;
    } else {
        idu = id;
    }

    if (idu >= obj->spec_attr->child_cnt) {
        return NULL;
    }

    return obj->spec_attr->children[id];
}

uint32_t lv_obj_get_child_cnt(const lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if (obj->spec_attr == NULL) {
        return 0;
    }

    return obj->spec_attr->child_cnt;
}

uint32_t lv_obj_get_index(const lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t *parent = lv_obj_get_parent(obj);
    if (parent == NULL) {
        return 0;
    }

    uint32_t i = 0;
    for (i = 0; i < lv_obj_get_child_cnt(parent); i++) {
        if (lv_obj_get_child(parent, i) == obj) {
            return i;
        }
    }

    return 0xFFFFFFFF;
}

void lv_obj_tree_walk(lv_obj_t *start_obj, lv_obj_tree_walk_cb_t cb, void *user_data)
{
    walk_core(start_obj, cb, user_data);
}

static void lv_obj_del_async_cb(void *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_del((lv_obj_t *)obj);
}

static void obj_del_core(lv_obj_t *obj)
{
    // 让用户释放`LV_EVENT_DELETE`中使用的资源
    lv_res_t res = lv_event_send(obj, LV_EVENT_DELETE, NULL);
    if (res == LV_RES_INV) {
        return;
    }

    // 递归删除孩子
    lv_obj_t *child = lv_obj_get_child(obj, 0);
    while (child) {
        obj_del_core(child);
        child = lv_obj_get_child(obj, 0);
    }

    lv_group_t *group = (lv_group_t *)lv_obj_get_group(obj);

    // 如果使用要删除的对象，则重置所有输入设备
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev) {
        if ((indev->proc.types.pointer.act_obj == obj) || (indev->proc.types.pointer.last_obj == obj)) {
            lv_indev_reset(indev, obj);
        }

        if (indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }

        if ((indev->group == group) && (obj == lv_indev_get_obj_act())) {
            lv_indev_reset(indev, obj);
        }

        indev = lv_indev_get_next(indev);
    }

    lv_res_t async_cancel_res = LV_RES_OK;
    while (async_cancel_res == LV_RES_OK) {
        async_cancel_res = lv_async_call_cancel(lv_obj_del_async_cb, obj);
    }

    // 所有孩子都被删除了。现在清理对象特定数据
    _lv_obj_destruct(obj);

    // 移除屏幕列表的屏幕
    if (obj->parent == NULL) {
        uint32_t i;
        lv_disp_t *disp = lv_obj_get_disp(obj);
        
        // 在列表中找到屏幕
        for (i = 0; i < disp->screen_cnt; i++) {
            if (disp->screens[i] == obj) {
                break;
            }
        }

        uint32_t id = i;
        for (i = id; i < (disp->screen_cnt - 1); i++) {
            disp->screens[i] = disp->screens[i + 1];
        }

        disp->screen_cnt--;
        disp->screens = (lv_obj_t **)lv_realloc(disp->screens, disp->screen_cnt * sizeof(lv_obj_t *));
    } else {
        uint32_t i;
        uint32_t id = lv_obj_get_index(obj);

        for (i = id; i < (obj->parent->spec_attr->child_cnt - 1); i++) {
            obj->parent->spec_attr->children[i] = obj->parent->spec_attr->children[i + 1];
        }

        obj->parent->spec_attr->child_cnt--;
        obj->parent->spec_attr->children = (lv_obj_t **)lv_realloc(obj->parent->spec_attr->children, obj->parent->spec_attr->child_cnt * sizeof(lv_obj_t *));
    }

    lv_free(obj);
}

static lv_obj_tree_walk_res_t walk_core(lv_obj_t *obj, lv_obj_tree_walk_cb_t cb, void *user_data)
{
    lv_obj_tree_walk_res_t res = LV_OBJ_TREE_WALK_NEXT;

    if (obj == NULL) {
        lv_disp_t *disp = lv_disp_get_next(NULL);
        while (disp) {
            uint32_t i;
            for (i = 0; i < disp->screen_cnt; i++) {
                walk_core(disp->screens[i], cb, user_data);
            }
    
            disp = lv_disp_get_next(disp);
        }

        return LV_OBJ_TREE_WALK_END;
    }

    res = cb(obj, user_data);
    if (res == LV_OBJ_TREE_WALK_END) {
        return LV_OBJ_TREE_WALK_END;
    }

    if (res != LV_OBJ_TREE_WALK_SKIP_CHILDREN) {
        uint32_t i;
        for (i = 0; i < lv_obj_get_child_cnt(obj); i++) {
            res = walk_core(lv_obj_get_child(obj, i), cb, user_data);
            if (res == LV_OBJ_TREE_WALK_END) {
                return LV_OBJ_TREE_WALK_END;
            }
        }
    }

    return LV_OBJ_TREE_WALK_NEXT;
}
