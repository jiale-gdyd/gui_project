#ifndef LIBDRM_DISPLAY_MUXER_H
#define LIBDRM_DISPLAY_MUXER_H

#include "media_config.h"
#include "media_reflector.h"
#include "media_type.h"
#include "stream.h"

namespace libdrm {
DECLARE_FACTORY(Muxer)

DECLARE_REFLECTOR(Muxer)

#define DEFINE_MUXER_FACTORY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT)                                        \
    DEFINE_MEDIA_CHILD_FACTORY(REAL_PRODUCT, REAL_PRODUCT::GetMuxName(), FINAL_EXPOSE_PRODUCT, Muxer)   \
    DEFINE_MEDIA_CHILD_FACTORY_EXTRA(REAL_PRODUCT)                                                      \
    DEFINE_MEDIA_NEW_PRODUCT_BY(REAL_PRODUCT, Muxer, Init() != true)

#define DEFINE_COMMON_MUXER_FACTORY(REAL_PRODUCT)                                                       \
    DEFINE_MUXER_FACTORY(REAL_PRODUCT, Muxer)

class MediaBuffer;

class Encoder;
typedef int (*write_callback_func)(void *, uint8_t *, int);

class Muxer {
public:
    Muxer(const char *param);
    virtual ~Muxer() = default;

    static const char *GetMuxName() {
        return nullptr;
    }

    virtual bool Init() = 0;
    virtual bool IncludeEncoder() {
        return false;
    }

    virtual bool NewMuxerStream(const DrmMediaConfig &mc, const std::shared_ptr<MediaBuffer> &enc_extra_data, int &stream_no) = 0;

    virtual bool SetIoStream(std::shared_ptr<Stream> output) {
        io_output = output;
        return true;
    }

    virtual bool SetWriteCallback(void *handler, write_callback_func callback) {
        m_handler = handler;
        m_write_callback_func = callback;
        return true;
    }

    virtual std::shared_ptr<MediaBuffer> WriteHeader(int stream_no) = 0;
    virtual std::shared_ptr<MediaBuffer> Write(std::shared_ptr<MediaBuffer> orig_data, int stream_no) = 0;

protected:
    std::shared_ptr<Stream> io_output;
    void                    *m_handler;
    write_callback_func     m_write_callback_func;

    DECLARE_PART_FINAL_EXPOSE_PRODUCT(Muxer)
};
}

#endif
