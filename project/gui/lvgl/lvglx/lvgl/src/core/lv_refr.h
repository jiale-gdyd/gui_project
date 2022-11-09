#ifndef LV_REFR_H
#define LV_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_obj.h"
#include <stdbool.h>

#define LV_REFR_TASK_PRIO       LV_TASK_PRIO_MID

/**
 * 函数名称: _lv_refr_init
 * 功能描述: 初始化屏幕刷新子系统
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 无
 */
void _lv_refr_init(void);

/**
 * 函数名称: lv_refr_now
 * 功能描述: 重绘无效区域。通常重绘在`lv_timer_handler`中周期性地执行，但是长时间的阻塞过程可以阻止`lv_timer_handler`的调用。在这种情况下，如果GUI在进程中更新(例如进度条)，则可以在应该更新屏幕时调用此函数
 * 输入参数: disp --> 显示刷新的指针。NULL刷新所有显示
 * 输出参数: 无
 * 返回说明: 无
 */
void lv_refr_now(lv_disp_t *disp);

/**
 * 函数名称: lv_obj_redraw
 * 功能描述: 使用传递的绘制上下文在对象及其所有子项上重绘
 * 输入参数: draw_ctx --> 指向初始化绘图上下文的指针
 *          obj      --> 重绘的起始对象应该开始
 * 输出参数: draw_ctx --> 指向初始化绘图上下文的指针
 * 返回说明: 无
 */
void lv_obj_redraw(lv_draw_ctx_t *draw_ctx, lv_obj_t *obj);

/**
 * 函数名称: _lv_inv_area
 * 功能描述: 使显示区域无效以重新绘制它
 * 输入参数: disp   --> 指向应使区域无效的显示位置的指针(如果只有一个显示，则可以使用NULL)
 *           area_p --> 指向应该失效的区域的指针(NULL: 删除失效区域)
 * 输出参数: 无
 * 返回说明: 无
 */
void _lv_inv_area(lv_disp_t *disp, const lv_area_t *area_p);

/**
 * 函数名称: _lv_refr_get_disp_refreshing
 * 功能描述: 获取正在刷新的显示
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 正在刷新的显示
 */
lv_disp_t *_lv_refr_get_disp_refreshing(void);

/**
 * 函数名称: _lv_refr_set_disp_refreshing
 * 功能描述: 设置正在刷新的显示。它不应该由用户直接使用。它可用于欺骗有关有活动显示的绘图功能
 * 输入参数: disp --> 显示正在刷新
 * 输出参数: 无
 * 返回说明: 无
 */
void _lv_refr_set_disp_refreshing(lv_disp_t *disp);

#if LV_USE_PERF_MONITOR
void lv_refr_reset_fps_counter(void);

/**
 * 函数名称: lv_refr_get_fps_avg
 * 功能描述: 获取自启动以来的平均FPS
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 平均FPS
 */
uint32_t lv_refr_get_fps_avg(void);
#endif

/**
 * 函数名称: _lv_disp_refr_timer
 * 功能描述: 定期调用以处理刷新
 * 输入参数: timer --> 指向计时器本身的指针
 * 输出参数: 无
 * 返回说明: 无
 */
void _lv_disp_refr_timer(lv_timer_t *timer);

#ifdef __cplusplus
}
#endif

#endif
