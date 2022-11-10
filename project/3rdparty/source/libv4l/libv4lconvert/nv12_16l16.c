#include <string.h>
#include "libv4lconvert-priv.h"

#define CLIP(color)             (unsigned char)(((color) > 0xff) ? 0xff : (((color) < 0) ? 0 : (color)))

static const int stride = 720;

static void v4lconvert_nv12_16l16_to_rgb(const unsigned char *src, unsigned char *dest, int width, int height, int rgb)
{
    int mb_size = 256;
    int r = rgb ? 0 : 2;
    int b = 2 - r;
    unsigned int y, x, i, j;
    const unsigned char *src_y;
    const unsigned char *src_uv;
    const unsigned char *y_base = src;
    const unsigned char *uv_base = src + stride * height;

    for (y = 0; y < height; y += 16) {
        int mb_y = (y / 16) * (stride / 16);
        int mb_uv = (y / 32) * (stride / 16);
        int maxy = (height - y < 16 ? height - y : 16);

        for (x = 0; x < width; x += 16, mb_y++, mb_uv++) {
            int maxx = (width - x < 16 ? width - x : 16);

            src_y = y_base + mb_y * mb_size;
            src_uv = uv_base + mb_uv * mb_size;

            if (y & 0x10) {
                src_uv += mb_size / 2;
            }

            for (i = 0; i < maxy; i++) {
                int idx = (x + (y + i) * width) * 3;

                for (j = 0; j < maxx; j++) {
                    int y = src_y[j];
                    int u = src_uv[j & ~1];
                    int v = src_uv[j | 1];
                    int u1 = (((u - 128) << 7) + (u - 128)) >> 6;
                    int rg = (((u - 128) << 1) + (u - 128) + ((v - 128) << 2) + ((v - 128) << 1)) >> 3;
                    int v1 = (((v - 128) << 1) + (v - 128)) >> 1;

                    dest[idx + r] = CLIP(y + v1);
                    dest[idx + 1] = CLIP(y - rg);
                    dest[idx + b] = CLIP(y + u1);
                    idx += 3;
                }

                src_y += 16;
                if (i & 1) {
                    src_uv += 16;
                }
            }
        }
    }
}

void v4lconvert_nv12_16l16_to_rgb24(const unsigned char *src, unsigned char *dest, int width, int height)
{
    v4lconvert_nv12_16l16_to_rgb(src, dest, width, height, 1);
}

void v4lconvert_nv12_16l16_to_bgr24(const unsigned char *src, unsigned char *dest, int width, int height)
{
    v4lconvert_nv12_16l16_to_rgb(src, dest, width, height, 0);
}

static void de_macro_uv(unsigned char *dstu, unsigned char *dstv, const unsigned char *src, int w, int h)
{
    unsigned int y, x, i, j;

    for (y = 0; y < h; y += 16) {
        for (x = 0; x < w; x += 8) {
            const unsigned char *src_uv = src + y * stride + x * 32;
            int maxy = (h - y < 16 ? h - y : 16);
            int maxx = (w - x < 8 ? w - x : 8);

            for (i = 0; i < maxy; i++) {
                int idx = x + (y + i) * w;

                for (j = 0; j < maxx; j++) {
                    dstu[idx+j] = src_uv[2 * j];
                    dstv[idx+j] = src_uv[2 * j + 1];
                }

                src_uv += 16;
            }
        }
    }
}

static void de_macro_y(unsigned char *dst, const unsigned char *src, int w, int h)
{
    unsigned int y, x, i;

    for (y = 0; y < h; y += 16) {
        for (x = 0; x < w; x += 16) {
            const unsigned char *src_y = src + y * stride + x * 16;
            int maxy = (h - y < 16 ? h - y : 16);
            int maxx = (w - x < 16 ? w - x : 16);

            for (i = 0; i < maxy; i++) {
                memcpy(dst + x + (y + i) * w, src_y, maxx);
                src_y += 16;
            }
        }
    }
}

void v4lconvert_nv12_16l16_to_yuv420(const unsigned char *src, unsigned char *dest, int width, int height, int yvu)
{
    de_macro_y(dest, src, width, height);
    dest += width * height;
    src += stride * height;

    if (yvu) {
        de_macro_uv(dest + width * height / 4, dest, src, width / 2, height / 2);
    } else {
        de_macro_uv(dest, dest + width * height / 4, src, width / 2, height / 2);
    }
}
