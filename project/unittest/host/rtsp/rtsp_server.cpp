#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <rtsp/rtsp.hpp>

#include "private.h"

#define MAX_SERVER_SESSIONS     5
static rtsp::RTSPLiveStreamer *gStreamers[MAX_SERVER_SESSIONS] = {nullptr, };

static void addServerSessions()
{
    gStreamers[0] = new rtsp::RTSPLiveStreamer();
    if (gStreamers[0]) {
        gStreamers[0]->open("rtsp://192.168.1.118/live/main", 0, "stream1");
        gStreamers[0]->run();
    }

    gStreamers[1] = new rtsp::RTSPLiveStreamer();
    if (gStreamers[1]) {
        gStreamers[1]->open("rtsp://admin:1234@192.168.1.118/h264", 0, "stream2");
        gStreamers[1]->run();
    }
}

static void removeServerSessions()
{
    for (int i = 0; i < MAX_SERVER_SESSIONS; i++) {
        if (gStreamers[i]) {
            gStreamers[i]->close();
            delete gStreamers[i];
            gStreamers[i] = nullptr;
        }
    }
}

int host_rtsp_server_main(int argc, char *argv[])
{
    addServerSessions();

    rtsp::RTSPServer *rtspServer = rtsp::RTSPServer::instance();
    if (rtspServer == nullptr) {
        return -1;
    }

    rtspServer->startServer(8554);

    static bool exit_flag = false;
    signal(SIGINT, [](int) {
        exit_flag = true;
    });

    while (!exit_flag) {
        sleep(1);
    }

    removeServerSessions();
    rtspServer->stopServer();
    rtspServer->destroy();

    return 0;
}
