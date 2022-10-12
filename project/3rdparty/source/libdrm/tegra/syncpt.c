#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#include "private.h"

drm_public int drm_tegra_syncpoint_new(struct drm_tegra *drm, struct drm_tegra_syncpoint **syncptp)
{
    int err;
    struct drm_tegra_syncpoint *syncpt;
    struct drm_tegra_syncpoint_allocate args;

    syncpt = (struct drm_tegra_syncpoint *)calloc(1, sizeof(*syncpt));
    if (!syncpt) {
        return -ENOMEM;
    }

    memset(&args, 0, sizeof(args));

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_SYNCPOINT_ALLOCATE, &args);
    if (err < 0) {
        free(syncpt);
        return -errno;
    }

    syncpt->drm = drm;
    syncpt->id = args.id;
    *syncptp = syncpt;

    return 0;
}

drm_public int drm_tegra_syncpoint_free(struct drm_tegra_syncpoint *syncpt)
{
    int err;
    struct drm_tegra *drm = syncpt->drm;
    struct drm_tegra_syncpoint_free args;

    if (!syncpt) {
        return -EINVAL;
    }

    memset(&args, 0, sizeof(args));
    args.id = syncpt->id;

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_SYNCPOINT_FREE, &args);
    if (err < 0) {
        return -errno;
    }
    free(syncpt);

    return 0;
}

drm_public int drm_tegra_fence_wait(struct drm_tegra_fence *fence, unsigned long timeout)
{
    int err;
    struct drm_tegra *drm = fence->drm;
    struct drm_tegra_syncpoint_wait args;

    memset(&args, 0, sizeof(args));
    args.timeout_ns = 0;
    args.id = fence->syncpt;
    args.threshold = fence->value;

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_SYNCPOINT_WAIT, &args);
    if (err < 0) {
        return -errno;
    }

    return 0;
}
