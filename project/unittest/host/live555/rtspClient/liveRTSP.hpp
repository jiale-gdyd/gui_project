#ifndef LIVERTSP_HPP
#define LIVERTSP_HPP

#include <string>
#include <vector>
#include <cstdint>

enum {
    RTSP_STREAM_CODEC_H264,
    RTSP_STREAM_CODEC_H265
};

typedef int(*rtspclient_callback)(uint8_t *frame, uint32_t frame_size, const char *streamId, void *user_data, bool &quit, int channel, int codec_type);

typedef struct {
    int                 codecType;
    std::string         rtspFullUrl;
    rtspclient_callback frameCallback;
    void                *userInsData;
    int                 decodeChannel;
} rtspPullType;

int liveRtspClientPull(std::vector<rtspPullType> &rtspInfo);

#endif
