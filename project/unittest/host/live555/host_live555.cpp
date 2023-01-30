#include "host_live555.h"

#if defined(CONFIG_HOST_LIVE555_RTSP_CLIENT)
#include "rtspClient/RTSPClient.h"
#endif

#if defined(CONFIG_HOST_LIVE555_RTSP_SERVER)
#include "rtspServer/RTSPServer.h"
#endif

int host_live555_unittest_init(int argc, char *argv[])
{
#if defined(CONFIG_HOST_LIVE555_RTSP_CLIENT)
    return live555_rtspclient_main(argc, argv);
#elif defined(CONFIG_HOST_LIVE555_RTSP_SERVER)
    return live555_rtspserver_main(argc, argv);
#endif

    return 0;
}

int host_live555_unittest_exit()
{
    return 0;
}
