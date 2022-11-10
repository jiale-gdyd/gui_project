#include "libv4lconvert-priv.h"

#define CLAMP(x)        ((x) < 0 ? 0 : ((x) > 255) ? 255 : (x))

struct code_table {
    int is_abs;
    int len;
    int val;
    int unk;
};

static int init_done;
static struct code_table table[256];

static void sonix_decompress_init(void)
{
    int i;
    int is_abs, val, len, unk;

    for (i = 0; i < 256; i++) {
        is_abs = 0;
        val = 0;
        len = 0;
        unk = 0;

        if ((i & 0x80) == 0) {
            val = 0;
            len = 1;
        } else if ((i & 0xE0) == 0x80) {
            val = 4;
            len = 3;
        } else if ((i & 0xE0) == 0xA0) {
            val = -4;
            len = 3;
        } else if ((i & 0xF0) == 0xD0) {
            val = 11;
            len = 4;
        } else if ((i & 0xF0) == 0xF0) {
            val = -11;
            len = 4;
        } else if ((i & 0xF8) == 0xC8) {
            val = 20;
            len = 5;
        } else if ((i & 0xFC) == 0xC0) {
            val = -20;
            len = 6;
        } else if ((i & 0xFC) == 0xC4) {
            val = 0;
            len = 8;
            unk = 1;
        } else if ((i & 0xF0) == 0xE0) {
            is_abs = 1;
            val = (i & 0x0F) << 4;
            len = 8;
        }

        table[i].is_abs = is_abs;
        table[i].val = val;
        table[i].len = len;
        table[i].unk = unk;
    }

    init_done = 1;
}

void v4lconvert_decode_sn9c10x(const unsigned char *inp, unsigned char *outp, int width, int height)
{
    int val;
    int bitpos;
    int row, col;
    unsigned char code;
    const unsigned char *addr;

    if (!init_done) {
        sonix_decompress_init();
    }

    bitpos = 0;
    for (row = 0; row < height; row++) {
        col = 0;

        if (row < 2) {
            addr = inp + (bitpos >> 3);
            code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
            bitpos += 8;
            *outp++ = code;

            addr = inp + (bitpos >> 3);
            code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
            bitpos += 8;
            *outp++ = code;

            col += 2;
        }

        while (col < width) {
            addr = inp + (bitpos >> 3);
            code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));

            bitpos += table[code].len;
            if (table[code].unk) {
                continue;
            }

            val = table[code].val;
            if (!table[code].is_abs) {
                if (col < 2) {
                    val += outp[-2 * width];
                } else if (row < 2) {
                    val += outp[-2];
                } else {
                    val += (outp[-2] + outp[-2 * width]) / 2;
                }
            }

            *outp++ = CLAMP(val);
            col++;
        }
    }
}
