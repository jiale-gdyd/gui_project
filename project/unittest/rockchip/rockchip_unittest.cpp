#include <linux/kconfig.h>
#include "rockchip_unittest.h"

#if defined(CONFIG_RV11XX)
#include "rv11xx/rv11xx_unittest.h"
#endif

int rockchip_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_RV11XX)
    return rockchip_rv11xx_unittest_init(argc, argv);
#endif

    return -1;
}

int rockchip_unittest_exit(void)
{
#if defined(CONFIG_RV11XX)
    return rockchip_rv11xx_unittest_exit();
#endif

    return -1;
}
