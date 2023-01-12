#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/sync_file.h>

#ifndef __cplusplus
#include <stdatomic.h>
#else
#include <atomic>
#define _Atomic(X)      std::atomic<X>
using namespace std;
#endif

#include <rockchip/rkrgax/rga_sync.h>

struct sync_legacy_merge_data {
    int32_t fd2;
    char    name[32];
    int32_t fence;
};

#define SYNC_IOC_LEGACY_MERGE   _IOWR(SYNC_IOC_MAGIC, 1, struct sync_legacy_merge_data)

enum uapi_version {
    UAPI_UNKNOWN,
    UAPI_MODERN,
    UAPI_LEGACY
};

static atomic_int g_uapi_version = ATOMIC_VAR_INIT(UAPI_UNKNOWN);

static size_t rga_strlcpy(char *dest, const char *src, size_t size)
{
    size_t len = 0;
    size_t src_len = strlen(src);

    if (size) {
        len = src_len >= size ? (size - 1) : src_len;
        memcpy(dest, src, len);
        dest[len] = '\0';
    }

    return src_len;
}

int rga_sync_wait(int fd, int timeout)
{
    int ret;
    struct pollfd fds;

    if (fd < 0) {
        errno = EINVAL;
        return -1;
    }

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

static int legacy_sync_merge(const char *name, int fd1, int fd2)
{
    int ret;
    struct sync_legacy_merge_data data;

    data.fd2 = fd2;
    rga_strlcpy(data.name, name, sizeof(data.name));
    ret = ioctl(fd1, SYNC_IOC_LEGACY_MERGE, &data);
    if (ret < 0) {
        return ret;
    }

    return data.fence;
}

static int modern_sync_merge(const char *name, int fd1, int fd2)
{
    int ret;
    struct sync_merge_data data;

    data.fd2 = fd2;
    rga_strlcpy(data.name, name, sizeof(data.name));
    data.flags = 0;
    data.pad = 0;

    ret = ioctl(fd1, SYNC_IOC_MERGE, &data);
    if (ret < 0) {
        return ret;
    }

    return data.fence;
}

int rga_sync_merge(const char *name, int fd1, int fd2)
{
    int ret;
    int uapi;

    uapi = atomic_load_explicit(&g_uapi_version, memory_order_acquire);

    if (uapi == UAPI_MODERN || uapi == UAPI_UNKNOWN) {
        ret = modern_sync_merge(name, fd1, fd2);
        if ((ret >= 0) || (errno != ENOTTY)) {
            if ((ret >= 0) && (uapi == UAPI_UNKNOWN)) {
                atomic_store_explicit(&g_uapi_version, (int)UAPI_MODERN, memory_order_release);
            }

            return ret;
        }
    }

    ret = legacy_sync_merge(name, fd1, fd2);
    if ((ret >= 0) && (uapi == UAPI_UNKNOWN)) {
        atomic_store_explicit(&g_uapi_version, (int)UAPI_LEGACY, memory_order_release);
    }

    return ret;
}
