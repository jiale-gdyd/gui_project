#include <linux/kconfig.h>

#include <time.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#include <memory>

#include <media/utils/utils.h>
#include <media/utils/buffer.h>

#if defined(CONFIG_RKRGA)
#if defined(CONFIG_RKRGA_OLD)
#include <rockchip/rkrga/rga.h>
#include <rockchip/rkrga/im2d.h>
#else
#include <rockchip/rkrgax/rga.h>
#include <rockchip/rkrgax/im2d.h>
#endif
#endif

#include "mpp_encoder.h"
#include "mpp_jpeg_packet.h"
#include "mpp_jpeg_fd_info.h"

namespace libdrm {
MPPEncoder::MPPEncoder()
    : coding_type(MPP_VIDEO_CodingAutoDetect), output_mb_flags(0),
    encoder_sta_en(false), encoded_bps(0), encoded_fps(0), stream_size_1s(0),
    frame_cnt_1s(0), last_ts(0), cur_ts(0), userdata_len(0),
    userdata_frame_id(0), userdata_all_frame_en(0), app2_len(0)
{
    mpp_ctx = NULL;
#if defined(CONFIG_RKMPP_HWOSD)
    memset(&osd_data, 0, sizeof(osd_data));
    memset(&osd_data2, 0, sizeof(osd_data2));
    osd_buf_grp = NULL;
#endif

#if defined(CONFIG_RKMPP_RGA_OSD)
    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        rga_osd_data[i].data.reset();
        rga_osd_data[i].pos_x = 0;
        rga_osd_data[i].pos_y = 0;
        rga_osd_data[i].width = 0;
        rga_osd_data[i].height = 0;
        rga_osd_data[i].enable = 0;
        rga_osd_data[i].inverse = 0;
        rga_osd_data[i].region_type = REGION_TYPE_OVERLAY_EX;
        rga_osd_data[i].cover_color = 0;
    }

    rga_osd_cnt = 0;
#endif

    memset(&ud_set, 0, sizeof(MppEncUserDataSet));
    memset(ud_datas, 0, sizeof(ud_datas) * 2);
    memset(&roi_cfg, 0, sizeof(roi_cfg));
    rc_api_brief_name = "default";
    memset(thumbnail_type, 0, sizeof(thumbnail_type));
    memset(thumbnail_width, 0, sizeof(thumbnail_width));
    memset(thumbnail_height, 0, sizeof(thumbnail_height));
}

MPPEncoder::~MPPEncoder()
{
#if defined(CONFIG_RKMPP_HWOSD)
    if (osd_data.buf) {
        DRM_MEDIA_LOGD("MPP Encoder: free osd buff");
        mpp_buffer_put(osd_data.buf);
        osd_data.buf = NULL;
    }

    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        DRM_MEDIA_LOGD("MPP Encoder: free osd2 buff");
        if (osd_data2.region[i].buf) {
            mpp_buffer_put(osd_data2.region[i].buf);
            osd_data2.region[i].buf = NULL;
        }
    }

    if (osd_buf_grp) {
        DRM_MEDIA_LOGD("MPP Encoder: free osd buff group");
        mpp_buffer_group_put(osd_buf_grp);
        osd_buf_grp = NULL;
    }
#endif

#if defined(CONFIG_RKMPP_RGA_OSD)
    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        rga_osd_data[i].data.reset();
    }

    rga_osd_cnt = 0;
#endif

    if (roi_cfg.regions) {
        DRM_MEDIA_LOGD("MPP Encoder: free enc roi region buff");
        free(roi_cfg.regions);
        roi_cfg.regions = NULL;
    }
}

void MPPEncoder::SetMppCodeingType(MppCodingType type)
{
    coding_type = type;
    if (type == MPP_VIDEO_CodingMJPEG) {
        codec_type = DRM_CODEC_TYPE_JPEG;
    } else if (type == MPP_VIDEO_CodingAVC) {
        codec_type = DRM_CODEC_TYPE_H264;
    } else if (type == MPP_VIDEO_CodingHEVC) {
        codec_type = DRM_CODEC_TYPE_H265;
    }

    if (type == MPP_VIDEO_CodingAVC || type == MPP_VIDEO_CodingHEVC) {
        output_mb_flags |= MediaBuffer::kSingleNalUnit;
    }
}

bool MPPEncoder::Init()
{
    int ret = 0;
    if (coding_type == MPP_VIDEO_CodingUnused) {
        return false;
    }

#if defined(CONFIG_RKMPP_HWOSD)
    ret = mpp_buffer_group_get_internal(&osd_buf_grp, MPP_BUFFER_TYPE_DRM);
    if (ret) {
        DRM_MEDIA_LOGE("MPP Encoder: failed to get mpp buffer group, return:[%d]", ret);
        return false;
    }

    ret = mpp_buffer_group_limit_config(osd_buf_grp, 0, OSD_REGIONS_CNT + 1);
    if (ret) {
        DRM_MEDIA_LOGE("MPP Encoder: failed to limit buffer group, return:[%d]", ret);
        return false;
    }
#endif

    mpp_ctx = std::make_shared<MPPContext>();
    if (!mpp_ctx) {
        return false;
    }

    MppCtx ctx = NULL;
    MppApi *mpi = NULL;

    ret = mpp_create(&ctx, &mpi);
    if (ret) {
        DRM_MEDIA_LOGE("mpp_create failed");
        return false;
    }

    mpp_ctx->ctx = ctx;
    mpp_ctx->mpi = mpi;

    ret = mpp_init(ctx, MPP_CTX_ENC, coding_type);
    if (ret != MPP_OK) {
        DRM_MEDIA_LOGE("mpp_init failed with type:[%d]", coding_type);

        mpp_destroy(ctx);
        ctx = NULL;
        mpi = NULL;
        return false;
    }

    return true;
}

int MPPEncoder::PrepareMppFrame(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &mdinfo, MppFrame &frame)
{
    MppBuffer pic_buf = nullptr;
    if (input->GetType() != DrmType::Image) {
        DRM_MEDIA_LOGE("mpp encoder input source only support image buffer");
        return -EINVAL;
    }

    DrmPixelFormat fmt = input->GetPixelFormat();
    if (fmt == DRM_PIX_FMT_NONE) {
        DRM_MEDIA_LOGE("mpp encoder input source invalid pixel format");
        return -EINVAL;
    }

    ImageBuffer *hw_buffer = static_cast<ImageBuffer *>(input.get());

    assert(input->GetValidSize() > 0);

    mpp_frame_set_pts(frame, hw_buffer->GetUSTimeStamp());
    mpp_frame_set_dts(frame, hw_buffer->GetUSTimeStamp());
    mpp_frame_set_width(frame, hw_buffer->GetWidth());
    mpp_frame_set_height(frame, hw_buffer->GetHeight());
    mpp_frame_set_offset_x(frame, this->GetConfig().img_cfg.rect_info.x);
    mpp_frame_set_offset_y(frame, this->GetConfig().img_cfg.rect_info.y);
    mpp_frame_set_fmt(frame, ConvertToMppPixFmt(fmt));

    if (fmt == DRM_PIX_FMT_YUYV422 || fmt == DRM_PIX_FMT_UYVY422) {
        mpp_frame_set_hor_stride(frame, hw_buffer->GetVirWidth() * 2);
    } else if (fmt == DRM_PIX_FMT_RGB888 || fmt == DRM_PIX_FMT_BGR888) {
        mpp_frame_set_hor_stride(frame, hw_buffer->GetVirWidth() * 3);
    } else {
        mpp_frame_set_hor_stride(frame, hw_buffer->GetVirWidth());
    }

    mpp_frame_set_ver_stride(frame, hw_buffer->GetVirHeight());

    MppMeta meta = mpp_frame_get_meta(frame);
    auto &related_vec = input->GetRelatedSPtrs();
    if (!related_vec.empty()) {
        mdinfo = std::static_pointer_cast<MediaBuffer>(related_vec[0]);
        DRM_MEDIA_LOGD("MPP Encoder: set mdinfo(%p, %zuBytes) to frame", mdinfo->GetPtr(), mdinfo->GetValidSize());
        mpp_meta_set_ptr(meta, KEY_MV_LIST, mdinfo->GetPtr());
    }

    if (roi_cfg.number && roi_cfg.regions) {
        DRM_MEDIA_LOGD("MPP Encoder: set roi cfg(cnt:%d,%p) to frame", roi_cfg.number, roi_cfg.regions);
        mpp_meta_set_ptr(meta, KEY_ROI_DATA, &roi_cfg);
    }

#if defined(CONFIG_RKMPP_HWOSD)
    if (osd_data.num_region && osd_data.buf) {
        DRM_MEDIA_LOGD("MPP Encoder: set osd data(%d regions) to frame", osd_data.num_region);
        mpp_meta_set_ptr(meta, KEY_OSD_DATA, (void *)&osd_data);
    }

    if (osd_data2.num_region) {
        DRM_MEDIA_LOGD("MPP Encoder: set osd data2(%d regions) to frame", osd_data2.num_region);
        mpp_meta_set_ptr(meta, KEY_OSD_DATA2, (void *)&osd_data2);
    }
#endif

    int ud_idx = 0;
    if (userdata_len) {
        DRM_MEDIA_LOGD("MPP Encoder: set userdata(%dBytes) to frame", userdata_len);

        bool skip_frame = false;
        if (!userdata_all_frame_en) {
            DrmMediaConfig &cfg = GetConfig();
            if (userdata_frame_id) {
                skip_frame = true;
            }

            userdata_frame_id++;
            if (userdata_frame_id == cfg.vid_cfg.gop_size) {
                userdata_frame_id = 0;
            }
        }

        if (!skip_frame) {
            ud_datas[ud_idx].pdata = userdata;
            ud_datas[ud_idx].len = userdata_len;
            ud_datas[ud_idx].uuid = NULL;
            ud_idx++;
        }
    }

    if (input->GetDbgInfo() && input->GetDbgInfoSize()) {
        ud_datas[ud_idx].pdata = input->GetDbgInfo();
        ud_datas[ud_idx].len = input->GetDbgInfoSize();
        ud_datas[ud_idx].uuid = NULL;
        ud_idx++;
    }

    if (app2_len) {
        ud_datas[ud_idx].pdata = app2;
        ud_datas[ud_idx].len = app2_len;
        ud_datas[ud_idx].uuid = NULL;
        ud_idx++;
    }

    if (ud_idx) {
        ud_set.count = ud_idx;
        ud_set.datas = ud_datas;
        DRM_MEDIA_LOGD("MPP Encoder: userdata: ud_set.count:[%d], ud_set.datas:[%p]...", ud_set.count, ud_set.datas);
        mpp_meta_set_ptr(meta, KEY_USER_DATAS, &ud_set);
    }

    MPP_RET ret = init_mpp_buffer_with_content(pic_buf, input);
    if (ret) {
        DRM_MEDIA_LOGE("prepare picture buffer failed");
        return ret;
    }

    mpp_frame_set_buffer(frame, pic_buf);
    if (input->IsEOF()) {
        mpp_frame_set_eos(frame, 1);
    }
    mpp_buffer_put(pic_buf);

    return 0;
}

int MPPEncoder::PrepareMppPacket(std::shared_ptr<MediaBuffer> &output, MppPacket &packet)
{
    MppBuffer mpp_buf = nullptr;
    if (!output->IsHwBuffer()) {
        return 0;
    }

    MPP_RET ret = init_mpp_buffer(mpp_buf, output, 0);
    if (ret) {
        DRM_MEDIA_LOGI("import output stream buffer failed");
        return ret;
    }

    if (mpp_buf) {
        mpp_packet_init_with_buffer(&packet, mpp_buf);
        mpp_buffer_put(mpp_buf);
    }

    return 0;
}

int MPPEncoder::PrepareThumbnail(const std::shared_ptr<MediaBuffer> &input, int thumbnail_num, char *buf, int *len)
{
    int ret = 0;
    MppEncCfg enc_cfg;
    int packet_len = 0;
    MppFrame frame = nullptr;
    MppPacket packet = nullptr;
    MppBuffer mv_buf = nullptr;
    MppBuffer frame_buf = nullptr;

    assert(thumbnail_num <= THUMBNAIL_MAX_CNT);
    assert(thumbnail_type[thumbnail_num] != 0);

    ImageBuffer *hw_buffer = static_cast<ImageBuffer *>(input.get());
    if (thumbnail_type[thumbnail_num] == THUMBNAIL_TYPE_APP2) {
        assert(thumbnail_width[thumbnail_num] < this->GetConfig().img_cfg.rect_info.w);
        assert(thumbnail_width[thumbnail_num] > 160);
        assert(thumbnail_height[thumbnail_num] < this->GetConfig().img_cfg.rect_info.h);
        assert(thumbnail_height[thumbnail_num] < 120);
    }

    if (thumbnail_type[thumbnail_num] == THUMBNAIL_TYPE_APP1) {
        assert(thumbnail_width[thumbnail_num] == 160);
        assert(thumbnail_height[thumbnail_num] == 120);
    }

    ret = mpp_frame_init(&frame);
    if (MPP_OK != ret) {
        DRM_MEDIA_LOGI("mpp_frame_init failed, return:[%d]", ret);
        return ret;
    }

    extern int get_rga_format(DrmPixelFormat f);

    DrmPixelFormat src_pixfmt = input->GetPixelFormat();
    DrmPixelFormat trg_pixfmt = DRM_PIX_FMT_NV12;
    uint32_t u32PicSize = CalPixFmtSize(trg_pixfmt, thumbnail_width[thumbnail_num], thumbnail_height[thumbnail_num], 8);
    void *pvPicPtr = malloc(u32PicSize);
    if (pvPicPtr == NULL) {
        ret = -1;
        goto THUMB_END;
    }

    rga_buffer_t src_info, dst_info;
    memset(&src_info, 0, sizeof(rga_buffer_t));
    memset(&dst_info, 0, sizeof(rga_buffer_t));

    src_info = wrapbuffer_virtualaddr(input->GetPtr(), hw_buffer->GetWidth(), hw_buffer->GetHeight(), get_rga_format(src_pixfmt), hw_buffer->GetVirWidth(), hw_buffer->GetVirHeight());
    dst_info = wrapbuffer_virtualaddr(pvPicPtr, thumbnail_width[thumbnail_num], thumbnail_height[thumbnail_num], get_rga_format(trg_pixfmt), DRM_UPALIGNTO(thumbnail_width[thumbnail_num], 8), DRM_UPALIGNTO(thumbnail_height[thumbnail_num], 8));

    ret = imresize(src_info, dst_info);
    if (ret <= 0) {
        DRM_MEDIA_LOGE("imcrop failed, return:[%d]", ret);
        DRM_MEDIA_LOGE("src_info:(%d, %d, %d, %d)", src_info.width, src_info.height, src_info.wstride, src_info.hstride);
        DRM_MEDIA_LOGE("dst_info:(%d, %d, %d, %d)", dst_info.width, dst_info.height, dst_info.wstride, dst_info.hstride);
        ret = -1;
        goto THUMB_END;
    }

    ret = mpp_enc_cfg_init(&enc_cfg);
    if (MPP_OK != ret) {
        DRM_MEDIA_LOGE("MPP Encoder: MPPConfig: cfg init failed, return:[%d]", ret);
        goto THUMB_END;
    }

    ret = mpp_enc_cfg_set_s32(enc_cfg, "prep:width", thumbnail_width[thumbnail_num]);
    ret |= mpp_enc_cfg_set_s32(enc_cfg, "prep:height", thumbnail_height[thumbnail_num]);
    ret |= mpp_enc_cfg_set_s32(enc_cfg, "prep:hor_stride", DRM_UPALIGNTO(thumbnail_width[thumbnail_num], 8));
    ret |= mpp_enc_cfg_set_s32(enc_cfg, "prep:ver_stride", DRM_UPALIGNTO(thumbnail_height[thumbnail_num], 8));
    ret = EncodeControl(MPP_ENC_SET_CFG, enc_cfg);
    if (ret) {
        DRM_MEDIA_LOGE("MPP Encoder[JPEG]: set resolution cfg failed, return:[%d]", ret);
        goto THUMB_END;
    }

    mpp_frame_set_pts(frame, hw_buffer->GetUSTimeStamp());
    mpp_frame_set_dts(frame, hw_buffer->GetUSTimeStamp());
    mpp_frame_set_width(frame, thumbnail_width[thumbnail_num]);
    mpp_frame_set_height(frame, thumbnail_height[thumbnail_num]);
    mpp_frame_set_offset_x(frame, 0);
    mpp_frame_set_offset_y(frame, 0);
    mpp_frame_set_fmt(frame, ConvertToMppPixFmt(DRM_PIX_FMT_NV12));
    mpp_frame_set_hor_stride(frame, DRM_UPALIGNTO(thumbnail_width[thumbnail_num], 8));
    mpp_frame_set_ver_stride(frame, DRM_UPALIGNTO(thumbnail_height[thumbnail_num], 8));

    ret = init_mpp_buffer(frame_buf, NULL, u32PicSize);
    if (ret) {
        DRM_MEDIA_LOGE("init frame buffer failed, return:[%d]", ret);
        goto THUMB_END;
    }

    if (frame_buf) {
        memcpy(mpp_buffer_get_ptr(frame_buf), pvPicPtr, u32PicSize);
        mpp_frame_set_buffer(frame, frame_buf);
        mpp_buffer_put(frame_buf);
    }

    mpp_frame_set_eos(frame, 1);
    ret = Process(frame, packet, mv_buf);
    if (ret) {
        DRM_MEDIA_LOGE("Process failed, return:[%d]", ret);
        goto THUMB_END;
    }

    packet_len = mpp_packet_get_length(packet);
    if (packet_len < *len) {
        memcpy(buf, mpp_packet_get_data(packet), packet_len);
        *len = packet_len;
    }

    ret = mpp_enc_cfg_set_s32(enc_cfg, "prep:width", this->GetConfig().img_cfg.rect_info.w);
    ret |= mpp_enc_cfg_set_s32(enc_cfg, "prep:height", this->GetConfig().img_cfg.rect_info.h);
    ret |= mpp_enc_cfg_set_s32(enc_cfg, "prep:hor_stride", this->GetConfig().img_cfg.image_info.vir_width);
    ret |= mpp_enc_cfg_set_s32(enc_cfg, "prep:ver_stride", this->GetConfig().img_cfg.image_info.vir_height);

    ret = EncodeControl(MPP_ENC_SET_CFG, enc_cfg);
    if (ret) {
        DRM_MEDIA_LOGE("MPP Encoder[JPEG]: set resolution cfg failed, return:[%d]", ret);
        goto THUMB_END;
    }

    if (enc_cfg) {
        mpp_enc_cfg_deinit(enc_cfg);
        DRM_MEDIA_LOGI("MPP Encoder: MPPConfig: cfg deinit done");
    }

THUMB_END:
    if (pvPicPtr) {
        free(pvPicPtr);
    }

    if (frame) {
        mpp_frame_deinit(&frame);
    }

    if (packet) {
        mpp_packet_deinit(&packet);
    }

    return ret;
}

int MPPEncoder::PrepareMppExtraBuffer(std::shared_ptr<MediaBuffer> extra_output, MppBuffer &buffer)
{
    MppBuffer mpp_buf = nullptr;

    if (!extra_output || !extra_output->IsValid()) {
        return 0;
    }

    MPP_RET ret = init_mpp_buffer(mpp_buf, extra_output, extra_output->GetValidSize());
    if (ret) {
        DRM_MEDIA_LOGE("import extra stream buffer failed");
        return ret;
    }

    buffer = mpp_buf;
    return 0;
}

class MPPPacketContext {
public:
    MPPPacketContext(std::shared_ptr<MPPContext> ctx, MppPacket p) : mctx(ctx), packet(p) {

    }

    ~MPPPacketContext() {
        if (packet) {
            mpp_packet_deinit(&packet);
        }
    }

private:
    std::shared_ptr<MPPContext> mctx;
    MppPacket                   packet;
};

static int __free_mpppacketcontext(void *p)
{
    assert(p);

    delete (MPPPacketContext *)p;
    return 0;
}

int MPPEncoder::Process(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &output, std::shared_ptr<MediaBuffer> extra_output)
{
    int64_t pts = 0;
    DrmType out_type;
    int image_cnt = 0;
    int temporal_id = -1;
    uint32_t out_eof = 0;
    size_t packet_len = 0;
    size_t packet_size = 0;
    uint32_t packet_flag = 0;
    MppFrame frame = nullptr;
    MppPacket packet = nullptr;
    MppBuffer mv_buf = nullptr;
    int mpf_len[3] = {0, 0, 0};
    MppPacket import_packet = nullptr;
    char *mpf[3] = {NULL, NULL, NULL};
    std::shared_ptr<MediaBuffer> mdinfo;

    if (!input) {
        return 0;
    }

    if (!output) {
        return -EINVAL;
    }

    while (HasChangeReq()) {
        auto change = PeekChange();
        if (change.first && !CheckConfigChange(change)) {
            return -1;
        }
    }

    DrmMediaConfig &cfg_check = GetConfig();
    ImageBuffer *hw_buffer = static_cast<ImageBuffer *>(input.get());

    if (cfg_check.img_cfg.image_info.width != hw_buffer->GetWidth()
        || cfg_check.img_cfg.image_info.height != hw_buffer->GetHeight()
        || cfg_check.img_cfg.image_info.vir_width != hw_buffer->GetVirWidth()
        || cfg_check.img_cfg.image_info.vir_height != hw_buffer->GetVirHeight())
    {
        DRM_MEDIA_LOGE("The resolution of the input buffer is wrong. check:[%d, %d, %d, %d], buffer:[%d, %d, %d, %d]",
            cfg_check.img_cfg.image_info.width,
            cfg_check.img_cfg.image_info.height,
            cfg_check.img_cfg.image_info.vir_width,
            cfg_check.img_cfg.image_info.vir_height, hw_buffer->GetWidth(),
            hw_buffer->GetHeight(), hw_buffer->GetVirWidth(),
            hw_buffer->GetVirHeight());
        return 0;
    }

    int ret = mpp_frame_init(&frame);
    if (MPP_OK != ret) {
        DRM_MEDIA_LOGE("mpp_frame_init failed");
        goto ENCODE_OUT;
    }

    if (thumbnail_type[0] == THUMBNAIL_TYPE_APP1) {
        int len = 32 * 1024;
        char *buf = (char *)malloc(len);
        if (buf) {
            if (!PrepareThumbnail(input, 0, buf, &len)) {
                memset(userdata, 0, MPP_ENCODER_USERDATA_MAX_SIZE);
                userdata_len = PackageApp1(stIfd0, stIfd1, sizeof(stIfd0) / sizeof(IFD_S), sizeof(stIfd1) / sizeof(IFD_S), userdata, buf, len);
                userdata_all_frame_en = 1;

                DRM_MEDIA_LOGD("app1 userdata_len:[%d], len:[%d]", userdata_len, len);
            }

            free(buf);
        }
    }

    if (thumbnail_type[1] == THUMBNAIL_TYPE_APP2) {
        mpf_len[1] = thumbnail_width[1] * thumbnail_height[1] * 2;
        mpf[1] = (char *)malloc(mpf_len[1]);
        if (mpf[1] && (!PrepareThumbnail(input, 1, mpf[1], &mpf_len[1]))) {
            image_cnt++;
        }
    }

#if defined(CONFIG_RKMPP_RGA_OSD)
    if (rga_osd_cnt > 0) {
        RgaOsdRegionProcess(hw_buffer);
    }
#endif

    if ((thumbnail_type[2] == THUMBNAIL_TYPE_APP2) && (image_cnt == 1)) {
        mpf_len[2] = thumbnail_width[2] * thumbnail_height[2] * 2;
        mpf[2] = (char *)malloc(mpf_len[2]);
        if (mpf[2] && (!PrepareThumbnail(input, 2, mpf[2], &mpf_len[2]))) {
            image_cnt++;
        }
    }

    if (image_cnt) {
        image_cnt++;
        app2_len = PackageApp2(stMpfd, sizeof(stMpfd) / sizeof(IFD_S), image_cnt, app2, mpf_len);
    } else {
        app2_len = 0;
    }

    ret = PrepareMppFrame(input, mdinfo, frame);
    if (ret) {
        DRM_MEDIA_LOGE("PrepareMppFrame failed");
        goto ENCODE_OUT;
    }

    if (output->IsValid()) {
        ret = PrepareMppPacket(output, packet);
        if (ret) {
            DRM_MEDIA_LOGE("PrepareMppPacket failed");
            goto ENCODE_OUT;
        }

        import_packet = packet;
    }

    ret = PrepareMppExtraBuffer(extra_output, mv_buf);
    if (ret) {
        DRM_MEDIA_LOGE("PrepareMppExtraBuffer failed");
        goto ENCODE_OUT;
    }

    ret = Process(frame, packet, mv_buf);
    if (ret) {
        goto ENCODE_OUT;
    }

    if (!packet) {
        DRM_MEDIA_LOGE("MPP Encoder: input frame:[%p], [%zu]Bytes; output null packet", frame, mpp_buffer_get_size(mpp_frame_get_buffer(frame)));
        goto ENCODE_OUT;
    }

    packet_len = mpp_packet_get_length(packet);
    packet_size = mpp_packet_get_size(packet);

    if (image_cnt) {
        char *ptr = (char *)mpp_packet_get_data(packet);
        if (image_cnt > 1) {
            if (mpf_len[1] > (int)(packet_size - packet_len)) {
                DRM_MEDIA_LOGE("MPP Encoder: mpf[1] = %d > %d - %d", mpf_len[1], packet_size, packet_len);
                mpf_len[1] = packet_size - packet_len;
            }

            if (ptr) {
                memcpy(ptr + packet_len, mpf[1], mpf_len[1]);
            }
        }

        if (image_cnt > 2) {
            if (mpf_len[1] > (int)(packet_size - packet_len - mpf_len[1])) {
                DRM_MEDIA_LOGE("MPP Encoder: mpf[2] = %d > %d - %d - %d", mpf_len[1], packet_size, packet_len, mpf_len[1]);
                mpf_len[2] = packet_size - packet_len - mpf_len[1];
            }

            if (ptr) {
                memcpy(ptr + packet_len + mpf_len[1], mpf[2], mpf_len[2]);
            }
        }

        mpf_len[0] = packet_len - (0x14 + userdata_len);
        if (ptr) {
            PackageApp2(stMpfd, 5, image_cnt, ptr + 0x14 + userdata_len, mpf_len);
        }
        packet_len += mpf_len[1];
        packet_len += mpf_len[2];
    }

    {
        int is_intra = 0;
        MppMeta packet_meta = mpp_packet_get_meta(packet);

        mpp_meta_get_s32(packet_meta, KEY_OUTPUT_INTRA, &is_intra);
        packet_flag = (is_intra) ? MediaBuffer::kIntra : MediaBuffer::kPredicted;
        mpp_meta_get_s32(packet_meta, KEY_TEMPORAL_ID, &temporal_id);
    }

    out_eof = mpp_packet_get_eos(packet);
    pts = mpp_packet_get_pts(packet);
    if (pts <= 0) {
        pts = mpp_packet_get_dts(packet);
    }

    if (packet_len == 0) {
        output->SetValidSize(0);
        if (extra_output) {
            extra_output->SetValidSize(0);
        }

        goto ENCODE_OUT;
    }

    if (encoder_sta_en) {
        int enable_bps = 1;
        DrmMediaConfig &cfg = GetConfig();
        int target_fps = cfg.vid_cfg.frame_rate;
        int target_bpsmax = cfg.vid_cfg.bit_rate_max;

        frame_cnt_1s += 1;
        stream_size_1s += packet_len;
        if (target_fps <= 0) {
            target_fps = 30;
            enable_bps = 0;
        }

        if ((frame_cnt_1s % target_fps) == 0) {
            cur_ts = gettimeofday();
            if (last_ts) {
                encoded_fps = ((float)target_fps / (cur_ts - last_ts)) * 1000000;
            } else {
                encoded_fps = 0;
            }

            last_ts = cur_ts;
            if (enable_bps) {
                encoded_bps = stream_size_1s * 8;
                DRM_MEDIA_LOGI("MPP ENCODER: bps:[%d], actual_bps:[%d], fps:[%d], actual_fps:[%f]", target_bpsmax, encoded_bps, target_fps, encoded_fps);
            } else {
                DRM_MEDIA_LOGI("MPP ENCODER: fps statistical period:[%d], actual_fps:[%f]", target_fps, encoded_fps);
            }

            stream_size_1s = 0;
            frame_cnt_1s = 0;
        }
    } else if (cur_ts) {
        stream_size_1s = 0;
        frame_cnt_1s = 0;
        cur_ts = 0;
        last_ts = 0;
    }

    if (output->IsValid()) {
        if (!import_packet) {
            void *ptr = output->GetPtr();
            assert(ptr);

            DRM_MEDIA_LOGD("extra time-consuming memcpy to cpu");
            memcpy(ptr, mpp_packet_get_data(packet), packet_len);
        }
    } else {
        MPPPacketContext *ctx = new MPPPacketContext(mpp_ctx, packet);
        if (!ctx) {
            DRM_LOG_MEMORY();
            ret = -ENOMEM;
            goto ENCODE_OUT;
        }

        output->SetFD(mpp_buffer_get_fd(mpp_packet_get_buffer(packet)));
        output->SetPtr(mpp_packet_get_data(packet));
        output->SetSize(mpp_packet_get_size(packet));
        output->SetUserData(ctx, __free_mpppacketcontext);
        packet = nullptr;
    }

    output->SetValidSize(packet_len);
    output->SetUserFlag(packet_flag | output_mb_flags);
    output->SetTsvcLevel(temporal_id);
    output->SetUSTimeStamp(input->GetUSTimeStamp());
    output->SetEOF(out_eof ? true : false);

    out_type = output->GetType();
    if (out_type == DrmType::Image) {
        auto out_img = std::static_pointer_cast<ImageBuffer>(output);
        auto &info = out_img->GetImageInfo();
        const auto &in_cfg = GetConfig();
        info = (coding_type == MPP_VIDEO_CodingMJPEG) ? in_cfg.img_cfg.image_info : in_cfg.vid_cfg.image_cfg.image_info;
    } else {
        output->SetType(DrmType::Video);
    }

    if (mv_buf) {
        if (extra_output->GetFD() < 0) {
            void *ptr = extra_output->GetPtr();
            assert(ptr);
            memcpy(ptr, mpp_buffer_get_ptr(mv_buf), mpp_buffer_get_size(mv_buf));
        }

        extra_output->SetValidSize(mpp_buffer_get_size(mv_buf));
        extra_output->SetUserFlag(packet_flag);
        extra_output->SetUSTimeStamp(input->GetUSTimeStamp());
    }

ENCODE_OUT:
    if (frame) {
        mpp_frame_deinit(&frame);
    }

    if (packet) {
        mpp_packet_deinit(&packet);
    }

    if (mv_buf) {
        mpp_buffer_put(mv_buf);
    }

    if (mpf[1]) {
        free(mpf[1]);
    }

    if (mpf[2]) {
        free(mpf[2]);
    }

    return ret;
}

int MPPEncoder::Process(MppFrame frame, MppPacket &packet, MppBuffer &mv_buf)
{
    MppCtx ctx = mpp_ctx->ctx;
    MppApi *mpi = mpp_ctx->mpi;

    if (mv_buf) {
        DRM_MEDIA_LOGI("TODO move detection frome mpp encoder...");
    }

    int ret = mpi->encode_put_frame(ctx, frame);
    if (ret) {
        DRM_MEDIA_LOGE("mpp encode put frame failed");
        return -1;
    }

    ret = mpi->encode_get_packet(ctx, &packet);
    if (ret) {
        DRM_MEDIA_LOGE("mpp encode get packet failed");
        return -1;
    }

    return 0;
}

int MPPEncoder::SendInput(const std::shared_ptr<MediaBuffer> &)
{
    errno = ENOSYS;
    return -1;
}

std::shared_ptr<MediaBuffer> MPPEncoder::FetchOutput()
{
    errno = ENOSYS;
    return nullptr;
}

int MPPEncoder::EncodeControl(int cmd, void *param)
{
    MpiCmd mpi_cmd = (MpiCmd)cmd;
    int ret = mpp_ctx->mpi->control(mpp_ctx->ctx, mpi_cmd, (MppParam)param);

    if (ret) {
        DRM_MEDIA_LOGE("mpp control cmd:[0x%08X] param:[%p] failed", cmd, param);
        return ret;
    }

    return 0;
}

void MPPEncoder::QueryChange(uint32_t change, void *value, int32_t size)
{
    if (!value || !size) {
        DRM_MEDIA_LOGE("MPP ENCODER: invalid argument");
        return;
    }

    switch (change) {
        case VideoEncoder::kMoveDetectionFlow:
            if (size < (int)sizeof(int32_t)) {
                DRM_MEDIA_LOGE("MPP ENCODER: change:[%d], size invalid", VideoEncoder::kMoveDetectionFlow);
                return;
            }
    
            if (rc_api_brief_name == "smart") {
                *((int32_t *)value) = 1;
            } else {
                *((int32_t *)value) = 0;
            }
            break;

        default:
            DRM_MEDIA_LOGW("MPP ENCODER: change:[%d] not support", change);
    }
}

void MPPEncoder::set_statistics_switch(bool value)
{
    DRM_MEDIA_LOGI("MPP ENCODER %s statistics", value ? "enable" : "disable");
    encoder_sta_en = value;
}

int MPPEncoder::get_statistics_bps()
{
    if (!encoder_sta_en) {
        DRM_MEDIA_LOGW("MPP ENCODER statistics should enable first");
    }

    return encoded_bps;
}

int MPPEncoder::get_statistics_fps()
{
    if (!encoder_sta_en) {
        DRM_MEDIA_LOGW("MPP ENCODER statistics should enable first");
    }

    return encoded_fps;
}

#if defined(CONFIG_RKMPP_HWOSD)

#define OSD_PTL_SIZE        1024

static void OsdDummpRegions(DrmOsdRegionData *rdata)
{
    if (!rdata) {
        return;
    }

    DRM_MEDIA_LOGD("#RegionData:[%p]:", rdata->buffer);
    DRM_MEDIA_LOGI("\t enable:[%u]", rdata->enable);
    DRM_MEDIA_LOGI("\t region_id:[%u]", rdata->region_id);
    DRM_MEDIA_LOGI("\t inverse:[%u]", rdata->inverse);
    DRM_MEDIA_LOGI("\t pos_x:[%u]", rdata->pos_x);
    DRM_MEDIA_LOGI("\t pos_y:[%u]", rdata->pos_y);
    DRM_MEDIA_LOGI("\t width:[%u]", rdata->width);
    DRM_MEDIA_LOGI("\t height:[%u]", rdata->height);
    DRM_MEDIA_LOGI("\t region_type:[%u]", rdata->region_type);
    DRM_MEDIA_LOGI("\t cover_color:[%u]", rdata->cover_color);
}

static void OsdDummpMppOsd(MppEncOSDData *osd)
{
    DRM_MEDIA_LOGD("MPP OsdData: cnt:[%d], buff:[%p], bufSize:[%zu]", osd->num_region, osd->buf, mpp_buffer_get_size(osd->buf));

    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        DRM_MEDIA_LOGD("MPP OsdData:[%d]:", i);
        DRM_MEDIA_LOGI("\t enable:[%u]", osd->region[i].enable);
        DRM_MEDIA_LOGI("\t inverse:[%u]", osd->region[i].inverse);
        DRM_MEDIA_LOGI("\t pos_x:[%u]", osd->region[i].start_mb_x * 16);
        DRM_MEDIA_LOGI("\t pos_y:[%u]", osd->region[i].start_mb_y * 16);
        DRM_MEDIA_LOGI("\t width:[%u]", osd->region[i].num_mb_x * 16);
        DRM_MEDIA_LOGI("\t height:[%u]", osd->region[i].num_mb_y * 16);
        DRM_MEDIA_LOGI("\t buf_offset:[%u]", osd->region[i].buf_offset);
    }
}

static void SaveOsdImg(MppEncOSDData *_data, int index)
{
    if (!_data->buf) {
        return;
    }

    char _path[64] = {0};
    sprintf(_path, "/tmp/osd_img%d", index);
    DRM_MEDIA_LOGD("MPP Encoder: save osd img to:[%s]", _path);

    int fd = open(_path, O_WRONLY | O_CREAT);
    if (fd <= 0) {
        return;
    }

    int size = _data->region[index].num_mb_x * 16;
    size *= _data->region[index].num_mb_y * 16;
    uint8_t *ptr = (uint8_t *)mpp_buffer_get_ptr(_data->buf);

    ptr += _data->region[index].buf_offset;
    if (ptr && size) {
        write(fd, ptr, size);
    }

    close(fd);
}

int MPPEncoder::OsdPaletteSet(uint32_t *ptl_data)
{
    if (!ptl_data) {
        return -1;
    }

    DRM_MEDIA_LOGD("MPP Encoder: setting yuva palette...");

    MppCtx ctx = mpp_ctx->ctx;
    MppApi *mpi = mpp_ctx->mpi;
    MppEncOSDPltCfg osd_plt_cfg;
    MppEncOSDPlt osd_plt;

    for (int k = 0; k < 256; k++) {
        osd_plt.data[k].val = *(ptl_data + k);
    }

    osd_plt_cfg.change = MPP_ENC_OSD_PLT_CFG_CHANGE_ALL;
    osd_plt_cfg.type = MPP_ENC_OSD_PLT_TYPE_USERDEF;
    osd_plt_cfg.plt = &osd_plt;

    int ret = mpi->control(ctx, MPP_ENC_SET_OSD_PLT_CFG, &osd_plt_cfg);
    if (ret) {
        DRM_MEDIA_LOGE("MPP Encoder: set osd plt failed, return:[%d]", ret);
    }

    return ret;
}

static int OsdUpdateRegionInfo2(MppEncOSDData2 *osd, DrmOsdRegionData *region_data, MppBufferGroup buf_grp)
{
    uint32_t new_size = 0;
    uint32_t old_size = 0;
    uint8_t *region_src = NULL;
    uint8_t *region_dst = NULL;
    uint8_t rid = region_data->region_id;

    if (!region_data->enable) {
        osd->region[rid].enable = 0;
        osd->num_region = 0;
            for (int i = 0; i < OSD_REGIONS_CNT; i++) {
            if (osd->region[i].enable) {
                osd->num_region = i + 1;
            }
        }

        assert(osd->num_region <= 8);
        return 0;
    }

    new_size = region_data->width * region_data->height;
    if (osd->region[rid].enable) {
        old_size = osd->region[rid].num_mb_x * osd->region[rid].num_mb_y * 256;
    }

    osd->region[rid].enable = 1;
    osd->region[rid].inverse = region_data->inverse;
    osd->region[rid].start_mb_x = region_data->pos_x / 16;
    osd->region[rid].start_mb_y = region_data->pos_y / 16;
    osd->region[rid].num_mb_x = region_data->width / 16;
    osd->region[rid].num_mb_y = region_data->height / 16;
    osd->num_region = 0;

    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        if (osd->region[i].enable) {
            osd->num_region = i + 1;
        }
    }

    assert(osd->num_region <= 8);
    assert(osd->region[rid].start_mb_x <= 256);
    assert(osd->region[rid].start_mb_y <= 256);
    assert(osd->region[rid].num_mb_x <= 256);
    assert(osd->region[rid].num_mb_y <= 256);

    if (old_size >= new_size) {
        DRM_MEDIA_LOGD("MPP Encoder: Region:[%d] reuse old buff:[%u], new_size:[%u]", rid, old_size, new_size);

        region_src = region_data->buffer;
        region_dst = (uint8_t *)mpp_buffer_get_ptr(osd->region[rid].buf);
        memcpy(region_dst, region_src, new_size);
    } else {
        MppBuffer new_buff = NULL;
        MppBuffer old_buff = NULL;
        old_buff = osd->region[rid].buf;

        int ret = mpp_buffer_get(buf_grp, &new_buff, new_size);
        if (ret) {
            DRM_MEDIA_LOGE("MPP Encoder: get osd [%d]Bytes buffer failed, return:[%d]", new_size, ret);

            osd->region[rid].enable = 0;
            osd->region[rid].start_mb_x = 0;
            osd->region[rid].start_mb_y = 0;
            osd->region[rid].buf_offset = 0;
            return -1;
        }

        region_src = region_data->buffer;
        region_dst = (uint8_t *)mpp_buffer_get_ptr(new_buff);
        memcpy(region_dst, region_src, new_size);
        osd->region[rid].buf = new_buff;

        if (old_buff) {
            mpp_buffer_put(old_buff);
            old_buff = NULL;
        }
    }

    return 0;
}

static int OsdUpdateRegionInfo(MppEncOSDData *osd, DrmOsdRegionData *region_data, MppBufferGroup buf_grp)
{
    uint32_t new_size = 0;
    uint32_t old_size = 0;
    uint8_t *region_src = NULL;
    uint8_t *region_dst = NULL;
    uint8_t rid = region_data->region_id;

    if (!region_data->enable) {
        osd->region[rid].enable = 0;
        osd->num_region = 0;
        for (int i = 0; i < OSD_REGIONS_CNT; i++) {
            if (osd->region[i].enable) {
                osd->num_region = i + 1;
            }
        }

        assert(osd->num_region <= 8);
        return 0;
    }

    new_size = region_data->width * region_data->height;
    if (osd->region[rid].enable) {
        old_size = osd->region[rid].num_mb_x * osd->region[rid].num_mb_y * 256;
    }

    osd->region[rid].enable = 1;
    osd->region[rid].inverse = region_data->inverse;
    osd->region[rid].start_mb_x = region_data->pos_x / 16;
    osd->region[rid].start_mb_y = region_data->pos_y / 16;
    osd->region[rid].num_mb_x = region_data->width / 16;
    osd->region[rid].num_mb_y = region_data->height / 16;

    assert(osd->region[rid].start_mb_x <= 256);
    assert(osd->region[rid].start_mb_y <= 256);
    assert(osd->region[rid].num_mb_x <= 256);
    assert(osd->region[rid].num_mb_y <= 256);

    if (old_size >= new_size) {
        DRM_MEDIA_LOGD("MPP Encoder: Region:[%d] reuse old buff:[%u], new_size:[%u]", rid, old_size, new_size);

        region_src = region_data->buffer;
        region_dst = (uint8_t *)mpp_buffer_get_ptr(osd->buf);
        region_dst += osd->region[rid].buf_offset;
        memcpy(region_dst, region_src, new_size);

        // SaveOsdImg(osd, rid);

        return 0;
    }

    uint32_t total_size = 0;
    uint32_t current_size = 0;
    MppBuffer new_buff = NULL;
    MppBuffer old_buff = NULL;
    uint32_t old_offset[OSD_REGIONS_CNT] = {0};

    osd->num_region = 0;
    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        if (osd->region[i].enable) {
            old_offset[i] = osd->region[i].buf_offset;
            osd->region[i].buf_offset = total_size;
            total_size += osd->region[i].num_mb_x * osd->region[i].num_mb_y * 256;
            osd->num_region = i + 1;
        } else {
            osd->region[i].start_mb_x = 0;
            osd->region[i].start_mb_y = 0;
            osd->region[i].buf_offset = 0;
            osd->region[i].num_mb_x = 0;
            osd->region[i].num_mb_y = 0;
        }
    }

    old_buff = osd->buf;

    int ret = mpp_buffer_get(buf_grp, &new_buff, total_size);
    if (ret) {
        DRM_MEDIA_LOGE("MPP Encoder: get osd [%d]Bytes buffer failed, return:[%d]", total_size, ret);

        osd->region[rid].enable = 0;
        osd->region[rid].start_mb_x = 0;
        osd->region[rid].start_mb_y = 0;
        osd->region[rid].buf_offset = 0;
        return -1;
    }

    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        if (!osd->region[i].enable) {
            continue;
        }

        if (i != rid) {
            region_src = (uint8_t *)mpp_buffer_get_ptr(old_buff);
            region_src += old_offset[i];
            region_dst = (uint8_t *)mpp_buffer_get_ptr(new_buff);
            region_dst += osd->region[i].buf_offset;
            current_size = osd->region[i].num_mb_x * osd->region[i].num_mb_y * 256;
        } else {
            region_src = region_data->buffer;
            region_dst = (uint8_t *)mpp_buffer_get_ptr(new_buff);
            region_dst += osd->region[i].buf_offset;
            current_size = new_size;
        }

        assert(region_src);
        assert(region_dst);
        memcpy(region_dst, region_src, current_size);

        // SaveOsdImg(osd, i);
    }

    osd->buf = new_buff;
    if (old_buff) {
        mpp_buffer_put(old_buff);
    }

    return 0;
}

int MPPEncoder::OsdRegionSet(DrmOsdRegionData *rdata)
{
    if (!rdata) {
        return -EINVAL;
    }

    DRM_MEDIA_LOGD("MPP Encoder: setting osd regions...");

    if ((rdata->region_id >= OSD_REGIONS_CNT)) {
        DRM_MEDIA_LOGE("MPP Encoder: invalid region id:[%d, should be:[0, %d)", rdata->region_id, OSD_REGIONS_CNT);
        return -EINVAL;
    }

    if (rdata->enable && !rdata->buffer) {
        DRM_MEDIA_LOGE("MPP Encoder: invalid region data");
        return -EINVAL;
    }

    if ((rdata->width % 16) || (rdata->height % 16) || (rdata->pos_x % 16) || (rdata->pos_y % 16)) {
        DRM_MEDIA_LOGW("MPP Encoder: osd size must be 16 aligned");

        rdata->width = DRM_UPALIGNTO16(rdata->width);
        rdata->height = DRM_UPALIGNTO16(rdata->height);
        rdata->pos_x = DRM_UPALIGNTO16(rdata->pos_x);
        rdata->pos_y = DRM_UPALIGNTO16(rdata->pos_y);
    }

    // OsdDummpRegions(rdata);

    int ret;
    if (1) {
        ret = OsdUpdateRegionInfo2(&osd_data2, rdata, osd_buf_grp);
    } else {
        ret = OsdUpdateRegionInfo(&osd_data, rdata, osd_buf_grp);
    }

    // OsdDummpMppOsd(&osd_data);

    return ret;
}

int MPPEncoder::OsdRegionGet(DrmOsdRegionData *rdata)
{
    DRM_MEDIA_LOGI("ToDo...%p", rdata);
    return 0;
}
#endif

#if defined(CONFIG_RKMPP_RGA_OSD)
int MPPEncoder::RgaOsdRegionSet(DrmOsdRegionData *rdata, DrmVideoConfig &vfg)
{
    if (!rdata) {
        return -EINVAL;
    }

    DRM_MEDIA_LOGD("MPP Encoder[OSD EX]: setting osd regions...");

    if ((rdata->region_id >= OSD_REGIONS_CNT)) {
        DRM_MEDIA_LOGE("MPP Encoder[OSD EX]: invalid region id:[%d], should be:[0, %d)", rdata->region_id, OSD_REGIONS_CNT);
        return -EINVAL;
    }

    if (rdata->enable && rdata->region_type == REGION_TYPE_OVERLAY_EX && !rdata->buffer) {
        DRM_MEDIA_LOGE("MPP Encoder[OSD EX]: invalid region data");
        return -EINVAL;
    }

    int rid = rdata->region_id;
    if (!rdata->enable) {
        if (rga_osd_data[rid].enable) {
            rga_osd_cnt--;
        }

        if (rga_osd_cnt < 0) {
            DRM_MEDIA_LOGW("MPP Encoder[OSD EX]: osd cnt incorrect");
            rga_osd_cnt = 0;
        }

        rga_osd_data[rid].enable = 0;
        rga_osd_data[rid].data.reset();
        return 0;
    }

    int img_width = vfg.image_cfg.image_info.width;
    int img_height = vfg.image_cfg.image_info.height;

    if (rdata->region_type == REGION_TYPE_OVERLAY_EX) {
        int total_size = rdata->width * rdata->height * 4;
        rga_osd_data[rid].data = MediaBuffer::Alloc(total_size, MediaBuffer::MemType::MEM_HARD_WARE);
        if (!rga_osd_data[rid].data) {
            DRM_MEDIA_LOGW("MPP Encoder[OSD EX]: no space left");
            return -ENOMEM;
        }

        if (vfg.rotation == 0) {
            rga_osd_data[rid].data->BeginCPUAccess(true);
            memcpy(rga_osd_data[rid].data->GetPtr(), rdata->buffer, total_size);
            rga_osd_data[rid].data->EndCPUAccess(true);
            rga_osd_data[rid].data->SetValidSize(total_size);
        } else {
            IM_USAGE usage;
            IM_STATUS STATUS;
            int src_w, src_h;
            int dst_w, dst_h;
            rga_buffer_t src, dst;

            memset(&src, 0, sizeof(src));
            memset(&dst, 0, sizeof(dst));
            src_w = rdata->width;
            src_h = rdata->height;

            switch (vfg.rotation) {
                case 90:
                    dst_w = src_h;
                    dst_h = src_w;
                    usage = IM_HAL_TRANSFORM_ROT_270;
                    break;

                case 180:
                    dst_w = src_w;
                    dst_h = src_h;
                    usage = IM_HAL_TRANSFORM_ROT_180;
                    break;

                case 270:
                    dst_w = src_h;
                    dst_h = src_w;
                    usage = IM_HAL_TRANSFORM_ROT_90;
                    break;

                default:
                    return -EINVAL;
            }

            src = wrapbuffer_virtualaddr(rdata->buffer, src_w, src_h, RK_FORMAT_BGRA_8888);
            dst = wrapbuffer_fd(rga_osd_data[rid].data->GetFD(), dst_w, dst_h, RK_FORMAT_BGRA_8888);
            STATUS = imrotate(src, dst, usage);
            if (STATUS != IM_STATUS_SUCCESS) {
                DRM_MEDIA_LOGE("MPP Encoder[OSD EX]: osd rotate failed");
                return -EBUSY;
            }
        }
    }

    if (!rga_osd_data[rid].enable) {
        rga_osd_cnt++;
    }

    if (rga_osd_cnt > OSD_REGIONS_CNT) {
        DRM_MEDIA_LOGW("MPP Encoder[OSD EX]: osd cnt > OSD_REGIONS_CNT");
        rga_osd_cnt = OSD_REGIONS_CNT;
    }

    switch (vfg.rotation) {
        case 0:
            rga_osd_data[rid].pos_x = rdata->pos_x;
            rga_osd_data[rid].pos_y = rdata->pos_y;
            rga_osd_data[rid].width = rdata->width;
            rga_osd_data[rid].height = rdata->height;
            break;

        case 90:
            rga_osd_data[rid].pos_x = rdata->pos_y;
            rga_osd_data[rid].pos_y = img_height - rdata->width - rdata->pos_x;
            rga_osd_data[rid].width = rdata->height;
            rga_osd_data[rid].height = rdata->width;
            break;

        case 180:
            rga_osd_data[rid].pos_x = img_width - rdata->width - rdata->pos_x;
            rga_osd_data[rid].pos_y = img_height - rdata->height - rdata->pos_y;
            rga_osd_data[rid].width = rdata->width;
            rga_osd_data[rid].height = rdata->height;
            break;

        case 270:
            rga_osd_data[rid].pos_x = img_width - rdata->height - rdata->pos_y;
            rga_osd_data[rid].pos_y = rdata->pos_x;
            rga_osd_data[rid].width = rdata->height;
            rga_osd_data[rid].height = rdata->width;
            break;

        default:
            return -EINVAL;
    }

    rga_osd_data[rid].enable = rdata->enable;
    rga_osd_data[rid].inverse = rdata->inverse;
    rga_osd_data[rid].region_type = rdata->region_type;
    rga_osd_data[rid].cover_color = rdata->cover_color;

    return 0;
}

static int ARGBTOABGR(int color)
{
    uint8_t transBlue = (uint8_t)(color >> 16);

    color &= 0xFF00FFFF;
    color |= (color << 16 & 0x00FF0000);
    color &= 0xFFFFFF00;
    color |= (transBlue & 0x000000FF);

    return color;
}

static int get_rga_format(DrmPixelFormat f)
{
    static std::map<DrmPixelFormat, int> rga_format_map = {
        {DRM_PIX_FMT_YUV420P,  RK_FORMAT_YCbCr_420_P},
        {DRM_PIX_FMT_NV12,     RK_FORMAT_YCbCr_420_SP},
        {DRM_PIX_FMT_NV21,     RK_FORMAT_YCrCb_420_SP},
        {DRM_PIX_FMT_YUV422P,  RK_FORMAT_YCbCr_422_P},
        {DRM_PIX_FMT_NV16,     RK_FORMAT_YCbCr_422_SP},
        {DRM_PIX_FMT_NV61,     RK_FORMAT_YCrCb_422_SP},
        {DRM_PIX_FMT_YUYV422,  RK_FORMAT_YUYV_422},
        {DRM_PIX_FMT_UYVY422,  RK_FORMAT_UYVY_422},
        {DRM_PIX_FMT_RGB565,   RK_FORMAT_RGB_565},
        {DRM_PIX_FMT_BGR565,   -1},
        {DRM_PIX_FMT_RGB888,   RK_FORMAT_BGR_888},
        {DRM_PIX_FMT_BGR888,   RK_FORMAT_RGB_888},
        {DRM_PIX_FMT_ARGB8888, RK_FORMAT_BGRA_8888},
        {DRM_PIX_FMT_ABGR8888, RK_FORMAT_RGBA_8888}
    };

    auto it = rga_format_map.find(f);
    if (it != rga_format_map.end()) {
        return it->second;
    }

    return -1;
}

int MPPEncoder::RgaOsdRegionProcess(ImageBuffer *hw_buffer)
{
    rga_buffer_t pat;
    rga_buffer_t src;
    IM_STATUS STATUS;

    if (!hw_buffer) {
        return 0;
    }

    DrmImageInfo imgInfo = hw_buffer->GetImageInfo();

    src = wrapbuffer_fd_t(hw_buffer->GetFD(), imgInfo.width, imgInfo.height, imgInfo.vir_width, imgInfo.vir_height, get_rga_format(hw_buffer->GetPixelFormat()));
    for (int i = 0; i < OSD_REGIONS_CNT; i++) {
        if (!rga_osd_data[i].enable) {
            continue;
        }

        im_rect src_rect = {
            (int)rga_osd_data[i].pos_x, (int)rga_osd_data[i].pos_y,
            (int)rga_osd_data[i].width, (int)rga_osd_data[i].height
        };

        if (rga_osd_data[i].region_type == REGION_TYPE_OVERLAY_EX) {
            pat = wrapbuffer_fd(rga_osd_data[i].data->GetFD(), rga_osd_data[i].width, rga_osd_data[i].height, RK_FORMAT_BGRA_8888);
            im_rect pat_rect = {0, 0, (int)rga_osd_data[i].width, (int)rga_osd_data[i].height};

            int usgae = IM_ALPHA_BLEND_DST_OVER | IM_ALPHA_BLEND_PRE_MUL;
            src.color_space_mode = IM_YUV_TO_RGB_BT601_FULL | IM_RGB_TO_YUV_BT601_FULL;
            STATUS = improcess(src, src, pat, src_rect, src_rect, pat_rect, usgae);
            if (STATUS != IM_STATUS_SUCCESS) {
                DRM_MEDIA_LOGE("MPP Encoder[OSD EX]: do overlay failed");
                break;
            }
        } else if (rga_osd_data[i].region_type == REGION_TYPE_COVER_EX) {
            STATUS = imfill(src, src_rect, ARGBTOABGR(rga_osd_data[i].cover_color));
            if (STATUS != IM_STATUS_SUCCESS) {
                DRM_MEDIA_LOGE("MPP Encoder[OSD EX]: do cover failed");
                break;
            }
        }
    }

    return 0;
}
#endif

int MPPEncoder::RoiUpdateRegions(DrmEncROIRegion *regions, int region_cnt)
{
    if (!regions || region_cnt == 0) {
        roi_cfg.number = 0;
        if (roi_cfg.regions) {
            free(roi_cfg.regions);
            roi_cfg.regions = NULL;
        }

        DRM_MEDIA_LOGI("MPP Encoder: disable roi function");
        return 0;
    }

    int msize = region_cnt * sizeof(MppEncROIRegion);
    MppEncROIRegion *region = (MppEncROIRegion *)malloc(msize);
    if (!region) {
        DRM_LOG_MEMORY();
        return -ENOMEM;
    }

    for (int i = 0; i < region_cnt; i++) {
        if ((regions[i].x % 16) || (regions[i].y % 16) || (regions[i].w % 16) || (regions[i].h % 16)) {
            DRM_MEDIA_LOGW("MPP Encoder: region parameter should be an integer multiple of 16");
            DRM_MEDIA_LOGW("MPP Encoder: reset region:[%d] frome:<%d,%d,%d,%d> to:<%d,%d,%d,%d>",
                i, regions[i].x, regions[i].y, regions[i].w, regions[i].h,
                DRM_UPALIGNTO16(regions[i].x), DRM_UPALIGNTO16(regions[i].y),
                DRM_UPALIGNTO16(regions[i].w), DRM_UPALIGNTO16(regions[i].h));

            regions[i].x = DRM_UPALIGNTO16(regions[i].x);
            regions[i].y = DRM_UPALIGNTO16(regions[i].y);
            regions[i].w = DRM_UPALIGNTO16(regions[i].w);
            regions[i].h = DRM_UPALIGNTO16(regions[i].h);
        }

        DRM_MEDIA_LOGD("MPP Encoder: roi region:[%d]:<%d,%d,%d,%d>", i, regions[i].x, regions[i].y, regions[i].w, regions[i].h);
        DRM_MEDIA_LOGD("MPP Encoder: roi region:[%d].intra:[%d],", i, regions[i].intra);
        DRM_MEDIA_LOGD("MPP Encoder: roi region:[%d].quality:[%d],", i, regions[i].quality);
        DRM_MEDIA_LOGD("MPP Encoder: roi region:[%d].abs_qp_en:[%d],", i, regions[i].abs_qp_en);
        DRM_MEDIA_LOGD("MPP Encoder: roi region:[%d].qp_area_idx:[%d],", i, regions[i].qp_area_idx);
        DRM_MEDIA_LOGD("MPP Encoder: roi region:[%d].area_map_en:[%d],", i, regions[i].area_map_en);

        assert(regions[i].x < 8192);
        assert(regions[i].y < 8192);
        assert(regions[i].w < 8192);
        assert(regions[i].h < 8192);
        assert(regions[i].x < 8192);
        assert(regions[i].intra <= 1);
        assert(regions[i].abs_qp_en <= 1);
        assert(regions[i].qp_area_idx <= 7);
        assert(regions[i].area_map_en <= 1);

        DRM_VALUE_SCOPE_CHECK(regions[i].quality, -48, 51);

        region[i].x = regions[i].x;
        region[i].y = regions[i].y;
        region[i].w = regions[i].w;
        region[i].h = regions[i].h;
        region[i].intra = regions[i].intra;
        region[i].quality = regions[i].quality;
        region[i].abs_qp_en = regions[i].abs_qp_en;
        region[i].qp_area_idx = regions[i].qp_area_idx;
        region[i].area_map_en = regions[i].area_map_en;
    }

    roi_cfg.number = region_cnt;
    if (roi_cfg.regions) {
        free(roi_cfg.regions);
    }
    roi_cfg.regions = region;

    return 0;
}

int MPPEncoder::SetUserData(const char *data, uint16_t len)
{
    uint32_t valid_size = len;

    if (!data && len) {
        DRM_MEDIA_LOGE("Mpp Encoder: invalid userdata");
        return -1;
    }

    if (valid_size > MPP_ENCODER_USERDATA_MAX_SIZE) {
        valid_size = MPP_ENCODER_USERDATA_MAX_SIZE;
        DRM_MEDIA_LOGW("Mpp Encoder: UserData exceeds maximum length:[%d], Reset to:[%d]", valid_size, valid_size);
    }

    if (valid_size) {
        memcpy(userdata, data, valid_size);
    }
    userdata_len = valid_size;

    return 0;
}

void MPPEncoder::ClearUserData()
{
    userdata_len = 0;
}

void MPPEncoder::RestartUserData()
{
    userdata_frame_id = 0;
}

void MPPEncoder::EnableUserDataAllFrame(bool value)
{
    userdata_all_frame_en = value ? 1 : 0;
}
}
