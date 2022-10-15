#include <linux/kconfig.h>
#include "host_unittest.h"

#if defined(CONFIG_LIBDRM)
#include "libdrm/libdrm_unittest.h"
#endif

int host_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_LIBDRM)
    return host_libdrm_unittest_init(argc, argv);
#endif

    return -1;
}

int host_unittest_exit(void)
{
#if defined(CONFIG_LIBDRM)
    return host_libdrm_unittest_exit();
#endif

    return -1;
}
