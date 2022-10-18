#include <sys/prctl.h>
#include <media/utils/flow.h>
#include <media/utils/utils.h>
#include <media/utils/buffer.h>
#include <media/utils/stream.h>

namespace libdrm {
class SourceStreamFlow : public Flow {
public:
    SourceStreamFlow(const char *param);
    virtual ~SourceStreamFlow();

    static const char *GetFlowName() {
        return "source_stream";
    }

    virtual int Control(unsigned long int request, ...) final {
        if (!stream) {
            return -1;
        }

        va_list vl;
        va_start(vl, request);
        void *arg = va_arg(vl, void *);
        va_end(vl);

        return stream->IoCtrl(request, arg);
    }

private:
    void ReadThreadRun();

    bool                    loop;
    std::thread             *read_thread;
    std::shared_ptr<Stream> stream;
    std::string             tag;
};

SourceStreamFlow::SourceStreamFlow(const char *param) : loop(false), read_thread(nullptr)
{
    std::list<std::string> separate_list;
    std::map<std::string, std::string> params;

    if (!ParseWrapFlowParams(param, params, separate_list)) {
        SetError(-EINVAL);
        return;
    }

    std::string &name = params[DRM_KEY_NAME];
    const char *stream_name = name.c_str();
    const std::string &stream_param = separate_list.back();

    stream = REFLECTOR(Stream)::Create<Stream>(stream_name, stream_param.c_str());
    if (!stream) {
        DRM_MEDIA_LOGE("Create stream %s failed", stream_name);
        SetError(-EINVAL);
        return;
    }

    tag = "SourceFlow:";
    tag.append(name);
    if (!SetAsSource(std::vector<int>({0}), void_transaction00, tag)) {
        SetError(-EINVAL);
        return;
    }

    loop = true;
    read_thread = new std::thread(&SourceStreamFlow::ReadThreadRun, this);
    if (!read_thread) {
        loop = false;
        SetError(-EINVAL);
        return;
    }

    SetFlowTag(tag);
}

SourceStreamFlow::~SourceStreamFlow()
{
    loop = false;
    StopAllThread();

    int stop = 1;
    if (stream && Control(S_STREAM_OFF, &stop)) {
        DRM_MEDIA_LOGE("Fail to stop source stream");
    }

    DRM_MEDIA_LOGD("SourceStreamFlow:[%s]: stream off....", GetFlowTag());
    if (read_thread) {
        source_start_cond_mtx->lock();
        loop = false;
        source_start_cond_mtx->notify();
        source_start_cond_mtx->unlock();
        read_thread->join();
        delete read_thread;
    }

    DRM_MEDIA_LOGI("SourceStreamFlow:[%s]: read thread exit sucessfully", GetFlowTag());
    stream.reset();
    DRM_MEDIA_LOGI("SourceStreamFlow:[%s]: stream reset sucessfully", GetFlowTag());
}

void SourceStreamFlow::ReadThreadRun()
{
    prctl(PR_SET_NAME, this->tag.c_str());
    source_start_cond_mtx->lock();
    if (waite_down_flow) {
        if (down_flow_num == 0 && IsEnable()) {
            source_start_cond_mtx->wait();
        }
    }

    source_start_cond_mtx->unlock();
    while (loop) {
        if (stream->Eof()) {
            SetDisable();
            break;
        }

        auto buffer = stream->Read();
        if (buffer) {
            buffer->TimeStampReset();
        }

        SendInput(buffer, 0);
    }
}

DEFINE_FLOW_FACTORY(SourceStreamFlow, Flow)
const char *FACTORY(SourceStreamFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_NOTHING;
}

const char *FACTORY(SourceStreamFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
