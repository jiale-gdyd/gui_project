#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/stddef.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86atomic.h>

#include "etnaviv_priv.h"

static pthread_mutex_t table_lock = PTHREAD_MUTEX_INITIALIZER;

drm_public struct etna_device *etna_device_new(int fd)
{
    struct etna_device *dev = calloc(sizeof(*dev), 1);
    if (!dev) {
        return NULL;
    }

    atomic_set(&dev->refcnt, 1);
    dev->fd = fd;
    dev->handle_table = drmHashCreate();
    dev->name_table = drmHashCreate();
    etna_bo_cache_init(&dev->bo_cache);

    return dev;
}

drm_public struct etna_device *etna_device_new_dup(int fd)
{
    int dup_fd = dup(fd);
    struct etna_device *dev = etna_device_new(dup_fd);

    if (dev) {
        dev->closefd = 1;
    } else {
        close(dup_fd);
    }

    return dev;
}

drm_public struct etna_device *etna_device_ref(struct etna_device *dev)
{
    atomic_inc(&dev->refcnt);
    return dev;
}

static void etna_device_del_impl(struct etna_device *dev)
{
    etna_bo_cache_cleanup(&dev->bo_cache, 0);
    drmHashDestroy(dev->handle_table);
    drmHashDestroy(dev->name_table);

    if (dev->closefd) {
        close(dev->fd);
    }
    free(dev);
}

drm_private void etna_device_del_locked(struct etna_device *dev)
{
    if (!atomic_dec_and_test(&dev->refcnt)) {
        return;
    }

    etna_device_del_impl(dev);
}

drm_public void etna_device_del(struct etna_device *dev)
{
    if (!atomic_dec_and_test(&dev->refcnt)) {
        return;
    }

    pthread_mutex_lock(&table_lock);
    etna_device_del_impl(dev);
    pthread_mutex_unlock(&table_lock);
}

drm_public int etna_device_fd(struct etna_device *dev)
{
    return dev->fd;
}
