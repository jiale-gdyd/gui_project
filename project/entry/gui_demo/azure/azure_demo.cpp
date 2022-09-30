#include <linux/kconfig.h>

#if defined(CONFIG_CORTEX_A7)
#include "cortex_a7/azure_cortex_a7_demo.h"
#endif

#if defined(CONFIG_LINUX_GNU)
#include "linux_gnu/azure_linux_gnu_demo.h"
#endif

#include "azure_demo.h"

int azure_threadx_guix_demo_main(int argc, char *argv[])
{
#if defined(CONFIG_CORTEX_A7)
    return azure_cortex_a7_threadx_guix_demo_main(argc, argv);
#elif defined(CONFIG_LINUX_GNU)
    return azure_linux_gnu_threadx_guix_demo_main(argc, argv);
#endif

    return -1;
}
