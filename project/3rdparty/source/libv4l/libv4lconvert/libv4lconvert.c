#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libv4l/libv4lconvert.h>

#include "../config.h"
#include "libv4lconvert-priv.h"
#include "libv4lsyscall-priv.h"

#define MIN(a, b)                   (((a) < (b)) ? (a) : (b))
#define BIT_MASK(nr)                (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)                ((nr) / BITS_PER_LONG)

static inline void set_bit(int nr, volatile unsigned long *addr)
{
    unsigned long mask = BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

    *p  |= mask;
}

static inline void clear_bit(int nr, volatile unsigned long *addr)
{
    unsigned long mask = BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

    *p &= ~mask;
}

static inline int test_bit(int nr, const volatile unsigned long *addr)
{
    return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}

static void *dev_init(int fd)
{
    return NULL;
}

static void dev_close(void *dev_ops_priv)
{

}

static int dev_ioctl(void *dev_ops_priv, int fd, unsigned long cmd, void *arg)
{
    return SYS_IOCTL(fd, cmd, arg);
}

static ssize_t dev_read(void *dev_ops_priv, int fd, void *buf, size_t len)
{
    return SYS_READ(fd, buf, len);
}

static ssize_t dev_write(void *dev_ops_priv, int fd, const void *buf, size_t len)
{
    return SYS_WRITE(fd, buf, len);
}

static const struct libv4l_dev_ops default_dev_ops = {
    .init  = dev_init,
    .close = dev_close,
    .ioctl = dev_ioctl,
    .read  = dev_read,
    .write = dev_write,
};

const struct libv4l_dev_ops *v4lconvert_get_default_dev_ops()
{
    return &default_dev_ops;
}

static void v4lconvert_get_framesizes(struct v4lconvert_data *data, unsigned int pixelformat, int index);

#define SUPPORTED_DST_PIXFMTS                       \
    { V4L2_PIX_FMT_RGB24,       24, 1,  5,  0 },    \
    { V4L2_PIX_FMT_BGR24,       24, 1,  5,  0 },    \
    { V4L2_PIX_FMT_YUV420,      12, 6,  1,  0 },    \
    { V4L2_PIX_FMT_YVU420,      12, 6,  1,  0 }

static const struct v4lconvert_pixfmt supported_src_pixfmts[] = {
    SUPPORTED_DST_PIXFMTS,
    /* packed rgb formats */
    { V4L2_PIX_FMT_RGB565,      16, 4,  6,  0 },
    { V4L2_PIX_FMT_BGR32,       32, 4,  6,  0 },
    { V4L2_PIX_FMT_RGB32,       32, 4,  6,  0 },
    { V4L2_PIX_FMT_XBGR32,      32, 4,  6,  0 },
    { V4L2_PIX_FMT_XRGB32,      32, 4,  6,  0 },
    { V4L2_PIX_FMT_ABGR32,      32, 4,  6,  0 },
    { V4L2_PIX_FMT_ARGB32,      32, 4,  6,  0 },
    /* yuv 4:2:2 formats */
    { V4L2_PIX_FMT_YUYV,        16, 5,   4, 0 },
    { V4L2_PIX_FMT_YVYU,        16, 5,   4, 0 },
    { V4L2_PIX_FMT_UYVY,        16, 5,   4, 0 },
    { V4L2_PIX_FMT_NV16,        16, 5,   4, 1 },
    { V4L2_PIX_FMT_NV61,        16, 5,   4, 1 },
    /* yuv 4:2:0 formats */
    { V4L2_PIX_FMT_SPCA501,     12, 6,   3, 1 },
    { V4L2_PIX_FMT_SPCA505,     12, 6,   3, 1 },
    { V4L2_PIX_FMT_SPCA508,     12, 6,   3, 1 },
    { V4L2_PIX_FMT_CIT_YYVYUY,  12, 6,   3, 1 },
    { V4L2_PIX_FMT_KONICA420,   12, 6,   3, 1 },
    { V4L2_PIX_FMT_SN9C20X_I420,12, 6,   3, 1 },
    { V4L2_PIX_FMT_M420,        12, 6,   3, 1 },
    { V4L2_PIX_FMT_NV12_16L16,  12, 6,   3, 1 },
    { V4L2_PIX_FMT_NV12,        12, 6,   3, 1 },
    { V4L2_PIX_FMT_CPIA1,       0,  6,   3, 1 },
    /* JPEG and variants */
    { V4L2_PIX_FMT_MJPEG,       0,   7,  7, 0 },
    { V4L2_PIX_FMT_JPEG,        0,   7,  7, 0 },
    { V4L2_PIX_FMT_PJPG,        0,   7,  7, 1 },
    { V4L2_PIX_FMT_JPGL,        0,   7,  7, 1 },
#ifdef HAVE_LIBV4LCONVERT_HELPERS
    { V4L2_PIX_FMT_OV511,       0,   7,  7, 1 },
    { V4L2_PIX_FMT_OV518,       0,   7,  7, 1 },
#endif
    /* uncompressed bayer */
    { V4L2_PIX_FMT_SBGGR8,      8,   8,  8, 0 },
    { V4L2_PIX_FMT_SGBRG8,      8,   8,  8, 0 },
    { V4L2_PIX_FMT_SGRBG8,      8,   8,  8, 0 },
    { V4L2_PIX_FMT_SRGGB8,      8,   8,  8, 0 },
    { V4L2_PIX_FMT_STV0680,     8,   8,  8, 1 },
    { V4L2_PIX_FMT_SBGGR10P,    10,  8,  8, 1 },
    { V4L2_PIX_FMT_SGBRG10P,    10,  8,  8, 1 },
    { V4L2_PIX_FMT_SGRBG10P,    10,  8,  8, 1 },
    { V4L2_PIX_FMT_SRGGB10P,    10,  8,  8, 1 },
    { V4L2_PIX_FMT_SBGGR10,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SGBRG10,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SGRBG10,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SRGGB10,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SBGGR16,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SGBRG16,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SGRBG16,     16,  8,  8, 1 },
    { V4L2_PIX_FMT_SRGGB16,     16,  8,  8, 1 },
    /* compressed bayer */
    { V4L2_PIX_FMT_SPCA561,     0,   9, 9,  1 },
    { V4L2_PIX_FMT_SN9C10X,     0,   9, 9,  1 },
    { V4L2_PIX_FMT_SN9C2028,    0,   9, 9,  1 },
    { V4L2_PIX_FMT_PAC207,      0,   9, 9,  1 },
    { V4L2_PIX_FMT_MR97310A,    0,   9, 9,  1 },
#if defined(CONFIG_JPEG)
    { V4L2_PIX_FMT_JL2005BCD,    0,  9,  9, 1 },
#endif
    { V4L2_PIX_FMT_SQ905C,      0,   9,  9, 1 },
    /* special */
    { V4L2_PIX_FMT_SE401,       0,   8,  9, 1 },
    /* grey formats */
    { V4L2_PIX_FMT_GREY,        8,  20, 20, 0 },
    { V4L2_PIX_FMT_Y4,          8,  20, 20, 0 },
    { V4L2_PIX_FMT_Y6,          8,  20, 20, 0 },
    { V4L2_PIX_FMT_Y10BPACK,    10, 20, 20, 0 },
    { V4L2_PIX_FMT_Y16,         16, 20, 20, 0 },
    { V4L2_PIX_FMT_Y16_BE,      16, 20, 20, 0 },
    /* hsv formats */
    { V4L2_PIX_FMT_HSV32,       32,  5, 4, 0 },
    { V4L2_PIX_FMT_HSV24,       24,  5, 4, 0 },
};

static const struct v4lconvert_pixfmt supported_dst_pixfmts[] = {
    SUPPORTED_DST_PIXFMTS
};

static const int v4lconvert_crop_res[][2] = {
    { 320, 240 },
    { 160, 120 },
    { 352, 288 },
    { 176, 144 },
};

struct v4lconvert_data *v4lconvert_create(int fd)
{
    return v4lconvert_create_with_dev_ops(fd, NULL, &default_dev_ops); 
}

struct v4lconvert_data *v4lconvert_create_with_dev_ops(int fd, void *dev_ops_priv, const struct libv4l_dev_ops *dev_ops)
{
    int i, j;
    struct v4l2_capability cap;
    int always_needs_conversion = 1;
    struct v4lconvert_data *data = calloc(1, sizeof(struct v4lconvert_data));

    if (!data) {
        fprintf(stderr, "libv4lconvert: error: out of memory!\n");
        return NULL;
    }

    data->fd = fd;
    data->dev_ops = dev_ops;
    data->dev_ops_priv = dev_ops_priv;
    data->decompress_pid = -1;
    data->fps = 30;

    for (i = 0; ; i++) {
        struct v4l2_fmtdesc fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };

        fmt.index = i;

        if (data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUM_FMT, &fmt)) {
            break;
        }

        for (j = 0; j < ARRAY_SIZE(supported_src_pixfmts); j++) {
            if (fmt.pixelformat == supported_src_pixfmts[j].fmt) {
                break;
            }
        }

        if (j < ARRAY_SIZE(supported_src_pixfmts)) {
            set_bit(j, data->supported_src_formats);
            v4lconvert_get_framesizes(data, fmt.pixelformat, j);
            if (!supported_src_pixfmts[j].needs_conversion) {
                always_needs_conversion = 0;
            }
        } else {
            always_needs_conversion = 0;
        }
    }

    data->no_formats = i;
    if (data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCAP, &cap) == 0) {
        if (!strcmp((char *)cap.driver, "uvcvideo")) {
            data->flags |= V4LCONVERT_IS_UVC;
        }

        if (cap.capabilities & V4L2_CAP_DEVICE_CAPS) {
            cap.capabilities = cap.device_caps;
        }

        if ((cap.capabilities & 0xff) & ~V4L2_CAP_VIDEO_CAPTURE) {
            always_needs_conversion = 0;
        }
    }

    data->control = v4lcontrol_create(fd, dev_ops_priv, dev_ops, always_needs_conversion);
    if (!data->control) {
        free(data);
        return NULL;
    }

    data->bandwidth = v4lcontrol_get_bandwidth(data->control);
    data->control_flags = v4lcontrol_get_flags(data->control);
    if (data->control_flags & V4LCONTROL_FORCE_TINYJPEG) {
        data->flags |= V4LCONVERT_USE_TINYJPEG;
    }

    data->processing = v4lprocessing_create(fd, data->control);
    if (!data->processing) {
        v4lcontrol_destroy(data->control);
        free(data);
        return NULL;
    }

    return data;
}

void v4lconvert_destroy(struct v4lconvert_data *data)
{
    if (!data) {
        return;
    }

    v4lprocessing_destroy(data->processing);
    v4lcontrol_destroy(data->control);
    if (data->tinyjpeg) {
        unsigned char *comps[3] = { NULL, NULL, NULL };

        tinyjpeg_set_components(data->tinyjpeg, comps, 3);
        tinyjpeg_free(data->tinyjpeg);
    }

#if defined(CONFIG_JPEG)
    if (data->cinfo_initialized) {
        jpeg_destroy_decompress(&data->cinfo);
    }
#endif

#ifdef HAVE_LIBV4LCONVERT_HELPERS
    v4lconvert_helper_cleanup(data);
#endif

    free(data->convert1_buf);
    free(data->convert2_buf);
    free(data->rotate90_buf);
    free(data->flip_buf);
    free(data->convert_pixfmt_buf);
    free(data->previous_frame);
    free(data);
}

int v4lconvert_supported_dst_format(unsigned int pixelformat)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(supported_dst_pixfmts); i++) {
        if (supported_dst_pixfmts[i].fmt == pixelformat) {
            break;
        }
    }

    return i != ARRAY_SIZE(supported_dst_pixfmts);
}

int v4lconvert_supported_dst_fmt_only(struct v4lconvert_data *data)
{
    int i;

    for (i = 0 ; i < ARRAY_SIZE(data->supported_src_formats); i++) {
        if (data->supported_src_formats[i]) {
            break;
        }
    }

    if (i == ARRAY_SIZE(data->supported_src_formats)) {
        return 0;
    }

    return v4lcontrol_needs_conversion(data->control);
}

int v4lconvert_enum_fmt(struct v4lconvert_data *data, struct v4l2_fmtdesc *fmt)
{
    int i, no_faked_fmts = 0;
    unsigned int faked_fmts[ARRAY_SIZE(supported_dst_pixfmts)];

    if ((fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) || (!v4lconvert_supported_dst_fmt_only(data) && (fmt->index < data->no_formats))) {
        return data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUM_FMT, fmt);
    }

    for (i = 0; i < ARRAY_SIZE(supported_dst_pixfmts); i++) {
        if (v4lconvert_supported_dst_fmt_only(data) || !test_bit(i, data->supported_src_formats)) {
            faked_fmts[no_faked_fmts] = supported_dst_pixfmts[i].fmt;
            no_faked_fmts++;
        }
    }

    if (!v4lconvert_supported_dst_fmt_only(data)) {
        i = fmt->index - data->no_formats;
    } else {
        i = fmt->index;
    }

    if (i >= no_faked_fmts) {
        errno = EINVAL;
        return -1;
    }

    fmt->flags = V4L2_FMT_FLAG_EMULATED;
    fmt->pixelformat = faked_fmts[i];
    fmt->description[0] = faked_fmts[i] & 0xff;
    fmt->description[1] = (faked_fmts[i] >> 8) & 0xff;
    fmt->description[2] = (faked_fmts[i] >> 16) & 0xff;
    fmt->description[3] = faked_fmts[i] >> 24;
    fmt->description[4] = '\0';
    memset(fmt->reserved, 0, sizeof(fmt->reserved));

    return 0;
}

static int v4lconvert_get_rank(struct v4lconvert_data *data, int src_index, int src_width, int src_height, unsigned int dest_pixelformat)
{
    int needed, rank = 0;

    switch (dest_pixelformat) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            rank = supported_src_pixfmts[src_index].rgb_rank;
            break;

        case V4L2_PIX_FMT_YUV420:
        case V4L2_PIX_FMT_YVU420:
            rank = supported_src_pixfmts[src_index].yuv_rank;
            break;
    }

    if (supported_src_pixfmts[src_index].fmt == dest_pixelformat) {
        rank--;
    }

    needed = src_width * src_height * data->fps * supported_src_pixfmts[src_index].bpp / 8;
    if (data->bandwidth && (needed > data->bandwidth)) {
        rank += 10;
    }

    return rank;
}

static int v4lconvert_do_try_format_uvc(struct v4lconvert_data *data, struct v4l2_format *dest_fmt, struct v4l2_format *src_fmt)
{
    int i, rank;
    int best_format = 0;
    int best_rank = 100;
    int best_framesize = 0;
    unsigned int closest_fmt_size_diff = -1;

    for (i = 0; i < data->no_framesizes; i++) {
        if ((data->framesizes[i].discrete.width <= dest_fmt->fmt.pix.width) && (data->framesizes[i].discrete.height <= dest_fmt->fmt.pix.height)) {
            int size_x_diff = dest_fmt->fmt.pix.width - data->framesizes[i].discrete.width;
            int size_y_diff = dest_fmt->fmt.pix.height - data->framesizes[i].discrete.height;
            unsigned int size_diff = size_x_diff * size_x_diff + size_y_diff * size_y_diff;

            if (size_diff < closest_fmt_size_diff) {
                closest_fmt_size_diff = size_diff;
                best_framesize = i;
            }
        }
    }

    for (i = 0; i < ARRAY_SIZE(supported_src_pixfmts); i++) {
        if (!(data->framesize_supported_src_formats[best_framesize] & (1ULL << i))) {
            continue;
        }

        rank = v4lconvert_get_rank(data, i, data->framesizes[best_framesize].discrete.width, data->framesizes[best_framesize].discrete.height, dest_fmt->fmt.pix.pixelformat);
        if (rank < best_rank) {
            best_rank = rank;
            best_format = supported_src_pixfmts[i].fmt;
        }
    }

    dest_fmt->fmt.pix.width = data->framesizes[best_framesize].discrete.width;
    dest_fmt->fmt.pix.height = data->framesizes[best_framesize].discrete.height;
    dest_fmt->fmt.pix.field = V4L2_FIELD_NONE;
    dest_fmt->fmt.pix.bytesperline = 0;
    dest_fmt->fmt.pix.sizeimage = 0;
    dest_fmt->fmt.pix.colorspace = V4L2_COLORSPACE_DEFAULT;
    dest_fmt->fmt.pix.priv = 0;

    *src_fmt = *dest_fmt;
    src_fmt->fmt.pix.pixelformat = best_format;

    return 0;
}

static int v4lconvert_do_try_format(struct v4lconvert_data *data, struct v4l2_format *dest_fmt, struct v4l2_format *src_fmt)
{
    unsigned int size_diff, closest_fmt_size_diff = -1;
    int i, size_x_diff, size_y_diff, rank, best_rank = 0;
    struct v4l2_format try_fmt, closest_fmt = { .type = 0 };
    unsigned int desired_pixfmt = dest_fmt->fmt.pix.pixelformat;

    if (data->flags & V4LCONVERT_IS_UVC) {
        return v4lconvert_do_try_format_uvc(data, dest_fmt, src_fmt);
    }

    for (i = 0; i < ARRAY_SIZE(supported_src_pixfmts); i++) {
        if (!test_bit(i, data->supported_src_formats)){
            continue;
        }

        try_fmt = *dest_fmt;
        try_fmt.fmt.pix.pixelformat = supported_src_pixfmts[i].fmt;
        if (data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_TRY_FMT, &try_fmt)){
            continue;
        }

        if (try_fmt.fmt.pix.pixelformat != supported_src_pixfmts[i].fmt){
            continue;
        }

        size_x_diff = (int)try_fmt.fmt.pix.width - (int)dest_fmt->fmt.pix.width;
        size_y_diff = (int)try_fmt.fmt.pix.height - (int)dest_fmt->fmt.pix.height;
        size_diff = size_x_diff * size_x_diff + size_y_diff * size_y_diff;

        rank = v4lconvert_get_rank(data, i, try_fmt.fmt.pix.width, try_fmt.fmt.pix.height, desired_pixfmt);
        if ((size_diff < closest_fmt_size_diff) || ((size_diff == closest_fmt_size_diff) && (rank < best_rank))) {
            closest_fmt = try_fmt;
            closest_fmt_size_diff = size_diff;
            best_rank = rank;
        }
    }

    if (closest_fmt.type == 0){
        return -1;
    }

    *dest_fmt = closest_fmt;
    if (closest_fmt.fmt.pix.pixelformat != desired_pixfmt){
        dest_fmt->fmt.pix.pixelformat = desired_pixfmt;
    }
    *src_fmt = closest_fmt;

    return 0;
}

void v4lconvert_fixup_fmt(struct v4l2_format *fmt)
{
    switch (fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            fmt->fmt.pix.bytesperline = fmt->fmt.pix.width * 3;
            fmt->fmt.pix.sizeimage = fmt->fmt.pix.width * fmt->fmt.pix.height * 3;
            break;

        case V4L2_PIX_FMT_YUV420:
        case V4L2_PIX_FMT_YVU420:
            fmt->fmt.pix.bytesperline = fmt->fmt.pix.width;
            fmt->fmt.pix.sizeimage = fmt->fmt.pix.width * fmt->fmt.pix.height * 3 / 2;
            break;
    }
}

int v4lconvert_try_format(struct v4lconvert_data *data, struct v4l2_format *dest_fmt, struct v4l2_format *src_fmt)
{
    int i, result;
    unsigned int desired_width = dest_fmt->fmt.pix.width;
    unsigned int desired_height = dest_fmt->fmt.pix.height;
    struct v4l2_format try_src, try_dest, try2_src, try2_dest;

    if ((dest_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) && v4lconvert_supported_dst_fmt_only(data) && !v4lconvert_supported_dst_format(dest_fmt->fmt.pix.pixelformat)){
        dest_fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    }

    try_dest = *dest_fmt;

    if (!v4lconvert_supported_dst_format(dest_fmt->fmt.pix.pixelformat)
        || dest_fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE
        || v4lconvert_do_try_format(data, &try_dest, &try_src))
    {
        result = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_TRY_FMT, dest_fmt);
        if (src_fmt){
            *src_fmt = *dest_fmt;
        }

        return result;
    }

    if ((try_dest.fmt.pix.width != desired_width) || (try_dest.fmt.pix.height != desired_height)) {
        try2_dest = *dest_fmt;
        try2_dest.fmt.pix.width  = desired_width + 7;
        try2_dest.fmt.pix.height = desired_height + 1;

        result = v4lconvert_do_try_format(data, &try2_dest, &try2_src);
        if (result == 0
            && try2_dest.fmt.pix.width >= desired_width
            && try2_dest.fmt.pix.width <= desired_width + 7
            && try2_dest.fmt.pix.height >= desired_height
            && try2_dest.fmt.pix.height <= desired_height + 1)
        {
            try2_dest.fmt.pix.width = desired_width;
            try2_dest.fmt.pix.height = desired_height;
            try_dest = try2_dest;
            try_src = try2_src;
        }
    }

    if ((try_dest.fmt.pix.width != desired_width) || (try_dest.fmt.pix.height != desired_height)) {
        for (i = 0; i < ARRAY_SIZE(v4lconvert_crop_res); i++) {
            if ((v4lconvert_crop_res[i][0] == desired_width) && (v4lconvert_crop_res[i][1] == desired_height)) {
                try2_dest = *dest_fmt;

                try2_dest.fmt.pix.width = desired_width * 113 / 100;
                try2_dest.fmt.pix.height = desired_height * 124 / 100;
                result = v4lconvert_do_try_format(data, &try2_dest, &try2_src);
                if (result == 0
                    && ((try2_dest.fmt.pix.width >= desired_width - 16
                    && try2_dest.fmt.pix.width <= desired_width
                    && try2_dest.fmt.pix.height >= desired_height - 16
                    && try2_dest.fmt.pix.height <= desired_height)
                    || (try2_dest.fmt.pix.width >= desired_width
                    && try2_dest.fmt.pix.width <= desired_width * 5 / 4
                    && try2_dest.fmt.pix.height >= desired_height
                    && try2_dest.fmt.pix.height <= desired_height * 5 / 4)
                    || (try2_dest.fmt.pix.width >= desired_width * 2
                    && try2_dest.fmt.pix.width <= desired_width * 5 / 2
                    && try2_dest.fmt.pix.height >= desired_height * 2
                    && try2_dest.fmt.pix.height <= desired_height * 5 / 2)))
                {
                    try2_dest.fmt.pix.width = desired_width;
                    try2_dest.fmt.pix.height = desired_height;
                    try_dest = try2_dest;
                    try_src = try2_src;
                }

                break;
            }
        }
    }

    try_dest.fmt.pix.width &= ~7;
    try_dest.fmt.pix.height &= ~1;

    if (try_src.fmt.pix.width != try_dest.fmt.pix.width
        || try_src.fmt.pix.height != try_dest.fmt.pix.height
        || try_src.fmt.pix.pixelformat != try_dest.fmt.pix.pixelformat)
    {
        v4lconvert_fixup_fmt(&try_dest);
    }

    *dest_fmt = try_dest;
    if (src_fmt){
        *src_fmt = try_src;
    }

    return 0;
}

int v4lconvert_needs_conversion(struct v4lconvert_data *data, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt)
{
    if (src_fmt->fmt.pix.width != dest_fmt->fmt.pix.width
        || src_fmt->fmt.pix.height != dest_fmt->fmt.pix.height
        || src_fmt->fmt.pix.pixelformat != dest_fmt->fmt.pix.pixelformat
        || (v4lcontrol_needs_conversion(data->control)
        && v4lconvert_supported_dst_format(dest_fmt->fmt.pix.pixelformat)))
    {
        return 1;
    }

    return 0;
}

static int v4lconvert_processing_needs_double_conversion(unsigned int src_pix_fmt, unsigned int dest_pix_fmt)
{
    switch (src_pix_fmt) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
        case V4L2_PIX_FMT_SPCA561:
        case V4L2_PIX_FMT_SN9C10X:
        case V4L2_PIX_FMT_PAC207:
        case V4L2_PIX_FMT_MR97310A:
#if defined(CONFIG_JPEG)
        case V4L2_PIX_FMT_JL2005BCD:
#endif
        case V4L2_PIX_FMT_SN9C2028:
        case V4L2_PIX_FMT_SQ905C:
        case V4L2_PIX_FMT_SBGGR8:
        case V4L2_PIX_FMT_SGBRG8:
        case V4L2_PIX_FMT_SGRBG8:
        case V4L2_PIX_FMT_SRGGB8:
        case V4L2_PIX_FMT_SBGGR10P:
        case V4L2_PIX_FMT_SGBRG10P:
        case V4L2_PIX_FMT_SGRBG10P:
        case V4L2_PIX_FMT_SRGGB10P:
        case V4L2_PIX_FMT_SBGGR10:
        case V4L2_PIX_FMT_SGBRG10:
        case V4L2_PIX_FMT_SGRBG10:
        case V4L2_PIX_FMT_SRGGB10:
        case V4L2_PIX_FMT_SBGGR16:
        case V4L2_PIX_FMT_SGBRG16:
        case V4L2_PIX_FMT_SGRBG16:
        case V4L2_PIX_FMT_SRGGB16:
        case V4L2_PIX_FMT_STV0680:
            return 0;
    }

    switch (dest_pix_fmt) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            return 0;
    }

    return 1;
}

unsigned char *v4lconvert_alloc_buffer(int needed, unsigned char **buf, int *buf_size)
{
    if (*buf_size < needed) {
        free(*buf);
        *buf = malloc(needed);
        if (*buf == NULL) {
            *buf_size = 0;
            return NULL;
        }

        *buf_size = needed;
    }

    return *buf;
}

int v4lconvert_oom_error(struct v4lconvert_data *data)
{
    V4LCONVERT_ERR("could not allocate memory\n");
    errno = ENOMEM;
    return -1;
}

static int v4lconvert_convert_pixfmt(struct v4lconvert_data *data, unsigned char *src, int src_size, unsigned char *dest, int dest_size, struct v4l2_format *fmt, unsigned int dest_pix_fmt)
{
    int result = 0;
    unsigned int width = fmt->fmt.pix.width;
    unsigned int height = fmt->fmt.pix.height;
    unsigned int src_pix_fmt = fmt->fmt.pix.pixelformat;
    unsigned int bytesperline = fmt->fmt.pix.bytesperline;

    switch (src_pix_fmt) {
        case V4L2_PIX_FMT_MJPEG:
        case V4L2_PIX_FMT_JPEG:
#if defined(CONFIG_JPEG)
            if (data->flags & V4LCONVERT_USE_TINYJPEG) {
#endif
                result = v4lconvert_decode_jpeg_tinyjpeg(data, src, src_size, dest, fmt, dest_pix_fmt, 0);
#if defined(CONFIG_JPEG)
            } else {
                result = v4lconvert_decode_jpeg_libjpeg(data, src, src_size, dest, fmt, dest_pix_fmt);
                if ((result == -1) && (errno == EOPNOTSUPP)) {
                    jpeg_destroy_decompress(&data->cinfo);
                    data->cinfo_initialized = 0;
                    data->flags |= V4LCONVERT_USE_TINYJPEG;
                    result = v4lconvert_decode_jpeg_tinyjpeg(data, src, src_size, dest, fmt, dest_pix_fmt, 0);
                }
            }
#endif
            break;

        case V4L2_PIX_FMT_PJPG:
            result = v4lconvert_decode_jpeg_tinyjpeg(data, src, src_size, dest, fmt, dest_pix_fmt, TINYJPEG_FLAGS_PIXART_JPEG);
            break;

        case V4L2_PIX_FMT_JPGL:
            result = v4lconvert_decode_jpgl(src, src_size, dest_pix_fmt, dest, width, height);
            break;

        case V4L2_PIX_FMT_SPCA501:
        case V4L2_PIX_FMT_SPCA505:
        case V4L2_PIX_FMT_SPCA508:
        case V4L2_PIX_FMT_CIT_YYVYUY:
        case V4L2_PIX_FMT_KONICA420:
        case V4L2_PIX_FMT_M420:
        case V4L2_PIX_FMT_SN9C20X_I420:
        case V4L2_PIX_FMT_CPIA1:
        case V4L2_PIX_FMT_OV511:
        case V4L2_PIX_FMT_OV518: {
            int d_size;
            int yvu = 0;
            unsigned char *d;

            if ((dest_pix_fmt != V4L2_PIX_FMT_YUV420) && (dest_pix_fmt != V4L2_PIX_FMT_YVU420)) {
                d = v4lconvert_alloc_buffer(width * height * 3 / 2, &data->convert_pixfmt_buf, &data->convert_pixfmt_buf_size);
                if (!d) {
                    return v4lconvert_oom_error(data);
                }

                d_size = width * height * 3 / 2;
            } else {
                d = dest;
                d_size = dest_size;
            }

            if (dest_pix_fmt == V4L2_PIX_FMT_YVU420) {
                yvu = 1;
            }

            switch (src_pix_fmt) {
                case V4L2_PIX_FMT_SPCA501:
                    v4lconvert_spca501_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_SPCA505:
                    v4lconvert_spca505_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_SPCA508:
                    v4lconvert_spca508_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_CIT_YYVYUY:
                    v4lconvert_cit_yyvyuy_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_KONICA420:
                    v4lconvert_konica_yuv420_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_M420:
                    v4lconvert_m420_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_SN9C20X_I420:
                    v4lconvert_sn9c20x_to_yuv420(src, d, width, height, yvu);
                    break;

                case V4L2_PIX_FMT_CPIA1:
                    if (v4lconvert_cpia1_to_yuv420(data, src, src_size, d, width, height, yvu)) {
                        errno = EAGAIN;
                        return -1;
                    }
                    break;
#ifdef HAVE_LIBV4LCONVERT_HELPERS
                case V4L2_PIX_FMT_OV511:
                    if (v4lconvert_helper_decompress(data, LIBV4LCONVERT_PRIV_DIR "/ov511-decomp", src, src_size, d, d_size, width, height, yvu)) {
                        errno = EAGAIN;
                        return -1;
                    }
                    break;

                case V4L2_PIX_FMT_OV518:
                    if (v4lconvert_helper_decompress(data, LIBV4LCONVERT_PRIV_DIR "/ov518-decomp", src, src_size, d, d_size, width, height, yvu)) {
                        errno = EAGAIN;
                        return -1;
                    }
                    break;
#endif
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_yuv420_to_rgb24(data->convert_pixfmt_buf, dest, width, height, bytesperline, yvu);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_yuv420_to_bgr24(data->convert_pixfmt_buf, dest, width, height, bytesperline, yvu);
                    break;
            }

            break;
        }

        case V4L2_PIX_FMT_NV12_16L16:
            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_nv12_16l16_to_rgb24(src, dest, width, height);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_nv12_16l16_to_bgr24(src, dest, width, height);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_nv12_16l16_to_yuv420(src, dest, width, height, 0);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_nv12_16l16_to_yuv420(src, dest, width, height, 1);
                    break;
            }
            break;

        case V4L2_PIX_FMT_NV12:
            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_nv12_to_rgb24(src, dest, width, height, bytesperline, 0);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_nv12_to_rgb24(src, dest, width, height, bytesperline, 1);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_nv12_to_yuv420(src, dest, width, height, bytesperline, 0);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_nv12_to_yuv420(src, dest, width, height, bytesperline, 1);
                    break;
            }
            break;

        case V4L2_PIX_FMT_SPCA561:
        case V4L2_PIX_FMT_SN9C10X:
        case V4L2_PIX_FMT_PAC207:
        case V4L2_PIX_FMT_MR97310A:
#if defined(CONFIG_JPEG)
        case V4L2_PIX_FMT_JL2005BCD:
#endif
        case V4L2_PIX_FMT_SN9C2028:
        case V4L2_PIX_FMT_SQ905C:
        case V4L2_PIX_FMT_STV0680: {
            unsigned char *tmpbuf;
            struct v4l2_format tmpfmt = *fmt;

            tmpbuf = v4lconvert_alloc_buffer(width * height, &data->convert_pixfmt_buf, &data->convert_pixfmt_buf_size);
            if (!tmpbuf) {
                return v4lconvert_oom_error(data);
            }

            switch (src_pix_fmt) {
                case V4L2_PIX_FMT_SPCA561:
                    v4lconvert_decode_spca561(src, tmpbuf, width, height);
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGBRG8;
                    break;

                case V4L2_PIX_FMT_SN9C10X:
                    v4lconvert_decode_sn9c10x(src, tmpbuf, width, height);
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
                    break;

                case V4L2_PIX_FMT_PAC207:
                    if (v4lconvert_decode_pac207(data, src, src_size, tmpbuf, width, height)) {
                        errno = EAGAIN;
                        return -1;
                    }
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
                    break;

                case V4L2_PIX_FMT_MR97310A:
                    if (v4lconvert_decode_mr97310a(data, src, src_size, tmpbuf, width, height)) {
                        errno = EAGAIN;
                        return -1;
                    }
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
                    break;
#if defined(CONFIG_JPEG)
                case V4L2_PIX_FMT_JL2005BCD:
                    if (v4lconvert_decode_jl2005bcd(data, src, src_size, tmpbuf, width, height)) {
                        errno = EAGAIN;
                        return -1;
                    }
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB8;
                    break;
#endif
                case V4L2_PIX_FMT_SN9C2028:
                    v4lconvert_decode_sn9c2028(src, tmpbuf, width, height);
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
                    break;

                case V4L2_PIX_FMT_SQ905C:
                    v4lconvert_decode_sq905c(src, tmpbuf, width, height);
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB8;
                    break;

                case V4L2_PIX_FMT_STV0680:
                    v4lconvert_decode_stv0680(src, tmpbuf, width, height);
                    tmpfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB8;
                    break;
            }

            tmpfmt.fmt.pix.bytesperline = width;
            tmpfmt.fmt.pix.sizeimage = width * height;
            v4lprocessing_processing(data->processing, tmpbuf, &tmpfmt);
            src_pix_fmt = tmpfmt.fmt.pix.pixelformat;
            src = tmpbuf;
            src_size = width * height;
        }

        case V4L2_PIX_FMT_SBGGR10P:
        case V4L2_PIX_FMT_SGBRG10P:
        case V4L2_PIX_FMT_SGRBG10P:
        case V4L2_PIX_FMT_SRGGB10P: {
            int b10format = 1;

            switch (src_pix_fmt) {
                case V4L2_PIX_FMT_SBGGR10P:
                    src_pix_fmt = V4L2_PIX_FMT_SBGGR8;
                    break;

                case V4L2_PIX_FMT_SGBRG10P:
                    src_pix_fmt = V4L2_PIX_FMT_SGBRG8;
                    break;

                case V4L2_PIX_FMT_SGRBG10P:
                    src_pix_fmt = V4L2_PIX_FMT_SGRBG8;
                    break;

                case V4L2_PIX_FMT_SRGGB10P:
                    src_pix_fmt = V4L2_PIX_FMT_SRGGB8;
                    break;

                default:
                    b10format = 0;
                    break;
            }

            if (b10format) {
                if (src_size < ((width * height * 10)/8)) {
                    V4LCONVERT_ERR("short raw bayer10 data frame\n");
                    errno = EPIPE;
                    result = -1;
                    break;
                }

                v4lconvert_bayer10p_to_bayer8(src, src, width, height);
                bytesperline = width;
            }
        }

        case V4L2_PIX_FMT_SBGGR10:
        case V4L2_PIX_FMT_SGBRG10:
        case V4L2_PIX_FMT_SGRBG10:
        case V4L2_PIX_FMT_SRGGB10: {
            int b10format = 1;

            switch (src_pix_fmt) {
                case V4L2_PIX_FMT_SBGGR10:
                    src_pix_fmt = V4L2_PIX_FMT_SBGGR8;
                    break;

                case V4L2_PIX_FMT_SGBRG10:
                    src_pix_fmt = V4L2_PIX_FMT_SGBRG8;
                    break;

                case V4L2_PIX_FMT_SGRBG10:
                    src_pix_fmt = V4L2_PIX_FMT_SGRBG8;
                    break;

                case V4L2_PIX_FMT_SRGGB10:
                    src_pix_fmt = V4L2_PIX_FMT_SRGGB8;
                    break;

                default:
                    b10format = 0;
                    break;
            }

            if (b10format) {
                if (src_size < (width * height * 2)) {
                    V4LCONVERT_ERR("short raw bayer10 data frame\n");
                    errno = EPIPE;
                    result = -1;
                    break;
                }

                v4lconvert_bayer10_to_bayer8(src, src, width, height);
                bytesperline = width;
            }
        }

        case V4L2_PIX_FMT_SBGGR16:
        case V4L2_PIX_FMT_SGBRG16:
        case V4L2_PIX_FMT_SGRBG16:
        case V4L2_PIX_FMT_SRGGB16: {
            int b16format = 1;

            switch (src_pix_fmt) {
                case V4L2_PIX_FMT_SBGGR16:
                    src_pix_fmt = V4L2_PIX_FMT_SBGGR8;
                    break;

                case V4L2_PIX_FMT_SGBRG16:
                    src_pix_fmt = V4L2_PIX_FMT_SGBRG8;
                    break;

                case V4L2_PIX_FMT_SGRBG16:
                    src_pix_fmt = V4L2_PIX_FMT_SGRBG8;
                    break;

                case V4L2_PIX_FMT_SRGGB16:
                    src_pix_fmt = V4L2_PIX_FMT_SRGGB8;
                    break;

                default:
                    b16format = 0;
                    break;
            }

            if (b16format) {
                if (src_size < ((width * height * 2))) {
                    V4LCONVERT_ERR("short raw bayer16 data frame\n");
                    errno = EPIPE;
                    result = -1;
                    break;
                }

                v4lconvert_bayer16_to_bayer8(src, src, width, height);
                bytesperline = width;
            }
        }

        case V4L2_PIX_FMT_SBGGR8:
        case V4L2_PIX_FMT_SGBRG8:
        case V4L2_PIX_FMT_SGRBG8:
        case V4L2_PIX_FMT_SRGGB8:
            if (src_size < (width * height)) {
                V4LCONVERT_ERR("short raw bayer data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_bayer_to_rgb24(src, dest, width, height, bytesperline, src_pix_fmt);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_bayer_to_bgr24(src, dest, width, height, bytesperline, src_pix_fmt);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_bayer_to_yuv420(src, dest, width, height, bytesperline, src_pix_fmt, 0);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_bayer_to_yuv420(src, dest, width, height, bytesperline, src_pix_fmt, 1);
                    break;
            }
            break;

        case V4L2_PIX_FMT_SE401: {
            unsigned char *d = NULL;

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    d = dest;
                    break;

                case V4L2_PIX_FMT_BGR24:
                case V4L2_PIX_FMT_YUV420:
                case V4L2_PIX_FMT_YVU420:
                    d = v4lconvert_alloc_buffer(width * height * 3, &data->convert_pixfmt_buf, &data->convert_pixfmt_buf_size);
                    if (!d) {
                        return v4lconvert_oom_error(data);
                    }

                    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
                    v4lconvert_fixup_fmt(fmt);
                    break;

                default:
                    V4LCONVERT_ERR("Unknown destination format in conversion\n");
                    errno = EINVAL;
                    return -1;
            }

            result = v4lconvert_se401_to_rgb24(data, src, src_size, d, width, height);
            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_swap_rgb(d, dest, width, height);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_rgb24_to_yuv420(d, dest, fmt, 0, 0, 3);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_rgb24_to_yuv420(d, dest, fmt, 0, 1, 3);
                    break;
            }
            break;
        }

        case V4L2_PIX_FMT_Y16:
        case V4L2_PIX_FMT_Y16_BE:
            if (src_size < (width * height * 2)) {
                V4LCONVERT_ERR("short y16 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    case V4L2_PIX_FMT_BGR24:
                    v4lconvert_y16_to_rgb24(src, dest, width, height, src_pix_fmt == V4L2_PIX_FMT_Y16);
                    break;

                case V4L2_PIX_FMT_YUV420:
                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_y16_to_yuv420(src, dest, fmt, src_pix_fmt == V4L2_PIX_FMT_Y16);
                    break;
            }
            break;

        case V4L2_PIX_FMT_GREY:
        case V4L2_PIX_FMT_Y4:
        case V4L2_PIX_FMT_Y6:
            if (src_size < (width * height)) {
                V4LCONVERT_ERR("short grey data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    case V4L2_PIX_FMT_BGR24:
                    v4lconvert_grey_to_rgb24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_YUV420:
                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_grey_to_yuv420(src, dest, fmt);
                    break;
            }
            break;

        case V4L2_PIX_FMT_Y10BPACK:
            if (src_size < (width * height * 10 / 8)) {
                V4LCONVERT_ERR("short y10b data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    case V4L2_PIX_FMT_BGR24:
                    result = v4lconvert_y10b_to_rgb24(data, src, dest, width, height);
                    break;

                case V4L2_PIX_FMT_YUV420:
                case V4L2_PIX_FMT_YVU420:
                    result = v4lconvert_y10b_to_yuv420(data, src, dest, width, height);
                    break;
            }
            break;

        case V4L2_PIX_FMT_RGB565:
            if (src_size < (width * height * 2)) {
                V4LCONVERT_ERR("short rgb565 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_rgb565_to_rgb24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_rgb565_to_bgr24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_rgb565_to_yuv420(src, dest, fmt, 0);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_rgb565_to_yuv420(src, dest, fmt, 1);
                    break;
            }
            break;

        case V4L2_PIX_FMT_RGB24:
            if (src_size < (width * height * 3)) {
                V4LCONVERT_ERR("short rgb24 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    memcpy(dest, src, width * height * 3);
                    break;
            
                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_swap_rgb(src, dest, width, height);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 0, 3);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 1, 3);
                    break;
            }
            break;

        case V4L2_PIX_FMT_BGR24:
            if (src_size < (width * height * 3)) {
                V4LCONVERT_ERR("short bgr24 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_swap_rgb(src, dest, width, height);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    memcpy(dest, src, width * height * 3);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 1, 0, 3);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 1, 1, 3);
                    break;
            }
            break;

        case V4L2_PIX_FMT_RGB32:
        case V4L2_PIX_FMT_XRGB32:
        case V4L2_PIX_FMT_ARGB32:
            if (src_size < (width * height * 4)) {
                V4LCONVERT_ERR("short rgb32 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            src++;
            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_rgb32_to_rgb24(src, dest, width, height, 0);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_rgb32_to_rgb24(src, dest, width, height, 1);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 0, 4);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 1, 4);
                    break;
            }
            break;

        case V4L2_PIX_FMT_BGR32:
        case V4L2_PIX_FMT_XBGR32:
        case V4L2_PIX_FMT_ABGR32:
            if (src_size < (width * height * 4)) {
                V4LCONVERT_ERR("short bgr32 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_rgb32_to_rgb24(src, dest, width, height, 1);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_rgb32_to_rgb24(src, dest, width, height, 0);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 1, 0, 4);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 1, 1, 4);
                    break;
            }
            break;

        case V4L2_PIX_FMT_YUV420:
            if (src_size < (width * height * 3 / 2)) {
                V4LCONVERT_ERR("short yuv420 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_yuv420_to_rgb24(src, dest, width, height, bytesperline, 0);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_yuv420_to_bgr24(src, dest, width, height, bytesperline, 0);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    memcpy(dest, src, width * height * 3 / 2);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_swap_uv(src, dest, fmt);
                    break;
            }
            break;

        case V4L2_PIX_FMT_YVU420:
            if (src_size < (width * height * 3 / 2)) {
                V4LCONVERT_ERR("short yvu420 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_yuv420_to_rgb24(src, dest, width, height, bytesperline, 1);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_yuv420_to_bgr24(src, dest, width, height, bytesperline, 1);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_swap_uv(src, dest, fmt);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    memcpy(dest, src, width * height * 3 / 2);
                    break;
            }
            break;

        case V4L2_PIX_FMT_NV16: {
            unsigned char *tmpbuf;

            tmpbuf = v4lconvert_alloc_buffer(width * height * 2, &data->convert_pixfmt_buf, &data->convert_pixfmt_buf_size);
            if (!tmpbuf) {
                return v4lconvert_oom_error(data);
            }

            v4lconvert_nv16_to_yuyv(src, tmpbuf, width, height, bytesperline);
            src_pix_fmt = V4L2_PIX_FMT_YUYV;
            src = tmpbuf;
            bytesperline = width * 2;
        }

        case V4L2_PIX_FMT_YUYV:
            if (src_size < (width * height * 2)) {
                V4LCONVERT_ERR("short yuyv data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_yuyv_to_rgb24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_yuyv_to_bgr24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_yuyv_to_yuv420(src, dest, width, height, bytesperline, 0);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_yuyv_to_yuv420(src, dest, width, height, bytesperline, 1);
                    break;
            }
            break;

        case V4L2_PIX_FMT_NV61: {
            unsigned char *tmpbuf;

            tmpbuf = v4lconvert_alloc_buffer(width * height * 2, &data->convert_pixfmt_buf, &data->convert_pixfmt_buf_size);
            if (!tmpbuf) {
                return v4lconvert_oom_error(data);
            }

            v4lconvert_nv16_to_yuyv(src, tmpbuf, width, height, bytesperline);
            src_pix_fmt = V4L2_PIX_FMT_YVYU;
            src = tmpbuf;
            bytesperline = width * 2;
        }

        case V4L2_PIX_FMT_YVYU:
            if (src_size < (width * height * 2)) {
                V4LCONVERT_ERR("short yvyu data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_yvyu_to_rgb24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_yvyu_to_bgr24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_yuyv_to_yuv420(src, dest, width, height, bytesperline, 1);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_yuyv_to_yuv420(src, dest, width, height, bytesperline, 0);
                    break;
            }
            break;

        case V4L2_PIX_FMT_UYVY:
            if (src_size < (width * height * 2)) {
                V4LCONVERT_ERR("short uyvy data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_uyvy_to_rgb24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_uyvy_to_bgr24(src, dest, width, height, bytesperline);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_uyvy_to_yuv420(src, dest, width, height, bytesperline, 0);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_uyvy_to_yuv420(src, dest, width, height, bytesperline, 1);
                    break;
            }
            break;

        case V4L2_PIX_FMT_HSV24:
            if (src_size < (width * height * 3)) {
                V4LCONVERT_ERR("short hsv24 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 0, 24, fmt->fmt.pix.hsv_enc);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 1, 24, fmt->fmt.pix.hsv_enc);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 0, 24, fmt->fmt.pix.hsv_enc);
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 0, 3);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 0, 24, fmt->fmt.pix.hsv_enc);
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 1, 3);
                    break;
            }
            break;

        case V4L2_PIX_FMT_HSV32:
            if (src_size < (width * height * 4)) {
                V4LCONVERT_ERR("short hsv32 data frame\n");
                errno = EPIPE;
                result = -1;
            }

            switch (dest_pix_fmt) {
                case V4L2_PIX_FMT_RGB24:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 0, 32, fmt->fmt.pix.hsv_enc);
                    break;

                case V4L2_PIX_FMT_BGR24:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 1, 32, fmt->fmt.pix.hsv_enc);
                    break;

                case V4L2_PIX_FMT_YUV420:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 0, 32, fmt->fmt.pix.hsv_enc);
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 0, 3);
                    break;

                case V4L2_PIX_FMT_YVU420:
                    v4lconvert_hsv_to_rgb24(src, dest, width, height, 0, 32, fmt->fmt.pix.hsv_enc);
                    v4lconvert_rgb24_to_yuv420(src, dest, fmt, 0, 1, 3);
                    break;
            }
            break;

        default:
            V4LCONVERT_ERR("Unknown src format in conversion\n");
            errno = EINVAL;
            return -1;
    }

    fmt->fmt.pix.pixelformat = dest_pix_fmt;
    v4lconvert_fixup_fmt(fmt);

    return result;
}

int v4lconvert_convert(struct v4lconvert_data *data, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt, unsigned char *src, int src_size, unsigned char *dest, int dest_size)
{
    unsigned char *crop_src = src;
    int rotate90, vflip, hflip, crop;
    int convert1_dest_size = dest_size;
    int convert2_dest_size = dest_size;
    unsigned char *convert1_dest = dest;
    struct v4l2_format my_src_fmt = *src_fmt;
    struct v4l2_format my_dest_fmt = *dest_fmt;
    unsigned char *flip_src = src, *flip_dest = dest;
    unsigned char *convert2_src = src, *convert2_dest = dest;
    unsigned char *rotate90_src = src, *rotate90_dest = dest;
    int res, dest_needed, temp_needed, processing, convert = 0;

    processing = v4lprocessing_pre_processing(data->processing);
    rotate90 = data->control_flags & V4LCONTROL_ROTATED_90_JPEG;
    hflip = v4lcontrol_get_ctrl(data->control, V4LCONTROL_HFLIP);
    vflip = v4lcontrol_get_ctrl(data->control, V4LCONTROL_VFLIP);
    crop = my_dest_fmt.fmt.pix.width != my_src_fmt.fmt.pix.width || my_dest_fmt.fmt.pix.height != my_src_fmt.fmt.pix.height;

    if ((src_fmt->fmt.pix.pixelformat == dest_fmt->fmt.pix.pixelformat
        && !processing && !rotate90 && !hflip && !vflip && !crop)
        || !v4lconvert_supported_dst_format(dest_fmt->fmt.pix.pixelformat))
    {
        int to_copy = MIN(dest_size, src_size);
        memcpy(dest, src, to_copy);
        return to_copy;
    }

    switch (my_dest_fmt.fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            dest_needed = my_dest_fmt.fmt.pix.width * my_dest_fmt.fmt.pix.height * 3;
            temp_needed = my_src_fmt.fmt.pix.width * my_src_fmt.fmt.pix.height * 3;
            break;

        case V4L2_PIX_FMT_YUV420:
        case V4L2_PIX_FMT_YVU420:
            dest_needed = my_dest_fmt.fmt.pix.width * my_dest_fmt.fmt.pix.height * 3 / 2;
            temp_needed = my_src_fmt.fmt.pix.width * my_src_fmt.fmt.pix.height * 3 / 2;
            break;

        default:
            V4LCONVERT_ERR("Unknown dest format in conversion\n");
            errno = EINVAL;
            return -1;
    }

    if (dest_size < dest_needed) {
        V4LCONVERT_ERR("destination buffer too small (%d < %d)\n", dest_size, dest_needed);
        errno = EFAULT;
        return -1;
    }

    if (processing && v4lconvert_processing_needs_double_conversion(my_src_fmt.fmt.pix.pixelformat, my_dest_fmt.fmt.pix.pixelformat)) {
        convert = 2;
    } else if ((my_dest_fmt.fmt.pix.pixelformat != my_src_fmt.fmt.pix.pixelformat) || (!rotate90 && !hflip && !vflip && !crop)) {
        convert = 1;
    }

    if (convert == 2) {
        convert1_dest = v4lconvert_alloc_buffer(my_src_fmt.fmt.pix.width * my_src_fmt.fmt.pix.height * 3, &data->convert1_buf, &data->convert1_buf_size);
        if (!convert1_dest) {
            return v4lconvert_oom_error(data);
        }

        convert1_dest_size = my_src_fmt.fmt.pix.width * my_src_fmt.fmt.pix.height * 3;
        convert2_src = convert1_dest;
    }

    if (convert && (rotate90 || hflip || vflip || crop)) {
        convert2_dest = v4lconvert_alloc_buffer(temp_needed, &data->convert2_buf, &data->convert2_buf_size);
        if (!convert2_dest) {
            return v4lconvert_oom_error(data);
        }

        convert2_dest_size = temp_needed;
        rotate90_src = flip_src = crop_src = convert2_dest;
    }

    if (rotate90 && (hflip || vflip || crop)) {
        rotate90_dest = v4lconvert_alloc_buffer(temp_needed, &data->rotate90_buf, &data->rotate90_buf_size);
        if (!rotate90_dest) {
            return v4lconvert_oom_error(data);
        }

        flip_src = crop_src = rotate90_dest;
    }

    if ((vflip || hflip) && crop) {
        flip_dest = v4lconvert_alloc_buffer(temp_needed, &data->flip_buf, &data->flip_buf_size);
        if (!flip_dest) {
            return v4lconvert_oom_error(data);
        }

        crop_src = flip_dest;
    }

    if (convert == 2) {
        res = v4lconvert_convert_pixfmt(data, src, src_size, convert1_dest, convert1_dest_size, &my_src_fmt, V4L2_PIX_FMT_RGB24);
        if (res) {
            return res;
        }

        src_size = my_src_fmt.fmt.pix.sizeimage;
    }

    if (processing) {
        v4lprocessing_processing(data->processing, convert2_src, &my_src_fmt);
    }

    if (convert) {
        res = v4lconvert_convert_pixfmt(data, convert2_src, src_size, convert2_dest, convert2_dest_size, &my_src_fmt, my_dest_fmt.fmt.pix.pixelformat);
        if (res) {
            return res;
        }

        src_size = my_src_fmt.fmt.pix.sizeimage;
        if (processing) {
            v4lprocessing_processing(data->processing, convert2_dest, &my_src_fmt);
        }
    }

    if (rotate90) {
        v4lconvert_rotate90(rotate90_src, rotate90_dest, &my_src_fmt);
    }

    if (hflip || vflip) {
        v4lconvert_flip(flip_src, flip_dest, &my_src_fmt, hflip, vflip);
    }

    if (crop) {
        v4lconvert_crop(crop_src, dest, &my_src_fmt, &my_dest_fmt);
    }

    return dest_needed;
}

const char *v4lconvert_get_error_message(struct v4lconvert_data *data)
{
    return data->error_msg;
}

static void v4lconvert_get_framesizes(struct v4lconvert_data *data, unsigned int pixelformat, int index)
{
    int i, j, match;
    struct v4l2_frmsizeenum frmsize = { .pixel_format = pixelformat };

    for (i = 0; ; i++) {
        frmsize.index = i;
        if (data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize)) {
            break;
        }

        match = 0;
        for (j = 0; j < data->no_framesizes; j++) {
            if (frmsize.type != data->framesizes[j].type) {
                continue;
            }

            switch (frmsize.type) {
                case V4L2_FRMSIZE_TYPE_DISCRETE:
                    if (!memcmp(&frmsize.discrete, &data->framesizes[j].discrete, sizeof(frmsize.discrete))) {
                        match = 1;
                    }
                    break;

                case V4L2_FRMSIZE_TYPE_CONTINUOUS:
                case V4L2_FRMSIZE_TYPE_STEPWISE:
                    if (!memcmp(&frmsize.stepwise, &data->framesizes[j].stepwise, sizeof(frmsize.stepwise))) {
                        match = 1;
                    }
                    break;
            }

            if (match) {
                break;
            }
        }

        if (!match) {
            if (data->no_framesizes == V4LCONVERT_MAX_FRAMESIZES) {
                fprintf(stderr, "libv4lconvert: warning more framesizes than I can handle!\n");
                return;
            }

            data->framesizes[data->no_framesizes].type = frmsize.type;
            data->framesize_supported_src_formats[data->no_framesizes] = 1ULL << index;

            switch (frmsize.type) {
                case V4L2_FRMSIZE_TYPE_DISCRETE:
                    data->framesizes[data->no_framesizes].discrete = frmsize.discrete;
                    break;
        
                case V4L2_FRMSIZE_TYPE_CONTINUOUS:
                case V4L2_FRMSIZE_TYPE_STEPWISE:
                    data->framesizes[data->no_framesizes].stepwise = frmsize.stepwise;
                    break;
            }

            data->no_framesizes++;
        } else {
            data->framesize_supported_src_formats[j] |= 1ULL << index;
        }
    }
}

int v4lconvert_enum_framesizes(struct v4lconvert_data *data, struct v4l2_frmsizeenum *frmsize)
{
    if (!v4lconvert_supported_dst_format(frmsize->pixel_format)) {
        if (v4lconvert_supported_dst_fmt_only(data)) {
            errno = EINVAL;
            return -1;
        }

        return data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUM_FRAMESIZES, frmsize);
    }

    if (frmsize->index >= data->no_framesizes) {
        errno = EINVAL;
        return -1;
    }

    frmsize->type = data->framesizes[frmsize->index].type;
    memset(frmsize->reserved, 0, sizeof(frmsize->reserved));
    switch (frmsize->type) {
        case V4L2_FRMSIZE_TYPE_DISCRETE:
            frmsize->discrete = data->framesizes[frmsize->index].discrete;
            frmsize->discrete.width &= ~7;
            frmsize->discrete.height &= ~1;
            break;

        case V4L2_FRMSIZE_TYPE_CONTINUOUS:
        case V4L2_FRMSIZE_TYPE_STEPWISE:
            frmsize->stepwise = data->framesizes[frmsize->index].stepwise;
            break;
    }

    return 0;
}

int v4lconvert_enum_frameintervals(struct v4lconvert_data *data, struct v4l2_frmivalenum *frmival)
{
    int res;
    struct v4l2_format src_fmt, dest_fmt;

    if (!v4lconvert_supported_dst_format(frmival->pixel_format)) {
        if (v4lconvert_supported_dst_fmt_only(data)) {
            errno = EINVAL;
            return -1;
        }

        res = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUM_FRAMEINTERVALS, frmival);
        if (res) {
            V4LCONVERT_ERR("%s\n", strerror(errno));
        }

        return res;
    }

    memset(&dest_fmt, 0, sizeof(dest_fmt));
    dest_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    dest_fmt.fmt.pix.pixelformat = frmival->pixel_format;
    dest_fmt.fmt.pix.width = frmival->width;
    dest_fmt.fmt.pix.height = frmival->height;

    res = v4lconvert_try_format(data, &dest_fmt, &src_fmt);
    if (res) {
        V4LCONVERT_ERR("trying format: %s\n", strerror(errno));
        return res;
    }

    if (dest_fmt.fmt.pix.pixelformat != frmival->pixel_format
        || dest_fmt.fmt.pix.width  != frmival->width
        || dest_fmt.fmt.pix.height != frmival->height)
    {
        int frmival_pixformat = frmival->pixel_format;
        int dest_pixformat = dest_fmt.fmt.pix.pixelformat;

        V4LCONVERT_ERR("Could not find matching framesize for: %c%c%c%c %dx%d "
                "closest match: %c%c%c%c %dx%d\n",
                frmival_pixformat & 0xff,
                (frmival_pixformat >> 8) & 0xff,
                (frmival_pixformat >> 16) & 0xff,
                frmival_pixformat >> 24,
                frmival->width, frmival->height,
                dest_pixformat & 0xff,
                (dest_pixformat >> 8) & 0xff,
                (dest_pixformat >> 16) & 0xff,
                dest_pixformat >> 24,
                dest_fmt.fmt.pix.width , dest_fmt.fmt.pix.height);
        errno = EINVAL;
        return -1;
    }

    frmival->pixel_format = src_fmt.fmt.pix.pixelformat;
    frmival->width = src_fmt.fmt.pix.width;
    frmival->height = src_fmt.fmt.pix.height;
    res = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUM_FRAMEINTERVALS, frmival);
    if (res) {
        int dest_pixfmt = dest_fmt.fmt.pix.pixelformat;
        int src_pixfmt  = src_fmt.fmt.pix.pixelformat;

        V4LCONVERT_ERR("Could not enum frameival index: %d for: %c%c%c%c %dx%d "
                "using src: %c%c%c%c %dx%d, error: %s\n",
                frmival->index,
                dest_pixfmt & 0xff,
                (dest_pixfmt >> 8) & 0xff,
                (dest_pixfmt >> 16) & 0xff,
                dest_pixfmt >> 24,
                dest_fmt.fmt.pix.width , dest_fmt.fmt.pix.height,
                src_pixfmt & 0xff,
                (src_pixfmt >> 8) & 0xff,
                (src_pixfmt >> 16) & 0xff,
                src_pixfmt >> 24,
                src_fmt.fmt.pix.width, src_fmt.fmt.pix.height, strerror(errno));
    }

    frmival->pixel_format = dest_fmt.fmt.pix.pixelformat;
    frmival->width = dest_fmt.fmt.pix.width;
    frmival->height = dest_fmt.fmt.pix.height;

    return res;
}

int v4lconvert_vidioc_queryctrl(struct v4lconvert_data *data, void *arg)
{
    return v4lcontrol_vidioc_queryctrl(data->control, arg);
}

int v4lconvert_vidioc_g_ctrl(struct v4lconvert_data *data, void *arg)
{
    return v4lcontrol_vidioc_g_ctrl(data->control, arg);
}

int v4lconvert_vidioc_s_ctrl(struct v4lconvert_data *data, void *arg)
{
    return v4lcontrol_vidioc_s_ctrl(data->control, arg);
}

int v4lconvert_vidioc_g_ext_ctrls(struct v4lconvert_data *data, void *arg)
{
    return v4lcontrol_vidioc_g_ext_ctrls(data->control, arg);
}

int v4lconvert_vidioc_try_ext_ctrls(struct v4lconvert_data *data, void *arg)
{
    return v4lcontrol_vidioc_try_ext_ctrls(data->control, arg);
}

int v4lconvert_vidioc_s_ext_ctrls(struct v4lconvert_data *data, void *arg)
{
    return v4lcontrol_vidioc_s_ext_ctrls(data->control, arg);
}

int v4lconvert_get_fps(struct v4lconvert_data *data)
{
    return data->fps;
}

void v4lconvert_set_fps(struct v4lconvert_data *data, int fps)
{
    data->fps = fps;
}
