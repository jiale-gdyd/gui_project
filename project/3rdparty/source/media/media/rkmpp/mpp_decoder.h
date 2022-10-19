#ifndef LIBDRM_MEDIA_MPP_DECODER_H
#define LIBDRM_MEDIA_MPP_DECODER_H

#include <media/utils/decoder.h>
#include "mpp_inc.h"

namespace libdrm {
class MPPDecoder : public VideoDecoder {
public:
    MPPDecoder(const char *param);
    virtual ~MPPDecoder() = default;

    static const char *GetCodecName() {
        return "rkmpp";
    }

    virtual bool Init() override;
    virtual int Process(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &output, std::shared_ptr<MediaBuffer> extra_output) override;

    virtual int SendInput(const std::shared_ptr<MediaBuffer> &input) override;
    virtual std::shared_ptr<MediaBuffer> FetchOutput() override;

private:
    DrmPixelFormat              output_format;
    int                         fg_limit_num;
    uint32_t                    need_split;
    MppPollType                 timeout;
    MppCodingType               coding_type;
    std::shared_ptr<MPPContext> mpp_ctx;
    bool                        support_sync;
    bool                        support_async;
    static const int            kFRAMEGROUP_MAX_FRAMES = 32;
};
}

#endif
