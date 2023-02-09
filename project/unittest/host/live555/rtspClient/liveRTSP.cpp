#include <thread>
#include <future>
#include <unordered_map>

#include <live555/liveMedia/liveMedia.hh>
#include <live555/BasicUsageEnvironment/BasicUsageEnvironment.hh>

#include "liveRTSP.hpp"

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE      (524275)  /* 512*1024 = 524288 - 13 */

typedef struct {
    RTSPClient *rtspClient;
} rtspClientHandle;

using rtsp_clients_map = std::unordered_map<std::string, rtspClientHandle>;

static bool sg_bInitEnv = false;
static bool sg_bFirstTime = true;
static UsageEnvironment *sg_env = NULL;
static TaskScheduler *sg_scheduler = NULL;

static int sg_rtspClientCount = 0;
static rtsp_clients_map sg_rtspCliWatch;
static char volatile sg_eventLoopWatchVariable = 0;

static void streamTimerHandler(void *clientData);
static void subsessionAfterPlaying(void *clientData);
static void subsessionByeHandler(void *clientData, char const *reason);

static void shutdownStream(RTSPClient *rtspClient);
static void setupNextSubsession(RTSPClient *rtspClient);

static void continueAfterPLAY(RTSPClient *rtspClient, int resultCode, char *resultString);
static void continueAfterSETUP(RTSPClient *rtspClient, int resultCode, char *resultString);
static void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode, char *resultString);

static void openURL(UsageEnvironment &env, char const *streamId, char const *rtspURL, rtspPullType *rtspParam, rtsp_clients_map &rtspClientsMap);

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
    static SelfRTSPClient *createNew(UsageEnvironment &env, char const *rtspURL, rtsp_clients_map &rtspClientsMap, int verbosityLevel = 0, char const *streamId = NULL, rtspPullType *rtspParam = NULL, portNumBits tunnelOverHTTPPortNum = 0);

protected:
    SelfRTSPClient(UsageEnvironment &env, char const *rtspURL, rtsp_clients_map &rtspClientsMap, int verbosityLevel, char const *streamId, rtspPullType *rtspParam, portNumBits tunnelOverHTTPPortNum);
    virtual ~SelfRTSPClient();

public:
    const char        *streamId;
    rtspPullType      *rtspPull;
    rtsp_clients_map  *rtspClientsMap;
    StreamClientState scs;
};

class SelfMediaSink : public MediaSink {
public:
    static SelfMediaSink *createNew(UsageEnvironment &env, MediaSubsession &subsession, rtsp_clients_map &rtspClientsMap, char const *streamId = NULL, rtspPullType *rtspParam = NULL);

private:
    SelfMediaSink(UsageEnvironment &env, MediaSubsession &subsession, rtsp_clients_map &rtspClientsMap, char const *streamId, rtspPullType *rtspParam);
    virtual ~SelfMediaSink();

    static void afterGettingFrame(void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);
    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);

private:
    virtual Boolean continuePlaying();

private:
    const char       *streamId;
    u_int8_t         *fReceiveBuffer;
    MediaSubsession  &fSubsession;
    char             *fStreamId;
    rtspPullType     *rtspPull;
    rtsp_clients_map *rtspClientsMap;
};

static void removeRtspClient(std::string streamId)
{
    rtsp_clients_map::iterator iter = sg_rtspCliWatch.begin();
    while (iter != sg_rtspCliWatch.end()) {
        if (iter->first == streamId) {
            sg_rtspCliWatch.erase(iter);
            break;
        }

        iter++;
    }
}

static void openURL(UsageEnvironment &env, char const *streamId, char const *rtspURL, rtspPullType *rtspParam, rtsp_clients_map &rtspClientsMap)
{
    rtspClientsMap[streamId].rtspClient = SelfRTSPClient::createNew(env, rtspURL, rtspClientsMap, 1, streamId, rtspParam);
    if (rtspClientsMap[streamId].rtspClient == NULL) {
        env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
        return;
    }

    sg_rtspClientCount += 1;
    rtspClientsMap[streamId].rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
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

        scs.subsession->sink = SelfMediaSink::createNew(env, *scs.subsession, *selfRtspClient->rtspClientsMap, rtspClient->url(), selfRtspClient->rtspPull);
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
        MediaSubsession *subsession;
        Boolean someSubsessionsWereActive = False;
        MediaSubsessionIterator iter(*scs.session);

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

    if (--sg_rtspClientCount <= 0) {
        sg_bInitEnv = false;
        sg_rtspClientCount = 0;
        sg_eventLoopWatchVariable = 1;
    }
}

SelfRTSPClient *SelfRTSPClient::createNew(UsageEnvironment &env, char const *rtspURL, rtsp_clients_map &rtspClientsMap, int verbosityLevel, char const *streamId, rtspPullType *rtspParam, portNumBits tunnelOverHTTPPortNum)
{
    return new SelfRTSPClient(env, rtspURL, rtspClientsMap, verbosityLevel, streamId, rtspParam, tunnelOverHTTPPortNum);
}

SelfRTSPClient::SelfRTSPClient(UsageEnvironment &env, char const *rtspURL, rtsp_clients_map &rtspClientsMap, int verbosityLevel, char const *streamId, rtspPullType *rtspParam, portNumBits tunnelOverHTTPPortNum)
    : RTSPClient(env, rtspURL, verbosityLevel, streamId, tunnelOverHTTPPortNum, -1)
{
    rtspPull = rtspParam;
    this->streamId = streamId;
    this->rtspClientsMap = &rtspClientsMap;
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

SelfMediaSink *SelfMediaSink::createNew(UsageEnvironment &env, MediaSubsession &subsession, rtsp_clients_map &rtspClientsMap, char const *streamId, rtspPullType *rtspParam)
{
    return new SelfMediaSink(env, subsession, rtspClientsMap, streamId, rtspParam);
}

SelfMediaSink::SelfMediaSink(UsageEnvironment &env, MediaSubsession &subsession, rtsp_clients_map &rtspClientsMap, char const *streamId, rtspPullType *rtspParam)
    : MediaSink(env), fSubsession(subsession)
{
    rtspPull = rtspParam;
    this->streamId = streamId;
    this->rtspClientsMap = &rtspClientsMap;

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

void SelfMediaSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned)
{
    if (strstr(fSubsession.mediumName(), "video")) {
        bool invokeCallback = false;

        if (rtspPull->codecType == RTSP_STREAM_CODEC_H264) {
            invokeCallback = strstr(fSubsession.codecName(), "264") ? true : false;
        } else if (rtspPull->codecType == RTSP_STREAM_CODEC_H265) {
            invokeCallback = strstr(fSubsession.codecName(), "265") ? true : false;
        }

        if (invokeCallback) {
            if (rtspPull->frameCallback) {
                bool quit_loop = false;
                rtspPull->frameCallback((uint8_t *)fReceiveBuffer, (uint32_t)frameSize, rtspPull->userInsData, quit_loop, rtspPull->decodeChannel);
                if (quit_loop) {
                    rtsp_clients_map::iterator iter = rtspClientsMap->find(streamId);
                    if (iter != rtspClientsMap->end()) {
                        shutdownStream(iter->second.rtspClient);
                    }

                    removeRtspClient(streamId);
                    return;
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

int liveRtspProcessPrepareThread(const std::vector<rtspPullType> &rtspInfo, UsageEnvironment *env)
{
    bool bFirst = sg_bFirstTime && (sg_rtspCliWatch.size() == 0);

    for (size_t idx = 0; idx < rtspInfo.size(); ++idx) {
        if (!rtspInfo[idx].rtspFullUrl.empty()
            && rtspInfo[idx].frameCallback
            && ((rtspInfo[idx].codecType == RTSP_STREAM_CODEC_H264) || (rtspInfo[idx].codecType == RTSP_STREAM_CODEC_H265)))
        {
            std::string streamId = std::to_string(idx) + "_" + rtspInfo[idx].rtspFullUrl;

            rtsp_clients_map::iterator iter = sg_rtspCliWatch.find(streamId);
            if (iter == sg_rtspCliWatch.end()) {
                if (sg_bFirstTime) {
                    sg_bFirstTime = false;
                }

                openURL(*env, streamId.c_str(), rtspInfo[idx].rtspFullUrl.c_str(), (rtspPullType *)&rtspInfo[idx], sg_rtspCliWatch);
            }
        }
    }

    return bFirst ? 1 : 0;
}

void liveRtspEventLoopMonitorThread(UsageEnvironment *env)
{
    env->taskScheduler().doEventLoop(&sg_eventLoopWatchVariable);
}

int liveRtspClientPullInit(const std::vector<rtspPullType> &rtspInfo)
{
    if (!sg_bInitEnv) {
        sg_scheduler = BasicTaskScheduler::createNew();
        if (!sg_scheduler) {
            return -1;
        }

        sg_env = BasicUsageEnvironment::createNew(*sg_scheduler);
        if (!sg_env) {
            delete sg_scheduler;
            return -2;
        }

        sg_bInitEnv = true;
        sg_rtspCliWatch.clear();
    }

    std::shared_future<int> threadReturn = std::async(std::launch::async, liveRtspProcessPrepareThread, std::cref(rtspInfo), std::ref(sg_env));
    if (threadReturn.get() == 1) {
        std::thread(liveRtspEventLoopMonitorThread, std::ref(sg_env)).detach();
    }

    return 0;
}

void liveRtspClientPullExit(void)
{
    rtsp_clients_map::iterator iter = sg_rtspCliWatch.begin();
    for (; iter != sg_rtspCliWatch.end(); iter++) {
        if (iter->second.rtspClient) {
            shutdownStream(iter->second.rtspClient);
        }

        sg_rtspCliWatch.erase(iter);
    }

    sg_eventLoopWatchVariable = 1;

    sg_env->reclaim();
    delete sg_scheduler;

    sg_bInitEnv = false;
    sg_bFirstTime = true;
    sg_rtspCliWatch.clear();
}
