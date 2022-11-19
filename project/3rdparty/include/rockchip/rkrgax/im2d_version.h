#ifndef ROCKCHIP_RKRGAX_IM2D_VERSION_H
#define ROCKCHIP_RKRGAX_IM2D_VERSION_H

#define RGA_VERSION_STR_HELPER(x)           #x
#define RGA_VERSION_STR(x)                  RGA_VERSION_STR_HELPER(x)

#define RGA_API_MAJOR_VERSION               1
#define RGA_API_MINOR_VERSION               8
#define RGA_API_REVISION_VERSION            1
#define RGA_API_BUILD_VERSION               4

#define RGA_API_VERSION                             \
    RGA_VERSION_STR(RGA_API_MAJOR_VERSION) "."      \
    RGA_VERSION_STR(RGA_API_MINOR_VERSION) "."      \
    RGA_VERSION_STR(RGA_API_REVISION_VERSION) "_["  \
    RGA_VERSION_STR(RGA_API_BUILD_VERSION) "]"

#define RGA_API_FULL_VERSION                "rga_api version " RGA_API_VERSION

#define RGA_SET_CURRENT_API_VERISON         ((RGA_API_MAJOR_VERSION & 0xff) << 24 | (RGA_API_MINOR_VERSION & 0xff) << 16 | (RGA_API_REVISION_VERSION & 0xff) << 8 | (RGA_API_BUILD_VERSION & 0xff))

#endif
