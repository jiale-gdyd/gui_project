#include <unordered_map>

#include <stdint.h>
#include <rockchip/rkrgax/rga.h>
#include <libdrm/drm/drm_fourcc.h>

enum format_colormodel {
    rgb,
    yuv,
};

struct table_entry {
    uint32_t          fourcc;
    format_colormodel colormodel;
};

const static std::unordered_map<uint32_t, uint32_t> drm_fourcc_table = {
    { DRM_FORMAT_RGBA8888, RK_FORMAT_ABGR_8888 },
    { DRM_FORMAT_BGRA8888, RK_FORMAT_ARGB_8888 },
    { DRM_FORMAT_ARGB8888, RK_FORMAT_BGRA_8888 },
    { DRM_FORMAT_ABGR8888, RK_FORMAT_RGBA_8888 },
    { DRM_FORMAT_RGBX8888, RK_FORMAT_XBGR_8888 },
    { DRM_FORMAT_BGRX8888, RK_FORMAT_XRGB_8888 },
    { DRM_FORMAT_XRGB8888, RK_FORMAT_BGRX_8888 },
    { DRM_FORMAT_XBGR8888, RK_FORMAT_RGBX_8888 },

    { DRM_FORMAT_RGBA5551, RK_FORMAT_ABGR_5551 },
    { DRM_FORMAT_BGRA5551, RK_FORMAT_ARGB_5551 },
    { DRM_FORMAT_ARGB1555, RK_FORMAT_BGRA_5551 },
    { DRM_FORMAT_ABGR1555, RK_FORMAT_RGBA_5551 },
    { DRM_FORMAT_RGBA4444, RK_FORMAT_ABGR_4444 },
    { DRM_FORMAT_BGRA4444, RK_FORMAT_ARGB_4444 },
    { DRM_FORMAT_ARGB4444, RK_FORMAT_BGRA_4444 },
    { DRM_FORMAT_ABGR4444, RK_FORMAT_RGBA_4444 },

    { DRM_FORMAT_RGB888,   RK_FORMAT_BGR_888 },
    { DRM_FORMAT_BGR888,   RK_FORMAT_RGB_888 },
    { DRM_FORMAT_RGB565,   RK_FORMAT_BGR_565 },
    { DRM_FORMAT_BGR565,   RK_FORMAT_RGB_565 },

    { DRM_FORMAT_NV16,     RK_FORMAT_YCbCr_422_SP },
    { DRM_FORMAT_NV61,     RK_FORMAT_YCrCb_422_SP },
    { DRM_FORMAT_YUV422,   RK_FORMAT_YCbCr_422_P },
    { DRM_FORMAT_YVU422,   RK_FORMAT_YCrCb_422_P },
    { DRM_FORMAT_NV12,     RK_FORMAT_YCbCr_420_SP },
    { DRM_FORMAT_NV21,     RK_FORMAT_YCrCb_420_SP },
    { DRM_FORMAT_YUV420,   RK_FORMAT_YCbCr_420_P },
    { DRM_FORMAT_YVU420,   RK_FORMAT_YCrCb_420_P },
    { DRM_FORMAT_NV15,     RK_FORMAT_YCbCr_420_SP_10B },
    { DRM_FORMAT_YUYV,     RK_FORMAT_YUYV_422 },
    { DRM_FORMAT_YVYU,     RK_FORMAT_YVYU_422 },
    { DRM_FORMAT_UYVY,     RK_FORMAT_UYVY_422 },
    { DRM_FORMAT_VYUY,     RK_FORMAT_VYUY_422 },
    { DRM_FORMAT_INVALID,  RK_FORMAT_UNKNOWN },
};

uint32_t get_format_from_drm_fourcc(uint32_t drm_fourcc)
{
    auto entry = drm_fourcc_table.find(drm_fourcc);
    if (entry == drm_fourcc_table.end()) {
        return RK_FORMAT_UNKNOWN;
    }

    return entry->second;
}
