#ifndef RTSP_INTERNAL_RTCP_H
#define RTSP_INTERNAL_RTCP_H

#include "util.h"
#include "HashTable.hh"
#include "RTSPCommon.h"

namespace rtsp {
class RTPReceptionStats;

class RTPReceptionStatsDB {
public:
    unsigned totNumPacketsReceived() const {
        return fTotNumPacketsReceived;
    }

    unsigned numActiveSourcesSinceLastReset() const {
        return fNumActiveSourcesSinceLastReset;
    }

    void reset();

    class Iterator {
    public:
        Iterator(RTPReceptionStatsDB &receptionStatsDB);
        virtual ~Iterator();

        RTPReceptionStats *next(bool includeInactiveSources = false);

    private:
        HashTable::Iterator *fIter;
    };

    void noteIncomingPacket(uint32_t SSRC, uint16_t seqNum, uint32_t rtpTimestamp, unsigned timestampFrequency, bool useForJitterCalculation,
    struct timeval &resultPresentationTime, bool &resultHasBeenSyncedUsingRTCP, unsigned packetSize);

    void noteIncomingSR(uint32_t SSRC, uint32_t ntpTimestampMSW, uint32_t ntpTimestampLSW, uint32_t rtpTimestamp);

    void removeRecord(uint32_t SSRC);

    RTPReceptionStats *lookup(uint32_t SSRC) const;

protected:
    friend class RTPSource;

    RTPReceptionStatsDB();
    virtual ~RTPReceptionStatsDB();

protected:
    void add(uint32_t SSRC, RTPReceptionStats *stats);

protected:
    friend class Iterator;
    unsigned fNumActiveSourcesSinceLastReset;

private:
    HashTable *fTable;
    unsigned  fTotNumPacketsReceived;
};

class RTPReceptionStats {
public:
    uint32_t SSRC() const {
        return fSSRC;
    }

    unsigned numPacketsReceivedSinceLastReset() const {
        return fNumPacketsReceivedSinceLastReset;
    }

    unsigned totNumPacketsReceived() const {
        return fTotNumPacketsReceived;
    }

    double totNumKBytesReceived() const;

    unsigned totNumPacketsExpected() const {
        return fHighestExtSeqNumReceived - fBaseExtSeqNumReceived;
    }

    unsigned baseExtSeqNumReceived() const {
        return fBaseExtSeqNumReceived;
    }

    unsigned lastResetExtSeqNumReceived() const {
        return fLastResetExtSeqNumReceived;
    }

    unsigned highestExtSeqNumReceived() const {
        return fHighestExtSeqNumReceived;
    }

    unsigned jitter() const;

    unsigned lastReceivedSR_NTPmsw() const {
        return fLastReceivedSR_NTPmsw;
    }

    unsigned lastReceivedSR_NTPlsw() const {
        return fLastReceivedSR_NTPlsw;
    }

    struct timeval const &lastReceivedSR_time() const {
        return fLastReceivedSR_time;
    }

    unsigned minInterPacketGapUS() const {
        return fMinInterPacketGapUS;
    }

    unsigned maxInterPacketGapUS() const {
        return fMaxInterPacketGapUS;
    }

    struct timeval const &totalInterPacketGaps() const {
        return fTotalInterPacketGaps;
    }

protected:
    friend class RTPReceptionStatsDB;

    RTPReceptionStats(uint32_t SSRC, uint16_t initialSeqNum);
    RTPReceptionStats(uint32_t SSRC);
    virtual ~RTPReceptionStats();

private:
    void noteIncomingPacket(uint16_t seqNum, uint32_t rtpTimestamp, unsigned timestampFrequency, bool useForJitterCalculation,
    struct timeval &resultPresentationTime, bool& resultHasBeenSyncedUsingRTCP, unsigned packetSize);
    void noteIncomingSR(uint32_t ntpTimestampMSW, uint32_t ntpTimestampLSW, uint32_t rtpTimestamp);
    void init(uint32_t SSRC);
    void initSeqNum(uint16_t initialSeqNum);
    void reset();

protected:
    uint32_t       fSSRC;
    unsigned       fNumPacketsReceivedSinceLastReset;
    unsigned       fTotNumPacketsReceived;
    uint32_t       fTotBytesReceived_hi, fTotBytesReceived_lo;
    bool           fHaveSeenInitialSequenceNumber;
    unsigned       fBaseExtSeqNumReceived;
    unsigned       fLastResetExtSeqNumReceived;
    unsigned       fHighestExtSeqNumReceived;
    int            fLastTransit;
    uint32_t       fPreviousPacketRTPTimestamp;
    double         fJitter;

    unsigned       fLastReceivedSR_NTPmsw;
    unsigned       fLastReceivedSR_NTPlsw;
    struct timeval fLastReceivedSR_time;
    struct timeval fLastPacketReceptionTime;
    unsigned       fMinInterPacketGapUS, fMaxInterPacketGapUS;
    struct timeval fTotalInterPacketGaps;

private:
    bool           fHasBeenSynchronized;
    uint32_t       fSyncTimestamp;
    struct timeval fSyncTime;
};
}

#endif
