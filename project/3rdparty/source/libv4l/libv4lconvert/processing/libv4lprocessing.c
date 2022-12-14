#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libv4lprocessing.h"
#include "libv4lprocessing-priv.h"
#include "../libv4lconvert-priv.h"

static const struct v4lprocessing_filter *filters[] = {
    &whitebalance_filter,
    &autogain_filter,
    &gamma_filter,
};

struct v4lprocessing_data *v4lprocessing_create(int fd, struct v4lcontrol_data *control)
{
    struct v4lprocessing_data *data = calloc(1, sizeof(struct v4lprocessing_data));
    if (!data) {
        fprintf(stderr, "libv4lprocessing: error: out of memory!\n");
        return NULL;
    }

    data->fd = fd;
    data->control = control;

    return data;
}

void v4lprocessing_destroy(struct v4lprocessing_data *data)
{
    free(data);
}

int v4lprocessing_pre_processing(struct v4lprocessing_data *data)
{
    int i;

    data->do_process = 0;
    for (i = 0; i < ARRAY_SIZE(filters); i++) {
        if (filters[i]->active(data)) {
            data->do_process = 1;
        }
    }

    data->controls_changed |= v4lcontrol_controls_changed(data->control);
    return data->do_process;
}

static void v4lprocessing_update_lookup_tables(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    int i;

    for (i = 0; i < 256; i++) {
        data->comp1[i] = i;
        data->green[i] = i;
        data->comp2[i] = i;
    }

    data->lookup_table_active = 0;
    for (i = 0; i < ARRAY_SIZE(filters); i++) {
        if (filters[i]->active(data)) {
            if (filters[i]->calculate_lookup_tables(data, buf, fmt)) {
                data->lookup_table_active = 1;
            }
        }
    }
}

static void v4lprocessing_do_processing(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    int x, y;

    switch (fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_SGBRG8:
        case V4L2_PIX_FMT_SGRBG8:
            for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
                for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
                    *buf = data->green[*buf];
                    buf++;
                    *buf = data->comp1[*buf];
                    buf++;
                }

                buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width;
                for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
                    *buf = data->comp2[*buf];
                    buf++;
                    *buf = data->green[*buf];
                    buf++;
                }

                buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width;
            }
            break;

        case V4L2_PIX_FMT_SBGGR8:
        case V4L2_PIX_FMT_SRGGB8:
            for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
                for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
                    *buf = data->comp1[*buf];
                    buf++;
                    *buf = data->green[*buf];
                    buf++;
                }

                buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width;
                for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
                    *buf = data->green[*buf];
                    buf++;
                    *buf = data->comp2[*buf];
                    buf++;
                }

                buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width;
            }
            break;

        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            for (y = 0; y < fmt->fmt.pix.height; y++) {
                for (x = 0; x < fmt->fmt.pix.width; x++) {
                    *buf = data->comp1[*buf];
                    buf++;
                    *buf = data->green[*buf];
                    buf++;
                    *buf = data->comp2[*buf];
                    buf++;
                }

                buf += fmt->fmt.pix.bytesperline - 3 * fmt->fmt.pix.width;
            }
            break;
    }
}

void v4lprocessing_processing(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    if (!data->do_process) {
        return;
    }

    switch (fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_SGBRG8:
        case V4L2_PIX_FMT_SGRBG8:
        case V4L2_PIX_FMT_SBGGR8:
        case V4L2_PIX_FMT_SRGGB8:
        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            break;

        default:
            return;
    }

    if (data->controls_changed || (data->lookup_table_update_counter == V4L2PROCESSING_UPDATE_RATE)) {
        data->controls_changed = 0;
        data->lookup_table_update_counter = 0;
        v4lprocessing_update_lookup_tables(data, buf, fmt);
    } else {
        data->lookup_table_update_counter++;
    }

    if (data->lookup_table_active) {
        v4lprocessing_do_processing(data, buf, fmt);
    }
    data->do_process = 0;
}
