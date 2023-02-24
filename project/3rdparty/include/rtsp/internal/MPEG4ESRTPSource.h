#ifndef RTSP_INTERNAL_MPEG4ES_RTP_SOURCE_H
#define RTSP_INTERNAL_MPEG4ES_RTP_SOURCE_H

#include "RTPSource.h"

namespace rtsp {
class MPEG4ESRTPSource : public RTPSource {
public:
    MPEG4ESRTPSource(int streamType, MediaSubsession &subsession, TaskScheduler &task);
    virtual ~MPEG4ESRTPSource();

protected:
    virtual void processFrame(RTPPacketBuffer *packet);
    unsigned char *parseConfigStr(char const *configStr, unsigned &configSize);

protected:
    struct AUHeader *fAUHeaders;
};
}

#endif
