#include <linux/kconfig.h>
#include "host_unittest.h"

#if defined(CONFIG_HOST_UNITTEST_LIBDRM)
#include "libdrm/libdrm_unittest.h"
#endif

#if defined(CONFIG_HOST_UNITTEST_SIGSLOT)
#include "signalslot/sigslot_unittest.h"
#endif

#if defined(CONFIG_HOST_UNITTEST_LIVE555)
#include "live555/host_live555.h"
#endif

int host_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_HOST_UNITTEST_LIBDRM)
    return host_libdrm_unittest_init(argc, argv);
#elif defined(CONFIG_HOST_UNITTEST_SIGSLOT)
    return host_sigslot_unittest_init(argc, argv);
#elif defined(CONFIG_HOST_UNITTEST_LIVE555)
    return host_live555_unittest_init(argc, argv);
#endif

    return -1;
}

int host_unittest_exit(void)
{
#if defined(CONFIG_HOST_UNITTEST_LIBDRM)
    return host_libdrm_unittest_exit();
#elif defined(CONFIG_HOST_UNITTEST_SIGSLOT)
    return host_sigslot_unittest_exit();
#elif defined(CONFIG_HOST_UNITTEST_LIVE555)
    return host_live555_unittest_exit();
#endif

    return -1;
}
