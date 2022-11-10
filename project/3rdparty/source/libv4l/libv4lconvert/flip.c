#include <string.h>

#include "libv4lconvert-priv.h"

static void v4lconvert_vflip_rgbbgr24(unsigned char *src, unsigned char *dest, struct v4l2_format *fmt)
{
    int y;

    src += fmt->fmt.pix.height * fmt->fmt.pix.bytesperline;
    for (y = 0; y < fmt->fmt.pix.height; y++) {
        src -= fmt->fmt.pix.bytesperline;
        memcpy(dest, src, fmt->fmt.pix.width * 3);
        dest += fmt->fmt.pix.width * 3;
    }
}

static void v4lconvert_vflip_yuv420(unsigned char *src, unsigned char *dest, struct v4l2_format *fmt)
{
    int y;

    src += fmt->fmt.pix.height * fmt->fmt.pix.bytesperline;
    for (y = 0; y < fmt->fmt.pix.height; y++) {
        src -= fmt->fmt.pix.bytesperline;
        memcpy(dest, src, fmt->fmt.pix.width);
        dest += fmt->fmt.pix.width;
    }

    src += fmt->fmt.pix.height * fmt->fmt.pix.bytesperline * 5 / 4;
    for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
        src -= fmt->fmt.pix.bytesperline / 2;
        memcpy(dest, src, fmt->fmt.pix.width / 2);
        dest += fmt->fmt.pix.width / 2;
    }

    src += fmt->fmt.pix.height * fmt->fmt.pix.bytesperline / 2;
    for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
        src -= fmt->fmt.pix.bytesperline / 2;
        memcpy(dest, src, fmt->fmt.pix.width / 2);
        dest += fmt->fmt.pix.width / 2;
    }
}

static void v4lconvert_hflip_rgbbgr24(unsigned char *src, unsigned char *dest, struct v4l2_format *fmt)
{
    int x, y;

    for (y = 0; y < fmt->fmt.pix.height; y++) {
        src += fmt->fmt.pix.width * 3;
        for (x = 0; x < fmt->fmt.pix.width; x++) {
            src -= 3;
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest += 3;
        }

        src += fmt->fmt.pix.bytesperline;
    }
}

static void v4lconvert_hflip_yuv420(unsigned char *src, unsigned char *dest, struct v4l2_format *fmt)
{
    int x, y;

    for (y = 0; y < fmt->fmt.pix.height; y++) {
        src += fmt->fmt.pix.width;
        for (x = 0; x < fmt->fmt.pix.width; x++) {
            *dest++ = *--src;
        }

        src += fmt->fmt.pix.bytesperline;
    }

    for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
        src += fmt->fmt.pix.width / 2;
        for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
            *dest++ = *--src;
        }

        src += fmt->fmt.pix.bytesperline / 2;
    }

    for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
        src += fmt->fmt.pix.width / 2;
        for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
            *dest++ = *--src;
        }

        src += fmt->fmt.pix.bytesperline / 2;
    }
}

static void v4lconvert_rotate180_rgbbgr24(const unsigned char *src, unsigned char *dst, int width, int height)
{
    int i;

    src += 3 * width * height - 3;

    for (i = 0; i < width * height; i++) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst += 3;
        src -= 3;
    }
}

static void v4lconvert_rotate180_yuv420(const unsigned char *src, unsigned char *dst, int width, int height)
{
    int i;

    src += width * height - 1;
    for (i = 0; i < width * height; i++) {
        *dst++ = *src--;
    }

    src += width * height * 5 / 4;
    for (i = 0; i < width * height / 4; i++) {
        *dst++ = *src--;
    }

    src += width * height / 2;
    for (i = 0; i < width * height / 4; i++) {
        *dst++ = *src--;
    }
}

static void v4lconvert_rotate90_rgbbgr24(const unsigned char *src, unsigned char *dst, int destwidth, int destheight)
{
    int x, y;
#define srcwidth destheight
#define srcheight destwidth

    for (y = 0; y < destheight; y++)
        for (x = 0; x < destwidth; x++) {
            int offset = ((srcheight - x - 1) * srcwidth + y) * 3;
            *dst++ = src[offset++];
            *dst++ = src[offset++];
            *dst++ = src[offset];
        }
}

static void v4lconvert_rotate90_yuv420(const unsigned char *src, unsigned char *dst, int destwidth, int destheight)
{
    int x, y;

    for (y = 0; y < destheight; y++) {
        for (x = 0; x < destwidth; x++) {
            int offset = (srcheight - x - 1) * srcwidth + y;
            *dst++ = src[offset];
        }
    }

    src += srcwidth * srcheight;
    destwidth /= 2;
    destheight /= 2;

    for (y = 0; y < destheight; y++) {
        for (x = 0; x < destwidth; x++) {
            int offset = (srcheight - x - 1) * srcwidth + y;
            *dst++ = src[offset];
        }
    }

    src += srcwidth * srcheight;
    for (y = 0; y < destheight; y++) {
        for (x = 0; x < destwidth; x++) {
            int offset = (srcheight - x - 1) * srcwidth + y;
            *dst++ = src[offset];
        }
    }
}

void v4lconvert_rotate90(unsigned char *src, unsigned char *dest, struct v4l2_format *fmt)
{
    int tmp;

    tmp = fmt->fmt.pix.width;
    fmt->fmt.pix.width = fmt->fmt.pix.height;
    fmt->fmt.pix.height = tmp;

    switch (fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            v4lconvert_rotate90_rgbbgr24(src, dest, fmt->fmt.pix.width, fmt->fmt.pix.height);
            break;

        case V4L2_PIX_FMT_YUV420:
        case V4L2_PIX_FMT_YVU420:
            v4lconvert_rotate90_yuv420(src, dest, fmt->fmt.pix.width, fmt->fmt.pix.height);
            break;
    }

    v4lconvert_fixup_fmt(fmt);
}

void v4lconvert_flip(unsigned char *src, unsigned char *dest, struct v4l2_format *fmt, int hflip, int vflip)
{
    if (vflip && hflip) {
        switch (fmt->fmt.pix.pixelformat) {
            case V4L2_PIX_FMT_RGB24:
            case V4L2_PIX_FMT_BGR24:
                v4lconvert_rotate180_rgbbgr24(src, dest, fmt->fmt.pix.width, fmt->fmt.pix.height);
                break;

            case V4L2_PIX_FMT_YUV420:
            case V4L2_PIX_FMT_YVU420:
                v4lconvert_rotate180_yuv420(src, dest, fmt->fmt.pix.width, fmt->fmt.pix.height);
                break;
        }
    } else if (hflip) {
        switch (fmt->fmt.pix.pixelformat) {
            case V4L2_PIX_FMT_RGB24:
            case V4L2_PIX_FMT_BGR24:
                v4lconvert_hflip_rgbbgr24(src, dest, fmt);
                break;

            case V4L2_PIX_FMT_YUV420:
            case V4L2_PIX_FMT_YVU420:
                v4lconvert_hflip_yuv420(src, dest, fmt);
                break;
        }
    } else if (vflip) {
        switch (fmt->fmt.pix.pixelformat) {
            case V4L2_PIX_FMT_RGB24:
            case V4L2_PIX_FMT_BGR24:
                v4lconvert_vflip_rgbbgr24(src, dest, fmt);
                break;

            case V4L2_PIX_FMT_YUV420:
            case V4L2_PIX_FMT_YVU420:
                v4lconvert_vflip_yuv420(src, dest, fmt);
                break;
        }
    }

    v4lconvert_fixup_fmt(fmt);
}
