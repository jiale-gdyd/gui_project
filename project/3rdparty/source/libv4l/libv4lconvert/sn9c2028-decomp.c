#include "libv4lconvert-priv.h"

#define PEEK_BITS(num, to)                                  \
    {                                                       \
        if (bitBufCount < num) {                            \
            do {                                            \
                bitBuf = (bitBuf << 8) | (*(src++));        \
                bitBufCount += 8;                           \
            } while (bitBufCount < 24);                     \
        }                                                   \
        to = bitBuf >> (bitBufCount - num);                 \
    }

#define EAT_BITS(num)       { bitBufCount -= num; bits_eaten += num; }

#define PARSE_PIXEL(val)                                    \
    {                                                       \
        PEEK_BITS(10, bits);                                \
        if ((bits & 0x200) == 0) {                          \
            EAT_BITS(1);                                    \
        } else if ((bits & 0x380) == 0x280) {               \
            EAT_BITS(3);                                    \
            val += 3;                                       \
            if (val > 255) {                                \
                val = 255;                                  \
            }                                               \
        } else if ((bits & 0x380) == 0x300) {               \
            EAT_BITS(3);                                    \
            val -= 3;                                       \
            if (val < 0) {                                  \
                val = 0;                                    \
            }                                               \
        } else if ((bits & 0x3c0) == 0x200) {               \
            EAT_BITS(4);                                    \
            val += 8;                                       \
            if (val > 255) {                                \
                val = 255;                                  \
            }                                               \
        } else if ((bits & 0x3c0) == 0x240) {               \
            EAT_BITS(4);                                    \
            val -= 8;                                       \
            if (val < 0) {                                  \
                val = 0;                                    \
            }                                               \
        } else if ((bits & 0x3c0) == 0x3c0) {               \
            EAT_BITS(4);                                    \
            val -= 20;                                      \
            if (val < 0) {                                  \
                val = 0;                                    \
            }                                               \
        } else if ((bits & 0x3e0) == 0x380) {               \
            EAT_BITS(5);                                    \
            val += 20;                                      \
            if (val > 255) {                                \
                val = 255;                                  \
            }                                               \
        } else {                                            \
            EAT_BITS(10);                                   \
            val = 8 * (bits & 0x1f);                        \
        }                                                   \
    }

#define PUT_PIXEL_PAIR                                      \
    {                                                       \
        long pp;                                            \
        pp = (c1val << 8) + c2val;                          \
        *((unsigned short *)(dst + dst_index)) = pp;        \
        dst_index += 2;                                     \
    }

void v4lconvert_decode_sn9c2028(const unsigned char *src, unsigned char *dst, int width, int height)
{
    int x, y;
    long dst_index = 0;
    short c1val, c2val;
    unsigned short bits;
    int starting_row = 0;
    unsigned long bitBuf = 0;
    unsigned long bits_eaten = 0;
    unsigned long bitBufCount = 0;

    src += 12;

    for (y = starting_row; y < height; y++) {
        PEEK_BITS(8, bits);
        EAT_BITS(8);
        c2val = (bits & 0xff);
        PEEK_BITS(8, bits);
        EAT_BITS(8);
        c1val = (bits & 0xff);

        PUT_PIXEL_PAIR;

        for (x = 2; x < width ; x += 2) {
            PARSE_PIXEL(c2val);
            PARSE_PIXEL(c1val);
            PUT_PIXEL_PAIR;
        }
    }
}
