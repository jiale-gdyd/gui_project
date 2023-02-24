#include <rtsp/internal/NetCommon.h>
#include <rtsp/internal/OutPacketBuffer.h>

namespace rtsp {
unsigned OutPacketBuffer::maxSize = 60000;

OutPacketBuffer::OutPacketBuffer(unsigned preferredPacketSize, unsigned maxPacketSize)
    : fPreferred(preferredPacketSize), fMax(maxPacketSize), fOverflowDataSize(0)
{
    unsigned maxNumPackets = (maxSize + (maxPacketSize - 1)) / maxPacketSize;
    fLimit = maxNumPackets * maxPacketSize;

    fBuf = new unsigned char[fLimit];
    resetPacketStart();
    resetOffset();
    resetOverflowData();
}

OutPacketBuffer::~OutPacketBuffer()
{
    delete[] fBuf;
}

void OutPacketBuffer::enqueue(unsigned char const* from, unsigned numBytes)
{
    if (numBytes > totalBytesAvailable()) {
        numBytes = totalBytesAvailable();
    }

    if (curPtr() != from) {
        memmove(curPtr(), from, numBytes);
    }

    increment(numBytes);
}

void OutPacketBuffer::enqueueWord(unsigned word)
{
    unsigned nWord = htonl(word);
    enqueue((unsigned char *)&nWord, 4);
}

void OutPacketBuffer::insert(unsigned char const* from, unsigned numBytes, unsigned toPosition)
{
    unsigned realToPosition = fPacketStart + toPosition;
    if (realToPosition + numBytes > fLimit) {
        if (realToPosition > fLimit) {
            return;
        }

        numBytes = fLimit - realToPosition;
    }

    memmove(&fBuf[realToPosition], from, numBytes);
    if ((toPosition + numBytes) > fCurOffset) {
        fCurOffset = toPosition + numBytes;
    }
}

void OutPacketBuffer::insertWord(unsigned word, unsigned toPosition)
{
    unsigned nWord = htonl(word);
    insert((unsigned char *)&nWord, 4, toPosition);
}

void OutPacketBuffer::extract(unsigned char* to, unsigned numBytes, unsigned fromPosition)
{
    unsigned realFromPosition = fPacketStart + fromPosition;
    if ((realFromPosition + numBytes) > fLimit) {
        if (realFromPosition > fLimit) {
            return;
        }

        numBytes = fLimit - realFromPosition;
    }

    memmove(to, &fBuf[realFromPosition], numBytes);
}

unsigned OutPacketBuffer::extractWord(unsigned fromPosition)
{
    unsigned nWord;
    extract((unsigned char *)&nWord, 4, fromPosition);
    return ntohl(nWord);
}

void OutPacketBuffer::skipBytes(unsigned numBytes)
{
    if (numBytes > totalBytesAvailable()) {
        numBytes = totalBytesAvailable();
    }

    increment(numBytes);
}

void OutPacketBuffer::setOverflowData(unsigned overflowDataOffset, unsigned overflowDataSize, struct timeval const &presentationTime, unsigned durationInMicroseconds)
{
    fOverflowDataOffset = overflowDataOffset;
    fOverflowDataSize = overflowDataSize;
    fOverflowPresentationTime = presentationTime;
    fOverflowDurationInMicroseconds = durationInMicroseconds;
}

void OutPacketBuffer::useOverflowData()
{
    enqueue(&fBuf[fPacketStart + fOverflowDataOffset], fOverflowDataSize);
    fCurOffset -= fOverflowDataSize;
    resetOverflowData();
}

void OutPacketBuffer::adjustPacketStart(unsigned numBytes)
{
    fPacketStart += numBytes;
    if (fOverflowDataOffset >= numBytes) {
        fOverflowDataOffset -= numBytes;
    } else {
        fOverflowDataOffset = 0;
        fOverflowDataSize = 0;
    }
}

void OutPacketBuffer::resetPacketStart()
{
    if (fOverflowDataSize > 0) {
        fOverflowDataOffset += fPacketStart;
    }

    fPacketStart = 0;
}
}
