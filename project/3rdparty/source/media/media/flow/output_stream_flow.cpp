#include <media/utils/flow.h>
#include <media/utils/stream.h>
#include <media/utils/media_type.h>

namespace libdrm {
static bool send_buffer(Flow *f, MediaBufferVector &input_vector);

class OutPutStreamFlow : public Flow {
public:
    OutPutStreamFlow(const char *param);

    virtual ~OutPutStreamFlow() {
        AutoPrintLine apl(__func__);
        StopAllThread();
    };

    static const char *GetFlowName() {
        return "output_stream";
    }

    virtual int Control(unsigned long int request, ...) final {
        if (!out_stream) {
            return -1;
        }

        va_list vl;
        va_start(vl, request);
        void *arg = va_arg(vl, void *);
        va_end(vl);

        return out_stream->IoCtrl(request, arg);
    }

private:
    std::shared_ptr<Stream> out_stream;
    friend bool send_buffer(Flow *f, MediaBufferVector &input_vector);
};

OutPutStreamFlow::OutPutStreamFlow(const char *param)
{
    std::list<std::string> separate_list;
    std::map<std::string, std::string> params;

    if (!ParseWrapFlowParams(param, params, separate_list)) {
        SetError(-EINVAL);
        return;
    }

    std::string &name = params[DRM_KEY_NAME];
    const char *stream_name = name.c_str();

    SlotMap sm;
    int input_maxcachenum = 10;

    ParseParamToSlotMap(params, sm, input_maxcachenum);
    if (sm.thread_model == Model::NONE) {
        sm.thread_model = !params[DRM_KEY_FPS].empty() ? Model::ASYNCATOMIC : Model::ASYNCCOMMON;
    }

    if (sm.mode_when_full == InputMode::NONE) {
        sm.mode_when_full = InputMode::DROPCURRENT;
    }

    const std::string &stream_param = separate_list.back();
    auto stream = REFLECTOR(Stream)::Create<Stream>(stream_name, stream_param.c_str());
    if (!stream) {
        DRM_MEDIA_LOGE("Fail to create stream:[%s]", stream_name);
        SetError(-EINVAL);
        return;
    }

    sm.input_slots.push_back(0);
    sm.input_maxcachenum.push_back(input_maxcachenum);
    sm.process = send_buffer;

    std::string tag = "OutputStreamFlow:";
    tag.append(stream_name);
    if (!InstallSlotMap(sm, tag, -1)) {
        DRM_MEDIA_LOGE("Fail to InstallSlotMap for %s", tag.c_str());
        SetError(-EINVAL);
        return;
    }

    out_stream = stream;
    SetFlowTag(tag);
}

bool send_buffer(Flow *f, MediaBufferVector &input_vector)
{
    OutPutStreamFlow *flow = static_cast<OutPutStreamFlow *>(f);
    auto &buffer = input_vector[0];
    if (!buffer) {
        return true;
    }

    return flow->out_stream->Write(buffer);
}

DEFINE_FLOW_FACTORY(OutPutStreamFlow, Flow)

const char *FACTORY(OutPutStreamFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_ANYTHING;
}

const char *FACTORY(OutPutStreamFlow)::OutPutDataType()
{
    return DRM_TYPE_NOTHING;
}
}
