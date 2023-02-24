#ifndef RTSP_INTERNAL_MEDIA_SESSION_H
#define RTSP_INTERNAL_MEDIA_SESSION_H

#include <stdio.h>
#include "RTPSource.h"

namespace rtsp {
class MediaSubsession;

class MediaSession {
public:
    static MediaSession *createNew(char const *sdpDescription);
    virtual ~MediaSession();

    bool hasSubsessions() const {
        return fSubsessionsHead != NULL;
    }

    double &playStartTime() {
        return fMaxPlayStartTime;
    }

    double &playEndTime() {
        return fMaxPlayEndTime;
    }

    char *connectionEndpointName() const {
        return fConnectionEndpointName;
    }

    char const *CNAME() const {
        return fCNAME;
    }

    struct in_addr const& sourceFilterAddr() const {
        return fSourceFilterAddr;
    }

    float &scale() {
        return fScale;
    }

    char *mediaSessionType() const {
        return fMediaSessionType;
    }

    char *sessionName() const {
        return fSessionName;
    }

    char *sessionDescription() const {
        return fSessionDescription;
    }

    char const *controlPath() const {
        return fControlPath;
    }

protected:
    MediaSession();

    bool initializeWithSDP(char const *sdpDescription);
    bool parseSDPLine(char const *input, char const *&nextLine);
    bool parseSDPLine_s(char const *sdpLine);
    bool parseSDPLine_i(char const *sdpLine);
    bool parseSDPLine_c(char const *sdpLine);
    bool parseSDPAttribute_type(char const *sdpLine);
    bool parseSDPAttribute_control(char const *sdpLine);
    bool parseSDPAttribute_range(char const *sdpLine);
    bool parseSDPAttribute_source_filter(char const *sdpLine);

    static char *lookupPayloadFormat(unsigned char rtpPayloadType, unsigned &rtpTimestampFrequency, unsigned &numChannels);
    static unsigned guessRTPTimestampFrequency(char const *mediumName, char const *codecName);

protected:
    friend class MediaSubsessionIterator;

    char            *fCNAME;

    MediaSubsession *fSubsessionsHead;
    MediaSubsession *fSubsessionsTail;

    char            *fConnectionEndpointName;
    double          fMaxPlayStartTime;
    double          fMaxPlayEndTime;
    struct in_addr  fSourceFilterAddr;
    float           fScale;
    char            *fMediaSessionType;
    char            *fSessionName;
    char            *fSessionDescription;
    char            *fControlPath;
};

class MediaSubsessionIterator  {
public:
    MediaSubsessionIterator(MediaSession &session);
    virtual ~MediaSubsessionIterator();

    MediaSubsession *next();
    void reset();

private:
    MediaSession    &fOurSession;
    MediaSubsession *fNextPtr;
};

class MediaSubsession  {
public:
    MediaSession &parentSession() {
        return fParent;
    }

    MediaSession const &parentSession() const {
        return fParent;
    }

    unsigned short clientPortNum() const {
        return fClientPortNum;
    }

    unsigned char rtpPayloadFormat() const {
        return fRTPPayloadFormat;
    }

    char const *savedSDPLines() const {
        return fSavedSDPLines;
    }

    char const *mediumName() const {
        return fMediumName;
    }

    char const *codecName() const {
        return fCodecName;
    }

    char const *protocolName() const {
        return fProtocolName;
    }

    char const *controlPath() const {
        return fControlPath;
    }

    bool isSSM() const {
        return fSourceFilterAddr.s_addr != 0;
    }

    unsigned short videoWidth() const {
        return fVideoWidth;
    }

    unsigned short videoHeight() const {
        return fVideoHeight;
    }

    unsigned videoFPS() const {
        return fVideoFPS;
    }

    unsigned numChannels() const {
        return fNumChannels;
    }

    float& scale() {
        return fScale;
    }

    unsigned rtpTimestampFrequency() const {
        return fRTPTimestampFrequency;
    }

    double playStartTime() const;
    double playEndTime() const;

    double &_playStartTime() {
        return fPlayStartTime;
    }

    double &_playEndTime() {
        return fPlayEndTime;
    }

    bool initiate(int streamType, TaskScheduler &task, bool rtpOnly = false);
    void deInitiate();

    bool setClientPortNum(unsigned short portNum);

    char *&connectionEndpointName() {
        return fConnectionEndpointName;
    }

    char const *connectionEndpointName() const {
        return fConnectionEndpointName;
    }

    unsigned fmtp_auxiliarydatasizelength() const {
        return fAuxiliarydatasizelength;
    }

    unsigned fmtp_constantduration() const {
        return fConstantduration;
    }

    unsigned fmtp_constantsize() const {
        return fConstantsize;
    }

    unsigned fmtp_crc() const {
        return fCRC;
    }

    unsigned fmtp_ctsdeltalength() const {
        return fCtsdeltalength;
    }

    unsigned fmtp_de_interleavebuffersize() const {
        return fDe_interleavebuffersize;
    }

    unsigned fmtp_dtsdeltalength() const {
        return fDtsdeltalength;
    }

    unsigned fmtp_indexdeltalength() const {
        return fIndexdeltalength;
    }

    unsigned fmtp_indexlength() const {
        return fIndexlength;
    }

    unsigned fmtp_interleaving() const {
        return fInterleaving;
    }

    unsigned fmtp_maxdisplacement() const {
        return fMaxdisplacement;
    }

    unsigned fmtp_objecttype() const {
        return fObjecttype;
    }

    unsigned fmtp_octetalign() const {
        return fOctetalign;
    }

    unsigned fmtp_profile_level_id() const {
        return fProfile_level_id;
    }

    unsigned fmtp_robustsorting() const {
        return fRobustsorting;
    }

    unsigned fmtp_sizelength() const {
        return fSizelength;
    }

    unsigned fmtp_streamstateindication() const {
        return fStreamstateindication;
    }

    unsigned fmtp_streamtype() const {
        return fStreamtype;
    }

    bool fmtp_cpresent() const {
        return fCpresent;
    }

    bool fmtp_randomaccessindication() const {
        return fRandomaccessindication;
    }

    char const *fmtp_config() const {
        return fConfig;
    }

    char const *fmtp_mode() const {
        return fMode;
    }

    char const *fmtp_spropparametersets() const {
        return fSpropParameterSets;
    }

    unsigned int connectionEndpointAddress() const;
    void setDestinations(unsigned int defaultDestAddress);

    char const     *sessionId;
    unsigned short serverPortNum;
    unsigned char  rtpChannelId, rtcpChannelId;

    struct {
        unsigned short seqNum;
        unsigned int   timestamp;
        bool           infoIsNew;
    } rtpInfo;

    double getNormalPlayTime(struct timeval const &presentationTime);

    RTPSource *fRTPSource;

protected:
    friend class MediaSession;
    friend class MediaSubsessionIterator;

    MediaSubsession(MediaSession &parent);
    virtual ~MediaSubsession();

    void setNext(MediaSubsession *next) {
        fNext = next;
    }

    bool parseSDPLine_c(char const *sdpLine);
    bool parseSDPAttribute_rtpmap(char const *sdpLine);
    bool parseSDPAttribute_control(char const *sdpLine);
    bool parseSDPAttribute_range(char const *sdpLine);
    bool parseSDPAttribute_fmtp(char const *sdpLine);
    bool parseSDPAttribute_source_filter(char const *sdpLine);
    bool parseSDPAttribute_x_dimensions(char const *sdpLine);
    bool parseSDPAttribute_framesize(char const *sdpLine);  
    bool parseSDPAttribute_framerate(char const *sdpLine);

protected:
    MediaSession    &fParent;
    MediaSubsession *fNext;

    char            *fConnectionEndpointName;
    unsigned short  fClientPortNum;
    unsigned char   fRTPPayloadFormat;
    char            *fSavedSDPLines;
    char            *fMediumName;
    char            *fCodecName;
    char            *fProtocolName;
    unsigned        fRTPTimestampFrequency;
    char            *fControlPath;
    struct in_addr  fSourceFilterAddr;

    unsigned        fAuxiliarydatasizelength, fConstantduration, fConstantsize;
    unsigned        fCRC, fCtsdeltalength, fDe_interleavebuffersize, fDtsdeltalength;
    unsigned        fIndexdeltalength, fIndexlength, fInterleaving;
    unsigned        fMaxdisplacement, fObjecttype;
    unsigned        fOctetalign, fProfile_level_id, fRobustsorting;
    unsigned        fSizelength, fStreamstateindication, fStreamtype;
    bool            fCpresent, fRandomaccessindication;
    char            *fConfig, *fMode, *fSpropParameterSets;

    double          fPlayStartTime;
    double          fPlayEndTime;
    unsigned short  fVideoWidth, fVideoHeight;
    unsigned        fVideoFPS;
    unsigned        fNumChannels;
    float           fScale;
    double          fNPT_PTS_Offset;
};
}

#endif
