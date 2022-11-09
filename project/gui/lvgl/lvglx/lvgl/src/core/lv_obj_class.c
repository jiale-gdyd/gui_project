#include "lv_obj.h"
#include "lv_theme.h"

#define MY_CLASS        &lv_obj_class

static void lv_obj_construct(lv_obj_t *obj);
static uint32_t get_instance_size(const lv_obj_class_t *class_p);

lv_obj_t *lv_obj_class_create_obj(const lv_obj_class_t *class_p, lv_obj_t *parent)
{
    LV_TRACE_OBJ_CREATE("Creating object with %p class on %p parent", (void *)class_p, (void *)parent);

    uint32_t s = get_instance_size(class_p);
    lv_obj_t *obj = (lv_obj_t *)lv_malloc(s);
    if (obj == NULL) {
        return NULL;
    }

    lv_memzero(obj, s);
    obj->class_p = class_p;
    obj->parent = parent;

    // 创建屏幕
    if (parent == NULL) {
        LV_TRACE_OBJ_CREATE("creating a screen");
        lv_disp_t *disp = lv_disp_get_default();
        if (!disp) {
            LV_LOG_WARN("No display created to yet. No place to assign the new screen");
            lv_free(obj);
            return NULL;
        }

        if (disp->screens == NULL) {
            disp->screens = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *));
            disp->screens[0] = obj;
            disp->screen_cnt = 1;
        } else {
            disp->screen_cnt++;
            disp->screens = (lv_obj_t **)lv_realloc(disp->screens, sizeof(lv_obj_t *) * disp->screen_cnt);
            disp->screens[disp->screen_cnt - 1] = obj;
        }

        // 将坐标设置为全屏尺寸
        obj->coords.x1 = 0;
        obj->coords.y1 = 0;
        obj->coords.x2 = lv_disp_get_hor_res(NULL) - 1;
        obj->coords.y2 = lv_disp_get_ver_res(NULL) - 1;
    } else {    // 创建一个普通对象
        LV_TRACE_OBJ_CREATE("creating normal object");

        LV_ASSERT_OBJ(parent, MY_CLASS);
        if (parent->spec_attr == NULL) {
            lv_obj_allocate_spec_attr(parent);
        }

        if (parent->spec_attr->children == NULL) {
            parent->spec_attr->children = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *));
            parent->spec_attr->children[0] = obj;
            parent->spec_attr->child_cnt = 1;
        } else {
            parent->spec_attr->child_cnt++;
            parent->spec_attr->children = (lv_obj_t **)lv_realloc(parent->spec_attr->children, sizeof(lv_obj_t *) * parent->spec_attr->child_cnt);
            parent->spec_attr->children[parent->spec_attr->child_cnt - 1] = obj;
        }
    }

    return obj;
}

void lv_obj_class_init_obj(lv_obj_t *obj)
{
    lv_obj_mark_layout_as_dirty(obj);
    lv_obj_enable_style_refresh(false);

    lv_theme_apply(obj);
    lv_obj_construct(obj);

    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);

    lv_obj_refresh_self_size(obj);

    lv_group_t *def_group = lv_group_get_default();
    if (def_group && lv_obj_is_group_def(obj)) {
        lv_group_add_obj(def_group, obj);
    }

    lv_obj_t *parent = lv_obj_get_parent(obj);
    if (parent) {
        // 将祖先的事件处理程序调用给父级以通知它有关新子级的信息。也触发布局更新
        lv_event_send(parent, LV_EVENT_CHILD_CHANGED, obj);
        lv_event_send(parent, LV_EVENT_CHILD_CREATED, obj);

        // 如果未创建屏幕，则使该区域无效
        lv_obj_invalidate(obj);
    }
}

void _lv_obj_destruct(lv_obj_t *obj)
{
    if (obj->class_p->destructor_cb) {
        obj->class_p->destructor_cb(obj->class_p, obj);
    }

    if (obj->class_p->base_class) {
        // 在销毁祖先类型期间不要让后代方法运行
        obj->class_p = obj->class_p->base_class;

        // 也调用基类的析构函数
        _lv_obj_destruct(obj);
    }
}

bool lv_obj_is_editable(lv_obj_t *obj)
{
    const lv_obj_class_t *class_p = obj->class_p;

    // 找到设置了可编辑的基础
    while (class_p && (class_p->editable == LV_OBJ_CLASS_EDITABLE_INHERIT)) {
        class_p = class_p->base_class;
    }

    if (class_p == NULL) {
        return false;
    }

    return class_p->editable == LV_OBJ_CLASS_EDITABLE_TRUE ? true : false;
}

bool lv_obj_is_group_def(lv_obj_t *obj)
{
    const lv_obj_class_t *class_p = obj->class_p;

    // 找到设置了group_def的基数
    while (class_p && (class_p->group_def == LV_OBJ_CLASS_GROUP_DEF_INHERIT)) {
        class_p = class_p->base_class;
    }

    if (class_p == NULL) {
        return false;
    }

    return class_p->group_def == LV_OBJ_CLASS_GROUP_DEF_TRUE ? true : false;
}

static void lv_obj_construct(lv_obj_t *obj)
{
    const lv_obj_class_t *original_class_p = obj->class_p;

    if (obj->class_p->base_class) {
        // 在构建祖先类型期间不要让后代方法运行
        obj->class_p = obj->class_p->base_class;

        // 先建基地
        lv_obj_construct(obj);
    }

    // 恢复原来的类别
    obj->class_p = original_class_p;

    if (obj->class_p->constructor_cb) {
        obj->class_p->constructor_cb(obj->class_p, obj);
    }
}

static uint32_t get_instance_size(const lv_obj_class_t *class_p)
{
    // 查找设置实例大小的基数
    const lv_obj_class_t *base = class_p;
    while (base && (base->instance_size == 0)) {
        base = base->base_class;
    }

    // 永远不会发生: 至少在`lv_obj`类中设置
    if (base == NULL) {
        return 0;
    }

    return base->instance_size;
}
