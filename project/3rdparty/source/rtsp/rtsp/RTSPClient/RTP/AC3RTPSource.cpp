#include <rtsp/internal/AC3RTPSource.h>
#include <rtsp/internal/MediaSession.h>

namespace rtsp {
AC3RTPSource::AC3RTPSource(int connType, MediaSubsession &subsession, TaskScheduler &task)
    : RTPSource(connType, subsession, task)
{

}

AC3RTPSource::~AC3RTPSource()
{

}

void AC3RTPSource::processFrame(RTPPacketBuffer *packet)
{
    int len = packet->payloadLen();
    uint8_t *buf = packet->payload();
    int64_t media_timestamp = packet->extTimestamp() == 0 ? getMediaTimestamp(packet->timestamp()) : packet->extTimestamp();

    unsigned char *headerStart = buf;

    if (len < 2) {
        return;
    }

    unsigned char FT = headerStart[0] & 0x03;
    fBeginFrame = FT != 3;

    if (fBeginFrame) {
        copyToFrameBuffer(&buf[2], len - 2);
    }

    if (packet->markerBit() || (FT == 0)) {
        if (fFrameHandlerFunc) {
            fFrameHandlerFunc(fFrameHandlerFuncData, fFrameType, media_timestamp, fFrameBuf, fFrameBufPos);
        }

        resetFrameBuf();
        fBeginFrame = false;
    }
}
}
