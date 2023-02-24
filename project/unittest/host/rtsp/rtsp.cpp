#include "rtsp.h"
#include "private.h"

int host_live555_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_HOST_RTSP_CLIENT)
    return host_rtsp_client_main(argc, argv);
#elif defined(CONFIG_HOST_RTSP_SERVER)
    return host_rtsp_server_main(argc, argv);
#endif

    return -1;
}

int host_live555_unittest_exit(void)
{
    return 0;
}
