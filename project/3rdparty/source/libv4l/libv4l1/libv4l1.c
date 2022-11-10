#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <libv4l/libv4l1.h>
#include <libv4l/libv4l2.h>
#include <libv4l/videodev2.h>

#include "../config.h"
#include "libv4l1-priv.h"
#include "../libv4lconvert/libv4lsyscall-priv.h"

#define V4L1_SUPPORTS_ENUMINPUT         0x01
#define V4L1_SUPPORTS_ENUMSTD           0x02
#define V4L1_PIX_FMT_TOUCHED            0x04
#define V4L1_PIX_SIZE_TOUCHED           0x08

static int devices_used;
static pthread_mutex_t v4l1_open_mutex = PTHREAD_MUTEX_INITIALIZER;

static struct v4l1_dev_info devices[V4L1_MAX_DEVICES] = {
    { .fd = -1 },
    { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 },
    { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 },
    { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }
};

static unsigned int palette_to_pixelformat(unsigned int palette)
{
    switch (palette) {
        case VIDEO_PALETTE_GREY:
            return V4L2_PIX_FMT_GREY;

        case VIDEO_PALETTE_RGB555:
            return V4L2_PIX_FMT_RGB555;

        case VIDEO_PALETTE_RGB565:
            return V4L2_PIX_FMT_RGB565;

        case VIDEO_PALETTE_RGB24:
            return V4L2_PIX_FMT_BGR24;

        case VIDEO_PALETTE_RGB32:
            return V4L2_PIX_FMT_BGR32;

        case VIDEO_PALETTE_YUYV:
            return V4L2_PIX_FMT_YUYV;

        case VIDEO_PALETTE_YUV422:
            return V4L2_PIX_FMT_YUYV;

        case VIDEO_PALETTE_UYVY:
            return V4L2_PIX_FMT_UYVY;

        case VIDEO_PALETTE_YUV410P:
            return V4L2_PIX_FMT_YUV410;

        case VIDEO_PALETTE_YUV420:
        case VIDEO_PALETTE_YUV420P:
            return V4L2_PIX_FMT_YUV420;

        case VIDEO_PALETTE_YUV411P:
            return V4L2_PIX_FMT_YUV411P;

        case VIDEO_PALETTE_YUV422P:
            return V4L2_PIX_FMT_YUV422P;
    }

    return 0;
}

static unsigned int pixelformat_to_palette(unsigned int pixelformat)
{
    switch (pixelformat) {
        case V4L2_PIX_FMT_GREY:
            return VIDEO_PALETTE_GREY;

        case V4L2_PIX_FMT_RGB555:
            return VIDEO_PALETTE_RGB555;

        case V4L2_PIX_FMT_RGB565:
            return VIDEO_PALETTE_RGB565;

        case V4L2_PIX_FMT_BGR24:
            return VIDEO_PALETTE_RGB24;

        case V4L2_PIX_FMT_BGR32:
            return VIDEO_PALETTE_RGB32;

        case V4L2_PIX_FMT_YUYV:
            return VIDEO_PALETTE_YUYV;

        case V4L2_PIX_FMT_UYVY:
            return VIDEO_PALETTE_UYVY;

        case V4L2_PIX_FMT_YUV410:
        case V4L2_PIX_FMT_YUV420:
            return VIDEO_PALETTE_YUV420P;

        case V4L2_PIX_FMT_YUV411P:
            return VIDEO_PALETTE_YUV411P;

        case V4L2_PIX_FMT_YUV422P:
            return VIDEO_PALETTE_YUV422P;
    }

    return 0;
}

static int count_inputs(int fd)
{
    int i;
    struct v4l2_input input2;

    for (i = 0;; i++) {
        memset(&input2, 0, sizeof(input2));
        input2.index = i;
        if (0 != v4l2_ioctl(fd, VIDIOC_ENUMINPUT, &input2)) {
            break;
        }
    }

    return i;
}

static int v4l1_set_format(int index, unsigned int width, unsigned int height, int v4l1_pal, int width_height_may_differ)
{
    int result;
    unsigned int v4l2_pixfmt;
    struct v4l2_format fmt2 = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };

    if (v4l1_pal != -1) {
        v4l2_pixfmt = palette_to_pixelformat(v4l1_pal);
        if (!v4l2_pixfmt) {
            V4L1_LOG("Unknown v4l1 palette number: %d\n", v4l1_pal);
            errno = EINVAL;
            return -1;
        }
    } else {
        v4l2_pixfmt = devices[index].v4l2_pixfmt;
        v4l1_pal = devices[index].v4l1_pal;
    }

    if ((width == devices[index].width) && (height == devices[index].height) && (v4l2_pixfmt == devices[index].v4l2_pixfmt)) {
        devices[index].v4l1_pal = v4l1_pal;
        return 0;
    }

    result = v4l2_ioctl(devices[index].fd, VIDIOC_G_FMT, &fmt2);
    if (result) {
        int saved_err = errno;

        V4L1_LOG_ERR("getting pixformat: %s\n", strerror(errno));
        errno = saved_err;
        return result;
    }

    fmt2.fmt.pix.pixelformat = v4l2_pixfmt;
    fmt2.fmt.pix.width  = width;
    fmt2.fmt.pix.height = height;
    result = v4l2_ioctl(devices[index].fd, VIDIOC_TRY_FMT, &fmt2);
    if (result) {
        int saved_err = errno;

        V4L1_LOG("error trying pixformat: %s\n", strerror(errno));
        errno = saved_err;
        return result;
    }

    if ((fmt2.fmt.pix.pixelformat != v4l2_pixfmt) || (!width_height_may_differ && ((fmt2.fmt.pix.width != width) || (fmt2.fmt.pix.height != height)))) {
        V4L1_LOG("requested fmt, width, height combo not available\n");
        errno = EINVAL;
        return -1;
    }

    if ((fmt2.fmt.pix.width  == devices[index].width) && (fmt2.fmt.pix.height == devices[index].height) && (fmt2.fmt.pix.pixelformat == devices[index].v4l2_pixfmt)) {
        devices[index].v4l1_pal = v4l1_pal;
        return 0;
    }

    result = v4l2_ioctl(devices[index].fd, VIDIOC_S_FMT, &fmt2);
    if (result) {
        int saved_err = errno;

        V4L1_LOG_ERR("setting pixformat: %s\n", strerror(errno));
        errno = saved_err;
        return result;
    }

    devices[index].width  = fmt2.fmt.pix.width;
    devices[index].height = fmt2.fmt.pix.height;
    devices[index].v4l2_pixfmt = v4l2_pixfmt;
    devices[index].v4l1_pal = v4l1_pal;
    devices[index].depth = ((fmt2.fmt.pix.bytesperline << 3) + (fmt2.fmt.pix.width - 1)) / fmt2.fmt.pix.width;

    return result;
}

static void v4l1_find_min_and_max_size(int index, struct v4l2_format *fmt2)
{
    int i;
    struct v4l2_fmtdesc fmtdesc2 = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };

    devices[index].min_width = -1;
    devices[index].min_height = -1;
    devices[index].max_width = 0;
    devices[index].max_height = 0;

    for (i = 0; ; i++) {
        fmtdesc2.index = i;

        if (v4l2_ioctl(devices[index].fd, VIDIOC_ENUM_FMT, &fmtdesc2)) {
            break;
        }

        fmt2->fmt.pix.pixelformat = fmtdesc2.pixelformat;
        fmt2->fmt.pix.width = 48;
        fmt2->fmt.pix.height = 32;

        if (v4l2_ioctl(devices[index].fd, VIDIOC_TRY_FMT, fmt2) == 0) {
            if (fmt2->fmt.pix.width < devices[index].min_width) {
                devices[index].min_width = fmt2->fmt.pix.width;
            }

            if (fmt2->fmt.pix.height < devices[index].min_height) {
                devices[index].min_height = fmt2->fmt.pix.height;
            }
        }

        fmt2->fmt.pix.pixelformat = fmtdesc2.pixelformat;
        fmt2->fmt.pix.width = 100000;
        fmt2->fmt.pix.height = 100000;

        if (v4l2_ioctl(devices[index].fd, VIDIOC_TRY_FMT, fmt2) == 0) {
            if (fmt2->fmt.pix.width > devices[index].max_width) {
                devices[index].max_width = fmt2->fmt.pix.width;
            }

            if (fmt2->fmt.pix.height > devices[index].max_height) {
                devices[index].max_height = fmt2->fmt.pix.height;
            }
        }
    }
}

int v4l1_open(const char *file, int oflag, ...)
{
    char *lfname;
    int index, fd;
    int v4l_device = 0;
    struct v4l2_format fmt2;
    struct v4l2_input input2;
    struct v4l2_capability cap2;
    struct v4l2_standard standard2;

    if (!strncmp(file, "/dev/video", 10) || !strncmp(file, "/dev/v4l/", 9)) {
        oflag = (oflag & ~O_ACCMODE) | O_RDWR;
        v4l_device = 1;
    }

    if (oflag & O_CREAT) {
        va_list ap;
        mode_t mode;

        va_start(ap, oflag);
        mode = va_arg(ap, PROMOTED_MODE_T);
        fd = SYS_OPEN(file, oflag, mode);
        va_end(ap);
    } else {
        fd = SYS_OPEN(file, oflag, 0);
    }

    if ((fd == -1) || !v4l_device) {
        return fd;
    }

    if (SYS_IOCTL(fd, VIDIOC_QUERYCAP, &cap2)) {
        return fd;
    }

    if (!v4l1_log_file) {
        lfname = getenv("LIBV4L1_LOG_FILENAME");
        if (lfname) {
            v4l1_log_file = fopen(lfname, "w");
        }
    }

    if (!v4l2_log_file) {
        v4l2_log_file = v4l1_log_file;
    }

    if (v4l2_fd_open(fd, 0) == -1) {
        int saved_err = errno;

        SYS_CLOSE(fd);
        errno = saved_err;
        return -1;
    }

    fmt2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(fd, VIDIOC_G_FMT, &fmt2)) {
        int saved_err = errno;

        SYS_CLOSE(fd);
        errno = saved_err;
        return -1;
    }

    pthread_mutex_lock(&v4l1_open_mutex);
    for (index = 0; index < V4L1_MAX_DEVICES; index++) {
        if (devices[index].fd == -1) {
            devices[index].fd = fd;
            break;
        }
    }
    pthread_mutex_unlock(&v4l1_open_mutex);

    if (index == V4L1_MAX_DEVICES) {
        V4L1_LOG_ERR("attempting to open more than %d video devices\n", V4L1_MAX_DEVICES);

        v4l2_close(fd);
        errno = EBUSY;
        return -1;
    }

    if (index >= devices_used) {
        devices_used = index + 1;
    }

    devices[index].flags = 0;
    devices[index].open_count = 1;
    devices[index].v4l1_frame_buf_map_count = 0;
    devices[index].v4l1_frame_pointer = MAP_FAILED;
    devices[index].width  = fmt2.fmt.pix.width;
    devices[index].height = fmt2.fmt.pix.height;
    devices[index].v4l2_pixfmt = fmt2.fmt.pix.pixelformat;
    devices[index].v4l1_pal = pixelformat_to_palette(fmt2.fmt.pix.pixelformat);
    devices[index].depth = ((fmt2.fmt.pix.bytesperline << 3) + (fmt2.fmt.pix.width - 1)) / fmt2.fmt.pix.width;

    v4l1_find_min_and_max_size(index, &fmt2);

    input2.index = 0;
    if (v4l2_ioctl(fd, VIDIOC_ENUMINPUT, &input2) == 0) {
        devices[index].flags |= V4L1_SUPPORTS_ENUMINPUT;
    }

    standard2.index = 0;
    if (v4l2_ioctl(fd, VIDIOC_ENUMSTD, &standard2) == 0) {
        devices[index].flags |= V4L1_SUPPORTS_ENUMSTD;
    }

    V4L1_LOG("open: %d\n", fd);
    return fd;
}

static int v4l1_get_index(int fd)
{
    int index;

    if (fd == -1) {
        return -1;
    }

    for (index = 0; index < devices_used; index++) {
        if (devices[index].fd == fd) {
            break;
        }
    }

    if (index == devices_used) {
        return -1;
    }

    return index;
}

int v4l1_close(int fd)
{
    int index, result;

    index = v4l1_get_index(fd);
    if (index == -1) {
        return SYS_CLOSE(fd);
    }

    pthread_mutex_lock(&devices[index].stream_lock);
    devices[index].open_count--;
    result = devices[index].open_count != 0;
    pthread_mutex_unlock(&devices[index].stream_lock);

    if (result) {
        return v4l2_close(fd);
    }

    if (devices[index].v4l1_frame_pointer != MAP_FAILED) {
        if (devices[index].v4l1_frame_buf_map_count) {
            V4L1_LOG("v4l1 capture buffer still mapped: %d times on close()\n", devices[index].v4l1_frame_buf_map_count);
        } else {
            SYS_MUNMAP(devices[index].v4l1_frame_pointer, V4L1_NO_FRAMES * V4L1_FRAME_BUF_SIZE);
        }

        devices[index].v4l1_frame_pointer = MAP_FAILED;
    }

    devices[index].fd = -1;
    result = v4l2_close(fd);
    V4L1_LOG("close: %d\n", fd);

    return result;
}

int v4l1_dup(int fd)
{
    int index = v4l1_get_index(fd);
    if (index == -1) {
        return syscall(SYS_dup, fd);
    }

    devices[index].open_count++;

    return v4l2_dup(fd);
}

int v4l1_ioctl(int fd, unsigned long int request, ...)
{
    void *arg;
    va_list ap;
    int result, index, saved_err, stream_locked = 0;

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    index = v4l1_get_index(fd);
    if (index == -1) {
        return SYS_IOCTL(fd, request, arg);
    }

    request = (unsigned int)request;
    switch (request) {
        case VIDIOCSPICT:
        case VIDIOCGPICT:
        case VIDIOCSWIN:
        case VIDIOCGWIN:
        case VIDIOCGMBUF:
        case VIDIOCMCAPTURE:
        case VIDIOCSYNC:
        case VIDIOC_S_FMT:
            pthread_mutex_lock(&devices[index].stream_lock);
            stream_locked = 1;
    }

    switch (request) {
        case VIDIOCGCAP: {
            struct video_capability *cap = arg;
            struct v4l2_framebuffer fbuf = { 0, };
            struct v4l2_capability cap2 = { { 0 }, };

            result = v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap2);
            if (result < 0) {
                break;
            }

            if (cap2.capabilities & V4L2_CAP_DEVICE_CAPS) {
                cap2.capabilities = cap2.device_caps;
            }

            if (cap2.capabilities & V4L2_CAP_VIDEO_OVERLAY) {
                result = v4l2_ioctl(fd, VIDIOC_G_FBUF, &fbuf);
                if (result < 0) {
                    memset(&fbuf, 0, sizeof(fbuf));
                }

                result = 0;
            }

            memcpy(cap->name, cap2.card, min(sizeof(cap->name), sizeof(cap2.card)));

            cap->name[sizeof(cap->name) - 1] = 0;

            if (cap2.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
                cap->type |= VID_TYPE_CAPTURE;
            }

            if (cap2.capabilities & V4L2_CAP_TUNER) {
                cap->type |= VID_TYPE_TUNER;
            }

            if (cap2.capabilities & V4L2_CAP_VBI_CAPTURE) {
                cap->type |= VID_TYPE_TELETEXT;
            }

            if (cap2.capabilities & V4L2_CAP_VIDEO_OVERLAY) {
                cap->type |= VID_TYPE_OVERLAY;
            }

            if (fbuf.capability & V4L2_FBUF_CAP_LIST_CLIPPING) {
                cap->type |= VID_TYPE_CLIPPING;
            }

            cap->channels  = count_inputs(fd);
            cap->minwidth  = devices[index].min_width;
            cap->minheight = devices[index].min_height;
            cap->maxwidth  = devices[index].max_width;
            cap->maxheight = devices[index].max_height;
            break;
        }

        case VIDIOCSPICT: {
            struct video_picture *pic = arg;

            devices[index].flags |= V4L1_PIX_FMT_TOUCHED;

            v4l2_set_control(fd, V4L2_CID_BRIGHTNESS, pic->brightness);
            v4l2_set_control(fd, V4L2_CID_HUE, pic->hue);
            v4l2_set_control(fd, V4L2_CID_CONTRAST, pic->contrast);
            v4l2_set_control(fd, V4L2_CID_SATURATION, pic->colour);
            v4l2_set_control(fd, V4L2_CID_WHITENESS, pic->whiteness);

            result = v4l1_set_format(index, devices[index].width, devices[index].height, pic->palette, 0);
            break;
        }

        case VIDIOCGPICT: {
            int i;
            struct video_picture *pic = arg;

            if (!(devices[index].flags & V4L1_PIX_FMT_TOUCHED) && !pixelformat_to_palette(devices[index].v4l2_pixfmt)) {
                v4l1_set_format(index, devices[index].width, devices[index].height, VIDEO_PALETTE_RGB24, (devices[index].flags & V4L1_PIX_SIZE_TOUCHED) ? 0 : 1);
            }
            devices[index].flags |= V4L1_PIX_FMT_TOUCHED;

            memset(pic, 0, sizeof(*pic));
            pic->depth = devices[index].depth;
            pic->palette = devices[index].v4l1_pal;

            i = v4l2_get_control(devices[index].fd, V4L2_CID_HUE);
            if (i >= 0) {
                pic->hue = i;
            }

            i = v4l2_get_control(devices[index].fd, V4L2_CID_SATURATION);
            if (i >= 0) {
                pic->colour = i;
            }

            i = v4l2_get_control(devices[index].fd, V4L2_CID_CONTRAST);
            if (i >= 0) {
                pic->contrast = i;
            }

            i = v4l2_get_control(devices[index].fd, V4L2_CID_WHITENESS);
            if (i >= 0) {
                pic->whiteness = i;
            }

            i = v4l2_get_control(devices[index].fd, V4L2_CID_BRIGHTNESS);
            if (i >= 0) {
                pic->brightness = i;
            }
            result = 0;
            break;
        }

        case VIDIOCSWIN:
        case VIDIOCGWIN: {
            struct video_window *win = arg;

            devices[index].flags |= V4L1_PIX_SIZE_TOUCHED;
            if (request == VIDIOCSWIN) {
                result = v4l1_set_format(index, win->width, win->height, -1, 1);
            } else {
                result = 0;
            }

            if (result == 0) {
                win->x = 0;
                win->y = 0;
                win->width  = devices[index].width;
                win->height = devices[index].height;
                win->flags = 0;
            }
            break;
        }

        case VIDIOCGCHAN: {
            struct video_channel *chan = arg;

            chan->tuners = 0;
            chan->flags = 0;
            chan->type = VIDEO_TYPE_CAMERA;
            chan->norm = 0;

            if (devices[index].flags & V4L1_SUPPORTS_ENUMINPUT) {
                struct v4l2_input input2 = { .index = chan->channel };

                result = v4l2_ioctl(fd, VIDIOC_ENUMINPUT, &input2);
                if (result < 0) {
                    break;
                }

                snprintf(chan->name, sizeof(chan->name), "%s", (char *)input2.name);
                if (input2.type == V4L2_INPUT_TYPE_TUNER) {
                    chan->tuners = 1;
                    chan->type = VIDEO_TYPE_TV;
                    chan->flags = VIDEO_VC_TUNER;
                }
            } else {
                if (chan->channel == 0) {
                    snprintf(chan->name, sizeof(chan->name), "Camera");
                    result = 0;
                } else {
                    errno  = EINVAL;
                    result = -1;
                    break;
                }
            }

            if (devices[index].flags & V4L1_SUPPORTS_ENUMSTD) {
                v4l2_std_id sid;

                result = v4l2_ioctl(fd, VIDIOC_G_STD, &sid);
                if (result < 0) {
                    break;
                }

                if (sid & V4L2_STD_PAL) {
                    chan->norm = VIDEO_MODE_PAL;
                }

                if (sid & V4L2_STD_NTSC) {
                    chan->norm = VIDEO_MODE_NTSC;
                }

                if (sid & V4L2_STD_SECAM) {
                    chan->norm = VIDEO_MODE_SECAM;
                }

                if (sid == V4L2_STD_ALL) {
                    chan->norm = VIDEO_MODE_AUTO;
                }
            }
            break;
        }

        case VIDIOCSCHAN: {
            struct video_channel *chan = arg;

            if (devices[index].flags & V4L1_SUPPORTS_ENUMINPUT) {
                result = v4l2_ioctl(fd, VIDIOC_S_INPUT, &chan->channel);
                if (result < 0) {
                    break;
                }
            } else {
                if (chan->channel != 0) {
                    errno  = EINVAL;
                    result = -1;
                    break;
                }

                result = 0;
            }

            if (devices[index].flags & V4L1_SUPPORTS_ENUMSTD) {
                v4l2_std_id sid = 0;

                switch (chan->norm) {
                    case VIDEO_MODE_PAL:
                        sid = V4L2_STD_PAL;
                        break;

                    case VIDEO_MODE_NTSC:
                        sid = V4L2_STD_NTSC;
                        break;

                    case VIDEO_MODE_SECAM:
                        sid = V4L2_STD_SECAM;
                        break;

                    case VIDEO_MODE_AUTO:
                        sid = V4L2_STD_ALL;
                        break;
                }

                if (sid) {
                    result = v4l2_ioctl(fd, VIDIOC_S_STD, &sid);
                }
            }
            break;
        }

        case VIDIOCGMBUF: {
            int i;
            struct video_mbuf *mbuf = arg;

            mbuf->size = V4L1_NO_FRAMES * V4L1_FRAME_BUF_SIZE;
            mbuf->frames = V4L1_NO_FRAMES;
            for (i = 0; i < mbuf->frames; i++) {
                mbuf->offsets[i] = i * V4L1_FRAME_BUF_SIZE;
            }

            if (devices[index].v4l1_frame_pointer == MAP_FAILED) {
                devices[index].v4l1_frame_pointer = (void *)SYS_MMAP(NULL, (size_t)mbuf->size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
                if (devices[index].v4l1_frame_pointer == MAP_FAILED) {
                    saved_err = errno;
                    V4L1_LOG_ERR("allocating v4l1 buffer: %s\n", strerror(errno));
                    errno = saved_err;
                    result = -1;
                    break;
                }

                V4L1_LOG("allocated v4l1 buffer @ %p\n", devices[index].v4l1_frame_pointer);
            }

            result = 0;
            break;
        }

        case VIDIOCMCAPTURE: {
            struct video_mmap *map = arg;

            devices[index].flags |= V4L1_PIX_FMT_TOUCHED | V4L1_PIX_SIZE_TOUCHED;
            result = v4l1_set_format(index, map->width, map->height, map->format, 0);
            break;
        }

        case VIDIOCSYNC: {
            int *frame_index = arg;

            if ((devices[index].v4l1_frame_pointer == MAP_FAILED) || (*frame_index < 0) || (*frame_index >= V4L1_NO_FRAMES)) {
                errno = EINVAL;
                result = -1;
                break;
            }

            result = v4l2_read(devices[index].fd, devices[index].v4l1_frame_pointer + *frame_index * V4L1_FRAME_BUF_SIZE, V4L1_FRAME_BUF_SIZE);
            result = (result > 0) ? 0 : result;
            break;
        }

        case VIDIOC_S_FMT: {
            struct v4l2_format *fmt2 = arg;

            result = v4l2_ioctl(fd, request, arg);

            if (result == 0 && fmt2->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
                if (devices[index].v4l2_pixfmt != fmt2->fmt.pix.pixelformat) {
                    devices[index].v4l2_pixfmt = fmt2->fmt.pix.pixelformat;
                    devices[index].v4l1_pal = pixelformat_to_palette(fmt2->fmt.pix.pixelformat);
                }

                devices[index].width  = fmt2->fmt.pix.width;
                devices[index].height = fmt2->fmt.pix.height;
            }
            break;
        }

        case VIDIOCGFBUF: {
            struct video_buffer *buffer = arg;
            struct v4l2_framebuffer fbuf = { 0, };

            result = v4l2_ioctl(fd, VIDIOC_G_FBUF, &fbuf);
            if (result < 0) {
                break;
            }

            buffer->base = fbuf.base;
            buffer->height = fbuf.fmt.height;
            buffer->width = fbuf.fmt.width;

            switch (fbuf.fmt.pixelformat) {
                case V4L2_PIX_FMT_RGB332:
                    buffer->depth = 8;
                    break;

                case V4L2_PIX_FMT_RGB555:
                    buffer->depth = 15;
                    break;

                case V4L2_PIX_FMT_RGB565:
                    buffer->depth = 16;
                    break;

                case V4L2_PIX_FMT_BGR24:
                    buffer->depth = 24;
                    break;

                case V4L2_PIX_FMT_BGR32:
                    buffer->depth = 32;
                    break;

                default:
                    buffer->depth = 0;
            }

            if (fbuf.fmt.bytesperline) {
                buffer->bytesperline = fbuf.fmt.bytesperline;
                if (!buffer->depth && buffer->width) {
                    buffer->depth = ((fbuf.fmt.bytesperline << 3) + (buffer->width - 1)) / buffer->width;
                }
            } else {
                buffer->bytesperline = (buffer->width * buffer->depth + 7) & 7;
                buffer->bytesperline >>= 3;
            }
            break;
        }

        case VIDIOCSFBUF: {
            struct video_buffer *buffer = arg;
            struct v4l2_framebuffer fbuf = { 0, };

            fbuf.base = buffer->base;
            fbuf.fmt.height = buffer->height;
            fbuf.fmt.width = buffer->width;

            switch (buffer->depth) {
                case 8:
                    fbuf.fmt.pixelformat = V4L2_PIX_FMT_RGB332;
                    break;

                case 15:
                    fbuf.fmt.pixelformat = V4L2_PIX_FMT_RGB555;
                    break;

                case 16:
                    fbuf.fmt.pixelformat = V4L2_PIX_FMT_RGB565;
                    break;

                case 24:
                    fbuf.fmt.pixelformat = V4L2_PIX_FMT_BGR24;
                    break;

                case 32:
                    fbuf.fmt.pixelformat = V4L2_PIX_FMT_BGR32;
                    break;
            }

            fbuf.fmt.bytesperline = buffer->bytesperline;
            result = v4l2_ioctl(fd, VIDIOC_S_FBUF, &fbuf);
            break;
        }

        case VIDIOCSTUNER: {
            struct video_tuner *tun = arg;
            struct v4l2_tuner t = { 0, };

            t.index = tun->tuner;
            result = v4l2_ioctl(fd, VIDIOC_S_TUNER, &t);

            break;
        }

        case VIDIOCGTUNER: {
            int i;
            v4l2_std_id sid;
            struct video_tuner *tun = arg;
            struct v4l2_tuner tun2 = { 0, };
            struct v4l2_standard std2 = { 0, };

            result = v4l2_ioctl(fd, VIDIOC_G_TUNER, &tun2);
            if (result < 0) {
                break;
            }

            memcpy(tun->name, tun2.name, min(sizeof(tun->name), sizeof(tun2.name)));
            tun->name[sizeof(tun->name) - 1] = 0;
            tun->rangelow = tun2.rangelow;
            tun->rangehigh = tun2.rangehigh;
            tun->flags = 0;
            tun->mode = VIDEO_MODE_AUTO;

            for (i = 0; i < 64; i++) {
                std2.index = i;
                if (0 != v4l2_ioctl(fd, VIDIOC_ENUMSTD, &std2)) {
                    break;
                }

                if (std2.id & V4L2_STD_PAL) {
                    tun->flags |= VIDEO_TUNER_PAL;
                }

                if (std2.id & V4L2_STD_NTSC) {
                    tun->flags |= VIDEO_TUNER_NTSC;
                }

                if (std2.id & V4L2_STD_SECAM) {
                    tun->flags |= VIDEO_TUNER_SECAM;
                }
            }

            if (v4l2_ioctl(fd, VIDIOC_G_STD, &sid) == 0) {
                if (sid & V4L2_STD_PAL) {
                    tun->mode = VIDEO_MODE_PAL;
                }

                if (sid & V4L2_STD_NTSC) {
                    tun->mode = VIDEO_MODE_NTSC;
                }

                if (sid & V4L2_STD_SECAM) {
                    tun->mode = VIDEO_MODE_SECAM;
                }
            }

            if (tun2.capability & V4L2_TUNER_CAP_LOW) {
                tun->flags |= VIDEO_TUNER_LOW;
            }

            if (tun2.rxsubchans & V4L2_TUNER_SUB_STEREO) {
                tun->flags |= VIDEO_TUNER_STEREO_ON;
            }
            tun->signal = tun2.signal;

            break;
        }

        case VIDIOCSFREQ: {
            unsigned long *freq = arg;
            struct v4l2_frequency freq2 = { 0, };

            result = v4l2_ioctl(fd, VIDIOC_G_FREQUENCY, &freq2);
            if (result < 0) {
                break;
            }

            freq2.frequency = *freq;
            result = v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq2);

            break;
        }

        case VIDIOCGFREQ: {
            unsigned long *freq = arg;
            struct v4l2_frequency freq2 = { 0, };

            freq2.tuner = 0;
            result = v4l2_ioctl(fd, VIDIOC_G_FREQUENCY, &freq2);
            if (result < 0) {
                break;
            }

            if (0 == result) {
                *freq = freq2.frequency;
            }
            break;
        }

        case VIDIOCCAPTURE: {
            int *on = arg;
            enum v4l2_buf_type captype = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (0 == *on) {
                v4l2_ioctl(fd, VIDIOC_STREAMOFF, &captype);
            }

            result = v4l2_ioctl(fd, VIDIOC_OVERLAY, on);
            break;
        }

        case VIDIOCSAUDIO: {
            struct video_audio *aud = arg;
            struct v4l2_audio aud2 = { 0, };
            struct v4l2_tuner tun2 = { 0, };

            aud2.index = aud->audio;
            result = v4l2_ioctl(fd, VIDIOC_S_AUDIO, &aud2);
            if (result < 0) {
                break;
            }

            v4l2_set_control(fd, V4L2_CID_AUDIO_VOLUME, aud->volume);
            v4l2_set_control(fd, V4L2_CID_AUDIO_BASS, aud->bass);
            v4l2_set_control(fd, V4L2_CID_AUDIO_TREBLE, aud->treble);
            v4l2_set_control(fd, V4L2_CID_AUDIO_BALANCE, aud->balance);
            v4l2_set_control(fd, V4L2_CID_AUDIO_MUTE, !!(aud->flags & VIDEO_AUDIO_MUTE));

            result = v4l2_ioctl(fd, VIDIOC_G_TUNER, &tun2);
            if (result == 0) {
                switch (aud->mode) {
                    default:
                    case VIDEO_SOUND_MONO:
                    case VIDEO_SOUND_LANG1:
                        tun2.audmode = V4L2_TUNER_MODE_MONO;
                        break;

                    case VIDEO_SOUND_STEREO:
                        tun2.audmode = V4L2_TUNER_MODE_STEREO;
                        break;

                    case VIDEO_SOUND_LANG2:
                        tun2.audmode = V4L2_TUNER_MODE_LANG2;
                        break;
                }

                result = v4l2_ioctl(fd, VIDIOC_S_TUNER, &tun2);
            }

            result = 0;
            break;
        }

        case VIDIOCGAUDIO: {
            int i;
            struct v4l2_tuner tun2;
            struct v4l2_queryctrl qctrl2;
            struct video_audio *aud = arg;
            struct v4l2_audio aud2 = { 0, };

            result = v4l2_ioctl(fd, VIDIOC_G_AUDIO, &aud2);
            if (result < 0) {
                break;
            }

            memcpy(aud->name, aud2.name, min(sizeof(aud->name), sizeof(aud2.name)));
            aud->name[sizeof(aud->name) - 1] = 0;
            aud->audio = aud2.index;
            aud->flags = 0;

            i = v4l2_get_control(fd, V4L2_CID_AUDIO_VOLUME);
            if (i >= 0) {
                aud->volume = i;
                aud->flags |= VIDEO_AUDIO_VOLUME;
            }

            i = v4l2_get_control(fd, V4L2_CID_AUDIO_BASS);
            if (i >= 0) {
                aud->bass = i;
                aud->flags |= VIDEO_AUDIO_BASS;
            }

            i = v4l2_get_control(fd, V4L2_CID_AUDIO_TREBLE);
            if (i >= 0) {
                aud->treble = i;
                aud->flags |= VIDEO_AUDIO_TREBLE;
            }

            i = v4l2_get_control(fd, V4L2_CID_AUDIO_BALANCE);
            if (i >= 0) {
                aud->balance = i;
                aud->flags |= VIDEO_AUDIO_BALANCE;
            }

            i = v4l2_get_control(fd, V4L2_CID_AUDIO_MUTE);
            if (i >= 0) {
                if (i) {
                    aud->flags |= VIDEO_AUDIO_MUTE;
                }
                aud->flags |= VIDEO_AUDIO_MUTABLE;
            }

            aud->step = 1;
            qctrl2.id = V4L2_CID_AUDIO_VOLUME;
            if ((v4l2_ioctl(fd, VIDIOC_QUERYCTRL, &qctrl2) == 0) && !(qctrl2.flags & V4L2_CTRL_FLAG_DISABLED)) {
                aud->step = qctrl2.step;
            }
            aud->mode = 0;

            result = v4l2_ioctl(fd, VIDIOC_G_TUNER, &tun2);
            if (result < 0) {
                result = 0;
                break;
            }

            if (tun2.rxsubchans & V4L2_TUNER_SUB_LANG2) {
                aud->mode = VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
            } else if (tun2.rxsubchans & V4L2_TUNER_SUB_STEREO) {
                aud->mode = VIDEO_SOUND_STEREO;
            } else if (tun2.rxsubchans & V4L2_TUNER_SUB_MONO) {
                aud->mode = VIDEO_SOUND_MONO;
            }

            break;
        }

        case VIDIOCSVBIFMT: {
            struct v4l2_format fmt2;
            struct vbi_format *fmt = arg;

            if (VIDEO_PALETTE_RAW != fmt->sample_format) {
                result = -EINVAL;
                break;
            }

            fmt2.type = V4L2_BUF_TYPE_VBI_CAPTURE;
            fmt2.fmt.vbi.samples_per_line = fmt->samples_per_line;
            fmt2.fmt.vbi.sampling_rate = fmt->sampling_rate;
            fmt2.fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
            fmt2.fmt.vbi.start[0] = fmt->start[0];
            fmt2.fmt.vbi.count[0] = fmt->count[0];
            fmt2.fmt.vbi.start[1] = fmt->start[1];
            fmt2.fmt.vbi.count[1] = fmt->count[1];
            fmt2.fmt.vbi.flags = fmt->flags;

            result  = v4l2_ioctl(fd, VIDIOC_TRY_FMT, fmt2);
            if (result < 0) {
                break;
            }

            if (fmt2.fmt.vbi.samples_per_line != fmt->samples_per_line
                || fmt2.fmt.vbi.sampling_rate != fmt->sampling_rate
                || fmt2.fmt.vbi.sample_format != V4L2_PIX_FMT_GREY
                || fmt2.fmt.vbi.start[0] != fmt->start[0]
                || fmt2.fmt.vbi.count[0] != fmt->count[0]
                || fmt2.fmt.vbi.start[1] != fmt->start[1]
                || fmt2.fmt.vbi.count[1] != fmt->count[1]
                || fmt2.fmt.vbi.flags != fmt->flags)
            {
                result = -EINVAL;
                break;
            }
            result = v4l2_ioctl(fd, VIDIOC_S_FMT, fmt2);
            break;
        }

        case VIDIOCGVBIFMT: {
            struct vbi_format *fmt = arg;
            struct v4l2_format fmt2 = { 0, };

            fmt2.type = V4L2_BUF_TYPE_VBI_CAPTURE;
            result = v4l2_ioctl(fd, VIDIOC_G_FMT, &fmt2);
            if (result < 0) {
                break;
            }

            if (fmt2.fmt.vbi.sample_format != V4L2_PIX_FMT_GREY) {
                result = -EINVAL;
                break;
            }

            fmt->samples_per_line = fmt2.fmt.vbi.samples_per_line;
            fmt->sampling_rate = fmt2.fmt.vbi.sampling_rate;
            fmt->sample_format = VIDEO_PALETTE_RAW;
            fmt->start[0] = fmt2.fmt.vbi.start[0];
            fmt->count[0] = fmt2.fmt.vbi.count[0];
            fmt->start[1] = fmt2.fmt.vbi.start[1];
            fmt->count[1] = fmt2.fmt.vbi.count[1];
            fmt->flags = fmt2.fmt.vbi.flags & 0x03;

            break;
        }

        default:
            result = v4l2_ioctl(fd, request, arg);
            break;
    }

    if (stream_locked) {
        pthread_mutex_unlock(&devices[index].stream_lock);
    }

    saved_err = errno;
    v4l1_log_ioctl(request, arg, result);
    errno = saved_err;

    return result;
}

ssize_t v4l1_read(int fd, void *buffer, size_t n)
{
    ssize_t result;
    int index = v4l1_get_index(fd);

    if (index == -1) {
        return SYS_READ(fd, buffer, n);
    }

    pthread_mutex_lock(&devices[index].stream_lock);
    result = v4l2_read(fd, buffer, n);
    pthread_mutex_unlock(&devices[index].stream_lock);

    return result;
}

void *v4l1_mmap(void *start, size_t length, int prot, int flags, int fd, int64_t offset)
{
    int index;
    void *result;

    index = v4l1_get_index(fd);
    if ((index == -1) || start || offset || (length != (V4L1_NO_FRAMES * V4L1_FRAME_BUF_SIZE))) {
        return v4l2_mmap(start, length, prot, flags, fd, offset);
    }

    pthread_mutex_lock(&devices[index].stream_lock);
    if (devices[index].v4l1_frame_pointer == MAP_FAILED) {
        result = v4l2_mmap(start, length, prot, flags, fd, offset);
        goto leave;
    }

    devices[index].v4l1_frame_buf_map_count++;
    V4L1_LOG("v4l1 buffer @ %p mapped by application\n", devices[index].v4l1_frame_pointer);
    result = devices[index].v4l1_frame_pointer;

leave:
    pthread_mutex_unlock(&devices[index].stream_lock);
    return result;
}

int v4l1_munmap(void *_start, size_t length)
{
    int index;
    unsigned char *start = _start;

    if ((start != MAP_FAILED) && (length == (V4L1_FRAME_BUF_SIZE * V4L1_NO_FRAMES))) {
        for (index = 0; index < devices_used; index++) {
            if ((devices[index].fd != -1) && (start == devices[index].v4l1_frame_pointer)) {
                break;
            }
        }

        if (index != devices_used) {
            int unmapped = 0;

            pthread_mutex_lock(&devices[index].stream_lock);
            if (start == devices[index].v4l1_frame_pointer) {
                if (devices[index].v4l1_frame_buf_map_count > 0) {
                    devices[index].v4l1_frame_buf_map_count--;
                }

                unmapped = 1;
            }
            pthread_mutex_unlock(&devices[index].stream_lock);

            if (unmapped) {
                V4L1_LOG("v4l1 buffer munmap %p, %d\n", start, (int)length);
                return 0;
            }
        }
    }

    V4L1_LOG("v4l1 unknown munmap %p, %d\n", start, (int)length);
    return v4l2_munmap(start, length);
}
