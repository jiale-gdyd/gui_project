#ifndef EXYNOS_DRMIF_H_
#define EXYNOS_DRMIF_H_

#include <stdint.h>

#include "../xf86drm.h"
#include "exynos_drm.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define EXYNOS_EVENT_CONTEXT_VERSION        1

struct exynos_device {
    int fd;
};

struct exynos_bo {
    struct exynos_device *dev;
    uint32_t             handle;
    uint32_t             flags;
    size_t               size;
    void                 *vaddr;
    uint32_t             name;
};

struct exynos_event_context {
    drmEventContext base;
    int             version;

    void (*g2d_event_handler)(int fd, unsigned int cmdlist_no, unsigned int tv_sec, unsigned int tv_usec, void *user_data);
};

struct exynos_device * exynos_device_create(int fd);
void exynos_device_destroy(struct exynos_device *dev);

struct exynos_bo *exynos_bo_create(struct exynos_device *dev, size_t size, uint32_t flags);
int exynos_bo_get_info(struct exynos_device *dev, uint32_t handle, size_t *size, uint32_t *flags);
void exynos_bo_destroy(struct exynos_bo *bo);
struct exynos_bo *exynos_bo_from_name(struct exynos_device *dev, uint32_t name);
int exynos_bo_get_name(struct exynos_bo *bo, uint32_t *name);
uint32_t exynos_bo_handle(struct exynos_bo *bo);
void *exynos_bo_map(struct exynos_bo *bo);
int exynos_prime_handle_to_fd(struct exynos_device *dev, uint32_t handle, int *fd);
int exynos_prime_fd_to_handle(struct exynos_device *dev, int fd, uint32_t *handle);

int exynos_vidi_connection(struct exynos_device *dev, uint32_t connect, uint32_t ext, void *edid);

int exynos_handle_event(struct exynos_device *dev, struct exynos_event_context *ctx);

#if defined(__cplusplus)
}
#endif

#endif
