#include <unistd.h>

#include "libv4lconvert-priv.h"
#include "libv4lsyscall-priv.h"

#define CLIP(x)             ((x) < 0 ? 0 : ((x) > 0xff) ? 0xff : (x))

#define MIN_CLOCKDIV_CID    V4L2_CID_PRIVATE_BASE

static int decoder_initialized;

static struct {
    unsigned char is_abs;
    unsigned char len;
    signed char   val;
} table[256];

static void init_mr97310a_decoder(void)
{
    int i;
    int is_abs, val, len;

    for (i = 0; i < 256; ++i) {
        is_abs = 0;
        val = 0;
        len = 0;

        if ((i & 0x80) == 0) {
            val = 0;
            len = 1;
        } else if ((i & 0xe0) == 0xc0) {
            val = -3;
            len = 3;
        } else if ((i & 0xe0) == 0xa0) {
            val = 3;
            len = 3;
        } else if ((i & 0xf0) == 0x80) {
            val = 8;
            len = 4;
        } else if ((i & 0xf0) == 0x90) {
            val = -8;
            len = 4;
        } else if ((i & 0xf0) == 0xf0) {
            val = -20;
            len = 4;
        } else if ((i & 0xf8) == 0xe0) {
            val = 20;
            len = 5;
        } else if ((i & 0xf8) == 0xe8) {
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

static inline unsigned char get_byte(const unsigned char *inp, unsigned int bitpos)
{
    const unsigned char *addr;

    addr = inp + (bitpos >> 3);
    return (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
}

int v4lconvert_decode_mr97310a(struct v4lconvert_data *data, const unsigned char *inp, int src_size, unsigned char *outp, int width, int height)
{
    int val;
    int bitpos;
    int row, col;
    unsigned char code;
    unsigned char lp, tp, tlp, trp;
    struct v4l2_control min_clockdiv = { .id = MIN_CLOCKDIV_CID };

    if (!decoder_initialized) {
        init_mr97310a_decoder();
    }

    inp += 12;
    bitpos = 0;

    for (row = 0; row < height; ++row) {
        col = 0;

        if (row < 2) {
            code = get_byte(inp, bitpos);
            bitpos += 8;
            *outp++ = code;

            code = get_byte(inp, bitpos);
            bitpos += 8;
            *outp++ = code;

            col += 2;
        }

        while (col < width) {
            code = get_byte(inp, bitpos);
            bitpos += table[code].len;

            if (table[code].is_abs) {
                code = get_byte(inp, bitpos);
                val = (code & 0xf8);
                bitpos += 5;
            } else {
                val = table[code].val;
                lp = outp[-2];
                if (row > 1) {
                    tlp = outp[-2 * width - 2];
                    tp  = outp[-2 * width];
                    trp = outp[-2 * width + 2];
                }

                if (row < 2) {
                    val += lp;
                } else if (col < 2) {
                    val += (tp + trp) / 2;
                } else if (col > width - 3) {
                    val += (tp + lp + tlp + 1) / 3;
                } else {
                    tlp >>= 1;
                    trp >>= 1;
                    val += (lp + tp + tlp + trp + 1) / 3;
                }
            }

            *outp++ = CLIP(val);
            ++col;
        }

        if (((bitpos - 1) / 8) >= (src_size - 12)) {
            data->frames_dropped++;
            if (data->frames_dropped == 3) {
                SYS_IOCTL(data->fd, VIDIOC_G_CTRL, &min_clockdiv);
                min_clockdiv.value++;
                SYS_IOCTL(data->fd, VIDIOC_S_CTRL, &min_clockdiv);

                data->frames_dropped = 0;
                return 0;
            }

            V4LCONVERT_ERR("incomplete mr97310a frame\n");
            return -1;
        }
    }

    data->frames_dropped = 0;
    return 0;
}
