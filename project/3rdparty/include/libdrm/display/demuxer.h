#ifndef LIBDRM_DISPLAY_DEMUXER_H
#define LIBDRM_DISPLAY_DEMUXER_H

#include <list>

#include "stream.h"
#include "media_config.h"
#include "media_reflector.h"

namespace libdrm {

DECLARE_FACTORY(Demuxer)
DECLARE_REFLECTOR(Demuxer)

#define DEFINE_DEMUXER_FACTORY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT)                                          \
    DEFINE_MEDIA_CHILD_FACTORY(REAL_PRODUCT, REAL_PRODUCT::GetDemuxName(), FINAL_EXPOSE_PRODUCT, Demuxer)   \
    DEFINE_MEDIA_CHILD_FACTORY_EXTRA(REAL_PRODUCT)                                                          \
    DEFINE_MEDIA_NEW_PRODUCT_BY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT, GetError() < 0)

class MediaBuffer;
class Demuxer {
public:
    Demuxer(const char *param);
    virtual ~Demuxer() = default;

    static const char *GetDemuxName() {
        return nullptr;
    }

    virtual bool IncludeDecoder() {
        return false;
    }

    virtual bool Init(std::shared_ptr<Stream> input, DrmMediaConfig *out_cfg) = 0;
    virtual char **GetComment() {
        return nullptr;
    }

    virtual std::shared_ptr<MediaBuffer> Read(size_t request_size = 0) = 0;

public:
    double total_time;

protected:
    std::string path;

    DEFINE_ERR_GETSET()
    DECLARE_PART_FINAL_EXPOSE_PRODUCT(Demuxer)
};
}

#endif
