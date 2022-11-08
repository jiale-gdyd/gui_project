#include <linux/kconfig.h>

#include "private.hpp"
#include "boost_unittest.h"

int host_boost_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_HOST_BOOST_SIGNAL_SLOTS)
    return host_boost_signal_slots_demo_init(argc, argv);
#endif

    return -1;
}

int host_boost_unittest_exit(void)
{
#if defined(CONFIG_HOST_BOOST_SIGNAL_SLOTS)
    return host_boost_signal_slots_demo_exit();
#endif

    return -1;
}
