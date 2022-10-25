#ifndef AWTK_DEMO_ANIMATOR_H
#define AWTK_DEMO_ANIMATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 函数名称: awtk_demo_animator_init
 * 功能描述: AWTK动画案例主函数
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int awtk_demo_animator_init(int argc, char *argv[]);

/**
 * 函数名称: awtk_demo_exit
 * 功能描述: AWTK动画案例退出函数
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int awtk_demo_animator_exit(void);

#ifdef __cplusplus
}
#endif

#endif
