#ifndef __OMAP_DRM_H__
#define __OMAP_DRM_H__

#include <stdint.h>
#include <libdrm/drm/drm.h>

#define OMAP_PARAM_CHIPSET_ID               1

struct drm_omap_param {
    uint64_t param;
    uint64_t value;
};

struct drm_omap_get_base {
    char     plugin_name[64];
    uint32_t ioctl_base;
    uint32_t __pad;
};

#define OMAP_BO_SCANOUT                     0x00000001
#define OMAP_BO_CACHE_MASK                  0x00000006
#define OMAP_BO_TILED_MASK                  0x00000f00

#define OMAP_BO_CACHED                      0x00000000
#define OMAP_BO_WC                          0x00000002
#define OMAP_BO_UNCACHED                    0x00000004

#define OMAP_BO_TILED_8                     0x00000100
#define OMAP_BO_TILED_16                    0x00000200
#define OMAP_BO_TILED_32                    0x00000300
#define OMAP_BO_TILED                       (OMAP_BO_TILED_8 | OMAP_BO_TILED_16 | OMAP_BO_TILED_32)

union omap_gem_size {
    uint32_t     bytes;
    struct {
        uint16_t width;
        uint16_t height;
    } tiled;
};

struct drm_omap_gem_new {
    union omap_gem_size size;
    uint32_t            flags;
    uint32_t            handle;
    uint32_t            __pad;
};

enum omap_gem_op {
    OMAP_GEM_READ  = 0x01,
    OMAP_GEM_WRITE = 0x02,
};

struct drm_omap_gem_cpu_prep {
    uint32_t handle;
    uint32_t op;
};

struct drm_omap_gem_cpu_fini {
    uint32_t handle;
    uint32_t op;
    uint32_t nregions;
    uint32_t __pad;
};

struct drm_omap_gem_info {
    uint32_t handle;
    uint32_t pad;
    uint64_t offset;
    uint32_t size;
    uint32_t __pad;
};

#define DRM_OMAP_GET_PARAM                  0x00
#define DRM_OMAP_SET_PARAM                  0x01
#define DRM_OMAP_GET_BASE                   0x02
#define DRM_OMAP_GEM_NEW                    0x03
#define DRM_OMAP_GEM_CPU_PREP               0x04
#define DRM_OMAP_GEM_CPU_FINI               0x05
#define DRM_OMAP_GEM_INFO                   0x06
#define DRM_OMAP_NUM_IOCTLS                 0x07

#define DRM_IOCTL_OMAP_GET_PARAM            DRM_IOWR(DRM_COMMAND_BASE + DRM_OMAP_GET_PARAM, struct drm_omap_param)
#define DRM_IOCTL_OMAP_SET_PARAM            DRM_IOW(DRM_COMMAND_BASE + DRM_OMAP_SET_PARAM, struct drm_omap_param)
#define DRM_IOCTL_OMAP_GET_BASE             DRM_IOWR(DRM_COMMAND_BASE + DRM_OMAP_GET_BASE, struct drm_omap_get_base)
#define DRM_IOCTL_OMAP_GEM_NEW              DRM_IOWR(DRM_COMMAND_BASE + DRM_OMAP_GEM_NEW, struct drm_omap_gem_new)
#define DRM_IOCTL_OMAP_GEM_CPU_PREP         DRM_IOW(DRM_COMMAND_BASE + DRM_OMAP_GEM_CPU_PREP, struct drm_omap_gem_cpu_prep)
#define DRM_IOCTL_OMAP_GEM_CPU_FINI         DRM_IOW(DRM_COMMAND_BASE + DRM_OMAP_GEM_CPU_FINI, struct drm_omap_gem_cpu_fini)
#define DRM_IOCTL_OMAP_GEM_INFO             DRM_IOWR(DRM_COMMAND_BASE + DRM_OMAP_GEM_INFO, struct drm_omap_gem_info)

#endif
