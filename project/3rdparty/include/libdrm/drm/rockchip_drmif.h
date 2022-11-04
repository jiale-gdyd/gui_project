#ifndef LIBDRM_ROCKCHIP_DRMIF_H
#define LIBDRM_ROCKCHIP_DRMIF_H

#include <stdint.h>
#include <libdrm/xf86drm.h>

#include "rockchip_drm.h"

struct rockchip_device {
    int fd;
};

struct rockchip_bo {
    struct rockchip_device *dev;
    uint32_t               handle;
    uint32_t               flags;
    size_t                 size;
    void                   *vaddr;
    uint32_t               name;
};

struct rockchip_device *rockchip_device_create(int fd);
void rockchip_device_destroy(struct rockchip_device *dev);

struct rockchip_bo *rockchip_bo_create(struct rockchip_device *dev, size_t size, uint32_t flags);

int rockchip_bo_get_info(struct rockchip_device *dev, uint32_t handle, size_t *size, uint32_t *flags);
void rockchip_bo_destroy(struct rockchip_bo *bo);

struct rockchip_bo *rockchip_bo_from_name(struct rockchip_device *dev, uint32_t name);
int rockchip_bo_get_name(struct rockchip_bo *bo, uint32_t *name);

uint32_t rockchip_bo_handle(struct rockchip_bo *bo);
struct rockchip_bo *rockchip_bo_from_handle(struct rockchip_device *dev, uint32_t handle, uint32_t flags, uint32_t size);

void *rockchip_bo_map(struct rockchip_bo *bo);

#endif
