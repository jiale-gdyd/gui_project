#ifndef LIBV4L_LIBV4LCONTROL_H
#define LIBV4L_LIBV4LCONTROL_H

#include <libv4l/libv4l-plugin.h>

#define V4LCONTROL_HFLIPPED              0x01
#define V4LCONTROL_VFLIPPED              0x02
#define V4LCONTROL_ROTATED_90_JPEG       0x04
#define V4LCONTROL_WANTS_WB              0x08
#define V4LCONTROL_WANTS_AUTOGAIN        0x10
#define V4LCONTROL_FORCE_TINYJPEG        0x20

#define V4LCONTROL_WANTS_WB_AUTOGAIN     (V4LCONTROL_WANTS_WB | V4LCONTROL_WANTS_AUTOGAIN)

enum {
    V4LCONTROL_WHITEBALANCE,
    V4LCONTROL_HFLIP,
    V4LCONTROL_VFLIP,
    V4LCONTROL_GAMMA,
    V4LCONTROL_AUTO_ENABLE_COUNT,
    V4LCONTROL_AUTOGAIN,
    V4LCONTROL_AUTOGAIN_TARGET,
    V4LCONTROL_COUNT
};

struct v4lcontrol_data;

struct v4lcontrol_data *v4lcontrol_create(int fd, void *dev_ops_priv, const struct libv4l_dev_ops *dev_ops, int always_needs_conversion);
void v4lcontrol_destroy(struct v4lcontrol_data *data);

int v4lcontrol_get_bandwidth(struct v4lcontrol_data *data);

int v4lcontrol_get_flags(struct v4lcontrol_data *data);
int v4lcontrol_get_ctrl(struct v4lcontrol_data *data, int ctrl);

int v4lcontrol_controls_changed(struct v4lcontrol_data *data);
int v4lcontrol_needs_conversion(struct v4lcontrol_data *data);

int v4lcontrol_vidioc_queryctrl(struct v4lcontrol_data *data, void *arg);
int v4lcontrol_vidioc_g_ctrl(struct v4lcontrol_data *data, void *arg);
int v4lcontrol_vidioc_s_ctrl(struct v4lcontrol_data *data, void *arg);
int v4lcontrol_vidioc_g_ext_ctrls(struct v4lcontrol_data *data, void *arg);
int v4lcontrol_vidioc_try_ext_ctrls(struct v4lcontrol_data *data, void *arg);
int v4lcontrol_vidioc_s_ext_ctrls(struct v4lcontrol_data *data, void *arg);

#endif
