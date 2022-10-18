#include <assert.h>

#include <media/utils/flow.h>
#include <media/utils/sound.h>
#include <media/utils/utils.h>
#include <media/utils/buffer.h>
#include <media/utils/decoder.h>
#include <media/utils/media_type.h>

namespace libdrm {
static bool decode(Flow *f, MediaBufferVector &input_vector);

class AudioDecoderFlow : public Flow {
public:
    AudioDecoderFlow(const char *param);
    virtual ~AudioDecoderFlow() {
        AutoPrintLine apl(__func__);
        StopAllThread();
    }

    static const char *GetFlowName() {
        return "audio_dec";
    }

    int GetInputSize() override;

private:
    std::shared_ptr<AudioDecoder> dec;
    int                           input_size;

    friend bool decode(Flow *f, MediaBufferVector &input_vector);
};

bool decode(Flow *f, MediaBufferVector &input_vector)
{
    AudioDecoderFlow *af = (AudioDecoderFlow *)f;
    std::shared_ptr<AudioDecoder> dec = af->dec;
    std::shared_ptr<MediaBuffer> &src = input_vector[0];
    std::shared_ptr<MediaBuffer> dst;

    bool result = true;
    if (!src) {
        return false;
    }

    if (!src->IsValid()) {
        dec->SendInput(src);
        dst = dec->FetchOutput();
        if (dst->IsEOF()) {
            DRM_MEDIA_LOGI("decode, first EOF");
            return true;
        }
    }

    while (src->GetValidSize() > 0) {
        dec->SendInput(src);
        while (1) {
            dst = dec->FetchOutput();
            if (dst == nullptr) {
                break;
            }
    
            if (dst->IsEOF()) {
                DRM_MEDIA_LOGI("decode, second EOF");
                break;
            }

            af->SetOutput(dst, 0);
        }
    }

    return result;
}

AudioDecoderFlow::AudioDecoderFlow(const char *param)
{
    DRM_MEDIA_LOGD("AudioDecoderFlow");

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

    const std::string &dec_param_str = separate_list.back();
    DRM_MEDIA_LOGI("dec_param_str = %s", dec_param_str.c_str());
    std::map<std::string, std::string> dec_params;

    if (!parse_media_param_map(dec_param_str.c_str(), dec_params)) {
        SetError(-EINVAL);
        return;
    }

    DrmMediaConfig mc;
    std::string value = "";
    DrmAudioConfig &aud_cfg = mc.aud_cfg;

    ParseSampleInfoFromMap(dec_params, aud_cfg.sample_info);
    mc.type = DrmType::Audio;
    auto decoder = REFLECTOR(Decoder)::Create<AudioDecoder>(ccodec_name, dec_param_str.c_str());
    if (!decoder) {
        DRM_MEDIA_LOGI("Fail to create audio decoder %s<%s>", ccodec_name, dec_param_str.c_str());
        SetError(-EINVAL);
        return;
    }

    if (!decoder->InitConfig(mc)) {
        DRM_MEDIA_LOGI("Fail to init config, %s", ccodec_name);
        SetError(-EINVAL);
        return;
    }

    dec = decoder;
    input_size = dec->GetNbSamples() * GetSampleSize(mc.aud_cfg.sample_info);

    SlotMap sm;
    sm.input_slots.push_back(0);
    sm.output_slots.push_back(0);

    sm.process = decode;
    sm.thread_model = Model::ASYNCCOMMON;
    sm.mode_when_full = InputMode::BLOCKING;
    sm.input_maxcachenum.push_back(10);
    if (!InstallSlotMap(sm, "AudioDecoderFlow", 40)) {
        DRM_MEDIA_LOGI("Fail to InstallSlotMap for AudioDecoderFlow");
        SetError(-EINVAL);
        return;
    }

    SetFlowTag("AudioDecoderFlow");
}

int AudioDecoderFlow::GetInputSize()
{
    return input_size;
}

DEFINE_FLOW_FACTORY(AudioDecoderFlow, Flow)

const char *FACTORY(AudioDecoderFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_ANYTHING;
}

const char *FACTORY(AudioDecoderFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
