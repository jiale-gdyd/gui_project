#ifndef LIBV4L_LIBV4L1_PRIV_H
#define LIBV4L_LIBV4L1_PRIV_H

#include <stdio.h>
#include <pthread.h>

#include "../libv4lconvert/libv4lsyscall-priv.h"

#define V4L1_MAX_DEVICES            16
#define V4L1_NO_FRAMES              4
#define V4L1_FRAME_BUF_SIZE         (4096 * 4096)

extern FILE *v4l1_log_file;

#define V4L1_LOG_ERR(...)                                                       \
    do {                                                                        \
        if (v4l1_log_file) {                                                    \
            fprintf(v4l1_log_file, "libv4l1: error " __VA_ARGS__);              \
            fflush(v4l1_log_file);                                              \
        } else {                                                                \
            fprintf(stderr, "libv4l1: error " __VA_ARGS__);                     \
        }                                                                       \
    } while (0)

#define V4L1_LOG_WARN(...)                                                      \
    do {                                                                        \
        if (v4l1_log_file) {                                                    \
            fprintf(v4l1_log_file, "libv4l1: warning " __VA_ARGS__);            \
            fflush(v4l1_log_file);                                              \
        } else {                                                                \
            fprintf(stderr, "libv4l1: warning " __VA_ARGS__);                   \
        }                                                                       \
    } while (0)

#define V4L1_LOG(...)                                                           \
    do {                                                                        \
        if (v4l1_log_file) {                                                    \
            fprintf(v4l1_log_file, "libv4l1: " __VA_ARGS__);                    \
            fflush(v4l1_log_file);                                              \
        }                                                                       \
    } while (0)

#ifndef min
#define min(a, b)                   (((a) < (b)) ? (a) : (b))
#endif

struct v4l1_dev_info {
    int             fd;
    int             flags;
    int             open_count;
    int             v4l1_frame_buf_map_count;
    pthread_mutex_t stream_lock;
    unsigned int    depth;
    unsigned int    v4l1_pal;
    unsigned int    v4l2_pixfmt;
    unsigned int    min_width, min_height, max_width, max_height;
    unsigned int    width, height;
    unsigned char   *v4l1_frame_pointer;
};

void v4l1_log_ioctl(unsigned long int request, void *arg, int result);

#endif
