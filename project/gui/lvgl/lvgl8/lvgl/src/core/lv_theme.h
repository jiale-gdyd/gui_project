#ifndef LV_THEME_H
#define LV_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../core/lv_obj.h"

struct _lv_disp_t;
struct _lv_theme_t;

typedef void (*lv_theme_apply_cb_t)(struct _lv_theme_t *, lv_obj_t *);

typedef struct _lv_theme_t {
    lv_theme_apply_cb_t apply_cb;
    struct _lv_theme_t  *parent;                // 在此主题之上应用当前主题的样式
    void                *user_data;
    struct _lv_disp_t   *disp;
    lv_color_t          color_primary;
    lv_color_t          color_secondary;
    const lv_font_t     *font_small;
    const lv_font_t     *font_normal;
    const lv_font_t     *font_large;
    uint32_t            flags;                 // 主题使用的任何自定义标志
} lv_theme_t;

/**
 * 函数名称: lv_theme_get_from_obj
 * 功能描述: 获取分配给对象显示的主题
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 对象显示的主题(可以为NULL)
 */
lv_theme_t *lv_theme_get_from_obj(lv_obj_t *obj);

/**
 * 函数名称: lv_theme_apply
 * 功能描述: 在对象上应用活动主题
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_theme_apply(lv_obj_t *obj);

/**
 * 函数名称: lv_theme_set_parent
 * 功能描述: 为主题设置基本主题。基础样式的样式将添加到当前主题的样式之前。可以通过设置基本主题来创建任意长的主题链
 * 输入参数: new_theme --> 指向应设置基数的主题的指针
 *           parent    --> 指向基本主题的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_theme_set_parent(lv_theme_t *new_theme, lv_theme_t *parent);

/**
 * 函数名称: lv_theme_set_apply_cb
 * 功能描述: 为主题设置应用回调。apply回调用于为不同的对象添加样式
 * 输入参数: theme    --> 指向应设置回调的主题的指针
 *           apply_cb --> 指向回调的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_theme_set_apply_cb(lv_theme_t *theme, lv_theme_apply_cb_t apply_cb);

/**
 * 函数名称: lv_theme_get_font_small
 * 功能描述: 获取主题的小字体
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 指向字体的指针
 */
const lv_font_t *lv_theme_get_font_small(lv_obj_t *obj);

/**
 * 函数名称: lv_theme_get_font_normal
 * 功能描述: 获取主题的正常字体
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 指向字体的指针
 */
const lv_font_t *lv_theme_get_font_normal(lv_obj_t *obj);

/**
 * 函数名称: lv_theme_get_font_large
 * 功能描述: 获取主题的字幕字体
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 指向字体的指针
 */
const lv_font_t *lv_theme_get_font_large(lv_obj_t *obj);

/**
 * 函数名称: lv_theme_get_color_primary
 * 功能描述: 获取主题的主色
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 颜色
 */
lv_color_t lv_theme_get_color_primary(lv_obj_t *obj);

/**
 * 函数名称: lv_theme_get_color_secondary
 * 功能描述: 获取主题的辅助颜色
 * 输入参数: obj --> 指向对象的指针
 * 输出参数: 无
 * 返回说明: 颜色
 */
lv_color_t lv_theme_get_color_secondary(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif
