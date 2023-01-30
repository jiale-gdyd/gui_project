#ifndef UNITTEST_HOST_EXAMPLES_LIVE555_RTSPSERVER_DYNAMICRTSPSERVER_H
#define UNITTEST_HOST_EXAMPLES_LIVE555_RTSPSERVER_DYNAMICRTSPSERVER_H

#ifndef _RTSP_SERVER_HH
#include <live555/liveMedia/RTSPServer.hh>
#endif

#define MEDIA_SERVER_VERSION_STRING         "1.10"

class DynamicRTSPServer : public RTSPServer {
public:
    static DynamicRTSPServer *createNew(UsageEnvironment &env, Port ourPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds = 65);

protected:
    DynamicRTSPServer(UsageEnvironment &env, int ourSocketIPv4, int ourSocketIPv6, Port ourPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds);
    virtual ~DynamicRTSPServer();

protected:
    virtual void lookupServerMediaSession(char const *streamName, lookupServerMediaSessionCompletionFunc *completionFunc, void *completionClientData, Boolean isFirstLookupInSession);
};

#endif
