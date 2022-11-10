#ifndef LIBV4L_V4L2_DRIVER_H
#define LIBV4L_V4L2_DRIVER_H

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

#include "videodev2.h"

struct drv_list {
    void            *curr;
    struct drv_list *next;
};

struct v4l2_t_buf {
    void   *start;
    size_t length;
};

typedef int v4l2_recebe_buffer (struct v4l2_buffer *v4l2_buf, struct v4l2_t_buf *buf);

struct v4l2_driver {
    int                        fd;
    int                        debug;
    struct v4l2_capability     cap;
    struct v4l2_streamparm     parm;
    struct drv_list            *stds,*inputs,*fmt_caps;
    struct v4l2_requestbuffers reqbuf;
    struct v4l2_buffer         **v4l2_bufs;
    struct v4l2_t_buf          *bufs;
    uint32_t                   sizeimage,n_bufs;
    uint32_t                   waitq, currq;
};

enum v4l2_direction {
    V4L2_GET         = 1,
    V4L2_SET         = 2,
    V4L2_SET_GET     = 3,
    V4L2_TRY         = 4,
    V4L2_TRY_SET     = 6,
    V4L2_TRY_SET_GET = 7,
};

int v4l2_open(char *device, int debug, struct v4l2_driver *drv);
int v4l2_close(struct v4l2_driver *drv);

int v4l2_enum_stds(struct v4l2_driver *drv);
int v4l2_enum_input(struct v4l2_driver *drv);

int v4l2_enum_fmt(struct v4l2_driver *drv,enum v4l2_buf_type type);
int v4l2_get_parm(struct v4l2_driver *drv);

int v4l2_setget_std(struct v4l2_driver *drv, enum v4l2_direction dir, v4l2_std_id *id);
int v4l2_setget_input(struct v4l2_driver *drv, enum v4l2_direction dir, struct v4l2_input *input);

int v4l2_gettryset_fmt_cap(struct v4l2_driver *drv, enum v4l2_direction dir, struct v4l2_format *fmt,uint32_t width, uint32_t height, uint32_t pixelformat, enum v4l2_field field);

int v4l2_mmap_bufs(struct v4l2_driver *drv, unsigned int num_buffers);
int v4l2_free_bufs(struct v4l2_driver *drv);

int v4l2_start_streaming(struct v4l2_driver *drv);
int v4l2_stop_streaming(struct v4l2_driver *drv);

int v4l2_rcvbuf(struct v4l2_driver *drv, v4l2_recebe_buffer *v4l2_rec_buf);
int v4l2_getset_freq(struct v4l2_driver *drv, enum v4l2_direction dir, double *freq);

#endif
