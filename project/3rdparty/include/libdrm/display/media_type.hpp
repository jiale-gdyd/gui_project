#ifndef LIBDRM_DISPLAY_MEDIA_TYPE_HPP
#define LIBDRM_DISPLAY_MEDIA_TYPE_HPP

enum class DrmType {
    None = -1,
    Audio = 0,
    Image,
    Video,
    Text
};

#define DRM_TYPENEAR(type)                          type "\n"

#define DRM_TYPE_NOTHING                            nullptr
#define DRM_TYPE_ANYTHING                           ""

#define DRM_IMAGE_PREFIX                            "image:"
#define DRM_IMAGE_GRAY8                             DRM_IMAGE_PREFIX "gray8"
#define DRM_IMAGE_GRAY16                            DRM_IMAGE_PREFIX "gray16"
#define DRM_IMAGE_YUV420P                           DRM_IMAGE_PREFIX "yuv420p"
#define DRM_IMAGE_NV12                              DRM_IMAGE_PREFIX "nv12"
#define DRM_IMAGE_NV21                              DRM_IMAGE_PREFIX "nv21"
#define DRM_IMAGE_YV12                              DRM_IMAGE_PREFIX "yv12"
#define DRM_IMAGE_FBC2                              DRM_IMAGE_PREFIX "fbc2"
#define DRM_IMAGE_FBC0                              DRM_IMAGE_PREFIX "fbc0"
#define DRM_IMAGE_YUV422P                           DRM_IMAGE_PREFIX "yuv422p"
#define DRM_IMAGE_NV16                              DRM_IMAGE_PREFIX "nv16"
#define DRM_IMAGE_NV61                              DRM_IMAGE_PREFIX "nv61"
#define DRM_IMAGE_YV16                              DRM_IMAGE_PREFIX "yv16"
#define DRM_IMAGE_YUYV422                           DRM_IMAGE_PREFIX "yuyv422"
#define DRM_IMAGE_UYVY422                           DRM_IMAGE_PREFIX "uyvy422"
#define DRM_IMAGE_YUV444SP                          DRM_IMAGE_PREFIX "yuv444sp"
#define DRM_IMAGE_RGB332                            DRM_IMAGE_PREFIX "rgb332"
#define DRM_IMAGE_RGB565                            DRM_IMAGE_PREFIX "rgb565"
#define DRM_IMAGE_BGR565                            DRM_IMAGE_PREFIX "bgr565"
#define DRM_IMAGE_RGB888                            DRM_IMAGE_PREFIX "rgb888"
#define DRM_IMAGE_BGR888                            DRM_IMAGE_PREFIX "bgr888"
#define DRM_IMAGE_ARGB8888                          DRM_IMAGE_PREFIX "argb8888"
#define DRM_IMAGE_ABGR8888                          DRM_IMAGE_PREFIX "abgr8888"
#define DRM_IMAGE_RGBA8888                          DRM_IMAGE_PREFIX "rgba8888"
#define DRM_IMAGE_BGRA8888                          DRM_IMAGE_PREFIX "bgra8888"

typedef enum {
    DRM_CODEC_TYPE_NONE = -1,
    DRM_CODEC_TYPE_MP3,
    DRM_CODEC_TYPE_MP2,
    DRM_CODEC_TYPE_G711A,
    DRM_CODEC_TYPE_G711U,
    DRM_CODEC_TYPE_G726,
    DRM_CODEC_TYPE_H264,
    DRM_CODEC_TYPE_H265,
    DRM_CODEC_TYPE_JPEG,
    DRM_CODEC_TYPE_NB
} DrmCodecType;

const char *CodecTypeToString(DrmCodecType fmt);
DrmCodecType StringToCodecType(const char *fmt_str);

#include <string>

namespace libdrm {
DrmType StringToDataType(const char *data_type);

class SupportMediaTypes {
public:
    std::string types;
};
}

#endif
