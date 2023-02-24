#ifndef SIMPLE_RTSP_SERVER_HPP
#define SIMPLE_RTSP_SERVER_HPP

#include <memory>
#include <string>
#include <cstdint>

#include <stdbool.h>

namespace rtsp {
enum {
    RTSP_SERVER_CODEC_H264 = 0,
    RTSP_SERVER_CODEC_H265
};

using rtsp_server_handler_t = void *;
using rtsp_server_session_t = void *;

class SimpleRTSPServer {
public:
    SimpleRTSPServer();
    ~SimpleRTSPServer();

    int init(std::string path, uint16_t port = 554, int codecFlag = RTSP_SERVER_CODEC_H264);
    int sendFrame(const uint8_t *frame, uint32_t frameSize, uint64_t timestamp);

private:
    bool                  mInitialized;         // 是否初始化完成
    rtsp_server_handler_t mRtspServerHandle;    // RTSP服务器句柄
    rtsp_server_session_t mRtspServerSession;   // RTSP服务器会话
};
}

#endif
