#ifndef OMAP_DRMIF_H_
#define OMAP_DRMIF_H_

#include <stdint.h>

#include "../xf86drm.h"
#include "omap_drm.h"

struct omap_bo;
struct omap_device;

struct omap_device *omap_device_new(int fd);
struct omap_device *omap_device_ref(struct omap_device *dev);

void omap_device_del(struct omap_device *dev);
int omap_get_param(struct omap_device *dev, uint64_t param, uint64_t *value);
int omap_set_param(struct omap_device *dev, uint64_t param, uint64_t value);

struct omap_bo *omap_bo_new(struct omap_device *dev, uint32_t size, uint32_t flags);
struct omap_bo *omap_bo_new_tiled(struct omap_device *dev, uint32_t width, uint32_t height, uint32_t flags);
struct omap_bo *omap_bo_ref(struct omap_bo *bo);
struct omap_bo *omap_bo_from_name(struct omap_device *dev, uint32_t name);
struct omap_bo *omap_bo_from_dmabuf(struct omap_device *dev, int fd);

void omap_bo_del(struct omap_bo *bo);
int omap_bo_get_name(struct omap_bo *bo, uint32_t *name);
uint32_t omap_bo_handle(struct omap_bo *bo);
int omap_bo_dmabuf(struct omap_bo *bo);
uint32_t omap_bo_size(struct omap_bo *bo);
void *omap_bo_map(struct omap_bo *bo);
int omap_bo_cpu_prep(struct omap_bo *bo, enum omap_gem_op op);
int omap_bo_cpu_fini(struct omap_bo *bo, enum omap_gem_op op);

#endif
