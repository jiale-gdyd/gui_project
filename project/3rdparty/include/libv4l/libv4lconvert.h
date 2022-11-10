#ifndef LIBV4L_LIBV4LCONVERT_H
#define LIBV4L_LIBV4LCONVERT_H

#include <sys/time.h>
#include <linux/types.h>
#include <linux/ioctl.h>

#include "videodev2.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_VISIBILITY
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC __attribute__ ((visibility("default")))
#endif
#else
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC
#endif
#endif

struct libv4l_dev_ops;
struct v4lconvert_data;

LIBV4L_PUBLIC const struct libv4l_dev_ops *v4lconvert_get_default_dev_ops();

LIBV4L_PUBLIC struct v4lconvert_data *v4lconvert_create(int fd);
LIBV4L_PUBLIC struct v4lconvert_data *v4lconvert_create_with_dev_ops(int fd, void *dev_ops_priv, const struct libv4l_dev_ops *dev_ops);
LIBV4L_PUBLIC void v4lconvert_destroy(struct v4lconvert_data *data);

LIBV4L_PUBLIC int v4lconvert_supported_dst_fmt_only(struct v4lconvert_data *data);

LIBV4L_PUBLIC int v4lconvert_try_format(struct v4lconvert_data *data, struct v4l2_format *dest_fmt, struct v4l2_format *src_fmt);
LIBV4L_PUBLIC int v4lconvert_enum_fmt(struct v4lconvert_data *data, struct v4l2_fmtdesc *fmt);

LIBV4L_PUBLIC int v4lconvert_needs_conversion(struct v4lconvert_data *data, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt);

LIBV4L_PUBLIC int v4lconvert_convert(struct v4lconvert_data *data, const struct v4l2_format *src_fmt, const struct v4l2_format *dest_fmt, unsigned char *src, int src_size, unsigned char *dest, int dest_size);

LIBV4L_PUBLIC const char *v4lconvert_get_error_message(struct v4lconvert_data *data);

LIBV4L_PUBLIC int v4lconvert_enum_framesizes(struct v4lconvert_data *data, struct v4l2_frmsizeenum *frmsize);
LIBV4L_PUBLIC int v4lconvert_enum_frameintervals(struct v4lconvert_data *data, struct v4l2_frmivalenum *frmival);

LIBV4L_PUBLIC int v4lconvert_vidioc_queryctrl(struct v4lconvert_data *data, void *arg);
LIBV4L_PUBLIC int v4lconvert_vidioc_g_ctrl(struct v4lconvert_data *data, void *arg);
LIBV4L_PUBLIC int v4lconvert_vidioc_s_ctrl(struct v4lconvert_data *data, void *arg);
LIBV4L_PUBLIC int v4lconvert_vidioc_g_ext_ctrls(struct v4lconvert_data *data, void *arg);
LIBV4L_PUBLIC int v4lconvert_vidioc_try_ext_ctrls(struct v4lconvert_data *data, void *arg);
LIBV4L_PUBLIC int v4lconvert_vidioc_s_ext_ctrls(struct v4lconvert_data *data, void *arg);

LIBV4L_PUBLIC int v4lconvert_supported_dst_format(unsigned int pixelformat);

LIBV4L_PUBLIC int v4lconvert_get_fps(struct v4lconvert_data *data);
LIBV4L_PUBLIC void v4lconvert_set_fps(struct v4lconvert_data *data, int fps);

LIBV4L_PUBLIC void v4lconvert_fixup_fmt(struct v4l2_format *fmt);

#ifdef __cplusplus
}
#endif

#endif
