#include <media/utils/utils.h>
#include <media/utils/buffer.h>
#include <media/utils/demuxer.h>

namespace libdrm {
DEFINE_REFLECTOR(Demuxer)

DEFINE_FACTORY_COMMON_PARSE(Demuxer)

Demuxer::Demuxer(const char *param) : total_time(0.0f)
{
    std::map<std::string, std::string> params;
    if (!parse_media_param_map(param, params)) {
        return;
    }

    for (auto &p : params) {
        const std::string &key = p.first;
        if (key == DRM_KEY_PATH) {
            path = p.second;
        }
    }
}

DEFINE_PART_FINAL_EXPOSE_PRODUCT(Demuxer, Demuxer)
}
