#include "azure_cortex_a7_demo.h"
#include "guix/azure_guix_demo.h"
#include "azure_cortex_a7_demo.h"

int azure_cortex_a7_threadx_guix_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_AZURE_GUIX)
    return azure_cortex_a7_guix_demo_init(argc, argv);
#else
    return azure_cortex_a7_threadx_demo_init(argc, argv);
#endif
}

int azure_cortex_a7_threadx_guix_demo_exit(void)
{
#if defined(CONFIG_AZURE_GUIX)
    return azure_cortex_a7_guix_demo_exit();
#else
    return azure_cortex_a7_threadx_demo_exit();
#endif
}
