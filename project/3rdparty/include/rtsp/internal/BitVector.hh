#ifndef RTSP_INTERNAL_BIT_VECTOR_HH
#define RTSP_INTERNAL_BIT_VECTOR_HH

namespace rtsp {
class BitVector {
public:
    BitVector(unsigned char *baseBytePtr, unsigned baseBitOffset, unsigned totNumBits);

    void setup(unsigned char *baseBytePtr, unsigned baseBitOffset, unsigned totNumBits);

    void putBits(unsigned from, unsigned numBits);
    void put1Bit(unsigned bit);

    unsigned getBits(unsigned numBits);
    unsigned get1Bit();

    bool get1BitBoolean() {
        return get1Bit() != 0;
    }

    void skipBits(unsigned numBits);

    unsigned curBitIndex() const {
        return fCurBitIndex;
    }

    unsigned totNumBits() const {
        return fTotNumBits;
    }

    unsigned numBitsRemaining() const {
        return fTotNumBits - fCurBitIndex;
    }

    unsigned get_expGolomb();

private:
    unsigned char *fBaseBytePtr;
    unsigned      fBaseBitOffset;
    unsigned      fTotNumBits;
    unsigned      fCurBitIndex;
};

void shiftBits(unsigned char *toBasePtr, unsigned toBitOffset, unsigned char const *fromBasePtr, unsigned fromBitOffset, unsigned numBits);
}

#endif
