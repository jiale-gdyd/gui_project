#include <assert.h>

#include <media/utils/flow.h>
#include <media/utils/buffer.h>
#include <media/utils/encoder.h>
#include <media/utils/media_type.h>

#define ENC_CONST_MAX_TIME          12000

namespace libdrm {
static bool encode(Flow *f, MediaBufferVector &input_vector);

class VideoEncoderFlow : public Flow {
public:
    VideoEncoderFlow(const char *param);
    virtual ~VideoEncoderFlow() {
        AutoPrintLine apl(__func__);
        StopAllThread();
    }

    static const char *GetFlowName() {
        return "video_enc";
    }

    int Control(unsigned long int request, ...);
    void Dump(std::string &dump_info) override;

    DrmMediaConfig GetConfig() {
        DrmMediaConfig cfg;
        memset(&cfg, 0, sizeof(cfg));
        if (enc) {
            cfg = enc->GetConfig();
        }

        return cfg;
    }

private:
    std::shared_ptr<VideoEncoder>           enc;
    bool                                    extra_output;
    bool                                    extra_merge;
    std::list<std::shared_ptr<MediaBuffer>> extra_buffer_list;

    friend bool encode(Flow *f, MediaBufferVector &input_vector);
};

bool encode(Flow *f, MediaBufferVector &input_vector)
{
    VideoEncoderFlow *vf = (VideoEncoderFlow *)f;
    std::shared_ptr<VideoEncoder> enc = vf->enc;
    std::shared_ptr<MediaBuffer> &src = input_vector[0];
    std::shared_ptr<MediaBuffer> dst, extra_dst;

    if (!src) {
        return false;
    }

    dst = std::make_shared<MediaBuffer>();
    if (!dst) {
        DRM_LOG_MEMORY();
        return false;
    }

    if (vf->extra_output) {
        extra_dst = std::make_shared<MediaBuffer>();
        if (!extra_dst) {
            DRM_LOG_MEMORY();
            return false;
        }
    }

    if (0 != enc->Process(src, dst, extra_dst)) {
        DRM_MEDIA_LOGI("encoder failed");
        return false;
    }

    bool ret = true;

    if (dst->GetValidSize() > 0) {
        dst = MediaBuffer::Clone(*dst);
        if (src && dst) {
            dst->TimeStampCopy(src);
        }

        ret = vf->SetOutput(dst, 0);
        if (vf->extra_output) {
            ret &= vf->SetOutput(extra_dst, 1);
        }
    }

    return ret;
}

VideoEncoderFlow::VideoEncoderFlow(const char *param) : extra_output(false), extra_merge(false)
{
    std::list<std::string> separate_list;
    std::map<std::string, std::string> params;

    DRM_MEDIA_LOGD("VEnc Flow: dump param:%s", param);
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

    std::string &extra_merge_value = params[DRM_KEY_NEED_EXTRA_MERGE];
    if (!extra_merge_value.empty()) {
        extra_merge = !!std::stoi(extra_merge_value);
    }

    const char *ccodec_name = codec_name.c_str();
    std::string &&rule = gen_datatype_rule(params);
    if (rule.empty()) {
        SetError(-EINVAL);
        return;
    }

    if (!REFLECTOR(Encoder)::IsMatch(ccodec_name, rule.c_str())) {
        DRM_MEDIA_LOGI("Unsupport for video encoder %s : [%s]", ccodec_name, rule.c_str());
        SetError(-EINVAL);
        return;
    }

    std::string &enc_param_str = separate_list.back();
    std::map<std::string, std::string> enc_params;

    if (!parse_media_param_map(enc_param_str.c_str(), enc_params)) {
        SetError(-EINVAL);
        return;
    }

    std::string::size_type idx;
    idx = enc_param_str.find(DRM_KEY_OUTPUTDATATYPE);
    if (idx == enc_param_str.npos) {
        PARAM_STRING_APPEND(enc_param_str, DRM_KEY_OUTPUTDATATYPE, params[DRM_KEY_OUTPUTDATATYPE]);
    }

    if (enc_params[DRM_KEY_INPUTDATATYPE].empty()) {
        enc_params[DRM_KEY_INPUTDATATYPE] = params[DRM_KEY_INPUTDATATYPE];
    }

    if (enc_params[DRM_KEY_OUTPUTDATATYPE].empty()) {
        enc_params[DRM_KEY_OUTPUTDATATYPE] = params[DRM_KEY_OUTPUTDATATYPE];
    }

    DrmMediaConfig mc;
    memset(&mc, 0, sizeof(DrmMediaConfig));
    if (!ParseMediaConfigFromMap(enc_params, mc)) {
        SetError(-EINVAL);
        return;
    }

    auto encoder = REFLECTOR(Encoder)::Create<VideoEncoder>(ccodec_name, enc_param_str.c_str());
    if (!encoder) {
        DRM_MEDIA_LOGI("Fail to create video encoder %s<%s>", ccodec_name, enc_param_str.c_str());
        SetError(-EINVAL);
        return;
    }

    if (!encoder->InitConfig(mc)) {
        DRM_MEDIA_LOGI("Fail to init config, %s", ccodec_name);
        SetError(-EINVAL);
        return;
    }

    std::string roi_region_str = enc_params[DRM_KEY_ROI_REGIONS];
    if (!roi_region_str.empty()) {
        int roi_regions_cnt = 0;
        std::vector<DrmEncROIRegion> roi_regions;

        roi_regions = StringToRoiRegions(roi_region_str);
        roi_regions_cnt = roi_regions.size();
        if (roi_regions_cnt) {
            DrmEncROIRegion *regions = (DrmEncROIRegion *)malloc(roi_regions_cnt * sizeof(DrmEncROIRegion));
            for (int i = 0; i < roi_regions_cnt; i++) {
                (regions + i)->x = roi_regions[i].x;
                (regions + i)->y = roi_regions[i].y;
                (regions + i)->w = roi_regions[i].w;
                (regions + i)->h = roi_regions[i].h;
                (regions + i)->intra = roi_regions[i].intra;
                (regions + i)->quality = roi_regions[i].quality;
                (regions + i)->qp_area_idx = roi_regions[i].qp_area_idx;
                (regions + i)->area_map_en = roi_regions[i].area_map_en;
                (regions + i)->abs_qp_en = roi_regions[i].abs_qp_en;

                DRM_MEDIA_LOGI(
                    "VEnc Flow: Roi Regions[%d]: (%d,%d,%d,%d,%d,%d,%d,%d,%d)", i,
                    roi_regions[i].x, roi_regions[i].y, roi_regions[i].w,
                    roi_regions[i].h, roi_regions[i].intra, roi_regions[i].quality,
                    roi_regions[i].qp_area_idx, roi_regions[i].area_map_en,
                    roi_regions[i].abs_qp_en);
            }

            auto pbuff = std::make_shared<ParameterBuffer>(0);
            pbuff->SetPtr(regions, roi_regions_cnt * sizeof(DrmEncROIRegion));
            encoder->RequestChange(VideoEncoder::kROICfgChange, pbuff);
        }
    }

    void *extra_data = nullptr;
    size_t extra_data_size = 0;
    encoder->GetExtraData(&extra_data, &extra_data_size);
    const std::string &output_dt = enc_params[DRM_KEY_OUTPUTDATATYPE];

    enc = encoder;

    SlotMap sm;
    sm.input_slots.push_back(0);
    sm.output_slots.push_back(0);
    if (params[DRM_KEY_NEED_EXTRA_OUTPUT] == "y") {
        extra_output = true;
        sm.output_slots.push_back(1);
    }

    sm.process = encode;
    sm.thread_model = Model::ASYNCCOMMON;
    sm.mode_when_full = InputMode::DROPFRONT;
    sm.input_maxcachenum.push_back(3);
    if (!InstallSlotMap(sm, "VideoEncoderFlow", 40)) {
        DRM_MEDIA_LOGI("Fail to InstallSlotMap, %s", ccodec_name);
        SetError(-EINVAL);
        return;
    }
    SetFlowTag("VideoEncoderFlow");

    if (extra_data && extra_data_size > 0 && (output_dt == DRM_VIDEO_H264 || output_dt == DRM_VIDEO_H265)) {
        if (extra_merge) {
            std::shared_ptr<MediaBuffer> extra_buf = std::make_shared<MediaBuffer>();
            extra_buf->SetPtr(extra_data);
            extra_buf->SetValidSize(extra_data_size);
            extra_buf->SetUserFlag(MediaBuffer::kExtraIntra);
            SetOutput(extra_buf, 0);
        } else {
            if (output_dt == DRM_VIDEO_H264) {
                extra_buffer_list = split_h264_separate((const uint8_t *)extra_data, extra_data_size, gettimeofday());
            } else {
                extra_buffer_list = split_h265_separate((const uint8_t *)extra_data, extra_data_size, gettimeofday());
            }

            for (auto &extra_buffer : extra_buffer_list) {
                assert(extra_buffer->GetUserFlag() & MediaBuffer::kExtraIntra);
                SetOutput(extra_buffer, 0);
            }
        }

        if (extra_output) {
            std::shared_ptr<MediaBuffer> nullbuffer;
            SetOutput(nullbuffer, 1);
        }
    }
}

int VideoEncoderFlow::Control(unsigned long int request, ...)
{
    va_list ap;
    va_start(ap, request);
    auto value = va_arg(ap, std::shared_ptr<ParameterBuffer>);
    va_end(ap);

    assert(value);

    enc->RequestChange(request, value);
    return 0;
}

void VideoEncoderFlow::Dump(std::string &dump_info)
{
    char str_line[1024] = {0};
    const DrmMediaConfig mc = GetConfig();

    DumpBase(dump_info);
    sprintf(str_line, "#Dump Flow(%s) advanced info:\r\n", GetFlowTag());
    dump_info.append(str_line);

    memset(str_line, 0, sizeof(str_line));
    sprintf(str_line, "  Name:%s\r\n", GetFlowName());
    dump_info.append(str_line);

    memset(str_line, 0, sizeof(str_line));
    if (mc.type == DrmType::Image) {
        dump_info.append("  CodecType: JPEG\r\n");
        sprintf(str_line, "  Input: %d(%d)x%d(%d) fmt:%s\r\n",
                mc.img_cfg.image_info.width, mc.img_cfg.image_info.vir_width,
                mc.img_cfg.image_info.height, mc.img_cfg.image_info.vir_height,
                PixFmtToString(mc.img_cfg.image_info.pix_fmt));
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  Qfactor:%d\n", mc.img_cfg.qfactor);
        dump_info.append(str_line);
    } else if (mc.type == DrmType::Video) {
        const DrmVideoConfig &vcfg = mc.vid_cfg;
        const DrmImageConfig &imgcfg = vcfg.image_cfg;

        if (imgcfg.codec_type == DRM_CODEC_TYPE_H264) {
            dump_info.append("  CodecType: H264\r\n");
        } else if (imgcfg.codec_type == DRM_CODEC_TYPE_H265) {
            dump_info.append("  CodecType: H265\r\n");
        } else {
            DRM_MEDIA_LOGE("VEnc Flow: config fatal error!\n");
            return;
        }

        sprintf(str_line, "  Input: %d(%d)x%d(%d) fmt:%s\r\n",
                imgcfg.image_info.width, imgcfg.image_info.vir_width,
                imgcfg.image_info.height, imgcfg.image_info.vir_height,
                PixFmtToString(imgcfg.image_info.pix_fmt));
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(
            str_line,
            "  QpArray: init:%d min:%d, max:%d, step:%d, min_i:%d, max_i:%d\r\n",
            vcfg.encode_qp.qp_init, vcfg.encode_qp.qp_min, vcfg.encode_qp.qp_max,
            vcfg.encode_qp.qp_step, vcfg.encode_qp.qp_min_i,
            vcfg.encode_qp.qp_max_i);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  BitRate: target:%d, min:%d, max:%d\r\n", vcfg.bit_rate, vcfg.bit_rate_min, vcfg.bit_rate_max);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  FrameRate: in:%d/%d, out:%d/%d\r\n", vcfg.frame_in_rate, vcfg.frame_in_rate_den, vcfg.frame_rate, vcfg.frame_rate_den);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  GopSize: %d\r\n", vcfg.gop_size);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  RcQuality: %s\r\n", vcfg.rc_quality);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  RcMode: %s\r\n", vcfg.rc_mode);
        dump_info.append(str_line);

        memset(str_line, 0, sizeof(str_line));
        sprintf(str_line, "  FullRange: %s\r\n", vcfg.full_range ? "Enable" : "Disable");
        dump_info.append(str_line);

        if (imgcfg.codec_type == DRM_CODEC_TYPE_H264) {
            memset(str_line, 0, sizeof(str_line));
            sprintf(str_line, "  Trans8x8: %s\r\n", vcfg.trans_8x8 ? "Enable" : "Disable");
            dump_info.append(str_line);

            memset(str_line, 0, sizeof(str_line));
            sprintf(str_line, "  H264Level: %d\r\n", vcfg.level);
            dump_info.append(str_line);

            memset(str_line, 0, sizeof(str_line));
            sprintf(str_line, "  H264Profile: %d\r\n", vcfg.profile);
            dump_info.append(str_line);
        }
    } else {
        DRM_MEDIA_LOGE("VEnc Flow: Dump: to do...");
        return;
    }
}

DEFINE_FLOW_FACTORY(VideoEncoderFlow, Flow)

const char *FACTORY(VideoEncoderFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_ANYTHING;
}

const char *FACTORY(VideoEncoderFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}
}
