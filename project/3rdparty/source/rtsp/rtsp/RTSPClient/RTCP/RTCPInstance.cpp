#include <rtsp/internal/util.h>
#include <rtsp/internal/RTPSource.h>
#include <rtsp/internal/RTCPInstance.h>
#include <rtsp/internal/RTSPCommonEnv.h>
#include <rtsp/internal/rtcp_from_spec.h>

namespace rtsp {
class RTCPMemberDatabase {
public:
    RTCPMemberDatabase(RTCPInstance &ourRTCPInstance) : fOurRTCPInstance(ourRTCPInstance), fNumMembers(1), fTable(HashTable::create(ONE_WORD_HASH_KEYS)) {

    }

    virtual ~RTCPMemberDatabase() {
        delete fTable;
    }

    bool isMember(unsigned ssrc) const {
        return fTable->Lookup((char *)(long)ssrc) != NULL;
    }

    bool noteMembership(unsigned ssrc, unsigned curTimeCount) {
        bool isNew = !isMember(ssrc);

        if (isNew) {
            ++fNumMembers;
        }

        fTable->Add((char *)(long)ssrc, (void *)(long)curTimeCount);
        return isNew;
    }

    bool remove(unsigned ssrc) {
        bool wasPresent = fTable->Remove((char *)(long)ssrc);
        if (wasPresent) {
            --fNumMembers;
        }

        return wasPresent;
    }

    unsigned numMembers() const {
        return fNumMembers;
    }

    void reapOldMembers(unsigned threshold);

private:
    RTCPInstance &fOurRTCPInstance;
    unsigned     fNumMembers;
    HashTable    *fTable;
};

void RTCPMemberDatabase::reapOldMembers(unsigned threshold)
{
    bool foundOldMember;
    unsigned oldSSRC = 0;

    do {
        foundOldMember = false;

        char const *key;
        unsigned long timeCount;
        HashTable::Iterator *iter = HashTable::Iterator::create(*fTable);

        while ((timeCount = (unsigned long)(iter->next(key))) != 0) {
            if (timeCount < (unsigned long)threshold) {
                unsigned long ssrc = (unsigned long)key;
                oldSSRC = (unsigned)ssrc;
                foundOldMember = true;
            }
        }

        delete iter;

        if (foundOldMember) {
            fOurRTCPInstance.removeSSRC(oldSSRC, true);
        }
    } while (foundOldMember);
}

static double dTimeNow()
{
    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);
    return (double) (timeNow.tv_sec + timeNow.tv_usec / 1000000.0);
}

static unsigned const maxPacketSize = 1450;
static unsigned const preferredPacketSize = 1000;

RTCPInstance::RTCPInstance(unsigned totSessionBW, unsigned char const *cname, RTPSource *source)
    : fTotSessionBW(totSessionBW),
    fSource(source),
    fCNAME(RTCP_SDES_CNAME, cname), fOutgoingReportCount(1),
    fAveRTCPSize(0), fIsInitial(1), fPrevNumMembers(0),
    fLastSentSize(0), fLastReceivedSize(0), fLastReceivedSSRC(0),
    fTypeOfEvent(EVENT_UNKNOWN), fTypeOfPacket(PACKET_UNKNOWN_TYPE),
    fHaveJustSentPacket(false), fLastPacketSentSize(0)
{
    if (fTotSessionBW == 0) {
        DPRINTF("RTCPInstance::RTCPInstance error: totSessionBW parameter should not be zero!\n");
        fTotSessionBW = 1;
    }

    double timeNow = dTimeNow();
    fPrevReportTime = fNextReportTime = timeNow;

    fKnownMembers = new RTCPMemberDatabase(*this);
    if (fKnownMembers == NULL) {
        return;
    }

    unsigned savedMaxSize = OutPacketBuffer::maxSize;
    OutPacketBuffer::maxSize = maxPacketSize;
    fOutBuf = new OutPacketBuffer(preferredPacketSize, maxPacketSize);
    OutPacketBuffer::maxSize = savedMaxSize;
    if (fOutBuf == NULL) {
        return;
    }

    fTypeOfEvent = EVENT_REPORT;
    onExpire(this);
}

RTCPInstance::~RTCPInstance()
{
    delete fKnownMembers;
    delete fOutBuf;
}

unsigned RTCPInstance::numMembers() const 
{
    if (fKnownMembers == NULL) {
        return 0;
    }

    return fKnownMembers->numMembers();
}

int RTCPInstance::checkNewSSRC() 
{
    return fKnownMembers->noteMembership(fLastReceivedSSRC, fOutgoingReportCount);
}

void RTCPInstance::removeLastReceivedSSRC() 
{
    removeSSRC(fLastReceivedSSRC, false);
}

void RTCPInstance::removeSSRC(u_int32_t ssrc, bool alsoRemoveStats) 
{
    fKnownMembers->remove(ssrc);

    if (alsoRemoveStats) {
        if (fSource != NULL) {
            fSource->receptionStatsDB().removeRecord(ssrc);
        }
    }
}

void RTCPInstance::onExpire(RTCPInstance* instance) 
{
    instance->onExpire1();
}

void RTCPInstance::onExpire1() 
{
    double rtcpBW = 0.05 * fTotSessionBW * 1024 / 8;
    OnExpire(this, numMembers(), 0, rtcpBW, 0, &fAveRTCPSize, &fIsInitial, dTimeNow(), &fPrevReportTime,&fPrevNumMembers);
}

static unsigned const IP_UDP_HDR_SIZE = 28;

#define ADVANCE(n)      pkt += (n); packetSize -= (n)

void RTCPInstance::rtcpPacketHandler(char *buf, int len)
{
    unsigned packetSize = len;
    int typeOfPacket = PACKET_UNKNOWN_TYPE;
    unsigned char *pkt = (unsigned char *)buf;

    do {
        int totPacketSize = IP_UDP_HDR_SIZE + packetSize;
        if (packetSize < 4) {
            break;
        }

        unsigned rtcpHdr = ntohl(*(unsigned*)pkt);
        if ((rtcpHdr & 0xE0FE0000) != (0x80000000 | (RTCP_PT_SR << 16))) {
            DPRINTF("rejected bad RTCP packet: header 0x%08x\n", rtcpHdr);
            break;
        }

        bool packetOK = false;
        unsigned reportSenderSSRC = 0;

        while (1) {
            unsigned rc = (rtcpHdr >> 24) & 0x1F;
            unsigned pt = (rtcpHdr >> 16) & 0xFF;
            unsigned length = 4 * (rtcpHdr & 0xFFFF);
            ADVANCE(4);
            if (length > packetSize) {
                break;
            }

            if (length < 4) {
                break; 
            }

            length -= 4;
            reportSenderSSRC = ntohl(*(unsigned *)pkt);
            ADVANCE(4);

            bool subPacketOK = false;
            switch (pt) {
                case RTCP_PT_SR: {
                    if (length < 20) {
                        break;
                    }
                    length -= 20;

                    unsigned NTPmsw = ntohl(*(unsigned *)pkt); ADVANCE(4);
                    unsigned NTPlsw = ntohl(*(unsigned *)pkt); ADVANCE(4);
                    unsigned rtpTimestamp = ntohl(*(unsigned *)pkt); ADVANCE(4);
                    if (fSource != NULL) {
                        RTPReceptionStatsDB &receptionStats = fSource->receptionStatsDB();
                        receptionStats.noteIncomingSR(reportSenderSSRC, NTPmsw, NTPlsw, rtpTimestamp);
                    }

                    ADVANCE(8);
                }

                case RTCP_PT_RR: {
                    unsigned reportBlocksSize = rc * (6 * 4);
                    if (length < reportBlocksSize) {
                        break;
                    }
                    length -= reportBlocksSize;

                    ADVANCE(reportBlocksSize);

                    if (pt == RTCP_PT_RR) {

                    }

                    subPacketOK = true;
                    typeOfPacket = PACKET_RTCP_REPORT;
                    break;
                }

                case RTCP_PT_BYE: {
                    subPacketOK = true;
                    typeOfPacket = PACKET_BYE;
                    break;
                }

                default: 
                    subPacketOK = true;
                    break;
            }

            if (!subPacketOK) {
                break;
            }

            ADVANCE(length);

            if (packetSize == 0) {
                packetOK = true;
                break;
            } else if (packetSize < 4) {
                break;
            }

            rtcpHdr = ntohl(*(unsigned *)pkt);
            if ((rtcpHdr & 0xC0000000) != 0x80000000) {
                break;
            }
        }

        if (!packetOK) {
            break;
        } else {

        }

        onReceive(typeOfPacket, totPacketSize, reportSenderSSRC);
    } while (0);
}

void RTCPInstance::onReceive(int typeOfPacket, int totPacketSize, unsigned ssrc) 
{
    fTypeOfPacket = typeOfPacket;
    fLastReceivedSize = totPacketSize;
    fLastReceivedSSRC = ssrc;

    int senders = 0;
    int members = (int)numMembers();

    OnReceive(this, this, &members, &fPrevNumMembers, &senders, &fAveRTCPSize, &fPrevReportTime, dTimeNow(), fNextReportTime);
}

void RTCPInstance::addRR()
{
    enqueueCommonReportPrefix(RTCP_PT_RR, fSource->SSRC());
    enqueueCommonReportSuffix();
}

void RTCPInstance::enqueueCommonReportPrefix(unsigned char packetType, unsigned SSRC, unsigned numExtraWords) 
{
    unsigned numReportingSources;

    if (fSource == NULL) {
        numReportingSources = 0;
    } else {
        RTPReceptionStatsDB& allReceptionStats = fSource->receptionStatsDB();
        numReportingSources = allReceptionStats.numActiveSourcesSinceLastReset();

        if (numReportingSources >= 32) {
            numReportingSources = 32;
        }
    }

    unsigned rtcpHdr = 0x80000000;
    rtcpHdr |= (numReportingSources << 24);
    rtcpHdr |= (packetType << 16);
    rtcpHdr |= (1 + numExtraWords + 6 * numReportingSources);

    fOutBuf->enqueueWord(rtcpHdr);
    fOutBuf->enqueueWord(SSRC);
}

void RTCPInstance::enqueueCommonReportSuffix()
{
    if (fSource != NULL) {
        RTPReceptionStatsDB &allReceptionStats = fSource->receptionStatsDB();

        RTPReceptionStatsDB::Iterator iterator(allReceptionStats);
        while (1) {
            RTPReceptionStats *receptionStats = iterator.next();
            if (receptionStats == NULL) {
                break;
            }

            enqueueReportBlock(receptionStats);
        }

        allReceptionStats.reset();
    }
}

void RTCPInstance::enqueueReportBlock(RTPReceptionStats *stats)
{
    fOutBuf->enqueueWord(stats->SSRC());

    unsigned highestExtSeqNumReceived = stats->highestExtSeqNumReceived();

    unsigned totNumExpected = highestExtSeqNumReceived - stats->baseExtSeqNumReceived();
    int totNumLost = totNumExpected - stats->totNumPacketsReceived();

    if (totNumLost > 0x007FFFFF) {
        totNumLost = 0x007FFFFF;
    } else if (totNumLost < 0) {
        if (totNumLost < -0x00800000) {
            totNumLost = 0x00800000;
        }

        totNumLost &= 0x00FFFFFF;
    }

    unsigned char lossFraction;
    unsigned numExpectedSinceLastReset = highestExtSeqNumReceived - stats->lastResetExtSeqNumReceived();
    int numLostSinceLastReset = numExpectedSinceLastReset - stats->numPacketsReceivedSinceLastReset();

    if ((numExpectedSinceLastReset == 0) || (numLostSinceLastReset < 0)) {
        lossFraction = 0;
    } else {
        lossFraction = (unsigned char)((numLostSinceLastReset << 8) / numExpectedSinceLastReset);
    }

    fOutBuf->enqueueWord((lossFraction<<24) | totNumLost);
    fOutBuf->enqueueWord(highestExtSeqNumReceived);

    fOutBuf->enqueueWord(stats->jitter());

    unsigned NTPmsw = stats->lastReceivedSR_NTPmsw();
    unsigned NTPlsw = stats->lastReceivedSR_NTPlsw();
    unsigned LSR = ((NTPmsw&0xFFFF)<<16)|(NTPlsw>>16);
    fOutBuf->enqueueWord(LSR);

    struct timeval const& LSRtime = stats->lastReceivedSR_time();
    struct timeval timeNow, timeSinceLSR;
    gettimeofday(&timeNow, NULL);

    if (timeNow.tv_usec < LSRtime.tv_usec) {
        timeNow.tv_usec += 1000000;
        timeNow.tv_sec -= 1;
    }

    timeSinceLSR.tv_sec = timeNow.tv_sec - LSRtime.tv_sec;
    timeSinceLSR.tv_usec = timeNow.tv_usec - LSRtime.tv_usec;

    unsigned DLSR;
    if (LSR == 0) {
        DLSR = 0;
    } else {
        DLSR = (timeSinceLSR.tv_sec << 16) | ((((timeSinceLSR.tv_usec << 11) + 15625) / 31250) & 0xFFFF);
    }

    fOutBuf->enqueueWord(DLSR);
}

void RTCPInstance::addSDES() 
{
    unsigned numBytes = 4;

    numBytes += fCNAME.totalSize();
    numBytes += 1;

    unsigned num4ByteWords = (numBytes + 3) / 4;

    unsigned rtcpHdr = 0x81000000;
    rtcpHdr |= (RTCP_PT_SDES<<16);
    rtcpHdr |= num4ByteWords;
    fOutBuf->enqueueWord(rtcpHdr);

    if (fSource != NULL) {
        fOutBuf->enqueueWord(fSource->SSRC());
    }

    fOutBuf->enqueue(fCNAME.data(), fCNAME.totalSize());

    unsigned char const zero = '\0';
    unsigned numPaddingBytesNeeded = 4 - (fOutBuf->curPacketSize() % 4);

    while (numPaddingBytesNeeded-- > 0) {
        fOutBuf->enqueue(&zero, 1);
    }
}

void RTCPInstance::sendReport()
{
    addRR();
    addSDES();

    sendBuiltPacket();
    
    const unsigned membershipReapPeriod = 5;
    if (((++fOutgoingReportCount) % membershipReapPeriod) == 0) {
        unsigned threshold = fOutgoingReportCount - membershipReapPeriod;
        fKnownMembers->reapOldMembers(threshold);
    }
}

void RTCPInstance::sendBuiltPacket() 
{
    unsigned reportSize = fOutBuf->curPacketSize();
    fSource->sendRtcpReport((char *)fOutBuf->packet(), reportSize);
    fOutBuf->resetOffset();

    fLastSentSize = IP_UDP_HDR_SIZE + reportSize;
    fHaveJustSentPacket = true;
    fLastPacketSentSize = reportSize;
}

SDESItem::SDESItem(unsigned char tag, unsigned char const *value)
{
    unsigned length = strlen((char const *)value);
    if (length > 511) {
        length = 511;
    }

    fData[0] = tag;
    fData[1] = (unsigned char)length;
    memmove(&fData[2], value, length);

    while ((length % 4) > 0) {
        fData[2 + length++] = '\0';
    }
}

unsigned SDESItem::totalSize() const
{
    return 2 + (unsigned)fData[1];
}

void Schedule(double nextTime, event e)
{
    RTCPInstance *instance = (RTCPInstance *)e;
    if (instance == NULL) {
        return;
    }

    // instance->reschedule(nextTime);
}

void Reschedule(double nextTime, event e)
{
    RTCPInstance *instance = (RTCPInstance *)e;
    if (instance == NULL) {
        return;
    }

    // instance->reschedule(nextTime);
}

void SendRTCPReport(event e)
{
    RTCPInstance *instance = (RTCPInstance *)e;
    if (instance == NULL) {
        return;
    }

    instance->sendReport();
}

void SendBYEPacket(event e)
{
    RTCPInstance *instance = (RTCPInstance *)e;
    if (instance == NULL) {
        return;
    }

    // instance->sendBYE();
}

int TypeOfEvent(event e)
{
    RTCPInstance *instance = (RTCPInstance *)e;
    if (instance == NULL) {
        return EVENT_UNKNOWN;
    }

    return instance->typeOfEvent();
}

int SentPacketSize(event e)
{
    RTCPInstance *instance = (RTCPInstance *)e;
    if (instance == NULL) {
        return 0;
    }

    return instance->sentPacketSize();
}

int PacketType(packet p)
{
    RTCPInstance *instance = (RTCPInstance *)p;
    if (instance == NULL) {
        return PACKET_UNKNOWN_TYPE;
    }

    return instance->packetType();
}

int ReceivedPacketSize(packet p)
{
    RTCPInstance *instance = (RTCPInstance *)p;
    if (instance == NULL) {
        return 0;
    }

    return instance->receivedPacketSize();
}

int NewMember(packet p)
{
    RTCPInstance *instance = (RTCPInstance *)p;
    if (instance == NULL) {
        return 0;
    }

    return instance->checkNewSSRC();
}

int NewSender(packet /*p*/)
{
    return 0;
}

void AddMember(packet /*p*/)
{

}

void AddSender(packet /*p*/)
{

}

void RemoveMember(packet p)
{
    RTCPInstance *instance = (RTCPInstance *)p;
    if (instance == NULL) {
        return;
    }

    instance->removeLastReceivedSSRC();
}

void RemoveSender(packet /*p*/)
{

}

double drand30()
{
    unsigned tmp = rand() & 0x3FFFFFFF;
    return tmp/(double)(1024*1024*1024);
}
}
