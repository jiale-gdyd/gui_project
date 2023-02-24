#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <rtsp/rtsp.hpp>

#include "private.h"

static void rtspFrameHandlerCallback(void *arg, rtsp::RTP_FRAME_TYPE frame_type, int64_t timestamp, unsigned char *buf, int len)
{
    rtsp::RTSPClient *rtspClient = reinterpret_cast<rtsp::RTSPClient *>(arg);

    switch (frame_type) {
        case rtsp::FRAME_TYPE_VIDEO:
            printf("[video]: videoWidth:[%d], videoHeight:[%d], videoFPS:[%d], frameSize:[%d]\n", rtspClient->videoWidth(), rtspClient->videoHeight(), rtspClient->videoFPS(), len);
            break;

        case rtsp::FRAME_TYPE_AUDIO:
            // printf("[audio]: videoWidth:[%d], videoHeight:[%d], videoFPS:[%d], frameSize:[%d]\n", rtspClient->videoWidth(), rtspClient->videoHeight(), rtspClient->videoFPS(), len);
            break;

        case rtsp::FRAME_TYPE_ETC:
            // printf("[RTC]: timestamp:[%ld], frameSize:[%d]\n", timestamp, len);
            break;

        default:
            printf("Unsuported\n");
            break;
    }
}

int host_rtsp_client_main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <rtspURL-1 ... rtspURL-n>\n", argv[0]);
        printf("Example: %s rtsp://lujiale:a_12345678@192.168.1.7 rtsp://admin:a12345678@192.168.1.165 ...\n", argv[0]);
        return 1;
    }

    rtsp::RTSPCommonEnv::SetDebugFlag(DEBUG_FLAG_RTSP);

    rtsp::RTSPClient *rtspClient = new rtsp::RTSPClient();
    if (rtspClient == nullptr) {
        printf("new rtsp::RTSPClient failed\n");
        return 2;
    }

    int ret = rtspClient->openURL(argv[1], 1, 2);
    if (ret == 0) {
        ret = rtspClient->playURL(rtspFrameHandlerCallback, rtspClient, NULL, NULL);
        if (ret != 0) {
            printf("play URL:[%s] failed, return:[%d]\n", argv[1], ret);
            goto exit;
        }
    } else {
        printf("open URL:[%s] failed, return:[%d]\n", argv[1], ret);
        goto exit;
    }

    static bool exit_flag = false;
    signal(SIGINT, [](int) {
        exit_flag = true;
    });

    while (!exit_flag) {
        sleep(1);
    }

exit:
    rtspClient->closeURL();
    delete rtspClient;

    return 0;
}
