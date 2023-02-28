#ifndef LIBDRM_LIBSYNC_H
#define LIBDRM_LIBSYNC_H

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/ioctl.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef SYNC_IOC_MERGE
struct sync_merge_data {
    char     name[32];
    int32_t  fd2;
    int32_t  fence;
    uint32_t flags;
    uint32_t pad;
};
#define SYNC_IOC_MAGIC          '>'
#define SYNC_IOC_MERGE          _IOWR(SYNC_IOC_MAGIC, 3, struct sync_merge_data)
#endif

static inline int sync_wait(int fd, int timeout)
{
    int ret;
    struct pollfd fds = {0};

    fds.fd = fd;
    fds.events = POLLIN;

    do {
        ret = poll(&fds, 1, timeout);
        if (ret > 0) {
            if (fds.revents & (POLLERR | POLLNVAL)) {
                errno = EINVAL;
                return -1;
            }

            return 0;
        } else if (ret == 0) {
            errno = ETIME;
            return -1;
        }
    } while ((ret == -1) && ((errno == EINTR) || (errno == EAGAIN)));

    return ret;
}

static inline int sync_merge(const char *name, int fd1, int fd2)
{
    int ret;
    struct sync_merge_data data = {0};

    data.fd2 = fd2;
    strncpy(data.name, name, sizeof(data.name));

    do {
        ret = ioctl(fd1, SYNC_IOC_MERGE, &data);
    } while ((ret == -1) && ((errno == EINTR) || (errno == EAGAIN)));

    if (ret < 0) {
        return ret;
    }

    return data.fence;
}

static inline int sync_accumulate(const char *name, int *fd1, int fd2)
{
    int ret;

    assert(fd2 >= 0);

    if (*fd1 < 0) {
        *fd1 = dup(fd2);
        return 0;
    }

    ret = sync_merge(name, *fd1, fd2);
    if (ret < 0) {
        return ret;
    }

    close(*fd1);
    *fd1 = ret;

    return 0;
}

#if defined(__cplusplus)
}
#endif

#endif
