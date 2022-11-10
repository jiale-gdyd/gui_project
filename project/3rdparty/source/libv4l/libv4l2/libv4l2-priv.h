#ifndef LIBV4L_LIBV4L2_PRIV_H
#define LIBV4L_LIBV4L2_PRIV_H

#include <stdio.h>
#include <pthread.h>
#include <libv4l/libv4lconvert.h>

#include "../libv4lconvert/libv4lsyscall-priv.h"

#define V4L2_MAX_DEVICES                    16
#define V4L2_MAX_NO_FRAMES                  32
#define V4L2_DEFAULT_NREADBUFFERS           4
#define V4L2_IGNORE_FIRST_FRAME_ERRORS      3
#define V4L2_DEFAULT_FPS                    30

#define V4L2_LOG_ERR(...)                                                           \
    do {                                                                            \
        if (v4l2_log_file) {                                                        \
            fprintf(v4l2_log_file, "libv4l2: error " __VA_ARGS__);                  \
            fflush(v4l2_log_file);                                                  \
        } else {                                                                    \
            fprintf(stderr, "libv4l2: error " __VA_ARGS__);                         \
        }                                                                           \
    } while (0)

#define V4L2_PERROR(format, ...)                                                    \
    do {                                                                            \
        if (errno == ENODEV) {                                                      \
            devices[index].gone = 1;                                                \
            break;                                                                  \
        }                                                                           \
        V4L2_LOG_ERR(format ": %s\n", ##__VA_ARGS__, strerror(errno));              \
    } while (0)

#define V4L2_LOG_WARN(...)                                                          \
    do {                                                                            \
        if (v4l2_log_file) {                                                        \
            fprintf(v4l2_log_file, "libv4l2: warning " __VA_ARGS__);                \
            fflush(v4l2_log_file);                                                  \
        } else {                                                                    \
            fprintf(stderr, "libv4l2: warning " __VA_ARGS__);                       \
        }                                                                           \
    } while (0)

#define V4L2_LOG(...)                                                               \
    do {                                                                            \
        if (v4l2_log_file) {                                                        \
            fprintf(v4l2_log_file, "libv4l2: " __VA_ARGS__);                        \
            fflush(v4l2_log_file);                                                  \
        }                                                                           \
    } while (0)

#define MIN(a, b)                           (((a) < (b)) ? (a) : (b))

struct v4l2_dev_info {
    int                         fd;
    int                         flags;
    int                         open_count;
    int                         gone;
    long                        page_size;
    struct v4l2_format          src_fmt;
    struct v4l2_format          dest_fmt;
    pthread_mutex_t             stream_lock;
    unsigned int                no_frames;
    unsigned int                nreadbuffers;
    int                         fps;
    int                         first_frame;
    struct v4lconvert_data      *convert;
    unsigned char               *convert_mmap_buf;
    size_t                      convert_mmap_buf_size;
    size_t                      convert_mmap_frame_size;
    unsigned char               *frame_pointers[V4L2_MAX_NO_FRAMES];
    int                         frame_sizes[V4L2_MAX_NO_FRAMES];
    int                         frame_queued;
    int                         frame_info_generation;
    unsigned char               frame_map_count[V4L2_MAX_NO_FRAMES];
    int                         readbuf_size;
    unsigned char               *readbuf;
    void                        *plugin_library;
    void                        *dev_ops_priv;
    const struct libv4l_dev_ops *dev_ops;
};

#if defined(HAVE_V4L_PLUGINS)
void v4l2_plugin_init(int fd, void **plugin_lib_ret, void **plugin_priv_ret, const struct libv4l_dev_ops **dev_ops_ret);
void v4l2_plugin_cleanup(void *plugin_lib, void *plugin_priv, const struct libv4l_dev_ops *dev_ops);
#else
static inline void v4l2_plugin_init(int fd, void **plugin_lib_ret, void **plugin_priv_ret, const struct libv4l_dev_ops **dev_ops_ret)
{
    *dev_ops_ret = v4lconvert_get_default_dev_ops();
    *plugin_lib_ret = NULL;
    *plugin_priv_ret = NULL;
}

static inline void v4l2_plugin_cleanup(void *plugin_lib, void *plugin_priv, const struct libv4l_dev_ops *dev_ops)
{

}
#endif

extern const char *v4l2_ioctls[];
void v4l2_log_ioctl(unsigned long int request, void *arg, int result);

#endif
