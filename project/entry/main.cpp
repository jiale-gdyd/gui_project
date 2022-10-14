#include "process_init.h"
#include "gui_demo/gui_demo.h"

/**
 * 函数名称: main
 * 功能描述: 应用进程启动连接主入口
 * 输入参数: argc --> 命令行参数个数
 *          argv --> 命令行参数列表
 * 输出参数: 无
 * 返回说明: 返回0或1
 */
int main(int argc, char *argv[])
{
    app_version_header();

    gui_demo_main(argc, argv);
    return gui_demo_exit();
}
