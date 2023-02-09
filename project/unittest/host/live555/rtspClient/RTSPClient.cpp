#include <csignal>
#include "RTSPClient.h"
#include "liveRTSP.hpp"

void live_rtsp_client_callback(uint8_t *frame, uint32_t frame_size, void *user_data, bool &quit, uint8_t channel)
{
    printf("channel:[%d], frame_size:[%08u]\n", (int)channel, frame_size);
}

int live555_rtspclient_main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <rtspURL1 rtspURL2>\n", argv[0]);
        printf("Example: %s rtsp://admin:xd@123456@192.168.1.7 rtsp://admin:a12345678@192.168.1.165\n", argv[0]);
        return 1;
    }

    rtspPullType info, info1;
    std::vector<rtspPullType> rtspInfo;

    info.userInsData = NULL;
    info.decodeChannel = 1;
    info.retryIntervalSec = 2;
    info.codecType = RTSP_STREAM_CODEC_H264;
    info.rtspFullUrl = std::move(argv[1]);
    info.frameCallback = live_rtsp_client_callback;
    rtspInfo.push_back(info);

    if (argc >= 3) {
        info1.userInsData = NULL;
        info1.decodeChannel = 2;
        info1.retryIntervalSec = 2;
        info1.codecType = RTSP_STREAM_CODEC_H264;
        info1.rtspFullUrl = std::move(argv[2]);
        info1.frameCallback = live_rtsp_client_callback;
        rtspInfo.push_back(info1);
    }

    liveRtspClientPullInit(rtspInfo);

    static bool exit_flag = false;
    signal(SIGINT, [](int) {
        exit_flag = true;
    });

    while (!exit_flag) {
        sleep(1);
    }

    liveRtspClientPullExit();
    return 0;
}
