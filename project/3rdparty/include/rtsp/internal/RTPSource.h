#ifndef RTSP_INTERNAL_RTP_SOURCE_H
#define RTSP_INTERNAL_RTP_SOURCE_H

#include "Sock.h"
#include "RTCP.h"
#include "RTCPInstance.h"
#include "TaskScheduler.h"
#include "RTPPacketBuffer.h"

namespace rtsp {
#define MAX_RTP_SIZE                (15000)
#define FRAME_BUFFER_SIZE           (1024*1024*4)

#define RTCP_SEND_DURATION          (2)

enum RTP_FRAME_TYPE {
    FRAME_TYPE_VIDEO,
    FRAME_TYPE_AUDIO,
    FRAME_TYPE_ETC
};

typedef void (*RTPHandlerFunc)(void *arg, char *trackId, char *buf, int len);
typedef void (*FrameHandlerFunc)(void *arg, RTP_FRAME_TYPE frame_type, int64_t timestamp, uint8_t *buf, int len);

class MediaSubsession;

class RTPSource {
public:
    RTPSource(int streamType, MediaSubsession &subsession, TaskScheduler &task); 
    virtual ~RTPSource();

    void startNetworkReading(FrameHandlerFunc frameHandler, void *frameHandlerData, RTPHandlerFunc rtpHandler, void *rtpHandlerData, RTPHandlerFunc rtcpHandler, void *rtcpHandlerData);
    void stopNetworkReading();

    void rtpReadHandler(char *buf, int len, struct sockaddr_in &fromAddress);
    void rtcpReadHandler(char *buf, int len, struct sockaddr_in &fromAddress);

    RTPReceptionStatsDB &receptionStatsDB() const {
        return *fReceptionStatsDB;
    }

    u_int32_t SSRC() const {
        return fSSRC;
    }

    void setRtspSock(MySock *rtspSock);
    void setServerPort(uint16_t serverPort);
    void setRtcpChannelId(unsigned char rtcpChannelId);
    void sendRtcpReport (char *buf, int len);

    void changeDestination(struct in_addr const &newDestAddr, short newDestPort);

protected:
    static void incomingRtpPacketHandler(void *, int);
    void incomingRtpPacketHandler1();

    static void incomingRtcpPacketHandler(void *, int);
    void incomingRtcpPacketHandler1();	

    virtual void processFrame(RTPPacketBuffer *packet);

    void processNextPacket();

protected:
    void copyToFrameBuffer(uint8_t *buf, int len);
    void resetFrameBuf();

protected:
    uint64_t getMediaTimestamp(uint32_t timestamp);

protected:
    ReorderingPacketBuffer *fReorderingBuffer;

    uint8_t                fRTPPayloadFormat;
    uint32_t               fTimestampFrequency;
    uint32_t               fSSRC;
    uint32_t               fLastTimestamp;
    uint16_t               fLastSeqNum, fLastSeqNum2;
    char                   *fCodecName;
    char                   *fTrackId;
    RTP_FRAME_TYPE         fFrameType;

    int                    fStreamType;

    bool                   fIsStartFrame;
    bool                   fBeginFrame;
    uint8_t                *fExtraData;
    unsigned               fExtraDataSize;

    RTPReceptionStatsDB    *fReceptionStatsDB;
    RTCPInstance           *fRtcpInstance;
    uint32_t               fSvrAddr;
    time_t                 fLastRtcpSendTime;
    
    uint8_t                *fFrameBuf;
    int                    fFrameBufPos;
    FrameHandlerFunc       fFrameHandlerFunc;
    void                   *fFrameHandlerFuncData;

    MySock                 *fRtspSock;
    uint8_t                fRtcpChannelId;

    MySock                 fRtpSock;
    MySock                 fRtcpSock;
    uint16_t               fRtcpHisPort;
    TaskScheduler          *fTask;
    char                   *fRecvBuf;

    RTPHandlerFunc         fRtpHandlerFunc;
    void                   *fRtpHandlerFuncData;

    RTPHandlerFunc         fRtcpHandlerFunc;
    void                   *fRtcpHandlerFuncData;
};
}

#endif
