#ifndef LIBDRM_DISPLAY_SOUND_H
#define LIBDRM_DISPLAY_SOUND_H

#include <stddef.h>

#include "utils.h"
#include "../drm_media_common.h"

typedef struct {
    DrmSampleFormat fmt;
    int             channels;
    int             sample_rate;
    int             nb_samples;
} DrmSampleInfo;

const char *SampleFmtToString(DrmSampleFormat fmt);
DrmSampleFormat StringToSampleFmt(const char *fmt_str);

bool SampleInfoIsValid(const DrmSampleInfo &sample_info);
size_t GetSampleSize(const DrmSampleInfo &sample_info);

#include <map>
#include <string>

namespace libdrm {
bool ParseSampleInfoFromMap(std::map<std::string, std::string> &params, DrmSampleInfo &si);
std::string to_param_string(const DrmSampleInfo &si);
}

#endif
