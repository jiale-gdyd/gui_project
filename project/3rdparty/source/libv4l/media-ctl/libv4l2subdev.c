#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <libv4l/mediactl.h>
#include <libv4l/v4l2subdev.h>
#include <libv4l/mediactl-private.h>
#include <libv4l/media-bus-format.h>

int v4l2_subdev_open(struct media_entity *entity)
{
    if (entity->fd != -1) {
        return 0;
    }

    entity->fd = open(entity->devname, O_RDWR);
    if (entity->fd == -1) {
        int ret = -errno;
        media_dbg(entity->media, "failed to open subdev device node:[%s]\n", entity->devname);
        return ret;
    }

    return 0;
}

void v4l2_subdev_close(struct media_entity *entity)
{
    close(entity->fd);
    entity->fd = -1;
}

int v4l2_subdev_get_format(struct media_entity *entity, struct v4l2_mbus_framefmt *format, unsigned int pad, enum v4l2_subdev_format_whence which)
{
    int ret;
    struct v4l2_subdev_format fmt;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(&fmt, 0x00, sizeof(fmt));
    fmt.pad = pad;
    fmt.which = which;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_G_FMT, &fmt);
    if (ret < 0) {
        return -errno;
    }
    *format = fmt.format;

    return 0;
}

int v4l2_subdev_set_format(struct media_entity *entity, struct v4l2_mbus_framefmt *format, unsigned int pad, enum v4l2_subdev_format_whence which)
{
    int ret;
    struct v4l2_subdev_format fmt;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(&fmt, 0x00, sizeof(fmt));
    fmt.pad = pad;
    fmt.which = which;
    fmt.format = *format;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_S_FMT, &fmt);
    if (ret < 0) {
        return -errno;
    }
    *format = fmt.format;

    return 0;
}

int v4l2_subdev_get_selection(struct media_entity *entity, struct v4l2_rect *rect, unsigned int pad, unsigned int target, enum v4l2_subdev_format_whence which)
{
    union {
        struct v4l2_subdev_selection sel;
        struct v4l2_subdev_crop      crop;
    } u;
    int ret;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(&u.sel, 0, sizeof(u.sel));
    u.sel.pad = pad;
    u.sel.target = target;
    u.sel.which = which;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_G_SELECTION, &u.sel);
    if (ret >= 0) {
        *rect = u.sel.r;
        return 0;
    }

    if ((errno != ENOTTY) || (target != V4L2_SEL_TGT_CROP)) {
        return -errno;
    }

    memset(&u.crop, 0x00, sizeof(u.crop));
    u.crop.pad = pad;
    u.crop.which = which;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_G_CROP, &u.crop);
    if (ret < 0) {
        return -errno;
    }
    *rect = u.crop.rect;

    return 0;
}

int v4l2_subdev_set_selection(struct media_entity *entity, struct v4l2_rect *rect, unsigned int pad, unsigned int target, enum v4l2_subdev_format_whence which)
{
    union {
        struct v4l2_subdev_selection sel;
        struct v4l2_subdev_crop crop;
    } u;
    int ret;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(&u.sel, 0x00, sizeof(u.sel));
    u.sel.pad = pad;
    u.sel.target = target;
    u.sel.which = which;
    u.sel.r = *rect;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_S_SELECTION, &u.sel);
    if (ret >= 0) {
        *rect = u.sel.r;
        return 0;
    }

    if ((errno != ENOTTY) || (target != V4L2_SEL_TGT_CROP)) {
        return -errno;
    }

    memset(&u.crop, 0, sizeof(u.crop));
    u.crop.pad = pad;
    u.crop.which = which;
    u.crop.rect = *rect;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_S_CROP, &u.crop);
    if (ret < 0) {
        return -errno;
    }
    *rect = u.crop.rect;

    return 0;
}

int v4l2_subdev_get_dv_timings_caps(struct media_entity *entity, struct v4l2_dv_timings_cap *caps)
{
    int ret;
    unsigned int pad = caps->pad;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(caps, 0x00, sizeof(*caps));
    caps->pad = pad;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_DV_TIMINGS_CAP, caps);
    if (ret < 0) {
        return -errno;
    }

    return 0;
}

int v4l2_subdev_query_dv_timings(struct media_entity *entity, struct v4l2_dv_timings *timings)
{
    int ret;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(timings, 0x00, sizeof(*timings));
    ret = ioctl(entity->fd, VIDIOC_SUBDEV_QUERY_DV_TIMINGS, timings);
    if (ret < 0) {
        return -errno;
    }

    return 0;
}

int v4l2_subdev_get_dv_timings(struct media_entity *entity, struct v4l2_dv_timings *timings)
{
    int ret;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(timings, 0x00, sizeof(*timings));
    ret = ioctl(entity->fd, VIDIOC_SUBDEV_G_DV_TIMINGS, timings);
    if (ret < 0) {
        return -errno;
    }

    return 0;
}

int v4l2_subdev_set_dv_timings(struct media_entity *entity, struct v4l2_dv_timings *timings)
{
    int ret;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_S_DV_TIMINGS, timings);
    if (ret < 0) {
        return -errno;
    }

    return 0;
}

int v4l2_subdev_get_frame_interval(struct media_entity *entity, struct v4l2_fract *interval, unsigned int pad)
{
    int ret;
    struct v4l2_subdev_frame_interval ival;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(&ival, 0x00, sizeof(ival));
    ival.pad = pad;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_G_FRAME_INTERVAL, &ival);
    if (ret < 0) {
        return -errno;
    }
    *interval = ival.interval;

    return 0;
}

int v4l2_subdev_set_frame_interval(struct media_entity *entity, struct v4l2_fract *interval, unsigned int pad)
{
    int ret;
    struct v4l2_subdev_frame_interval ival;

    ret = v4l2_subdev_open(entity);
    if (ret < 0) {
        return ret;
    }

    memset(&ival, 0x00, sizeof(ival));
    ival.pad = pad;
    ival.interval = *interval;

    ret = ioctl(entity->fd, VIDIOC_SUBDEV_S_FRAME_INTERVAL, &ival);
    if (ret < 0) {
        return -errno;
    }
    *interval = ival.interval;

    return 0;
}

static int v4l2_subdev_parse_format(struct media_device *media, struct v4l2_mbus_framefmt *format, const char *p, char **endp)
{
    char *fmt;
    char *end;
    unsigned int width, height;
    enum v4l2_mbus_pixelcode code;

    for (; isspace(*p); ++p);
    for (end = (char *)p; *end != '/' && *end != ' ' && *end != '\0'; ++end);

    fmt = strndup(p, end - p);
    if (!fmt) {
        return -ENOMEM;
    }

    code = v4l2_subdev_string_to_pixelcode(fmt);
    free(fmt);
    if (code == (enum v4l2_mbus_pixelcode)-1) {
        media_dbg(media, "Invalid pixel code '%.*s'\n", end - p, p);
        return -EINVAL;
    }

    p = end + 1;
    width = strtoul(p, &end, 10);
    if (*end != 'x') {
        media_dbg(media, "Expected 'x'\n");
        return -EINVAL;
    }

    p = end + 1;
    height = strtoul(p, &end, 10);
    *endp = end;

    memset(format, 0, sizeof(*format));
    format->width = width;
    format->height = height;
    format->code = code;

    return 0;
}

static int v4l2_subdev_parse_rectangle(struct media_device *media, struct v4l2_rect *r, const char *p, char **endp)
{
    char *end;

    if (*p++ != '(') {
        media_dbg(media, "Expected '('\n");
        *endp = (char *)p - 1;
        return -EINVAL;
    }

    r->left = strtoul(p, &end, 10);
    if (*end != ',') {
        media_dbg(media, "Expected ','\n");
        *endp = end;
        return -EINVAL;
    }

    p = end + 1;
    r->top = strtoul(p, &end, 10);
    if (*end++ != ')') {
        media_dbg(media, "Expected ')'\n");
        *endp = end - 1;
        return -EINVAL;
    }

    if (*end != '/') {
        media_dbg(media, "Expected '/'\n");
        *endp = end;
        return -EINVAL;
    }

    p = end + 1;
    r->width = strtoul(p, &end, 10);
    if (*end != 'x') {
        media_dbg(media, "Expected 'x'\n");
        *endp = end;
        return -EINVAL;
    }

    p = end + 1;
    r->height = strtoul(p, &end, 10);
    *endp = end;

    return 0;
}

static int v4l2_subdev_parse_frame_interval(struct media_device *media, struct v4l2_fract *interval, const char *p, char **endp)
{
    char *end;

    for (; isspace(*p); ++p);

    interval->numerator = strtoul(p, &end, 10);

    for (p = end; isspace(*p); ++p);
    if (*p++ != '/') {
        media_dbg(media, "Expected '/'\n");
        *endp = (char *)p - 1;
        return -EINVAL;
    }

    for (; isspace(*p); ++p);
    interval->denominator = strtoul(p, &end, 10);

    *endp = end;
    return 0;
}

static bool strhazit(const char *str, const char **p)
{
    int len = strlen(str);

    if (strncmp(str, *p, len)) {
        return false;
    }

    for (*p += len; isspace(**p); ++*p);
    return true;
}

static struct media_pad *v4l2_subdev_parse_pad_format(struct media_device *media, struct v4l2_mbus_framefmt *format, struct v4l2_rect *crop, struct v4l2_rect *compose, struct v4l2_fract *interval, const char *p, char **endp)
{
    int ret;
    char *end;
    bool first;
    struct media_pad *pad;

    for (; isspace(*p); ++p);

    pad = media_parse_pad(media, p, &end);
    if (pad == NULL) {
        *endp = end;
        return NULL;
    }

    for (p = end; isspace(*p); ++p);
    if (*p++ != '[') {
        media_dbg(media, "Expected '['\n");
        *endp = (char *)p - 1;
        return NULL;
    }

    for (first = true; ; first = false) {
        for (; isspace(*p); p++);

        if (strhazit("fmt:", &p) || (first && isupper(*p))) {
            ret = v4l2_subdev_parse_format(media, format, p, &end);
            if (ret < 0) {
                *endp = end;
                return NULL;
            }

            p = end;
            continue;
        }

        if (strhazit("field:", &p)) {
            char *strfield;
            enum v4l2_field field;

            for (end = (char *)p; isalpha(*end) || *end == '-'; ++end);

            strfield = strndup(p, end - p);
            if (!strfield) {
                *endp = (char *)p;
                return NULL;
            }

            field = v4l2_subdev_string_to_field(strfield);
            free(strfield);
            if (field == (enum v4l2_field)-1) {
                media_dbg(media, "Invalid field value '%*s'\n", end - p, p);
                *endp = (char *)p;
                return NULL;
            }

            format->field = field;

            p = end;
            continue;
        }

        if (strhazit("colorspace:", &p)) {
            char *strfield;
            enum v4l2_colorspace colorspace;

            for (end = (char *)p; isalnum(*end) || *end == '-'; ++end);

            strfield = strndup(p, end - p);
            if (!strfield) {
                *endp = (char *)p;
                return NULL;
            }

            colorspace = v4l2_subdev_string_to_colorspace(strfield);
            free(strfield);
            if (colorspace == (enum v4l2_colorspace)-1) {
                media_dbg(media, "Invalid colorspace value '%*s'\n", end - p, p);
                *endp = (char *)p;
                return NULL;
            }

            format->colorspace = colorspace;

            p = end;
            continue;
        }

        if (strhazit("xfer:", &p)) {
            char *strfield;
            enum v4l2_xfer_func xfer_func;

            for (end = (char *)p; isalnum(*end) || *end == '-'; ++end);

            strfield = strndup(p, end - p);
            if (!strfield) {
                *endp = (char *)p;
                return NULL;
            }

            xfer_func = v4l2_subdev_string_to_xfer_func(strfield);
            free(strfield);

            if (xfer_func == (enum v4l2_xfer_func)-1) {
                media_dbg(media, "Invalid transfer function value '%*s'\n", end - p, p);
                *endp = (char *)p;
                return NULL;
            }

            format->xfer_func = xfer_func;

            p = end;
            continue;
        }

        if (strhazit("ycbcr:", &p)) {
            char *strfield;
            enum v4l2_ycbcr_encoding ycbcr_enc;

            for (end = (char *)p; isalnum(*end) || *end == '-'; ++end);

            strfield = strndup(p, end - p);
            if (!strfield) {
                *endp = (char *)p;
                return NULL;
            }

            ycbcr_enc = v4l2_subdev_string_to_ycbcr_encoding(strfield);
            free(strfield);
            if (ycbcr_enc == (enum v4l2_ycbcr_encoding)-1) {
                media_dbg(media, "Invalid YCbCr encoding value '%*s'\n", end - p, p);
                *endp = (char *)p;
                return NULL;
            }

            format->ycbcr_enc = ycbcr_enc;

            p = end;
            continue;
        }

        if (strhazit("quantization:", &p)) {
            char *strfield;
            enum v4l2_quantization quantization;

            for (end = (char *)p; isalnum(*end) || *end == '-'; ++end);

            strfield = strndup(p, end - p);
            if (!strfield) {
                *endp = (char *)p;
                return NULL;
            }

            quantization = v4l2_subdev_string_to_quantization(strfield);
            free(strfield);

            if (quantization == (enum v4l2_quantization)-1) {
                media_dbg(media, "Invalid quantization value '%*s'\n", end - p, p);
                *endp = (char *)p;
                return NULL;
            }

            format->quantization = quantization;

            p = end;
            continue;
        }

        if (strhazit("crop:", &p) || *p == '(') {
            ret = v4l2_subdev_parse_rectangle(media, crop, p, &end);
            if (ret < 0) {
                *endp = end;
                return NULL;
            }

            p = end;
            continue;
        }

        if (strhazit("compose:", &p)) {
            ret = v4l2_subdev_parse_rectangle(media, compose, p, &end);
            if (ret < 0) {
                *endp = end;
                return NULL;
            }

            for (p = end; isspace(*p); p++);
            continue;
        }

        if (*p == '@') {
            ret = v4l2_subdev_parse_frame_interval(media, interval, ++p, &end);
            if (ret < 0) {
                *endp = end;
                return NULL;
            }

            p = end;
            continue;
        }

        break;
    }

    if (*p != ']') {
        media_dbg(media, "Expected ']'\n");
        *endp = (char *)p;
        return NULL;
    }

    *endp = (char *)p + 1;
    return pad;
}

static int set_format(struct media_pad *pad, struct v4l2_mbus_framefmt *format)
{
    int ret;

    if ((format->width == 0) || (format->height == 0)) {
        return 0;
    }

    media_dbg(pad->entity->media, "Setting up format %s %ux%u on pad %s/%u\n", v4l2_subdev_pixelcode_to_string(format->code), format->width, format->height, pad->entity->info.name, pad->index);

    ret = v4l2_subdev_set_format(pad->entity, format, pad->index, V4L2_SUBDEV_FORMAT_ACTIVE);
    if (ret < 0) {
        media_dbg(pad->entity->media, "Unable to set format: %s (%d)\n", strerror(-ret), ret);
        return ret;
    }

    media_dbg(pad->entity->media, "Format set: %s %ux%u\n", v4l2_subdev_pixelcode_to_string(format->code), format->width, format->height);
    return 0;
}

static int set_selection(struct media_pad *pad, unsigned int target, struct v4l2_rect *rect)
{
    int ret;

    if ((rect->left == -1) || (rect->top == -1)) {
        return 0;
    }

    media_dbg(pad->entity->media, "Setting up selection target %u rectangle (%u,%u)/%ux%u on pad %s/%u\n", target, rect->left, rect->top, rect->width, rect->height, pad->entity->info.name, pad->index);

    ret = v4l2_subdev_set_selection(pad->entity, rect, pad->index, target, V4L2_SUBDEV_FORMAT_ACTIVE);
    if (ret < 0) {
        media_dbg(pad->entity->media, "Unable to set selection rectangle: %s (%d)\n", strerror(-ret), ret);
        return ret;
    }

    media_dbg(pad->entity->media, "Selection rectangle set: (%u,%u)/%ux%u\n", rect->left, rect->top, rect->width, rect->height);
    return 0;
}

static int set_frame_interval(struct media_pad *pad, struct v4l2_fract *interval)
{
    int ret;

    if (interval->numerator == 0) {
        return 0;
    }

    media_dbg(pad->entity->media, "Setting up frame interval %u/%u on pad %s/%u\n", interval->numerator, interval->denominator, pad->entity->info.name, pad->index);

    ret = v4l2_subdev_set_frame_interval(pad->entity, interval, pad->index);
    if (ret < 0) {
        media_dbg(pad->entity->media, "Unable to set frame interval: %s (%d)", strerror(-ret), ret);
        return ret;
    }

    media_dbg(pad->entity->media, "Frame interval set: %u/%u\n", interval->numerator, interval->denominator);
    return 0;
}

static int v4l2_subdev_parse_setup_format(struct media_device *media, const char *p, char **endp)
{
    int ret;
    char *end;
    unsigned int i;
    struct media_pad *pad;
    struct v4l2_rect crop = { -1, -1, -1, -1 };
    struct v4l2_rect compose = crop;
    struct v4l2_fract interval = { 0, 0 };
    struct v4l2_mbus_framefmt format = { 0, 0, 0 };

    pad = v4l2_subdev_parse_pad_format(media, &format, &crop, &compose, &interval, p, &end);
    if (pad == NULL) {
        media_print_streampos(media, p, end);
        media_dbg(media, "Unable to parse format\n");
        return -EINVAL;
    }

    if (pad->flags & MEDIA_PAD_FL_SINK) {
        ret = set_format(pad, &format);
        if (ret < 0) {
            return ret;
        }
    }

    ret = set_selection(pad, V4L2_SEL_TGT_CROP, &crop);
    if (ret < 0) {
        return ret;
    }

    ret = set_selection(pad, V4L2_SEL_TGT_COMPOSE, &compose);
    if (ret < 0) {
        return ret;
    }

    if (pad->flags & MEDIA_PAD_FL_SOURCE) {
        ret = set_format(pad, &format);
        if (ret < 0) {
            return ret;
        }
    }

    ret = set_frame_interval(pad, &interval);
    if (ret < 0) {
        return ret;
    }

    if (pad->flags & MEDIA_PAD_FL_SOURCE) {
        for (i = 0; i < pad->entity->num_links; ++i) {
            struct v4l2_mbus_framefmt remote_format;
            struct media_link *link = &pad->entity->links[i];

            if (!(link->flags & MEDIA_LNK_FL_ENABLED)) {
                continue;
            }

            if ((link->source == pad) && (link->sink->entity->info.type == MEDIA_ENT_T_V4L2_SUBDEV)) {
                remote_format = format;
                set_format(link->sink, &remote_format);

                ret = set_frame_interval(link->sink, &interval);
                if ((ret < 0) && (ret != -EINVAL) && (ret != -ENOTTY)) {
                    return ret;
                }
            }
        }
    }

    *endp = end;
    return 0;
}

int v4l2_subdev_parse_setup_formats(struct media_device *media, const char *p)
{
    int ret;
    char *end;

    do {
        ret = v4l2_subdev_parse_setup_format(media, p, &end);
        if (ret < 0) {
            return ret;
        }
    
        for (; isspace(*end); end++);
        p = end + 1;
    } while (*end == ',');

    return *end ? -EINVAL : 0;
}

static const struct {
    const char               *name;
    enum v4l2_mbus_pixelcode code;
} mbus_formats[] = {
    { "RGB444_1X12", MEDIA_BUS_FMT_RGB444_1X12 },
    { "RGB444_2X8_PADHI_BE", MEDIA_BUS_FMT_RGB444_2X8_PADHI_BE },
    { "RGB444_2X8_PADHI_LE", MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE },
    { "RGB555_2X8_PADHI_BE", MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE },
    { "RGB555_2X8_PADHI_LE", MEDIA_BUS_FMT_RGB555_2X8_PADHI_LE },
    { "RGB565_1X16", MEDIA_BUS_FMT_RGB565_1X16 },
    { "BGR565_2X8_BE", MEDIA_BUS_FMT_BGR565_2X8_BE },
    { "BGR565_2X8_LE", MEDIA_BUS_FMT_BGR565_2X8_LE },
    { "RGB565_2X8_BE", MEDIA_BUS_FMT_RGB565_2X8_BE },
    { "RGB565_2X8_LE", MEDIA_BUS_FMT_RGB565_2X8_LE },
    { "RGB666_1X18", MEDIA_BUS_FMT_RGB666_1X18 },
    { "RBG888_1X24", MEDIA_BUS_FMT_RBG888_1X24 },
    { "RGB666_1X24_CPADHI", MEDIA_BUS_FMT_RGB666_1X24_CPADHI },
    { "RGB666_1X7X3_SPWG", MEDIA_BUS_FMT_RGB666_1X7X3_SPWG },
    { "BGR888_1X24", MEDIA_BUS_FMT_BGR888_1X24 },
    { "BGR888_3X8", MEDIA_BUS_FMT_BGR888_3X8 },
    { "GBR888_1X24", MEDIA_BUS_FMT_GBR888_1X24 },
    { "RGB888_1X24", MEDIA_BUS_FMT_RGB888_1X24 },
    { "RGB888_2X12_BE", MEDIA_BUS_FMT_RGB888_2X12_BE },
    { "RGB888_2X12_LE", MEDIA_BUS_FMT_RGB888_2X12_LE },
    { "RGB888_3X8", MEDIA_BUS_FMT_RGB888_3X8 },
    { "RGB888_3X8_DELTA", MEDIA_BUS_FMT_RGB888_3X8_DELTA },
    { "RGB888_1X7X4_SPWG", MEDIA_BUS_FMT_RGB888_1X7X4_SPWG },
    { "RGB888_1X7X4_JEIDA", MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA },
    { "RGB666_1X30_CPADLO", MEDIA_BUS_FMT_RGB666_1X30_CPADLO },
    { "RGB888_1X30_CPADLO", MEDIA_BUS_FMT_RGB888_1X30_CPADLO },
    { "ARGB8888_1X32", MEDIA_BUS_FMT_ARGB8888_1X32 },
    { "RGB888_1X32_PADHI", MEDIA_BUS_FMT_RGB888_1X32_PADHI },
    { "RGB101010_1X30", MEDIA_BUS_FMT_RGB101010_1X30 },
    { "RGB666_1X36_CPADLO", MEDIA_BUS_FMT_RGB666_1X36_CPADLO },
    { "RGB888_1X36_CPADLO", MEDIA_BUS_FMT_RGB888_1X36_CPADLO },
    { "RGB121212_1X36", MEDIA_BUS_FMT_RGB121212_1X36 },
    { "RGB161616_1X48", MEDIA_BUS_FMT_RGB161616_1X48 },
    { "Y8_1X8", MEDIA_BUS_FMT_Y8_1X8 },
    { "UV8_1X8", MEDIA_BUS_FMT_UV8_1X8 },
    { "UYVY8_1_5X8", MEDIA_BUS_FMT_UYVY8_1_5X8 },
    { "VYUY8_1_5X8", MEDIA_BUS_FMT_VYUY8_1_5X8 },
    { "YUYV8_1_5X8", MEDIA_BUS_FMT_YUYV8_1_5X8 },
    { "YVYU8_1_5X8", MEDIA_BUS_FMT_YVYU8_1_5X8 },
    { "UYVY8_2X8", MEDIA_BUS_FMT_UYVY8_2X8 },
    { "VYUY8_2X8", MEDIA_BUS_FMT_VYUY8_2X8 },
    { "YUYV8_2X8", MEDIA_BUS_FMT_YUYV8_2X8 },
    { "YVYU8_2X8", MEDIA_BUS_FMT_YVYU8_2X8 },
    { "Y10_1X10", MEDIA_BUS_FMT_Y10_1X10 },
    { "Y10_2X8_PADHI_LE", MEDIA_BUS_FMT_Y10_2X8_PADHI_LE },
    { "UYVY10_2X10", MEDIA_BUS_FMT_UYVY10_2X10 },
    { "VYUY10_2X10", MEDIA_BUS_FMT_VYUY10_2X10 },
    { "YUYV10_2X10", MEDIA_BUS_FMT_YUYV10_2X10 },
    { "YVYU10_2X10", MEDIA_BUS_FMT_YVYU10_2X10 },
    { "Y12_1X12", MEDIA_BUS_FMT_Y12_1X12 },
    { "UYVY12_2X12", MEDIA_BUS_FMT_UYVY12_2X12 },
    { "VYUY12_2X12", MEDIA_BUS_FMT_VYUY12_2X12 },
    { "YUYV12_2X12", MEDIA_BUS_FMT_YUYV12_2X12 },
    { "YVYU12_2X12", MEDIA_BUS_FMT_YVYU12_2X12 },
    { "Y14_1X14", MEDIA_BUS_FMT_Y14_1X14 },
    { "UYVY8_1X16", MEDIA_BUS_FMT_UYVY8_1X16 },
    { "VYUY8_1X16", MEDIA_BUS_FMT_VYUY8_1X16 },
    { "YUYV8_1X16", MEDIA_BUS_FMT_YUYV8_1X16 },
    { "YVYU8_1X16", MEDIA_BUS_FMT_YVYU8_1X16 },
    { "YDYUYDYV8_1X16", MEDIA_BUS_FMT_YDYUYDYV8_1X16 },
    { "UYVY10_1X20", MEDIA_BUS_FMT_UYVY10_1X20 },
    { "VYUY10_1X20", MEDIA_BUS_FMT_VYUY10_1X20 },
    { "YUYV10_1X20", MEDIA_BUS_FMT_YUYV10_1X20 },
    { "YVYU10_1X20", MEDIA_BUS_FMT_YVYU10_1X20 },
    { "VUY8_1X24", MEDIA_BUS_FMT_VUY8_1X24 },
    { "YUV8_1X24", MEDIA_BUS_FMT_YUV8_1X24 },
    { "UYYVYY8_0_5X24", MEDIA_BUS_FMT_UYYVYY8_0_5X24 },
    { "UYVY12_1X24", MEDIA_BUS_FMT_UYVY12_1X24 },
    { "VYUY12_1X24", MEDIA_BUS_FMT_VYUY12_1X24 },
    { "YUYV12_1X24", MEDIA_BUS_FMT_YUYV12_1X24 },
    { "YVYU12_1X24", MEDIA_BUS_FMT_YVYU12_1X24 },
    { "YUV10_1X30", MEDIA_BUS_FMT_YUV10_1X30 },
    { "UYYVYY10_0_5X30", MEDIA_BUS_FMT_UYYVYY10_0_5X30 },
    { "AYUV8_1X32", MEDIA_BUS_FMT_AYUV8_1X32 },
    { "UYYVYY12_0_5X36", MEDIA_BUS_FMT_UYYVYY12_0_5X36 },
    { "YUV12_1X36", MEDIA_BUS_FMT_YUV12_1X36 },
    { "YUV16_1X48", MEDIA_BUS_FMT_YUV16_1X48 },
    { "UYYVYY16_0_5X48", MEDIA_BUS_FMT_UYYVYY16_0_5X48 },
    { "SBGGR8_1X8", MEDIA_BUS_FMT_SBGGR8_1X8 },
    { "SGBRG8_1X8", MEDIA_BUS_FMT_SGBRG8_1X8 },
    { "SGRBG8_1X8", MEDIA_BUS_FMT_SGRBG8_1X8 },
    { "SRGGB8_1X8", MEDIA_BUS_FMT_SRGGB8_1X8 },
    { "SBGGR10_ALAW8_1X8", MEDIA_BUS_FMT_SBGGR10_ALAW8_1X8 },
    { "SGBRG10_ALAW8_1X8", MEDIA_BUS_FMT_SGBRG10_ALAW8_1X8 },
    { "SGRBG10_ALAW8_1X8", MEDIA_BUS_FMT_SGRBG10_ALAW8_1X8 },
    { "SRGGB10_ALAW8_1X8", MEDIA_BUS_FMT_SRGGB10_ALAW8_1X8 },
    { "SBGGR10_DPCM8_1X8", MEDIA_BUS_FMT_SBGGR10_DPCM8_1X8 },
    { "SGBRG10_DPCM8_1X8", MEDIA_BUS_FMT_SGBRG10_DPCM8_1X8 },
    { "SGRBG10_DPCM8_1X8", MEDIA_BUS_FMT_SGRBG10_DPCM8_1X8 },
    { "SRGGB10_DPCM8_1X8", MEDIA_BUS_FMT_SRGGB10_DPCM8_1X8 },
    { "SBGGR10_2X8_PADHI_BE", MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_BE },
    { "SBGGR10_2X8_PADHI_LE", MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_LE },
    { "SBGGR10_2X8_PADLO_BE", MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_BE },
    { "SBGGR10_2X8_PADLO_LE", MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_LE },
    { "SBGGR10_1X10", MEDIA_BUS_FMT_SBGGR10_1X10 },
    { "SGBRG10_1X10", MEDIA_BUS_FMT_SGBRG10_1X10 },
    { "SGRBG10_1X10", MEDIA_BUS_FMT_SGRBG10_1X10 },
    { "SRGGB10_1X10", MEDIA_BUS_FMT_SRGGB10_1X10 },
    { "SBGGR12_1X12", MEDIA_BUS_FMT_SBGGR12_1X12 },
    { "SGBRG12_1X12", MEDIA_BUS_FMT_SGBRG12_1X12 },
    { "SGRBG12_1X12", MEDIA_BUS_FMT_SGRBG12_1X12 },
    { "SRGGB12_1X12", MEDIA_BUS_FMT_SRGGB12_1X12 },
    { "SBGGR14_1X14", MEDIA_BUS_FMT_SBGGR14_1X14 },
    { "SGBRG14_1X14", MEDIA_BUS_FMT_SGBRG14_1X14 },
    { "SGRBG14_1X14", MEDIA_BUS_FMT_SGRBG14_1X14 },
    { "SRGGB14_1X14", MEDIA_BUS_FMT_SRGGB14_1X14 },
    { "SBGGR16_1X16", MEDIA_BUS_FMT_SBGGR16_1X16 },
    { "SGBRG16_1X16", MEDIA_BUS_FMT_SGBRG16_1X16 },
    { "SGRBG16_1X16", MEDIA_BUS_FMT_SGRBG16_1X16 },
    { "SRGGB16_1X16", MEDIA_BUS_FMT_SRGGB16_1X16 },
    { "JPEG_1X8", MEDIA_BUS_FMT_JPEG_1X8 },
    { "S5C_UYVY_JPEG_1X8", MEDIA_BUS_FMT_S5C_UYVY_JPEG_1X8 },
    { "AHSV8888_1X32", MEDIA_BUS_FMT_AHSV8888_1X32 },

    { "FIXED", MEDIA_BUS_FMT_FIXED },
    { "Y8", MEDIA_BUS_FMT_Y8_1X8 },
    { "Y10", MEDIA_BUS_FMT_Y10_1X10 },
    { "Y12", MEDIA_BUS_FMT_Y12_1X12 },
    { "YUYV", MEDIA_BUS_FMT_YUYV8_1X16 },
    { "YUYV1_5X8", MEDIA_BUS_FMT_YUYV8_1_5X8 },
    { "YUYV2X8", MEDIA_BUS_FMT_YUYV8_2X8 },
    { "UYVY", MEDIA_BUS_FMT_UYVY8_1X16 },
    { "UYVY1_5X8", MEDIA_BUS_FMT_UYVY8_1_5X8 },
    { "UYVY2X8", MEDIA_BUS_FMT_UYVY8_2X8 },
    { "VUY24", MEDIA_BUS_FMT_VUY8_1X24 },
    { "SBGGR8", MEDIA_BUS_FMT_SBGGR8_1X8 },
    { "SGBRG8", MEDIA_BUS_FMT_SGBRG8_1X8 },
    { "SGRBG8", MEDIA_BUS_FMT_SGRBG8_1X8 },
    { "SRGGB8", MEDIA_BUS_FMT_SRGGB8_1X8 },
    { "SBGGR10", MEDIA_BUS_FMT_SBGGR10_1X10 },
    { "SGBRG10", MEDIA_BUS_FMT_SGBRG10_1X10 },
    { "SGRBG10", MEDIA_BUS_FMT_SGRBG10_1X10 },
    { "SRGGB10", MEDIA_BUS_FMT_SRGGB10_1X10 },
    { "SBGGR10_DPCM8", MEDIA_BUS_FMT_SBGGR10_DPCM8_1X8 },
    { "SGBRG10_DPCM8", MEDIA_BUS_FMT_SGBRG10_DPCM8_1X8 },
    { "SGRBG10_DPCM8", MEDIA_BUS_FMT_SGRBG10_DPCM8_1X8 },
    { "SRGGB10_DPCM8", MEDIA_BUS_FMT_SRGGB10_DPCM8_1X8 },
    { "SBGGR12", MEDIA_BUS_FMT_SBGGR12_1X12 },
    { "SGBRG12", MEDIA_BUS_FMT_SGBRG12_1X12 },
    { "SGRBG12", MEDIA_BUS_FMT_SGRBG12_1X12 },
    { "SRGGB12", MEDIA_BUS_FMT_SRGGB12_1X12 },
    { "AYUV32", MEDIA_BUS_FMT_AYUV8_1X32 },
    { "RBG24", MEDIA_BUS_FMT_RBG888_1X24 },
    { "RGB32", MEDIA_BUS_FMT_RGB888_1X32_PADHI },
    { "ARGB32", MEDIA_BUS_FMT_ARGB8888_1X32 },
};

const char *v4l2_subdev_pixelcode_to_string(enum v4l2_mbus_pixelcode code)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(mbus_formats); ++i) {
        if (mbus_formats[i].code == code) {
            return mbus_formats[i].name;
        }
    }

    return "unknown";
}

enum v4l2_mbus_pixelcode v4l2_subdev_string_to_pixelcode(const char *string)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(mbus_formats); ++i) {
        if (strcmp(mbus_formats[i].name, string) == 0) {
            return mbus_formats[i].code;
        }
    }

    return (enum v4l2_mbus_pixelcode)-1;
}

static struct {
    const char      *name;
    enum v4l2_field field;
} fields[] = {
    { "any", V4L2_FIELD_ANY },
    { "none", V4L2_FIELD_NONE },
    { "top", V4L2_FIELD_TOP },
    { "bottom", V4L2_FIELD_BOTTOM },
    { "interlaced", V4L2_FIELD_INTERLACED },
    { "seq-tb", V4L2_FIELD_SEQ_TB },
    { "seq-bt", V4L2_FIELD_SEQ_BT },
    { "alternate", V4L2_FIELD_ALTERNATE },
    { "interlaced-tb", V4L2_FIELD_INTERLACED_TB },
    { "interlaced-bt", V4L2_FIELD_INTERLACED_BT },
};

const char *v4l2_subdev_field_to_string(enum v4l2_field field)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(fields); ++i) {
        if (fields[i].field == field) {
            return fields[i].name;
        }
    }

    return "unknown";
}

enum v4l2_field v4l2_subdev_string_to_field(const char *string)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(fields); ++i) {
        if (strcasecmp(fields[i].name, string) == 0) {
            return fields[i].field;
        }
    }

    return (enum v4l2_field)-1;
}

static struct {
    const char           *name;
    enum v4l2_colorspace colorspace;
} colorspaces[] = {
    { "default", V4L2_COLORSPACE_DEFAULT },
    { "smpte170m", V4L2_COLORSPACE_SMPTE170M },
    { "smpte240m", V4L2_COLORSPACE_SMPTE240M },
    { "rec709", V4L2_COLORSPACE_REC709 },
    { "470m", V4L2_COLORSPACE_470_SYSTEM_M },
    { "470bg", V4L2_COLORSPACE_470_SYSTEM_BG },
    { "jpeg", V4L2_COLORSPACE_JPEG },
    { "srgb", V4L2_COLORSPACE_SRGB },
    { "oprgb", V4L2_COLORSPACE_OPRGB },
    { "bt2020", V4L2_COLORSPACE_BT2020 },
    { "raw", V4L2_COLORSPACE_RAW },
    { "dcip3", V4L2_COLORSPACE_DCI_P3 },
};

const char *v4l2_subdev_colorspace_to_string(enum v4l2_colorspace colorspace)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(colorspaces); ++i) {
        if (colorspaces[i].colorspace == colorspace) {
            return colorspaces[i].name;
        }
    }

    return "unknown";
}

enum v4l2_colorspace v4l2_subdev_string_to_colorspace(const char *string)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(colorspaces); ++i) {
        if (strcasecmp(colorspaces[i].name, string) == 0) {
            return colorspaces[i].colorspace;
        }
    }

    return (enum v4l2_colorspace)-1;
}

static struct {
    const char          *name;
    enum v4l2_xfer_func xfer_func;
} xfer_funcs[] = {
    { "default", V4L2_XFER_FUNC_DEFAULT },
    { "709", V4L2_XFER_FUNC_709 },
    { "srgb", V4L2_XFER_FUNC_SRGB },
    { "oprgb", V4L2_XFER_FUNC_OPRGB },
    { "smpte240m", V4L2_XFER_FUNC_SMPTE240M },
    { "smpte2084", V4L2_XFER_FUNC_SMPTE2084 },
    { "dcip3", V4L2_XFER_FUNC_DCI_P3 },
    { "none", V4L2_XFER_FUNC_NONE },
};

const char *v4l2_subdev_xfer_func_to_string(enum v4l2_xfer_func xfer_func)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(xfer_funcs); ++i) {
        if (xfer_funcs[i].xfer_func == xfer_func) {
            return xfer_funcs[i].name;
        }
    }

    return "unknown";
}

enum v4l2_xfer_func v4l2_subdev_string_to_xfer_func(const char *string)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(xfer_funcs); ++i) {
        if (strcasecmp(xfer_funcs[i].name, string) == 0) {
            return xfer_funcs[i].xfer_func;
        }
    }

    return (enum v4l2_xfer_func)-1;
}

static struct {
    const char               *name;
    enum v4l2_ycbcr_encoding ycbcr_enc;
} ycbcr_encs[] = {
    { "default", V4L2_YCBCR_ENC_DEFAULT },
    { "601", V4L2_YCBCR_ENC_601 },
    { "709", V4L2_YCBCR_ENC_709 },
    { "xv601", V4L2_YCBCR_ENC_XV601 },
    { "xv709", V4L2_YCBCR_ENC_XV709 },
    { "bt2020", V4L2_YCBCR_ENC_BT2020 },
    { "bt2020c", V4L2_YCBCR_ENC_BT2020_CONST_LUM },
    { "smpte240m", V4L2_YCBCR_ENC_SMPTE240M },
};

const char *v4l2_subdev_ycbcr_encoding_to_string(enum v4l2_ycbcr_encoding ycbcr_enc)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(ycbcr_encs); ++i) {
        if (ycbcr_encs[i].ycbcr_enc == ycbcr_enc) {
            return ycbcr_encs[i].name;
        }
    }

    return "unknown";
}

enum v4l2_ycbcr_encoding v4l2_subdev_string_to_ycbcr_encoding(const char *string)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(ycbcr_encs); ++i) {
        if (strcasecmp(ycbcr_encs[i].name, string) == 0) {
            return ycbcr_encs[i].ycbcr_enc;
        }
    }

    return (enum v4l2_ycbcr_encoding)-1;
}

static struct {
    const char             *name;
    enum v4l2_quantization quantization;
} quantizations[] = {
    { "default", V4L2_QUANTIZATION_DEFAULT },
    { "full-range", V4L2_QUANTIZATION_FULL_RANGE },
    { "lim-range", V4L2_QUANTIZATION_LIM_RANGE },
};

const char *v4l2_subdev_quantization_to_string(enum v4l2_quantization quantization)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(quantizations); ++i) {
        if (quantizations[i].quantization == quantization) {
            return quantizations[i].name;
        }
    }

    return "unknown";
}

enum v4l2_quantization v4l2_subdev_string_to_quantization(const char *string)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(quantizations); ++i) {
        if (strcasecmp(quantizations[i].name, string) == 0) {
            return quantizations[i].quantization;
        }
    }

    return (enum v4l2_quantization)-1;
}

static const enum v4l2_mbus_pixelcode mbus_codes[] = {
    MEDIA_BUS_FMT_RGB444_1X12,
    MEDIA_BUS_FMT_RGB444_2X8_PADHI_BE,
    MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE,
    MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE,
    MEDIA_BUS_FMT_RGB555_2X8_PADHI_LE,
    MEDIA_BUS_FMT_RGB565_1X16,
    MEDIA_BUS_FMT_BGR565_2X8_BE,
    MEDIA_BUS_FMT_BGR565_2X8_LE,
    MEDIA_BUS_FMT_RGB565_2X8_BE,
    MEDIA_BUS_FMT_RGB565_2X8_LE,
    MEDIA_BUS_FMT_RGB666_1X18,
    MEDIA_BUS_FMT_RBG888_1X24,
    MEDIA_BUS_FMT_RGB666_1X24_CPADHI,
    MEDIA_BUS_FMT_RGB666_1X7X3_SPWG,
    MEDIA_BUS_FMT_BGR888_1X24,
    MEDIA_BUS_FMT_BGR888_3X8,
    MEDIA_BUS_FMT_GBR888_1X24,
    MEDIA_BUS_FMT_RGB888_1X24,
    MEDIA_BUS_FMT_RGB888_2X12_BE,
    MEDIA_BUS_FMT_RGB888_2X12_LE,
    MEDIA_BUS_FMT_RGB888_3X8,
    MEDIA_BUS_FMT_RGB888_3X8_DELTA,
    MEDIA_BUS_FMT_RGB888_1X7X4_SPWG,
    MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA,
    MEDIA_BUS_FMT_RGB666_1X30_CPADLO,
    MEDIA_BUS_FMT_RGB888_1X30_CPADLO,
    MEDIA_BUS_FMT_ARGB8888_1X32,
    MEDIA_BUS_FMT_RGB888_1X32_PADHI,
    MEDIA_BUS_FMT_RGB101010_1X30,
    MEDIA_BUS_FMT_RGB666_1X36_CPADLO,
    MEDIA_BUS_FMT_RGB888_1X36_CPADLO,
    MEDIA_BUS_FMT_RGB121212_1X36,
    MEDIA_BUS_FMT_RGB161616_1X48,
    MEDIA_BUS_FMT_Y8_1X8,
    MEDIA_BUS_FMT_UV8_1X8,
    MEDIA_BUS_FMT_UYVY8_1_5X8,
    MEDIA_BUS_FMT_VYUY8_1_5X8,
    MEDIA_BUS_FMT_YUYV8_1_5X8,
    MEDIA_BUS_FMT_YVYU8_1_5X8,
    MEDIA_BUS_FMT_UYVY8_2X8,
    MEDIA_BUS_FMT_VYUY8_2X8,
    MEDIA_BUS_FMT_YUYV8_2X8,
    MEDIA_BUS_FMT_YVYU8_2X8,
    MEDIA_BUS_FMT_Y10_1X10,
    MEDIA_BUS_FMT_Y10_2X8_PADHI_LE,
    MEDIA_BUS_FMT_UYVY10_2X10,
    MEDIA_BUS_FMT_VYUY10_2X10,
    MEDIA_BUS_FMT_YUYV10_2X10,
    MEDIA_BUS_FMT_YVYU10_2X10,
    MEDIA_BUS_FMT_Y12_1X12,
    MEDIA_BUS_FMT_UYVY12_2X12,
    MEDIA_BUS_FMT_VYUY12_2X12,
    MEDIA_BUS_FMT_YUYV12_2X12,
    MEDIA_BUS_FMT_YVYU12_2X12,
    MEDIA_BUS_FMT_Y14_1X14,
    MEDIA_BUS_FMT_UYVY8_1X16,
    MEDIA_BUS_FMT_VYUY8_1X16,
    MEDIA_BUS_FMT_YUYV8_1X16,
    MEDIA_BUS_FMT_YVYU8_1X16,
    MEDIA_BUS_FMT_YDYUYDYV8_1X16,
    MEDIA_BUS_FMT_UYVY10_1X20,
    MEDIA_BUS_FMT_VYUY10_1X20,
    MEDIA_BUS_FMT_YUYV10_1X20,
    MEDIA_BUS_FMT_YVYU10_1X20,
    MEDIA_BUS_FMT_VUY8_1X24,
    MEDIA_BUS_FMT_YUV8_1X24,
    MEDIA_BUS_FMT_UYYVYY8_0_5X24,
    MEDIA_BUS_FMT_UYVY12_1X24,
    MEDIA_BUS_FMT_VYUY12_1X24,
    MEDIA_BUS_FMT_YUYV12_1X24,
    MEDIA_BUS_FMT_YVYU12_1X24,
    MEDIA_BUS_FMT_YUV10_1X30,
    MEDIA_BUS_FMT_UYYVYY10_0_5X30,
    MEDIA_BUS_FMT_AYUV8_1X32,
    MEDIA_BUS_FMT_UYYVYY12_0_5X36,
    MEDIA_BUS_FMT_YUV12_1X36,
    MEDIA_BUS_FMT_YUV16_1X48,
    MEDIA_BUS_FMT_UYYVYY16_0_5X48,
    MEDIA_BUS_FMT_SBGGR8_1X8,
    MEDIA_BUS_FMT_SGBRG8_1X8,
    MEDIA_BUS_FMT_SGRBG8_1X8,
    MEDIA_BUS_FMT_SRGGB8_1X8,
    MEDIA_BUS_FMT_SBGGR10_ALAW8_1X8,
    MEDIA_BUS_FMT_SGBRG10_ALAW8_1X8,
    MEDIA_BUS_FMT_SGRBG10_ALAW8_1X8,
    MEDIA_BUS_FMT_SRGGB10_ALAW8_1X8,
    MEDIA_BUS_FMT_SBGGR10_DPCM8_1X8,
    MEDIA_BUS_FMT_SGBRG10_DPCM8_1X8,
    MEDIA_BUS_FMT_SGRBG10_DPCM8_1X8,
    MEDIA_BUS_FMT_SRGGB10_DPCM8_1X8,
    MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_BE,
    MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_LE,
    MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_BE,
    MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_LE,
    MEDIA_BUS_FMT_SBGGR10_1X10,
    MEDIA_BUS_FMT_SGBRG10_1X10,
    MEDIA_BUS_FMT_SGRBG10_1X10,
    MEDIA_BUS_FMT_SRGGB10_1X10,
    MEDIA_BUS_FMT_SBGGR12_1X12,
    MEDIA_BUS_FMT_SGBRG12_1X12,
    MEDIA_BUS_FMT_SGRBG12_1X12,
    MEDIA_BUS_FMT_SRGGB12_1X12,
    MEDIA_BUS_FMT_SBGGR14_1X14,
    MEDIA_BUS_FMT_SGBRG14_1X14,
    MEDIA_BUS_FMT_SGRBG14_1X14,
    MEDIA_BUS_FMT_SRGGB14_1X14,
    MEDIA_BUS_FMT_SBGGR16_1X16,
    MEDIA_BUS_FMT_SGBRG16_1X16,
    MEDIA_BUS_FMT_SGRBG16_1X16,
    MEDIA_BUS_FMT_SRGGB16_1X16,
    MEDIA_BUS_FMT_JPEG_1X8,
    MEDIA_BUS_FMT_S5C_UYVY_JPEG_1X8,
    MEDIA_BUS_FMT_AHSV8888_1X32,
};

const enum v4l2_mbus_pixelcode *v4l2_subdev_pixelcode_list(unsigned int *length)
{
    *length = ARRAY_SIZE(mbus_codes);
    return mbus_codes;
}
