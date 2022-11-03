#include <media/utils/sound.h>
#include <media/utils/utils.h>
#include <media/utils/media_type.h>

#include "media_utils.h"

std::string ImageTypeToString(drm_image_type_e type)
{
    switch (type) {
        case DRM_IMAGE_TYPE_GRAY8:
            return DRM_IMAGE_GRAY8;

        case DRM_IMAGE_TYPE_GRAY16:
            return DRM_IMAGE_GRAY16;

        case DRM_IMAGE_TYPE_YUV420P:
            return DRM_IMAGE_YUV420P;

        case DRM_IMAGE_TYPE_NV12:
            return DRM_IMAGE_NV12;

        case DRM_IMAGE_TYPE_NV21:
            return DRM_IMAGE_NV21;

        case DRM_IMAGE_TYPE_YV12:
            return DRM_IMAGE_YV12;

        case DRM_IMAGE_TYPE_FBC2:
            return DRM_IMAGE_FBC2;

        case DRM_IMAGE_TYPE_FBC0:
            return DRM_IMAGE_FBC0;

        case DRM_IMAGE_TYPE_YUV422P:
            return DRM_IMAGE_YUV422P;

        case DRM_IMAGE_TYPE_NV16:
            return DRM_IMAGE_NV16;

        case DRM_IMAGE_TYPE_NV61:
            return DRM_IMAGE_NV61;

        case DRM_IMAGE_TYPE_YV16:
            return DRM_IMAGE_YV16;

        case DRM_IMAGE_TYPE_YUYV422:
            return DRM_IMAGE_YUYV422;

        case DRM_IMAGE_TYPE_UYVY422:
            return DRM_IMAGE_UYVY422;

        case DRM_IMAGE_TYPE_YUV444SP:
            return DRM_IMAGE_YUV444SP;

        case DRM_IMAGE_TYPE_RGB332:
            return DRM_IMAGE_RGB332;

        case DRM_IMAGE_TYPE_RGB565:
            return DRM_IMAGE_RGB565;

        case DRM_IMAGE_TYPE_BGR565:
            return DRM_IMAGE_BGR565;

        case DRM_IMAGE_TYPE_RGB888:
            return DRM_IMAGE_RGB888;

        case DRM_IMAGE_TYPE_BGR888:
            return DRM_IMAGE_BGR888;

        case DRM_IMAGE_TYPE_ARGB8888:
            return DRM_IMAGE_ARGB8888;

        case DRM_IMAGE_TYPE_XRGB8888:
            return DRM_IMAGE_XRGB8888;

        case DRM_IMAGE_TYPE_ABGR8888:
            return DRM_IMAGE_ABGR8888;

        case DRM_IMAGE_TYPE_RGBA8888:
            return DRM_IMAGE_RGBA8888;

        case DRM_IMAGE_TYPE_BGRA8888:
            return DRM_IMAGE_BGRA8888;

        case DRM_IMAGE_TYPE_JPEG:
            return DRM_IMAGE_JPEG;

        default:
            DRM_MEDIA_LOGE("not support image type:%d", type);
            return "";
    }
}

drm_image_type_e StringToImageType(std::string type)
{
    if (type == DRM_IMAGE_GRAY8) {
        return DRM_IMAGE_TYPE_GRAY8;
    } else if (type == DRM_IMAGE_GRAY16) {
        return DRM_IMAGE_TYPE_GRAY16;
    } else if (type == DRM_IMAGE_YUV420P) {
        return DRM_IMAGE_TYPE_YUV420P;
    } else if (type == DRM_IMAGE_NV12) {
        return DRM_IMAGE_TYPE_NV12;
    } else if (type == DRM_IMAGE_NV21) {
        return DRM_IMAGE_TYPE_NV21;
    } else if (type == DRM_IMAGE_YV12) {
        return DRM_IMAGE_TYPE_YV12;
    } else if (type == DRM_IMAGE_FBC2) {
        return DRM_IMAGE_TYPE_FBC2;
    } else if (type == DRM_IMAGE_FBC0) {
        return DRM_IMAGE_TYPE_FBC0;
    } else if (type == DRM_IMAGE_YUV422P) {
        return DRM_IMAGE_TYPE_YUV422P;
    } else if (type == DRM_IMAGE_NV16) {
        return DRM_IMAGE_TYPE_NV16;
    } else if (type == DRM_IMAGE_NV61) {
        return DRM_IMAGE_TYPE_NV61;
    } else if (type == DRM_IMAGE_YV16) {
        return DRM_IMAGE_TYPE_YV16;
    } else if (type == DRM_IMAGE_YUYV422) {
        return DRM_IMAGE_TYPE_YUYV422;
    } else if (type == DRM_IMAGE_UYVY422) {
        return DRM_IMAGE_TYPE_UYVY422;
    } else if (type == DRM_IMAGE_YUV444SP) {
        return DRM_IMAGE_TYPE_YUV444SP;
    } else if (type == DRM_IMAGE_RGB332) {
        return DRM_IMAGE_TYPE_RGB332;
    } else if (type == DRM_IMAGE_RGB565) {
        return DRM_IMAGE_TYPE_RGB565;
    } else if (type == DRM_IMAGE_BGR565) {
        return DRM_IMAGE_TYPE_BGR565;
    } else if (type == DRM_IMAGE_RGB888) {
        return DRM_IMAGE_TYPE_RGB888;
    } else if (type == DRM_IMAGE_BGR888) {
        return DRM_IMAGE_TYPE_BGR888;
    } else if (type == DRM_IMAGE_ARGB8888) {
        return DRM_IMAGE_TYPE_ARGB8888;
    } else if (type == DRM_IMAGE_ABGR8888) {
        return DRM_IMAGE_TYPE_ABGR8888;
    } else if (type == DRM_IMAGE_RGBA8888) {
        return DRM_IMAGE_TYPE_RGBA8888;
    } else if (type == DRM_IMAGE_BGRA8888) {
        return DRM_IMAGE_TYPE_BGRA8888;
    } else if (type == DRM_IMAGE_JPEG) {
        return DRM_IMAGE_TYPE_JPEG;
    } else if (type == DRM_IMAGE_XRGB8888) {
        return DRM_IMAGE_TYPE_XRGB8888;
    } else {
        DRM_MEDIA_LOGE("unknown image type:[%s]", type.c_str());
    }

    return DRM_IMAGE_TYPE_UNKNOW;
}

std::string CodecToString(drm_codec_type_e type)
{
    switch (type) {
        case DRM_CODEC_TYPE_MP3:
            return DRM_AUDIO_MP3;
    
        case DRM_CODEC_TYPE_MP2:
            return DRM_AUDIO_MP2;

        case DRM_CODEC_TYPE_G711A:
            return DRM_AUDIO_G711A;

        case DRM_CODEC_TYPE_G711U:
            return DRM_AUDIO_G711U;

        case DRM_CODEC_TYPE_G726:
            return DRM_AUDIO_G726;

        case DRM_CODEC_TYPE_H264:
            return DRM_VIDEO_H264;

        case DRM_CODEC_TYPE_H265:
            return DRM_VIDEO_H265;

        case DRM_CODEC_TYPE_JPEG:
        case DRM_CODEC_TYPE_MJPEG:
            return DRM_IMAGE_JPEG;

        default:
            return "";
    }
}

std::string SampleFormatToString(drm_sample_format_e type)
{
    switch (type) {
        case DRM_SAMPLE_FMT_U8:
            return DRM_AUDIO_PCM_U8;

        case DRM_SAMPLE_FMT_S16:
            return DRM_AUDIO_PCM_S16;

        case DRM_SAMPLE_FMT_S32:
            return DRM_AUDIO_PCM_S32;

        case DRM_SAMPLE_FMT_FLT:
            return DRM_AUDIO_PCM_FLT;

        case DRM_SAMPLE_FMT_U8P:
            return DRM_AUDIO_PCM_U8P;

        case DRM_SAMPLE_FMT_S16P:
            return DRM_AUDIO_PCM_S16P;

        case DRM_SAMPLE_FMT_S32P:
            return DRM_AUDIO_PCM_S32P;

        case DRM_SAMPLE_FMT_FLTP:
            return DRM_AUDIO_PCM_FLTP;

        case DRM_SAMPLE_FMT_G711A:
            return DRM_AUDIO_PCM_G711A;

        case DRM_SAMPLE_FMT_G711U:
            return DRM_AUDIO_PCM_G711U;

        default:
            return "";
    }
}
