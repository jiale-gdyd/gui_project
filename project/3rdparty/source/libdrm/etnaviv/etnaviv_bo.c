#include "etnaviv_priv.h"

drm_private pthread_mutex_t table_lock = PTHREAD_MUTEX_INITIALIZER;
drm_private void bo_del(struct etna_bo *bo);

static void set_name(struct etna_bo *bo, uint32_t name)
{
    bo->name = name;
    drmHashInsert(bo->dev->name_table, name, bo);
}

drm_private void bo_del(struct etna_bo *bo)
{
    if (bo->map) {
        drm_munmap(bo->map, bo->size);
    }

    if (bo->name) {
        drmHashDelete(bo->dev->name_table, bo->name);
    }

    if (bo->handle) {
        drmHashDelete(bo->dev->handle_table, bo->handle);
        drmCloseBufferHandle(bo->dev->fd, bo->handle);
    }

    free(bo);
}

static struct etna_bo *lookup_bo(void *tbl, uint32_t handle)
{
    struct etna_bo *bo = NULL;

    if (!drmHashLookup(tbl, handle, (void **)&bo)) {
        bo = etna_bo_ref(bo);
        list_delinit(&bo->list);
    }

    return bo;
}

static struct etna_bo *bo_from_handle(struct etna_device *dev, uint32_t size, uint32_t handle, uint32_t flags)
{
    struct etna_bo *bo = calloc(sizeof(*bo), 1);
    if (!bo) {
        drmCloseBufferHandle(dev->fd, handle);
        return NULL;
    }

    bo->dev = etna_device_ref(dev);
    bo->size = size;
    bo->handle = handle;
    bo->flags = flags;
    atomic_set(&bo->refcnt, 1);
    list_inithead(&bo->list);

    drmHashInsert(dev->handle_table, handle, bo);

    return bo;
}

drm_public struct etna_bo *etna_bo_new(struct etna_device *dev, uint32_t size, uint32_t flags)
{
    int ret;
    struct etna_bo *bo;
    struct drm_etnaviv_gem_new req = {
        .flags = flags,
    };

    bo = etna_bo_cache_alloc(&dev->bo_cache, &size, flags);
    if (bo) {
        return bo;
    }

    req.size = size;
    ret = drmCommandWriteRead(dev->fd, DRM_ETNAVIV_GEM_NEW, &req, sizeof(req));
    if (ret) {
        return NULL;
    }

    pthread_mutex_lock(&table_lock);
    bo = bo_from_handle(dev, size, req.handle, flags);
    bo->reuse = 1;
    pthread_mutex_unlock(&table_lock);

    return bo;
}

drm_public struct etna_bo *etna_bo_ref(struct etna_bo *bo)
{
    atomic_inc(&bo->refcnt);
    return bo;
}

static int get_buffer_info(struct etna_bo *bo)
{
    int ret;
    struct drm_etnaviv_gem_info req = {
        .handle = bo->handle,
    };

    ret = drmCommandWriteRead(bo->dev->fd, DRM_ETNAVIV_GEM_INFO, &req, sizeof(req));
    if (ret) {
        return ret;
    }

    bo->offset = req.offset;

    return 0;
}

drm_public struct etna_bo *etna_bo_from_name(struct etna_device *dev, uint32_t name)
{
    struct etna_bo *bo;
    struct drm_gem_open req = {
        .name = name,
    };

    pthread_mutex_lock(&table_lock);
    bo = lookup_bo(dev->name_table, name);
    if (bo) {
        goto out_unlock;
    }

    if (drmIoctl(dev->fd, DRM_IOCTL_GEM_OPEN, &req)) {
        ERROR_MSG("gem-open failed: %s", strerror(errno));
        goto out_unlock;
    }

    bo = lookup_bo(dev->handle_table, req.handle);
    if (bo) {
        goto out_unlock;
    }

    bo = bo_from_handle(dev, req.size, req.handle, 0);
    if (bo) {
        set_name(bo, name);
    }

out_unlock:
    pthread_mutex_unlock(&table_lock);
    return bo;
}

drm_public struct etna_bo *etna_bo_from_dmabuf(struct etna_device *dev, int fd)
{
    int ret, size;
    uint32_t handle;
    struct etna_bo *bo;

    pthread_mutex_lock(&table_lock);
    ret = drmPrimeFDToHandle(dev->fd, fd, &handle);
    if (ret) {
        pthread_mutex_unlock(&table_lock);
        return NULL;
    }

    bo = lookup_bo(dev->handle_table, handle);
    if (bo) {
        goto out_unlock;
    }

    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_CUR);

    bo = bo_from_handle(dev, size, handle, 0);

out_unlock:
    pthread_mutex_unlock(&table_lock);
    return bo;
}

drm_public void etna_bo_del(struct etna_bo *bo)
{
    struct etna_device *dev = bo->dev;
    if (!bo) {
        return;
    }

    if (!atomic_dec_and_test(&bo->refcnt)) {
        return;
    }

    pthread_mutex_lock(&table_lock);

    if (bo->reuse && (etna_bo_cache_free(&dev->bo_cache, bo) == 0)) {
        goto out;
    }

    bo_del(bo);
    etna_device_del_locked(dev);

out:
    pthread_mutex_unlock(&table_lock);
}

drm_public int etna_bo_get_name(struct etna_bo *bo, uint32_t *name)
{
    if (!bo->name) {
        int ret;
        struct drm_gem_flink req = {
            .handle = bo->handle,
        };

        ret = drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_FLINK, &req);
        if (ret) {
            return ret;
        }

        pthread_mutex_lock(&table_lock);
        set_name(bo, req.name);
        pthread_mutex_unlock(&table_lock);
        bo->reuse = 0;
    }

    *name = bo->name;
    return 0;
}

drm_public uint32_t etna_bo_handle(struct etna_bo *bo)
{
    return bo->handle;
}

drm_public int etna_bo_dmabuf(struct etna_bo *bo)
{
    int ret, prime_fd;

    ret = drmPrimeHandleToFD(bo->dev->fd, bo->handle, DRM_CLOEXEC, &prime_fd);
    if (ret) {
        ERROR_MSG("failed to get dmabuf fd: %d", ret);
        return ret;
    }

    bo->reuse = 0;
    return prime_fd;
}

drm_public uint32_t etna_bo_size(struct etna_bo *bo)
{
    return bo->size;
}

drm_public void *etna_bo_map(struct etna_bo *bo)
{
    if (!bo->map) {
        if (!bo->offset) {
            get_buffer_info(bo);
        }

        bo->map = drm_mmap(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED, bo->dev->fd, bo->offset);
        if (bo->map == MAP_FAILED) {
            ERROR_MSG("mmap failed: %s", strerror(errno));
            bo->map = NULL;
        }
    }

    return bo->map;
}

drm_public int etna_bo_cpu_prep(struct etna_bo *bo, uint32_t op)
{
    struct drm_etnaviv_gem_cpu_prep req = {
        .handle = bo->handle,
        .op     = op,
    };

    get_abs_timeout(&req.timeout, 5000000000);
    return drmCommandWrite(bo->dev->fd, DRM_ETNAVIV_GEM_CPU_PREP, &req, sizeof(req));
}

drm_public void etna_bo_cpu_fini(struct etna_bo *bo)
{
    struct drm_etnaviv_gem_cpu_fini req = {
        .handle = bo->handle,
    };

    drmCommandWrite(bo->dev->fd, DRM_ETNAVIV_GEM_CPU_FINI, &req, sizeof(req));
}
