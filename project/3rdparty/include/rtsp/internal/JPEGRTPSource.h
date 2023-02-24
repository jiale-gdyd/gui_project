#ifndef RTSP_INTERNAL_JPEG_RTP_SOURCE_H
#define RTSP_INTERNAL_JPEG_RTP_SOURCE_H

#include "RTPSource.h"

namespace rtsp {
#define MAX_JPEG_HEADER_SIZE    (1024)

class JPEGRTPSource : public RTPSource {
public:
    JPEGRTPSource(int connType, MediaSubsession &subsession, TaskScheduler &task);
    virtual ~JPEGRTPSource();

protected:
    virtual void processFrame(RTPPacketBuffer *packet);

protected:
    unsigned fDefaultWidth, fDefaultHeight;
};
}

#endif
