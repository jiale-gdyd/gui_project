#undef _FORTIFY_SOURCE
#undef _LARGEFILE_SOURCE
#undef _FILE_OFFSET_BITS

#define _LARGEFILE64_SOURCE     1

#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <libv4l/libv4l2.h>
#include <libv4l/videodev2.h>

#include "../config.h"
#include "../libv4lconvert/libv4lsyscall-priv.h"

#if HAVE_VISIBILITY
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC           __attribute__ ((visibility("default")))
#endif
#else
#ifndef LIBV4L_PUBLIC
#define LIBV4L_PUBLIC
#endif
#endif

LIBV4L_PUBLIC int open(const char *file, int oflag, ...)
{
    int fd;
    int v4l_device = 0;

    if (!strncmp(file, "/dev/video", 10) || !strncmp(file, "/dev/v4l/", 9)) {
        oflag = (oflag & ~O_ACCMODE) | O_RDWR;
        v4l_device = 1;
    }

    if (oflag & O_CREAT) {
        va_list ap;
        mode_t mode;

        va_start(ap, oflag);
        mode = va_arg(ap, PROMOTED_MODE_T);
        fd = SYS_OPEN(file, oflag, mode);
        va_end(ap);
    } else {
        fd = SYS_OPEN(file, oflag, 0);
    }

    if ((fd == -1) || !v4l_device) {
        return fd;
    }

    v4l2_fd_open(fd, 0);
    return fd;
}

LIBV4L_PUBLIC int open64(const char *file, int oflag, ...)
{
    int fd;

    if (oflag & O_CREAT) {
        va_list ap;
        mode_t mode;

        va_start(ap, oflag);
        mode = va_arg(ap, PROMOTED_MODE_T);
        fd = open(file, oflag | O_LARGEFILE, mode);
        va_end(ap);
    } else {
        fd = open(file, oflag | O_LARGEFILE);
    }

    return fd;
}

LIBV4L_PUBLIC int close(int fd)
{
    return v4l2_close(fd);
}

LIBV4L_PUBLIC int dup(int fd)
{
    return v4l2_dup(fd);
}

LIBV4L_PUBLIC int ioctl(int fd, unsigned long int request, ...)
{
    void *arg;
    va_list ap;

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    return v4l2_ioctl(fd, request, arg);
}

LIBV4L_PUBLIC ssize_t read(int fd, void *buffer, size_t n)
{
    return v4l2_read(fd, buffer, n);
}

LIBV4L_PUBLIC void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return v4l2_mmap(start, length, prot, flags, fd, offset);
}

LIBV4L_PUBLIC void *mmap64(void *start, size_t length, int prot, int flags, int fd, off64_t offset)
{
    return v4l2_mmap(start, length, prot, flags, fd, offset);
}

LIBV4L_PUBLIC int munmap(void *start, size_t length)
{
    return v4l2_munmap(start, length);
}
