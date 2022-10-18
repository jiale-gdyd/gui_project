#ifndef LIBDRM_DISPLAY_CODEC_H
#define LIBDRM_DISPLAY_CODEC_H

#include <list>
#include <memory>

#include "media_config.h"

namespace libdrm {
class MediaBuffer;

class Codec {
public:
    Codec() {
        memset(&config, 0x00, sizeof(config));
    }

    ~Codec() {

    }

    static const char *GetCodecName() {
        return nullptr;
    }

    DrmMediaConfig &GetConfig() {
        return config;
    }

    void SetConfig(const DrmMediaConfig &cfg) {
        config = cfg;
    }

    std::shared_ptr<MediaBuffer> GetExtraData(void **data = nullptr, size_t *size = nullptr);

    bool SetExtraData(void *data, size_t size, bool realloc = true);
    void SetExtraData(const std::shared_ptr<MediaBuffer> &data) {
        extra_data = data;
    }

    virtual bool Init() = 0;

    virtual int Process(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &output, std::shared_ptr<MediaBuffer> extra_output = nullptr) = 0;

    virtual int SendInput(const std::shared_ptr<MediaBuffer> &input) = 0;
    virtual std::shared_ptr<MediaBuffer> FetchOutput() = 0;

private:
    DrmMediaConfig               config;
    std::shared_ptr<MediaBuffer> extra_data;
};

const uint8_t *find_nalu_startcode(const uint8_t *p, const uint8_t *end);

std::list<std::shared_ptr<MediaBuffer>> split_h264_separate(const uint8_t *buffer, size_t length, int64_t timestamp);
std::list<std::shared_ptr<MediaBuffer>> split_h265_separate(const uint8_t *buffer, size_t length, int64_t timestamp);

void *GetVpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
void *GetSpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
void *GetPpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
void *GetSpsPpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
void *GetVpsSpsPpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
void *GetSeiFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
void *GetIntraFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type);
}

#endif
