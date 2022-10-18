#include <string.h>
#include <media/utils/utils.h>
#include <media/utils/media_type.h>

static const struct CodecTypeEntry {
    DrmCodecType fmt;
    const char   *fmt_str;
} codec_type_string_map[] = {
    {DRM_CODEC_TYPE_MP3,   DRM_AUDIO_MP3},   {DRM_CODEC_TYPE_MP2,   DRM_AUDIO_MP2},
    {DRM_CODEC_TYPE_G711A, DRM_AUDIO_G711A}, {DRM_CODEC_TYPE_G711U, DRM_AUDIO_G711U},
    {DRM_CODEC_TYPE_G726,  DRM_AUDIO_G726},  {DRM_CODEC_TYPE_H264,  DRM_VIDEO_H264},
    {DRM_CODEC_TYPE_H265,  DRM_VIDEO_H265},  {DRM_CODEC_TYPE_JPEG,  DRM_VIDEO_MJPEG},
    {DRM_CODEC_TYPE_JPEG,  DRM_IMAGE_JPEG},
};

const char *CodecTypeToString(DrmCodecType fmt) {
    FIND_ENTRY_TARGET(fmt, codec_type_string_map, fmt, fmt_str)
    return nullptr;
}

DrmCodecType StringToCodecType(const char *fmt_str)
{
    FIND_ENTRY_TARGET_BY_STRCMP(fmt_str, codec_type_string_map, fmt_str, fmt)
    return DRM_CODEC_TYPE_NONE;
}

namespace libdrm {
DrmType StringToDataType(const char *data_type)
{
    if (string_start_withs(data_type, DRM_AUDIO_PREFIX)) {
        return DrmType::Audio;
    } else if (string_start_withs(data_type, DRM_IMAGE_PREFIX)) {
        return DrmType::Image;
    } else if (string_start_withs(data_type, DRM_VIDEO_PREFIX)) {
        return DrmType::Video;
    } else if (string_start_withs(data_type, DRM_TEXT_PREFIX)) {
        return DrmType::Text;
    }

    return DrmType::None;
}
}
