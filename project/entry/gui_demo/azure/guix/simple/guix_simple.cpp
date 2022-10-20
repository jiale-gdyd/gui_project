#include <stdio.h>
#include <gx_api.h>

#include "guix_simple.h"
#include "../display_driver.h"
#include "guix_simple_resources.h"
#include "guix_simple_specifications.h"

static GX_WINDOW_ROOT *root;
static GX_WINDOW *pHelloScreen;

static void start_guix(void);

int azure_guix_simple_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_DRM_DISP_DRIVER)
    gx_drm_graphics_driver_setup(0, 0, 0, 0, 1);
#endif

    tx_kernel_enter();
    return 0;
}

int azure_guix_simple_demo_exit(void)
{
    return gx_drm_graphics_driver_exit();
}

void tx_application_define(void *first_unused_memory)
{
    start_guix();
}

void start_guix(void)
{
    // 初始化GUIX
    gx_system_initialize();

#if defined(CONFIG_DRM_DISP_DRIVER)
    gx_studio_display_configure(MAIN_DISPLAY, gx_drm_graphics_driver_setup_24xrgb, LANGUAGE_ENGLISH, MAIN_DISPLAY_THEME_1, &root);
#elif defined(CONFIG_X11_DISP_DRIVER)
    gx_studio_display_configure(MAIN_DISPLAY, gx_x11_graphics_driver_setup_24xrgb, LANGUAGE_ENGLISH, MAIN_DISPLAY_THEME_1, &root);
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
    gx_studio_display_configure(MAIN_DISPLAY, gx_fbdev_graphics_driver_setup_24xrgb, LANGUAGE_ENGLISH, MAIN_DISPLAY_THEME_1, &root);
#else
    return;
#endif

    // 创建hello world屏幕
    gx_studio_named_widget_create("simple_window", (GX_WIDGET *)root, (GX_WIDGET **)&pHelloScreen);

    // 显示根窗口，使它和患者屏幕可见
    gx_widget_show(root);

    // 开始GUIX线程
    gx_system_start();
}
