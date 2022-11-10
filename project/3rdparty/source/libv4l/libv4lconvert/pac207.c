#include <string.h>
#include "libv4lconvert-priv.h"

#define CLIP(color)         (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

static int decoder_initialized;

static struct {
    unsigned char is_abs;
    unsigned char len;
    signed char   val;
} table[256];

static void init_pixart_decoder(void)
{
    int i;
    int is_abs, val, len;

    for (i = 0; i < 256; i++) {
        is_abs = 0;
        val = 0;
        len = 0;

        if ((i & 0xC0) == 0) {
            val = 0;
            len = 2;
        } else if ((i & 0xC0) == 0x40) {
            val = -1;
            len = 2;
        } else if ((i & 0xC0) == 0x80) {
            val = 1;
            len = 2;
        } else if ((i & 0xF0) == 0xC0) {
            val = -2;
            len = 4;
        } else if ((i & 0xF0) == 0xD0) {
            val = 2;
            len = 4;
        } else if ((i & 0xF8) == 0xE0) {
            val = -3;
            len = 5;
        } else if ((i & 0xF8) == 0xE8) {
            val = 3;
            len = 5;
        } else if ((i & 0xFC) == 0xF0) {
            val = -4;
            len = 6;
        } else if ((i & 0xFC) == 0xF4) {
            val = 4;
            len = 6;
        } else if ((i & 0xF8) == 0xF8) {
            is_abs = 1;
            val = 0;
            len = 5;
        }

        table[i].is_abs = is_abs;
        table[i].val = val;
        table[i].len = len;
    }

    decoder_initialized = 1;
}

static inline unsigned char getByte(const unsigned char *inp, unsigned int bitpos)
{
    const unsigned char *addr;

    addr = inp + (bitpos >> 3);
    return (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
}

static inline unsigned short getShort(const unsigned char *pt)
{
    return ((pt[0] << 8) | pt[1]);
}

static int pac_decompress_row(const unsigned char *inp, unsigned char *outp, int width, int step_size, int abs_bits)
{
    int col;
    int val;
    int bitpos;
    unsigned char code;

    if (!decoder_initialized) {
        init_pixart_decoder();
    }

    *outp++ = inp[2];
    *outp++ = inp[3];
    bitpos = 32;

    for (col = 2; col < width; col++) {
        code = getByte(inp, bitpos);
        bitpos += table[code].len;

        if (table[code].is_abs) {
            code = getByte(inp, bitpos);
            bitpos += abs_bits;
            *outp++ = code & ~(0xff >> abs_bits);
        } else {
            val = outp[-2] + table[code].val * step_size;
            *outp++ = CLIP(val);
        }
    }

    return 2 * ((bitpos + 15) / 16);
}

int v4lconvert_decode_pac207(struct v4lconvert_data *data, const unsigned char *inp, int src_size, unsigned char *outp, int width, int height)
{
    int row;
    unsigned short word;
    const unsigned char *end = inp + src_size;

    for (row = 0; row < height; row++) {
        if ((inp + 2) > end) {
            V4LCONVERT_ERR("incomplete pac207 frame\n");
            return -1;
        }

        word = getShort(inp);
        switch (word) {
            case 0x0FF0:
                memcpy(outp, inp + 2, width);
                inp += (2 + width);
                break;

            case 0x1EE1:
                inp += pac_decompress_row(inp, outp, width, 5, 6);
                break;

            case 0x2DD2:
                inp += pac_decompress_row(inp, outp, width, 9, 5);
                break;

            case 0x3CC3:
                inp += pac_decompress_row(inp, outp, width, 17, 4);
                break;

            case 0x4BB4:
                memcpy(outp, outp - 2 * width, width);
                inp += 2;
                break;

            default:
                V4LCONVERT_ERR("unknown pac207 row header: 0x%04x\n", (int)word);
                return -1;
        }

        outp += width;
    }

    return 0;
}
