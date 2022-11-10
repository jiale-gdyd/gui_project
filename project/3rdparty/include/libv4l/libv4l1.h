#ifndef LIBV4L_LIBV4L1_H
#define LIBV4L_LIBV4L1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "libv4l1-videodev.h"

#if HAVE_VISIBILITY
#define LIBV4L_PUBLIC __attribute__ ((visibility("default")))
#else
#define LIBV4L_PUBLIC
#endif

LIBV4L_PUBLIC extern FILE *v4l1_log_file;

LIBV4L_PUBLIC int v4l1_open(const char *file, int oflag, ...);
LIBV4L_PUBLIC int v4l1_close(int fd);

LIBV4L_PUBLIC int v4l1_dup(int fd);
LIBV4L_PUBLIC int v4l1_ioctl(int fd, unsigned long int request, ...);

LIBV4L_PUBLIC ssize_t v4l1_read(int fd, void *buffer, size_t n);

LIBV4L_PUBLIC void *v4l1_mmap(void *start, size_t length, int prot, int flags, int fd, int64_t offset);
LIBV4L_PUBLIC int v4l1_munmap(void *_start, size_t length);

#ifdef __cplusplus
}
#endif

#endif
