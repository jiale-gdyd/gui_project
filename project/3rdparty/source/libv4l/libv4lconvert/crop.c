#include <string.h>

#include "libv4lconvert-priv.h"

static void v4lconvert_reduceandcrop_rgbbgr24(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    int x, y;
    int startx = src_fmt->fmt.pix.width / 2 - dest_fmt->fmt.pix.width;
    int starty = src_fmt->fmt.pix.height / 2 - dest_fmt->fmt.pix.height;

    src += starty * src_fmt->fmt.pix.bytesperline + 3 * startx;

    for (y = 0; y < dest_fmt->fmt.pix.height; y++) {
        unsigned char *mysrc = src;
        for (x = 0; x < dest_fmt->fmt.pix.width; x++) {
            *(dest++) = *(mysrc++);
            *(dest++) = *(mysrc++);
            *(dest++) = *(mysrc++);
            mysrc += 3;
        }

        src += 2 * src_fmt->fmt.pix.bytesperline;
    }
}

static void v4lconvert_crop_rgbbgr24(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    int x;
    int startx = (src_fmt->fmt.pix.width - dest_fmt->fmt.pix.width) / 2;
    int starty = (src_fmt->fmt.pix.height - dest_fmt->fmt.pix.height) / 2;

    src += starty * src_fmt->fmt.pix.bytesperline + 3 * startx;

    for (x = 0; x < dest_fmt->fmt.pix.height; x++) {
        memcpy(dest, src, dest_fmt->fmt.pix.width * 3);
        src += src_fmt->fmt.pix.bytesperline;
        dest += dest_fmt->fmt.pix.bytesperline;
    }
}

static void v4lconvert_reduceandcrop_yuv420(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    int x, y;
    unsigned char *mysrc, *mysrc2;
    int dest_height_half = dest_fmt->fmt.pix.height / 2;
    int dest_width_half = dest_fmt->fmt.pix.width / 2;
    int startx = (src_fmt->fmt.pix.width / 2 - dest_fmt->fmt.pix.width) & ~1;
    int starty = (src_fmt->fmt.pix.height / 2 - dest_fmt->fmt.pix.height) & ~1;

    mysrc = src + starty * src_fmt->fmt.pix.bytesperline + startx;
    for (y = 0; y < dest_fmt->fmt.pix.height; y++) {
        mysrc2 = mysrc;
        for (x = 0; x < dest_fmt->fmt.pix.width; x++) {
            *(dest++) = *mysrc2;
            mysrc2 += 2;
        }

        mysrc += 2 * src_fmt->fmt.pix.bytesperline;
    }

    mysrc = src + src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline + (starty / 2) * src_fmt->fmt.pix.bytesperline / 2 + startx / 2;
    for (y = 0; y < dest_height_half; y++) {
        mysrc2 = mysrc;
        for (x = 0; x < dest_width_half; x++) {
            *(dest++) = *mysrc2;
            mysrc2 += 2;
        }

        mysrc += src_fmt->fmt.pix.bytesperline;
    }

    mysrc = src + src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline * 5 / 4 + (starty / 2) * src_fmt->fmt.pix.bytesperline / 2 + startx / 2;
    for (y = 0; y < dest_height_half; y++) {
        mysrc2 = mysrc;
        for (x = 0; x < dest_width_half; x++) {
            *(dest++) = *mysrc2;
            mysrc2 += 2;
        }

        mysrc += src_fmt->fmt.pix.bytesperline;
    }
}

static void v4lconvert_crop_yuv420(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    int x;
    int startx = ((src_fmt->fmt.pix.width - dest_fmt->fmt.pix.width) / 2) & ~1;
    int starty = ((src_fmt->fmt.pix.height - dest_fmt->fmt.pix.height) / 2) & ~1;
    unsigned char *mysrc = src + starty * src_fmt->fmt.pix.bytesperline + startx;

    for (x = 0; x < dest_fmt->fmt.pix.height; x++) {
        memcpy(dest, mysrc, dest_fmt->fmt.pix.width);
        mysrc += src_fmt->fmt.pix.bytesperline;
        dest += dest_fmt->fmt.pix.bytesperline;
    }

    mysrc = src + src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline + (starty / 2) * src_fmt->fmt.pix.bytesperline / 2 + startx / 2;
    for (x = 0; x < dest_fmt->fmt.pix.height / 2; x++) {
        memcpy(dest, mysrc, dest_fmt->fmt.pix.width / 2);
        mysrc += src_fmt->fmt.pix.bytesperline / 2;
        dest += dest_fmt->fmt.pix.bytesperline / 2;
    }

    mysrc = src + src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline * 5 / 4 + (starty / 2) * src_fmt->fmt.pix.bytesperline / 2 + startx / 2;
    for (x = 0; x < dest_fmt->fmt.pix.height / 2; x++) {
        memcpy(dest, mysrc, dest_fmt->fmt.pix.width / 2);
        mysrc += src_fmt->fmt.pix.bytesperline / 2;
        dest += dest_fmt->fmt.pix.bytesperline / 2;
    }
}

static void v4lconvert_add_border_rgbbgr24(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    int y;
    int borderx = (dest_fmt->fmt.pix.width - src_fmt->fmt.pix.width) / 2;
    int bordery = (dest_fmt->fmt.pix.height - src_fmt->fmt.pix.height) / 2;

    for (y = 0; y < bordery; y++) {
        memset(dest, 0, dest_fmt->fmt.pix.width * 3);
        dest += dest_fmt->fmt.pix.bytesperline;
    }

    for (y = 0; y < src_fmt->fmt.pix.height; y++) {
        memset(dest, 0, borderx * 3);
        dest += borderx * 3;

        memcpy(dest, src, src_fmt->fmt.pix.width * 3);
        src += src_fmt->fmt.pix.bytesperline;
        dest += src_fmt->fmt.pix.width * 3;

        memset(dest, 0, borderx * 3);
        dest += dest_fmt->fmt.pix.bytesperline - (borderx + src_fmt->fmt.pix.width) * 3;
    }

    for (y = 0; y < bordery; y++) {
        memset(dest, 0, dest_fmt->fmt.pix.width * 3);
        dest += dest_fmt->fmt.pix.bytesperline;
    }
}

static void v4lconvert_add_border_yuv420(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    int y;
    int borderx = ((dest_fmt->fmt.pix.width - src_fmt->fmt.pix.width) / 2) & ~1;
    int bordery = ((dest_fmt->fmt.pix.height - src_fmt->fmt.pix.height) / 2) & ~1;

    for (y = 0; y < bordery; y++) {
        memset(dest, 16, dest_fmt->fmt.pix.width);
        dest += dest_fmt->fmt.pix.bytesperline;
    }

    for (y = 0; y < src_fmt->fmt.pix.height; y++) {
        memset(dest, 16, borderx);
        dest += borderx;

        memcpy(dest, src, src_fmt->fmt.pix.width);
        src += src_fmt->fmt.pix.bytesperline;
        dest += src_fmt->fmt.pix.width;

        memset(dest, 16, borderx);
        dest += dest_fmt->fmt.pix.bytesperline - (borderx + src_fmt->fmt.pix.width);
    }

    for (y = 0; y < bordery; y++) {
        memset(dest, 16, dest_fmt->fmt.pix.width);
        dest += dest_fmt->fmt.pix.bytesperline;
    }

    for (y = 0; y < bordery / 2; y++) {
        memset(dest, 128, dest_fmt->fmt.pix.width / 2);
        dest += dest_fmt->fmt.pix.bytesperline / 2;
    }

    for (y = 0; y < src_fmt->fmt.pix.height / 2; y++) {
        memset(dest, 128, borderx / 2);
        dest += borderx / 2;

        memcpy(dest, src, src_fmt->fmt.pix.width / 2);
        src += src_fmt->fmt.pix.bytesperline / 2;
        dest += src_fmt->fmt.pix.width / 2;

        memset(dest, 128, borderx / 2);
        dest += (dest_fmt->fmt.pix.bytesperline - (borderx + src_fmt->fmt.pix.width)) / 2;
    }

    for (y = 0; y < bordery / 2; y++) {
        memset(dest, 128, dest_fmt->fmt.pix.width / 2);
        dest += dest_fmt->fmt.pix.bytesperline / 2;
    }

    for (y = 0; y < bordery / 2; y++) {
        memset(dest, 128, dest_fmt->fmt.pix.width / 2);
        dest += dest_fmt->fmt.pix.bytesperline / 2;
    }

    for (y = 0; y < src_fmt->fmt.pix.height / 2; y++) {
        memset(dest, 128, borderx / 2);
        dest += borderx / 2;

        memcpy(dest, src, src_fmt->fmt.pix.width / 2);
        src += src_fmt->fmt.pix.bytesperline / 2;
        dest += src_fmt->fmt.pix.width / 2;

        memset(dest, 128, borderx / 2);
        dest += (dest_fmt->fmt.pix.bytesperline - (borderx + src_fmt->fmt.pix.width)) / 2;
    }

    for (y = 0; y < bordery / 2; y++) {
        memset(dest, 128, dest_fmt->fmt.pix.width / 2);
        dest += dest_fmt->fmt.pix.bytesperline / 2;
    }
}

void v4lconvert_crop(unsigned char *src, unsigned char *dest, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    switch (dest_fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            if ((src_fmt->fmt.pix.width <= dest_fmt->fmt.pix.width) && (src_fmt->fmt.pix.height <= dest_fmt->fmt.pix.height)) {
                v4lconvert_add_border_rgbbgr24(src, dest, src_fmt, dest_fmt);
            } else if ((src_fmt->fmt.pix.width >= (2 * dest_fmt->fmt.pix.width)) && (src_fmt->fmt.pix.height >= (2 * dest_fmt->fmt.pix.height))) {
                v4lconvert_reduceandcrop_rgbbgr24(src, dest, src_fmt, dest_fmt);
            } else {
                v4lconvert_crop_rgbbgr24(src, dest, src_fmt, dest_fmt);
            }
            break;

        case V4L2_PIX_FMT_YUV420:
        case V4L2_PIX_FMT_YVU420:
            if ((src_fmt->fmt.pix.width <= dest_fmt->fmt.pix.width) && (src_fmt->fmt.pix.height <= dest_fmt->fmt.pix.height)) {
                v4lconvert_add_border_yuv420(src, dest, src_fmt, dest_fmt);
            } else if ((src_fmt->fmt.pix.width >= (2 * dest_fmt->fmt.pix.width)) && (src_fmt->fmt.pix.height >= (2 * dest_fmt->fmt.pix.height))) {
                v4lconvert_reduceandcrop_yuv420(src, dest, src_fmt, dest_fmt);
            } else {
                v4lconvert_crop_yuv420(src, dest, src_fmt, dest_fmt);
            }
            break;
    }
}
