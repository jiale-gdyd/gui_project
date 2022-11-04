#ifndef LIBDRM_ROCKCHIP_DRM_H
#define LIBDRM_ROCKCHIP_DRM_H

#include <stdint.h>
#include "drm.h"

struct drm_rockchip_gem_create {
    uint64_t size;
    uint32_t flags;
    uint32_t handle;
};

struct drm_rockchip_gem_map_off {
    uint32_t handle;
    uint32_t pad;
    uint64_t offset;
};

struct drm_rockchip_rga_get_ver {
    __u32 major;
    __u32 minor;
};

struct drm_rockchip_rga_cmd {
    __u32 offset;
    __u32 data;
};

enum drm_rockchip_rga_buf_type {
    RGA_BUF_TYPE_USERPTR = 1 << 31,
    RGA_BUF_TYPE_GEMFD   = 1 << 30,
};

struct drm_rockchip_rga_userptr {
    unsigned long userptr;
    unsigned long size;
};

struct drm_rockchip_rga_set_cmdlist {
    __u64 cmd;
    __u64 cmd_buf;
    __u32 cmd_nr;
    __u32 cmd_buf_nr;
    __u64 user_data;
};

struct drm_rockchip_rga_exec {
    __u64 async;
};

#define DRM_ROCKCHIP_GEM_CREATE             0x00
#define DRM_ROCKCHIP_GEM_MAP_OFFSET         0x01
#define DRM_ROCKCHIP_RGA_GET_VER            0x20
#define DRM_ROCKCHIP_RGA_SET_CMDLIST        0x21
#define DRM_ROCKCHIP_RGA_EXEC               0x22

#define DRM_IOCTL_ROCKCHIP_RGA_GET_VER      DRM_IOWR(DRM_COMMAND_BASE + DRM_ROCKCHIP_RGA_GET_VER, struct drm_rockchip_rga_get_ver)
#define DRM_IOCTL_ROCKCHIP_RGA_SET_CMDLIST  DRM_IOWR(DRM_COMMAND_BASE + DRM_ROCKCHIP_RGA_SET_CMDLIST, struct drm_rockchip_rga_set_cmdlist)

#define DRM_IOCTL_ROCKCHIP_RGA_EXEC         DRM_IOWR(DRM_COMMAND_BASE + DRM_ROCKCHIP_RGA_EXEC, struct drm_rockchip_rga_exec)

#define DRM_IOCTL_ROCKCHIP_GEM_CREATE       DRM_IOWR(DRM_COMMAND_BASE + DRM_ROCKCHIP_GEM_CREATE, struct drm_rockchip_gem_create)
#define DRM_IOCTL_ROCKCHIP_GEM_MAP_OFFSET   DRM_IOWR(DRM_COMMAND_BASE + DRM_ROCKCHIP_GEM_MAP_OFFSET, struct drm_rockchip_gem_map_off)

#endif
