#ifndef LIBV4L_LIBV4LPROCESSING_H
#define LIBV4L_LIBV4LPROCESSING_H

#include <libv4l/videodev2.h>
#include "../libv4lsyscall-priv.h"

struct v4lcontrol_data;
struct v4lprocessing_data;

struct v4lprocessing_data *v4lprocessing_create(int fd, struct v4lcontrol_data *data);
void v4lprocessing_destroy(struct v4lprocessing_data *data);

int v4lprocessing_pre_processing(struct v4lprocessing_data *data);
void v4lprocessing_processing(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt);

#endif
