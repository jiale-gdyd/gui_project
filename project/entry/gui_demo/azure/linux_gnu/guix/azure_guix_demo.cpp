#include <stdio.h>
#include <gx_api.h>

#include "azure_guix_demo.h"
#include "simple/guix_simple.h"
#include "industrial/guix_industrial.h"

int azure_linux_gnu_guix_demo_init(int argc, char *argv[])
{
    // return azure_linux_gnu_guix_simple_demo_init(argc, argv);
    return azure_linux_gnu_guix_industrial_demo_init(argc, argv);
}

int azure_linux_gnu_guix_demo_exit(void)
{
    // return azure_linux_gnu_guix_simple_demo_exit();
    return azure_linux_gnu_guix_industrial_demo_exit();
}
