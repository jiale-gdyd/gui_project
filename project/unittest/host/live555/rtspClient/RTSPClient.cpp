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
        printf("Usage: %s <rtspURL-1 ... rtspURL-n>\n", argv[0]);
        printf("Example: %s rtsp://admin:xd@123456@192.168.1.7 rtsp://admin:a12345678@192.168.1.165 ...\n", argv[0]);
        return 1;
    }

    rtspPullType info;
    std::vector<rtspPullType> rtspInfo;

    for (int i = 1; i <= (argc - 1); ++i) {
        info.userInsData = NULL;
        info.decodeChannel = i;
        info.retryIntervalSec = 2;
        info.codecType = RTSP_STREAM_CODEC_H264;
        info.rtspFullUrl = std::move(argv[i]);
        info.frameCallback = live_rtsp_client_callback;
        rtspInfo.push_back(info);
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
