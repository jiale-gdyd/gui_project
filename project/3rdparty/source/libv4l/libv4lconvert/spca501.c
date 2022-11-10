#include <string.h>
#include "libv4lconvert-priv.h"

void v4lconvert_spca501_to_yuv420(const unsigned char *src, unsigned char *dst, int width, int height, int yvu)
{
    int i, j;
    unsigned long *lsrc = (unsigned long *)src;

    for (i = 0; i < height; i += 2) {
        unsigned long *ldst = (unsigned long *)(dst + i * width);

        for (j = 0; j < width; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        if (yvu) {
            ldst = (unsigned long *)(dst + (width * height * 5) / 4 + i * width / 4);
        } else {
            ldst = (unsigned long *)(dst + width * height + i * width / 4);
        }

        for (j = 0; j < width/2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        ldst = (unsigned long *)(dst + i * width + width);
        for (j = 0; j < width; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        if (yvu) {
            ldst = (unsigned long *)(dst + width * height + i * width / 4);
        } else {
            ldst = (unsigned long *)(dst + (width * height * 5) / 4 + i * width / 4);
        }

        for (j = 0; j < width/2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }
    }
}

void v4lconvert_spca505_to_yuv420(const unsigned char *src, unsigned char *dst, int width, int height, int yvu)
{
    int i, j;
    unsigned long *lsrc = (unsigned long *)src;

    for (i = 0; i < height; i += 2) {
        unsigned long *ldst = (unsigned long *)(dst + i * width);

        for (j = 0; j < width*2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        if (yvu) {
            ldst = (unsigned long *)(dst + (width * height * 5) / 4 + i * width / 4);
        } else {
            ldst = (unsigned long *)(dst + width * height + i * width / 4);
        }

        for (j = 0; j < width/2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        if (yvu) {
            ldst = (unsigned long *)(dst + width * height + i * width / 4);
        } else {
            ldst = (unsigned long *)(dst + (width * height * 5) / 4 + i * width / 4);
        }

        for (j = 0; j < width/2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }
    }
}

void v4lconvert_spca508_to_yuv420(const unsigned char *src, unsigned char *dst, int width, int height, int yvu)
{
    int i, j;
    unsigned long *lsrc = (unsigned long *)src;

    for (i = 0; i < height; i += 2) {
        unsigned long *ldst = (unsigned long *)(dst + i * width);
        for (j = 0; j < width; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        if (yvu) {
            ldst = (unsigned long *)(dst + (width * height * 5) / 4 + i * width / 4);
        } else {
            ldst = (unsigned long *)(dst + width * height + i * width / 4);
        }

        for (j = 0; j < width / 2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        if (yvu) {
            ldst = (unsigned long *)(dst + width * height + i * width / 4);
        } else {
            ldst = (unsigned long *)(dst + (width * height * 5) / 4 + i * width / 4);
        }

        for (j = 0; j < width / 2; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }

        ldst = (unsigned long *)(dst + i * width + width);
        for (j = 0; j < width; j += sizeof(long)) {
            *ldst = *lsrc++;
            *ldst++ ^= 0x8080808080808080ULL;
        }
    }
}

void v4lconvert_cit_yyvyuy_to_yuv420(const unsigned char *src, unsigned char *ydest, int width, int height, int yvu)
{
    int x, y;
    unsigned char *udest, *vdest;

    if (yvu) {
        vdest = ydest + width * height;
        udest = vdest + (width * height) / 4;
    } else {
        udest = ydest + width * height;
        vdest = udest + (width * height) / 4;
    }

    for (y = 0; y < height; y += 2) {
        memcpy(ydest, src, width);
        src += width;
        ydest += width;

        for (x = 0; x < width; x += 2) {
            *vdest++ = *src++;
            *ydest++ = *src++;
            *udest++ = *src++;
            *ydest++ = *src++;
        }
    }
}

void v4lconvert_konica_yuv420_to_yuv420(const unsigned char *src, unsigned char *ydest, int width, int height, int yvu)
{
    int i, no_blocks;
    unsigned char *udest, *vdest;

    if (yvu) {
        vdest = ydest + width * height;
        udest = vdest + (width * height) / 4;
    } else {
        udest = ydest + width * height;
        vdest = udest + (width * height) / 4;
    }

    no_blocks = width * height / 256;
    for (i = 0; i < no_blocks; i++) {
        memcpy(ydest, src, 256);
        src += 256;
        ydest += 256;

        memcpy(udest, src, 64);
        src += 64;
        udest += 64;

        memcpy(vdest, src, 64);
        src += 64;
        vdest += 64;
    }
}

void v4lconvert_m420_to_yuv420(const unsigned char *src, unsigned char *ydest, int width, int height, int yvu)
{
    int x, y;
    unsigned char *udest, *vdest;

    if (yvu) {
        vdest = ydest + width * height;
        udest = vdest + (width * height) / 4;
    } else {
        udest = ydest + width * height;
        vdest = udest + (width * height) / 4;
    }

    for (y = 0; y < height; y += 2) {
        memcpy(ydest, src, 2 * width);
        src += 2 * width;
        ydest += 2 * width;

        for (x = 0; x < width; x += 2) {
            *udest++ = *src++;
            *vdest++ = *src++;
        }
    }
}
