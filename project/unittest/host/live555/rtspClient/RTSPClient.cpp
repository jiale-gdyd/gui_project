#include <live555/liveMedia/liveMedia.hh>
#include <live555/BasicUsageEnvironment/BasicUsageEnvironment.hh>

#include "RTSPClient.h"

#define RTSP_CLIENT_VERBOSITY_LEVEL         1
#define DEBUG_PRINT_EACH_RECEIVED_FRAME     1
#define REQUEST_STREAMING_OVER_TCP          False
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE      100000

char eventLoopWatchVariable = 0;
static unsigned rtspClientCount = 0;

void streamTimerHandler(void *clientData);
void subsessionAfterPlaying(void *clientData);
void subsessionByeHandler(void *clientData, char const *reason);

void setupNextSubsession(RTSPClient *rtspClient);
void shutdownStream(RTSPClient *rtspClient, int exitCode = 1);

void continueAfterPLAY(RTSPClient *rtspClient, int resultCode, char *resultString);
void continueAfterSETUP(RTSPClient *rtspClient, int resultCode, char *resultString);
void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode, char *resultString);

void openURL(UsageEnvironment &env, char const *progName, char const *rtspURL);

UsageEnvironment &operator<<(UsageEnvironment &env, const RTSPClient &rtspClient)
{
    return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}

UsageEnvironment &operator<<(UsageEnvironment &env, const MediaSubsession &subsession)
{
    return env << subsession.mediumName() << "/" << subsession.codecName();
}

void usage(UsageEnvironment &env, char const *progName)
{
    env << "Usage: " << progName << " <rtsp-url-1> ... <rtsp-url-N>\n";
    env << "\t(where each <rtsp-url-i> is a \"rtsp://\" URL)\n";
}

class StreamClientState {
public:
    StreamClientState();
    virtual ~StreamClientState();

public:
    MediaSubsessionIterator *iter;
    MediaSession            *session;
    MediaSubsession         *subsession;
    TaskToken               streamTimerTask;
    double                  duration;
};

class ourRTSPClient : public RTSPClient {
public:
    static ourRTSPClient *createNew(UsageEnvironment &env, char const *rtspURL, int verbosityLevel = 0, char const *applicationName = NULL, portNumBits tunnelOverHTTPPortNum = 0);

protected:
    ourRTSPClient(UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum);
    virtual ~ourRTSPClient();

public:
    StreamClientState scs;
};

class DummySink : public MediaSink {
public:
    static DummySink *createNew(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId = NULL);

private:
    DummySink(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId);
    virtual ~DummySink();

    static void afterGettingFrame(void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);
    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);

private:
    virtual Boolean continuePlaying();

private:
    u_int8_t        *fReceiveBuffer;
    MediaSubsession &fSubsession;
    char            *fStreamId;
};

void openURL(UsageEnvironment &env, char const *progName, char const *rtspURL)
{
    RTSPClient *rtspClient = ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
    if (rtspClient == NULL) {
        env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
        return;
    }

    ++rtspClientCount;
    rtspClient->sendDescribeCommand(continueAfterDESCRIBE); 
}

void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode, char *resultString)
{
    do {
        UsageEnvironment &env = rtspClient->envir();
        StreamClientState &scs = ((ourRTSPClient *)rtspClient)->scs;

        if (resultCode != 0) {
            env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
            delete[] resultString;
            break;
        }

        char *const sdpDescription = resultString;
        env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

        scs.session = MediaSession::createNew(env, sdpDescription);
        delete[] sdpDescription;

        if (scs.session == NULL) {
            env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
            break;
        } else if (!scs.session->hasSubsessions()) {
            env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
            break;
        }

        scs.iter = new MediaSubsessionIterator(*scs.session);
        setupNextSubsession(rtspClient);

        return;
    } while (0);

    shutdownStream(rtspClient);
}

void setupNextSubsession(RTSPClient *rtspClient)
{
    UsageEnvironment &env = rtspClient->envir();
    StreamClientState &scs = ((ourRTSPClient *)rtspClient)->scs;

    scs.subsession = scs.iter->next();
    if (scs.subsession != NULL) {
        if (!scs.subsession->initiate()) {
            env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
            setupNextSubsession(rtspClient);
        } else {
            env << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
            if (scs.subsession->rtcpIsMuxed()) {
                env << "client port " << scs.subsession->clientPortNum();
            } else {
                env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
            }
            env << ")\n";

            rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
        }

        return;
    }

    if (scs.session->absStartTime() != NULL) {
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
    } else {
        scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
    }
}

void continueAfterSETUP(RTSPClient *rtspClient, int resultCode, char *resultString)
{
    do {
        UsageEnvironment &env = rtspClient->envir();
        StreamClientState &scs = ((ourRTSPClient *)rtspClient)->scs;

        if (resultCode != 0) {
            env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
            break;
        }

        env << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
        if (scs.subsession->rtcpIsMuxed()) {
            env << "client port " << scs.subsession->clientPortNum();
        } else {
            env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
        }
        env << ")\n";

        scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url());
        if (scs.subsession->sink == NULL) {
            env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
            break;
        }

        env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";

        scs.subsession->miscPtr = rtspClient;
        scs.subsession->sink->startPlaying(*(scs.subsession->readSource()), subsessionAfterPlaying, scs.subsession);
        if (scs.subsession->rtcpInstance() != NULL) {
            scs.subsession->rtcpInstance()->setByeWithReasonHandler(subsessionByeHandler, scs.subsession);
        }
    } while (0);

    delete[] resultString;
    setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient *rtspClient, int resultCode, char *resultString)
{
    Boolean success = False;

    do {
        UsageEnvironment &env = rtspClient->envir();
        StreamClientState &scs = ((ourRTSPClient *)rtspClient)->scs;

        if (resultCode != 0) {
            env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
            break;
        }

        if (scs.duration > 0) {
            unsigned const delaySlop = 2;
            scs.duration += delaySlop;
            unsigned uSecsToDelay = (unsigned)(scs.duration * 1000000);
            scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc *)streamTimerHandler, rtspClient);
        }

        env << *rtspClient << "Started playing session";
        if (scs.duration > 0) {
            env << " (for up to " << scs.duration << " seconds)";
        }
        env << "...\n";

        success = True;
    } while (0);

    delete[] resultString;
    if (!success) {
        shutdownStream(rtspClient);
    }
}

void subsessionAfterPlaying(void *clientData)
{
    MediaSubsession *subsession = (MediaSubsession *)clientData;
    RTSPClient *rtspClient = (RTSPClient *)(subsession->miscPtr);

    Medium::close(subsession->sink);
    subsession->sink = NULL;

    MediaSession &session = subsession->parentSession();
    MediaSubsessionIterator iter(session);
    while ((subsession = iter.next()) != NULL) {
        if (subsession->sink != NULL) {
            return;
        }
    }

    shutdownStream(rtspClient);
}

void subsessionByeHandler(void *clientData, char const *reason)
{
    MediaSubsession *subsession = (MediaSubsession *)clientData;
    RTSPClient *rtspClient = (RTSPClient *)subsession->miscPtr;
    UsageEnvironment &env = rtspClient->envir();

    env << *rtspClient << "Received RTCP \"BYE\"";
    if (reason != NULL) {
        env << " (reason:\"" << reason << "\")";
        delete[] (char *)reason;
    }
    env << " on \"" << *subsession << "\" subsession\n";

    subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void *clientData)
{
    ourRTSPClient *rtspClient = (ourRTSPClient *)clientData;
    StreamClientState &scs = rtspClient->scs;

    scs.streamTimerTask = NULL;

    shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient *rtspClient, int exitCode)
{
    UsageEnvironment &env = rtspClient->envir();
    StreamClientState &scs = ((ourRTSPClient *)rtspClient)->scs;

    if (scs.session != NULL) { 
        Boolean someSubsessionsWereActive = False;
        MediaSubsessionIterator iter(*scs.session);
        MediaSubsession *subsession;

        while ((subsession = iter.next()) != NULL) {
            if (subsession->sink != NULL) {
                Medium::close(subsession->sink);
                subsession->sink = NULL;

                if (subsession->rtcpInstance() != NULL) {
                    subsession->rtcpInstance()->setByeHandler(NULL, NULL);
                }

                someSubsessionsWereActive = True;
            }
        }

        if (someSubsessionsWereActive) {
            rtspClient->sendTeardownCommand(*scs.session, NULL);
        }
    }

    env << *rtspClient << "Closing the stream.\n";
    Medium::close(rtspClient);

    if (--rtspClientCount == 0) {
        exit(exitCode);
    }
}

ourRTSPClient *ourRTSPClient::createNew(UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum)
{
    return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum)
    : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1)
{

}

ourRTSPClient::~ourRTSPClient()
{

}

StreamClientState::StreamClientState() : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0)
{

}

StreamClientState::~StreamClientState()
{
    delete iter;
    if (session != NULL) {
        UsageEnvironment &env = session->envir();

        env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
        Medium::close(session);
    }
}

DummySink *DummySink::createNew(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId)
{
    return new DummySink(env, subsession, streamId);
}

DummySink::DummySink(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId)
    : MediaSink(env), fSubsession(subsession)
{
    fStreamId = strDup(streamId);
    fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
}

DummySink::~DummySink()
{
    delete[] fReceiveBuffer;
    delete[] fStreamId;
}

void DummySink::afterGettingFrame(void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds)
{
    DummySink *sink = (DummySink *)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
    if (fStreamId != NULL) {
        envir() << "Stream \"" << fStreamId << "\"; ";
    }
    envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";

    if (numTruncatedBytes > 0) {
        envir() << " (with " << numTruncatedBytes << " bytes truncated)";
    }

    char uSecsStr[6 + 1];
    sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
    envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
    if ((fSubsession.rtpSource() != NULL) && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
        envir() << "!";
    }
#ifdef DEBUG_PRINT_NPT
    envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
    envir() << "\n";
#endif

    continuePlaying();
}

Boolean DummySink::continuePlaying()
{
    if (fSource == NULL) {
        return False;
    }

    fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE, afterGettingFrame, this, onSourceClosure, this);
    return True;
}

int live555_rtspclient_main(int argc, char *argv[])
{
    TaskScheduler *scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment *env = BasicUsageEnvironment::createNew(*scheduler);

    if (argc < 2) {
        usage(*env, argv[0]);
        return 1;
    }

    for (int i = 1; i <= (argc - 1); ++i) {
        openURL(*env, argv[0], argv[i]);
    }

#if 1
    env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
    return 0;
#else
    env->reclaim(); env = NULL;
    delete scheduler; scheduler = NULL;
    return 0;
#endif
}
