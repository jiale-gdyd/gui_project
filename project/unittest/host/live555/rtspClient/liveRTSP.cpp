#include <thread>
#include <unordered_map>

#include <live555/liveMedia/liveMedia.hh>
#include <live555/BasicUsageEnvironment/BasicUsageEnvironment.hh>

#include "liveRTSP.hpp"

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE      (524275)  /* 512*1024 = 524288 - 13 */

typedef struct {
    char       variable;
    bool       quit_loop;
    RTSPClient *rtspClient;
} rtspProCliWatch;

static uint8_t sg_rtspClientCount = 0;
static std::unordered_map<std::string, rtspProCliWatch> sg_rtspCliWatch;

static void streamTimerHandler(void *clientData);
static void subsessionAfterPlaying(void *clientData);
static void subsessionByeHandler(void *clientData, char const *reason);

static void shutdownStream(RTSPClient *rtspClient);
static void setupNextSubsession(RTSPClient *rtspClient);

static void continueAfterPLAY(RTSPClient *rtspClient, int resultCode, char *resultString);
static void continueAfterSETUP(RTSPClient *rtspClient, int resultCode, char *resultString);
static void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode, char *resultString);

static void openURL(UsageEnvironment &env, char const *streamId, char const *rtspURL, rtspPullType *rtspParam);

UsageEnvironment &operator<<(UsageEnvironment &env, const RTSPClient &rtspClient)
{
    return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}

UsageEnvironment &operator<<(UsageEnvironment &env, const MediaSubsession &subsession)
{
    return env << subsession.mediumName() << "/" << subsession.codecName();
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

class SelfRTSPClient : public RTSPClient {
public:
    static SelfRTSPClient *createNew(UsageEnvironment &env, char const *rtspURL, int verbosityLevel = 0, char const *streamId = NULL, rtspPullType *rtspParam = NULL, portNumBits tunnelOverHTTPPortNum = 0);

protected:
    SelfRTSPClient(UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *streamId, rtspPullType *rtspParam, portNumBits tunnelOverHTTPPortNum);
    virtual ~SelfRTSPClient();

public:
    const char        *streamId;
    rtspPullType      *rtspPull;
    StreamClientState scs;
};

class SelfMediaSink : public MediaSink {
public:
    static SelfMediaSink *createNew(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId = NULL, rtspPullType *rtspParam = NULL);

private:
    SelfMediaSink(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId, rtspPullType *rtspParam);
    virtual ~SelfMediaSink();

    static void afterGettingFrame(void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);
    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);

private:
    virtual Boolean continuePlaying();

private:
    const char      *streamId;
    u_int8_t        *fReceiveBuffer;
    MediaSubsession &fSubsession;
    char            *fStreamId;
    rtspPullType    *rtspPull;
};

static void openURL(UsageEnvironment &env, char const *streamId, char const *rtspURL, rtspPullType *rtspParam)
{
    sg_rtspCliWatch[streamId].rtspClient = SelfRTSPClient::createNew(env, rtspURL, 1, streamId, rtspParam);
    if (sg_rtspCliWatch[streamId].rtspClient == NULL) {
        env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
        return;
    }

    sg_rtspClientCount += 1;
    sg_rtspCliWatch[streamId].rtspClient->sendDescribeCommand(continueAfterDESCRIBE); 
}

static void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode, char *resultString)
{
    do {
        UsageEnvironment &env = rtspClient->envir();
        StreamClientState &scs = ((SelfRTSPClient *)rtspClient)->scs;

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

static void setupNextSubsession(RTSPClient *rtspClient)
{
    UsageEnvironment &env = rtspClient->envir();
    StreamClientState &scs = ((SelfRTSPClient *)rtspClient)->scs;

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

            rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, False);
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

static void continueAfterSETUP(RTSPClient *rtspClient, int resultCode, char *resultString)
{
    do {
        UsageEnvironment &env = rtspClient->envir();
        SelfRTSPClient *selfRtspClient = (SelfRTSPClient *)rtspClient;
        StreamClientState &scs = selfRtspClient->scs;

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

        scs.subsession->sink = SelfMediaSink::createNew(env, *scs.subsession, rtspClient->url(), selfRtspClient->rtspPull);
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

static void continueAfterPLAY(RTSPClient *rtspClient, int resultCode, char *resultString)
{
    Boolean success = False;

    do {
        UsageEnvironment &env = rtspClient->envir();
        StreamClientState &scs = ((SelfRTSPClient *)rtspClient)->scs;

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

static void subsessionAfterPlaying(void *clientData)
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

static void subsessionByeHandler(void *clientData, char const *reason)
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

static void streamTimerHandler(void *clientData)
{
    SelfRTSPClient *rtspClient = (SelfRTSPClient *)clientData;
    StreamClientState &scs = rtspClient->scs;

    scs.streamTimerTask = NULL;

    shutdownStream(rtspClient);
}

static void shutdownStream(RTSPClient *rtspClient)
{
    UsageEnvironment &env = rtspClient->envir();
    SelfRTSPClient *selfRtspClient = (SelfRTSPClient *)rtspClient;
    StreamClientState &scs = selfRtspClient->scs;

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

    if (--sg_rtspClientCount == 0) {
        for (auto &cli : sg_rtspCliWatch) {
            sg_rtspCliWatch[cli.first].variable = 1;
        }
    }
}

SelfRTSPClient *SelfRTSPClient::createNew(UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *streamId, rtspPullType *rtspParam, portNumBits tunnelOverHTTPPortNum)
{
    return new SelfRTSPClient(env, rtspURL, verbosityLevel, streamId, rtspParam, tunnelOverHTTPPortNum);
}

SelfRTSPClient::SelfRTSPClient(UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *streamId, rtspPullType *rtspParam, portNumBits tunnelOverHTTPPortNum)
    : RTSPClient(env,rtspURL, verbosityLevel, streamId, tunnelOverHTTPPortNum, -1)
{
    rtspPull = rtspParam;
    this->streamId = streamId;
}

SelfRTSPClient::~SelfRTSPClient()
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

SelfMediaSink *SelfMediaSink::createNew(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId, rtspPullType *rtspParam)
{
    return new SelfMediaSink(env, subsession, streamId, rtspParam);
}

SelfMediaSink::SelfMediaSink(UsageEnvironment &env, MediaSubsession &subsession, char const *streamId, rtspPullType *rtspParam)
    : MediaSink(env), fSubsession(subsession)
{
    rtspPull = rtspParam;
    this->streamId = streamId;
    fStreamId = strDup(streamId);
    fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
}

SelfMediaSink::~SelfMediaSink()
{
    delete[] fReceiveBuffer;
    delete[] fStreamId;
}

void SelfMediaSink::afterGettingFrame(void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds)
{
    SelfMediaSink *sink = (SelfMediaSink *)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void SelfMediaSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
    // envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes\n";

    if (strstr(fSubsession.mediumName(), "video")) {
        bool invokeCallback = false;
        if (rtspPull->codecType == RTSP_STREAM_CODEC_H264) {
            invokeCallback = strstr(fSubsession.codecName(), "264") ? true : false;
        } else if (rtspPull->codecType == RTSP_STREAM_CODEC_H265) {
            invokeCallback = strstr(fSubsession.codecName(), "265") ? true : false;
        }

        if (invokeCallback) {
            if (rtspPull->frameCallback) {
                std::string streamIdStr = streamId;
                streamIdStr = streamIdStr.substr(streamIdStr.find("_") + 1);

                rtspPull->frameCallback((uint8_t *)fReceiveBuffer, (uint32_t)frameSize, streamIdStr.c_str(),
                    rtspPull->userInsData, sg_rtspCliWatch[streamId].quit_loop, rtspPull->decodeChannel, rtspPull->codecType);

                if (sg_rtspCliWatch[streamId].quit_loop) {
                    sg_rtspCliWatch[streamId].variable = 1;
                }
            }
        }
    }

    continuePlaying();
}

Boolean SelfMediaSink::continuePlaying()
{
    if (fSource == NULL) {
        return False;
    }

    fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE, afterGettingFrame, this, onSourceClosure, this);
    return True;
}

void liveRtspProcessPrepareThread(const rtspPullType &info, std::string streamId)
{
    TaskScheduler *scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment *env = BasicUsageEnvironment::createNew(*scheduler);

    sg_rtspCliWatch[streamId].variable = 0;
    sg_rtspCliWatch[streamId].quit_loop = false;

    openURL(*env, streamId.c_str(), info.rtspFullUrl.c_str(), (rtspPullType *)&info);

    env->taskScheduler().doEventLoop(&sg_rtspCliWatch[streamId].variable);

    std::unordered_map<std::string, rtspProCliWatch>::iterator iter;
    for (iter = sg_rtspCliWatch.begin(); iter != sg_rtspCliWatch.end(); iter++) {
        if (iter->first == streamId) {
            sg_rtspCliWatch.erase(iter);
        }
    }
}

int liveRtspClientPull(std::vector<rtspPullType> &rtspInfo)
{
    int count = 0;

    for (size_t idx = 0; idx < rtspInfo.size(); ++idx) {
        if (!rtspInfo[idx].rtspFullUrl.empty()
            && rtspInfo[idx].frameCallback
            && ((rtspInfo[idx].codecType == RTSP_STREAM_CODEC_H264) || (rtspInfo[idx].codecType == RTSP_STREAM_CODEC_H265)))
        {
            std::string streamId = std::to_string(idx) + "_" + rtspInfo[idx].rtspFullUrl;
            std::thread(liveRtspProcessPrepareThread, std::cref(rtspInfo[idx]), streamId).detach();

            count += 1;
        }
    }

    return count > 0 ? 0 : -1;
}
