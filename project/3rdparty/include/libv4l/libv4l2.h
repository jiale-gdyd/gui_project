#ifndef LIBV4L_LIBV4L2_H
#define LIBV4L_LIBV4L2_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_VISIBILITY
#define LIBV4L_PUBLIC                   __attribute__ ((visibility("default")))
#else
#define LIBV4L_PUBLIC
#endif

#define V4L2_DISABLE_CONVERSION         0x01
#define V4L2_ENABLE_ENUM_FMT_EMULATION  0x02

LIBV4L_PUBLIC extern FILE *v4l2_log_file;

LIBV4L_PUBLIC int v4l2_open(const char *file, int oflag, ...);
LIBV4L_PUBLIC int v4l2_close(int fd);

LIBV4L_PUBLIC int v4l2_dup(int fd);
LIBV4L_PUBLIC int v4l2_ioctl(int fd, unsigned long int request, ...);

LIBV4L_PUBLIC ssize_t v4l2_read(int fd, void *buffer, size_t n);
LIBV4L_PUBLIC ssize_t v4l2_write(int fd, const void *buffer, size_t n);

LIBV4L_PUBLIC void *v4l2_mmap(void *start, size_t length, int prot, int flags, int fd, int64_t offset);
LIBV4L_PUBLIC int v4l2_munmap(void *_start, size_t length);

LIBV4L_PUBLIC int v4l2_set_control(int fd, int cid, int value);
LIBV4L_PUBLIC int v4l2_get_control(int fd, int cid);

LIBV4L_PUBLIC int v4l2_fd_open(int fd, int v4l2_flags);

#ifdef __cplusplus
}
#endif

#endif
