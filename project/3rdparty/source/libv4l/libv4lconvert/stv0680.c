#include "libv4lconvert-priv.h"

void v4lconvert_decode_stv0680(const unsigned char *src, unsigned char *dst, int width, int height)
{
    int x, y;
    const unsigned char *src1 = src;
    const unsigned char *src2 = src + width / 2;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width / 2; x++) {
            *dst++ = *src1++;
            *dst++ = *src2++;
        }

        src1 += width / 2;
        src2 += width / 2;
    }
}
