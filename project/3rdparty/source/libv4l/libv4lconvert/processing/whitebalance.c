#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "libv4lprocessing.h"
#include "libv4lprocessing-priv.h"
#include "../libv4lconvert-priv.h"

#define CLIP256(color)          (((color) > 0xff) ? 0xff : (((color) < 0) ? 0 : (color)))
#define CLIP(color, min, max)   (((color) > (max)) ? (max) : (((color) < (min)) ? (min) : (color)))

static int whitebalance_active(struct v4lprocessing_data *data)
{
    int wb;

    wb = v4lcontrol_get_ctrl(data->control, V4LCONTROL_WHITEBALANCE);
    if (!wb) {
        data->green_avg = 0;
    }

    return wb;
}

static int whitebalance_calculate_lookup_tables_generic(struct v4lprocessing_data *data, int green_avg, int comp1_avg, int comp2_avg)
{
    int i, avg_avg;
    const int threshold = 64;
    const int max_step = 128;

    green_avg = CLIP(green_avg, 512, 3072);
    comp1_avg = CLIP(comp1_avg, 512, 3072);
    comp2_avg = CLIP(comp2_avg, 512, 3072);

    if (data->green_avg == 0) {
        data->green_avg = green_avg;
        data->comp1_avg = comp1_avg;
        data->comp2_avg = comp2_avg;
    } else {
        int throttling = 0;

        if (abs(data->green_avg - green_avg) > max_step) {
            if (data->green_avg < green_avg) {
                data->green_avg += max_step;
            } else {
                data->green_avg -= max_step;
            }

            throttling = 1;
        } else {
            data->green_avg = green_avg;
        }

        if (abs(data->comp1_avg - comp1_avg) > max_step) {
            if (data->comp1_avg < comp1_avg) {
                data->comp1_avg += max_step;
            } else {
                data->comp1_avg -= max_step;
            }

            throttling = 1;
        } else {
            data->comp1_avg = comp1_avg;
        }

        if (abs(data->comp2_avg - comp2_avg) > max_step) {
            if (data->comp2_avg < comp2_avg) {
                data->comp2_avg += max_step;
            } else {
                data->comp2_avg -= max_step;
            }

            throttling = 1;
        } else {
            data->comp2_avg = comp2_avg;
        }

        if (throttling && (data->lookup_table_update_counter == 0)) {
            data->lookup_table_update_counter = V4L2PROCESSING_UPDATE_RATE;
        }
    }

    if (abs(data->green_avg - data->comp1_avg) < threshold
        && abs(data->green_avg - data->comp2_avg) < threshold
        && abs(data->comp1_avg - data->comp2_avg) < threshold)
    {
        return 0;
    }

    avg_avg = (data->green_avg + data->comp1_avg + data->comp2_avg) / 3;

    for (i = 0; i < 256; i++) {
        data->comp1[i] = CLIP256(data->comp1[i] * avg_avg / data->comp1_avg);
        data->green[i] = CLIP256(data->green[i] * avg_avg / data->green_avg);
        data->comp2[i] = CLIP256(data->comp2[i] * avg_avg / data->comp2_avg);
    }

    return 1;
}

static int whitebalance_calculate_lookup_tables_bayer(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt, int starts_with_green)
{
    int green_avg, comp1_avg, comp2_avg;
    int x, y, a1 = 0, a2 = 0, b1 = 0, b2 = 0;

    for (y = 0; y < fmt->fmt.pix.height; y += 2) {
        for (x = 0; x < fmt->fmt.pix.width; x += 2) {
            a1 += *buf++;
            a2 += *buf++;
        }

        buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width;
        for (x = 0; x < fmt->fmt.pix.width; x += 2) {
            b1 += *buf++;
            b2 += *buf++;
        }

        buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width;
    }

    if (starts_with_green) {
        green_avg = a1 / 2 + b2 / 2;
        comp1_avg = a2;
        comp2_avg = b1;
    } else {
        green_avg = a2 / 2 + b1 / 2;
        comp1_avg = a1;
        comp2_avg = b2;
    }

    green_avg /= fmt->fmt.pix.width * fmt->fmt.pix.height / 64;
    comp1_avg /= fmt->fmt.pix.width * fmt->fmt.pix.height / 64;
    comp2_avg /= fmt->fmt.pix.width * fmt->fmt.pix.height / 64;

    return whitebalance_calculate_lookup_tables_generic(data, green_avg, comp1_avg, comp2_avg);
}

static int whitebalance_calculate_lookup_tables_rgb(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    int x, y, green_avg = 0, comp1_avg = 0, comp2_avg = 0;

    for (y = 0; y < fmt->fmt.pix.height; y++) {
        for (x = 0; x < fmt->fmt.pix.width; x++) {
            comp1_avg += *buf++;
            green_avg += *buf++;
            comp2_avg += *buf++;
        }

        buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width * 3;
    }

    green_avg /= fmt->fmt.pix.width * fmt->fmt.pix.height / 16;
    comp1_avg /= fmt->fmt.pix.width * fmt->fmt.pix.height / 16;
    comp2_avg /= fmt->fmt.pix.width * fmt->fmt.pix.height / 16;

    return whitebalance_calculate_lookup_tables_generic(data, green_avg, comp1_avg, comp2_avg);
}

static int whitebalance_calculate_lookup_tables(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    switch (fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_SGBRG8:
        case V4L2_PIX_FMT_SGRBG8:
            return whitebalance_calculate_lookup_tables_bayer(data, buf, fmt, 1);

        case V4L2_PIX_FMT_SBGGR8:
        case V4L2_PIX_FMT_SRGGB8:
            return whitebalance_calculate_lookup_tables_bayer(data, buf, fmt, 0);

        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            return whitebalance_calculate_lookup_tables_rgb(data, buf, fmt);
    }

    return 0;
}

const struct v4lprocessing_filter whitebalance_filter = {
    whitebalance_active, whitebalance_calculate_lookup_tables
};
