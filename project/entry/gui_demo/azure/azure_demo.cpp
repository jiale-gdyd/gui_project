#include <linux/kconfig.h>

#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "azure_demo.h"

int azure_threadx_guix_demo_main(int argc, char *argv[])
{
    tx_kernel_enter();
    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    
}
