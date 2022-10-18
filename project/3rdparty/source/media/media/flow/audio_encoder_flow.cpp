#include <assert.h>

#include <media/utils/flow.h>
#include <media/utils/sound.h>
#include <media/utils/buffer.h>
#include <media/utils/encoder.h>
#include <media/utils/media_type.h>

namespace libdrm {
static bool encode(Flow *f, MediaBufferVector &input_vector);

class AudioEncoderFlow : public Flow {
public:
    AudioEncoderFlow(const char *param);
    virtual ~AudioEncoderFlow() {
        AutoPrintLine apl(__func__);
        StopAllThread();
    }

    static const char *GetFlowName() {
        return "audio_enc";
    }

    int GetInputSize() override;
    int Control(unsigned long int request, ...) override;

private:
    std::shared_ptr<AudioEncoder> enc;
    int                           input_size;
    bool                          is_mute;

    friend bool encode(Flow *f, MediaBufferVector &input_vector);
};

bool encode(Flow *f, MediaBufferVector &input_vector)
{
    AudioEncoderFlow *af = (AudioEncoderFlow *)f;
    std::shared_ptr<AudioEncoder> enc = af->enc;
    std::shared_ptr<MediaBuffer> &src = input_vector[0];
    std::shared_ptr<MediaBuffer> dst;

    bool result = true;
    bool feed_null = false;
    size_t limit_size = af->input_size;

    if (!src) {
        return false;
    }

    if (af->is_mute) {
        auto src_sb = std::static_pointer_cast<SampleBuffer>(src);
        DrmSampleInfo sampleinfo;
        sampleinfo.fmt = src_sb->GetFormat();
        sampleinfo.channels = src_sb->GetChannels();
        sampleinfo.nb_samples = src_sb->GetSamples();
        sampleinfo.sample_rate = src_sb->GetSampleRate();

        auto mute_sb = std::make_shared<libdrm::SampleBuffer>(MediaBuffer::Alloc2(src->GetValidSize()), sampleinfo);
        if (mute_sb) {
            mute_sb->SetValidSize(src->GetValidSize());
            mute_sb->SetType(src->GetType());
            mute_sb->SetUserFlag(src->GetUserFlag());
            mute_sb->SetUSTimeStamp(src->GetUSTimeStamp());
            mute_sb->SetEOF(src->IsEOF());
            mute_sb->SetTsvcLevel(src->GetTsvcLevel());
            memset((char *)mute_sb->GetPtr(), 0, mute_sb->GetValidSize());

            src = mute_sb;
        }
    }

    if (limit_size && (src->GetValidSize() > limit_size)) {
        DRM_MEDIA_LOGW("AudioEncFlow: buffer(%d) is bigger than expected(%d)", (int)src->GetValidSize(), (int)limit_size);
    } else if (src->GetValidSize() < limit_size) {
        if (src->GetValidSize() <= 0)
            src->SetValidSize(0);
        else if (src->GetSize() >= limit_size) {
            char *null_ptr = (char *)src->GetPtr() + src->GetValidSize();
            memset(null_ptr, 0, limit_size - src->GetValidSize());
            src->SetValidSize(limit_size);
            feed_null = true;
        } else if (src->GetSize() == src->GetValidSize()) {

        } else {
            src->SetValidSize(0);
        }
    }

    if ((src->GetValidSize() == limit_size) && src->IsEOF()) {
        feed_null = true;
    }

    int ret = enc->SendInput(src);
    if (ret < 0) {
        fprintf(stderr, "[Audio]: frame encode failed, ret=%d\n", ret);
    }

    if (feed_null) {
        auto null_buffer = MediaBuffer::Alloc(1, MediaBuffer::MemType::MEM_COMMON);
        if (!null_buffer) {
            DRM_LOG_MEMORY();
        } else {
            enc->SendInput(null_buffer);
        }
    }

    while (ret >= 0) {
        dst = enc->FetchOutput();
        if (!dst) {
            if (errno != EAGAIN) {
                fprintf(stderr, "[Audio]: frame fetch failed, ret=%d\n", errno);
                result = false;
            }

            break;
        }

        size_t out_len = dst->GetValidSize();
        if (out_len == 0) {
            break;
        }

        DRM_MEDIA_LOGD("[Audio]: frame encoded, out %d bytes\n", (int)out_len);

        result = af->SetOutput(dst, 0);
        if (!result) {
            break;
        }
    }

    return result;
}

AudioEncoderFlow::AudioEncoderFlow(const char *param)
{
    std::list<std::string> separate_list;
    std::map<std::string, std::string> params;
    if (!ParseWrapFlowParams(param, params, separate_list)) {
        SetError(-EINVAL);
        return;
    }

    std::string &codec_name = params[DRM_KEY_NAME];
    if (codec_name.empty()) {
        DRM_MEDIA_LOGI("missing codec name");
        SetError(-EINVAL);
        return;
    }

    const char *ccodec_name = codec_name.c_str();
    std::string &&rule = gen_datatype_rule(params);
    if (rule.empty()) {
        SetError(-EINVAL);
        return;
    }

    if (!REFLECTOR(Encoder)::IsMatch(ccodec_name, rule.c_str())) {
        DRM_MEDIA_LOGI("Unsupport for audio encoder %s : [%s]", ccodec_name, rule.c_str());
        SetError(-EINVAL);
        return;
    }

    const std::string &enc_param_str = separate_list.back();
    std::map<std::string, std::string> enc_params;
    if (!parse_media_param_map(enc_param_str.c_str(), enc_params)) {
        SetError(-EINVAL);
        return;
    }

    DrmMediaConfig mc;
    if (!ParseMediaConfigFromMap(enc_params, mc)) {
        SetError(-EINVAL);
        return;
    }

    is_mute = false;

    auto encoder = REFLECTOR(Encoder)::Create<AudioEncoder>(ccodec_name, enc_param_str.c_str());
    if (!encoder) {
        DRM_MEDIA_LOGI("Fail to create audio encoder %s<%s>", ccodec_name, enc_param_str.c_str());
        SetError(-EINVAL);
        return;
    }

    if (!encoder->InitConfig(mc)) {
        DRM_MEDIA_LOGI("Fail to init config, %s", ccodec_name);
        SetError(-EINVAL);
        return;
    }

    enc = encoder;
    input_size = enc->GetNbSamples() * GetSampleSize(mc.aud_cfg.sample_info);

    SlotMap sm;
    sm.input_slots.push_back(0);
    sm.output_slots.push_back(0);

    sm.process = encode;
    sm.thread_model = Model::ASYNCCOMMON;
    sm.mode_when_full = InputMode::DROPFRONT;
    sm.input_maxcachenum.push_back(20);
    if (!InstallSlotMap(sm, "AudioEncoderFlow", 40)) {
        DRM_MEDIA_LOGI("Fail to InstallSlotMap for AudioEncoderFlow");
        SetError(-EINVAL);
        return;
    }

    SetFlowTag("AudioEncoderFlow");
}

int AudioEncoderFlow::GetInputSize()
{
    return input_size;
}

int AudioEncoderFlow::Control(unsigned long int request, ...)
{
    va_list vl;
    int ret = 0;

    va_start(vl, request);
    switch (request) {
        case S_SET_MUTE: {
            int mute = va_arg(vl, int);
            is_mute = mute ? true : false;
            DRM_MEDIA_LOGI("%s mute", is_mute ? "enable" : "disable");
        }
        break;

        default:
            ret = -1;
        break;
    }
    va_end(vl);

    return ret;
}

DEFINE_FLOW_FACTORY(AudioEncoderFlow, Flow)

const char *FACTORY(AudioEncoderFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_ANYTHING;
}

const char *FACTORY(AudioEncoderFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
