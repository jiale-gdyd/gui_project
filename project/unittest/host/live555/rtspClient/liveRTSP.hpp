#ifndef LIVERTSP_HPP
#define LIVERTSP_HPP

#include <string>
#include <vector>
#include <cstdint>

enum {
    RTSP_STREAM_CODEC_H264,
    RTSP_STREAM_CODEC_H265
};

typedef void (*rtspclient_callback)(uint8_t *frame, uint32_t frame_size, void *user_data, bool &quit, uint8_t channel);

typedef struct {
    uint8_t             codecType;
    std::string         rtspFullUrl;
    rtspclient_callback frameCallback;
    void                *userInsData;
    uint8_t             decodeChannel;
    uint8_t             retryIntervalSec;
} rtspPullType;

void liveRtspClientPullExit(void);
int liveRtspClientPullInit(const std::vector<rtspPullType> &rtspInfo);

#endif
