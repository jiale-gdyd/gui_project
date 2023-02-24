#ifndef RTSP_INTERNAL_LIVE_SERVER_MEDIA_SESSION_H
#define RTSP_INTERNAL_LIVE_SERVER_MEDIA_SESSION_H

#include "ServerMediaSession.h"

namespace rtsp {
class LiveServerMediaSession : public ServerMediaSession {
public:
    LiveServerMediaSession(char const *streamName, char const *info, char const *description, bool isSSM, char const *miscSDPLines, StreamControl *streamControl = NULL);
    virtual ~LiveServerMediaSession();
};

class LiveServerMediaSubsession : public ServerMediaSubsession {
public:
    LiveServerMediaSubsession(char const *trackId, char const *sdp, char const *codec, unsigned char rtpPayloadType, unsigned timestampFrequency);
    virtual ~LiveServerMediaSubsession();
};
}

#endif
