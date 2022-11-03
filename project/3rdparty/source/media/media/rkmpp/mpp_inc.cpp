#include <assert.h>

#include <media/utils/utils.h>
#include <media/utils/media_type.h>
#include <media/utils/media_config.h>

#include "mpp_inc.h"

namespace libdrm {
MppFrameFormat ConvertToMppPixFmt(const DrmPixelFormat &fmt)
{
    static_assert(DRM_PIX_FMT_YUV420P == 0, "The index should greater than 0\n");

    static MppFrameFormat mpp_fmts[DRM_PIX_FMT_NB] = {
        [DRM_PIX_FMT_YUV420P]  = MPP_FMT_YUV420P,
        [DRM_PIX_FMT_NV12]     = MPP_FMT_YUV420SP,
        [DRM_PIX_FMT_NV21]     = MPP_FMT_YUV420SP_VU,
        [DRM_PIX_FMT_YUV422P]  = MPP_FMT_YUV422P,
        [DRM_PIX_FMT_NV16]     = MPP_FMT_YUV422SP,
        [DRM_PIX_FMT_NV61]     = MPP_FMT_YUV422SP_VU,
        [DRM_PIX_FMT_YUYV422]  = MPP_FMT_YUV422_YUYV,
        [DRM_PIX_FMT_UYVY422]  = MPP_FMT_YUV422_UYVY,
        [DRM_PIX_FMT_YUV444SP] = MPP_FMT_YUV444SP,
        [DRM_PIX_FMT_RGB332]   = (MppFrameFormat)-1,
        [DRM_PIX_FMT_RGB565]   = MPP_FMT_RGB565,
        [DRM_PIX_FMT_BGR565]   = MPP_FMT_BGR565,
        [DRM_PIX_FMT_RGB888]   = MPP_FMT_RGB888,
        [DRM_PIX_FMT_BGR888]   = MPP_FMT_BGR888,
        [DRM_PIX_FMT_ARGB8888] = MPP_FMT_ARGB8888,
        [DRM_PIX_FMT_ABGR8888] = MPP_FMT_ABGR8888,
        [DRM_PIX_FMT_RGBA8888] = MPP_FMT_RGBA8888,
        [DRM_PIX_FMT_BGRA8888] = MPP_FMT_BGRA8888,
        [DRM_PIX_FMT_FBC0]     = (MppFrameFormat)(MPP_FMT_YUV420SP | MPP_FRAME_FBC_AFBC_V1),
        [DRM_PIX_FMT_FBC2]     = (MppFrameFormat)(MPP_FMT_YUV422SP | MPP_FRAME_FBC_AFBC_V1)
    };

    if (fmt >= 0 && fmt < DRM_PIX_FMT_NB) {
        return mpp_fmts[fmt];
    }

    return (MppFrameFormat)-1;
}

DrmPixelFormat ConvertToPixFmt(const MppFrameFormat &mfmt)
{
    switch (mfmt) {
        case MPP_FMT_YUV420P:
            return DRM_PIX_FMT_YUV420P;

        case MPP_FMT_YUV420SP:
            return DRM_PIX_FMT_NV12;

        case MPP_FMT_YUV420SP_VU:
            return DRM_PIX_FMT_NV21;

        case MPP_FMT_YUV422P:
            return DRM_PIX_FMT_YUV422P;

        case MPP_FMT_YUV422SP:
            return DRM_PIX_FMT_NV16;

        case MPP_FMT_YUV422SP_VU:
            return DRM_PIX_FMT_NV61;

        case MPP_FMT_YUV422_YUYV:
            return DRM_PIX_FMT_YUYV422;

        case MPP_FMT_YUV422_UYVY:
            return DRM_PIX_FMT_UYVY422;

        case MPP_FMT_YUV444SP:
            return DRM_PIX_FMT_YUV444SP;

        case MPP_FMT_RGB565:
            return DRM_PIX_FMT_RGB565;

        case MPP_FMT_BGR565:
            return DRM_PIX_FMT_BGR565;

        case MPP_FMT_RGB888:
            return DRM_PIX_FMT_RGB888;

        case MPP_FMT_BGR888:
            return DRM_PIX_FMT_BGR888;

        case MPP_FMT_ARGB8888:
            return DRM_PIX_FMT_ARGB8888;

        case MPP_FMT_ABGR8888:
            return DRM_PIX_FMT_ABGR8888;

        case MPP_FMT_RGBA8888:
            return DRM_PIX_FMT_RGBA8888;

        case MPP_FMT_BGRA8888:
            return DRM_PIX_FMT_BGRA8888;

        default:
            DRM_MEDIA_LOGE("unsupport for mpp pixel fmt:[%d]", mfmt);
            return DRM_PIX_FMT_NONE;
    }
}

class _MPP_SUPPORT_TYPES : public SupportMediaTypes {
public:
    _MPP_SUPPORT_TYPES() {
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUV420P));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV12));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV21));
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUV422P));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV16));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV61));
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUYV422));
        types.append(DRM_TYPENEAR(DRM_IMAGE_UYVY422));
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUV444SP));
        types.append(DRM_TYPENEAR(DRM_IMAGE_RGB565));
        types.append(DRM_TYPENEAR(DRM_IMAGE_BGR565));
        types.append(DRM_TYPENEAR(DRM_IMAGE_RGB888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_BGR888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_ARGB8888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_ABGR8888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_RGBA8888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_BGRA8888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_FBC0));
        types.append(DRM_TYPENEAR(DRM_IMAGE_FBC2));
        types.append(DRM_TYPENEAR(DRM_IMAGE_XRGB8888));
    }
};
static _MPP_SUPPORT_TYPES priv_types;

const char *MppAcceptImageFmts()
{
    return priv_types.types.c_str();
}

MppCodingType GetMPPCodingType(const std::string &data_type)
{
    if (data_type == DRM_VIDEO_H264) {
        return MPP_VIDEO_CodingAVC;
    } else if (data_type == DRM_VIDEO_H265) {
        return MPP_VIDEO_CodingHEVC;
    } else if (data_type == DRM_VIDEO_MJPEG) {
        return MPP_VIDEO_CodingMJPEG;
    } else if (data_type == DRM_IMAGE_JPEG) {
        return MPP_VIDEO_CodingMJPEG;
    }

    DRM_MEDIA_LOGE("mpp decoder TODO for [%s]", data_type.c_str());
    return MPP_VIDEO_CodingUnused;
}

static MppEncRcQuality mpp_rc_qualitys[] = {
    MPP_ENC_RC_QUALITY_WORST,  MPP_ENC_RC_QUALITY_WORSE,
    MPP_ENC_RC_QUALITY_MEDIUM, MPP_ENC_RC_QUALITY_BETTER,
    MPP_ENC_RC_QUALITY_BEST,   MPP_ENC_RC_QUALITY_CQP,
    MPP_ENC_RC_QUALITY_AQ_ONLY
};

MppEncRcQuality GetMPPRCQuality(const char *quality)
{
    static_assert(ARRAY_ELEMS(rc_quality_strings) == ARRAY_ELEMS(mpp_rc_qualitys), "rc quality strings miss match mpp rc quality enum");

    if (!quality) {
        return MPP_ENC_RC_QUALITY_BUTT;
    }

    for (size_t i = 0; i < ARRAY_ELEMS(rc_quality_strings); i++) {
        if (!strcasecmp(quality, rc_quality_strings[i])) {
            return mpp_rc_qualitys[i];
        }
    }

    return MPP_ENC_RC_QUALITY_BUTT;
}

static const MppEncRcMode mpp_rc_modes[] = {
    MPP_ENC_RC_MODE_VBR, MPP_ENC_RC_MODE_CBR, MPP_ENC_RC_MODE_FIXQP,
    MPP_ENC_RC_MODE_AVBR
};

MppEncRcMode GetMPPRCMode(const char *rc_mode)
{
    static_assert(ARRAY_ELEMS(rc_mode_strings) == ARRAY_ELEMS(mpp_rc_modes), "rc mode strings miss match mpp rc mode enum");

    if (!rc_mode) {
        return MPP_ENC_RC_MODE_BUTT;
    }

    for (size_t i = 0; i < ARRAY_ELEMS(rc_mode_strings); i++) {
        if (!strcasecmp(rc_mode, rc_mode_strings[i])) {
            return mpp_rc_modes[i];
        }
    }

    return MPP_ENC_RC_MODE_BUTT;
}

MPPContext::MPPContext() : ctx(NULL), mpi(NULL), frame_group(NULL)
{

}

MPPContext::~MPPContext()
{
    if (mpi) {
        mpi->reset(ctx);
        mpp_destroy(ctx);
        ctx = NULL;

        DRM_MEDIA_LOGI("mpp destroy ctx done");
    }

    if (frame_group) {
        mpp_buffer_group_put(frame_group);
        frame_group = NULL;

        DRM_MEDIA_LOGI("mpp buffer group free done");
    }
}

MPP_RET init_mpp_buffer(MppBuffer &buffer, const std::shared_ptr<MediaBuffer> &mb, size_t frame_size)
{
    int fd;
    void *ptr;
    size_t size;
    MPP_RET ret;

    if (mb == NULL) {
        fd = -1;
        ptr = NULL;
        size = 0;
    } else {
        fd = mb->GetFD();
        ptr = mb->GetPtr();
        size = mb->GetValidSize();
    }

    if (fd >= 0) {
        MppBufferInfo info;

        memset(&info, 0, sizeof(info));
        info.type = MPP_BUFFER_TYPE_ION;
        info.size = size;
        info.fd = fd;
        info.ptr = ptr;

        ret = mpp_buffer_import(&buffer, &info);
        if (ret) {
            DRM_MEDIA_LOGE("import input picture buffer failed");
            goto fail;
        }
    } else {
        if (frame_size == 0) {
            return MPP_OK;
        }

        if (size == 0) {
            size = frame_size;
            assert(frame_size > 0);
        }

        ret = mpp_buffer_get(NULL, &buffer, size);
        if (ret) {
            DRM_MEDIA_LOGE("allocate output stream buffer failed");
            goto fail;
        }
    }

    return MPP_OK;

fail:
    if (buffer) {
        mpp_buffer_put(buffer);
        buffer = nullptr;
    }

    return ret;
}

MPP_RET init_mpp_buffer_with_content(MppBuffer &buffer, const std::shared_ptr<MediaBuffer> &mb)
{
    size_t size = mb->GetValidSize();
    MPP_RET ret = init_mpp_buffer(buffer, mb, size);
    if (ret) {
        return ret;
    }

    int fd = mb->GetFD();
    void *ptr = mb->GetPtr();

    if (fd < 0 && ptr) {
        memcpy(mpp_buffer_get_ptr(buffer), ptr, size);
    }

    return MPP_OK;
}

MPP_RET mpi_enc_gen_ref_cfg(MppEncRefCfg ref, RK_S32 gop_mode, RK_S32 gop_len, RK_S32 vi_len)
{
    RK_S32 lt_cnt = 0;
    RK_S32 st_cnt = 0;
    MPP_RET ret = MPP_OK;
    MppEncRefLtFrmCfg lt_ref[4];
    MppEncRefStFrmCfg st_ref[16];

    memset(&lt_ref, 0, sizeof(lt_ref));
    memset(&st_ref, 0, sizeof(st_ref));

    switch (gop_mode) {
        case GOP_MODE_TSVC4: {
            lt_cnt = 1;

            /* set 8 frame lt-ref gap */
            lt_ref[0].lt_idx = 0;
            lt_ref[0].temporal_id = 0;
            lt_ref[0].ref_mode = REF_TO_PREV_LT_REF;
            lt_ref[0].lt_gap = 8;
            lt_ref[0].lt_delay = 0;

            st_cnt = 9;
            /* set tsvc4 st-ref struct */
            /* st 0 layer 0 - ref */
            st_ref[0].is_non_ref = 0;
            st_ref[0].temporal_id = 0;
            st_ref[0].ref_mode = REF_TO_TEMPORAL_LAYER;
            st_ref[0].ref_arg = 0;
            st_ref[0].repeat = 0;
            /* st 1 layer 3 - non-ref */
            st_ref[1].is_non_ref = 1;
            st_ref[1].temporal_id = 3;
            st_ref[1].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[1].ref_arg = 0;
            st_ref[1].repeat = 0;
            /* st 2 layer 2 - ref */
            st_ref[2].is_non_ref = 0;
            st_ref[2].temporal_id = 2;
            st_ref[2].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[2].ref_arg = 0;
            st_ref[2].repeat = 0;
            /* st 3 layer 3 - non-ref */
            st_ref[3].is_non_ref = 1;
            st_ref[3].temporal_id = 3;
            st_ref[3].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[3].ref_arg = 0;
            st_ref[3].repeat = 0;
            /* st 4 layer 1 - ref */
            st_ref[4].is_non_ref = 0;
            st_ref[4].temporal_id = 1;
            st_ref[4].ref_mode = REF_TO_PREV_LT_REF;
            st_ref[4].ref_arg = 0;
            st_ref[4].repeat = 0;
            /* st 5 layer 3 - non-ref */
            st_ref[5].is_non_ref = 1;
            st_ref[5].temporal_id = 3;
            st_ref[5].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[5].ref_arg = 0;
            st_ref[5].repeat = 0;
            /* st 6 layer 2 - ref */
            st_ref[6].is_non_ref = 0;
            st_ref[6].temporal_id = 2;
            st_ref[6].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[6].ref_arg = 0;
            st_ref[6].repeat = 0;
            /* st 7 layer 3 - non-ref */
            st_ref[7].is_non_ref = 1;
            st_ref[7].temporal_id = 3;
            st_ref[7].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[7].ref_arg = 0;
            st_ref[7].repeat = 0;
            /* st 8 layer 0 - ref */
            st_ref[8].is_non_ref = 0;
            st_ref[8].temporal_id = 0;
            st_ref[8].ref_mode = REF_TO_TEMPORAL_LAYER;
            st_ref[8].ref_arg = 0;
            st_ref[8].repeat = 0;
        } break;

        case GOP_MODE_TSVC3: {
            lt_cnt = 0;

            st_cnt = 5;
            /* set tsvc4 st-ref struct */
            /* st 0 layer 0 - ref */
            st_ref[0].is_non_ref = 0;
            st_ref[0].temporal_id = 0;
            st_ref[0].ref_mode = REF_TO_TEMPORAL_LAYER;
            st_ref[0].ref_arg = 0;
            st_ref[0].repeat = 0;
            /* st 1 layer 2 - non-ref */
            st_ref[1].is_non_ref = 1;
            st_ref[1].temporal_id = 2;
            st_ref[1].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[1].ref_arg = 0;
            st_ref[1].repeat = 0;
            /* st 2 layer 1 - ref */
            st_ref[2].is_non_ref = 0;
            st_ref[2].temporal_id = 1;
            st_ref[2].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[2].ref_arg = 0;
            st_ref[2].repeat = 0;
            /* st 3 layer 2 - non-ref */
            st_ref[3].is_non_ref = 1;
            st_ref[3].temporal_id = 2;
            st_ref[3].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[3].ref_arg = 0;
            st_ref[3].repeat = 0;
            /* st 4 layer 0 - ref */
            st_ref[4].is_non_ref = 0;
            st_ref[4].temporal_id = 0;
            st_ref[4].ref_mode = REF_TO_TEMPORAL_LAYER;
            st_ref[4].ref_arg = 0;
            st_ref[4].repeat = 0;
        }
        break;

        case GOP_MODE_TSVC2: {
            lt_cnt = 0;

            st_cnt = 3;
            /* set tsvc4 st-ref struct */
            /* st 0 layer 0 - ref */
            st_ref[0].is_non_ref = 0;
            st_ref[0].temporal_id = 0;
            st_ref[0].ref_mode = REF_TO_TEMPORAL_LAYER;
            st_ref[0].ref_arg = 0;
            st_ref[0].repeat = 0;
            /* st 1 layer 2 - non-ref */
            st_ref[1].is_non_ref = 1;
            st_ref[1].temporal_id = 1;
            st_ref[1].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[1].ref_arg = 0;
            st_ref[1].repeat = 0;
            /* st 2 layer 1 - ref */
            st_ref[2].is_non_ref = 0;
            st_ref[2].temporal_id = 0;
            st_ref[2].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[2].ref_arg = 0;
            st_ref[2].repeat = 0;
        }
        break;

        case GOP_MODE_SMARTP: {
            int pos = 0;
            lt_cnt = 1;
            st_cnt = 3;
            /* set gop_len frame lt-ref gap */
            lt_ref[0].lt_idx = 0;
            lt_ref[0].temporal_id = 0;
            lt_ref[0].ref_mode = REF_TO_PREV_LT_REF;
            lt_ref[0].lt_gap = gop_len;
            lt_ref[0].lt_delay = 0;

            /* set smartp st-ref struct */
            /* st 0 layer 0 - ref */
            st_ref[pos].is_non_ref = 0;
            st_ref[pos].temporal_id = 0;
            st_ref[pos].ref_mode = REF_TO_PREV_INTRA;
            st_ref[pos].ref_arg = 0;
            st_ref[pos].repeat = 0;
            pos++;

            /* st 1 layer 1 - non-ref */
            if (vi_len > 1) {
            st_ref[pos].is_non_ref = 0;
            st_ref[pos].temporal_id = 1;
            st_ref[pos].ref_mode = REF_TO_PREV_REF_FRM;
            st_ref[pos].ref_arg = 0;
            st_ref[pos].repeat = (vi_len > 2) ? (vi_len - 2) : 0;
            pos++;
            }

            /* st 2 layer 0 - ref */
            st_ref[pos].is_non_ref = 0;
            st_ref[pos].temporal_id = 0;
            st_ref[pos].ref_mode = REF_TO_PREV_INTRA;
            st_ref[pos].ref_arg = 0;
            st_ref[pos].repeat = 0;
        }
        break;

        default: {
            DRM_MEDIA_LOGE("MPP: unsupport gop mode:[%d]", gop_mode);
        }
        break;
    }

    if (lt_cnt || st_cnt) {
        ret = mpp_enc_ref_cfg_set_cfg_cnt(ref, lt_cnt, st_cnt);
        if (lt_cnt) {
            ret = mpp_enc_ref_cfg_add_lt_cfg(ref, lt_cnt, lt_ref);
        }

        if (st_cnt) {
            ret = mpp_enc_ref_cfg_add_st_cfg(ref, st_cnt, st_ref);
        }

        ret = mpp_enc_ref_cfg_check(ref);
    }

    return ret;
}
}
