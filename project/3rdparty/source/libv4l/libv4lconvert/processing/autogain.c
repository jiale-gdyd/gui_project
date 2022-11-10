#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "libv4lprocessing.h"
#include "libv4lprocessing-priv.h"
#include "../libv4lconvert-priv.h"
#include "../libv4lsyscall-priv.h"

static int autogain_active(struct v4lprocessing_data *data)
{
    int autogain;

    autogain = v4lcontrol_get_ctrl(data->control, V4LCONTROL_AUTOGAIN);
    if (!autogain) {
        data->last_gain_correction = 0;
    }

    return autogain;
}

static void autogain_adjust(struct v4l2_queryctrl *ctrl, int *value, int steps, int limit, int accel)
{
    int ctrl_range = (ctrl->maximum - ctrl->minimum) / ctrl->step;

    if (accel && abs(steps) >= 3 && ctrl_range > 256) {
        *value += steps * ctrl->step * (ctrl_range / 256);
    } else if (accel && (ctrl_range > 1024)) {
        *value += steps * ctrl->step * (ctrl_range / 1024);
    } else {
        *value += steps * ctrl->step;
    }

    if (steps > 0) {
        if (*value > limit) {
            *value = limit;
        }
    } else {
        if (*value < limit) {
            *value = limit;
        }
    }
}

static int autogain_calculate_lookup_tables(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt)
{
    const int deadzone = 6;
    struct v4l2_control ctrl;
    int x, y, target, steps, avg_lum = 0;
    struct v4l2_queryctrl gainctrl, expoctrl;
    int gain, exposure, orig_gain, orig_exposure, exposure_low;

    ctrl.id = V4L2_CID_EXPOSURE;
    expoctrl.id = V4L2_CID_EXPOSURE;
    if (SYS_IOCTL(data->fd, VIDIOC_QUERYCTRL, &expoctrl) || SYS_IOCTL(data->fd, VIDIOC_G_CTRL, &ctrl)) {
        return 0;
    }

    exposure = orig_exposure = ctrl.value;
    exposure_low = (expoctrl.maximum - expoctrl.minimum) / 10;
    steps = exposure_low / expoctrl.step;
    if (steps > 10) {
        steps = 10;
    }
    exposure_low = steps * expoctrl.step + expoctrl.minimum;

    ctrl.id = V4L2_CID_GAIN;
    gainctrl.id = V4L2_CID_GAIN;
    if (SYS_IOCTL(data->fd, VIDIOC_QUERYCTRL, &gainctrl) || SYS_IOCTL(data->fd, VIDIOC_G_CTRL, &ctrl)) {
        return 0;
    }
    gain = orig_gain = ctrl.value;

    switch (fmt->fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_SGBRG8:
        case V4L2_PIX_FMT_SGRBG8:
        case V4L2_PIX_FMT_SBGGR8:
        case V4L2_PIX_FMT_SRGGB8:
            buf += fmt->fmt.pix.height * fmt->fmt.pix.bytesperline / 4 + fmt->fmt.pix.width / 4;

            for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
                for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
                    avg_lum += *buf++;
                }

                buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width / 2;
            }
            avg_lum /= fmt->fmt.pix.height * fmt->fmt.pix.width / 4;
            break;

        case V4L2_PIX_FMT_RGB24:
        case V4L2_PIX_FMT_BGR24:
            buf += fmt->fmt.pix.height * fmt->fmt.pix.bytesperline / 4 + fmt->fmt.pix.width * 3 / 4;

            for (y = 0; y < fmt->fmt.pix.height / 2; y++) {
                for (x = 0; x < fmt->fmt.pix.width / 2; x++) {
                    avg_lum += *buf++;
                    avg_lum += *buf++;
                    avg_lum += *buf++;
                }

                buf += fmt->fmt.pix.bytesperline - fmt->fmt.pix.width * 3 / 2;
            }
            avg_lum /= fmt->fmt.pix.height * fmt->fmt.pix.width * 3 / 4;
            break;
    }

    target = v4lcontrol_get_ctrl(data->control, V4LCONTROL_AUTOGAIN_TARGET);
    steps = (target - avg_lum) / deadzone;

    if (((steps > 0) && (data->last_gain_correction < 0)) || ((steps < 0) && (data->last_gain_correction > 0))) {
        steps /= 2;
    }

    if (steps == 0) {
        return 0;
    }

    if (steps < 0) {
        if (exposure > expoctrl.default_value) {
            autogain_adjust(&expoctrl, &exposure, steps, expoctrl.default_value, 1);
        } else if (gain > gainctrl.default_value) {
            autogain_adjust(&gainctrl, &gain, steps, gainctrl.default_value, 1);
        } else if (exposure > exposure_low) {
            autogain_adjust(&expoctrl, &exposure, steps, exposure_low, 1);
        } else if (gain > gainctrl.minimum) {
            autogain_adjust(&gainctrl, &gain, steps, gainctrl.minimum, 1);
        } else if (exposure > expoctrl.minimum) {
            autogain_adjust(&expoctrl, &exposure, steps, expoctrl.minimum, 0);
        } else {
            steps = 0;
        }
    } else {
        if (exposure < exposure_low) {
            autogain_adjust(&expoctrl, &exposure, steps, exposure_low, 0);
        } else if (gain < gainctrl.default_value) {
            autogain_adjust(&gainctrl, &gain, steps, gainctrl.default_value, 1);
        } else if (exposure < expoctrl.default_value) {
            autogain_adjust(&expoctrl, &exposure, steps, expoctrl.default_value, 1);
        } else if (gain < gainctrl.maximum) {
            autogain_adjust(&gainctrl, &gain, steps, gainctrl.maximum, 1);
        } else if (exposure < expoctrl.maximum) {
            autogain_adjust(&expoctrl, &exposure, steps, expoctrl.maximum, 1);
        } else {
            steps = 0;
        }
    }

    if (steps) {
        data->last_gain_correction = steps;
        data->lookup_table_update_counter = V4L2PROCESSING_UPDATE_RATE - 2;
    }

    if (gain != orig_gain) {
        ctrl.id = V4L2_CID_GAIN;
        ctrl.value = gain;
        SYS_IOCTL(data->fd, VIDIOC_S_CTRL, &ctrl);
    }

    if (exposure != orig_exposure) {
        ctrl.id = V4L2_CID_EXPOSURE;
        ctrl.value = exposure;
        SYS_IOCTL(data->fd, VIDIOC_S_CTRL, &ctrl);
    }

    return 0;
}

const struct v4lprocessing_filter autogain_filter = {
    autogain_active, autogain_calculate_lookup_tables
};
