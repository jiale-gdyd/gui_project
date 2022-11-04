#ifndef _AMDGPU_INTERNAL_H_
#define _AMDGPU_INTERNAL_H_

#include <assert.h>
#include <pthread.h>

#include <libdrm/xf86atomic.h>
#include <libdrm/drm/amdgpu.h>
#include <libdrm/libdrm_macros.h>
#include <libdrm/util_double_list.h>

#include "handle_table.h"

#define AMDGPU_CS_MAX_RINGS                 8
#define __round_mask(x, y)                  ((__typeof__(x))((y)-1))
#define ROUND_UP(x, y)                      ((((x)-1) | __round_mask(x, y))+1)
#define ROUND_DOWN(x, y)                    ((x) & ~__round_mask(x, y))

#define AMDGPU_INVALID_VA_ADDRESS           0xffffffffffffffff
#define AMDGPU_NULL_SUBMIT_SEQ              0

struct amdgpu_bo_va_hole {
    struct list_head list;
    uint64_t         offset;
    uint64_t         size;
};

struct amdgpu_bo_va_mgr {
    uint64_t         va_max;
    struct list_head va_holes;
    pthread_mutex_t  bo_va_mutex;
    uint32_t         va_alignment;
};

struct amdgpu_va {
    amdgpu_device_handle     dev;
    uint64_t                 address;
    uint64_t                 size;
    enum amdgpu_gpu_va_range range;
    struct amdgpu_bo_va_mgr  *vamgr;
};

struct amdgpu_device {
    atomic_t                      refcount;
    struct amdgpu_device          *next;
    int                           fd;
    int                           flink_fd;
    unsigned                      major_version;
    unsigned                      minor_version;

    char                          *marketing_name;
    struct handle_table           bo_handles;
    struct handle_table           bo_flink_names;
    pthread_mutex_t               bo_table_mutex;
    struct drm_amdgpu_info_device dev_info;
    struct amdgpu_gpu_info        info;
    struct amdgpu_bo_va_mgr       vamgr;
    struct amdgpu_bo_va_mgr       vamgr_32;
    struct amdgpu_bo_va_mgr       vamgr_high;
    struct amdgpu_bo_va_mgr       vamgr_high_32;
};

struct amdgpu_bo {
    atomic_t             refcount;
    struct amdgpu_device *dev;
    uint64_t             alloc_size;
    uint32_t             handle;
    uint32_t             flink_name;
    pthread_mutex_t      cpu_access_mutex;
    void                 *cpu_ptr;
    int64_t              cpu_map_count;
};

struct amdgpu_bo_list {
    struct amdgpu_device *dev;
    uint32_t             handle;
};

struct amdgpu_context {
    struct amdgpu_device *dev;
    pthread_mutex_t      sequence_mutex;
    uint32_t             id;
    uint64_t             last_seq[AMDGPU_HW_IP_NUM][AMDGPU_HW_IP_INSTANCE_MAX_COUNT][AMDGPU_CS_MAX_RINGS];
    struct list_head     sem_list[AMDGPU_HW_IP_NUM][AMDGPU_HW_IP_INSTANCE_MAX_COUNT][AMDGPU_CS_MAX_RINGS];
};

struct amdgpu_semaphore {
    atomic_t               refcount;
    struct list_head       list;
    struct amdgpu_cs_fence signal_fence;
};

drm_private void amdgpu_vamgr_init(struct amdgpu_bo_va_mgr *mgr, uint64_t start, uint64_t max, uint64_t alignment);
drm_private void amdgpu_vamgr_deinit(struct amdgpu_bo_va_mgr *mgr);

drm_private void amdgpu_parse_asic_ids(struct amdgpu_device *dev);
drm_private int amdgpu_query_gpu_info_init(amdgpu_device_handle dev);

drm_private uint64_t amdgpu_cs_calculate_timeout(uint64_t timeout);

static inline bool update_references(atomic_t *dst, atomic_t *src)
{
    if (dst != src) {
        if (src) {
            assert(atomic_read(src) > 0);
            atomic_inc(src);
        }

        if (dst) {
            assert(atomic_read(dst) > 0);
            return atomic_dec_and_test(dst);
        }
    }

    return false;
}

#endif
