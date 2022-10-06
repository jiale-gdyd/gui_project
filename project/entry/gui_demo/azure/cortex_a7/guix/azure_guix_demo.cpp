#include "azure_guix_demo.h"

int azure_cortex_a7_guix_demo_init(int argc, char *argv[])
{
    tx_kernel_enter();
    return 0;
}

int azure_cortex_a7_guix_demo_exit(void)
{
    return -1;
}

void tx_application_define(void *first_unused_memory)
{
    
}
