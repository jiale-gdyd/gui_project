#ifndef LIBDRM_DISPLAY_MEDIA_TYPE_H
#define LIBDRM_DISPLAY_MEDIA_TYPE_H

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
#define DRM_IMAGE_GRAY8                             "imamge:gray8"
#define DRM_IMAGE_GRAY16                            "image:gray16"
#define DRM_IMAGE_YUV420P                           "image:yuv420p"
#define DRM_IMAGE_NV12                              "image:nv12"
#define DRM_IMAGE_NV21                              "image:nv21"
#define DRM_IMAGE_YV12                              "image:yv12"
#define DRM_IMAGE_FBC2                              "image:fbc2"
#define DRM_IMAGE_FBC0                              "image:fbc0"
#define DRM_IMAGE_YUV422P                           "image:yuv422p"
#define DRM_IMAGE_NV16                              "image:nv16"
#define DRM_IMAGE_NV61                              "image:nv61"
#define DRM_IMAGE_YV16                              "image:yv16"
#define DRM_IMAGE_YUYV422                           "image:yuyv422"
#define DRM_IMAGE_UYVY422                           "image:uyvy422"
#define DRM_IMAGE_YUV444SP                          "image:yuv444sp"
#define DRM_IMAGE_RGB332                            "image:rgb332"
#define DRM_IMAGE_RGB565                            "image:rgb565"
#define DRM_IMAGE_BGR565                            "image:bgr565"
#define DRM_IMAGE_RGB888                            "image:rgb888"
#define DRM_IMAGE_BGR888                            "image:bgr888"
#define DRM_IMAGE_ARGB8888                          "image:argb8888"
#define DRM_IMAGE_XRGB8888                          "image:xrgb8888"
#define DRM_IMAGE_ABGR8888                          "image:abgr8888"
#define DRM_IMAGE_RGBA8888                          "image:rgba8888"
#define DRM_IMAGE_BGRA8888                          "image:bgra8888"

#define DRM_IMAGE_PREFIX                            "image:"
#define DRM_IMAGE_JPEG                              "image:jpeg"

#define DRM_VIDEO_PREFIX                            "video:"
#define DRM_VIDEO_H264                              "video:h264"
#define DRM_VIDEO_H265                              "video:h265"
#define DRM_VIDEO_MJPEG                             "video:mjpeg"

#define DRM_AUDIO_MP3                               "audio:mp3"
#define DRM_AUDIO_MP2                               "audio:mp2"
#define DRM_AUDIO_G711A                             "audio:g711a"
#define DRM_AUDIO_G711U                             "audio:g711U"
#define DRM_AUDIO_G726                              "audio:g726"

#define DRM_AUDIO_PREFIX                            "audio:"
#define DRM_AUDIO_PCM_U8                            "audio:pcm_u8"
#define DRM_AUDIO_PCM_S16                           "audio:pcm_s16"
#define DRM_AUDIO_PCM_S32                           "audio:pcm_s32"
#define DRM_AUDIO_PCM_FLT                           "audio:pcm_flt"
#define DRM_AUDIO_PCM_U8P                           "audio:pcm_u8p"
#define DRM_AUDIO_PCM_S16P                          "audio:pcm_s16p"
#define DRM_AUDIO_PCM_S32P                          "audio:pcm_s32p"
#define DRM_AUDIO_PCM_FLTP                          "audio:pcm_fltp"
#define DRM_AUDIO_PCM_G711A                         "audio:pcm_g711a"
#define DRM_AUDIO_PCM_G711U                         "audio:pcm_g711u"

#define DRM_AUDIO_PCM                               \
    DRM_TYPENEAR(DRM_AUDIO_PCM_U8)                  \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S16)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S32)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_FLT)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_U8P)                 \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S16P)                \
    DRM_TYPENEAR(DRM_AUDIO_PCM_S32P)                \
    DRM_TYPENEAR(DRM_AUDIO_PCM_FLTP)

#define DRM_TEXT_PREFIX                             "text:"

#define DRM_STREAM_OGG                              "stream:ogg"
#define DRM_STREAM_FILE                             "stream:file"

#define DRM_NN_MODEL_PREFIX                         "nn_model:"
#define DRM_NN_FLOAT32                              "nn:float32"
#define DRM_NN_FLOAT16                              "nn:float16"
#define DRM_NN_INT8                                 "nn:int8"
#define DRM_NN_UINT8                                "nn:uint8"
#define DRM_NN_INT16                                "nn:int16"

#define DRM_MUXER_MP4                               "mp4"
#define DRM_MUXER_FLV                               "flv"
#define DRM_MUXER_F4V                               "f4v"
#define DRM_MUXER_MKV                               "mkv"
#define DRM_MUXER_AVI                               "avi"
#define DRM_MUXER_MPEG_TS                           "mpegts"
#define DRM_MUXER_MPEG_PS                           "mpeg"

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
