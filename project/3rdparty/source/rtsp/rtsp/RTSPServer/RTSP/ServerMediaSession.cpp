#include <stdio.h>
#include <rtsp/internal/util.h>
#include <rtsp/internal/SockCommon.h>
#include <rtsp/internal/RTSPCommonEnv.h>
#include <rtsp/internal/ServerMediaSession.h>

namespace rtsp {
char const *const libVersionStr = "2023.02.22";
static char const *const libNameStr = "ljl Streaming Media v";

static pthread_mutex_t hMutex = PTHREAD_MUTEX_INITIALIZER;

StreamControl::StreamControl(void *arg, StartStreamCallback startFunc, StopStreamCallback stopFunc,
    ControlStreamCallback0 pauseFunc, ControlStreamCallback0 resumeFunc,
    ControlStreamCallback1 playDirFunc,
    ControlStreamCallback1 playNextFunc, ControlStreamCallback0 playContinueFunc,
    ControlStreamCallback1 seekFunc,
    ControlStreamCallback2 speedFunc)
{
    fThis = arg;
    fStartStreamFunc = startFunc;
    fStopStreamFunc = stopFunc;
    fPauseStreamFunc = pauseFunc;
    fResumeStreamFunc = resumeFunc;
    fPlayDirStreamFunc = playDirFunc;
    fPlayNextStreamFunc = playNextFunc;
    fPlayContinueStreamFunc = playContinueFunc;
    fSeekStreamFunc = seekFunc;
    fSpeedStreamFunc = speedFunc;
    fStreamState = STREAM_STATE_STOPPED;
}

StreamControl::~StreamControl()
{

}

int StreamControl::startStream(double start_time)
{
    if (fStreamState != STREAM_STATE_STOPPED) {
        return -1;
    }

    if (fStartStreamFunc) {
        int err = fStartStreamFunc(fThis, start_time);
        if (err == 0) {
            fStreamState = STREAM_STATE_RUNNING;
        }

        return err;
    }

    return -1;
}

void StreamControl::stopStream()
{
    fStreamState = STREAM_STATE_STOPPED;
    if (fStopStreamFunc) {
        fStopStreamFunc(fThis);
    }
}

void StreamControl::pauseStream()
{
    if (fStreamState == STREAM_STATE_RUNNING) {
        if (fPauseStreamFunc) {
            fPauseStreamFunc(fThis);
            fStreamState = STREAM_STATE_PAUSED;
        }
    }
}

void StreamControl::resumeStream()
{
    if (fStreamState == STREAM_STATE_PAUSED) {
        if (fResumeStreamFunc) {
            fResumeStreamFunc(fThis);
            fStreamState = STREAM_STATE_RUNNING;
        }
    }
}

void StreamControl::playDirStream(int dir)
{
    if (fPlayDirStreamFunc) {
        fPlayDirStreamFunc(fThis, dir);
    }
}

void StreamControl::playNextStream(int dir)
{
    if ((fStreamState == STREAM_STATE_RUNNING) || (fStreamState == STREAM_STATE_PAUSED)) {
        if (fPlayNextStreamFunc) {
            fPlayNextStreamFunc(fThis, dir);
            fStreamState = STREAM_STATE_RUNNING;
        }
    }
}

void StreamControl::playContinueStream()
{
    if ((fStreamState == STREAM_STATE_RUNNING) || (fStreamState == STREAM_STATE_PAUSED)) {
        if (fPlayContinueStreamFunc) {
            fPlayContinueStreamFunc(fThis);
            fStreamState = STREAM_STATE_RUNNING;
        }
    }
}

void StreamControl::seekStream(int timestamp)
{
    if (fStreamState != STREAM_STATE_STOPPED) {
        if (fSeekStreamFunc) {
            fSeekStreamFunc(fThis, timestamp);
        }
    }
}

void StreamControl::speedStream(float speed)
{
    if (fStreamState != STREAM_STATE_STOPPED) {
        if (fSpeedStreamFunc) {
            fSpeedStreamFunc(fThis, speed);
        }
    }
}

ServerMediaSession::ServerMediaSession(const char *streamName, const char *info, const char *description, bool isSSM, const char *miscSDPLines, StreamControl *streamControl)
    : fIsSSM(isSSM), fSubsessionsHead(NULL), fSubsessionsTail(NULL),
    fSubsessionCounter(0), fReferenceCount(0), fDeleteWhenUnreferenced(false), fStreamControl(streamControl)
{
    fStreamName = strDup(streamName);

    char *libNamePlusVersionStr = NULL;
    if ((info == NULL) || (description == NULL)) {
        libNamePlusVersionStr = new char[strlen(libNameStr) + strlen(libVersionStr) + 1];
        sprintf(libNamePlusVersionStr, "%s%s", libNameStr, libVersionStr);
    }

    fInfoSDPString = strDup(info == NULL ? libNamePlusVersionStr : info);
    fDescriptionSDPString = strDup(description == NULL ? libNamePlusVersionStr : description);
    delete[] libNamePlusVersionStr;

    fMiscSDPLines = strDup(miscSDPLines == NULL ? "" : miscSDPLines);
    gettimeofday(&fCreationTime, NULL);

    MUTEX_INIT(&fMutex);
}

ServerMediaSession::~ServerMediaSession()
{
    deleteAllSubsessions();
    delete[] fStreamName;
    delete[] fInfoSDPString;
    delete[] fDescriptionSDPString;
    delete[] fMiscSDPLines;

    MUTEX_DESTROY(&fMutex);
}

bool ServerMediaSession::addSubsession(ServerMediaSubsession *subsession)
{
    if (subsession->fParentSession != NULL) {
        return false;
    }

    if (fSubsessionsTail == NULL) {
        fSubsessionsHead = subsession;
    } else {
        fSubsessionsTail->fNext = subsession;
    }
    fSubsessionsTail = subsession;

    subsession->fParentSession = this;
    subsession->fTrackNumber = ++fSubsessionCounter;

    return true;
}

void ServerMediaSession::deleteAllSubsessions()
{
    if (fSubsessionsHead) {
        delete fSubsessionsHead;
    }

    fSubsessionsHead = fSubsessionsTail = NULL;
    fSubsessionCounter = 0;
}

char* ServerMediaSession::generateSDPDescription()
{
    struct sockaddr_in ourAddress;
    ourAddress.sin_addr.s_addr = ourIPAddress();
    char *ipAddressStr = inet_ntoa(ourAddress.sin_addr);
    unsigned ipAddressStrSize = strlen(ipAddressStr);

    char *sourceFilterLine;
    if (fIsSSM) {
        char const *const sourceFilterFmt = "a=source-filter: incl IN IP4 * %s\r\na=rtcp-unicast: reflection\r\n";
        unsigned const sourceFilterFmtSize = strlen(sourceFilterFmt) + ipAddressStrSize + 1;

        sourceFilterLine = new char[sourceFilterFmtSize];
        sprintf(sourceFilterLine, sourceFilterFmt, ipAddressStr);
    } else {
        sourceFilterLine = strDup("");
    }

    char *sdp = NULL;
    char *rangeLine = NULL;

    do {
        unsigned sdpLength = 0;
        ServerMediaSubsession *subsession;

        for (subsession = fSubsessionsHead; subsession != NULL; subsession = subsession->fNext) {
            char const *sdpLines = subsession->sdpLines();
            if (sdpLines == NULL) {
                continue;
            }

            sdpLength += strlen(sdpLines);
        }

        if (sdpLength == 0) {
            break;
        }

        float dur = duration();
        if (dur == 0.0) {
            rangeLine = strDup("a=range:npt=0-\r\n");
        } else if (dur > 0.0) {
            char buf[100];
            sprintf(buf, "a=range:npt=0-%.3f\r\n", dur);
            rangeLine = strDup(buf);
        } else {
            rangeLine = strDup("");
        }

        char const *const sdpPrefixFmt =
            "v=0\r\n"
            "o=- %ld%06ld %d IN IP4 %s\r\n"
            "s=%s\r\n"
            "i=%s\r\n"
            "t=0 0\r\n"
            "a=tool:%s%s\r\n"
            "a=type:broadcast\r\n"
            "a=control:*\r\n"
            "%s"
            "%s"
            "a=x-qt-text-nam:%s\r\n"
            "a=x-qt-text-inf:%s\r\n"
            "%s";
        sdpLength += strlen(sdpPrefixFmt)
            + 20 + 6 + 20 + ipAddressStrSize
            + strlen(fDescriptionSDPString)
            + strlen(fInfoSDPString)
            + strlen(libNameStr) + strlen(libVersionStr)
            + strlen(sourceFilterLine)
            + strlen(rangeLine)
            + strlen(fDescriptionSDPString)
            + strlen(fInfoSDPString)
            + strlen(fMiscSDPLines);
        sdpLength += 1000;

        sdp = new char[sdpLength];
        if (sdp == NULL) {
            break;
        }

        snprintf(sdp, sdpLength, sdpPrefixFmt,
            fCreationTime.tv_sec, fCreationTime.tv_usec,
            1,
            ipAddressStr,
            fDescriptionSDPString,
            fInfoSDPString,
            libNameStr, libVersionStr,
            sourceFilterLine,
            rangeLine,
            fDescriptionSDPString,
            fInfoSDPString,
            fMiscSDPLines);

        char *mediaSDP = sdp;
        for (subsession = fSubsessionsHead; subsession != NULL; subsession = subsession->fNext) {
            unsigned mediaSDPLength = strlen(mediaSDP);
            mediaSDP += mediaSDPLength;
            sdpLength -= mediaSDPLength;
            if (sdpLength <= 1) {
                break;
            }

            char const *sdpLines = subsession->sdpLines();
            if (sdpLines != NULL) {
                snprintf(mediaSDP, sdpLength, "%s", sdpLines);
            }
        }
    } while (0);

    delete[] rangeLine; delete[] sourceFilterLine;
    return sdp;
}

float ServerMediaSession::duration() const
{
    return 0.0;
}

void ServerMediaSession::removeClientSocket(ClientSocket *sock)
{
    ServerMediaSubsession *subsession;
    ServerMediaSubsessionIterator iter(*this);

    while ((subsession = iter.next()) != NULL) {
        if (subsession->removeClientSock(sock)) {
            break;
        }
    }
}

int ServerMediaSession::sendClientRtp(const char *trackId, char *buf, int len)
{
    int err = 0;
    ServerMediaSubsession *subsession;
    ServerMediaSubsessionIterator iter(*this);

    while ((subsession = iter.next()) != NULL) {
        if (strstr(trackId, subsession->trackId()) || strstr(subsession->trackId(), trackId)) {
            err = subsession->sendClientRtp(buf, len);
            break;
        }
    }

    return err;
}

int ServerMediaSession::sendClientRtcp(const char *trackId, char *buf, int len)
{
    int err = 0;
    ServerMediaSubsession *subsession;
    ServerMediaSubsessionIterator iter(*this);

    while ((subsession = iter.next()) != NULL) {
        if (strstr(trackId, subsession->trackId()) || strstr(subsession->trackId(), trackId)) {
            err = subsession->sendClientRtcp(buf, len);
            break;
        }
    }

    return err;
}

void ServerMediaSession::closeStreamControl()
{
    MUTEX_LOCK(&fMutex);
    DELETE_OBJECT(fStreamControl);
    MUTEX_UNLOCK(&fMutex);
}

int ServerMediaSession::startStream(double start_time)
{
    int err = -1;

    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        err = fStreamControl->startStream(start_time);
    }
    MUTEX_UNLOCK(&fMutex);

    return err;
}

void ServerMediaSession::stopStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->stopStream();
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::pauseStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->pauseStream();
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::resumeStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->resumeStream();
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::forwardStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->playDirStream(0);
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::backwardStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->playDirStream(1);
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::forwardNextStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->playNextStream(0);
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::backwardNextStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->playNextStream(1);
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::playContinueStream()
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->playContinueStream();
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::seekStream(int timestamp)
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->seekStream(timestamp);
    }
    MUTEX_UNLOCK(&fMutex);
}

void ServerMediaSession::speedStream(float speed)
{
    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        fStreamControl->speedStream(speed);
    }
    MUTEX_UNLOCK(&fMutex);
}

STREAM_STATE ServerMediaSession::streamState()
{
    STREAM_STATE state = STREAM_STATE_STOPPED;

    MUTEX_LOCK(&fMutex);
    if (fStreamControl) {
        state = fStreamControl->state();
    }
    MUTEX_UNLOCK(&fMutex);

    return state;
}

ServerMediaSubsessionIterator::ServerMediaSubsessionIterator(ServerMediaSession &session) : fOurSession(session)
{
    reset();
}

ServerMediaSubsessionIterator::~ServerMediaSubsessionIterator()
{

}

ServerMediaSubsession *ServerMediaSubsessionIterator::next()
{
    ServerMediaSubsession *result = fNextPtr;
    if (fNextPtr != NULL) {
        fNextPtr = fNextPtr->fNext;
    }

    return result;
}

void ServerMediaSubsessionIterator::reset()
{
    fNextPtr = fOurSession.fSubsessionsHead;
}

ServerMediaSubsession::ServerMediaSubsession(char const *trackId, char const *codec, unsigned char rtpPayload, unsigned timestampFreq)
    : fParentSession(NULL), fNext(NULL), fTrackNumber(0), fRTPPayloadType(rtpPayload), fTimestampFrequency(timestampFreq)
{
    fTrackId = strDup(trackId);
    fCodecName = strDup(codec);
}

ServerMediaSubsession::~ServerMediaSubsession()
{
    fClientSockList.clear();
    delete[] (char *)fTrackId;
    delete[] fCodecName;
    delete fNext;
}

char const *ServerMediaSubsession::trackId() 
{
    if (fTrackNumber == 0) {
        return NULL;
    }

    if (fTrackId == NULL) {
        char buf[100];
        sprintf(buf, "track%d", fTrackNumber);
        fTrackId = strDup(buf);
    }

    return fTrackId;
}

char const *ServerMediaSubsession::sdpLines()
{
    return fSDPLines;
}

void ServerMediaSubsession::getStreamParameters(
    unsigned clientSessionId,
    unsigned int clientAddress,
    unsigned short const &clientRTPPort,
    unsigned short const &clientRTCPPort,
    int tcpSocketNum,
    unsigned char rtpChannelId,
    unsigned char rtcpChannelId,
    unsigned int &destinationAddress,
    unsigned char &destinationTTL,
    bool& isMulticast,
    unsigned short &serverRTPPort,
    unsigned short &serverRTCPPort)
{
    if (destinationAddress == 0) {
        destinationAddress = clientAddress;
    }

    isMulticast = false;
    static int serverSockPort = RTSPCommonEnv::nServerPortRangeMin;

    if ((serverSockPort < RTSPCommonEnv::nServerPortRangeMin) || (serverSockPort > RTSPCommonEnv::nServerPortRangeMax)) {
        serverSockPort = RTSPCommonEnv::nServerPortRangeMin;
    }

    MUTEX_LOCK(&hMutex);
    while (1) {
        serverSockPort = (serverSockPort + 1) & ~1;
        if (serverSockPort > RTSPCommonEnv::nServerPortRangeMax) {
            serverSockPort = RTSPCommonEnv::nServerPortRangeMin;
        }

        if (CheckUdpPort(serverSockPort) == 0) {
            serverRTPPort = serverSockPort;
            serverRTCPPort = serverSockPort+1;
            serverSockPort += 2;
            break;
        }

        DPRINTF("Rtp port(%d) already used the other rtp port\r\n", serverSockPort);
        serverSockPort += 2;
    }
    MUTEX_UNLOCK(&hMutex);
}

float ServerMediaSubsession::getCurrentNPT()
{
    return 0.0;
}

float ServerMediaSubsession::duration() const
{
    return 0.0;
}

void ServerMediaSubsession::addClientSock(ClientSocket *sock)
{
    fClientSockList.lock();
    fClientSockList.insert(sock);
    fClientSockList.unlock();
}

bool ServerMediaSubsession::removeClientSock(ClientSocket *sock)
{
    bool ret = false;

    fClientSockList.lock();

    fClientSockList.gotoBeginCursor();
    ClientSocket *cursor = fClientSockList.getCursor();
    while (cursor) {
        if (cursor == sock) {
            fClientSockList.remove();
            ret = true;
            break;
        }

        fClientSockList.getNext();
        cursor = fClientSockList.getCursor();
    }

    fClientSockList.unlock();
    return ret;
}

int ServerMediaSubsession::sendClientRtp(char *buf, int len)
{
    int err = 0;

    fClientSockList.lock();

    fClientSockList.gotoBeginCursor();
    ClientSocket *cursor = fClientSockList.getNextCursor();
    while (cursor) {
        if (cursor->isActivated()) {
            if (cursor->sendRTP(buf, len) < 0) {
                err = WSAGetLastError();
                DPRINTF("rtp send error %d\n", err);
            }
        }
        cursor = fClientSockList.getNextCursor();
    }

    fClientSockList.unlock();
    return err;
}

int ServerMediaSubsession::sendClientRtcp(char *buf, int len)
{
    int err = 0;

    fClientSockList.lock();

    fClientSockList.gotoBeginCursor();
    ClientSocket *cursor = fClientSockList.getNextCursor();
    while (cursor) {
        if (cursor->isActivated()) {
            if (cursor->sendRTCP(buf, len) < 0) {
                err = WSAGetLastError();
                DPRINTF("rtcp send error %d\n", err);
            }
        }
        cursor = fClientSockList.getNextCursor();
    }

    fClientSockList.unlock();
    return err;
}
}
