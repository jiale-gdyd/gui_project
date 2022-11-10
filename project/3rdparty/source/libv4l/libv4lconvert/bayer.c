#include <string.h>

#include "libv4lconvert-priv.h"

static void v4lconvert_border_bayer_line_to_bgr24(const unsigned char *bayer, const unsigned char *adjacent_bayer, unsigned char *bgr, int width, const int start_with_green, const int blue_line)
{
    int t0, t1;

    if (start_with_green) {
        if (blue_line) {
            *bgr++ = bayer[1];
            *bgr++ = bayer[0];
            *bgr++ = adjacent_bayer[0];
        } else {
            *bgr++ = adjacent_bayer[0];
            *bgr++ = bayer[0];
            *bgr++ = bayer[1];
        }

        t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
        t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
        if (blue_line) {
            *bgr++ = bayer[1];
            *bgr++ = t0;
            *bgr++ = t1;
        } else {
            *bgr++ = t1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
        }

        bayer++;
        adjacent_bayer++;
        width -= 2;
    } else {
        t0 = (bayer[1] + adjacent_bayer[0] + 1) >> 1;
        if (blue_line) {
            *bgr++ = bayer[0];
            *bgr++ = t0;
            *bgr++ = adjacent_bayer[1];
        } else {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = t0;
            *bgr++ = bayer[0];
        }

        width--;
    }

    if (blue_line) {
        for ( ; width > 2; width -= 2) {
            t0 = (bayer[0] + bayer[2] + 1) >> 1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
            bayer++;
            adjacent_bayer++;

            t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
            t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
            *bgr++ = bayer[1];
            *bgr++ = t0;
            *bgr++ = t1;
            bayer++;
            adjacent_bayer++;
        }
    } else {
        for ( ; width > 2; width -= 2) {
            t0 = (bayer[0] + bayer[2] + 1) >> 1;
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = t0;
            bayer++;
            adjacent_bayer++;

            t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
            t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
            *bgr++ = t1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
            bayer++;
            adjacent_bayer++;
        }
    }

    if (width == 2) {
        t0 = (bayer[0] + bayer[2] + 1) >> 1;
        if (blue_line) {
            *bgr++ = t0;
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
        } else {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = t0;
        }

        t0 = (bayer[1] + adjacent_bayer[2] + 1) >> 1;
        if (blue_line) {
            *bgr++ = bayer[2];
            *bgr++ = t0;
            *bgr++ = adjacent_bayer[1];
        } else {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = t0;
            *bgr++ = bayer[2];
        }
    } else {
        if (blue_line) {
            *bgr++ = bayer[0];
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
        } else {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = bayer[0];
        }
    }
}

static void bayer_to_rgbbgr24(const unsigned char *bayer, unsigned char *bgr, int width, int height, const unsigned int stride, unsigned int pixfmt, int start_with_green, int blue_line)
{
    v4lconvert_border_bayer_line_to_bgr24(bayer, bayer + stride, bgr, width, start_with_green, blue_line);
    bgr += width * 3;

    for (height -= 2; height; height--) {
        int t0, t1;
        const unsigned char *bayer_end = bayer + (width - 2);

        if (start_with_green) {
            t0 = (bayer[1] + bayer[stride * 2 + 1] + 1) >> 1;
            t1 = (bayer[0] + bayer[stride * 2] + bayer[stride + 1] + 1) / 3;
            if (blue_line) {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[stride];
            } else {
                *bgr++ = bayer[stride];
                *bgr++ = t1;
                *bgr++ = t0;
            }

            t1 = (bayer[stride] + bayer[stride + 2] + 1) >> 1;
            if (blue_line) {
                *bgr++ = t0;
                *bgr++ = bayer[stride + 1];
                *bgr++ = t1;
            } else {
                *bgr++ = t1;
                *bgr++ = bayer[stride + 1];
                *bgr++ = t0;
            }

            bayer++;
        } else {
            t0 = (bayer[0] + bayer[stride * 2] + 1) >> 1;
            if (blue_line) {
                *bgr++ = t0;
                *bgr++ = bayer[stride];
                *bgr++ = bayer[stride + 1];
            } else {
                *bgr++ = bayer[stride + 1];
                *bgr++ = bayer[stride];
                *bgr++ = t0;
            }
        }

        if (blue_line) {
            for (; bayer <= bayer_end - 2; bayer += 2) {
                t0 = (bayer[0] + bayer[2] + bayer[stride * 2] + bayer[stride * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[stride] + bayer[stride + 2] + bayer[stride * 2 + 1] + 2) >> 2;
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[stride + 1];

                t0 = (bayer[2] + bayer[stride * 2 + 2] + 1) >> 1;
                t1 = (bayer[stride + 1] + bayer[stride + 3] + 1) >> 1;
                *bgr++ = t0;
                *bgr++ = bayer[stride + 2];
                *bgr++ = t1;
            }
        } else {
            for (; bayer <= bayer_end - 2; bayer += 2) {
                t0 = (bayer[0] + bayer[2] + bayer[stride * 2] + bayer[stride * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[stride] + bayer[stride + 2] + bayer[stride * 2 + 1] + 2) >> 2;
                *bgr++ = bayer[stride + 1];
                *bgr++ = t1;
                *bgr++ = t0;

                t0 = (bayer[2] + bayer[stride * 2 + 2] + 1) >> 1;
                t1 = (bayer[stride + 1] + bayer[stride + 3] + 1) >> 1;
                *bgr++ = t1;
                *bgr++ = bayer[stride + 2];
                *bgr++ = t0;
            }
        }

        if (bayer < bayer_end) {
            t0 = (bayer[0] + bayer[2] + bayer[stride * 2] + bayer[stride * 2 + 2] + 2) >> 2;
            t1 = (bayer[1] + bayer[stride] + bayer[stride + 2] + bayer[stride * 2 + 1] + 2) >> 2;
            if (blue_line) {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[stride + 1];
            } else {
                *bgr++ = bayer[stride + 1];
                *bgr++ = t1;
                *bgr++ = t0;
            }

            t0 = (bayer[2] + bayer[stride * 2 + 2] + 1) >> 1;
            if (blue_line) {
                *bgr++ = t0;
                *bgr++ = bayer[stride + 2];
                *bgr++ = bayer[stride + 1];
            } else {
                *bgr++ = bayer[stride + 1];
                *bgr++ = bayer[stride + 2];
                *bgr++ = t0;
            }

            bayer++;

        } else {
            t0 = (bayer[0] + bayer[stride * 2] + 1) >> 1;
            t1 = (bayer[1] + bayer[stride * 2 + 1] + bayer[stride] + 1) / 3;
            if (blue_line) {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[stride + 1];
            } else {
                *bgr++ = bayer[stride + 1];
                *bgr++ = t1;
                *bgr++ = t0;
            }

        }

        bayer += (stride - width) + 2;
        blue_line = !blue_line;
        start_with_green = !start_with_green;
    }

    v4lconvert_border_bayer_line_to_bgr24(bayer + stride, bayer, bgr, width, !start_with_green, !blue_line);
}

void v4lconvert_bayer_to_rgb24(const unsigned char *bayer, unsigned char *bgr, int width, int height, const unsigned int stride, unsigned int pixfmt)
{
    bayer_to_rgbbgr24(bayer, bgr, width, height, stride, pixfmt, (pixfmt == V4L2_PIX_FMT_SGBRG8) || (pixfmt == V4L2_PIX_FMT_SGRBG8), (pixfmt != V4L2_PIX_FMT_SBGGR8) && (pixfmt != V4L2_PIX_FMT_SGBRG8));
}

void v4lconvert_bayer_to_bgr24(const unsigned char *bayer, unsigned char *bgr, int width, int height, const unsigned int stride, unsigned int pixfmt)
{
    bayer_to_rgbbgr24(bayer, bgr, width, height, stride, pixfmt, (pixfmt == V4L2_PIX_FMT_SGBRG8) || (pixfmt == V4L2_PIX_FMT_SGRBG8), (pixfmt == V4L2_PIX_FMT_SBGGR8) || (pixfmt == V4L2_PIX_FMT_SGBRG8));
}

static void v4lconvert_border_bayer_line_to_y(const unsigned char *bayer, const unsigned char *adjacent_bayer, unsigned char *y, int width, int start_with_green, int blue_line)
{
    int t0, t1;

    if (start_with_green) {
        if (blue_line) {
            *y++ = (8453 * adjacent_bayer[0] + 16594 * bayer[0] + 3223 * bayer[1] + 524288) >> 15;
        } else {
            *y++ = (8453 * bayer[1] + 16594 * bayer[0] + 3223 * adjacent_bayer[0] + 524288) >> 15;
        }

        t0 = bayer[0] + bayer[2] + adjacent_bayer[1];
        t1 = adjacent_bayer[0] + adjacent_bayer[2];
        if (blue_line) {
            *y++ = (4226 * t1 + 5531 * t0 + 3223 * bayer[1] + 524288) >> 15;
        } else {
            *y++ = (8453 * bayer[1] + 5531 * t0 + 1611 * t1 + 524288) >> 15;
        }

        bayer++;
        adjacent_bayer++;
        width -= 2;
    } else {
        t0 = bayer[1] + adjacent_bayer[0];
        if (blue_line) {
            *y++ = (8453 * adjacent_bayer[1] + 8297 * t0 + 3223 * bayer[0] + 524288) >> 15;
        } else {
            *y++ = (8453 * bayer[0] + 8297 * t0 + 3223 * adjacent_bayer[1] + 524288) >> 15;
        }

        width--;
    }

    if (blue_line) {
        for ( ; width > 2; width -= 2) {
            t0 = bayer[0] + bayer[2];
            *y++ = (8453 * adjacent_bayer[1] + 16594 * bayer[1] + 1611 * t0 + 524288) >> 15;
            bayer++;
            adjacent_bayer++;

            t0 = bayer[0] + bayer[2] + adjacent_bayer[1];
            t1 = adjacent_bayer[0] + adjacent_bayer[2];
            *y++ = (4226 * t1 + 5531 * t0 + 3223 * bayer[1] + 524288) >> 15;
            bayer++;
            adjacent_bayer++;
        }
    } else {
        for ( ; width > 2; width -= 2) {
            t0 = bayer[0] + bayer[2];
            *y++ = (4226 * t0 + 16594 * bayer[1] + 3223 * adjacent_bayer[1] + 524288) >> 15;
            bayer++;
            adjacent_bayer++;

            t0 = bayer[0] + bayer[2] + adjacent_bayer[1];
            t1 = adjacent_bayer[0] + adjacent_bayer[2];
            *y++ = (8453 * bayer[1] + 5531 * t0 + 1611 * t1 + 524288) >> 15;
            bayer++;
            adjacent_bayer++;
        }
    }

    if (width == 2) {
        t0 = bayer[0] + bayer[2];
        if (blue_line) {
            *y++ = (8453 * adjacent_bayer[1] + 16594 * bayer[1] + 1611 * t0 + 524288) >> 15;
        } else {
            *y++ = (4226 * t0 + 16594 * bayer[1] + 3223 * adjacent_bayer[1] + 524288) >> 15;
        }

        t0 = bayer[1] + adjacent_bayer[2];
        if (blue_line) {
            *y++ = (8453 * adjacent_bayer[1] + 8297 * t0 + 3223 * bayer[2] + 524288) >> 15;
        } else {
            *y++ = (8453 * bayer[2] + 8297 * t0 + 3223 * adjacent_bayer[1] + 524288) >> 15;
        }
    } else {
        if (blue_line) {
            *y++ = (8453 * adjacent_bayer[1] + 16594 * bayer[1] + 3223 * bayer[0] + 524288) >> 15;
        } else {
            *y++ = (8453 * bayer[0] + 16594 * bayer[1] + 3223 * adjacent_bayer[1] + 524288) >> 15;
        }
    }
}

void v4lconvert_bayer_to_yuv420(const unsigned char *bayer, unsigned char *yuv, int width, int height, const unsigned int stride, unsigned int src_pixfmt, int yvu)
{
    unsigned char *ydst = yuv;
    unsigned char *udst, *vdst;
    int blue_line = 0, start_with_green = 0, x, y;

    if (yvu) {
        vdst = yuv + width * height;
        udst = vdst + width * height / 4;
    } else {
        udst = yuv + width * height;
        vdst = udst + width * height / 4;
    }

    switch (src_pixfmt) {
        case V4L2_PIX_FMT_SBGGR8:
            for (y = 0; y < height; y += 2) {
                for (x = 0; x < width; x += 2) {
                    int b, g, r;

                    b  = bayer[x];
                    g  = bayer[x + 1];
                    g += bayer[x + stride];
                    r  = bayer[x + stride + 1];
                    *udst++ = (-4878 * r - 4789 * g + 14456 * b + 4210688) >> 15;
                    *vdst++ = (14456 * r - 6052 * g -  2351 * b + 4210688) >> 15;
                }
                bayer += 2 * stride;
            }
            blue_line = 1;
            break;

        case V4L2_PIX_FMT_SRGGB8:
            for (y = 0; y < height; y += 2) {
                for (x = 0; x < width; x += 2) {
                    int b, g, r;

                    r  = bayer[x];
                    g  = bayer[x + 1];
                    g += bayer[x + stride];
                    b  = bayer[x + stride + 1];
                    *udst++ = (-4878 * r - 4789 * g + 14456 * b + 4210688) >> 15;
                    *vdst++ = (14456 * r - 6052 * g -  2351 * b + 4210688) >> 15;
                }
                bayer += 2 * stride;
            }
            break;

        case V4L2_PIX_FMT_SGBRG8:
            for (y = 0; y < height; y += 2) {
                for (x = 0; x < width; x += 2) {
                    int b, g, r;

                    g  = bayer[x];
                    b  = bayer[x + 1];
                    r  = bayer[x + stride];
                    g += bayer[x + stride + 1];
                    *udst++ = (-4878 * r - 4789 * g + 14456 * b + 4210688) >> 15;
                    *vdst++ = (14456 * r - 6052 * g -  2351 * b + 4210688) >> 15;
                }
                bayer += 2 * stride;
            }
            blue_line = 1;
            start_with_green = 1;
            break;

        case V4L2_PIX_FMT_SGRBG8:
            for (y = 0; y < height; y += 2) {
                for (x = 0; x < width; x += 2) {
                    int b, g, r;

                    g  = bayer[x];
                    r  = bayer[x + 1];
                    b  = bayer[x + stride];
                    g += bayer[x + stride + 1];
                    *udst++ = (-4878 * r - 4789 * g + 14456 * b + 4210688) >> 15;
                    *vdst++ = (14456 * r - 6052 * g -  2351 * b + 4210688) >> 15;
                }
                bayer += 2 * stride;
            }
            start_with_green = 1;
            break;
    }

    bayer -= stride * height;
    v4lconvert_border_bayer_line_to_y(bayer, bayer + stride, ydst, width, start_with_green, blue_line);
    ydst += width;

    for (height -= 2; height; height--) {
        int t0, t1;
        const unsigned char *bayer_end = bayer + (width - 2);

        if (start_with_green) {
            t0 = bayer[1] + bayer[stride * 2 + 1];
            t1 = bayer[0] + bayer[stride * 2] + bayer[stride + 1];
            if (blue_line) {
                *ydst++ = (8453 * bayer[stride] + 5516 * t1 + 1661 * t0 + 524288) >> 15;
            } else {
                *ydst++ = (4226 * t0 + 5516 * t1 + 3223 * bayer[stride] + 524288) >> 15;
            }

            t1 = bayer[stride] + bayer[stride + 2];
            if (blue_line) {
                *ydst++ = (4226 * t1 + 16594 * bayer[stride + 1] + 1611 * t0 + 524288) >> 15;
            } else {
                *ydst++ = (4226 * t0 + 16594 * bayer[stride + 1] + 1611 * t1 + 524288) >> 15;
            }
            bayer++;
        } else {
            t0 = bayer[0] + bayer[stride * 2];
            if (blue_line) {
                *ydst++ = (8453 * bayer[stride + 1] + 16594 * bayer[stride] + 1661 * t0 + 524288) >> 15;
            } else {
                *ydst++ = (4226 * t0 + 16594 * bayer[stride] + 3223 * bayer[stride + 1] + 524288) >> 15;
            }
        }

        if (blue_line) {
            for (; bayer <= bayer_end - 2; bayer += 2) {
                t0 = bayer[0] + bayer[2] + bayer[stride * 2] + bayer[stride * 2 + 2];
                t1 = bayer[1] + bayer[stride] + bayer[stride + 2] + bayer[stride * 2 + 1];
                *ydst++ = (8453 * bayer[stride + 1] + 4148 * t1 + 806 * t0 + 524288) >> 15;

                t0 = bayer[2] + bayer[stride * 2 + 2];
                t1 = bayer[stride + 1] + bayer[stride + 3];
                *ydst++ = (4226 * t1 + 16594 * bayer[stride + 2] + 1611 * t0 + 524288) >> 15;
            }
        } else {
            for (; bayer <= bayer_end - 2; bayer += 2) {
                t0 = bayer[0] + bayer[2] + bayer[stride * 2] + bayer[stride * 2 + 2];
                t1 = bayer[1] + bayer[stride] + bayer[stride + 2] + bayer[stride * 2 + 1];
                *ydst++ = (2113 * t0 + 4148 * t1 + 3223 * bayer[stride + 1] + 524288) >> 15;

                t0 = bayer[2] + bayer[stride * 2 + 2];
                t1 = bayer[stride + 1] + bayer[stride + 3];
                *ydst++ = (4226 * t0 + 16594 * bayer[stride + 2] + 1611 * t1 + 524288) >> 15;
            }
        }

        if (bayer < bayer_end) {
            t0 = bayer[0] + bayer[2] + bayer[stride * 2] + bayer[stride * 2 + 2];
            t1 = bayer[1] + bayer[stride] + bayer[stride + 2] + bayer[stride * 2 + 1];
            if (blue_line) {
                *ydst++ = (8453 * bayer[stride + 1] + 4148 * t1 + 806 * t0 + 524288) >> 15;
            } else {
                *ydst++ = (2113 * t0 + 4148 * t1 + 3223 * bayer[stride + 1] + 524288) >> 15;
            }

            t0 = bayer[2] + bayer[stride * 2 + 2];
            if (blue_line) {
                *ydst++ = (8453 * bayer[stride + 1] + 16594 * bayer[stride + 2] + 1661 * t0 + 524288) >> 15;
            } else {
                *ydst++ = (4226 * t0 + 16594 * bayer[stride + 2] + 3223 * bayer[stride + 1] + 524288) >> 15;
            }

            bayer++;
        } else {
            t0 = bayer[0] + bayer[stride * 2];
            t1 = bayer[1] + bayer[stride * 2 + 1] + bayer[stride];
            if (blue_line) {
                *ydst++ = (8453 * bayer[stride + 1] + 5516 * t1 + 1661 * t0 + 524288) >> 15;
            } else {
                *ydst++ = (4226 * t0 + 5516 * t1 + 3223 * bayer[stride + 1] + 524288) >> 15;
            }
        }

        bayer += (stride - width) + 2;
        blue_line = !blue_line;
        start_with_green = !start_with_green;
    }

    v4lconvert_border_bayer_line_to_y(bayer + stride, bayer, ydst, width, !start_with_green, !blue_line);
}

void v4lconvert_bayer10_to_bayer8(void *bayer10, unsigned char *bayer8, int width, int height)
{
    int i;
    uint16_t *src = bayer10;

    for (i = 0; i < width * height; i++) {
        bayer8[i] = src[i] >> 2;
    }
}

void v4lconvert_bayer10p_to_bayer8(unsigned char *bayer10p, unsigned char *bayer8, int width, int height)
{
    unsigned long i;
    unsigned long len = width * height;

    for (i = 0; i < len ; i += 4) {
        bayer8[0] = bayer10p[0];
        bayer8[1] = bayer10p[1];
        bayer8[2] = bayer10p[2];
        bayer8[3] = bayer10p[3];
        bayer10p += 5;
        bayer8 += 4;
    }
}

void v4lconvert_bayer16_to_bayer8(unsigned char *bayer16, unsigned char *bayer8, int width, int height)
{
    int i;

    for (i = 0; i < width * height; i++) {
        bayer8[i] = bayer16[2 * i + 1];
    }
}
