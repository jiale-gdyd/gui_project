#ifndef LIBV4L_LIBV4L_PLUGIN_H
#define LIBV4L_LIBV4L_PLUGIN_H

#include <sys/types.h>

struct libv4l_dev_ops {
    void * (*init)(int fd);
    void (*close)(void *dev_ops_priv);

    int (*ioctl)(void *dev_ops_priv, int fd, unsigned long int request, void *arg);

    ssize_t (*read)(void *dev_ops_priv, int fd, void *buffer, size_t n);
    ssize_t (*write)(void *dev_ops_priv, int fd, const void *buffer, size_t n);

    void *(*mmap)(void *dev_ops_priv, void *start, size_t length, int prot, int flags, int fd, int64_t offset);

    void (*reserved1)(void);
    void (*reserved2)(void);
    void (*reserved3)(void);
    void (*reserved4)(void);
    void (*reserved5)(void);
    void (*reserved6)(void);
    void (*reserved7)(void);
};

#endif
