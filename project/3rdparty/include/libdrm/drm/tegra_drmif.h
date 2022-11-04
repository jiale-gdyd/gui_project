#ifndef __DRM_TEGRA_DRMIF_H__
#define __DRM_TEGRA_DRMIF_H__ 1

#include <stdint.h>
#include <stdlib.h>

#include "tegra_drm.h"

enum drm_tegra_class {
    DRM_TEGRA_HOST1X,
    DRM_TEGRA_GR2D,
    DRM_TEGRA_GR3D,
    DRM_TEGRA_VIC,
};

struct drm_tegra;
struct drm_tegra_bo;

void drm_tegra_close(struct drm_tegra *drm);
int drm_tegra_new(int fd, struct drm_tegra **drmp);


int drm_tegra_bo_new(struct drm_tegra *drm, uint32_t flags, uint32_t size, struct drm_tegra_bo **bop);
int drm_tegra_bo_wrap(struct drm_tegra *drm, uint32_t handle, uint32_t flags, uint32_t size, struct drm_tegra_bo **bop);

struct drm_tegra_bo *drm_tegra_bo_ref(struct drm_tegra_bo *bo);
void drm_tegra_bo_unref(struct drm_tegra_bo *bo);
int drm_tegra_bo_get_handle(struct drm_tegra_bo *bo, uint32_t *handle);
int drm_tegra_bo_map(struct drm_tegra_bo *bo, void **ptr);
int drm_tegra_bo_unmap(struct drm_tegra_bo *bo);

int drm_tegra_bo_get_name(struct drm_tegra_bo *bo, uint32_t *name);
int drm_tegra_bo_open(struct drm_tegra *drm, uint32_t name, uint32_t flags, struct drm_tegra_bo **bop);

int drm_tegra_bo_export(struct drm_tegra_bo *bo, uint32_t flags);
int drm_tegra_bo_import(struct drm_tegra *drm, int fd, struct drm_tegra_bo **bop);

struct drm_tegra_job;
struct drm_tegra_channel;
struct drm_tegra_mapping;
struct drm_tegra_pushbuf;
struct drm_tegra_syncpoint;

enum drm_tegra_sync_cond {
    DRM_TEGRA_SYNC_COND_IMMEDIATE,
    DRM_TEGRA_SYNC_COND_OP_DONE,
    DRM_TEGRA_SYNC_COND_RD_DONE,
    DRM_TEGRA_SYNC_COND_WR_SAFE,
    DRM_TEGRA_SYNC_COND_MAX,
};

struct drm_tegra_fence {
    struct drm_tegra *drm;
    uint32_t         syncpt;
    uint32_t         value;
};

int drm_tegra_channel_open(struct drm_tegra *drm, enum drm_tegra_class client, struct drm_tegra_channel **channelp);
int drm_tegra_channel_close(struct drm_tegra_channel *channel);

unsigned int drm_tegra_channel_get_version(struct drm_tegra_channel *channel);

int drm_tegra_channel_map(struct drm_tegra_channel *channel, struct drm_tegra_bo *bo, uint32_t flags, struct drm_tegra_mapping **mapp);
int drm_tegra_channel_unmap(struct drm_tegra_mapping *map);

int drm_tegra_job_new(struct drm_tegra_channel *channel, struct drm_tegra_job **jobp);
int drm_tegra_job_free(struct drm_tegra_job *job);

int drm_tegra_job_get_pushbuf(struct drm_tegra_job *job, struct drm_tegra_pushbuf **pushbufp);
int drm_tegra_job_submit(struct drm_tegra_job *job, struct drm_tegra_fence *fence);

int drm_tegra_job_wait(struct drm_tegra_job *job, unsigned long timeout);

int drm_tegra_pushbuf_begin(struct drm_tegra_pushbuf *pushbuf, unsigned int words, uint32_t **ptrp);
int drm_tegra_pushbuf_end(struct drm_tegra_pushbuf *pushbuf, uint32_t *ptr);

int drm_tegra_pushbuf_wait(struct drm_tegra_pushbuf *pushbuf, struct drm_tegra_syncpoint *syncpt, uint32_t value);

int drm_tegra_pushbuf_relocate(struct drm_tegra_pushbuf *pushbuf, uint32_t **ptrp, struct drm_tegra_mapping *target, unsigned long offset, unsigned int shift, uint32_t flags);
int drm_tegra_pushbuf_sync(struct drm_tegra_pushbuf *pushbuf, struct drm_tegra_syncpoint *syncpt, unsigned int count);
int drm_tegra_pushbuf_sync_cond(struct drm_tegra_pushbuf *pushbuf, uint32_t **ptrp, struct drm_tegra_syncpoint *syncpt, enum drm_tegra_sync_cond cond);

int drm_tegra_syncpoint_new(struct drm_tegra *drm, struct drm_tegra_syncpoint **syncptp);
int drm_tegra_syncpoint_free(struct drm_tegra_syncpoint *syncpt);
int drm_tegra_fence_wait(struct drm_tegra_fence *fence, unsigned long timeout);

#endif
