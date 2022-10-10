#ifndef LIBDRM_DISPLAY_MEDIA_TYPE_HPP
#define LIBDRM_DISPLAY_MEDIA_TYPE_HPP

#include "../drm_media_common.h"

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

#define DRM_IMAGE_PREFIX                            "image:"
#define DRM_IMAGE_JPEG                              DRM_IMAGE_PREFIX "jpeg"

#define DRM_VIDEO_PREFIX                            "video:"
#define DRM_VIDEO_H264                              DRM_VIDEO_PREFIX "h264"
#define DRM_VIDEO_H265                              DRM_VIDEO_PREFIX "h265"
#define DRM_VIDEO_MJPEG                             DRM_VIDEO_PREFIX "mjpeg"

#define DRM_AUDIO_PREFIX                            "audio:"
#define DRM_AUDIO_MP3                               DRM_AUDIO_PREFIX "mp3"
#define DRM_AUDIO_MP2                               DRM_AUDIO_PREFIX "mp2"
#define DRM_AUDIO_G711A                             DRM_AUDIO_PREFIX "g711a"
#define DRM_AUDIO_G711U                             DRM_AUDIO_PREFIX "g711U"
#define DRM_AUDIO_G726                              DRM_AUDIO_PREFIX "g726"

#define DRM_TEXT_PREFIX                             "text:"

#define DRM_AUDIO_PREFIX                            "audio:"
#define DRM_AUDIO_PCM_U8                            DRM_AUDIO_PREFIX "pcm_u8"
#define DRM_AUDIO_PCM_S16                           DRM_AUDIO_PREFIX "pcm_s16"
#define DRM_AUDIO_PCM_S32                           DRM_AUDIO_PREFIX "pcm_s32"
#define DRM_AUDIO_PCM_FLT                           DRM_AUDIO_PREFIX "pcm_flt"
#define DRM_AUDIO_PCM_U8P                           DRM_AUDIO_PREFIX "pcm_u8p"
#define DRM_AUDIO_PCM_S16P                          DRM_AUDIO_PREFIX "pcm_s16p"
#define DRM_AUDIO_PCM_S32P                          DRM_AUDIO_PREFIX "pcm_s32p"
#define DRM_AUDIO_PCM_FLTP                          DRM_AUDIO_PREFIX "pcm_fltp"
#define DRM_AUDIO_PCM_G711A                         DRM_AUDIO_PREFIX "pcm_g711a"
#define DRM_AUDIO_PCM_G711U                         DRM_AUDIO_PREFIX "pcm_g711u"

#define DRM_AUDIO_PCM                               \
    DRM_TYPENEAR(DRM_AUDIO_PCM_U8)                  \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S16)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S32)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_FLT)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_U8P)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S16P)                \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S32P)                \
    DRM_TYPENEAR(DRM_AUDIO_PCM_FLTP)

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
