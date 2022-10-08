#ifndef AZURE_RTOS_GUIX_INDUSTRIAL_DEMO_H
#define AZURE_RTOS_GUIX_INDUSTRIAL_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <azure/azure_threadx.h>
#include <azure/azure_guix.h>

/**
 * 函数名称: azure_guix_industrial_demo_init
 * 功能描述: guix案例主函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int azure_guix_industrial_demo_init(int argc, char *argv[]);

/**
 * 函数名称: azure_guix_industrial_demo_exit
 * 功能描述: guix案例退出函数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int azure_guix_industrial_demo_exit(void);

#ifdef __cplusplus
}
#endif

#endif
