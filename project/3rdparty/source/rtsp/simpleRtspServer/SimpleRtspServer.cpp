#include "simpleRtspServer.h"
#include "SimpleRtspServer.hpp"

namespace rtsp {
SimpleRTSPServer::SimpleRTSPServer() : mInitialized(false), mRtspServerHandle(NULL), mRtspServerSession(NULL)
{

}

SimpleRTSPServer::~SimpleRTSPServer()
{
    if (mRtspServerHandle) {
        simple_rtsp_delete_handle(mRtspServerHandle);
        mRtspServerHandle = NULL;
    }

    if (mRtspServerSession) {
        simple_rtsp_delete_session(mRtspServerSession);
        mRtspServerSession = NULL;
    }

    mInitialized = false;
}

int SimpleRTSPServer::init(std::string path, uint16_t port, int codecFlag)
{
    mRtspServerHandle = simple_create_rtsp_server(port);
    if (mRtspServerHandle == NULL) {
        return -1;
    }

    mRtspServerSession = simple_create_rtsp_session(mRtspServerHandle, path.c_str(), (enum rtspCodecFlags)codecFlag);
    if (mRtspServerSession == NULL) {
        return -2;
    }

    if (RTSP_SERVER_CODEC_H264 == codecFlag) {
        simple_rtsp_set_video(mRtspServerSession, RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
    } else if (RTSP_SERVER_CODEC_H265 == codecFlag) {
        simple_rtsp_set_video(mRtspServerSession, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    } else {
        return -3;
    }

    mInitialized = true;
    simple_rtsp_sync_video_timestamp(mRtspServerSession, simple_rtsp_get_reltime(), simple_rtsp_get_ntptime());

    return 0;
}

int SimpleRTSPServer::sendFrame(const uint8_t *frame, uint32_t frameSize, uint64_t timestamp)
{
    if (!mInitialized) {
        return -1;
    }

    simple_rtsp_send_video(mRtspServerSession, frame, frameSize, timestamp);
    return simple_rtsp_wait_frame_finished(mRtspServerHandle);
}
}
