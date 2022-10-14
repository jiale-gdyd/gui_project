#ifndef UNITTEST_ROCKCHIP_RV11XX_UNITTEST_H
#define UNITTEST_ROCKCHIP_RV11XX_UNITTEST_H

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * 函数名称: rockchip_rv11xx_unittest_init
 * 功能描述: rockchip rv11xx单元测试初始化函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int rockchip_rv11xx_unittest_init(int argc, char *argv[]);

/**
 * 函数名称: rockchip_rv11xx_unittest_exit
 * 功能描述: rockchip rv11xx单元测试退出函数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int rockchip_rv11xx_unittest_exit(void);

#if defined(__cplusplus)
}
#endif

#endif
