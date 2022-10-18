#include <media/utils/encoder.h>

namespace libdrm {
#define VIDEO_ENC_CHANGE_MAX        50

DEFINE_REFLECTOR(Encoder)

DEFINE_FACTORY_COMMON_PARSE(Encoder)

void VideoEncoder::RequestChange(uint32_t change, std::shared_ptr<ParameterBuffer> value)
{
    change_mtx.lock();
    if (change & VideoEncoder::kGetFlag) {
        auto p = std::pair<uint32_t, std::shared_ptr<ParameterBuffer>>(change, std::move(value));
        CheckConfigChange(p);
        change_mtx.unlock();
        return;
    }

    if (VideoEncoder::kOSDDataChange == change) {
        DrmOsdRegionData *osd = (DrmOsdRegionData *)value->GetPtr();
        for (auto it = change_list.begin(); it != change_list.end();) {
            auto p = *it;
            if (VideoEncoder::kOSDDataChange == p.first) {
                auto v = p.second;
                DrmOsdRegionData *d = (DrmOsdRegionData *)v->GetPtr();
                if (osd->region_id == d->region_id) {
                    it = change_list.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    if (change_list.size() > VIDEO_ENC_CHANGE_MAX) {
        DRM_MEDIA_LOGW("Video Encoder: change list reached max cnt:%d. Drop front", VIDEO_ENC_CHANGE_MAX);
        change_list.pop_front();
    }

    change_list.emplace_back(change, std::move(value));
    change_mtx.unlock();
}

void VideoEncoder::QueryChange(uint32_t change, void *value, int32_t size)
{
    DRM_MEDIA_LOGW("Video Encoder: %s should be reloaded first", __func__);
    DRM_UNUSED(change);
    DRM_UNUSED(value);
    DRM_UNUSED(size);
}

std::pair<uint32_t, std::shared_ptr<ParameterBuffer>> VideoEncoder::PeekChange()
{
    std::lock_guard<std::mutex> _lg(change_mtx);
    if (change_list.empty()) {
        static auto empty = std::pair<uint32_t, std::shared_ptr<ParameterBuffer>>(0, nullptr);
        return empty;
    }

    std::pair<uint32_t, std::shared_ptr<ParameterBuffer>> p = change_list.front();
    change_list.pop_front();

    return p;
}

DEFINE_PART_FINAL_EXPOSE_PRODUCT(VideoEncoder, Encoder)
DEFINE_PART_FINAL_EXPOSE_PRODUCT(AudioEncoder, Encoder)
}
