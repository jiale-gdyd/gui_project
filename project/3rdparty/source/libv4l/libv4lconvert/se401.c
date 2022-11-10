#include <errno.h>
#include "libv4lconvert-priv.h"

#define SE401_QUANT_FACT    8

static void wr_pixel(int p, uint8_t **dest, int pitch, int *x)
{
    int i = *x;

    if (i < 3) {
        (*dest)[i] = p * SE401_QUANT_FACT;
    } else {
        (*dest)[i] = (*dest)[i - 3] + p * SE401_QUANT_FACT;
    }

    *x += 1;
    if (*x == pitch) {
        *x = 0;
        *dest += pitch;
    }
}

enum decode_state {
    get_len,
    sign_bit,
    other_bits,
};

static int decode_JangGu(const uint8_t *data, int bits, int plen, int pixels, uint8_t **dest, int pitch, int *x)
{
    int bit;
    int bitnr;
    int len = 0;
    int value = 0;
    enum decode_state state = get_len;

    while (plen) {
        bitnr = 8;
        while (bitnr && bits) {
            bit = ((*data) >> (bitnr - 1))&1;
            switch (state) {
                case get_len:
                    if (bit) {
                        len++;
                    } else {
                        if (!len) {
                            wr_pixel(0, dest, pitch, x);
                            if (!--pixels) {
                                return 0;
                            }
                        } else {
                            state = sign_bit;
                            value = 0;
                        }
                    }
                    break;

                case sign_bit:
                    if (bit) {
                        value = 0;
                    } else {
                        value = -(1 << len) + 1;
                    }
                    state = other_bits;

                case other_bits:
                    len--;
                    value += bit << len;
                    if (len == 0) {
                        state = get_len;
                        wr_pixel(value, dest, pitch, x);
                        if (!--pixels) {
                            return 0;
                        }
                    }
                    break;
            }

            bitnr--;
            bits--;
        }

        data++;
        plen--;
    }

    return -1;
}

int v4lconvert_se401_to_rgb24(struct v4lconvert_data *data, const unsigned char *src, int src_size, unsigned char *dest, int width, int height)
{
    int x = 0, total_pixels = 0;
    int in, plen, bits, pixels, info;

    if (!src || !dest) {
        goto error;
    }

    for (in = 0; in + 4 < src_size; in += plen) {
        bits = src[in + 3] + (src[in + 2] << 8);
        pixels = src[in + 1] + ((src[in + 0] & 0x3f) << 8);
        info = (src[in + 0] & 0xc0) >> 6;
        plen = ((bits + 47) >> 4) << 1;

        if (plen > 1024) {
            V4LCONVERT_ERR("invalid se401 packet len %d", plen);
            goto error;
        }

        if ((in + plen) > src_size) {
            V4LCONVERT_ERR("incomplete se401 packet");
            goto error;
        }

        if ((total_pixels + pixels) > (width * height)) {
            V4LCONVERT_ERR("se401 frame overflow");
            goto error;
        }

        if (((in == 0) && (info != 2)) || ((in > 0) && ((in + plen) < src_size) && (info != 0)) || (((in + plen) == src_size) && (info != 1))) {
            V4LCONVERT_ERR("invalid se401 frame info value");
            goto error;
        }

        if (decode_JangGu(&src[in + 4], bits, plen, pixels * 3, &dest, width * 3, &x)) {
            V4LCONVERT_ERR("short se401 packet");
            goto error;
        }

        total_pixels += pixels;
    }

    if ((in != src_size) || (total_pixels != (width * height))) {
        V4LCONVERT_ERR("se401 frame size mismatch");
        goto error;
    }

    return 0;

error:
    errno = EIO;
    return -1;
}
