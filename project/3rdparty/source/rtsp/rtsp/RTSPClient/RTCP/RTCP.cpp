#include <rtsp/internal/RTCP.h>

namespace rtsp {
#ifndef MILLION
#define MILLION         1000000
#endif

RTPReceptionStatsDB::RTPReceptionStatsDB()
    : fTable(HashTable::create(ONE_WORD_HASH_KEYS)), fTotNumPacketsReceived(0)
{
    reset();
}

void RTPReceptionStatsDB::reset()
{
    fNumActiveSourcesSinceLastReset = 0;

    Iterator iter(*this);
    RTPReceptionStats *stats;
    while ((stats = iter.next()) != NULL) {
        stats->reset();
    }
}

RTPReceptionStatsDB::~RTPReceptionStatsDB()
{
    RTPReceptionStats *stats;
    while ((stats = (RTPReceptionStats *)fTable->RemoveNext()) != NULL) {
        delete stats;
    }

    delete fTable;
}

void RTPReceptionStatsDB::noteIncomingPacket(uint32_t SSRC, uint16_t seqNum, uint32_t rtpTimestamp, unsigned timestampFrequency, bool useForJitterCalculation, struct timeval &resultPresentationTime, bool &resultHasBeenSyncedUsingRTCP, unsigned packetSize) 
{
    ++fTotNumPacketsReceived;

    RTPReceptionStats *stats = lookup(SSRC);
    if (stats == NULL) {
        stats = new RTPReceptionStats(SSRC, seqNum);
        if (stats == NULL) {
            return;
        }

        add(SSRC, stats);
    }

    if (stats->numPacketsReceivedSinceLastReset() == 0) {
        ++fNumActiveSourcesSinceLastReset;
    }

    stats->noteIncomingPacket(seqNum, rtpTimestamp, timestampFrequency, useForJitterCalculation, resultPresentationTime, resultHasBeenSyncedUsingRTCP, packetSize);
}

void RTPReceptionStatsDB::noteIncomingSR(uint32_t SSRC, uint32_t ntpTimestampMSW, uint32_t ntpTimestampLSW, uint32_t rtpTimestamp)
{
    RTPReceptionStats *stats = lookup(SSRC);
    if (stats == NULL) {
        stats = new RTPReceptionStats(SSRC);
        if (stats == NULL) {
            return;
        }

        add(SSRC, stats);
    }

    stats->noteIncomingSR(ntpTimestampMSW, ntpTimestampLSW, rtpTimestamp);
}

void RTPReceptionStatsDB::removeRecord(uint32_t SSRC)
{
    RTPReceptionStats *stats = lookup(SSRC);
    if (stats != NULL) {
        long SSRC_long = (long)SSRC;
        fTable->Remove((char const *)SSRC_long);
        delete stats;
    }
}

RTPReceptionStatsDB::Iterator::Iterator(RTPReceptionStatsDB &receptionStatsDB)
    : fIter(HashTable::Iterator::create(*(receptionStatsDB.fTable)))
{

}

RTPReceptionStatsDB::Iterator::~Iterator()
{
    delete fIter;
}

RTPReceptionStats *RTPReceptionStatsDB::Iterator::next(bool includeInactiveSources)
{
    char const *key;
    RTPReceptionStats *stats;

    do {
        stats = (RTPReceptionStats *)(fIter->next(key));
    } while ((stats != NULL) && !includeInactiveSources && (stats->numPacketsReceivedSinceLastReset() == 0));

    return stats;
}

RTPReceptionStats *RTPReceptionStatsDB::lookup(uint32_t SSRC) const
{
    long SSRC_long = (long)SSRC;
    return (RTPReceptionStats *)(fTable->Lookup((char const *)SSRC_long));
}

void RTPReceptionStatsDB::add(uint32_t SSRC, RTPReceptionStats *stats)
{
    long SSRC_long = (long)SSRC;
    fTable->Add((char const *)SSRC_long, stats);
}

RTPReceptionStats::RTPReceptionStats(uint32_t SSRC, uint16_t initialSeqNum)
{
    initSeqNum(initialSeqNum);
    init(SSRC);
}

RTPReceptionStats::RTPReceptionStats(uint32_t SSRC)
{
    init(SSRC);
}

RTPReceptionStats::~RTPReceptionStats()
{

}

void RTPReceptionStats::init(uint32_t SSRC)
{
    fSSRC = SSRC;
    fTotNumPacketsReceived = 0;
    fTotBytesReceived_hi = fTotBytesReceived_lo = 0;
    fHaveSeenInitialSequenceNumber = false;
    fLastTransit = ~0;
    fPreviousPacketRTPTimestamp = 0;
    fJitter = 0.0;
    fLastReceivedSR_NTPmsw = fLastReceivedSR_NTPlsw = 0;
    fLastReceivedSR_time.tv_sec = fLastReceivedSR_time.tv_usec = 0;
    fLastPacketReceptionTime.tv_sec = fLastPacketReceptionTime.tv_usec = 0;
    fMinInterPacketGapUS = 0x7FFFFFFF;
    fMaxInterPacketGapUS = 0;
    fTotalInterPacketGaps.tv_sec = fTotalInterPacketGaps.tv_usec = 0;
    fHasBeenSynchronized = false;
    fSyncTime.tv_sec = fSyncTime.tv_usec = 0;

    reset();
}

void RTPReceptionStats::initSeqNum(uint16_t initialSeqNum)
{
    fBaseExtSeqNumReceived = initialSeqNum-1;
    fHighestExtSeqNumReceived = initialSeqNum;
    fHaveSeenInitialSequenceNumber = true;
}

void RTPReceptionStats::noteIncomingPacket(uint16_t seqNum, uint32_t rtpTimestamp, unsigned timestampFrequency, bool useForJitterCalculation, struct timeval &resultPresentationTime, bool &resultHasBeenSyncedUsingRTCP, unsigned packetSize) 
{
    if (!fHaveSeenInitialSequenceNumber) {
        initSeqNum(seqNum);
    }

    ++fNumPacketsReceivedSinceLastReset;
    ++fTotNumPacketsReceived;

    uint32_t prevTotBytesReceived_lo = fTotBytesReceived_lo;
    fTotBytesReceived_lo += packetSize;
    if (fTotBytesReceived_lo < prevTotBytesReceived_lo) {
        ++fTotBytesReceived_hi;
    }

    unsigned oldSeqNum = (fHighestExtSeqNumReceived & 0xFFFF);
    if (seqNumLT((uint16_t)oldSeqNum, seqNum)) {
        unsigned seqNumCycle = (fHighestExtSeqNumReceived & 0xFFFF0000);
        unsigned seqNumDifference = (unsigned)((int)seqNum - (int)oldSeqNum);
        if (seqNumDifference >= 0x8000) {
            seqNumCycle += 0x10000;
        }

        unsigned newSeqNum = seqNumCycle | seqNum;
        if (newSeqNum > fHighestExtSeqNumReceived) {
            fHighestExtSeqNumReceived = newSeqNum;
        }
    }

    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);

    if ((fLastPacketReceptionTime.tv_sec != 0) || (fLastPacketReceptionTime.tv_usec != 0)) {
        unsigned gap = (timeNow.tv_sec - fLastPacketReceptionTime.tv_sec) * MILLION + timeNow.tv_usec - fLastPacketReceptionTime.tv_usec; 
        if (gap > fMaxInterPacketGapUS) {
            fMaxInterPacketGapUS = gap;
        }

        if (gap < fMinInterPacketGapUS) {
            fMinInterPacketGapUS = gap;
        }

        fTotalInterPacketGaps.tv_usec += gap;
        if (fTotalInterPacketGaps.tv_usec >= MILLION) {
            ++fTotalInterPacketGaps.tv_sec;
            fTotalInterPacketGaps.tv_usec -= MILLION;
        }
    }
    fLastPacketReceptionTime = timeNow;

    if (useForJitterCalculation && (rtpTimestamp != fPreviousPacketRTPTimestamp)) {
        unsigned arrival = (timestampFrequency * timeNow.tv_sec);
        arrival += (unsigned)((2.0 * timestampFrequency * timeNow.tv_usec + 1000000.0) / 2000000);

        int transit = arrival - rtpTimestamp;
        if (fLastTransit == (~0)) {
            fLastTransit = transit;
        }

        int d = transit - fLastTransit;
        fLastTransit = transit;
        if (d < 0) {
            d = -d;
        }

        fJitter += (1.0/16.0) * ((double)d - fJitter);
    }

    if ((fSyncTime.tv_sec == 0) && (fSyncTime.tv_usec == 0)) {
        fSyncTimestamp = rtpTimestamp;
        fSyncTime = timeNow;
    }

    int timestampDiff = rtpTimestamp - fSyncTimestamp;
    double timeDiff = timestampDiff / (double)timestampFrequency;

    unsigned seconds, uSeconds;
    unsigned const million = 1000000;

    if (timeDiff >= 0.0) {
        seconds = fSyncTime.tv_sec + (unsigned)(timeDiff);
        uSeconds = fSyncTime.tv_usec + (unsigned)((timeDiff - (unsigned)timeDiff)*million);
        if (uSeconds >= million) {
            uSeconds -= million;
            ++seconds;
        }
    } else {
        timeDiff = -timeDiff;
        seconds = fSyncTime.tv_sec - (unsigned)(timeDiff);
        uSeconds = fSyncTime.tv_usec - (unsigned)((timeDiff - (unsigned)timeDiff)*million);
        if ((int)uSeconds < 0) {
            uSeconds += million;
            --seconds;
        }
    }

    resultPresentationTime.tv_sec = seconds;
    resultPresentationTime.tv_usec = uSeconds;
    resultHasBeenSyncedUsingRTCP = fHasBeenSynchronized;

    fSyncTimestamp = rtpTimestamp;
    fSyncTime = resultPresentationTime;

    fPreviousPacketRTPTimestamp = rtpTimestamp;
}

void RTPReceptionStats::noteIncomingSR(uint32_t ntpTimestampMSW, uint32_t ntpTimestampLSW, uint32_t rtpTimestamp)
{
    fLastReceivedSR_NTPmsw = ntpTimestampMSW;
    fLastReceivedSR_NTPlsw = ntpTimestampLSW;

    gettimeofday(&fLastReceivedSR_time, NULL);

    fSyncTimestamp = rtpTimestamp;
    fSyncTime.tv_sec = ntpTimestampMSW - 0x83AA7E80;
    double microseconds = (ntpTimestampLSW * 15625.0) / 0x04000000;
    fSyncTime.tv_usec = (unsigned)(microseconds + 0.5);
    fHasBeenSynchronized = true;
}

double RTPReceptionStats::totNumKBytesReceived() const
{
    double const hiMultiplier = 0x20000000 / 125.0;
    return fTotBytesReceived_hi * hiMultiplier + fTotBytesReceived_lo / 1000.0;
}

unsigned RTPReceptionStats::jitter() const
{
    return (unsigned)fJitter;
}

void RTPReceptionStats::reset()
{
    fNumPacketsReceivedSinceLastReset = 0;
    fLastResetExtSeqNumReceived = fHighestExtSeqNumReceived;
}
}
