#include "etnaviv_priv.h"

drm_private void bo_del(struct etna_bo *bo);
drm_private extern pthread_mutex_t table_lock;

static void add_bucket(struct etna_bo_cache *cache, int size)
{
    unsigned i = cache->num_buckets;

    assert(i < ARRAY_SIZE(cache->cache_bucket));

    list_inithead(&cache->cache_bucket[i].list);
    cache->cache_bucket[i].size = size;
    cache->num_buckets++;
}

drm_private void etna_bo_cache_init(struct etna_bo_cache *cache)
{
    unsigned long size, cache_max_size = 64 * 1024 * 1024;

    add_bucket(cache, 4096);
    add_bucket(cache, 4096 * 2);
    add_bucket(cache, 4096 * 3);

    for (size = 4 * 4096; size <= cache_max_size; size *= 2) {
        add_bucket(cache, size);
        add_bucket(cache, size + size * 1 / 4);
        add_bucket(cache, size + size * 2 / 4);
        add_bucket(cache, size + size * 3 / 4);
    }
}

drm_private void etna_bo_cache_cleanup(struct etna_bo_cache *cache, time_t time)
{
    unsigned i;

    if (cache->time == time) {
        return;
    }

    for (i = 0; i < cache->num_buckets; i++) {
        struct etna_bo *bo;
        struct etna_bo_bucket *bucket = &cache->cache_bucket[i];

        while (!LIST_IS_EMPTY(&bucket->list)) {
            bo = LIST_ENTRY(struct etna_bo, bucket->list.next, list);
            if (time && ((time - bo->free_time) <= 1)) {
                break;
            }

            list_del(&bo->list);
            bo_del(bo);
        }
    }

    cache->time = time;
}

static struct etna_bo_bucket *get_bucket(struct etna_bo_cache *cache, uint32_t size)
{
    unsigned i;

    for (i = 0; i < cache->num_buckets; i++) {
        struct etna_bo_bucket *bucket = &cache->cache_bucket[i];
        if (bucket->size >= size) {
            return bucket;
        }
    }

    return NULL;
}

static int is_idle(struct etna_bo *bo)
{
    return etna_bo_cpu_prep(bo, DRM_ETNA_PREP_READ | DRM_ETNA_PREP_WRITE | DRM_ETNA_PREP_NOSYNC) == 0;
}

static struct etna_bo *find_in_bucket(struct etna_bo_bucket *bucket, uint32_t flags)
{
    struct etna_bo *bo = NULL, *tmp;

    pthread_mutex_lock(&table_lock);
    if (LIST_IS_EMPTY(&bucket->list)) {
        goto out_unlock;
    }

    LIST_FOR_EACH_ENTRY_SAFE(bo, tmp, &bucket->list, list) {
        if (bo->flags != flags) {
            continue;
        }

        if (is_idle(bo)) {
            list_delinit(&bo->list);
            goto out_unlock;
        }

        break;
    }

    bo = NULL;

out_unlock:
    pthread_mutex_unlock(&table_lock);
    return bo;
}

drm_private struct etna_bo *etna_bo_cache_alloc(struct etna_bo_cache *cache, uint32_t *size, uint32_t flags)
{
    struct etna_bo *bo;
    struct etna_bo_bucket *bucket;

    *size = ALIGN(*size, 4096);
    bucket = get_bucket(cache, *size);

    if (bucket) {
        *size = bucket->size;
        bo = find_in_bucket(bucket, flags);
        if (bo) {
            atomic_set(&bo->refcnt, 1);
            etna_device_ref(bo->dev);
            return bo;
        }
    }

    return NULL;
}

drm_private int etna_bo_cache_free(struct etna_bo_cache *cache, struct etna_bo *bo)
{
    struct etna_bo_bucket *bucket = get_bucket(cache, bo->size);

    if (bucket) {
        struct timespec time;

        clock_gettime(CLOCK_MONOTONIC, &time);

        bo->free_time = time.tv_sec;
        list_addtail(&bo->list, &bucket->list);
        etna_bo_cache_cleanup(cache, time.tv_sec);
        etna_device_del_locked(bo->dev);

        return 0;
    }

    return -1;
}
