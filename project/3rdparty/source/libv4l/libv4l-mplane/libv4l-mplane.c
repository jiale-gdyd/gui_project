#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <libv4l/videodev2.h>
#include <libv4l/libv4l-plugin.h>

#include "../config.h"

#define SYS_IOCTL(fd, cmd, arg)             syscall(SYS_ioctl, (int)(fd), (unsigned long)(cmd), (void *)(arg))
#define SYS_READ(fd, buf, len)              syscall(SYS_read, (int)(fd), (void *)(buf), (size_t)(len));
#define SYS_WRITE(fd, buf, len)             syscall(SYS_write, (int)(fd), (const void *)(buf), (size_t)(len));

#if HAVE_VISIBILITY
#ifndef LIBV4L_PUBLIC
#define PLUGIN_PUBLIC                       __attribute__ ((visibility("default")))
#endif
#else
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC
#endif
#endif

struct mplane_plugin {
    union {
        struct {
            unsigned int mplane_capture : 1;
            unsigned int mplane_output : 1;
        };
        unsigned int     mplane;
    };
};

#define SIMPLE_CONVERT_IOCTL(fd, cmd, arg, __struc)                                                         \
    ({                                                                                                      \
        int __ret;                                                                                          \
        struct __struc *req = arg;                                                                          \
        uint32_t type = req->type;                                                                          \
        if ((type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) || (type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE)) {  \
            errno = EINVAL;                                                                                 \
            __ret = -1;                                                                                     \
        } else {                                                                                            \
            req->type = convert_type(type);                                                                 \
            __ret = SYS_IOCTL(fd, cmd, arg);                                                                \
            req->type = type;                                                                               \
        }                                                                                                   \
        __ret;                                                                                              \
    })

static void *plugin_init(int fd)
{
    int ret;
    struct v4l2_capability cap;
    struct mplane_plugin plugin, *ret_plugin;

    memset(&plugin, 0x00, sizeof(plugin));

    memset(&cap, 0x00, sizeof(cap));
    ret = SYS_IOCTL(fd, VIDIOC_QUERYCAP, &cap);
    if (ret) {
        perror("Failed to query video capabilities");
        return NULL;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) && (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
        plugin.mplane_capture = 1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT) && (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE)) {
        plugin.mplane_output = 1;
    }

    if (!plugin.mplane) {
        return NULL;
    }

    ret_plugin = calloc(1, sizeof(*ret_plugin));
    if (!ret_plugin) {
        perror("Couldn't allocate memory for plugin");
        return NULL;
    }
    *ret_plugin = plugin;

    printf("Using mplane plugin for %s%s\n", plugin.mplane_capture ? "capture " : "", plugin.mplane_output ? "output " : "");
    return ret_plugin;
}

static void plugin_close(void *dev_ops_priv)
{
    if (dev_ops_priv == NULL) {
        return;
    }

    free(dev_ops_priv);
}

static int querycap_ioctl(int fd, unsigned long int cmd, struct v4l2_capability *arg)
{
    int ret;
    struct v4l2_capability *cap = arg;

    ret = SYS_IOCTL(fd, cmd, cap);
    if (ret) {
        return ret;
    }

    if (cap->capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        cap->capabilities |= V4L2_CAP_VIDEO_CAPTURE;
    }

    if (cap->capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE) {
        cap->capabilities |= V4L2_CAP_VIDEO_OUTPUT;
    }

    if (cap->device_caps & V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        cap->device_caps |= V4L2_CAP_VIDEO_CAPTURE;
    }

    if (cap->device_caps & V4L2_CAP_VIDEO_OUTPUT_MPLANE) {
        cap->device_caps |= V4L2_CAP_VIDEO_OUTPUT;
    }

    cap->capabilities |= V4L2_CAP_EXT_PIX_FORMAT;
    cap->device_caps |= V4L2_CAP_EXT_PIX_FORMAT;

    cap->capabilities &= ~(V4L2_CAP_VIDEO_OUTPUT_MPLANE | V4L2_CAP_VIDEO_CAPTURE_MPLANE);
    cap->device_caps &= ~(V4L2_CAP_VIDEO_OUTPUT_MPLANE | V4L2_CAP_VIDEO_CAPTURE_MPLANE);

    return 0;
}

static int convert_type(int type)
{
    switch (type) {
        case V4L2_BUF_TYPE_VIDEO_CAPTURE:
            return V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

        case V4L2_BUF_TYPE_VIDEO_OUTPUT:
            return V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;

        default:
            return type;
    }
}

static void sanitize_format(struct v4l2_format *fmt)
{
    unsigned int offset;

    if (fmt->fmt.pix.priv == V4L2_PIX_FMT_PRIV_MAGIC) {
        return;
    }

    fmt->fmt.pix.priv = V4L2_PIX_FMT_PRIV_MAGIC;

    offset = offsetof(struct v4l2_pix_format, priv) + sizeof(fmt->fmt.pix.priv);
    memset(((char *)&fmt->fmt.pix) + offset, 0, sizeof(fmt->fmt.pix) - offset);
}

static int try_set_fmt_ioctl(int fd, unsigned long int cmd, struct v4l2_format *arg)
{
    int ret;
    struct v4l2_format fmt = {0};
    struct v4l2_format *org = arg;

    switch (arg->type) {
        case V4L2_BUF_TYPE_VIDEO_CAPTURE:
            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
            break;

        case V4L2_BUF_TYPE_VIDEO_OUTPUT:
            fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
            break;

        case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
        case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
            errno = EINVAL;
            return -1;

        default:
            return SYS_IOCTL(fd, cmd, arg);
    }

    sanitize_format(org);

    fmt.fmt.pix_mp.width = org->fmt.pix.width;
    fmt.fmt.pix_mp.height = org->fmt.pix.height;
    fmt.fmt.pix_mp.pixelformat = org->fmt.pix.pixelformat;
    fmt.fmt.pix_mp.field = org->fmt.pix.field;
    fmt.fmt.pix_mp.colorspace = org->fmt.pix.colorspace;
    fmt.fmt.pix_mp.xfer_func = org->fmt.pix.xfer_func;
    fmt.fmt.pix_mp.ycbcr_enc = org->fmt.pix.ycbcr_enc;
    fmt.fmt.pix_mp.quantization = org->fmt.pix.quantization;
    fmt.fmt.pix_mp.num_planes = 1;
    fmt.fmt.pix_mp.flags = org->fmt.pix.flags;
    fmt.fmt.pix_mp.plane_fmt[0].bytesperline = org->fmt.pix.bytesperline;
    fmt.fmt.pix_mp.plane_fmt[0].sizeimage = org->fmt.pix.sizeimage;

    ret = SYS_IOCTL(fd, cmd, &fmt);
    if (ret) {
        return ret;
    }

    org->fmt.pix.width = fmt.fmt.pix_mp.width;
    org->fmt.pix.height = fmt.fmt.pix_mp.height;
    org->fmt.pix.pixelformat = fmt.fmt.pix_mp.pixelformat;
    org->fmt.pix.field = fmt.fmt.pix_mp.field;
    org->fmt.pix.colorspace = fmt.fmt.pix_mp.colorspace;
    org->fmt.pix.xfer_func = fmt.fmt.pix_mp.xfer_func;
    org->fmt.pix.ycbcr_enc = fmt.fmt.pix_mp.ycbcr_enc;
    org->fmt.pix.quantization = fmt.fmt.pix_mp.quantization;
    org->fmt.pix.bytesperline = fmt.fmt.pix_mp.plane_fmt[0].bytesperline;
    org->fmt.pix.sizeimage = fmt.fmt.pix_mp.plane_fmt[0].sizeimage;
    org->fmt.pix.flags = fmt.fmt.pix_mp.flags;

    return 0;
}

static int create_bufs_ioctl(int fd, unsigned long int cmd, struct v4l2_create_buffers *arg)
{
    int ret;
    struct v4l2_create_buffers cbufs = {0};
    struct v4l2_format *org = &arg->format;
    struct v4l2_format *fmt = &cbufs.format;

    switch (arg->format.type) {
        case V4L2_BUF_TYPE_VIDEO_CAPTURE:
            fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
            break;

        case V4L2_BUF_TYPE_VIDEO_OUTPUT:
            fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
            break;

        case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
        case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
            errno = EINVAL;
            return -1;

        default:
            return SYS_IOCTL(fd, cmd, arg);
    }

    cbufs.index = arg->index;
    cbufs.count = arg->count;
    cbufs.memory = arg->memory;
    sanitize_format(org);
    fmt->fmt.pix_mp.width = org->fmt.pix.width;
    fmt->fmt.pix_mp.height = org->fmt.pix.height;
    fmt->fmt.pix_mp.pixelformat = org->fmt.pix.pixelformat;
    fmt->fmt.pix_mp.field = org->fmt.pix.field;
    fmt->fmt.pix_mp.colorspace = org->fmt.pix.colorspace;
    fmt->fmt.pix_mp.xfer_func = org->fmt.pix.xfer_func;
    fmt->fmt.pix_mp.ycbcr_enc = org->fmt.pix.ycbcr_enc;
    fmt->fmt.pix_mp.quantization = org->fmt.pix.quantization;
    fmt->fmt.pix_mp.num_planes = 1;
    fmt->fmt.pix_mp.flags = org->fmt.pix.flags;
    fmt->fmt.pix_mp.plane_fmt[0].bytesperline = org->fmt.pix.bytesperline;
    fmt->fmt.pix_mp.plane_fmt[0].sizeimage = org->fmt.pix.sizeimage;

    ret = SYS_IOCTL(fd, cmd, &cbufs);

    arg->index = cbufs.index;
    arg->count = cbufs.count;
    org->fmt.pix.width = fmt->fmt.pix_mp.width;
    org->fmt.pix.height = fmt->fmt.pix_mp.height;
    org->fmt.pix.pixelformat = fmt->fmt.pix_mp.pixelformat;
    org->fmt.pix.field = fmt->fmt.pix_mp.field;
    org->fmt.pix.colorspace = fmt->fmt.pix_mp.colorspace;
    org->fmt.pix.xfer_func = fmt->fmt.pix_mp.xfer_func;
    org->fmt.pix.ycbcr_enc = fmt->fmt.pix_mp.ycbcr_enc;
    org->fmt.pix.quantization = fmt->fmt.pix_mp.quantization;
    org->fmt.pix.bytesperline = fmt->fmt.pix_mp.plane_fmt[0].bytesperline;
    org->fmt.pix.sizeimage = fmt->fmt.pix_mp.plane_fmt[0].sizeimage;
    org->fmt.pix.flags = fmt->fmt.pix_mp.flags;

    return ret;
}

static int get_fmt_ioctl(int fd, unsigned long int cmd, struct v4l2_format *arg)
{
    int ret;
    struct v4l2_format fmt = {0};
    struct v4l2_format *org = arg;

    switch (arg->type) {
        case V4L2_BUF_TYPE_VIDEO_CAPTURE:
            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
            break;

        case V4L2_BUF_TYPE_VIDEO_OUTPUT:
            fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
            break;

        case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
        case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
            errno = EINVAL;
            return -1;

        default:
            return SYS_IOCTL(fd, cmd, arg);
    }

    ret = SYS_IOCTL(fd, cmd, &fmt);
    if (ret) {
        return ret;
    }

    memset(&org->fmt.pix, 0, sizeof(org->fmt.pix));
    org->fmt.pix.width = fmt.fmt.pix_mp.width;
    org->fmt.pix.height = fmt.fmt.pix_mp.height;
    org->fmt.pix.pixelformat = fmt.fmt.pix_mp.pixelformat;
    org->fmt.pix.field = fmt.fmt.pix_mp.field;
    org->fmt.pix.colorspace = fmt.fmt.pix_mp.colorspace;
    org->fmt.pix.xfer_func = fmt.fmt.pix_mp.xfer_func;
    org->fmt.pix.ycbcr_enc = fmt.fmt.pix_mp.ycbcr_enc;
    org->fmt.pix.quantization = fmt.fmt.pix_mp.quantization;
    org->fmt.pix.bytesperline = fmt.fmt.pix_mp.plane_fmt[0].bytesperline;
    org->fmt.pix.sizeimage = fmt.fmt.pix_mp.plane_fmt[0].sizeimage;
    org->fmt.pix.priv = V4L2_PIX_FMT_PRIV_MAGIC;
    org->fmt.pix.flags = fmt.fmt.pix_mp.flags;

    if (fmt.fmt.pix_mp.num_planes > 1) {
        errno = EINVAL;
        return -1;
    }

    return ret;
}

static int buf_ioctl(int fd, unsigned long int cmd, struct v4l2_buffer *arg)
{
    int ret;
    struct v4l2_buffer buf = *arg;
    struct v4l2_plane plane = { 0 };

    if ((arg->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) || (arg->type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE)) {
        errno = EINVAL;
        return -1;
    }

    buf.type = convert_type(arg->type);
    if (buf.type == arg->type) {
        return SYS_IOCTL(fd, cmd, &buf);
    }

    memcpy(&plane.m, &arg->m, sizeof(plane.m));
    plane.length = arg->length;
    plane.bytesused = arg->bytesused;
    
    buf.m.planes = &plane;
    buf.length = 1;

    ret = SYS_IOCTL(fd, cmd, &buf);

    arg->index = buf.index;
    arg->memory = buf.memory;
    arg->flags = buf.flags;
    arg->field = buf.field;
    arg->timestamp = buf.timestamp;
    arg->timecode = buf.timecode;
    arg->sequence = buf.sequence;

    arg->length = plane.length;
    arg->bytesused = plane.bytesused;
    memcpy(&arg->m, &plane.m, sizeof(arg->m));

    return ret;
}

static int plugin_ioctl(void *dev_ops_priv, int fd, unsigned long int cmd, void *arg)
{
    switch (cmd) {
    case VIDIOC_QUERYCAP:
        return querycap_ioctl(fd, cmd, arg);

    case VIDIOC_TRY_FMT:
    case VIDIOC_S_FMT:
        return try_set_fmt_ioctl(fd, cmd, arg);

    case VIDIOC_G_FMT:
        return get_fmt_ioctl(fd, cmd, arg);

    case VIDIOC_ENUM_FMT:
        return SIMPLE_CONVERT_IOCTL(fd, cmd, arg, v4l2_fmtdesc);

    case VIDIOC_S_PARM:
    case VIDIOC_G_PARM:
        return SIMPLE_CONVERT_IOCTL(fd, cmd, arg, v4l2_streamparm);

    case VIDIOC_QBUF:
    case VIDIOC_DQBUF:
    case VIDIOC_QUERYBUF:
    case VIDIOC_PREPARE_BUF:
        return buf_ioctl(fd, cmd, arg);

    case VIDIOC_CREATE_BUFS:
        return create_bufs_ioctl(fd, cmd, arg);

    case VIDIOC_REQBUFS:
        return SIMPLE_CONVERT_IOCTL(fd, cmd, arg, v4l2_requestbuffers);

    case VIDIOC_STREAMON:
    case VIDIOC_STREAMOFF: {
        int type = *(int *)arg;
        if ((type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) || (type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE)) {
            errno = EINVAL;
            return -1;
        }

        type = convert_type(type);
        return SYS_IOCTL(fd, cmd, &type);
    }

    default:
        return SYS_IOCTL(fd, cmd, arg);
    }
}

static ssize_t plugin_read(void *dev_ops_priv, int fd, void *buf, size_t len)
{
    return SYS_READ(fd, buf, len);
}

static ssize_t plugin_write(void *dev_ops_priv, int fd, const void *buf, size_t len)
{
    return SYS_WRITE(fd, buf, len);
}

PLUGIN_PUBLIC const struct libv4l_dev_ops libv4l2_plugin = {
    .init  = &plugin_init,
    .close = &plugin_close,
    .ioctl = &plugin_ioctl,
    .read  = &plugin_read,
    .write = &plugin_write,
};
