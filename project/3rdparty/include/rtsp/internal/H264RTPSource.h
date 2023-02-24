#ifndef RTSP_INTERNAL_H264_RTP_SOURCE_H
#define RTSP_INTERNAL_H264_RTP_SOURCE_H

#include "RTPSource.h"

namespace rtsp {
class H264RTPSource : public RTPSource {
public:
    H264RTPSource(int connType, MediaSubsession &subsession, TaskScheduler &task);
    virtual ~H264RTPSource();

protected:
    virtual void processFrame(RTPPacketBuffer *packet);

    void putStartCode();
    int parseSpropParameterSets(char *spropParameterSets);
};
}

#endif
