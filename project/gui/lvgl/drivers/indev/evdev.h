#ifndef EVDEV_H
#define EVDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_EVDEV || USE_BSD_EVDEV

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../../lvgl.h"
#include "../../src/core/lv_indev_private.h"
#include "../../src/core/lv_indev.h"
#endif

/**
 * 函数名称: evdev_init
 * 功能描述: 输入事件设备初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 无
 */
void evdev_init(void);

/**
 * 函数名称: evdev_set_file
 * 功能描述: 为evdev重新配置设备文件
 * 输入参数: dev_name --> 设置evdev设备文件名
 * 输出参数: 无
 * 返回说明: true: 设备文件设置完成。false: 当前系统不存在设备文件
 */
bool evdev_set_file(char *dev_name);

/**
 * 函数名称: evdev_read
 * 功能描述: 获取evdev的当前位置和状态
 * 输入参数: drv  --> 输入设备驱动
 * 输出参数: data --> 将evdev数据存储在这里
 * 返回说明: 无
 */
void evdev_read(lv_indev_t *drv, lv_indev_data_t *data);

#endif

#ifdef __cplusplus
}
#endif

#endif
