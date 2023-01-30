#ifndef UNITTEST_HOST_EXAMPLES_LIVE555_LIVE555_H
#define UNITTEST_HOST_EXAMPLES_LIVE555_LIVE555_H

#include <linux/kconfig.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * 函数名称: host_live555_unittest_init
 * 功能描述: 构建主机live555单元测试初始化函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int host_live555_unittest_init(int argc, char *argv[]);

/**
 * 函数名称: host_live555_unittest_exit
 * 功能描述: 构建主机live555单元测试退出函数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int host_live555_unittest_exit();

#if defined(__cplusplus)
}
#endif

#endif
