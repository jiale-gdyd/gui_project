#ifndef LV_DEMO_H
#define LV_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

/**
 * 函数名称: lvgl_demo_init
 * 功能描述: LVGL案例主函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int lvgl_demo_init(int argc, char *argv[]);

/**
 * 函数名称: lvgl_demo_exit
 * 功能描述: LVGL案例退出函数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int lvgl_demo_exit(void);

#ifdef __cplusplus
}
#endif

#endif
