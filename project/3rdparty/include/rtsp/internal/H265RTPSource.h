#ifndef RTSP_INTERNAL_H265_RTP_SOURCE_H
#define RTSP_INTERNAL_H265_RTP_SOURCE_H

#include "H264RTPSource.h"

namespace rtsp {
class H265RTPSource : public H264RTPSource {
public:
    H265RTPSource(int connType, MediaSubsession &subsession, TaskScheduler &task);
    virtual ~H265RTPSource();

protected:
    virtual void processFrame(RTPPacketBuffer *packet);
};
}

#endif
