#ifndef LIBDRM_MEDIA_UTILS_H
#define LIBDRM_MEDIA_UTILS_H

#include <string>
#include <media/drm_media_common.h>

std::string ImageTypeToString(drm_image_type_e type);
drm_image_type_e StringToImageType(std::string type);

std::string CodecToString(drm_codec_type_e type);
std::string SampleFormatToString(drm_sample_format_e type);

#endif
