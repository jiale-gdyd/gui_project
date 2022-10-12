#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#include "private.h"

drm_public int drm_tegra_channel_open(struct drm_tegra *drm, enum drm_tegra_class client, struct drm_tegra_channel **channelp)
{
    int err;
    enum host1x_class classe;
    struct drm_tegra_channel *channel;
    struct drm_tegra_channel_open args;

    switch (client) {
        case DRM_TEGRA_HOST1X:
            classe = HOST1X_CLASS_HOST1X;
            break;

        case DRM_TEGRA_GR2D:
            classe = HOST1X_CLASS_GR2D;
            break;

        case DRM_TEGRA_GR3D:
            classe = HOST1X_CLASS_GR3D;
            break;

        case DRM_TEGRA_VIC:
            classe = HOST1X_CLASS_VIC;
            break;

        default:
            return -EINVAL;
    }

    channel = (struct drm_tegra_channel *)calloc(1, sizeof(*channel));
    if (!channel) {
        return -ENOMEM;
    }
    channel->drm = drm;

    memset(&args, 0, sizeof(args));
    args.host1x_class = classe;

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_CHANNEL_OPEN, &args);
    if (err < 0) {
        free(channel);
        return -errno;
    }

    channel->context = args.context;
    channel->version = args.version;
    channel->capabilities = args.capabilities;
    channel->classe = classe;

    switch (channel->version) {
        case 0x20:
        case 0x30:
        case 0x35:
        case 0x40:
        case 0x21:
            channel->cond_shift = 8;
            break;

        case 0x18:
        case 0x19:
            channel->cond_shift = 10;
            break;

        default:
            return -ENOTSUP;
    }

    *channelp = channel;
    return 0;
}

drm_public int drm_tegra_channel_close(struct drm_tegra_channel *channel)
{
    int err;
    struct drm_tegra *drm;
    struct drm_tegra_channel_close args;

    if (!channel) {
        return -EINVAL;
    }

    drm = channel->drm;
    memset(&args, 0, sizeof(args));
    args.context = channel->context;

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_CHANNEL_CLOSE, &args);
    if (err < 0) {
        return -errno;
    }
    free(channel);

    return 0;
}

drm_public unsigned int drm_tegra_channel_get_version(struct drm_tegra_channel *channel)
{
    return channel->version;
}

drm_public int drm_tegra_channel_map(struct drm_tegra_channel *channel, struct drm_tegra_bo *bo, uint32_t flags, struct drm_tegra_mapping **mapp)
{
    int err;
    struct drm_tegra_mapping *map;
    struct drm_tegra_channel_map args;
    struct drm_tegra *drm = channel->drm;

    if (!drm || !bo || !mapp) {
        return -EINVAL;
    }

    map = (struct drm_tegra_mapping *)calloc(1, sizeof(*map));
    if (!map) {
        return -ENOMEM;
    }

    memset(&args, 0, sizeof(args));
    args.context = channel->context;
    args.handle = bo->handle;
    args.flags = flags;

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_CHANNEL_MAP, &args);
    if (err < 0) {
        free(map);
        return -errno;
    }

    map->channel = channel;
    map->id = args.mapping;
    *mapp = map;

    return 0;
}

drm_public int drm_tegra_channel_unmap(struct drm_tegra_mapping *map)
{
    int err;
    struct drm_tegra_channel_unmap args;
    struct drm_tegra_channel *channel = map->channel;
    struct drm_tegra *drm = channel->drm;

    if (!channel || !map) {
        return -EINVAL;
    }

    memset(&args, 0, sizeof(args));
    args.context = channel->context;
    args.mapping = map->id;

    err = ioctl(drm->fd, DRM_IOCTL_TEGRA_CHANNEL_UNMAP, &args);
    if (err < 0) {
        return -errno;
    }
    free(map);

    return 0;
}
