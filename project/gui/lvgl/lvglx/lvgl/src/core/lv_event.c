#include "lv_obj.h"
#include "lv_indev.h"

#define MY_CLASS        &lv_obj_class

typedef struct _lv_event_dsc_t{
    lv_event_cb_t   cb;
    void            *user_data;
    lv_event_code_t filter : 8;
} lv_event_dsc_t;

static lv_event_t *event_head;

static bool event_is_bubbled(lv_event_t *e);
static lv_res_t event_send_core(lv_event_t *e);
static lv_event_dsc_t *lv_obj_get_event_dsc(const lv_obj_t *obj, uint32_t id);

#if LV_LOG_TRACE_EVENT
#define EVENT_TRACE(...)            LV_LOG_TRACE(__VA_ARGS__)
#else
#define EVENT_TRACE(...)
#endif

lv_res_t lv_event_send(lv_obj_t *obj, lv_event_code_t event_code, void *param)
{
    if (obj == NULL) {
        return LV_RES_OK;
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_event_t e;
    e.target = obj;
    e.current_target = obj;
    e.code = event_code;
    e.user_data = NULL;
    e.param = param;
    e.deleted = 0;
    e.stop_bubbling = 0;
    e.stop_processing = 0;

    // 从事件中使用的对象构建一个简单的链表, 了解this对象是否被从此`event_cb`调用的嵌套事件删除是很重要的
    e.prev = event_head;
    event_head = &e;

    // 发送事件
    lv_res_t res = event_send_core(&e);

    // 从列表中删除此元素
    event_head = e.prev;

    return res;
}

lv_res_t lv_obj_event_base(const lv_obj_class_t *class_p, lv_event_t *e)
{
    const lv_obj_class_t *base;
    if (class_p == NULL) {
        base = e->current_target->class_p;
    } else {
        base = class_p->base_class;
    }

    // 找到一个基础，在其中设置了Call祖先的事件handler_cb
    while (base && (base->event_cb == NULL)) {
        base = base->base_class;
    }

    if (base == NULL) {
        return LV_RES_OK;
    }

    if (base->event_cb == NULL) {
        return LV_RES_OK;
    }

    // 调用实际事件回调
    e->user_data = NULL;
    base->event_cb(base, e);

    lv_res_t res = LV_RES_OK;
    // 如果对象被删除则停止
    if (e->deleted) {
        res = LV_RES_INV;
    }

    return res;
}

lv_obj_t *lv_event_get_target(lv_event_t *e)
{
    return e->target;
}

lv_obj_t *lv_event_get_current_target(lv_event_t *e)
{
    return e->current_target;
}

lv_event_code_t lv_event_get_code(lv_event_t *e)
{
    return (lv_event_code_t)(e->code & ~LV_EVENT_PREPROCESS);
}

void *lv_event_get_param(lv_event_t *e)
{
    return e->param;
}

void *lv_event_get_user_data(lv_event_t *e)
{
    return e->user_data;
}

void lv_event_stop_bubbling(lv_event_t *e)
{
    e->stop_bubbling = 1;
}

void lv_event_stop_processing(lv_event_t *e)
{
    e->stop_processing = 1;
}

uint32_t lv_event_register_id(void)
{
    static uint32_t last_id = _LV_EVENT_LAST;
    last_id ++;
    return last_id;
}

void _lv_event_mark_deleted(lv_obj_t *obj)
{
    lv_event_t *e = event_head;

    while (e) {
        if ((e->current_target == obj) || (e->target == obj)) {
            e->deleted = 1;
        }

        e = e->prev;
    }
}

struct _lv_event_dsc_t *lv_obj_add_event_cb(lv_obj_t *obj, lv_event_cb_t event_cb, lv_event_code_t filter, void *user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_allocate_spec_attr(obj);

    obj->spec_attr->event_dsc_cnt++;
    obj->spec_attr->event_dsc = (struct _lv_event_dsc_t *)lv_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
    LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);

    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].cb = event_cb;
    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].filter = filter;
    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].user_data = user_data;

    return &obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1];
}

bool lv_obj_remove_event_cb(lv_obj_t *obj, lv_event_cb_t event_cb)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if (obj->spec_attr == NULL) {
        return false;
    }

    int32_t i = 0;
    for (i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if ((event_cb == NULL) || (obj->spec_attr->event_dsc[i].cb == event_cb)) {
            // 将剩余的事件处理程序向前移动
            for (; i < (obj->spec_attr->event_dsc_cnt - 1); i++) {
                obj->spec_attr->event_dsc[i] = obj->spec_attr->event_dsc[i + 1];
            }
    
            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = (struct _lv_event_dsc_t *)lv_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);

            return true;
        }
    }

    return false;
}

bool lv_obj_remove_event_cb_with_user_data(lv_obj_t *obj, lv_event_cb_t event_cb, const void *event_user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if (obj->spec_attr == NULL) {
        return false;
    }

    int32_t i = 0;
    for (i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if (((event_cb == NULL) || (obj->spec_attr->event_dsc[i].cb == event_cb)) && (obj->spec_attr->event_dsc[i].user_data == event_user_data)) {
            for (; i < (obj->spec_attr->event_dsc_cnt - 1); i++) {
                obj->spec_attr->event_dsc[i] = obj->spec_attr->event_dsc[i + 1];
            }

            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = (lv_event_dsc_t *)lv_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);

            return true;
        }
    }

    return false;
}

bool lv_obj_remove_event_dsc(lv_obj_t *obj, struct _lv_event_dsc_t *event_dsc)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if (obj->spec_attr == NULL) {
        return false;
    }

    int32_t i = 0;
    for (i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if (&obj->spec_attr->event_dsc[i] == event_dsc) {
            // 将剩余的事件处理程序向前移动
            for (; i < (obj->spec_attr->event_dsc_cnt - 1); i++) {
                obj->spec_attr->event_dsc[i] = obj->spec_attr->event_dsc[i + 1];
            }

            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = (struct _lv_event_dsc_t *)lv_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);

            return true;
        }
    }

    return false;
}

void *lv_obj_get_event_user_data(struct _lv_obj_t *obj, lv_event_cb_t event_cb)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if (obj->spec_attr == NULL) {
        return NULL;
    }

    int32_t i = 0;
    for (i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if (event_cb == obj->spec_attr->event_dsc[i].cb) {
            return obj->spec_attr->event_dsc[i].user_data;
        }
    }

    return NULL;
}

lv_indev_t *lv_event_get_indev(lv_event_t *e)
{
    if ((e->code == LV_EVENT_PRESSED)
        || (e->code == LV_EVENT_PRESSING)
        || (e->code == LV_EVENT_PRESS_LOST)
        || (e->code == LV_EVENT_SHORT_CLICKED)
        || (e->code == LV_EVENT_LONG_PRESSED)
        || (e->code == LV_EVENT_LONG_PRESSED_REPEAT)
        || (e->code == LV_EVENT_CLICKED)
        || (e->code == LV_EVENT_RELEASED)
        || (e->code == LV_EVENT_SCROLL_BEGIN)
        || (e->code == LV_EVENT_SCROLL_END)
        || (e->code == LV_EVENT_SCROLL)
        || (e->code == LV_EVENT_GESTURE)
        || (e->code == LV_EVENT_KEY)
        || (e->code == LV_EVENT_FOCUSED)
        || (e->code == LV_EVENT_DEFOCUSED)
        || (e->code == LV_EVENT_LEAVE))
    {
        return (lv_indev_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

lv_obj_draw_part_dsc_t *lv_event_get_draw_part_dsc(lv_event_t *e)
{
    if ((e->code == LV_EVENT_DRAW_PART_BEGIN) || (e->code == LV_EVENT_DRAW_PART_END)) {
        return (lv_obj_draw_part_dsc_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

lv_draw_ctx_t *lv_event_get_draw_ctx(lv_event_t *e)
{
    if ((e->code == LV_EVENT_DRAW_MAIN)
        || (e->code == LV_EVENT_DRAW_MAIN_BEGIN)
        || (e->code == LV_EVENT_DRAW_MAIN_END)
        || (e->code == LV_EVENT_DRAW_POST)
        || (e->code == LV_EVENT_DRAW_POST_BEGIN)
        || (e->code == LV_EVENT_DRAW_POST_END))
    {
        return (lv_draw_ctx_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

const lv_area_t *lv_event_get_old_size(lv_event_t *e)
{
    if (e->code == LV_EVENT_SIZE_CHANGED) {
        return (const lv_area_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

uint32_t lv_event_get_key(lv_event_t *e)
{
    if(e->code == LV_EVENT_KEY) {
        uint32_t *k = (uint32_t *)lv_event_get_param(e);
        if (k) {
            return *k;
        } else {
            return 0;
        }
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return 0;
    }
}

lv_anim_t *lv_event_get_scroll_anim(lv_event_t *e)
{
    if (e->code == LV_EVENT_SCROLL_BEGIN) {
        return (lv_anim_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

void lv_event_set_ext_draw_size(lv_event_t *e, lv_coord_t size)
{
    if(e->code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t *cur_size = (lv_coord_t *)lv_event_get_param(e);
        *cur_size = LV_MAX(*cur_size, size);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
    }
}

lv_point_t *lv_event_get_self_size_info(lv_event_t *e)
{
    if (e->code == LV_EVENT_GET_SELF_SIZE) {
        return (lv_point_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

lv_hit_test_info_t *lv_event_get_hit_test_info(lv_event_t *e)
{
    if (e->code == LV_EVENT_HIT_TEST) {
        return (lv_hit_test_info_t *)lv_event_get_param(e);
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

const lv_area_t *lv_event_get_cover_area(lv_event_t *e)
{
    if (e->code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t *p = (lv_cover_check_info_t *)lv_event_get_param(e);
        return p->area;
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

void lv_event_set_cover_res(lv_event_t *e, lv_cover_res_t res)
{
    if (e->code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t *p = (lv_cover_check_info_t *)lv_event_get_param(e);
        if (res > p->res) {
            p->res = res;
        }
    } else {
        LV_LOG_WARN("Not interpreted with this event code");
    }
}

static lv_event_dsc_t *lv_obj_get_event_dsc(const lv_obj_t *obj, uint32_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if (!obj->spec_attr) {
        return NULL;
    }

    if (id >= obj->spec_attr->event_dsc_cnt) {
        return NULL;
    }

    return &obj->spec_attr->event_dsc[id];
}

static lv_res_t event_send_core(lv_event_t *e)
{
    EVENT_TRACE("Sending event %d to %p with %p param", e->code, (void *)e->current_target, e->param);

    // 如果设置，则调用输入设备的反馈回调
    lv_indev_t *indev_act = lv_indev_get_act();
    if (indev_act) {
        if(indev_act->driver->feedback_cb) {
            indev_act->driver->feedback_cb(indev_act->driver, e->code);
        }

        if (e->stop_processing) {
            return LV_RES_OK;
        }

        if (e->deleted) {
            return LV_RES_INV;
        }
    }

    uint32_t i = 0;
    lv_res_t res = LV_RES_OK;
    lv_event_dsc_t *event_dsc = lv_obj_get_event_dsc(e->current_target, 0);

    while (event_dsc && (res == LV_RES_OK)) {
        if (event_dsc->cb 
            && ((event_dsc->filter & LV_EVENT_PREPROCESS) == LV_EVENT_PREPROCESS)
            && (event_dsc->filter == (LV_EVENT_ALL | LV_EVENT_PREPROCESS)
            || (event_dsc->filter & ~LV_EVENT_PREPROCESS) == e->code))
        {
            e->user_data = event_dsc->user_data;
            event_dsc->cb(e);

            if (e->stop_processing) {
                return LV_RES_OK;
            }

            // 如果对象被删除则停止
            if (e->deleted) {
                return LV_RES_INV;
            }
        }

        i++;
        event_dsc = lv_obj_get_event_dsc(e->current_target, i);
    }

    res = lv_obj_event_base(NULL, e);
    event_dsc = res == LV_RES_INV ? NULL : lv_obj_get_event_dsc(e->current_target, 0);

    i = 0;
    while (event_dsc && (res == LV_RES_OK)) {
        if (event_dsc->cb
            && ((event_dsc->filter & LV_EVENT_PREPROCESS) == 0)
            && (event_dsc->filter == LV_EVENT_ALL
            || event_dsc->filter == e->code))
        {
            e->user_data = event_dsc->user_data;
            event_dsc->cb(e);

            if (e->stop_processing) {
                return LV_RES_OK;
            }

            // 如果对象被删除则停止
            if (e->deleted) {
                return LV_RES_INV;
            }
        }

        i++;
        event_dsc = lv_obj_get_event_dsc(e->current_target, i);
    }

    if ((res == LV_RES_OK) && e->current_target->parent && event_is_bubbled(e)) {
        e->current_target = e->current_target->parent;
        res = event_send_core(e);
        if (res != LV_RES_OK) {
            return LV_RES_INV;
        }
    }

    return res;
}

static bool event_is_bubbled(lv_event_t *e)
{
    if (e->stop_bubbling) {
        return false;
    }

    // 事件代码总是冒泡
    switch (e->code) {
        case LV_EVENT_CHILD_CREATED:
        case LV_EVENT_CHILD_DELETED:
            return true;

        default:
            break;
    }

    if (lv_obj_has_flag(e->current_target, LV_OBJ_FLAG_EVENT_BUBBLE) == false) {
        return false;
    }

    switch (e->code) {
        case LV_EVENT_HIT_TEST:
        case LV_EVENT_COVER_CHECK:
        case LV_EVENT_REFR_EXT_DRAW_SIZE:
        case LV_EVENT_DRAW_MAIN_BEGIN:
        case LV_EVENT_DRAW_MAIN:
        case LV_EVENT_DRAW_MAIN_END:
        case LV_EVENT_DRAW_POST_BEGIN:
        case LV_EVENT_DRAW_POST:
        case LV_EVENT_DRAW_POST_END:
        case LV_EVENT_DRAW_PART_BEGIN:
        case LV_EVENT_DRAW_PART_END:
        case LV_EVENT_REFRESH:
        case LV_EVENT_DELETE:
        case LV_EVENT_CHILD_CREATED:
        case LV_EVENT_CHILD_DELETED:
        case LV_EVENT_CHILD_CHANGED:
        case LV_EVENT_SIZE_CHANGED:
        case LV_EVENT_STYLE_CHANGED:
        case LV_EVENT_GET_SELF_SIZE:
            return false;

        default:
            return true;
    }
}
