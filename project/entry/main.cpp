#include "process_init.h"

#if defined(CONFIG_GUI)
#include "gui_demo/gui_demo.h"
#endif

#if defined(CONFIG_LIBDRM_TOOLS)
#include <libdrm/libdrm.h>
#endif

/**
 * 函数名称: main
 * 功能描述: 应用进程启动连接主入口
 * 输入参数: argc --> 命令行参数个数
 *           argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 返回0或1
 */
int main(int argc, char *argv[])
{
    app_version_header();

#if defined(CONFIG_LIBDRM_TOOLS)
#if defined(CONFIG_LIBDRM_MODETEST)
    return libdrm_modetest_main(argc, argv);
#elif defined(CONFIG_LIBDRM_MODEPRINT)
    return libdrm_modeprint_main(argc, argv);
#endif
#else
#if defined(CONFIG_GUI)
    gui_demo_main(argc, argv);
    gui_demo_exit();
#endif
#endif

    return 0;
}
