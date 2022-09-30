#ifndef AZURE_RTOS_CORTEX_A7_THREADX_GUIX_DEMO_H
#define AZURE_RTOS_CORTEX_A7_THREADX_GUIX_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <azure/azure_threadx.h>
#include <azure/azure_guix.h>

/**
 * 函数名称: azure_cortex_a7_threadx_guix_demo_main
 * 功能描述: threadx+guix案例主函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int azure_cortex_a7_threadx_guix_demo_main(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
