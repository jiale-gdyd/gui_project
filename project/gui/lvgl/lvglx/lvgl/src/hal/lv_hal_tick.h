#ifndef LV_HAL_TICK_H
#define LV_HAL_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../lv_conf_internal.h"

#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

#if !LV_TICK_CUSTOM
/**
 * 函数名称: lv_tick_inc
 * 功能描述: 您必须定期调用此函数
 * 输入参数: tick_period --> 此函数的调用周期(以毫秒为单位)
 * 输出参数: 无
 * 返回说明: 无
 */
LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period);
#endif

/**
 * 函数名称: lv_tick_get
 * 功能描述: 获取自启动以来经过的毫秒数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 经过的毫秒数
 */
uint32_t lv_tick_get(void);

/**
 * 函数名称: lv_tick_elaps
 * 功能描述: 获取自上一个时间戳以来经过的毫秒数
 * 输入参数: prev_tick --> 前一个时间戳(lv_tick_get()的返回值)
 * 输出参数: 无
 * 返回说明: 自`prev_tick`以来经过的毫秒数
 */
uint32_t lv_tick_elaps(uint32_t prev_tick);

#ifdef __cplusplus
}
#endif

#endif
