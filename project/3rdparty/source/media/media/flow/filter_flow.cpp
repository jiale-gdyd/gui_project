#include <linux/kconfig.h>
#include <assert.h>

#include <media/utils/flow.h>
#include <media/utils/image.h>
#include <media/utils/buffer.h>
#include <media/utils/filter.h>
#include <media/utils/key_string.h>

#if defined(CONFIG_RKRGA)
#if defined(CONFIG_RKRGA_OLD)
#include <rockchip/rkrga/rga.h>
#include <rockchip/rkrga/im2d.h>
#else
#include <rockchip/rkrgax/rga.h>
#include <rockchip/rkrgax/im2d.h>
#endif
#endif

namespace libdrm {
static bool do_filters(Flow *f, MediaBufferVector &input_vector);

class FilterFlow : public Flow {
public:
    FilterFlow(const char *param);
    virtual ~FilterFlow() {
        AutoPrintLine apl(__func__);
        StopAllThread();
    }

    static const char *GetFlowName() {
        return "filter";
    }

    virtual int Control(unsigned long int request, ...) final {
        int i = 0;
        int ret = 0;

        if (!filters.size()) {
            return -1;
        }

        for (auto &filter : filters) {
            va_list vl;
            va_start(vl, request);
            void *arg = va_arg(vl, void *);
            va_end(vl);

            switch (request) {
                case G_RGA_REGION_LUMA: {
                    DrmImageRegionLuma *p = (DrmImageRegionLuma *)arg;
                    if (p->priv == i)
                    ret |= filter->IoCtrl(request, arg);
                    break;
                }

                case S_RGA_OSD_INFO: {
                    DrmImageOsd *osd = (DrmImageOsd *)arg;
                    if (osd->priv == i) {
                        ret |= filter->IoCtrl(request, arg);
                    }
                    break;
                }

                case S_RGA_SHOW:
                case S_RGA_HIDE: {
                    int *chn = (int *)arg;
                    if (*chn == i) {
                        ret |= filter->IoCtrl(request, arg);
                    }
                    break;
                }

                case S_RGA_LINE_INFO: {
                    DrmImageBorder *line = (DrmImageBorder *)arg;
                    if (line->priv == i) {
                        ret |= filter->IoCtrl(request, arg);
                    }
                    break;
                }

                default:
                    ret |= filter->IoCtrl(request, arg);
                    break;
            }

            i++;
        }

        return ret;
    }

private:
    std::vector<std::shared_ptr<Filter>> filters;
    bool                                 support_async;
    Model                                thread_model;
    DrmPixelFormat                       input_pix_fmt;
    DrmImageInfo                         out_img_info;
    std::shared_ptr<BufferPool>          buffer_pool;

    friend bool do_filters(Flow *f, MediaBufferVector &input_vector);
};

FilterFlow::FilterFlow(const char *param) : support_async(true), thread_model(Model::NONE), input_pix_fmt(DRM_PIX_FMT_NONE)
{
    memset(&out_img_info, 0x00, sizeof(out_img_info));
    out_img_info.pix_fmt = DRM_PIX_FMT_NONE;

    std::list<std::string> separate_list;
    std::map<std::string, std::string> params;

    if (!ParseWrapFlowParams(param, params, separate_list)) {
        SetError(-EINVAL);
        return;
    }

    std::string &name = params[DRM_KEY_NAME];
    const char *filter_name = name.c_str();

    std::string &&rule = gen_datatype_rule(params);
    if (!rule.empty()) {
        if (!REFLECTOR(Filter)::IsMatch(filter_name, rule.c_str())) {
            DRM_MEDIA_LOGE("Unsupport for filter %s : [%s]", filter_name, rule.c_str());
            SetError(-EINVAL);
            return;
        }
    }

    input_pix_fmt = StringToPixFmt(params[DRM_KEY_INPUTDATATYPE].c_str());

    SlotMap sm;
    int input_maxcachenum = 2;

    ParseParamToSlotMap(params, sm, input_maxcachenum);
    if (sm.thread_model == Model::NONE) {
        sm.thread_model = !params[DRM_KEY_FPS].empty() ? Model::ASYNCATOMIC : Model::SYNC;
    }

    thread_model = sm.thread_model;
    if (sm.mode_when_full == InputMode::NONE) {
        sm.mode_when_full = InputMode::DROPCURRENT;
    }

    int input_idx = 0;
    for (auto &param_str : separate_list) {
        auto filter = REFLECTOR(Filter)::Create<Filter>(filter_name, param_str.c_str());
        if (!filter) {
            DRM_MEDIA_LOGE("Fail to create filter %s<%s>", filter_name, param_str.c_str());
            SetError(-EINVAL);
            return;
        }

        filters.push_back(filter);
        sm.input_slots.push_back(input_idx++);
        sm.input_maxcachenum.push_back(input_maxcachenum);
    }

    sm.output_slots.push_back(0);
    auto &hold = params[DRM_KEY_OUTPUT_HOLD_INPUT];
    if (!hold.empty()) {
        sm.hold_input.push_back((HoldInputMode)std::stoi(hold));
    }

    sm.process = do_filters;
    std::string tag = "FilterFlow:";
    tag.append(filter_name);
    if (!InstallSlotMap(sm, tag, -1)) {
        DRM_MEDIA_LOGE("Fail to InstallSlotMap for %s", tag.c_str());
        SetError(-EINVAL);
        return;
    }

    SetFlowTag(tag);

    if (filters[0]->SendInput(nullptr) == -1 && errno == ENOSYS) {
        support_async = false;
        if (input_pix_fmt != DRM_PIX_FMT_NONE && !ParseImageInfoFromMap(params, out_img_info, false)) {
            if (filters.size() > 1) {
                DRM_MEDIA_LOGE("missing out image info for multi filters");
                SetError(-EINVAL);
                return;
            }
        }

        std::string &mem_type = params[DRM_KEY_MEM_TYPE];
        std::string &mem_cnt = params[DRM_KEY_MEM_CNT];
        if ((input_pix_fmt != DRM_PIX_FMT_NONE) && (out_img_info.vir_height > 0) && (out_img_info.vir_width > 0) && (!mem_type.empty()) && (!mem_cnt.empty())) {
            DRM_MEDIA_LOGI("%s: Enable BufferPool! memtype:%s, memcnt:%s", tag.c_str(), mem_type.c_str(), mem_cnt.c_str());

            int m_cnt = std::stoi(mem_cnt);
            if (m_cnt <= 0) {
                DRM_MEDIA_LOGE("%s: mem_cnt %s invalid", tag.c_str(), mem_cnt.c_str());
                SetError(-EINVAL);
                return;
            }

            size_t m_size = CalPixFmtSize(out_img_info, 16);
            MediaBuffer::MemType m_type = StringToMemType(mem_type.c_str());

            buffer_pool = std::make_shared<BufferPool>(m_cnt, m_size, m_type);

            for (int i = 0; i < m_cnt; i++) {
                auto mb = buffer_pool->GetBuffer(false);
                if (!mb) {
                    DRM_MEDIA_LOGE("%s: buffer_pool get null buffer", GetFlowTag());
                    return;
                }
            }
        }
    } else {
        support_async = true;
        return;
    }
}

bool do_filters(Flow *f, MediaBufferVector &input_vector)
{
    FilterFlow *flow = static_cast<FilterFlow *>(f);

    int i = 0;
    bool has_valid_input = false;

    std::shared_ptr<Filter> last_filter;
    assert(flow->filters.size() == input_vector.size());

    for (auto &in : input_vector) {
        if (in) {
            has_valid_input = true;
            break;
        }
    }

    if (!has_valid_input) {
        return false;
    }

    std::shared_ptr<MediaBuffer> out_buffer;

    if (!flow->support_async) {
        const auto &info = flow->out_img_info;
        if (info.pix_fmt == DRM_PIX_FMT_NONE) {
            out_buffer = std::make_shared<MediaBuffer>();
        } else {
            if (info.vir_width > 0 && info.vir_height > 0) {
                if (flow->buffer_pool) {
                    auto mb = flow->buffer_pool->GetBuffer(false);
                    if (!mb) {
                        DRM_MEDIA_LOGE("%s: buffer_pool get null buffer", flow->GetFlowTag());
                        return false;
                    }

                    out_buffer = std::make_shared<ImageBuffer>(*(mb.get()), info);
                } else {
                    size_t size = CalPixFmtSize(info, 16);
                    auto &&mb = MediaBuffer::Alloc2(size, MediaBuffer::MemType::MEM_HARD_WARE);
                    out_buffer = std::make_shared<ImageBuffer>(mb, info);
                }
            } else {
                auto ib = std::make_shared<ImageBuffer>();
                if (ib) {
                    ib->GetImageInfo().pix_fmt = info.pix_fmt;
                    out_buffer = ib;
                }
            }
        }
    }

    for (auto &filter : flow->filters) {
        auto &in = input_vector[i];
        if (!in) {
            i++;
            continue;
        }

        last_filter = filter;

        if (flow->support_async) {
            int ret = filter->SendInput(in);
            if (ret == -EAGAIN) {
                flow->SendInput(in, i);
            }
        } else {
            if (filter->Process(in, out_buffer)) {
                return false;
            }

            if (in && out_buffer) {
                out_buffer->TimeStampCopy(in);
            }
        }

        i++;
    }

    bool ret = false;
    if (!flow->support_async) {
        ret = flow->SetOutput(out_buffer, 0);
    } else {
        do {
            auto out = last_filter->FetchOutput();
            if (!out) {
                break;
            }

            if (flow->SetOutput(out, 0)) {
                ret = true;
            }
        } while (true);
    }

    return ret;
}

DEFINE_FLOW_FACTORY(FilterFlow, Flow)

const char *FACTORY(FilterFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_ANYTHING;
}

const char *FACTORY(FilterFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
