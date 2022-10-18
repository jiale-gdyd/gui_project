#include <media/utils/buffer.h>
#include <media/utils/filter.h>

namespace libdrm {
class ThroughGuard : public Filter {
public:
    ThroughGuard(const char *param);
    virtual ~ThroughGuard() = default;

    static const char *GetFilterName() {
        return "through_guard";
    }

    virtual int Process(std::shared_ptr<MediaBuffer> input, std::shared_ptr<MediaBuffer> &output) override;
    virtual int IoCtrl(unsigned long int request, ...) override;

private:
    int allow_through_count;
};

ThroughGuard::ThroughGuard(const char *param) : allow_through_count(0)
{
    std::map<std::string, std::string> params;
    if (!parse_media_param_map(param, params)) {
        SetError(-EINVAL);
        return;
    }

    std::string value = params[DRM_KEY_ALLOW_THROUGH_COUNT];
    if (!value.empty()) {
        allow_through_count = std::stoi(value);
    }
}

int ThroughGuard::Process(std::shared_ptr<MediaBuffer> input, std::shared_ptr<MediaBuffer> &output)
{
    if (allow_through_count > 0) {
        DRM_MEDIA_LOGI("Process allow through count %d", allow_through_count);

        output = input;
        allow_through_count--;
        return 0;
    }

    return 1;
}

int ThroughGuard::IoCtrl(unsigned long int request, ...)
{
    va_list vl;

    va_start(vl, request);
    void *arg = va_arg(vl, void *);
    va_end(vl);

    if (!arg) {
        return -1;
    }

    int ret = 0;
    switch (request) {
        case S_ALLOW_THROUGH_COUNT:
            allow_through_count = *((int *)arg);
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

DEFINE_COMMON_FILTER_FACTORY(ThroughGuard)

const char *FACTORY(ThroughGuard)::ExpectedInputDataType()
{
    return DRM_TYPE_ANYTHING;
}

const char *FACTORY(ThroughGuard)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
