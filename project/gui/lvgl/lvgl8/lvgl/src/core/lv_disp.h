#ifndef LV_DISP_H
#define LV_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_obj.h"
#include "lv_theme.h"
#include "../hal/lv_hal.h"

typedef enum {
    LV_SCR_LOAD_ANIM_NONE,
    LV_SCR_LOAD_ANIM_OVER_LEFT,
    LV_SCR_LOAD_ANIM_OVER_RIGHT,
    LV_SCR_LOAD_ANIM_OVER_TOP,
    LV_SCR_LOAD_ANIM_OVER_BOTTOM,
    LV_SCR_LOAD_ANIM_MOVE_LEFT,
    LV_SCR_LOAD_ANIM_MOVE_RIGHT,
    LV_SCR_LOAD_ANIM_MOVE_TOP,
    LV_SCR_LOAD_ANIM_MOVE_BOTTOM,
    LV_SCR_LOAD_ANIM_FADE_IN,
    LV_SCR_LOAD_ANIM_FADE_ON = LV_SCR_LOAD_ANIM_FADE_IN,
    LV_SCR_LOAD_ANIM_FADE_OUT,
    LV_SCR_LOAD_ANIM_OUT_LEFT,
    LV_SCR_LOAD_ANIM_OUT_RIGHT,
    LV_SCR_LOAD_ANIM_OUT_TOP,
    LV_SCR_LOAD_ANIM_OUT_BOTTOM,
} lv_scr_load_anim_t;

/**
 * 函数名称: lv_disp_get_scr_act
 * 功能描述: 返回一个指向活动屏幕的指针
 * 输入参数: disp --> 显示应获取哪个活动屏幕的指针。(NULL使用默认屏幕)
 * 输出参数: 无
 * 返回说明: 指向活动屏幕对象的指针(由'lv_scr_load()'加载)
 */
lv_obj_t *lv_disp_get_scr_act(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_scr_prev
 * 功能描述: 返回前一个屏幕的指针。仅在屏幕转换期间使用
 * 输入参数: disp --> 显示应获取哪个活动屏幕的指针。(NULL使用默认屏幕)
 * 输出参数: 无
 * 返回说明: 指向前一个屏幕对象的指针，如果现在不使用则为NULL
 */
lv_obj_t *lv_disp_get_scr_prev(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_load_scr
 * 功能描述: 激活屏幕
 * 输入参数: scr --> 指向屏幕的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_load_scr(lv_obj_t *scr);

/**
 * 函数名称: lv_disp_get_layer_top
 * 功能描述: 返回顶层。(每个屏幕都一样，它在普通屏幕层之上)
 * 输入参数: disp --> 指向显示应获取哪个顶层的指针。(NULL使用默认屏幕)
 * 输出参数: 无
 * 返回说明: 指向顶层对象的指针(透明屏幕大小的lv_obj)
 */
lv_obj_t *lv_disp_get_layer_top(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_get_layer_sys
 * 功能描述: 与系统一起返回层。(每个屏幕都一样，它在普通屏幕和顶层之上)
 * 输入参数: disp --> 指向显示应获取哪个顶层的指针。(NULL使用默认屏幕)
 * 输出参数: 无
 * 返回说明: 指向sys层对象的指针(透明屏幕大小的lv_obj)
 */
lv_obj_t *lv_disp_get_layer_sys(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_set_theme
 * 功能描述: 设置显示主题
 * 输入参数: disp --> 指向显示器的指针
 *           th   --> 指向设置主题的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_set_theme(lv_disp_t *disp, lv_theme_t *th);

/**
 * 函数名称: lv_disp_get_theme
 * 功能描述: 获取显示主题
 * 输入参数: disp --> 指向显示器的指针
 * 输出参数: 无
 * 返回说明: 显示的主题(可以为NULL)
 */
lv_theme_t *lv_disp_get_theme(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_set_bg_color
 * 功能描述: 设置显示的背景颜色
 * 输入参数: disp  --> 指向显示器的指针
 *           color --> 背景颜色
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_set_bg_color(lv_disp_t *disp, lv_color_t color);

/**
 * 函数名称: lv_disp_set_bg_image
 * 功能描述: 设置显示器的背景图像
 * 输入参数: disp    --> 指向显示器的指针
 *           img_src --> 文件路径或指向`lv_img_dsc_t`变量的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_set_bg_image(lv_disp_t *disp, const void *img_src);

/**
 * 函数名称: lv_disp_set_bg_opa
 * 功能描述: 设置背景的不透明度
 * 输入参数: disp --> 指向显示器的指针
 *           opa  --> 不透明度(0..255)
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_set_bg_opa(lv_disp_t *disp, lv_opa_t opa);

lv_color_t lv_disp_get_chroma_key_color(lv_disp_t *disp);

/**
 * 函数名称: lv_scr_load_anim
 * 功能描述: 用动画切换屏幕
 * 输入参数: scr       --> 指向要加载的新屏幕的指针
 *           anim_type --> 来自`lv_scr_load_anim_t`的动画类型。例如`LV_SCR_LOAD_ANIM_MOVE_LEFT`
 *           time      --> 动画时间
 *           delay     --> 过渡前的延迟
 *           auto_del  --> true: 自动删除旧屏
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_scr_load_anim(lv_obj_t *scr, lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool auto_del);

/**
 * 函数名称: lv_disp_get_inactive_time
 * 功能描述: 获取自上次用户在显示器上进行活动(例如点击)以来经过的时间
 * 输入参数: disp --> 指向显示的指针(NULL以获得整体最小的不活动状态)
 * 输出参数: 无
 * 返回说明: 自上次活动以来经过的滴答声(毫秒)
 */
uint32_t lv_disp_get_inactive_time(const lv_disp_t *disp);

/**
 * 函数名称: lv_disp_trig_activity
 * 功能描述: 手动触发显示器上的活动
 * 输入参数: disp --> 指向显示的指针(NULL使用默认显示)
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_trig_activity(lv_disp_t *disp);

/**
 * 函数名称: lv_disp_clean_dcache
 * 功能描述: 清除与显示器相关的所有CPU缓存
 * 输入参数: disp --> 指向显示的指针(NULL使用默认显示)
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_disp_clean_dcache(lv_disp_t *disp);

/**
 * 临时启用和禁用显示的失效
 * @param disp 指向显示的指针(NULL使用默认显示)
 * @param en   true：启用失效；false：失效
 */
void lv_disp_enable_invalidation(lv_disp_t *disp, bool en);

/**
 * 获取显示失效已启用
 * @param disp 指向显示的指针(NULL使用默认显示)
 * @return     如果启用了失效，则返回true
 */
bool lv_disp_is_invalidation_enabled(lv_disp_t *disp);

/**
 * 函数名称: _lv_disp_get_refr_timer
 * 功能描述: 获取指向屏幕刷新计时器的指针，以使用`lv_timer_...`函数修改其参数
 * 输入参数: disp --> 指向显示器的指针
 * 输出参数: 无
 * 返回说明: 指向显示刷新计时器的指针。(错误时为NULL)
 */
lv_timer_t *_lv_disp_get_refr_timer(lv_disp_t *disp);

/**
 * 函数名称: lv_scr_act
 * 功能描述: 获取默认显示的活动屏幕
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 指向活动屏幕的指针
 */
static inline lv_obj_t *lv_scr_act(void)
{
    return lv_disp_get_scr_act(lv_disp_get_default());
}

/**
 * 函数名称: lv_layer_top
 * 功能描述: 获取默认显示的顶层
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 指向顶层的指针
 */
static inline lv_obj_t *lv_layer_top(void)
{
    return lv_disp_get_layer_top(lv_disp_get_default());
}

/**
 * 函数名称: lv_layer_sys
 * 功能描述: 获取默认显示的活动屏幕
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 指向sys层的指针
 */
static inline lv_obj_t *lv_layer_sys(void)
{
    return lv_disp_get_layer_sys(lv_disp_get_default());
}

/**
 * 函数名称: lv_scr_load
 * 功能描述: 激活屏幕
 * 输入参数: scr --> 指向屏幕的指针
 * 输出参数: 无
 * 返回说明: 无
 */
static inline void lv_scr_load(lv_obj_t *scr)
{
    lv_disp_load_scr(scr);
}

#ifndef LV_HOR_RES
// 当前活动显示器的水平分辨率
#define LV_HOR_RES lv_disp_get_hor_res(lv_disp_get_default())
#endif

#ifndef LV_VER_RES
// 当前活动显示器的垂直分辨率
#define LV_VER_RES lv_disp_get_ver_res(lv_disp_get_default())
#endif

/**
 * 函数名称: lv_dpx
 * 功能描述: 考虑到默认显示器的DPI，相对于160 DPI显示器缩放给定数量的像素(距离或大小)。它确保例如 无论显示器的DPI如何，`lv_dpx(100)`都将具有相同的物理尺寸
 * 输入参数: n --> 要缩放的像素数
 * 输出参数: 无
 * 返回说明: `n x current_dpi/160`
 */
static inline lv_coord_t lv_dpx(lv_coord_t n)
{
    return LV_DPX(n);
}

/**
 * 函数名称: lv_disp_dpx
 * 功能描述: 考虑给定显示器的DPI，相对于160 DPI显示器缩放给定数量的像素(距离或大小)。它确保例如 无论显示器的DPI如何，`lv_dpx(100)`都将具有相同的物理尺寸
 * 输入参数: disp --> 应考虑其dpi的显示器
 *           n    --> 要缩放的像素数
 * 输出参数: 无
 * 返回说明: `n x current_dpi/160`
 */
static inline lv_coord_t lv_disp_dpx(const lv_disp_t *disp, lv_coord_t n)
{
    return _LV_DPX_CALC(lv_disp_get_dpi(disp), n);
}

#ifdef __cplusplus
}
#endif

#endif
