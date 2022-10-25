#include <linux/kconfig.h>

#if defined(CONFIG_AWTK_UI_DEMO)
#include "demo_ui/demo_ui.h"
#elif defined(CONFIG_AWTK_DESKTOP_DEMO)
#include "demo_desktop/demo_desktop.h"
#elif defined(CONFIG_AWTK_ANIMATOR_DEMO)
#include "demo_animator/demo_animator.h"
#endif

#include "awtk_demo.h"

int awtk_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_AWTK_UI_DEMO)
    return awtk_ui_demo_init(argc, argv);
#elif defined(CONFIG_AWTK_DESKTOP_DEMO)
    return awtk_demo_desktop_init(argc, argv);
#elif defined(CONFIG_AWTK_ANIMATOR_DEMO)
    return awtk_demo_animator_init(argc, argv);
#endif

    return -1;
}

int awtk_demo_exit(void)
{
#if defined(CONFIG_AWTK_UI_DEMO)
    return awtk_ui_demo_exit();
#elif defined(CONFIG_AWTK_DESKTOP_DEMO)
    return awtk_demo_desktop_exit();
#elif defined(CONFIG_AWTK_ANIMATOR_DEMO)
    return awtk_demo_animator_exit();
#endif

    return -1;
}
