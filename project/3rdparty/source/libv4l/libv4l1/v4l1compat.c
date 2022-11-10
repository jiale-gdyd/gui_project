#undef _LARGEFILE_SOURCE
#undef _FILE_OFFSET_BITS

#define _LARGEFILE64_SOURCE     1

#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <libv4l/libv4l1.h>

#include "../config.h"

#if HAVE_VISIBILITY
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC               __attribute__ ((visibility("default")))
#endif
#else
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC
#endif
#endif

LIBV4L_PUBLIC int open(const char *file, int oflag, ...)
{
    int fd;

    if (oflag & O_CREAT) {
        va_list ap;
        mode_t mode;

        va_start(ap, oflag);
        mode = va_arg(ap, PROMOTED_MODE_T);
        fd = v4l1_open(file, oflag, mode);
        va_end(ap);
    } else {
        fd = v4l1_open(file, oflag);
    }

    return fd;
}

LIBV4L_PUBLIC int open64(const char *file, int oflag, ...)
{
    int fd;

    if (oflag & O_CREAT) {
        va_list ap;
        mode_t mode;

        va_start(ap, oflag);
        mode = va_arg(ap, mode_t);
        fd = v4l1_open(file, oflag | O_LARGEFILE, mode);
        va_end(ap);
    } else {
        fd = v4l1_open(file, oflag | O_LARGEFILE);
    }

    return fd;
}

LIBV4L_PUBLIC int close(int fd)
{
    return v4l1_close(fd);
}

LIBV4L_PUBLIC int dup(int fd)
{
    return v4l1_dup(fd);
}

LIBV4L_PUBLIC int ioctl(int fd, unsigned long int request, ...)
{
    void *arg;
    va_list ap;

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    return v4l1_ioctl(fd, request, arg);
}

LIBV4L_PUBLIC ssize_t read(int fd, void *buffer, size_t n)
{
    return v4l1_read(fd, buffer, n);
}

LIBV4L_PUBLIC void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return v4l1_mmap(start, length, prot, flags, fd, offset);
}

LIBV4L_PUBLIC void *mmap64(void *start, size_t length, int prot, int flags, int fd, __off64_t offset)
{
    return v4l1_mmap(start, length, prot, flags, fd, offset);
}

LIBV4L_PUBLIC int munmap(void *start, size_t length)
{
    return v4l1_munmap(start, length);
}
