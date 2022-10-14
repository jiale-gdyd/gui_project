#include <linux/kconfig.h>
#include "unittest.h"

#if defined(CONFIG_X86_64)
#include "host/host_unittest.h"
#endif

#if defined(CONFIG_ROCKCHIP)
#include "rockchip/rockchip_unittest.h"
#endif

int unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_X86_64)
    return host_unittest_init(argc, argv);
#elif defined(CONFIG_ROCKCHIP)
    return rockchip_unittest_init(argc, argv);
#endif

    return -1;
}

int unittest_exit(void)
{
#if defined(CONFIG_X86_64)
    return host_unittest_exit();
#elif defined(CONFIG_ROCKCHIP)
    return rockchip_unittest_exit();
#endif

    return -1;
}
