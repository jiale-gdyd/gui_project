#include <ctype.h>
#include <rtsp/internal/util.h>
#include <rtsp/internal/MediaSession.h>
#include <rtsp/internal/AC3RTPSource.h>
#include <rtsp/internal/H264RTPSource.h>
#include <rtsp/internal/H265RTPSource.h>
#include <rtsp/internal/JPEGRTPSource.h>
#include <rtsp/internal/RTSPCommonEnv.h>
#include <rtsp/internal/MPEG4ESRTPSource.h>
#include <rtsp/internal/MPEG4GenericRTPSource.h>

namespace rtsp {
static pthread_mutex_t hMutex = PTHREAD_MUTEX_INITIALIZER;

static char *parseCLine(char const *sdpLine) 
{
    char *resultStr = NULL;
    char *buffer = strDupSize(sdpLine);

    if (sscanf(sdpLine, "c=IN IP4 %[^/ ]", buffer) == 1) {
        resultStr = strDup(buffer);
    }

    delete[] buffer;
    return resultStr;
}

MediaSession *MediaSession::createNew(char const *sdpDescription) 
{
    MediaSession *newSession = new MediaSession();
    if (newSession != NULL) {
        if (!newSession->initializeWithSDP(sdpDescription)) {
            delete newSession;
            return NULL;
        }
    }

    return newSession;
}

MediaSession::MediaSession()
    : fSubsessionsHead(NULL), fSubsessionsTail(NULL),
    fConnectionEndpointName(NULL), fMaxPlayStartTime(0.0f), fMaxPlayEndTime(0.0f),
    fScale(1.0f), fMediaSessionType(NULL), fSessionName(NULL), fSessionDescription(NULL),
    fControlPath(NULL)
{
    fSourceFilterAddr.s_addr = 0;

    const unsigned maxCNAMElen = 100;
    char CNAME[maxCNAMElen + 1];

#ifndef CRIS
    gethostname((char *)CNAME, maxCNAMElen);
#else
    sprintf(CNAME, "unknown host %d", (unsigned)(our_random()*0x7FFFFFFF));
#endif

    CNAME[maxCNAMElen] = '\0';
    fCNAME = strDup(CNAME);
}

MediaSession::~MediaSession()
{
    if (fSubsessionsHead) {
        delete fSubsessionsHead;
    }

    if (fCNAME) {
        delete[] fCNAME;
    }

    if (fConnectionEndpointName) {
        delete[] fConnectionEndpointName;
    }

    if (fMediaSessionType) {
        delete[] fMediaSessionType;
    }

    if (fSessionName) {
        delete[] fSessionName;
    }

    if (fSessionDescription) {
        delete[] fSessionDescription;
    }

    if (fControlPath) {
        delete[] fControlPath;
    }
}

bool MediaSession::initializeWithSDP(char const *sdpDescription) 
{
    if (sdpDescription == NULL) {
        return false;
    }

    char const *nextSDPLine;
    char const *sdpLine = sdpDescription;

    while (1)  {
        if (!parseSDPLine(sdpLine, nextSDPLine)) {
            return false;
        }

        if (sdpLine[0] == 'm') {
            break;
        }

        sdpLine = nextSDPLine;
        if (sdpLine == NULL) {
            break;
        }

        if (parseSDPLine_s(sdpLine)) {
            continue;
        }

        if (parseSDPLine_i(sdpLine)) {
            continue;
        }

        if (parseSDPLine_c(sdpLine)) {
            continue;
        }

        if (parseSDPAttribute_control(sdpLine)) {
            continue;
        }

        if (parseSDPAttribute_range(sdpLine)) {
            continue;
        }

        if (parseSDPAttribute_type(sdpLine)) {
            continue;
        }

        if (parseSDPAttribute_source_filter(sdpLine)) {
            continue;
        }
    }

    while (sdpLine != NULL)  {
        MediaSubsession *subsession = new MediaSubsession(*this);
        if (subsession == NULL) {
            DPRINTF("Unable to create new MediaSubsession\n");
            return false;
        }

        unsigned payloadFormat;
        char *protocolName = NULL;
        char *mediumName = strDupSize(sdpLine);

        if ((sscanf(sdpLine, "m=%s %hu RTP/AVP %u", mediumName, &subsession->fClientPortNum, &payloadFormat) == 3
            || sscanf(sdpLine, "m=%s %hu/%*u RTP/AVP %u", mediumName, &subsession->fClientPortNum, &payloadFormat) == 3)
            && payloadFormat <= 127)
        {
            protocolName = "RTP";
        } else if ((sscanf(sdpLine, "m=%s %hu UDP %u", mediumName, &subsession->fClientPortNum, &payloadFormat) == 3
            || sscanf(sdpLine, "m=%s %hu udp %u", mediumName, &subsession->fClientPortNum, &payloadFormat) == 3
            || sscanf(sdpLine, "m=%s %hu RAW/RAW/UDP %u", mediumName, &subsession->fClientPortNum, &payloadFormat) == 3)
            && payloadFormat <= 127)
        {
            protocolName = "UDP";
        } else {
            char *sdpLineStr;
            if (nextSDPLine == NULL) {
                sdpLineStr = (char*)sdpLine;
            } else {
                sdpLineStr = strDup(sdpLine);
                sdpLineStr[nextSDPLine - sdpLine] = '\0';
            }

            DPRINTF("Bad SDP \"m=\" linue: %s\n", sdpLineStr);
            if (sdpLineStr != (char *)sdpLine) {
                delete[] sdpLineStr;
            }

            delete[] mediumName;
            delete subsession;

            while (1) {
                sdpLine = nextSDPLine;
                if (sdpLine == NULL) {
                    break;
                }

                if (!parseSDPLine(sdpLine, nextSDPLine)) {
                    return false;
                }

                if (sdpLine[0] == 'm') {
                    break;
                }
            }

            continue;
        }

        if (fSubsessionsTail == NULL) {
            fSubsessionsHead = fSubsessionsTail = subsession;
        } else {
            fSubsessionsTail->setNext(subsession);
            fSubsessionsTail = subsession;
        }

        subsession->serverPortNum = subsession->fClientPortNum;

        char const *mStart = sdpLine;
        subsession->fSavedSDPLines = strDup(mStart);

        subsession->fMediumName = strDup(mediumName);
        delete[] mediumName;
        subsession->fProtocolName = strDup(protocolName);
        subsession->fRTPPayloadFormat = payloadFormat;

        while (1) {
            sdpLine = nextSDPLine;
            if (sdpLine == NULL) {
                break;
            }

            if (!parseSDPLine(sdpLine, nextSDPLine)) {
                return false;
            }

            if (sdpLine[0] == 'm') {
                break;
            }

            if (subsession->parseSDPLine_c(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_rtpmap(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_control(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_range(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_fmtp(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_source_filter(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_x_dimensions(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_framesize(sdpLine)) {
                continue;
            }

            if (subsession->parseSDPAttribute_framerate(sdpLine)) {
                continue;
            }
        }

        if (sdpLine != NULL) {
            subsession->fSavedSDPLines[sdpLine - mStart] = '\0';
        }

        if (subsession->fCodecName == NULL) {
            subsession->fCodecName = lookupPayloadFormat(subsession->fRTPPayloadFormat, subsession->fRTPTimestampFrequency, subsession->fNumChannels);
            if (subsession->fCodecName == NULL) {
                char typeStr[20];
                sprintf(typeStr, "%d", subsession->fRTPPayloadFormat);
                DPRINTF("Unknown codec name for RTP payload type %s", typeStr);
                return false;
            }
        }

        if (subsession->fRTPTimestampFrequency == 0) {
            subsession->fRTPTimestampFrequency = guessRTPTimestampFrequency(subsession->fMediumName, subsession->fCodecName);
        }
    }

    return true;
}

bool MediaSession::parseSDPLine(char const *inputLine, char const *&nextLine)
{
    nextLine = NULL;
    for (char const *ptr = inputLine; *ptr != '\0'; ++ptr) {
        if ((*ptr == '\r') || (*ptr == '\n')) {
            ++ptr;
            while ((*ptr == '\r') || (*ptr == '\n')) {
                ++ptr;
            }

            nextLine = ptr;
            if (nextLine[0] == '\0') {
                nextLine = NULL;
            }

            break;
        }
    }

    if ((inputLine[0] == '\r') || (inputLine[0] == '\n')) {
        return true;
    }

    if ((strlen(inputLine) < 2) || (inputLine[1] != '=') || (inputLine[0] < 'a') || (inputLine[0] > 'z')) {
        DPRINTF("Invalid SDP line: %s\n", inputLine);
        return false;
    }

    return true;
}

bool MediaSession::parseSDPLine_s(char const *sdpLine) 
{
    bool parseSuccess = false;
    char *buffer = strDupSize(sdpLine);

    if (sscanf(sdpLine, "s=%[^\r\n]", buffer) == 1) {
        delete[] fSessionName; fSessionName = strDup(buffer);
        parseSuccess = true;
    }

    delete[] buffer;
    return parseSuccess;
}

bool MediaSession::parseSDPLine_i(char const *sdpLine)
{
    bool parseSuccess = false;
    char *buffer = strDupSize(sdpLine);

    if (sscanf(sdpLine, "i=%[^\r\n]", buffer) == 1) {
        delete[] fSessionDescription; fSessionDescription = strDup(buffer);
        parseSuccess = true;
    }

    delete[] buffer;
    return parseSuccess;
}

bool MediaSession::parseSDPLine_c(char const *sdpLine)
{
    char *connectionEndpointName = parseCLine(sdpLine);
    if (connectionEndpointName != NULL) {
        delete[] fConnectionEndpointName;
        fConnectionEndpointName = connectionEndpointName;
        return true;
    }

    return false;
}

bool MediaSession::parseSDPAttribute_type(char const *sdpLine)
{
    bool parseSuccess = false;
    char *buffer = strDupSize(sdpLine);

    if (sscanf(sdpLine, "a=type: %[^ ]", buffer) == 1) {
        delete[] fMediaSessionType;
        fMediaSessionType = strDup(buffer);
        parseSuccess = true;
    }

    delete[] buffer;
    return parseSuccess;
}

static bool parseRangeAttribute(char const *sdpLine, double &startTime, double &endTime) 
{
    if (sscanf(sdpLine, "a=range: npt = %lg - %lg", &startTime, &endTime) != 2)  {
        return sscanf(sdpLine, "a=range:npt:%lg-%lg", &startTime, &endTime) == 2;
    }

    return true;
}

bool MediaSession::parseSDPAttribute_control(char const *sdpLine)
{
    bool parseSuccess = false;
    char *controlPath = strDupSize(sdpLine);

    if (sscanf(sdpLine, "a=control: %s", controlPath) == 1) {
        parseSuccess = true;
        delete[] fControlPath; fControlPath = strDup(controlPath);
    }

    delete[] controlPath;
    return parseSuccess;
}

bool MediaSession::parseSDPAttribute_range(char const *sdpLine)
{
    double playEndTime;
    double playStartTime;
    bool parseSuccess = false;

    if (parseRangeAttribute(sdpLine, playStartTime, playEndTime)) {
        parseSuccess = true;
        if (playStartTime > fMaxPlayStartTime) {
            fMaxPlayStartTime = playStartTime;
        }

        if (playEndTime > fMaxPlayEndTime) {
            fMaxPlayEndTime = playEndTime;
        }
    }

    return parseSuccess;
}

static bool parseSourceFilterAttribute(char const *sdpLine, struct in_addr &sourceAddr) 
{
    bool result = false;
    char *sourceName = strDupSize(sdpLine);

    do {
        if (sscanf(sdpLine, "a=source-filter: incl IN IP4 %*s %s", sourceName) != 1) {
            break;
        }

        sourceAddr.s_addr = inet_addr(sourceName);
        result = true;
    } while (0);

    delete[] sourceName;
    return result;
}

bool MediaSession::parseSDPAttribute_source_filter(char const *sdpLine)
{
    return parseSourceFilterAttribute(sdpLine, fSourceFilterAddr);
}

unsigned MediaSession::guessRTPTimestampFrequency(char const *mediumName, char const *codecName) 
{
    if (strcmp(codecName, "L16") == 0) {
        return 44100;
    }

    if (!strcmp(codecName, "MPA") || !strcmp(codecName, "MPA-ROBUST") || !strcmp(codecName, "X-MP3-DRAFT-00")) {
        return 90000;
    }

    if (strcmp(mediumName, "video") == 0) {
        return 90000;
    }

    return 8000;
}

char* MediaSession::lookupPayloadFormat(unsigned char rtpPayloadType, unsigned &freq, unsigned &nCh) 
{
    char *temp = NULL;

    switch (rtpPayloadType) {
        case 0: {temp = "PCMU"; freq = 8000; nCh = 1; break;}
        case 2: {temp = "G726-32"; freq = 8000; nCh = 1; break;}
        case 3: {temp = "GSM"; freq = 8000; nCh = 1; break;}
        case 4: {temp = "G723"; freq = 8000; nCh = 1; break;}
        case 5: {temp = "DVI4"; freq = 8000; nCh = 1; break;}
        case 6: {temp = "DVI4"; freq = 16000; nCh = 1; break;}
        case 7: {temp = "LPC"; freq = 8000; nCh = 1; break;}
        case 8: {temp = "PCMA"; freq = 8000; nCh = 1; break;}
        case 9: {temp = "G722"; freq = 8000; nCh = 1; break;}
        case 10: {temp = "L16"; freq = 44100; nCh = 2; break;}
        case 11: {temp = "L16"; freq = 44100; nCh = 1; break;}
        case 12: {temp = "QCELP"; freq = 8000; nCh = 1; break;}
        case 14: {temp = "MPA"; freq = 90000; nCh = 1; break;}

        case 15: {temp = "G728"; freq = 8000; nCh = 1; break;}
        case 16: {temp = "DVI4"; freq = 11025; nCh = 1; break;}
        case 17: {temp = "DVI4"; freq = 22050; nCh = 1; break;}
        case 18: {temp = "G729"; freq = 8000; nCh = 1; break;}
        case 25: {temp = "CELB"; freq = 90000; nCh = 1; break;}
        case 26: {temp = "JPEG"; freq = 90000; nCh = 1; break;}
        case 28: {temp = "NV"; freq = 90000; nCh = 1; break;}
        case 31: {temp = "H261"; freq = 90000; nCh = 1; break;}
        case 32: {temp = "MPV"; freq = 90000; nCh = 1; break;}
        case 33: {temp = "MP2T"; freq = 90000; nCh = 1; break;}
        case 34: {temp = "H263"; freq = 90000; nCh = 1; break;}
    };

    return strDup(temp);
}

MediaSubsessionIterator::MediaSubsessionIterator(MediaSession &session) : fOurSession(session) 
{
    reset();
}

MediaSubsessionIterator::~MediaSubsessionIterator() 
{

}

MediaSubsession *MediaSubsessionIterator::next() 
{
    MediaSubsession *result = fNextPtr;
    if (fNextPtr != NULL) {
        fNextPtr = fNextPtr->fNext;
    }

    return result;
}

void MediaSubsessionIterator::reset() 
{
    fNextPtr = fOurSession.fSubsessionsHead;
}

MediaSubsession::MediaSubsession(MediaSession &parent)
    : sessionId(NULL), serverPortNum(0),
    fParent(parent), fNext(NULL),
    fConnectionEndpointName(NULL),
    fClientPortNum(0), fRTPPayloadFormat(0xFF),
    fSavedSDPLines(NULL), fMediumName(NULL), fCodecName(NULL), fProtocolName(NULL),
    fRTPTimestampFrequency(0), fControlPath(NULL),
    fSourceFilterAddr(parent.sourceFilterAddr()),
    fAuxiliarydatasizelength(0), fConstantduration(0), fConstantsize(0),
    fCRC(0), fCtsdeltalength(0), fDe_interleavebuffersize(0), fDtsdeltalength(0),
    fIndexdeltalength(0), fIndexlength(0), fInterleaving(0), fMaxdisplacement(0),
    fObjecttype(0), fOctetalign(0), fProfile_level_id(0), fRobustsorting(0),
    fSizelength(0), fStreamstateindication(0), fStreamtype(0),
    fCpresent(false), fRandomaccessindication(false),
    fConfig(NULL), fMode(NULL), fSpropParameterSets(NULL),
    fPlayStartTime(0.0), fPlayEndTime(0.0),
    fVideoWidth(0), fVideoHeight(0), fVideoFPS(0), fNumChannels(1), fScale(1.0f), fNPT_PTS_Offset(0.0f),
    fRTPSource(NULL)
{
    rtpInfo.seqNum = 0; rtpInfo.timestamp = 0; rtpInfo.infoIsNew = false;
}

MediaSubsession::~MediaSubsession() 
{
    deInitiate();

    delete[] fConnectionEndpointName;
    delete[] fSavedSDPLines;
    delete[] fMediumName;
    delete[] fCodecName;
    delete[] fProtocolName;
    delete[] fControlPath;
    delete[] fConfig;
    delete[] fMode;
    delete[] fSpropParameterSets;

    if (sessionId) {
        delete[] sessionId;
    }

    delete fNext;
}

double MediaSubsession::playStartTime() const 
{
    if (fPlayStartTime > 0) {
        return fPlayStartTime;
    }

    return fParent.playStartTime();
}

double MediaSubsession::playEndTime() const 
{
    if (fPlayEndTime > 0) {
        return fPlayEndTime;
    }

    return fParent.playEndTime();
}

bool MediaSubsession::initiate(int streamType, TaskScheduler &task, bool rtpOnly) 
{
    static int clientSockPort = RTSPCommonEnv::nClientPortRangeMin;

    if ((clientSockPort < RTSPCommonEnv::nClientPortRangeMin) || (clientSockPort > RTSPCommonEnv::nClientPortRangeMax)) {
        clientSockPort = RTSPCommonEnv::nClientPortRangeMin;
    }

    MUTEX_LOCK(&hMutex);

    while (1) {
        clientSockPort = (clientSockPort+1)&~1;
        if (clientSockPort > RTSPCommonEnv::nClientPortRangeMax) {
            clientSockPort = RTSPCommonEnv::nClientPortRangeMin;
        }

        if (CheckUdpPort(clientSockPort) == 0) {
            fClientPortNum = clientSockPort;
            clientSockPort += 2;
            break;
        }

        DPRINTF("Rtp port(%d) already used the other rtp port\r\n", clientSockPort);
        clientSockPort += 2;
    }

    MUTEX_UNLOCK(&hMutex);

    if (strcmp(fProtocolName, "RTP") == 0) {
        if (rtpOnly) {
            fRTPSource = new RTPSource(streamType, *this, task);
        } else {
            if (strcmp(fCodecName, "H264") == 0) {
                fRTPSource = new H264RTPSource(streamType, *this, task);
            } else if (strcmp(fCodecName, "H265") == 0) {
                fRTPSource = new H265RTPSource(streamType, *this, task);
            } else if (strcmp(fCodecName, "MP4V-ES") == 0) {
                fRTPSource = new MPEG4ESRTPSource(streamType, *this, task);
            } else if (strcmp(fCodecName, "MPEG4-GENERIC") == 0) {
                fRTPSource = new MPEG4GenericRTPSource(streamType, *this, task, fMode, fSizelength, fIndexlength, fIndexdeltalength);
            } else if (strcmp(fCodecName, "JPEG") == 0) {
                fRTPSource = new JPEGRTPSource(streamType, *this, task);
            } else if (strcmp(fCodecName, "AC3") == 0) {
                fRTPSource = new AC3RTPSource(streamType, *this, task);
            } else if (strcmp(fCodecName, "PCMU") == 0
                || strcmp(fCodecName, "GSM") == 0
                || strcmp(fCodecName, "DVI4") == 0
                || strcmp(fCodecName, "PCMA") == 0
                || strcmp(fCodecName, "MP1S") == 0
                || strcmp(fCodecName, "MP2P") == 0
                || strcmp(fCodecName, "L8") == 0
                || strcmp(fCodecName, "L16") == 0
                || strcmp(fCodecName, "L20") == 0
                || strcmp(fCodecName, "L24") == 0
                || strcmp(fCodecName, "G726-16") == 0
                || strcmp(fCodecName, "G726-24") == 0
                || strcmp(fCodecName, "G726-32") == 0
                || strcmp(fCodecName, "G726-40") == 0
                || strcmp(fCodecName, "SPEEX") == 0
                || strcmp(fCodecName, "ILBC") == 0
                || strcmp(fCodecName, "OPUS") == 0
                || strcmp(fCodecName, "T140") == 0
                || strcmp(fCodecName, "DAT12") == 0
                || strcmp(fCodecName, "VND.ONVIF.METADATA") == 0) 
            {
                fRTPSource = new RTPSource(streamType, *this, task);
            } else {
                return false;
            }
        }
    } else {
        return false;
    }

    return true;
}

void MediaSubsession::deInitiate()
{
    if (fRTPSource) {
        delete fRTPSource;
        fRTPSource = NULL;
    }
}

bool MediaSubsession::setClientPortNum(unsigned short portNum) 
{
    fClientPortNum = portNum;
    return true;
}

unsigned int MediaSubsession::connectionEndpointAddress() const
{
    do {
        char const *endpointString = connectionEndpointName();
        if (endpointString == NULL) {
            endpointString = parentSession().connectionEndpointName();
        }

        if (endpointString == NULL) {
            break;
        }

        return inet_addr(endpointString);
    } while (0);

    return 0;
}

void MediaSubsession::setDestinations(unsigned int defaultDestAddress)
{
    unsigned int destAddress = connectionEndpointAddress();
    if (destAddress == 0) {
        destAddress = defaultDestAddress;
    }

    struct in_addr destAddr;
    destAddr.s_addr = destAddress;

    if (fRTPSource) {
        fRTPSource->changeDestination(destAddr, serverPortNum);
    }
}

bool MediaSubsession::parseSDPLine_c(char const *sdpLine) 
{
    char *connectionEndpointName = parseCLine(sdpLine);
    if (connectionEndpointName != NULL) {
        delete[] fConnectionEndpointName;
        fConnectionEndpointName = connectionEndpointName;
        return true;
    }

    return false;
}

bool MediaSubsession::parseSDPAttribute_rtpmap(char const *sdpLine)
{
    unsigned numChannels = 1;
    bool parseSuccess = false;
    unsigned rtpmapPayloadFormat;
    unsigned rtpTimestampFrequency = 0;
    char *codecName = strDupSize(sdpLine);

    if (sscanf(sdpLine, "a=rtpmap: %u %[^/]/%u/%u", &rtpmapPayloadFormat, codecName, &rtpTimestampFrequency, &numChannels) == 4
        || sscanf(sdpLine, "a=rtpmap: %u %[^/]/%u", &rtpmapPayloadFormat, codecName, &rtpTimestampFrequency) == 3
        || sscanf(sdpLine, "a=rtpmap: %u %s", &rtpmapPayloadFormat, codecName) == 2)
    {
        parseSuccess = true;
        if (rtpmapPayloadFormat == fRTPPayloadFormat) {
            {
                for (char *p = codecName; *p != '\0'; ++p) {
                    *p = toupper(*p);
                }
            }

            delete[] fCodecName;
            fCodecName = strDup(codecName);
            fRTPTimestampFrequency = rtpTimestampFrequency;
            fNumChannels = numChannels;
        }
    }

    delete[] codecName;
    return parseSuccess;
}

bool MediaSubsession::parseSDPAttribute_control(char const *sdpLine)
{
    bool parseSuccess = false;
    char *controlPath = strDupSize(sdpLine);

    if (sscanf(sdpLine, "a=control: %s", controlPath) == 1) {
        parseSuccess = true;
        delete[] fControlPath; fControlPath = strDup(controlPath);
    }

    delete[] controlPath;
    return parseSuccess;
}

bool MediaSubsession::parseSDPAttribute_range(char const *sdpLine) 
{
    double playEndTime;
    double playStartTime;
    bool parseSuccess = false;

    if (parseRangeAttribute(sdpLine, playStartTime, playEndTime)) {
        parseSuccess = true;
        if (playStartTime > fPlayStartTime) {
            fPlayStartTime = playStartTime;
            if (playStartTime > fParent.playStartTime()) {
                fParent.playStartTime() = playStartTime;
            }
        }

        if (playEndTime > fPlayEndTime) {
            fPlayEndTime = playEndTime;
            if (playEndTime > fParent.playEndTime()) {
                fParent.playEndTime() = playEndTime;
            }
        }
    }

    return parseSuccess;
}

bool MediaSubsession::parseSDPAttribute_fmtp(char const *sdpLine) 
{
    do {
        if (strncmp(sdpLine, "a=fmtp:", 7) != 0) {
            break;
        }

        sdpLine += 7;
        while (isdigit(*sdpLine)) {
            ++sdpLine;
        }

        char *const lineCopy = strDup(sdpLine);
        char *line = lineCopy;

        {
            for (char *c = line; *c != '\0'; ++c) {
                *c = tolower(*c);
            }
        }

        while ((*line != '\0') && (*line != '\r') && (*line != '\n')) {
            unsigned u;
            char *valueStr = strDupSize(line);

            if (sscanf(line, " auxiliarydatasizelength = %u", &u) == 1) {
                fAuxiliarydatasizelength = u;
            } else if (sscanf(line, " constantduration = %u", &u) == 1) {
                fConstantduration = u;
            } else if (sscanf(line, " constantsize; = %u", &u) == 1) {
                fConstantsize = u;
            } else if (sscanf(line, " crc = %u", &u) == 1) {
                fCRC = u;
            } else if (sscanf(line, " ctsdeltalength = %u", &u) == 1) {
                fCtsdeltalength = u;
            } else if (sscanf(line, " de-interleavebuffersize = %u", &u) == 1) {
                fDe_interleavebuffersize = u;
            } else if (sscanf(line, " dtsdeltalength = %u", &u) == 1) {
                fDtsdeltalength = u;
            } else if (sscanf(line, " indexdeltalength = %u", &u) == 1) {
                fIndexdeltalength = u;
            } else if (sscanf(line, " indexlength = %u", &u) == 1) {
                fIndexlength = u;
            } else if (sscanf(line, " interleaving = %u", &u) == 1) {
                fInterleaving = u;
            } else if (sscanf(line, " maxdisplacement = %u", &u) == 1) {
                fMaxdisplacement = u;
            } else if (sscanf(line, " objecttype = %u", &u) == 1) {
                fObjecttype = u;
            } else if (sscanf(line, " octet-align = %u", &u) == 1) {
                fOctetalign = u;
            } else if (sscanf(line, " profile-level-id = %x", &u) == 1) {
                fProfile_level_id = u;
            } else if (sscanf(line, " robust-sorting = %u", &u) == 1) {
                fRobustsorting = u;
            } else if (sscanf(line, " sizelength = %u", &u) == 1) {
                fSizelength = u;
            } else if (sscanf(line, " streamstateindication = %u", &u) == 1) {
                fStreamstateindication = u;
            } else if (sscanf(line, " streamtype = %u", &u) == 1) {
                fStreamtype = u;
            } else if (sscanf(line, " cpresent = %u", &u) == 1) {
                fCpresent = u != 0;
            } else if (sscanf(line, " randomaccessindication = %u", &u) == 1) {
                fRandomaccessindication = u != 0;
            } else if (sscanf(line, " config = %[^; \t\r\n]", valueStr) == 1) {
                delete[] fConfig;
                fConfig = strDup(valueStr);
            } else if (sscanf(line, " mode = %[^; \t\r\n]", valueStr) == 1) {
                delete[] fMode;
                fMode = strDup(valueStr);
            } else if (sscanf(sdpLine, " sprop-parameter-sets = %[^; \t\r\n]", valueStr) == 1) {
                delete[] fSpropParameterSets;
                fSpropParameterSets = strDup(valueStr);
            } else {
                if (sscanf(line, " %[^; \t\r\n]", valueStr) == 1) {
                    if (strcmp(valueStr, "octet-align") == 0) {
                        fOctetalign = 1;
                    } else if (strcmp(valueStr, "cpresent") == 0) {
                        fCpresent = true;
                    } else if (strcmp(valueStr, "crc") == 0) {
                        fCRC = 1;
                    } else if (strcmp(valueStr, "robust-sorting") == 0) {
                        fRobustsorting = 1;
                    } else if (strcmp(valueStr, "randomaccessindication") == 0) {
                        fRandomaccessindication = true;
                    }
                }
            }

            delete[] valueStr;

            while ((*line != '\0') && (*line != '\r') && (*line != '\n') && (*line != ';')) {
                ++line;
            }

            while (*line == ';') {
                ++line;
            }

            while ((*sdpLine != '\0') && (*sdpLine != '\r') && (*sdpLine != '\n') && (*sdpLine != ';')) {
                ++sdpLine;
            }

            while (*sdpLine == ';') {
                ++sdpLine;
            }
        }

        delete[] lineCopy;
        return true;
    } while (0);

    return false;
}

bool MediaSubsession::parseSDPAttribute_source_filter(char const *sdpLine)
{
    return parseSourceFilterAttribute(sdpLine, fSourceFilterAddr);
}

bool MediaSubsession::parseSDPAttribute_x_dimensions(char const *sdpLine) 
{
    int width, height;
    bool parseSuccess = false;

    if (sscanf(sdpLine, "a=x-dimensions:%d,%d", &width, &height) == 2) {
        parseSuccess = true;
        fVideoWidth = (unsigned short)width;
        fVideoHeight = (unsigned short)height;
    }

    return parseSuccess;
}

bool MediaSubsession::parseSDPAttribute_framesize(char const *sdpLine) 
{
    int x, y;
    int payload;
    int width, height;
    bool parseSuccess = false;

    if (sscanf(sdpLine, "a=framesize:%d %d-%d", &payload, &width, &height) == 3) {
        parseSuccess = true;
        fVideoWidth = (unsigned short)width;
        fVideoHeight = (unsigned short)height;
    } else if (sscanf(sdpLine, "a=cliprect:%d,%d,%d,%d", &x, &y, &height, &width) == 4) {
        parseSuccess = true;
        fVideoWidth = (unsigned short)width;
        fVideoHeight = (unsigned short)height;
    } else {
        if (sscanf(sdpLine, "a=Width:integer;%d", &width) == 1) {
            parseSuccess = true;
            fVideoWidth = (unsigned short)width;
        }

        if (sscanf(sdpLine, "a=Height:integer;%d", &height) == 1) {
            parseSuccess = true;
            fVideoHeight = (unsigned short)height;
        }
    }

    return parseSuccess;
}

bool MediaSubsession::parseSDPAttribute_framerate(char const *sdpLine) 
{
    int rate;
    float frate;
    bool parseSuccess = false;

    if (sscanf(sdpLine, "a=framerate: %f", &frate) == 1) {
        parseSuccess = true;
        fVideoFPS = (unsigned)frate;
    } else if (sscanf(sdpLine, "a=x-framerate: %d", &rate) == 1) {
        parseSuccess = true;
        fVideoFPS = (unsigned)rate;
    }

    return parseSuccess;
}
}
