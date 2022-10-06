#include "azure_linux_gnu_demo.h"
#include "guix/azure_guix_demo.h"
#include "threadx/azure_threadx_demo.h"

int azure_linux_gnu_threadx_guix_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_AZURE_GUIX)
    return azure_linux_gnu_guix_demo_init(argc, argv);
#else
    return azure_linux_gnu_threadx_demo_init(argc, argv);
#endif
}

int azure_linux_gnu_threadx_guix_demo_exit(void)
{
#if defined(CONFIG_AZURE_GUIX)
    return azure_linux_gnu_guix_demo_exit();
#else
    return azure_linux_gnu_threadx_demo_exit();
#endif
}
