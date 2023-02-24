#ifndef RTSP_INTERNAL_AC3_RTP_SOURCE_H
#define RTSP_INTERNAL_AC3_RTP_SOURCE_H

#include "RTPSource.h"

namespace rtsp {
class AC3RTPSource : public RTPSource {
public:
    AC3RTPSource(int connType, MediaSubsession &subsession, TaskScheduler &task);
    virtual ~AC3RTPSource();

protected:
    virtual void processFrame(RTPPacketBuffer *packet);
};
}

#endif
