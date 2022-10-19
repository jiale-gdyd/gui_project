#include <assert.h>
#include <unistd.h>

#include <media/utils/buffer.h>
#include "mpp_decoder.h"

namespace libdrm {
MPPDecoder::MPPDecoder(const char *param)
    : output_format(DRM_PIX_FMT_NONE), fg_limit_num(kFRAMEGROUP_MAX_FRAMES),
    need_split(1), timeout(MPP_POLL_NON_BLOCK), coding_type(MPP_VIDEO_CodingUnused), support_sync(true), support_async(true)
{
    DrmMediaConfig &cfg = GetConfig();
    DrmImageInfo &img_info = cfg.img_cfg.image_info;
    img_info.pix_fmt = DRM_PIX_FMT_NONE;
    cfg.type = DrmType::None;

    std::string stimeout;
    std::string split_mode;
    std::string input_data_type;
    std::string output_data_type;
    std::string limit_max_frame_num;
    std::map<std::string, std::string> params;
    std::list<std::pair<const std::string, std::string &>> req_list;

    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_INPUTDATATYPE, input_data_type));
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_OUTPUTDATATYPE, output_data_type));
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_MPP_GROUP_MAX_FRAMES, limit_max_frame_num));
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_MPP_SPLIT_MODE, split_mode));
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_OUTPUT_TIMEOUT, stimeout));

    int ret = parse_media_param_match(param, params, req_list);
    if (ret == 0 || input_data_type.empty()) {
        DRM_MEDIA_LOGE("missing:[%s]", DRM_KEY_INPUTDATATYPE);
        return;
    }

    coding_type = GetMPPCodingType(input_data_type);
    if (!output_data_type.empty()) {
        output_format = StringToPixFmt(output_data_type.c_str());
        if (output_format == DRM_PIX_FMT_NONE) {
            DRM_MEDIA_LOGE("invalid output format:[%s]", output_data_type.c_str());
            return;
        }
    }

    if (!limit_max_frame_num.empty()) {
        fg_limit_num = std::stoi(limit_max_frame_num);
        if (fg_limit_num > 0 && fg_limit_num <= 3) {
            DRM_MEDIA_LOGI("invalid framegroup limit frame num:[%d]", fg_limit_num);
            return;
        }
    }

    if (!split_mode.empty()) {
        need_split = std::stoi(split_mode);
    }

    if (!stimeout.empty()) {
        int tout = std::stoi(stimeout);
        if (tout == 0) {
            timeout = MPP_POLL_NON_BLOCK;
        } else if (tout < 0) {
            timeout = MPP_POLL_BLOCK;
        } else {
            timeout = (MppPollType)tout;
        }
    }
}

bool MPPDecoder::Init()
{
    if (coding_type == MPP_VIDEO_CodingUnused) {
        return false;
    }

    if (coding_type == MPP_VIDEO_CodingMJPEG) {
        support_async = false;
    }

    mpp_ctx = std::make_shared<MPPContext>();
    if (!mpp_ctx) {
        return false;
    }

    MppCtx ctx = NULL;
    MppApi *mpi = NULL;
    MPP_RET ret = mpp_create(&ctx, &mpi);
    if (MPP_OK != ret) {
        DRM_MEDIA_LOGE("mpp_create failed");
        return false;
    }

    mpp_ctx->ctx = ctx;
    mpp_ctx->mpi = mpi;

    assert(ctx);
    assert(mpi);

    MppParam param = NULL;

    if (need_split) {
        param = &need_split;
        ret = mpi->control(ctx, MPP_DEC_SET_PARSER_SPLIT_MODE, param);
        DRM_MEDIA_LOGI("mpi control MPP_DEC_SET_PARSER_SPLIT_MODE return:[%d]", ret);
        if (MPP_OK != ret) {
            return false;
        }
    }

    ret = mpp_init(ctx, MPP_CTX_DEC, coding_type);
    if (ret != MPP_OK) {
        DRM_MEDIA_LOGE("mpp_init dec failed with type:[%d]", coding_type);
        return false;
    }

    if (timeout != MPP_POLL_NON_BLOCK) {
        RK_U32 to = timeout;

        param = &to;
        ret = mpi->control(ctx, MPP_SET_OUTPUT_TIMEOUT, param);
        DRM_MEDIA_LOGI("mpi set output timeout:[%d], return:[%d]", timeout, ret);
        if (MPP_OK != ret)
            return false;
    }

    if (fg_limit_num > 0) {
        ret = mpp_buffer_group_get_internal(&mpp_ctx->frame_group, MPP_BUFFER_TYPE_ION);
        if (ret != MPP_OK) {
            DRM_MEDIA_LOGE("Failed to retrieve buffer group (return:[%d])", ret);
            return false;
        }

        ret = mpi->control(ctx, MPP_DEC_SET_EXT_BUF_GROUP, mpp_ctx->frame_group);
        if (ret != MPP_OK) {
            DRM_MEDIA_LOGE("Failed to assign buffer group (return:[%d])", ret);
            return false;
        }

        ret = mpp_buffer_group_limit_config(mpp_ctx->frame_group, 0, fg_limit_num);
        if (ret != MPP_OK) {
            DRM_MEDIA_LOGE("Failed to set buffer group limit (return:[%d])", ret);
            return false;
        }

        DRM_MEDIA_LOGI("mpi set group limit:[%d]", fg_limit_num);
    }

    if (coding_type == MPP_VIDEO_CodingMJPEG && output_format != DRM_PIX_FMT_NONE) {
        auto oformat = ConvertToMppPixFmt(output_format);
        if (oformat < 0) {
            DRM_MEDIA_LOGE("unsupport set output format:[%s]", PixFmtToString(output_format));
            return false;
        }

        ret = mpi->control(ctx, MPP_DEC_SET_OUTPUT_FORMAT, &oformat);
        if (ret != MPP_OK) {
            DRM_MEDIA_LOGI("Failed to set output format (return:[%d])", ret);
            return false;
        }
    }

    return true;
}

class MPPFrameContext {
public:
    MPPFrameContext(std::shared_ptr<MPPContext> ctx, MppFrame f) : mctx(ctx), frame(f) {

    }

    ~MPPFrameContext() {
        if (frame) {
            mpp_frame_deinit(&frame);
        }
    }

private:
    std::shared_ptr<MPPContext> mctx;
    MppFrame                    frame;
};

static int __free_mppframecontext(void *p)
{
    assert(p);

    delete (MPPFrameContext *)p;
    return 0;
}

static int SetImageBufferWithMppFrame(std::shared_ptr<ImageBuffer> ib, std::shared_ptr<MPPContext> mctx, MppFrame &frame)
{
    const MppBuffer buffer = mpp_frame_get_buffer(frame);
    if (!buffer || mpp_buffer_get_size(buffer) == 0) {
        DRM_MEDIA_LOGE("Failed to retrieve the frame buffer");
        return -EFAULT;
    }

    DrmImageInfo &info = ib->GetImageInfo();
    info.pix_fmt = ConvertToPixFmt(mpp_frame_get_fmt(frame));
    info.width = mpp_frame_get_width(frame);
    info.height = mpp_frame_get_height(frame);
    info.vir_width = mpp_frame_get_hor_stride(frame);
    info.vir_height = mpp_frame_get_ver_stride(frame);

    size_t size = CalPixFmtSize(info);
    auto pts = mpp_frame_get_pts(frame);
    bool eos = mpp_frame_get_eos(frame) ? true : false;

    if (!ib->IsValid()) {
        MPPFrameContext *ctx = new MPPFrameContext(mctx, frame);
        if (!ctx) {
            DRM_LOG_MEMORY();
            return -ENOMEM;
        }

        ib->SetFD(mpp_buffer_get_fd(buffer));
        ib->SetPtr(mpp_buffer_get_ptr(buffer));
        assert(size <= mpp_buffer_get_size(buffer));
        ib->SetSize(mpp_buffer_get_size(buffer));
        ib->SetUserData(ctx, __free_mppframecontext);
    } else {
        assert(ib->GetSize() >= size);

        if (!ib->IsHwBuffer()) {
        void *ptr = ib->GetPtr();
            assert(ptr);

            DRM_MEDIA_LOGD("extra time-consuming memcpy to cpu");
            memcpy(ptr, mpp_buffer_get_ptr(buffer), size);
        }

        mpp_frame_deinit(&frame);
    }

    ib->SetValidSize(size);
    ib->SetUSTimeStamp(pts);
    ib->SetEOF(eos);

    return 0;
}

int MPPDecoder::Process(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &output, std::shared_ptr<MediaBuffer> extra_output _DRM_UNUSED)
{
    if (!support_sync) {
        errno = ENOSYS;
        return -ENOSYS;
    }

    if (!input || !input->IsValid()) {
        return 0;
    }

    if (!output) {
        return -EINVAL;
    }

    if (output->GetType() != DrmType::Image) {
        DRM_MEDIA_LOGE("mpp decoder output must be image buffer");
        return -EINVAL;
    }

    MPP_RET ret;
    MppTask task = NULL;
    MppFrame frame = NULL;
    MppPacket packet = NULL;
    MppBuffer mpp_buf = NULL;
    MppFrame frame_out = NULL;
    MppCtx ctx = mpp_ctx->ctx;
    MppApi *mpi = mpp_ctx->mpi;
    size_t output_buffer_size = 0;

    ret = init_mpp_buffer_with_content(mpp_buf, input);
    if (ret || !mpp_buf) {
        DRM_MEDIA_LOGE("Failed to init MPP buffer with content (return:[%d])", ret);
        return -EFAULT;
    }

    ret = mpp_packet_init_with_buffer(&packet, mpp_buf);
    mpp_buffer_put(mpp_buf);
    mpp_buf = NULL;
    if (ret != MPP_OK) {
        DRM_MEDIA_LOGI("Failed to init MPP packet with buffer (return:[%d])", ret);
        goto out;
    }

    assert(packet);

    mpp_packet_set_length(packet, input->GetValidSize());
    mpp_packet_set_pts(packet, input->GetUSTimeStamp());
    if (input->IsEOF()) {
        DRM_MEDIA_LOGI("send eos packet to MPP");
        mpp_packet_set_eos(packet);
    }

    output_buffer_size = output->GetSize();
    if (output_buffer_size == 0 || !output->IsHwBuffer()) {
        uint8_t *buffer = static_cast<uint8_t *>(input->GetPtr());
        if (coding_type == MPP_VIDEO_CodingMJPEG && buffer) {
            int a;
            size_t pos = 0;
            int w = 0, h = 0;
            size_t buffer_size = input->GetValidSize();

            if (buffer_size < 4) {
                goto out;
            }

            a = (int)buffer[pos++];
            if (a != 0xFF || buffer[pos++] != 0xD8) {
                DRM_MEDIA_LOGE("input is not jpeg");
                goto out;
            }

            for (; ;) {
                int marker = 0;
                uint16_t ll, lh;
                uint16_t itemlen;

                if (pos > buffer_size - 1) {
                    break;
                }

                for (a = 0; a <= 16 && pos < buffer_size - 1; a++) {
                    marker = (int)buffer[pos++];
                    if (marker != 0xff) {
                        break;
                    }

                    if (a >= 16) {
                        DRM_MEDIA_LOGE("too many padding bytes");
                        goto out;
                    }
                }

                if (marker == 0xD9 || marker == 0xDA) {
                    break;
                }

                if (pos > buffer_size - 2) {
                    break;
                }

                lh = (uint16_t)buffer[pos++];
                ll = (uint16_t)buffer[pos++];

                itemlen = (lh << 8) | ll;
                if (itemlen < 2) {
                    DRM_MEDIA_LOGI("invalid marker");
                    goto out;
                }

                if (pos + itemlen - 2 > buffer_size) {
                    DRM_MEDIA_LOGI("Premature end of jpeg?");
                    goto out;
                }

                if (((marker >> 4) ^ 0xC) == 0 && marker != 0xC4 && marker != 0xC8 && marker != 0xCC) {
                    if (pos + 5 > buffer_size) {
                        DRM_MEDIA_LOGI("Invalid Section Marker:[0x%02X]", marker);
                        goto out;
                    }

                    lh = (uint16_t)buffer[pos + 1];
                    ll = (uint16_t)buffer[pos + 2];
                    h = (lh << 8) | ll;
                    lh = (uint16_t)buffer[pos + 3];
                    ll = (uint16_t)buffer[pos + 4];
                    w = (lh << 8) | ll;
                    break;
                }

                pos += itemlen - 2;
            }

            if (w == 0 || h == 0) {
                DRM_MEDIA_LOGE("can not get width and height of jpeg");
                goto out;
            }

            auto fmt = output_format;
            if (fmt == DRM_PIX_FMT_NONE) {
                fmt = DRM_PIX_FMT_ARGB8888;
            }

            output_buffer_size = CalPixFmtSize(fmt, DRM_UPALIGNTO16(w), DRM_UPALIGNTO16(h)) + DRM_UPALIGNTO16(w) * DRM_UPALIGNTO16(h) / 2;
        } else {
            DRM_MEDIA_LOGE("TODO: mpp need external output buffer now");
            goto out;
        }
    }

    ret = init_mpp_buffer(mpp_buf, output, output_buffer_size);
    if (ret || !mpp_buf) {
        DRM_MEDIA_LOGI("Failed to init MPP buffer (return:[%d])", ret);
        goto out;
    }

    if (mpp_buf) {
        ret = mpp_frame_init(&frame);
        if (MPP_OK != ret) {
            DRM_MEDIA_LOGI("Failed mpp_frame_init (return:[%d])", ret);
            goto out;
        }

        mpp_frame_set_buffer(frame, mpp_buf);
        mpp_buffer_put(mpp_buf);
        mpp_buf = NULL;
    }

    ret = mpi->poll(ctx, MPP_PORT_INPUT, MPP_POLL_MAX);
    if (ret) {
        DRM_MEDIA_LOGE("mpp input poll failed (return:[%d])", ret);
        goto out;
    }

    ret = mpi->dequeue(ctx, MPP_PORT_INPUT, &task);
    if (ret) {
        DRM_MEDIA_LOGE("mpp task input dequeue failed (return:[%d])", ret);
        goto out;
    }

    assert(task);

    mpp_task_meta_set_packet(task, KEY_INPUT_PACKET, packet);
    if (frame) {
        mpp_task_meta_set_frame(task, KEY_OUTPUT_FRAME, frame);
    }

    ret = mpi->enqueue(ctx, MPP_PORT_INPUT, task);
    if (ret) {
        DRM_MEDIA_LOGE("mpp task input enqueue failed (return:[%d])", ret);
        goto out;
    }

    ret = mpi->poll(ctx, MPP_PORT_OUTPUT, timeout != MPP_POLL_NON_BLOCK ? (MppPollType)timeout : MPP_POLL_MAX);
    if (ret) {
        DRM_MEDIA_LOGE("mpp output poll failed (return:[%d])", ret);
        goto out;
    }

    ret = mpi->dequeue(ctx, MPP_PORT_OUTPUT, &task);
    if (ret) {
        DRM_MEDIA_LOGE("mpp task output dequeue failed (return:[%d])", ret);
        goto out;
    }

    mpp_packet_deinit(&packet);
    if (!task) {
        goto out;
    }

    mpp_task_meta_get_frame(task, KEY_OUTPUT_FRAME, &frame_out);
    assert(frame_out == frame);
    ret = mpi->enqueue(ctx, MPP_PORT_OUTPUT, task);
    if (ret) {
        DRM_MEDIA_LOGE("mpp task output enqueue failed");
    }

    if (mpp_frame_get_errinfo(frame_out)) {
        DRM_MEDIA_LOGE("Received a errinfo frame");
        goto out;
    }

    if (SetImageBufferWithMppFrame(std::static_pointer_cast<ImageBuffer>(output), mpp_ctx, frame)) {
        goto out;
    }

    return 0;

out:
    if (mpp_buf) {
        mpp_buffer_put(mpp_buf);
    }

    if (packet) {
        mpp_packet_deinit(&packet);
    }

    if (frame) {
        mpp_frame_deinit(&frame);
    }

    return -EFAULT;
}

int MPPDecoder::SendInput(const std::shared_ptr<MediaBuffer> &input)
{
    if (!support_async) {
        errno = ENOSYS;
        return -ENOSYS;
    }

    if (!input) {
        return 0;
    }

    MPP_RET ret;
    int fret = 0;
    MppPacket packet = NULL;
    void *buffer = input->GetPtr();
    size_t size = input->GetValidSize();

    ret = mpp_packet_init(&packet, buffer, size);
    if (ret != MPP_OK) {
        DRM_MEDIA_LOGE("Failed to init MPP packet (return:[%d])", ret);
        return -EFAULT;
    }

    mpp_packet_set_pts(packet, input->GetUSTimeStamp());
    if (input->IsEOF()) {
        DRM_MEDIA_LOGE("send eos packet to MPP");
        mpp_packet_set_eos(packet);
    }

    ret = mpp_ctx->mpi->decode_put_packet(mpp_ctx->ctx, packet);
    if (ret != MPP_OK) {
        if (ret == MPP_ERR_BUFFER_FULL) {
            fret = -EAGAIN;
        } else {
            DRM_MEDIA_LOGE("Failed to put a packet to MPP (return:[%d])", ret);
            fret = -EFAULT;
        }
    }

    mpp_packet_deinit(&packet);
    return fret;
}

std::shared_ptr<MediaBuffer> MPPDecoder::FetchOutput()
{
    MppFrame mppframe = NULL;
    MppCtx ctx = mpp_ctx->ctx;
    MppApi *mpi = mpp_ctx->mpi;

RETRY_GET_FRAME:
    MPP_RET ret = mpi->decode_get_frame(ctx, &mppframe);
    errno = 0;

    DRM_MEDIA_LOGD("decode_get_frame return:[%d], mpp frame is:[%p]", ret, mppframe);

    if (ret != MPP_OK) {
        if (ret != MPP_ERR_TIMEOUT) {
            DRM_MEDIA_LOGE("Failed to get a frame from MPP (return:[%d])", ret);
        }

        return nullptr;
    }

    if (!mppframe) {
        return nullptr;
    }

    if (mpp_frame_get_info_change(mppframe)) {
        DrmMediaConfig &cfg = GetConfig();
        DrmImageInfo &img_info = cfg.img_cfg.image_info;
        img_info.pix_fmt = ConvertToPixFmt(mpp_frame_get_fmt(mppframe));
        img_info.width = mpp_frame_get_width(mppframe);
        img_info.height = mpp_frame_get_height(mppframe);
        img_info.vir_width = mpp_frame_get_hor_stride(mppframe);
        img_info.vir_height = mpp_frame_get_ver_stride(mppframe);
        mpp_frame_deinit(&mppframe);

        mppframe = NULL;
        cfg.type = DrmType::Image;

        ret = mpi->control(ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
        if (ret != MPP_OK) {
            DRM_MEDIA_LOGE("info change ready failed, return:[%d]", ret);
        }

        DRM_MEDIA_LOGI("MppDec Info change get, %dx%d in (%dx%d)", img_info.width, img_info.height, img_info.vir_width, img_info.vir_height);

        if ((timeout == MPP_POLL_BLOCK) && (need_split == 0)) {
            goto RETRY_GET_FRAME;
        }

        auto mb = std::make_shared<ImageBuffer>();
        if (!mb) {
            errno = ENOMEM;
            goto out;
        }

        mb->GetImageInfo() = img_info;
        return mb;
    } else if (mpp_frame_get_eos(mppframe)) {
        DRM_MEDIA_LOGW("Received EOS frame");

        auto mb = std::make_shared<ImageBuffer>();
        if (!mb) {
            errno = ENOMEM;
            goto out;
        }

        mb->SetUSTimeStamp(mpp_frame_get_pts(mppframe));
        mb->SetEOF(true);
        mpp_frame_deinit(&mppframe);

        return mb;
    } else if (mpp_frame_get_discard(mppframe)) {
        DRM_MEDIA_LOGE("Received a discard frame");
        goto out;
    } else if (mpp_frame_get_errinfo(mppframe)) {
        DRM_MEDIA_LOGE("Received a errinfo frame");
        goto out;
    } else {
        auto mb = std::make_shared<ImageBuffer>();
        if (!mb) {
            errno = ENOMEM;
            goto out;
        }

        if (SetImageBufferWithMppFrame(mb, mpp_ctx, mppframe)) {
            goto out;
        }

        return mb;
    }

out:
    mpp_frame_deinit(&mppframe);
    return nullptr;
}

DEFINE_VIDEO_DECODER_FACTORY(MPPDecoder)

const char *FACTORY(MPPDecoder)::ExpectedInputDataType()
{
    return DRM_TYPENEAR(DRM_IMAGE_JPEG) DRM_TYPENEAR(DRM_VIDEO_H264) DRM_TYPENEAR(DRM_VIDEO_H265);
}

const char *FACTORY(MPPDecoder)::OutPutDataType()
{
    return DRM_IMAGE_NV12;
}
}
