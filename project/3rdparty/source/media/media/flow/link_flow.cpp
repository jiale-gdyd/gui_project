#include <media/utils/flow.h>
#include <media/utils/utils.h>
#include <media/utils/buffer.h>
#include <media/utils/stream.h>
#include <media/utils/link_config.h>

namespace libdrm {
static bool process_buffer(Flow *f, MediaBufferVector &input_vector);

class LinkFlow : public Flow {
public:
    LinkFlow(const char *param);
    virtual ~LinkFlow();

    static const char *GetFlowName() {
        return "link_flow";
    }

private:
    friend bool process_buffer(Flow *f, MediaBufferVector &input_vector);

private:
    LinkType    link_type_;
    std::string extra_data;
};

LinkFlow::LinkFlow(const char *param)
{
    std::map<std::string, std::string> params;
    if (!parse_media_param_map(param, params)) {
        SetError(-EINVAL);
        return;
    }

    SetVideoHandler(nullptr);
    SetAudioHandler(nullptr);
    SetCaptureHandler(nullptr);
    SetUserCallBack(nullptr, nullptr);

    SlotMap sm;
    sm.input_slots.push_back(0);
    if (sm.thread_model == Model::NONE) {
        sm.thread_model = !params[DRM_KEY_FPS].empty() ? Model::ASYNCATOMIC : Model::ASYNCCOMMON;
    }

    if (sm.mode_when_full == InputMode::NONE) {
        sm.mode_when_full = InputMode::DROPCURRENT;
    }

    sm.input_maxcachenum.push_back(0);
    sm.process = process_buffer;

    if (!InstallSlotMap(sm, "LinkFLow", 0)) {
        DRM_MEDIA_LOGE("Fail to InstallSlotMap for LinkFLow");
        return;
    }

    SetFlowTag("LinkFLow");
    std::string &type = params[DRM_KEY_INPUTDATATYPE];

    link_type_ = LINK_NONE;
    if (type.find(DRM_VIDEO_PREFIX) != std::string::npos) {
        link_type_ = LINK_VIDEO;
    } else if (type.find(DRM_AUDIO_PREFIX) != std::string::npos) {
        link_type_ = LINK_AUDIO;
    } else if (type.find(DRM_IMAGE_PREFIX) != std::string::npos) {
        link_type_ = LINK_PICTURE;
    }
}

LinkFlow::~LinkFlow()
{
    StopAllThread();
}

bool process_buffer(Flow *f, MediaBufferVector &input_vector)
{
    LinkFlow *flow = static_cast<LinkFlow *>(f);

    auto &buffer = input_vector[0];
    if (!buffer || !flow) {
        return false;
    }

    if (flow->link_type_ == LINK_VIDEO) {
        auto link_handler = flow->GetVideoHandler();
        auto nal_type = (buffer->GetUserFlag() & MediaBuffer::kIntra) ? 1 : 0;
        if (link_handler) {
            link_handler((unsigned char *)buffer->GetPtr(), buffer->GetValidSize(), buffer->GetUSTimeStamp(), nal_type);
        }
    } else if (flow->link_type_ == LINK_AUDIO) {
        auto link_audio_handler = flow->GetAudioHandler();
        if (link_audio_handler) {
            link_audio_handler((unsigned char *)buffer->GetPtr(), buffer->GetValidSize(), buffer->GetUSTimeStamp());
        }
    } else if (flow->link_type_ == LINK_PICTURE) {
        auto link_handler = flow->GetCaptureHandler();
        if (link_handler) {
            link_handler((unsigned char *)buffer->GetPtr(), buffer->GetValidSize(), 0, NULL);
        }
    }

    return false;
}

DEFINE_FLOW_FACTORY(LinkFlow, Flow)

const char *FACTORY(LinkFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_NOTHING;
}

const char *FACTORY(LinkFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
