
#include <string.h>
#include <media/utils/sound.h>
#include <media/utils/utils.h>
#include <media/utils/key_string.h>
#include <media/utils/media_type.h>

static const struct SampleFormatEntry {
    DrmSampleFormat fmt;
    const char      *fmt_str;
} sample_format_string_map[] = {
    {DRM_SAMPLE_FMT_U8,    DRM_AUDIO_PCM_U8},    {DRM_SAMPLE_FMT_S16,   DRM_AUDIO_PCM_S16},
    {DRM_SAMPLE_FMT_S32,   DRM_AUDIO_PCM_S32},   {DRM_SAMPLE_FMT_FLT,   DRM_AUDIO_PCM_FLT},
    {DRM_SAMPLE_FMT_U8P,   DRM_AUDIO_PCM_U8P},   {DRM_SAMPLE_FMT_S16P,  DRM_AUDIO_PCM_S16P},
    {DRM_SAMPLE_FMT_S32P,  DRM_AUDIO_PCM_S32P},  {DRM_SAMPLE_FMT_FLTP,  DRM_AUDIO_PCM_FLTP},
    {DRM_SAMPLE_FMT_G711A, DRM_AUDIO_PCM_G711A}, {DRM_SAMPLE_FMT_G711U, DRM_AUDIO_PCM_G711U},
};

const char *SampleFmtToString(DrmSampleFormat fmt)
{
    FIND_ENTRY_TARGET(fmt, sample_format_string_map, fmt, fmt_str)
    return nullptr;
}

DrmSampleFormat StringToSampleFmt(const char *fmt_str)
{
    FIND_ENTRY_TARGET_BY_STRCMP(fmt_str, sample_format_string_map, fmt_str, fmt)
    return DRM_SAMPLE_FMT_NONE;
}

bool SampleInfoIsValid(const DrmSampleInfo &sample_info)
{
    return (sample_info.fmt != DRM_SAMPLE_FMT_NONE) && (sample_info.channels > 0) && (sample_info.sample_rate > 0);
}

size_t GetSampleSize(const DrmSampleInfo &sample_info)
{
    size_t sample_size = sample_info.channels;
    switch (sample_info.fmt) {
        case DRM_SAMPLE_FMT_U8:
        case DRM_SAMPLE_FMT_U8P:
        case DRM_SAMPLE_FMT_G711A:
        case DRM_SAMPLE_FMT_G711U:
            return sample_size;

        case DRM_SAMPLE_FMT_S16:
        case DRM_SAMPLE_FMT_S16P:
            return sample_size << 1;

        case DRM_SAMPLE_FMT_S32:
        case DRM_SAMPLE_FMT_S32P:
        case DRM_SAMPLE_FMT_FLT:
        case DRM_SAMPLE_FMT_FLTP:
            return sample_size << 2;

        default:
            return 0;
    }
}

namespace libdrm {
bool ParseSampleInfoFromMap(std::map<std::string, std::string> &params, DrmSampleInfo &si)
{
    std::string value;
    CHECK_EMPTY(value, params, DRM_KEY_INPUTDATATYPE)

    si.fmt = StringToSampleFmt(value.c_str());
    if (si.fmt == DRM_SAMPLE_FMT_NONE) {
        DRM_MEDIA_LOGE("unsupport sample fmt %s", value.c_str());
        return false;
    }

    CHECK_EMPTY(value, params, DRM_KEY_CHANNELS)
    si.channels = std::stoi(value);

    CHECK_EMPTY(value, params, DRM_KEY_SAMPLE_RATE)
    si.sample_rate = std::stoi(value);

    CHECK_EMPTY(value, params, DRM_KEY_FRAMES)
    si.nb_samples = std::stoi(value);

    return true;
}

std::string to_param_string(const DrmSampleInfo &si)
{
    std::string s;
    const char *fmt = SampleFmtToString(si.fmt);
    if (!fmt) {
        return s;
    }

    PARAM_STRING_APPEND(s, DRM_KEY_INPUTDATATYPE, fmt);
    PARAM_STRING_APPEND_TO(s, DRM_KEY_CHANNELS, si.channels);
    PARAM_STRING_APPEND_TO(s, DRM_KEY_SAMPLE_RATE, si.sample_rate);
    PARAM_STRING_APPEND_TO(s, DRM_KEY_FRAMES, si.nb_samples);

    return s;
}
}
