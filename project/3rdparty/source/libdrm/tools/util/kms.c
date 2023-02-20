#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>
#include <libdrm/util/common.h>

struct type_name {
    unsigned int type;
    const char   *name;
};

static const char *util_lookup_type_name(unsigned int type, const struct type_name *table, unsigned int count)
{
    unsigned int i;

    for (i = 0; i < count; i++) {
        if (table[i].type == type) {
            return table[i].name;
        }
    }

    return NULL;
}

static const struct type_name encoder_type_names[] = {
    { DRM_MODE_ENCODER_NONE,    "none" },
    { DRM_MODE_ENCODER_DAC,     "DAC" },
    { DRM_MODE_ENCODER_TMDS,    "TMDS" },
    { DRM_MODE_ENCODER_LVDS,    "LVDS" },
    { DRM_MODE_ENCODER_TVDAC,   "TVDAC" },
    { DRM_MODE_ENCODER_VIRTUAL, "Virtual" },
    { DRM_MODE_ENCODER_DSI,     "DSI" },
    { DRM_MODE_ENCODER_DPMST,   "DPMST" },
    { DRM_MODE_ENCODER_DPI,     "DPI" },
};

const char *util_lookup_encoder_type_name(unsigned int type)
{
    return util_lookup_type_name(type, encoder_type_names, ARRAY_SIZE(encoder_type_names));
}

static const struct type_name connector_status_names[] = {
    { DRM_MODE_CONNECTED,         "connected" },
    { DRM_MODE_DISCONNECTED,      "disconnected" },
    { DRM_MODE_UNKNOWNCONNECTION, "unknown" },
};

const char *util_lookup_connector_status_name(unsigned int status)
{
    return util_lookup_type_name(status, connector_status_names, ARRAY_SIZE(connector_status_names));
}

static const char *const modules[] = {
    "i915",
    "amdgpu",
    "radeon",
    "nouveau",
    "vmwgfx",
    "omapdrm",
    "exynos",
    "tilcdc",
    "msm",
    "sti",
    "tegra",
    "imx-drm",
    "rockchip",
    "atmel-hlcdc",
    "fsl-dcu-drm",
    "vc4",
    "virtio_gpu",
    "mediatek",
    "meson",
    "pl111",
    "stm",
    "sun4i-drm",
    "armada-drm",
    "komeda",
    "imx-dcss",
    "mxsfb-drm",
    "simpledrm",
    "imx-lcdif",
    "vkms",
};

int util_open(const char *device, const char *module)
{
    int fd;

    if (module) {
        fd = drmOpen(module, device);
        if (fd < 0) {
            fprintf(stderr, "failed to open device '%s': %s\n", module, strerror(errno));
            return -errno;
        }
    } else {
        unsigned int i;

        for (i = 0; i < ARRAY_SIZE(modules); i++) {
            printf("trying to open device '%s'...", modules[i]);

            fd = drmOpen(modules[i], device);
            if (fd < 0) {
                printf("failed\n");
            } else {
                printf("done\n");
                break;
            }
        }

        if (fd < 0) {
            fprintf(stderr, "no device found\n");
            return -ENODEV;
        }
    }

    return fd;
}
