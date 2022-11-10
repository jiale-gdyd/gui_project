#include <math.h>

#include "libv4lprocessing.h"
#include "libv4lprocessing-priv.h"

#define CLIP(color)         (unsigned char)(((color) > 0xff) ? 0xff : (((color) < 0) ? 0 : (color)))

static int gamma_active(struct v4lprocessing_data *data)
{
    int gamma = v4lcontrol_get_ctrl(data->control, V4LCONTROL_GAMMA);
    return gamma && (gamma != 1000);
}

static int gamma_calculate_lookup_tables(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    int i, x, gamma;

    gamma = v4lcontrol_get_ctrl(data->control, V4LCONTROL_GAMMA);
    if (gamma == 0) {
        return 0;
    }

    if (gamma != data->last_gamma) {
        for (i = 0; i < 256; i++) {
            x = powf(i / 255.0, 1000.0 / gamma) * 255;
            data->gamma_table[i] = CLIP(x);
        }

        data->last_gamma = gamma;
    }

    for (i = 0; i < 256; i++) {
        data->comp1[i] = data->gamma_table[data->comp1[i]];
        data->green[i] = data->gamma_table[data->green[i]];
        data->comp2[i] = data->gamma_table[data->comp2[i]];
    }

    return 1;
}

const struct v4lprocessing_filter gamma_filter = {
    gamma_active, gamma_calculate_lookup_tables
};
