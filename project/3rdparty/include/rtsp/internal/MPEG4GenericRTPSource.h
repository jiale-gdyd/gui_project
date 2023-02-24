#ifndef RTSP_INTERNAL_MPEG4GENERIC_RTP_SOURCE_H
#define RTSP_INTERNAL_MPEG4GENERIC_RTP_SOURCE_H

#include "RTPSource.h"

namespace rtsp {
class MPEG4GenericRTPSource : public RTPSource {
public:
    MPEG4GenericRTPSource(int streamType, MediaSubsession &subsession, TaskScheduler &task, char const *mode, unsigned sizeLength, unsigned indexLength, unsigned indexDeltaLength);
    virtual ~MPEG4GenericRTPSource();

protected:
    virtual void processFrame(RTPPacketBuffer *packet);

protected:
    char            *fMode;
    unsigned        fSizeLength, fIndexLength, fIndexDeltaLength;
    unsigned        fNumAUHeaders;
    unsigned        fNextAUHeader;
    struct AUHeader *fAUHeaders;
};
}

#endif
