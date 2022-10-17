#include <linux/kconfig.h>

#include "private.h"
#include "rv11xx_unittest.h"

int rockchip_rv11xx_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_ROCKCHIP_RV11XX_LIBDRM)
    return rv11xx_unittest_libdrm_display_init(argc, argv);
#elif defined(CONFIG_ROCKCHIP_RV11XX_CAMERA_LIBDRM)
    return rv11xx_unittest_camera_libdrm_display_init(argc, argv);
#endif

    return -1;
}

int rockchip_rv11xx_unittest_exit(void)
{
#if defined(CONFIG_ROCKCHIP_RV11XX_LIBDRM)
    return rv11xx_unittest_libdrm_display_exit();
#elif defined(CONFIG_ROCKCHIP_RV11XX_CAMERA_LIBDRM)
    return rv11xx_unittest_camera_libdrm_display_exit();
#endif

    return -1;
}
