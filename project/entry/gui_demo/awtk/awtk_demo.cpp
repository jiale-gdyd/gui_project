#include <linux/kconfig.h>

#if defined(CONFIG_AWTK_UI_DEMO)
#include "demo_ui/demo_ui.h"
#endif

#include "awtk_demo.h"

int awtk_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_AWTK_UI_DEMO)
    return awtk_ui_demo_init(argc, argv);
#endif

    return -1;
}

int awtk_demo_exit(void)
{
#if defined(CONFIG_AWTK_UI_DEMO)
    return awtk_ui_demo_exit();
#endif

    return -1;
}
