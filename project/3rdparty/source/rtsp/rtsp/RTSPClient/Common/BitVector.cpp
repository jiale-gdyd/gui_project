#include <rtsp/internal/BitVector.hh>

namespace rtsp {
#define MAX_LENGTH      32

static unsigned char const singleBitMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

BitVector::BitVector(unsigned char *baseBytePtr, unsigned baseBitOffset, unsigned totNumBits)
{
    setup(baseBytePtr, baseBitOffset, totNumBits);
}

void BitVector::setup(unsigned char *baseBytePtr, unsigned baseBitOffset, unsigned totNumBits)
{
    fBaseBytePtr = baseBytePtr;
    fBaseBitOffset = baseBitOffset;
    fTotNumBits = totNumBits;
    fCurBitIndex = 0;
}

void BitVector::putBits(unsigned from, unsigned numBits)
{
    if (numBits == 0) {
        return;
    }

    unsigned char tmpBuf[4];
    unsigned overflowingBits = 0;

    if (numBits > MAX_LENGTH) {
        numBits = MAX_LENGTH;
    }

    if (numBits > (fTotNumBits - fCurBitIndex)) {
        overflowingBits = numBits - (fTotNumBits - fCurBitIndex);
    }

    tmpBuf[0] = (unsigned char)(from>>24);
    tmpBuf[1] = (unsigned char)(from>>16);
    tmpBuf[2] = (unsigned char)(from>>8);
    tmpBuf[3] = (unsigned char)from;

    shiftBits(fBaseBytePtr, fBaseBitOffset + fCurBitIndex, tmpBuf, MAX_LENGTH - numBits, numBits - overflowingBits);
    fCurBitIndex += numBits - overflowingBits;
}

void BitVector::put1Bit(unsigned bit)
{
    if (fCurBitIndex >= fTotNumBits) {
        return;
    } else {
        unsigned totBitOffset = fBaseBitOffset + fCurBitIndex++;
        unsigned char mask = singleBitMask[totBitOffset % 8];
        if (bit) {
            fBaseBytePtr[totBitOffset/8] |= mask;
        } else {
            fBaseBytePtr[totBitOffset/8] &=~ mask;
        }
    }
}

unsigned BitVector::getBits(unsigned numBits)
{
    if (numBits == 0) {
        return 0;
    }

    unsigned char tmpBuf[4];
    unsigned overflowingBits = 0;

    if (numBits > MAX_LENGTH) {
        numBits = MAX_LENGTH;
    }

    if (numBits > (fTotNumBits - fCurBitIndex)) {
        overflowingBits = numBits - (fTotNumBits - fCurBitIndex);
    }

    shiftBits(tmpBuf, 0, fBaseBytePtr, fBaseBitOffset + fCurBitIndex, numBits - overflowingBits);
    fCurBitIndex += numBits - overflowingBits;

    unsigned result = (tmpBuf[0] << 24) | (tmpBuf[1] << 16) | (tmpBuf[2] << 8) | tmpBuf[3];
    result >>= (MAX_LENGTH - numBits);
    result &= (0xFFFFFFFF << overflowingBits);

    return result;
}

unsigned BitVector::get1Bit()
{
    if (fCurBitIndex >= fTotNumBits) {
        return 0;
    } else {
        unsigned totBitOffset = fBaseBitOffset + fCurBitIndex++;
        unsigned char curFromByte = fBaseBytePtr[totBitOffset/8];
        unsigned result = (curFromByte >> (7 - (totBitOffset % 8))) & 0x01;

        return result;
    }
}

void BitVector::skipBits(unsigned numBits)
{
    if (numBits > (fTotNumBits - fCurBitIndex)) {
        fCurBitIndex = fTotNumBits;
    } else {
        fCurBitIndex += numBits;
    }
}

unsigned BitVector::get_expGolomb()
{
    unsigned codeStart = 1;
    unsigned numLeadingZeroBits = 0;

    while ((get1Bit() == 0) && (fCurBitIndex < fTotNumBits)) {
        ++numLeadingZeroBits;
        codeStart *= 2;
    }

    return codeStart - 1 + getBits(numLeadingZeroBits);
}

void shiftBits(unsigned char* toBasePtr, unsigned toBitOffset, unsigned char const* fromBasePtr, unsigned fromBitOffset, unsigned numBits)
{
    if (numBits == 0) {
        return;
    }

    unsigned toBitRem = toBitOffset % 8;
    unsigned fromBitRem = fromBitOffset % 8;
    unsigned char *toBytePtr = toBasePtr + toBitOffset / 8;
    unsigned char const *fromBytePtr = fromBasePtr + fromBitOffset / 8;

    while (numBits-- > 0) {
        unsigned char fromBitMask = singleBitMask[fromBitRem];
        unsigned char fromBit = (*fromBytePtr) & fromBitMask;
        unsigned char toBitMask = singleBitMask[toBitRem];

        if (fromBit != 0) {
            *toBytePtr |= toBitMask;
        } else {
            *toBytePtr &=~ toBitMask;
        }

        if (++fromBitRem == 8) {
            ++fromBytePtr;
            fromBitRem = 0;
        }

        if (++toBitRem == 8) {
            ++toBytePtr;
            toBitRem = 0;
        }
    }
}
}
