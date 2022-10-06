#include "gui_demo.h"

#if defined(CONFIG_LVGL)
#include "lvgl/lvgl_demo.h"
#elif defined(CONFIG_AZURE_GUIX)
#include "azure/azure_demo.h"
#endif

int gui_demo_main(int argc, char *argv[])
{
#if defined(CONFIG_LVGL)
    return lvgl_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX)
    return azure_threadx_guix_demo_init(argc, argv);
#endif

    return -1;
}

int gui_demo_exit(void)
{
#if defined(CONFIG_LVGL)
    return lvgl_demo_exit();
#elif defined(CONFIG_AZURE_GUIX)
    return azure_threadx_guix_demo_exit();
#endif
}
