#ifndef GUI_DEMO_GUI_DEMO_H
#define GUI_DEMO_GUI_DEMO_H

#include <linux/kconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 函数名称: gui_demo_main
 * 功能描述: GUI主函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int gui_demo_main(int argc, char *argv[]);

/**
 * 函数名称: gui_demo_exit
 * 功能描述: GUI退出函数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int gui_demo_exit(void);

#ifdef __cplusplus
}
#endif

#endif
