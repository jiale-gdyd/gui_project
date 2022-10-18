#ifndef LIBDRM_DISPLAY_DECODER_H
#define LIBDRM_DISPLAY_DECODER_H

#include "codec.h"
#include "media_reflector.h"

namespace libdrm {
DECLARE_FACTORY(Decoder)
DECLARE_REFLECTOR(Decoder)

#define DEFINE_DECODER_FACTORY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT)                                          \
    DEFINE_MEDIA_CHILD_FACTORY(REAL_PRODUCT, REAL_PRODUCT::GetCodecName(), FINAL_EXPOSE_PRODUCT, Decoder)   \
    DEFINE_MEDIA_CHILD_FACTORY_EXTRA(REAL_PRODUCT)                                                          \
    DEFINE_MEDIA_NEW_PRODUCT_BY(REAL_PRODUCT, Decoder, Init() != true)

#define DEFINE_AUDIO_DECODER_FACTORY(REAL_PRODUCT)                                                          \
    DEFINE_DECODER_FACTORY(REAL_PRODUCT, AudioDecoder)

class Decoder : public Codec {
public:
    virtual ~Decoder() = default;
    virtual bool InitConfig(const DrmMediaConfig &cfg) {
        DRM_UNUSED(cfg);
        return true;
    }
};

class AudioDecoder : public Decoder {
public:
    virtual ~AudioDecoder() = default;
    virtual int GetNbSamples() {
        return 0;
    }

protected:
    DrmCodecType codec_type;

    DECLARE_PART_FINAL_EXPOSE_PRODUCT(Decoder)
};

#define DEFINE_VIDEO_DECODER_FACTORY(REAL_PRODUCT)                                                          \
    DEFINE_DECODER_FACTORY(REAL_PRODUCT, VideoDecoder)

class VideoDecoder : public Decoder {
public:
    virtual ~VideoDecoder() = default;

    DECLARE_PART_FINAL_EXPOSE_PRODUCT(Decoder)
};
}

#endif
