#include <linux/kconfig.h>

#include "private.h"
#include "libdrm_unittest.h"

int host_libdrm_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_HOST_LIBDRM_DISP)
    return host_unittest_libdrm_display_init(argc, argv);
#endif

    return -1;
}

int host_libdrm_unittest_exit(void)
{
#if defined(CONFIG_HOST_LIBDRM_DISP)
    return host_unittest_libdrm_display_exit();
#endif

    return -1;
}
