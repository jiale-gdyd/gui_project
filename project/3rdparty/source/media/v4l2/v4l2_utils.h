#ifndef LIBDRM_V4L2_UTILS_H
#define LIBDRM_V4L2_UTILS_H

#include <linux/kconfig.h>

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <linux/videodev2.h>

#include <media/utils/utils.h>
#include <media/utils/key_string.h>
#include <media/utils/media_type.h>

#if defined(CONFIG_RKAIQ)
#include "rkaiq/rkaiq_media.h"
#endif

#ifndef V4L2_PIX_FMT_FBC2
#define V4L2_PIX_FMT_FBC2           v4l2_fourcc('F', 'B', 'C', '2')
#define V4L2_PIX_FMT_FBC0           v4l2_fourcc('F', 'B', 'C', '0')
#endif

#define DRIVER_NAME                 "rkispp"
#define MB_ENTITY_NAME              DRIVER_NAME "_m_bypass"
#define S0_ENTITY_NAME              DRIVER_NAME "_scale0"
#define S1_ENTITY_NAME              DRIVER_NAME "_scale1"
#define S2_ENTITY_NAME              DRIVER_NAME "_scale2"

namespace libdrm {
__u32 GetV4L2Type(const char *v4l2type);
__u32 GetV4L2FmtByString(const char *type);
__u32 GetV4L2ColorSpaceByString(const char *type);

const std::string &GetStringOfV4L2Fmts();

typedef struct {
    int (*open_f)(const char *file, int oflag, ...);
    int (*close_f)(int fd);
    int (*dup_f)(int fd);
    int (*ioctl_f)(int fd, unsigned long int request, ...);
    ssize_t (*read_f)(int fd, void *buffer, size_t n);
    void *(*mmap_f)(void *start, size_t length, int prot, int flags, int fd, int64_t offset);
    int (*munmap_f)(void *_start, size_t length);
} v4l2_io_t;

bool SetV4L2IoFunction(v4l2_io_t *vio, bool use_libv4l2 = false);
int V4L2IoCtl(v4l2_io_t *vio, int fd, unsigned long int request, void *arg);
}

#endif
