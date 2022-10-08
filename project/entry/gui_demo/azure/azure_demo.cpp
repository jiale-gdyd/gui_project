#include <linux/kconfig.h>

#if defined(CONFIG_AZURE_GUIX)
#include "guix/azure_guix_demo.h"
#else
#include "threadx/azure_threadx_demo.h"
#endif

#include "azure_demo.h"

int azure_threadx_guix_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_AZURE_GUIX)
    return azure_guix_demo_init(argc, argv);
#else
    return azure_threadx_demo_init(argc, argv);
#endif
}

int azure_threadx_guix_demo_exit(void)
{
#if defined(CONFIG_AZURE_GUIX)
    return azure_guix_demo_exit();
#else
    return azure_threadx_demo_exit();
#endif
}
