#ifndef ETNAVIV_PRIV_H_
#define ETNAVIV_PRIV_H_

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86atomic.h>
#include <libdrm/libdrm_macros.h>
#include <libdrm/drm/etnaviv_drm.h>
#include <libdrm/util_double_list.h>
#include <libdrm/drm/etnaviv_drmif.h>

struct etna_bo_bucket {
    uint32_t         size;
    struct list_head list;
};

struct etna_bo_cache {
    struct etna_bo_bucket cache_bucket[14 * 4];
    unsigned              num_buckets;
    time_t                time;
};

struct etna_device {
    int                  fd;
    atomic_t             refcnt;
    void                 *handle_table, *name_table;
    struct etna_bo_cache bo_cache;
    int                  closefd;
};

drm_private void etna_bo_cache_init(struct etna_bo_cache *cache);
drm_private void etna_bo_cache_cleanup(struct etna_bo_cache *cache, time_t time);
drm_private struct etna_bo *etna_bo_cache_alloc(struct etna_bo_cache *cache, uint32_t *size, uint32_t flags);
drm_private int etna_bo_cache_free(struct etna_bo_cache *cache, struct etna_bo *bo);

drm_private void etna_device_del_locked(struct etna_device *dev);

struct etna_bo {
    struct etna_device     *dev;
    void                   *map;
    uint32_t               size;
    uint32_t               handle;
    uint32_t               flags;
    uint32_t               name;
    uint64_t               offset;
    atomic_t               refcnt;
    struct etna_cmd_stream *current_stream;
    uint32_t               idx;
    int                    reuse;
    struct list_head       list;
    time_t                 free_time;
};

struct etna_gpu {
    struct etna_device *dev;
    uint32_t           core;
    uint32_t           model;
    uint32_t           revision;
};

struct etna_pipe {
    enum etna_pipe_id id;
    struct etna_gpu   *gpu;
};

struct etna_cmd_stream_priv {
    struct etna_cmd_stream base;
    struct etna_pipe       *pipe;
    uint32_t               last_timestamp;

    struct {
        struct drm_etnaviv_gem_submit_bo    *bos;
        uint32_t                            nr_bos, max_bos;
        struct drm_etnaviv_gem_submit_reloc *relocs;
        uint32_t                            nr_relocs, max_relocs;
        struct drm_etnaviv_gem_submit_pmr   *pmrs;
        uint32_t                            nr_pmrs, max_pmrs;
    } submit;

    struct etna_bo **bos;
    uint32_t       nr_bos, max_bos;

    void (*reset_notify)(struct etna_cmd_stream *stream, void *priv);
    void *reset_notify_priv;
};

struct etna_perfmon {
    struct list_head domains;
    struct etna_pipe *pipe;
};

struct etna_perfmon_domain {
    struct list_head head;
    struct list_head signals;
    uint8_t          id;
    char             name[64];
};

struct etna_perfmon_signal {
    struct list_head           head;
    struct etna_perfmon_domain *domain;
    uint8_t                    signal;
    char                       name[64];
};

#ifndef ALIGN
#define ALIGN(v, a)                     (((v) + (a) - 1) & ~((a) - 1))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof((arr)[0]))
#endif

#define enable_debug                    1

#define INFO_MSG(fmt, ...)                                                          \
    do {                                                                            \
        drmMsg("[I] "fmt " (%s:%d)\n", ##__VA_ARGS__, __FUNCTION__, __LINE__);      \
    } while (0)

#define DEBUG_MSG(fmt, ...)                                                         \
    do {                                                                            \
        if (enable_debug) {                                                         \
            drmMsg("[D] "fmt " (%s:%d)\n", ##__VA_ARGS__, __FUNCTION__, __LINE__);  \
        }                                                                           \
    } while (0)

#define WARN_MSG(fmt, ...)                                                          \
    do {                                                                            \
        drmMsg("[W] "fmt " (%s:%d)\n", ##__VA_ARGS__, __FUNCTION__, __LINE__);      \
    } while (0)

#define ERROR_MSG(fmt, ...)                                                         \
    do {                                                                            \
        drmMsg("[E] " fmt " (%s:%d)\n", ##__VA_ARGS__, __FUNCTION__, __LINE__);     \
    } while (0)

#define VOID2U64(x)                     ((uint64_t)(unsigned long)(x))

static inline void get_abs_timeout(struct drm_etnaviv_timespec *tv, uint64_t ns)
{
    struct timespec t;
    uint32_t s = ns / 1000000000;

    clock_gettime(CLOCK_MONOTONIC, &t);
    tv->tv_sec = t.tv_sec + s;
    tv->tv_nsec = t.tv_nsec + ns - (s * 1000000000);
}

#endif
