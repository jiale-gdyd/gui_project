#include <linux/kconfig.h>

#include "private.h"
#include "sigslot_unittest.h"

int host_sigslot_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_HOST_SIGNAL_SLOTS)
    return host_unittest_sigslot_init(argc, argv);
#endif

    return -1;
}

int host_sigslot_unittest_exit(void)
{
#if defined(CONFIG_HOST_SIGNAL_SLOTS)
    return host_unittest_sigslot_exit();
#endif

    return -1;
}
