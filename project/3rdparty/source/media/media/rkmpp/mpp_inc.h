#ifndef LIBDRM_MEDIA_MPP_INC_H
#define LIBDRM_MEDIA_MPP_INC_H

#include <media/utils/image.h>
#include <media/utils/buffer.h>
#include <rockchip/rkmpp/rk_mpi.h>

namespace libdrm {
#ifndef MPP_PACKET_FLAG_INTRA
#define MPP_PACKET_FLAG_INTRA               (0x00000008)
#endif

MppFrameFormat ConvertToMppPixFmt(const DrmPixelFormat &fmt);
DrmPixelFormat ConvertToPixFmt(const MppFrameFormat &mfmt);

const char *MppAcceptImageFmts();

MppCodingType GetMPPCodingType(const std::string &data_type);
MppEncRcQuality GetMPPRCQuality(const char *quality);
MppEncRcMode GetMPPRCMode(const char *rc_mode);

struct MPPContext {
    MPPContext();
    ~MPPContext();

    MppCtx         ctx;
    MppApi         *mpi;
    MppBufferGroup frame_group;
};

MPP_RET init_mpp_buffer(MppBuffer &buffer, const std::shared_ptr<MediaBuffer> &mb, size_t frame_size);
MPP_RET init_mpp_buffer_with_content(MppBuffer &buffer, const std::shared_ptr<MediaBuffer> &mb);

MPP_RET mpi_enc_gen_ref_cfg(MppEncRefCfg ref, RK_S32 gop_mode = 2, RK_S32 gop_len = 0, RK_S32 vi_len = 0);
}

#endif
