#include <linux/kconfig.h>

#include <mutex>
#include <string>
#include <sstream>
#include <algorithm>
#include <condition_variable>

#include <fcntl.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#if defined(CONFIG_RKRGA)
#if defined(CONFIG_RKRGA_OLD)
#include <rockchip/rkrga/rga.h>
#include <rockchip/rkrga/im2d.h>
#else
#include <rockchip/rkrgax/rga.h>
#include <rockchip/rkrgax/im2d.h>
#endif
#endif

#include <media/drm_media_vi.h>
#include <media/drm_media_vo.h>
#include <media/drm_media_vp.h>
#include <media/drm_media_rga.h>
#include <media/drm_media_venc.h>
#include <media/drm_media_vdec.h>
#include <media/drm_media_vmix.h>

#include <media/drm_media_api.h>

#include <media/utils/utils.h>
#include <media/utils/image.h>
#include <media/utils/stream.h>
#include <media/utils/control.h>
#include <media/utils/encoder.h>
#include <media/utils/message.h>
#include <media/drm_media_buffer.h>
#include <media/utils/key_string.h>
#include <media/utils/media_type.h>
#include <media/utils/media_config.h>

#include "media_utils.h"
#include "osd/color_table.h"
#include "drm_media_buffer_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CHN_OUT_CB_STATUS {
    CHN_OUT_CB_INIT,
    CHN_OUT_CB_USER,
    CHN_OUT_CB_LUMA,
    CHN_OUT_CB_CLOSE
} chn_out_cb_status_e;

typedef enum CHN_STATUS {
    CHN_STATUS_CLOSED,
    CHN_STATUS_READY,
    CHN_STATUS_OPEN,
    CHN_STATUS_BIND,
} chn_status_e;

typedef struct venc_channel_attr {
    drm_venc_chn_attr_t  attr;
    drm_venc_chn_param_t param;
    drm_venc_rc_param_t  stRcPara;
    drm_venc_roi_attr_t  astRoiAttr[8];
    bool                 bFullFunc;
} venc_channel_attr_t;

typedef struct drm_media_channel {
    mod_id_e                                 mode_id;
    uint16_t                                 chn_id;
    chn_status_e                             status;
    std::shared_ptr<libdrm::Flow>            media_flow;
    std::list<std::shared_ptr<libdrm::Flow>> media_flow_list;
    OutCallbackFunction                      out_cb;
    OutCallbackFunctionEx                    out_ex_cb;
    void                                     *out_handle;
    EventCallbackFunction                    event_cb;
    void                                     *event_handle;

    union {
        drm_vi_chn_attr_t                    vi_attr;
        drm_vp_chn_attr_t                    vp_attr;
        drm_vo_chn_attr_t                    vo_attr;
        drm_vdec_chn_attr_t                  vdec_attr;
        venc_channel_attr_t                  venc_attr;
    };

    uint16_t                                 chn_ref_cnt;
    int16_t                                  bind_ref_pre;
    int16_t                                  bind_ref_nxt;
    std::mutex                               buffer_list_mtx;
    std::mutex                               buffer_list_pop_mtx;
    std::condition_variable                  buffer_list_cond;
    bool                                     buffer_list_quit;
    int                                      wake_fd[2];
    std::list<media_buffer_t>                buffer_list;

    chn_out_cb_status_e                      media_out_cb_status;
    uint32_t                                 buffer_depth;

    bool                                     bColorTblInit;
    bool                                     bColorDichotomyEnable;
    uint32_t                                 u32ArgbColorTbl[256];

    std::mutex                               luma_buf_mtx;
    std::condition_variable                  luma_buf_cond;
    bool                                     luma_buf_quit;
    bool                                     luma_buf_start;
    std::shared_ptr<libdrm::MediaBuffer>     luma_media_buf;
} drm_media_channel_t;

typedef struct video_mix_device {
    bool                          bInit;
    uint16_t                      u16Fps;
    uint16_t                      u16ChnCnt;
    drm_rect_t                    stSrcRect[DRM_VMIX_CHANNEL_BUTT];
    drm_rect_t                    stDstRect[DRM_VMIX_CHANNEL_BUTT];
    std::shared_ptr<libdrm::Flow> media_flow;
    uint16_t                      u16RefCnt;
    std::mutex                    VmixMtx;
    drm_media_channel_t           VmixChns[DRM_VMIX_CHANNEL_BUTT];
} video_mix_devices_t;

static std::mutex g_vi_mtx;
static std::mutex g_vo_mtx;
static std::mutex g_vp_mtx;
static std::mutex g_rga_mtx;
static std::mutex g_venc_mtx;
static std::mutex g_vdec_mtx;

static drm_media_channel_t g_vi_chns[DRM_VI_CHANNEL_BUTT];
static drm_media_channel_t g_vp_chns[DRM_VP_CHANNEL_BUTT];
static drm_media_channel_t g_vo_chns[DRM_VO_CHANNEL_BUTT];
static video_mix_devices_t g_vmix_dev[DRM_VMIX_DEVICE_BUTT];
static drm_media_channel_t g_rga_chns[DRM_RGA_CHANNEL_BUTT];
static drm_media_channel_t g_venc_chns[DRM_VENC_CHANNEL_BUTT];
static drm_media_channel_t g_vdec_chns[DRM_VDEC_CHANNEL_BUTT];

static unsigned char g_sys_init = 0;

extern std::mutex g_handle_mb_mutex;
extern std::list<handle_mb_t *> g_handle_mb;

static const char *mod_tag_list[MOD_ID_BUTT] = {
    "UNKNOW", "VB", "SYS", "VDEC", "VENC", "VO", "VI", "VP", "RGA", "VMIX",
};

static const char *ModIdToString(mod_id_e mod_id)
{
    if ((mod_id < MOD_ID_UNKNOW) || (mod_id >= MOD_ID_BUTT)) {
        DRM_MEDIA_LOGE("mod_id is incorrect");
        return mod_tag_list[MOD_ID_UNKNOW];
    }

    return mod_tag_list[mod_id];
}

#if defined(CONFIG_RKRGA)
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
        {DRM_PIX_FMT_ABGR8888, RK_FORMAT_RGBA_8888},
        {DRM_PIX_FMT_XRGB8888, RK_FORMAT_XRGB_8888}
    };

    auto it = rga_format_map.find(f);
    if (it != rga_format_map.end()) {
        return it->second;
    }

    return -1;
}
#endif

static mb_type_e get_buffer_type(drm_media_channel_t *target_chn)
{
    mb_type_e type = (mb_type_e)0;

    if (!target_chn) {
        return type;
    }

    switch (target_chn->mode_id) {
        case MOD_ID_VI:
        case MOD_ID_VP:
        case MOD_ID_RGA:
        case MOD_ID_VDEC:
        case MOD_ID_VMIX:
            type = MB_TYPE_IMAGE;
            break;

        case MOD_ID_VENC:
            if (target_chn->venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_H264) {
                type = MB_TYPE_H264;
            } else if (target_chn->venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_H265) {
                type = MB_TYPE_H265;
            } else if (target_chn->venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
                type = MB_TYPE_MJPEG;
            } else if (target_chn->venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_JPEG) {
                type = MB_TYPE_JPEG;
            }
            break;

        default:
            break;
    }

    return type;
}

static void media_pop_pipfd(int fd)
{
    int i = 0;
    ssize_t read_size = (ssize_t)sizeof(i);
    ssize_t ret = read(fd, &i, sizeof(i));
    if (ret != read_size) {
        DRM_MEDIA_LOGE("read(%d) failed, errstr:[%s]", fd, strerror(errno));
    }
}

static void media_push_pipfd(int fd)
{
    int i = 0;
    ssize_t count = write(fd, &i, sizeof(i));
    if (count < 0) {
        DRM_MEDIA_LOGE("write(%d) failed, errstr:[%s]", fd, strerror(errno));
    }
}

static media_buffer_t media_channel_pop_buffer(drm_media_channel_t *ptrChn, int s32MilliSec)
{
    if (!ptrChn) {
        return NULL;
    }

    std::unique_lock<std::mutex> lck_pop(ptrChn->buffer_list_pop_mtx);
    std::unique_lock<std::mutex> lck(ptrChn->buffer_list_mtx);
    if (ptrChn->buffer_list.empty()) {
        if (s32MilliSec < 0 && !ptrChn->buffer_list_quit) {
            ptrChn->buffer_list_cond.wait(lck);
        } else if (s32MilliSec > 0) {
            if (ptrChn->buffer_list_cond.wait_for(lck, std::chrono::milliseconds(s32MilliSec)) == std::cv_status::timeout) {
                DRM_MEDIA_LOGW("mode:[%s]-chn:[%d] get media buffer timeout", ModIdToString(ptrChn->mode_id), ptrChn->chn_id);
                return NULL;
            }
        } else {
            return NULL;
        }
    }

    media_buffer_t mb = NULL;
    if (!ptrChn->buffer_list.empty()) {
        if (ptrChn->wake_fd[0] > 0) {
            media_pop_pipfd(ptrChn->wake_fd[0]);
        }

        mb = ptrChn->buffer_list.front();
        ptrChn->buffer_list.pop_front();
    }

    return mb;
}

static int media_channel_push_buffer(drm_media_channel_t *ptrChn, media_buffer_t buffer)
{
    if (!ptrChn || !buffer) {
        return -1;
    }

    ptrChn->buffer_list_mtx.lock();
    if (!ptrChn->buffer_list_quit) {
        ptrChn->buffer_list.push_back(buffer);
        if (ptrChn->wake_fd[1] > 0) {
            media_push_pipfd(ptrChn->wake_fd[1]);
        }
    } else {
        drm_mpi_mb_release_buffer(buffer);
    }

    ptrChn->buffer_list_cond.notify_all();
    ptrChn->buffer_list_mtx.unlock();
    pthread_yield();

    return 0;
}

static void init_media_channel_buffer(drm_media_channel_t *ptrChn)
{
    if (!ptrChn) {
        return;
    }

    ptrChn->buffer_list_mtx.lock();
    ptrChn->buffer_list_quit = false;
    ptrChn->buffer_list_mtx.unlock();
}

static void clear_media_channel_buffer(drm_media_channel_t *ptrChn)
{
    if (!ptrChn) {
        return;
    }

    DRM_MEDIA_LOGD("mode:[%s] chn:[%d] clear media buffer starting......", ModIdToString(ptrChn->mode_id), ptrChn->chn_id);

    media_buffer_t mb = NULL;
    ptrChn->buffer_list_mtx.lock();
    while (!ptrChn->buffer_list.empty()) {
        mb = ptrChn->buffer_list.front();
        ptrChn->buffer_list.pop_front();
        drm_mpi_mb_release_buffer(mb);
    }
    ptrChn->buffer_list_quit = true;
    ptrChn->buffer_list_cond.notify_all();
    ptrChn->buffer_list_mtx.unlock();

    DRM_MEDIA_LOGD("mode:[%s] chn:[%d] clear media buffer finished......", ModIdToString(ptrChn->mode_id), ptrChn->chn_id);
}

static void reset_channel_table(drm_media_channel_t *tbl, int cnt, mod_id_e mid)
{
    for (int i = 0; i < cnt; i++) {
        tbl[i].mode_id = mid;
        tbl[i].chn_id = i;
        tbl[i].status = CHN_STATUS_CLOSED;
        tbl[i].out_cb = nullptr;
        tbl[i].out_ex_cb = nullptr;
        tbl[i].out_handle = nullptr;
        tbl[i].event_cb = nullptr;
        tbl[i].event_handle = nullptr;
        tbl[i].chn_ref_cnt = 0;
        tbl[i].bind_ref_pre = 0;
        tbl[i].bind_ref_nxt = 0;
        tbl[i].bColorTblInit = false;
        tbl[i].bColorDichotomyEnable = false;
        memset(tbl[i].u32ArgbColorTbl, 0, 0);
        tbl[i].buffer_depth = 2;
    }
}

static void flow_event_callback(void *handle, void *user_data)
{
    if (!user_data) {
        return;
    }

    if (!handle) {
        DRM_MEDIA_LOGE("without handle");
        return;
    }

    drm_media_channel_t *target_chn = (drm_media_channel_t *)handle;
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] in status:[%d] should not call output callback", ModIdToString(target_chn->mode_id), target_chn->chn_id, target_chn->status);
        return;
    }

    if (!target_chn->event_cb) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] in has no callback", ModIdToString(target_chn->mode_id), target_chn->chn_id);
        return;
    }

    switch (target_chn->mode_id) {
        default:
            DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] not support event callback", ModIdToString(target_chn->mode_id), target_chn->chn_id);
            break;
    }
}

static void flow_output_callback(void *handle, std::shared_ptr<libdrm::MediaBuffer> media_mb)
{
    if (!media_mb) {
        return;
    }

    if (!handle) {
        DRM_MEDIA_LOGE("without handle");
        return;
    }

    drm_media_channel_t *target_chn = (drm_media_channel_t *)handle;
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] in status:[%d] should not call output callback", ModIdToString(target_chn->mode_id), target_chn->chn_id, target_chn->status);
        return;
    }

    bool busy = false;
    uint32_t depth = 0;

    g_handle_mb_mutex.lock();
    for (auto &p : g_handle_mb) {
        if (handle == p->handle)
        depth++;
    }

    if (depth >= target_chn->buffer_depth) {
        busy = true;
    }

    g_handle_mb_mutex.unlock();
    if (busy) {
        if (target_chn->buffer_depth < 2) {
            DRM_MEDIA_LOGW("mode:[%s]-chn:[%d] drop current buffer", ModIdToString(target_chn->mode_id), target_chn->chn_id);
            return;
        } else {
            DRM_MEDIA_LOGW("mode:[%s]-chn:[%d] drop front buffer", ModIdToString(target_chn->mode_id), target_chn->chn_id);

            target_chn->buffer_list_mtx.lock();
            if (!target_chn->buffer_list.empty()) {
                if (target_chn->wake_fd[0] > 0) {
                    media_pop_pipfd(target_chn->wake_fd[0]);
                }

                media_buffer_t mb = target_chn->buffer_list.front();
                target_chn->buffer_list.pop_front();
                drm_mpi_mb_release_buffer(mb);
            }
            target_chn->buffer_list_mtx.unlock();
        }
    }

    mb_type_e mb_type = get_buffer_type(target_chn);

    if (target_chn->mode_id == MOD_ID_VI) {
        std::unique_lock<std::mutex> lck(target_chn->luma_buf_mtx);
        if (!target_chn->luma_buf_quit && target_chn->luma_buf_start) {
            target_chn->luma_media_buf = media_mb;
        }
        target_chn->luma_buf_cond.notify_all();

        if (target_chn->vi_attr.enWorkMode == DRM_VI_WORK_MODE_LUMA_ONLY) {
            return;
        }
    }

    media_buffer_impl_t *mb = new media_buffer_impl_t;
    if (!mb) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] no space left for new mb", ModIdToString(target_chn->mode_id), target_chn->chn_id);
        return;
    }

    mb->ptr = media_mb->GetPtr();
    mb->fd = media_mb->GetFD();
    mb->handle = media_mb->GetHandle();
    mb->dev_fd = media_mb->GetDevFD();
    mb->size = media_mb->GetValidSize();
    mb->media_mb = media_mb;
    mb->mode_id = target_chn->mode_id;
    mb->chn_id = target_chn->chn_id;
    mb->timestamp = (uint64_t)media_mb->GetUSTimeStamp();
    mb->type = mb_type;
    if ((mb_type == MB_TYPE_H264) || (mb_type == MB_TYPE_H265)) {
        mb->flag = (media_mb->GetUserFlag() & libdrm::MediaBuffer::kIntra) ? VENC_NALU_IDRSLICE : VENC_NALU_PSLICE;
        mb->tsvc_level = media_mb->GetTsvcLevel();
    } else {
        mb->flag = 0;
        mb->tsvc_level = 0;
    }

    if (mb_type == MB_TYPE_IMAGE) {
        libdrm::ImageBuffer *media_ib = static_cast<libdrm::ImageBuffer *>(media_mb.get());
        mb->stImageInfo.u32Width = media_ib->GetWidth();
        mb->stImageInfo.u32Height = media_ib->GetHeight();
        mb->stImageInfo.u32HorStride = media_ib->GetVirWidth();
        mb->stImageInfo.u32VerStride = media_ib->GetVirHeight();
        std::string strPixFmt = PixFmtToString(media_ib->GetPixelFormat());
        mb->stImageInfo.enImgType = StringToImageType(strPixFmt);
    }

    handle_mb_t *hmb = (handle_mb_t *)malloc(sizeof(handle_mb_t));
    if (hmb) {
        hmb->handle = handle;
        hmb->mb = mb;
        g_handle_mb_mutex.lock();
        g_handle_mb.push_back(hmb);
        g_handle_mb_mutex.unlock();
    } else {
        DRM_MEDIA_LOGE("no space left");
    }

    if (target_chn->out_cb) {
        target_chn->out_cb(mb);
    } else if (target_chn->out_ex_cb) {
        target_chn->out_ex_cb(mb, target_chn->out_handle);
    } else {
        media_channel_push_buffer(target_chn, mb);
    }
}

int drm_mpi_system_init(void)
{
    if (g_sys_init) {
        printf("drm_mpi_system_init has been called\n");
        return -1;
    }

    drm_log_init();

    for (int i = MOD_ID_UNKNOW; i < MOD_ID_BUTT; i++) {
        g_level_list[i] = 3;
    }

    reset_channel_table(g_vi_chns, DRM_VI_CHANNEL_BUTT, MOD_ID_VI);
    reset_channel_table(g_vp_chns, DRM_VP_CHANNEL_BUTT, MOD_ID_VP);
    reset_channel_table(g_vo_chns, DRM_VO_CHANNEL_BUTT, MOD_ID_VO);
    reset_channel_table(g_rga_chns, DRM_RGA_CHANNEL_BUTT, MOD_ID_RGA);
    reset_channel_table(g_venc_chns, DRM_VENC_CHANNEL_BUTT, MOD_ID_VENC);
    reset_channel_table(g_vdec_chns, DRM_VDEC_CHANNEL_BUTT, MOD_ID_VDEC);

    for (int i = DRM_VMIX_DEVICE_00; i < DRM_VMIX_DEVICE_BUTT; i++) {
        g_vmix_dev[i].bInit = false;
        g_vmix_dev[i].u16ChnCnt = 0;
        g_vmix_dev[i].u16RefCnt = 0;
        reset_channel_table(g_vmix_dev[i].VmixChns, DRM_VMIX_CHANNEL_BUTT, MOD_ID_VMIX);
    }

    g_sys_init = 1;
    return 0;
}

int drm_mpi_system_bind(const drm_chn_t *pstSrcChn, const drm_chn_t *pstDstChn)
{
    int dst_in_idx = 0;
    int src_out_idx = 0;
    std::mutex *src_mutex = NULL;
    std::mutex *dst_mutex = NULL;
    std::shared_ptr<libdrm::Flow> src;
    std::shared_ptr<libdrm::Flow> sink;
    drm_media_channel_t *src_chn = NULL;
    drm_media_channel_t *dst_chn = NULL;

    if (!pstSrcChn || !pstDstChn) {
        return -1;
    }

    DRM_MEDIA_LOGI("bind mode:[%s]-chn:[%d] to mode:[%s]-chn:[%d]", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId, ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId);

    switch (pstSrcChn->enModId) {
        case MOD_ID_VI:
            src = g_vi_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_vi_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_vi_mtx;
            break;

        case MOD_ID_VENC:
            src = g_venc_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_venc_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            src = g_vdec_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_vdec_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            src = g_rga_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_rga_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX:
            src = g_vmix_dev[pstSrcChn->s32DevId].media_flow;
            src_mutex = &g_vmix_dev[pstSrcChn->s32DevId].VmixMtx;
            break;

        default:
            return -2;
    }

    if ((pstSrcChn->enModId != MOD_ID_VMIX) && (!src_chn || (src_chn->status < CHN_STATUS_OPEN) || !src)) {
        DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d] is not ready", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId);
        return -3;
    }

    switch (pstDstChn->enModId) {
        case MOD_ID_VO:
            sink = g_vo_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vo_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vo_mtx;
            break;

        case MOD_ID_VENC:
            sink = g_venc_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_venc_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            sink = g_vdec_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vdec_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            sink = g_rga_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_rga_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX:
            sink = g_vmix_dev[pstDstChn->s32DevId].VmixChns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vmix_dev[pstDstChn->s32DevId].VmixChns[pstDstChn->s32ChnId];
            dst_mutex = &g_vmix_dev[pstDstChn->s32DevId].VmixMtx;
            dst_in_idx = pstDstChn->s32ChnId;
            break;

        case MOD_ID_VP:
            sink = g_vp_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vp_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vp_mtx;
            break;

        default:
            return -4;
    }

    if ((!dst_chn || (dst_chn->status < CHN_STATUS_OPEN) || !sink)) {
        DRM_MEDIA_LOGE("dst mode:[%s]-chn:[%d] is not ready", ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId);
        return -5;
    }

    src_mutex->lock();
    if (!src->AddDownFlow(sink, src_out_idx, dst_in_idx)) {
        src_mutex->unlock();
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d]-out:[%d] -> mode:[%s]-chn:[%d]-in:[%d] failed", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId, src_out_idx, ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId, dst_in_idx);
        return -6;
    }

    if (pstSrcChn->enModId == MOD_ID_VMIX) {
        uint16_t u16ChnMaxCnt = g_vmix_dev[pstSrcChn->s32DevId].u16ChnCnt;
        for (int i = 0; i < u16ChnMaxCnt; i++) {
            src_chn = &g_vmix_dev[pstSrcChn->s32DevId].VmixChns[i];
            if (src_chn->status != CHN_STATUS_OPEN) {
                DRM_MEDIA_LOGW("SrcChn vmix dev:[%d]-chn:[%d] status:[%d] invalid", pstSrcChn->s32DevId, i, src_chn->status);
            } else {
                src_chn->status = CHN_STATUS_BIND;
                src_chn->bind_ref_nxt++;
            }

            if ((src_chn->media_out_cb_status == CHN_OUT_CB_INIT) || (src_chn->media_out_cb_status == CHN_OUT_CB_CLOSE)) {
                DRM_MEDIA_LOGD("disable media flow output callback");

                src_chn->media_out_cb_status = CHN_OUT_CB_CLOSE;
                src->SetOutputCallBack(NULL, NULL);
                clear_media_channel_buffer(src_chn);
            }
        }
    } else {
        if ((src_chn->media_out_cb_status == CHN_OUT_CB_INIT) || (src_chn->media_out_cb_status == CHN_OUT_CB_CLOSE)) {
            DRM_MEDIA_LOGD("disable media flow output callback");
            src_chn->media_out_cb_status = CHN_OUT_CB_CLOSE;
            src->SetOutputCallBack(NULL, NULL);
            clear_media_channel_buffer(src_chn);
        }

        src_chn->status = CHN_STATUS_BIND;
        src_chn->bind_ref_nxt++;
    }

    src_mutex->unlock();

    dst_mutex->lock();
    dst_chn->status = CHN_STATUS_BIND;
    dst_chn->bind_ref_pre++;
    dst_mutex->unlock();

    return 0;
}

int drm_mpi_system_unbind(const drm_chn_t *pstSrcChn, const drm_chn_t *pstDstChn)
{
    std::mutex *src_mutex = NULL;
    std::mutex *dst_mutex = NULL;
    std::shared_ptr<libdrm::Flow> src;
    std::shared_ptr<libdrm::Flow> sink;
    drm_media_channel_t *src_chn = NULL;
    drm_media_channel_t *dst_chn = NULL;

    DRM_MEDIA_LOGI("unbind mode:[%s]-chn:[%d] to mode:[%s]-chn[%d]", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId, ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId);

    switch (pstSrcChn->enModId) {
        case MOD_ID_VI:
            src = g_vi_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_vi_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_vi_mtx;
            break;

        case MOD_ID_VENC:
            src = g_venc_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_venc_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            src = g_vdec_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_vdec_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            src = g_rga_chns[pstSrcChn->s32ChnId].media_flow;
            src_chn = &g_rga_chns[pstSrcChn->s32ChnId];
            src_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX:
            src = g_vmix_dev[pstSrcChn->s32DevId].media_flow;
            src_mutex = &g_vmix_dev[pstSrcChn->s32DevId].VmixMtx;
            break;

        default:
            return -1;
    }

    if (!src) {
        DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d]: flow is null", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId);
        return -2;
    }

    if (pstSrcChn->enModId != MOD_ID_VMIX) {
        if (!src_chn) {
            DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d]: chn is null", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId);
            return -3;
        }

        if (src_chn->status != CHN_STATUS_BIND) {
            DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d]: chn not bind", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId);
            return -4;
        }

        if (src_chn->bind_ref_nxt <= 0) {
            DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d]: nxt-ref:[%d]", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId, src_chn->bind_ref_nxt);
            return -5;
        }
    }

    switch (pstDstChn->enModId) {
        case MOD_ID_VO:
            sink = g_vo_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vo_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vo_mtx;
            break;

        case MOD_ID_VENC:
            sink = g_venc_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_venc_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            sink = g_vdec_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vdec_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            sink = g_rga_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_rga_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX:
            sink = g_vmix_dev[pstDstChn->s32DevId].VmixChns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vmix_dev[pstDstChn->s32DevId].VmixChns[pstDstChn->s32ChnId];
            dst_mutex = &g_vmix_dev[pstDstChn->s32DevId].VmixMtx;
            break;

        case MOD_ID_VP:
            sink = g_vp_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vp_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vp_mtx;
            break;

        default:
            return -6;
    }

    if (!sink) {
        DRM_MEDIA_LOGE("dst mode:[%s]-chn:[%d]: flow is null", ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId);
        return -7;
    }

    if (!dst_chn) {
        DRM_MEDIA_LOGE("dst mode:[%s]-chn:[%d]: chn is null", ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId);
        return -8;
    }

    if ((dst_chn->status != CHN_STATUS_BIND)) {
        DRM_MEDIA_LOGE("dst mode:[%s]-chn:[%d]: chn not bind", ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId);
        return -9;
    }

    if ((dst_chn->bind_ref_pre <= 0) || (!sink)) {
        DRM_MEDIA_LOGE("dst mode:[%s]-chn:[%d](pre-ref:[%d]):Serious error status", ModIdToString(pstDstChn->enModId), pstDstChn->s32ChnId, dst_chn->bind_ref_pre);
        return -10;
    }

    src_mutex->lock();
    src->RemoveDownFlow(sink);

    if (pstSrcChn->enModId == MOD_ID_VMIX) {
        uint16_t u16ChnMaxCnt = g_vmix_dev[pstSrcChn->s32DevId].u16ChnCnt;
        for (int i = 0; i < u16ChnMaxCnt; i++) {
            src_chn = &g_vmix_dev[pstSrcChn->s32DevId].VmixChns[i];
            if (src_chn->status != CHN_STATUS_BIND) {
                DRM_MEDIA_LOGW("SrcChn vmix dev:[%d]-chn:[%d] status(%d) invalid", pstSrcChn->s32DevId, i, src_chn->status);
                continue;
            }

            src_chn->bind_ref_nxt--;
            if ((src_chn->bind_ref_nxt <= 0) && (src_chn->bind_ref_pre <= 0)) {
                src_chn->status = CHN_STATUS_OPEN;
                src_chn->bind_ref_pre = 0;
                src_chn->bind_ref_nxt = 0;
            }
        }
    } else {
        src_chn->bind_ref_nxt--;
        if ((src_chn->bind_ref_nxt <= 0) && (src_chn->bind_ref_pre <= 0)) {
            src_chn->status = CHN_STATUS_OPEN;
            src_chn->bind_ref_pre = 0;
            src_chn->bind_ref_nxt = 0;
        }
    }
    src_mutex->unlock();

    dst_mutex->lock();
    dst_chn->bind_ref_pre--;
    if ((dst_chn->bind_ref_nxt <= 0) && (dst_chn->bind_ref_pre <= 0)) {
        dst_chn->status = CHN_STATUS_OPEN;
        dst_chn->bind_ref_pre = 0;
        dst_chn->bind_ref_nxt = 0;
    }
    dst_mutex->unlock();

    return 0;
}

int drm_mpi_system_set_framerate(mod_id_e enModID, int s32ChnId, drm_fps_attr_t *pstFpsAttr)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    if (!pstFpsAttr) {
        DRM_MEDIA_LOGE("fps attribute null");
        return -1;
    }

    int s32FpsInRatio = pstFpsAttr->s32FpsInNum * pstFpsAttr->s32FpsOutDen;
    int s32FpsOutRatio = pstFpsAttr->s32FpsOutNum * pstFpsAttr->s32FpsInDen;

    if (s32FpsInRatio < s32FpsOutRatio) {
        DRM_MEDIA_LOGE("s32FpsInRatio:[%d] < s32FpsOutRatio:[%d]", s32FpsInRatio, s32FpsOutRatio);
        return -2;
    }

    switch (enModID) {
        case MOD_ID_VO:
            if ((s32ChnId < DRM_VO_CHANNEL_00) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid channel id:[%d]", s32ChnId);
                return -3;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                return -4;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                return -5;
            }
            target_chn = &g_venc_chns[s32ChnId];
            target_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                return -6;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            target_mutex = &g_vdec_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -7;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("channel:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -8;
    }

    int ret = 0;
    ret = target_chn->media_flow->SetInputFpsControl(s32FpsInRatio, s32FpsOutRatio);
    target_mutex->unlock();
    if (ret) {
        DRM_MEDIA_LOGE("SetInputFpsControl failed, return:[%d]", ret);
        return -9;
    }

    return 0;
}

int drm_mpi_system_start_recv_frame(mod_id_e enModID, int s32ChnId, const drm_recv_pic_param_t *pstRecvParam)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    if (!pstRecvParam) {
        DRM_MEDIA_LOGE("recv param is NULL");
        return -1;
    }

    switch (enModID) {
        case MOD_ID_VO:
            if ((s32ChnId < DRM_VO_CHANNEL_00) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid channel id:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                return -3;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                return -4;
            }
            target_chn = &g_venc_chns[s32ChnId];
            target_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                return -5;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            target_mutex = &g_vdec_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -6;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("channel:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -7;
    }

    int ret = target_chn->media_flow->SetRunTimes(pstRecvParam->s32RecvPicNum);
    target_mutex->unlock();
    if (ret != pstRecvParam->s32RecvPicNum) {
        DRM_MEDIA_LOGE("SetRunTimes failed, return:[%d]", ret);
        return -8;
    }

    return 0;
}

int drm_mpi_system_set_media_buffer_depth(mod_id_e enModID, int s32ChnId, int depth)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VI:
            if ((s32ChnId < DRM_VI_CHANNEL_00) || (s32ChnId >= DRM_VI_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vi channel:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vi_chns[s32ChnId];
            target_mutex = &g_vi_mtx;
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid venc channel:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_venc_chns[s32ChnId];
            target_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vdec channel:[%d]", s32ChnId);
                return -3;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            target_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid RGA channe:[%d]", s32ChnId);
                return -4;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX: {
            int s32DevId = s32ChnId;
            if ((s32DevId < DRM_VMIX_DEVICE_00) || (s32DevId >= DRM_VMIX_DEVICE_BUTT)) {
                return -5;
            }
            target_chn = &g_vmix_dev[s32DevId].VmixChns[0];
            target_mutex = &g_vmix_dev[s32DevId].VmixMtx;
            break;
        }

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -6;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -7;
    }

    target_mutex->lock();
    target_chn->buffer_depth = depth;
    target_mutex->unlock();

    return 0;
}

int drm_mpi_system_send_media_buffer(mod_id_e enModID, int s32ChnId, media_buffer_t buffer)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VO:
            if ((s32ChnId < DRM_VO_CHANNEL_00) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid channel id:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                return -2;
            }
            target_chn = &g_venc_chns[s32ChnId];
            target_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                return -3;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            target_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                return -4;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VP:
            if ((s32ChnId < DRM_VP_CHANNEL_00) || (s32ChnId >= DRM_VP_CHANNEL_BUTT)) {
                return -5;
            }
            target_chn = &g_vp_chns[s32ChnId];
            target_mutex = &g_vp_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -6;
    }

    media_buffer_impl_t *mb = (media_buffer_impl_t *)buffer;
    target_mutex->lock();
    if (target_chn->media_flow) {
        target_chn->media_flow->SendInput(mb->media_mb, 0);
    } else {
        target_mutex->unlock();
        DRM_MEDIA_LOGE("media flow null");
        return -7;
    }

    target_mutex->unlock();
    return 0;
}

int drm_mpi_system_device_send_media_buffer(mod_id_e enModID, int s32DevId, int s32ChnId, media_buffer_t buffer)
{
    int target_slot = 0;
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VMIX:
            if ((s32ChnId < DRM_VMIX_CHANNEL_00) || (s32ChnId >= DRM_VMIX_CHANNEL_BUTT) || (s32DevId < DRM_VMIX_DEVICE_00) || (s32DevId >= DRM_VMIX_DEVICE_BUTT)) {
                return -1;
            }
            target_chn = &g_vmix_dev[s32DevId].VmixChns[s32ChnId];
            target_mutex = &g_vmix_dev[s32DevId].VmixMtx;
            target_slot = s32ChnId;
            break;

        default:
            return drm_mpi_system_send_media_buffer(enModID, s32ChnId, buffer);
    }

    media_buffer_impl_t *mb = (media_buffer_impl_t *)buffer;

    target_mutex->lock();
    if (target_chn->media_flow) {
        target_chn->media_flow->SendInput(mb->media_mb, target_slot);
    } else {
        target_mutex->unlock();
        return -2;
    }
    target_mutex->unlock();

    return 0;
}

int drm_mpi_system_stop_get_media_buffer(mod_id_e enModID, int s32ChnId)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VI:
            if ((s32ChnId < DRM_VI_CHANNEL_00) || (s32ChnId >= DRM_VI_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid VI channel:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vi_chns[s32ChnId];
            target_mutex = &g_vi_mtx;
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid venc channel:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_venc_chns[s32ChnId];
            target_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vdec channel:[%d]", s32ChnId);
                return -3;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            target_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid RGA channel:[%d]", s32ChnId);
                return -4;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -5;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -6;
    }

    clear_media_channel_buffer(target_chn);

    target_mutex->lock();
    target_chn->media_out_cb_status = CHN_OUT_CB_CLOSE;
    target_chn->media_flow->SetOutputCallBack(NULL, NULL);
    DRM_MEDIA_LOGD("disable media output callback");
    target_mutex->unlock();

    return 0;
}

int drm_mpi_system_start_get_media_buffer(mod_id_e enModID, int s32ChnId)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VI:
            if ((s32ChnId < DRM_VI_CHANNEL_00) || (s32ChnId >= DRM_VI_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid VI channel:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vi_chns[s32ChnId];
            target_mutex = &g_vi_mtx;
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vecn channel:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_venc_chns[s32ChnId];
            target_mutex = &g_venc_mtx;
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vdec channel:[%d]", s32ChnId);
                return -3;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            target_mutex = &g_vdec_mtx;
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid RGA channel:[%d]", s32ChnId);
                return -4;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX: {
            int s32DevId = s32ChnId;
            if ((s32DevId < DRM_VMIX_DEVICE_00) || (s32DevId >= DRM_VMIX_DEVICE_BUTT)) {
                return -5;
            }
            target_chn = &g_vmix_dev[s32DevId].VmixChns[0];
            target_mutex = &g_vmix_dev[s32DevId].VmixMtx;
            break;
        }

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -6;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -7;
    }

    init_media_channel_buffer(target_chn);

    target_mutex->lock();
    if (target_chn->media_out_cb_status == CHN_OUT_CB_USER) {
        target_mutex->unlock();
        return 0;
    } else if (target_chn->media_out_cb_status == CHN_OUT_CB_CLOSE) {
        DRM_MEDIA_LOGD("enable media output callback");
        target_chn->media_flow->SetOutputCallBack(target_chn, flow_output_callback);
    }
    target_chn->media_out_cb_status = CHN_OUT_CB_USER;
    target_mutex->unlock();

    return 0;
}

media_buffer_t drm_mpi_system_get_media_buffer(mod_id_e enModID, int s32ChnId, int s32MilliSec)
{
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VI:
            if ((s32ChnId < DRM_VO_CHANNEL_00) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vi channel:[%d]", s32ChnId);
                return NULL;
            }
            target_chn = &g_vi_chns[s32ChnId];
            break;

        case MOD_ID_VENC:
            if ((s32ChnId < DRM_VENC_CHANNEL_00) || (s32ChnId >= DRM_VENC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid venc channel:[%d]", s32ChnId);
                return NULL;
            }
            target_chn = &g_venc_chns[s32ChnId];
            break;

        case MOD_ID_VDEC:
            if ((s32ChnId < DRM_VDEC_CHANNEL_00) || (s32ChnId >= DRM_VDEC_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid vdec channel:[%d]", s32ChnId);
                return NULL;
            }
            target_chn = &g_vdec_chns[s32ChnId];
            break;

        case MOD_ID_RGA:
            if ((s32ChnId < DRM_RGA_CHANNEL_00) || (s32ChnId >= DRM_RGA_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid RGA channel:[%d]", s32ChnId);
                return NULL;
            }
            target_chn = &g_rga_chns[s32ChnId];
            break;

        case MOD_ID_VMIX: {
            int s32DevId = s32ChnId;
            if ((s32DevId < DRM_VMIX_DEVICE_00) || (s32DevId >= DRM_VMIX_DEVICE_BUTT)) {
                DRM_MEDIA_LOGE("invalid vmix device:[%d]", s32DevId);
                return NULL;
            }
            target_chn = &g_vmix_dev[s32DevId].VmixChns[0];
            break;
        }

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return NULL;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] in status:[%d], this operation is not allowed", ModIdToString(enModID), s32ChnId, target_chn->status);
        return NULL;
    }

    if (drm_mpi_system_start_get_media_buffer(enModID, s32ChnId)) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] start get media buffer failed", ModIdToString(enModID), s32ChnId);
        return NULL;
    }

    return media_channel_pop_buffer(target_chn, s32MilliSec);
}

int drm_mpi_system_register_output_callback(const drm_chn_t *pstChn, OutCallbackFunction callback)
{
    std::shared_ptr<libdrm::Flow> flow;
    drm_media_channel_t *target_chn = NULL;

    switch (pstChn->enModId) {
        case MOD_ID_VO:
            target_chn = &g_vo_chns[pstChn->s32ChnId];
            break;

        case MOD_ID_RGA:
            target_chn = &g_rga_chns[pstChn->s32ChnId];
            break;

        case MOD_ID_VENC:
            target_chn = &g_venc_chns[pstChn->s32ChnId];
            break;

        case MOD_ID_VDEC:
            target_chn = &g_vdec_chns[pstChn->s32ChnId];
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -1;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] not open", ModIdToString(target_chn->mode_id), target_chn->chn_id);
        return -2;
    }

    if (!target_chn->media_flow_list.empty()) {
        flow = target_chn->media_flow_list.back();
    } else if (target_chn->media_flow) {
        flow = target_chn->media_flow;
    }

    if (!flow) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] fatal error, status does not match the resource", ModIdToString(pstChn->enModId), pstChn->s32ChnId);
        return -3;
    }

    target_chn->out_cb = callback;
    return 0;
}

int drm_mpi_system_register_output_callbackEx(const drm_chn_t *pstChn, OutCallbackFunctionEx callback, void *handle)
{
    std::shared_ptr<libdrm::Flow> flow;
    drm_media_channel_t *target_chn = NULL;

    switch (pstChn->enModId) {
        case MOD_ID_VO:
            target_chn = &g_vo_chns[pstChn->s32ChnId];
            break;

        case MOD_ID_RGA:
            target_chn = &g_rga_chns[pstChn->s32ChnId];
            break;

        case MOD_ID_VENC:
            target_chn = &g_venc_chns[pstChn->s32ChnId];
            break;

        case MOD_ID_VDEC:
            target_chn = &g_vdec_chns[pstChn->s32ChnId];
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -1;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] not open", ModIdToString(target_chn->mode_id), target_chn->chn_id);
        return -2;
    }

    if (!target_chn->media_flow_list.empty()) {
        flow = target_chn->media_flow_list.back();
    } else if (target_chn->media_flow) {
        flow = target_chn->media_flow;
    }

    if (!flow) {
        DRM_MEDIA_LOGE("mode:[%s]-chn:[%d] fatal error, status does not match the resource", ModIdToString(pstChn->enModId), pstChn->s32ChnId);
        return -3;
    }

    target_chn->out_ex_cb = callback;
    target_chn->out_handle = handle;

    return 0;
}

int drm_mpi_system_register_event_callback(const drm_chn_t *pstChn, void *handle, EventCallbackFunction callback)
{
    std::shared_ptr<libdrm::Flow> flow;
    drm_media_channel_t *target_chn = NULL;

    switch (pstChn->enModId) {
        default:
            return -1;
    }

    target_chn->event_cb = callback;
    target_chn->event_handle = handle;
    flow->SetEventCallBack(target_chn, flow_event_callback);

    return 0;
}

int drm_mpi_vp_enable_channel(int channel)
{
    if ((channel < DRM_VP_CHANNEL_00) || (channel >= DRM_VP_CHANNEL_BUTT)) {
        return -1;
    }

    g_vp_mtx.lock();
    if (g_vp_chns[channel].status != CHN_STATUS_READY) {
        g_vp_mtx.unlock();
        return (g_vp_chns[channel].status > CHN_STATUS_READY) ? -2 : -3;
    }

    DRM_MEDIA_LOGI("Enable vp channel:[%d]-[%s], wxh:[%dx%d] starting......", channel, g_vp_chns[channel].vp_attr.pcVideoNode, g_vp_chns[channel].vp_attr.u32Width, g_vp_chns[channel].vp_attr.u32Height);

    std::string flow_param;
    std::string flow_name = "output_stream";

    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "v4l2_output_stream");

    std::string stream_param;
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_CAMERA_ID, 0);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_DEVICE, g_vp_chns[channel].vp_attr.pcVideoNode);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_CAP_TYPE, DRM_KEY_V4L2_C_TYPE(VIDEO_OUTPUT));

    bool bDbgFlag = false;
    if (bDbgFlag) {
        PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_USE_LIBV4L2, 0);
        PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_MEM_TYPE, DRM_KEY_V4L2_M_TYPE(MEMORY_DMABUF));
    } else {
#if defined(CONFIG_LIBV4L2)
        PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_USE_LIBV4L2, 1);
#else
        PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_USE_LIBV4L2, 0);
#endif
        if (g_vp_chns[channel].vp_attr.enBufType == VP_CHN_BUF_TYPE_MMAP) {
            PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_MEM_TYPE, DRM_KEY_V4L2_M_TYPE(MEMORY_MMAP));
        } else {
            PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_MEM_TYPE, DRM_KEY_V4L2_M_TYPE(MEMORY_DMABUF));
        }
    }

    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_FRAMES, g_vp_chns[channel].vp_attr.u32BufCnt);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_INPUTDATATYPE, ImageTypeToString(g_vp_chns[channel].vp_attr.enPixFmt));
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_BUFFER_WIDTH, g_vp_chns[channel].vp_attr.u32Width);
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_BUFFER_HEIGHT, g_vp_chns[channel].vp_attr.u32Height);

    flow_param = libdrm::JoinFlowParam(flow_param, 1, stream_param);
    DRM_MEDIA_LOGD("VP: v4l2 output flow param:[%s]", flow_param.c_str());

    char s8RetryCnt = 3;
    while (s8RetryCnt > 0) {
        g_vp_chns[channel].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
        if (g_vp_chns[channel].media_flow) {
            break;
        }

        DRM_MEDIA_LOGW("vp channe:[%d]-[%s] buffer may be occupied by other modules or apps, try again......", channel, g_vp_chns[channel].vp_attr.pcVideoNode);

        s8RetryCnt--;
        libdrm::msleep(50);
    }

    if (!g_vp_chns[channel].media_flow) {
        g_vp_mtx.unlock();
        return -4;
    }

    g_vp_chns[channel].status = CHN_STATUS_OPEN;
    g_vp_mtx.unlock();

    DRM_MEDIA_LOGI("Enable vp channel:[%d]-[%s], wxh:[%dx%d] finished......", channel, g_vp_chns[channel].vp_attr.pcVideoNode, g_vp_chns[channel].vp_attr.u32Width, g_vp_chns[channel].vp_attr.u32Height);

    return 0;
}

int drm_mpi_vp_disable_channel(int channel)
{
    if ((channel < DRM_VP_CHANNEL_00) || (channel >= DRM_VP_CHANNEL_BUTT)) {
        return -1;
    }

    g_vp_mtx.lock();
    if (g_vp_chns[channel].status == CHN_STATUS_BIND) {
        g_vp_mtx.unlock();
        return -2;
    }

    DRM_MEDIA_LOGI("Disable vo channel:[%d]-[%s], wxh:[%dx%d] starting......", channel, g_vp_chns[channel].vp_attr.pcVideoNode, g_vp_chns[channel].vp_attr.u32Width, g_vp_chns[channel].vp_attr.u32Height);

    g_vp_chns[channel].status = CHN_STATUS_CLOSED;
    g_vp_chns[channel].media_flow.reset();
    g_vp_mtx.unlock();

    DRM_MEDIA_LOGI("Disable vo channel:[%d]-[%s], wxh:[%dx%d] finished......", channel, g_vp_chns[channel].vp_attr.pcVideoNode, g_vp_chns[channel].vp_attr.u32Width, g_vp_chns[channel].vp_attr.u32Height);

    return 0;
}

int drm_mpi_vp_set_channel_attribute(int channel, const drm_vp_chn_attr_t *pstChnAttr)
{
    if ((channel < DRM_VP_CHANNEL_00) || (channel >= DRM_VP_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstChnAttr || !pstChnAttr->pcVideoNode) {
        return -2;
    }

    g_vp_mtx.lock();
    if (g_vp_chns[channel].status != CHN_STATUS_CLOSED) {
        g_vp_mtx.unlock();
        return -3;
    }

    memcpy(&g_vp_chns[channel].vp_attr, pstChnAttr, sizeof(drm_vp_chn_attr_t));
    g_vp_chns[channel].status = CHN_STATUS_READY;
    g_vp_mtx.unlock();

    return 0;
}

int drm_mpi_vi_get_channel_attribute(int channel, drm_vi_chn_attr_t *pstChnAttr)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstChnAttr) {
        return -2;
    }

    int ret = 0;
    g_vi_mtx.lock();
    do {
        if (g_vi_chns[channel].status == CHN_STATUS_CLOSED) {
            ret = -3;
            break;
        }

        memcpy(pstChnAttr, &g_vi_chns[channel].vi_attr, sizeof(drm_vi_chn_attr_t));
    } while (0);
    g_vi_mtx.unlock();

    return ret;
}

int drm_mpi_vi_set_channel_attribute(int channel, const drm_vi_chn_attr_t *pstChnAttr)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstChnAttr || !pstChnAttr->pcVideoNode) {
        return -2;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status != CHN_STATUS_CLOSED) {
        g_vi_mtx.unlock();
        return -3;
    }

    memcpy(&g_vi_chns[channel].vi_attr, pstChnAttr, sizeof(drm_vi_chn_attr_t));
    g_vi_chns[channel].status = CHN_STATUS_READY;
    g_vi_mtx.unlock();

    return 0;
}

int drm_mpi_vi_enable_channel(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status != CHN_STATUS_READY) {
        g_vi_mtx.unlock();
        return (g_vi_chns[channel].status > CHN_STATUS_READY) ? -2 : -3;
    }

    DRM_MEDIA_LOGI("Enable vi channel:[%d]:[%s], wxh:[%dx%d] starting......", channel, g_vi_chns[channel].vi_attr.pcVideoNode, g_vi_chns[channel].vi_attr.u32Width, g_vi_chns[channel].vi_attr.u32Height);

    std::string flow_name = "source_stream";
    std::string flow_param;
    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "v4l2_capture_stream");

    std::string stream_param;
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_CAMERA_ID, 0);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_DEVICE, g_vi_chns[channel].vi_attr.pcVideoNode);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_CAP_TYPE, DRM_KEY_V4L2_C_TYPE(VIDEO_CAPTURE));

#if defined(CONFIG_LIBV4L2)
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_USE_LIBV4L2, 1);
#else
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_USE_LIBV4L2, 0);
#endif
    if (g_vi_chns[channel].vi_attr.enBufType == DRM_VI_CHN_BUF_TYPE_MMAP) {
        PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_MEM_TYPE, DRM_KEY_V4L2_M_TYPE(MEMORY_MMAP));
    } else {
        PARAM_STRING_APPEND(stream_param, DRM_KEY_V4L2_MEM_TYPE, DRM_KEY_V4L2_M_TYPE(MEMORY_DMABUF));
    }

    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_FRAMES, g_vi_chns[channel].vi_attr.u32BufCnt);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_OUTPUTDATATYPE, ImageTypeToString(g_vi_chns[channel].vi_attr.enPixFmt));
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_BUFFER_WIDTH, g_vi_chns[channel].vi_attr.u32Width);
    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_BUFFER_HEIGHT, g_vi_chns[channel].vi_attr.u32Height);

    flow_param = libdrm::JoinFlowParam(flow_param, 1, stream_param);
    DRM_MEDIA_LOGD("v4l2 source flow param:[%s]", flow_param.c_str());

    char s8RetryCnt = 3;
    while (s8RetryCnt > 0) {
        g_vi_chns[channel].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
        if (g_vi_chns[channel].media_flow) {
            break;
        }

        DRM_MEDIA_LOGW("vi channel:[%d]:[%s] buffer may be occupied by other modules or apps, try again......", channel, g_vi_chns[channel].vi_attr.pcVideoNode);
        s8RetryCnt--;
        libdrm::msleep(50);
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -2;
    }

    g_vi_chns[channel].luma_buf_mtx.lock();
    g_vi_chns[channel].luma_buf_quit = false;
    g_vi_chns[channel].luma_buf_start = false;
    g_vi_chns[channel].luma_buf_mtx.unlock();

    init_media_channel_buffer(&g_vi_chns[channel]);
    g_vi_chns[channel].media_flow->SetOutputCallBack(&g_vi_chns[channel], flow_output_callback);
    g_vi_chns[channel].status = CHN_STATUS_OPEN;

    g_vi_mtx.unlock();
    DRM_MEDIA_LOGI("Enable vi channel:[%d]:[%s], wxh:[%dx%d] finished......", channel, g_vi_chns[channel].vi_attr.pcVideoNode, g_vi_chns[channel].vi_attr.u32Width, g_vi_chns[channel].vi_attr.u32Height);

    return 0;
}

int drm_mpi_vi_disable_channel(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status == CHN_STATUS_BIND) {
        g_vi_mtx.unlock();
        return -2;
    }

    DRM_MEDIA_LOGI("Disable vi channel:[%d]:[%s], wxh:[%dx%d] starting......", channel, g_vi_chns[channel].vi_attr.pcVideoNode, g_vi_chns[channel].vi_attr.u32Width, g_vi_chns[channel].vi_attr.u32Height);

    clear_media_channel_buffer(&g_vi_chns[channel]);
    g_vi_chns[channel].status = CHN_STATUS_CLOSED;
    g_vi_chns[channel].luma_buf_mtx.lock();
    g_vi_chns[channel].luma_media_buf.reset();
    g_vi_chns[channel].luma_buf_cond.notify_all();
    g_vi_chns[channel].luma_buf_quit = true;
    g_vi_chns[channel].luma_buf_mtx.unlock();

    g_vi_chns[channel].media_flow.reset();
    if (!g_vi_chns[channel].buffer_list.empty()) {
        DRM_MEDIA_LOGI("clear buffer list again......");
        clear_media_channel_buffer(&g_vi_chns[channel]);
    }
    g_vi_mtx.unlock();

    DRM_MEDIA_LOGI("Disable vi channel:[%d]:[%s], wxh:[%dx%d] finished......", channel, g_vi_chns[channel].vi_attr.pcVideoNode, g_vi_chns[channel].vi_attr.u32Width, g_vi_chns[channel].vi_attr.u32Height);

    return 0;
}

int drm_mpi_vi_get_status(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    g_vi_mtx.lock();
    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -2;
    }

    struct sysinfo info;
    int64_t recent_time, current_time;

    sysinfo(&info);
    current_time = info.uptime * 1000000LL;
    g_vi_chns[channel].media_flow->Control(libdrm::G_STREAM_RECENT_TIME, &recent_time);
    assert(current_time - recent_time > 0);

    if ((recent_time != 0) && ((current_time - recent_time) > 3000000)) {
        DRM_MEDIA_LOGW("video input timeout more than 3 seconds");
        g_vi_mtx.unlock();
        return 3;
    }
    g_vi_mtx.unlock();

    return 0;
}

int drm_mpi_vi_start_stream(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        g_vi_mtx.unlock();
        return -2;
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -3;
    }

    g_vi_chns[channel].media_flow->StartStream();
    g_vi_mtx.unlock();

    return 0;
}

int drm_mpi_vi_enable_user_picture(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        g_vi_mtx.unlock();
        return -2;
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -3;
    }

    int ret = g_vi_chns[channel].media_flow->Control(libdrm::S_ENABLE_USER_PICTURE);
    g_vi_mtx.unlock();

    return ret ? -4 : 0;
}

int drm_mpi_vi_disable_user_picture(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        g_vi_mtx.unlock();
        return -2;
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -3;
    }

    int ret = g_vi_chns[channel].media_flow->Control(libdrm::S_DISABLE_USER_PICTURE);
    g_vi_mtx.unlock();

    return ret ? -4 : 0;
}

int drm_mpi_vi_set_user_picture(int channel, drm_vi_userpic_attr_t *pstUsrPicAttr)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstUsrPicAttr) {
        return -2;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        g_vi_mtx.unlock();
        return -3;
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -4;
    }

    DrmPixelFormat src_pixfmt = StringToPixFmt(ImageTypeToString(pstUsrPicAttr->enPixFmt).c_str());
    DrmPixelFormat trg_pixfmt = StringToPixFmt(ImageTypeToString(g_vi_chns[channel].vi_attr.enPixFmt).c_str());
    uint32_t u32PicSize = CalPixFmtSize(trg_pixfmt, g_vi_chns[channel].vi_attr.u32Width, g_vi_chns[channel].vi_attr.u32Height);

    void *pvPicPtr = malloc(u32PicSize);
    if (pvPicPtr == NULL) {
        g_vi_mtx.unlock();
        return -5;
    }

#if defined(CONFIG_RKRGA)
    rga_buffer_t src_info, dst_info;
    memset(&src_info, 0, sizeof(rga_buffer_t));
    memset(&dst_info, 0, sizeof(rga_buffer_t));
    src_info = wrapbuffer_virtualaddr(pstUsrPicAttr->pvPicPtr, pstUsrPicAttr->u32Width, pstUsrPicAttr->u32Height, get_rga_format(src_pixfmt));
    dst_info = wrapbuffer_virtualaddr(pvPicPtr, g_vi_chns[channel].vi_attr.u32Width, g_vi_chns[channel].vi_attr.u32Height, get_rga_format(trg_pixfmt));
    imresize(src_info, dst_info);
#else
    if ((pstUsrPicAttr->u32Width != g_vi_chns[channel].vi_attr.u32Width)
        || (pstUsrPicAttr->u32Height != g_vi_chns[channel].vi_attr.u32Height)
        || (src_pixfmt != trg_pixfmt))
    {
        DRM_MEDIA_LOGE("picWidth:[%u], viWidth:[%u], picHeight:[%u], viHeight:[%u], picFmt:[%d], viFmt:[%d]",
            pstUsrPicAttr->u32Width, g_vi_chns[channel].vi_attr.u32Width,
            pstUsrPicAttr->u32Height, g_vi_chns[channel].vi_attr.u32Height,
            src_pixfmt, trg_pixfmt);
        return -6;
    }
#endif

    libdrm::UserPicArg media_user_pic_arg;
    media_user_pic_arg.size = u32PicSize;
    media_user_pic_arg.data = pvPicPtr;
    int ret = g_vi_chns[channel].media_flow->Control(libdrm::S_INSERT_USER_PICTURE, &media_user_pic_arg);

    free(pvPicPtr);
    g_vi_mtx.unlock();

    return ret ? -7 : 0;
}

static uint64_t calculate_media_region_luma(std::shared_ptr<libdrm::ImageBuffer> &media_mb, const drm_rect_t *ptrRect)
{
    uint64_t sum = 0;
    DrmImageInfo &imgInfo = media_mb->GetImageInfo();

    if ((imgInfo.pix_fmt != DRM_PIX_FMT_YUV420P)
        && (imgInfo.pix_fmt != DRM_PIX_FMT_NV12)
        && (imgInfo.pix_fmt != DRM_PIX_FMT_NV21)
        && (imgInfo.pix_fmt != DRM_PIX_FMT_YUV422P)
        && (imgInfo.pix_fmt != DRM_PIX_FMT_NV16)
        && (imgInfo.pix_fmt != DRM_PIX_FMT_NV61)
        && (imgInfo.pix_fmt != DRM_PIX_FMT_YUYV422))
    {
        DRM_MEDIA_LOGE("not support image type");
        return 0;
    }

    if (((int)(ptrRect->s32X + ptrRect->u32Width) > imgInfo.width) || ((int)(ptrRect->s32Y + ptrRect->u32Height) > imgInfo.height)) {
        DRM_MEDIA_LOGE("rect:[%d,%d,%u,%u] out of image wxh:[%d, %d]", ptrRect->s32X, ptrRect->s32Y, ptrRect->u32Width, ptrRect->u32Height, imgInfo.width, imgInfo.height);
        return 0;
    }

    if (imgInfo.pix_fmt == DRM_PIX_FMT_YUYV422) {
        uint32_t line_size = imgInfo.vir_width * 2;
        uint8_t *rect_start = (uint8_t *)media_mb->GetPtr() + ptrRect->s32Y * line_size + ptrRect->s32X * 2;

        for (uint32_t i = 0; i < ptrRect->u32Height; i++) {
            uint8_t *line_start = rect_start + i * line_size;
            for (uint32_t j = 0; j < ptrRect->u32Width; j++) {
                sum += *(line_start + j * 2);
            }
        }
    } else {
        uint32_t line_size = imgInfo.vir_width;
        uint8_t *rect_start = (uint8_t *)media_mb->GetPtr() + ptrRect->s32Y * line_size + ptrRect->s32X;

        for (uint32_t i = 0; i < ptrRect->u32Height; i++) {
            uint8_t *line_start = rect_start + i * line_size;
            for (uint32_t j = 0; j < ptrRect->u32Width; j++) {
                sum += *(line_start + j);
            }
        }
    }

    return sum;
}

int drm_mpi_vi_stop_region_luma(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_vi_chns[channel].luma_buf_mtx.lock();
    g_vi_chns[channel].luma_buf_start = false;
    g_vi_chns[channel].luma_media_buf.reset();
    g_vi_chns[channel].luma_buf_mtx.unlock();

    g_vi_mtx.lock();
    if (g_vi_chns[channel].media_out_cb_status == CHN_OUT_CB_LUMA) {
        DRM_MEDIA_LOGD("luma mode: disable media out callback");
        g_vi_chns[channel].media_out_cb_status = CHN_OUT_CB_CLOSE;
        g_vi_chns[channel].media_flow->SetOutputCallBack(&g_vi_chns[channel], flow_output_callback);
    }
    g_vi_mtx.unlock();

    return 0;
}

int drm_mpi_vi_start_region_luma(int channel)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_vi_chns[channel].luma_buf_mtx.lock();
    if (g_vi_chns[channel].luma_buf_start) {
        g_vi_chns[channel].luma_buf_mtx.unlock();
        return 0;
    }

    g_vi_chns[channel].luma_buf_start = true;
    g_vi_chns[channel].luma_buf_mtx.unlock();

    g_vi_mtx.lock();
    if (g_vi_chns[channel].media_out_cb_status == CHN_OUT_CB_CLOSE) {
        DRM_MEDIA_LOGD("luma mode: enable media out callback");
        g_vi_chns[channel].media_out_cb_status = CHN_OUT_CB_LUMA;
        g_vi_chns[channel].media_flow->SetOutputCallBack(&g_vi_chns[channel], flow_output_callback);
    }
    g_vi_mtx.unlock();

    return 0;
}

int drm_mpi_vi_get_channel_region_luma(int channel, const drm_video_region_info_t *pstRegionInfo, uint64_t *pu64LumaData, int s32MilliSec)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRegionInfo || !pstRegionInfo->u32RegionNum || !pu64LumaData) {
        return -2;
    }

    int s32Ret = 0;
    uint32_t u32XOffset = 0;
    uint32_t u32YOffset = 0;
    uint32_t u32ImgWidth = 0;
    uint32_t u32ImgHeight = 0;
    std::shared_ptr<libdrm::ImageBuffer> media_mb;
    drm_media_channel_t *target_chn = &g_vi_chns[channel];

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -3;
    }

    u32ImgWidth = g_vi_chns[channel].vi_attr.u32Width;
    u32ImgHeight = g_vi_chns[channel].vi_attr.u32Height;

    for (uint32_t i = 0; i < pstRegionInfo->u32RegionNum; i++) {
        u32XOffset = pstRegionInfo->pstRegion[i].s32X + pstRegionInfo->pstRegion[i].u32Width;
        u32YOffset = pstRegionInfo->pstRegion[i].s32Y + pstRegionInfo->pstRegion[i].u32Height;

        if ((u32XOffset > u32ImgWidth) || (u32YOffset > u32ImgHeight)) {
            DRM_MEDIA_LOGE("lumaRgn:[%d]:<%d, %d, %d, %d> is invalid for vi channel:[%d], wxh:[%dx%d]",
                i, pstRegionInfo->pstRegion[i].s32X, pstRegionInfo->pstRegion[i].s32Y,
                pstRegionInfo->pstRegion[i].u32Width, pstRegionInfo->pstRegion[i].u32Height, channel, u32ImgWidth, u32ImgHeight);
            return -4;
        }
    }

    s32Ret = drm_mpi_vi_start_region_luma(channel);
    if (s32Ret) {
        return s32Ret;
    }

    {
        std::unique_lock<std::mutex> lck(target_chn->luma_buf_mtx);

        if (!target_chn->luma_media_buf) {
            if ((s32MilliSec < 0) && !target_chn->luma_buf_quit) {
                target_chn->luma_buf_cond.wait(lck);
            } else if (s32MilliSec > 0) {
                if (target_chn->luma_buf_cond.wait_for(lck, std::chrono::milliseconds(s32MilliSec)) == std::cv_status::timeout) {
                    return -5;
                }
            } else {
                return -6;
            }
        }

        if (target_chn->luma_media_buf) {
            media_mb = std::static_pointer_cast<libdrm::ImageBuffer>(target_chn->luma_media_buf);
        }

        target_chn->luma_media_buf.reset();
    }

    if (!media_mb) {
        return -7;
    }

    for (uint32_t i = 0; i < pstRegionInfo->u32RegionNum; i++) {
        *(pu64LumaData + i) = calculate_media_region_luma(media_mb, (pstRegionInfo->pstRegion + i));
    }

    return 0;
}

int drm_mpi_vi_region_set_cover(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_cover_info_t *pstCoverInfo)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT))
        return -1;

    if (!pstRgnInfo) {
        return -2;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        g_vi_mtx.unlock();
        return -3;
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -4;
    }

    DrmImageBorder line;
    memset(&line, 0x00, sizeof(DrmImageBorder));
    line.id = pstRgnInfo->enRegionId;
    line.enable = pstRgnInfo->u8Enable;
    line.x = pstRgnInfo->u32PosX;
    line.y = pstRgnInfo->u32PosY;
    line.w = pstRgnInfo->u32Width;
    line.h = pstRgnInfo->u32Height;

    if (pstCoverInfo) {
        line.color = pstCoverInfo->u32Color;
    }

    int ret = g_vi_chns[channel].media_flow->Control(libdrm::S_RGA_LINE_INFO, &line);
    g_vi_mtx.unlock();

    return ret ? -5 : 0;
}

static DrmPixelFormat get_osd_pix_format(drm_osd_pixel_format_e f)
{
    static std::map<drm_osd_pixel_format_e, DrmPixelFormat> format_map = {
        {PIXEL_FORMAT_ARGB_8888, DRM_PIX_FMT_ARGB8888}
    };

    auto it = format_map.find(f);
    if (it != format_map.end()) {
        return it->second;
    }

    return DRM_PIX_FMT_NONE;
}

int drm_mpi_vi_region_set_bitmap(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_bitmap_t *pstBitmap)
{
    if ((channel < DRM_VI_CHANNEL_00) || (channel >= DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRgnInfo) {
        return -2;
    }

    g_vi_mtx.lock();
    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        g_vi_mtx.unlock();
        return -3;
    }

    if (!g_vi_chns[channel].media_flow) {
        g_vi_mtx.unlock();
        return -4;
    }

    DrmImageOsd osd;
    memset(&osd, 0x00, sizeof(osd));
    osd.id = pstRgnInfo->enRegionId;
    osd.enable = pstRgnInfo->u8Enable;
    osd.x = pstRgnInfo->u32PosX;
    osd.y = pstRgnInfo->u32PosY;
    osd.w = pstRgnInfo->u32Width;
    osd.h = pstRgnInfo->u32Height;
    osd.data = pstBitmap->pData;
    osd.pix_fmt = get_osd_pix_format(pstBitmap->enPixelFormat);

    int ret = g_vi_chns[channel].media_flow->Control(libdrm::S_RGA_OSD_INFO, &osd);
    g_vi_mtx.unlock();

    return ret ? -5 : 0;
}

int drm_mpi_vo_create_channel(int channel, const drm_vo_chn_attr_t *pstAttr)
{
    const char *pcPlaneType = NULL;

    if (((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT)) || !pstAttr) {
        DRM_MEDIA_LOGE("invalid channel id:[%d] or channel attribute null", channel);
        return -1;
    }

    switch (pstAttr->emPlaneType) {
        case VO_PLANE_PRIMARY:
            pcPlaneType = "Primary";
            break;

        case VO_PLANE_OVERLAY:
            pcPlaneType = "Overlay";
            break;

        case VO_PLANE_CURSOR:
            pcPlaneType = "Cursor";
            break;

        default:
            break;
    }

    if (!pcPlaneType) {
        DRM_MEDIA_LOGE("plane type null");
        return -2;
    }

    g_vo_mtx.lock();
    if (g_vo_chns[channel].status != CHN_STATUS_CLOSED) {
        g_vo_mtx.unlock();
        DRM_MEDIA_LOGE("channel not closed");
        return -3;
    }

    DRM_MEDIA_LOGI("Enable vo channel:[%d] starting......", channel);

    std::string flow_name = "output_stream";
    std::string flow_param = "";
    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "drm_output_stream");

    std::string stream_param = "";
    if (!pstAttr->pcDevNode) {
        DRM_MEDIA_LOGI("use default DevNode:/dev/dri/card0");
        PARAM_STRING_APPEND(stream_param, DRM_KEY_DEVICE, "/dev/dri/card0");
    } else {
        PARAM_STRING_APPEND(stream_param, DRM_KEY_DEVICE, pstAttr->pcDevNode);
    }

    if (pstAttr->u16ConIdx) {
        PARAM_STRING_APPEND_TO(stream_param, "connector_id", pstAttr->u16ConIdx);
    }

    if (pstAttr->u16EncIdx) {
        PARAM_STRING_APPEND_TO(stream_param, "encoder_id", pstAttr->u16EncIdx);
    }

    if (pstAttr->u16CrtcIdx) {
        PARAM_STRING_APPEND_TO(stream_param, "crtc_id", pstAttr->u16CrtcIdx);
    }

    if (pstAttr->u32Width) {
        PARAM_STRING_APPEND_TO(stream_param, "width", pstAttr->u32Width);
    }

    if (pstAttr->u32Height) {
        PARAM_STRING_APPEND_TO(stream_param, "height", pstAttr->u32Height);
    }

    PARAM_STRING_APPEND_TO(stream_param, "framerate", pstAttr->u16Fps);
    PARAM_STRING_APPEND(stream_param, "plane_type", pcPlaneType);
    PARAM_STRING_APPEND_TO(stream_param, "ZPOS", pstAttr->u16Zpos);
    PARAM_STRING_APPEND(stream_param, DRM_KEY_OUTPUTDATATYPE, ImageTypeToString(pstAttr->enImgType));

    flow_param = libdrm::JoinFlowParam(flow_param, 1, stream_param);
    DRM_MEDIA_LOGD("DrmDisplay flow params:[%s]", flow_param.c_str());

    g_vo_chns[channel].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!g_vo_chns[channel].media_flow) {
        g_vo_mtx.unlock();
        DRM_MEDIA_LOGE("create media flow failed");
        return -4;
    }

    if (pstAttr->stDispRect.s32X || pstAttr->stDispRect.s32Y || pstAttr->stDispRect.u32Width || pstAttr->stDispRect.u32Height) {
        DrmImageRect PlaneRect = {
            pstAttr->stDispRect.s32X,
            pstAttr->stDispRect.s32Y,
            (int)pstAttr->stDispRect.u32Width,
            (int)pstAttr->stDispRect.u32Height
        };

        if (g_vo_chns[channel].media_flow->Control(libdrm::S_DESTINATION_RECT, &PlaneRect)) {
            g_vo_chns[channel].media_flow.reset();
            g_vo_mtx.unlock();
            DRM_MEDIA_LOGE("set dest rect failed");
            return -5;
        }
    }

    if (pstAttr->stImgRect.s32X || pstAttr->stImgRect.s32Y || pstAttr->stImgRect.u32Width || pstAttr->stImgRect.u32Height) {
        DrmImageRect ImgRect = {
            pstAttr->stImgRect.s32X,
            pstAttr->stImgRect.s32Y,
            (int)pstAttr->stImgRect.u32Width,
            (int)pstAttr->stImgRect.u32Height
        };

        if (g_vo_chns[channel].media_flow->Control(libdrm::S_SOURCE_RECT, &ImgRect)) {
            g_vo_chns[channel].media_flow.reset();
            g_vo_mtx.unlock();
            DRM_MEDIA_LOGE("set source rect failed");
            return -6;
        }
    }

    memcpy(&g_vo_chns[channel].vo_attr, pstAttr, sizeof(drm_vo_chn_attr_t));

    DrmImageInfo ImageInfoFinal;
    memset(&ImageInfoFinal, 0x00, sizeof(ImageInfoFinal));

    g_vo_chns[channel].media_flow->Control(libdrm::G_PLANE_IMAGE_INFO, &ImageInfoFinal);
    g_vo_chns[channel].vo_attr.u32Width = ImageInfoFinal.width;
    g_vo_chns[channel].vo_attr.u32Height = ImageInfoFinal.height;

    DrmImageRect ImageRectTotal[2];
    memset(&ImageRectTotal, 0x00, 2 * sizeof(DrmImageRect));

    g_vo_chns[channel].media_flow->Control(libdrm::G_SRC_DST_RECT, &ImageRectTotal);
    g_vo_chns[channel].vo_attr.stImgRect.s32X = ImageRectTotal[0].x;
    g_vo_chns[channel].vo_attr.stImgRect.s32Y = ImageRectTotal[0].y;
    g_vo_chns[channel].vo_attr.stImgRect.u32Width = (uint32_t)ImageRectTotal[0].w;
    g_vo_chns[channel].vo_attr.stImgRect.u32Height = (uint32_t)ImageRectTotal[0].h;
    g_vo_chns[channel].vo_attr.stDispRect.s32X = ImageRectTotal[1].x;
    g_vo_chns[channel].vo_attr.stDispRect.s32Y = ImageRectTotal[1].y;
    g_vo_chns[channel].vo_attr.stDispRect.u32Width = (uint32_t)ImageRectTotal[1].w;
    g_vo_chns[channel].vo_attr.stDispRect.u32Height = (uint32_t)ImageRectTotal[1].h;

    g_vo_chns[channel].status = CHN_STATUS_OPEN;
    g_vo_mtx.unlock();
    DRM_MEDIA_LOGI("Enable vo channel:[%d] finished......", channel);

    return 0;
}

int drm_mpi_vo_destroy_channel(int channel)
{
    if ((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT)) {
        return -1;
    }

    g_vo_mtx.lock();
    if (g_vo_chns[channel].status == CHN_STATUS_BIND) {
        g_vo_mtx.unlock();
        return -2;
    }

    g_vo_chns[channel].media_flow.reset();
    g_vo_chns[channel].status = CHN_STATUS_CLOSED;
    g_vo_mtx.unlock();

    return 0;
}

int drm_mpi_vo_get_channel_attribute(int channel, drm_vo_chn_attr_t *pstAttr)
{
    if ((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT))
        return -1;

    if (!pstAttr) {
        return -2;
    }

    g_vo_mtx.lock();
    if (g_vo_chns[channel].status < CHN_STATUS_OPEN) {
        g_vo_mtx.unlock();
        return -3;
    }

    memcpy(pstAttr, &g_vo_chns[channel].vo_attr, sizeof(drm_vo_chn_attr_t));
    g_vo_mtx.unlock();

    return 0;
}

int drm_mpi_vo_set_channel_attribute(int channel, const drm_vo_chn_attr_t *pstAttr)
{
    if ((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstAttr) {
        return -2;
    }

    g_vo_mtx.lock();
    if (g_vo_chns[channel].status < CHN_STATUS_OPEN) {
        g_vo_mtx.unlock();
        return -3;
    }

    if (!g_vo_chns[channel].media_flow) {
        g_vo_mtx.unlock();
        return -4;
    }

    DrmImageRect ImageRectTotal[2];
    ImageRectTotal[0].x = pstAttr->stImgRect.s32X;
    ImageRectTotal[0].y = pstAttr->stImgRect.s32Y;
    ImageRectTotal[0].w = (int)pstAttr->stImgRect.u32Width;
    ImageRectTotal[0].h = (int)pstAttr->stImgRect.u32Height;
    ImageRectTotal[1].x = pstAttr->stDispRect.s32X;
    ImageRectTotal[1].y = pstAttr->stDispRect.s32Y;
    ImageRectTotal[1].w = (int)pstAttr->stDispRect.u32Width;
    ImageRectTotal[1].h = (int)pstAttr->stDispRect.u32Height;

    int ret = g_vo_chns[channel].media_flow->Control(libdrm::S_SRC_DST_RECT, &ImageRectTotal);
    if (ret) {
        g_vo_mtx.unlock();
        return -5;
    }

    g_vo_chns[channel].vo_attr.stImgRect.s32X = ImageRectTotal[0].x;
    g_vo_chns[channel].vo_attr.stImgRect.s32Y = ImageRectTotal[0].y;
    g_vo_chns[channel].vo_attr.stImgRect.u32Width = (uint32_t)ImageRectTotal[0].w;
    g_vo_chns[channel].vo_attr.stImgRect.u32Height = (uint32_t)ImageRectTotal[0].h;
    g_vo_chns[channel].vo_attr.stDispRect.s32X = ImageRectTotal[1].x;
    g_vo_chns[channel].vo_attr.stDispRect.s32Y = ImageRectTotal[1].y;
    g_vo_chns[channel].vo_attr.stDispRect.u32Width = (uint32_t)ImageRectTotal[1].w;
    g_vo_chns[channel].vo_attr.stDispRect.u32Height = (uint32_t)ImageRectTotal[1].h;
    g_vo_mtx.unlock();

    return 0;
}

int drm_mpi_rga_destroy_channel(int channel)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status == CHN_STATUS_BIND) {
        g_rga_mtx.unlock();
        return -2;
    }

    DRM_MEDIA_LOGI("Disable RGA channel:[%d] starting......", channel);

    clear_media_channel_buffer(&g_rga_chns[channel]);
    g_rga_chns[channel].media_flow.reset();
    g_rga_chns[channel].status = CHN_STATUS_CLOSED;
    g_rga_mtx.unlock();

    DRM_MEDIA_LOGI("Disable RGA channel:[%d] finished......", channel);

    return 0;
}

int drm_mpi_rga_create_channel(int channel, drm_rga_attr_t *pstRgaAttr)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRgaAttr) {
        return -2;
    }

    DRM_MEDIA_LOGI("Enable RGA channel:[%d], rect:[%d,%d,%d,%d] starting......", channel, pstRgaAttr->stImgIn.u32X, pstRgaAttr->stImgIn.u32Y, pstRgaAttr->stImgIn.u32Width, pstRgaAttr->stImgIn.u32Height);

    uint32_t u32InX = pstRgaAttr->stImgIn.u32X;
    uint32_t u32InY = pstRgaAttr->stImgIn.u32Y;
    uint32_t u32InWidth = pstRgaAttr->stImgIn.u32Width;
    uint32_t u32InHeight = pstRgaAttr->stImgIn.u32Height;
    // uint32_t u32InVirWidth = pstRgaAttr->stImgIn.u32HorStride;
    // uint32_t u32InVirHeight = pstRgaAttr->stImgIn.u32VirStride;
    std::string InPixelFmt = ImageTypeToString(pstRgaAttr->stImgIn.imgType);

    uint32_t u32OutX = pstRgaAttr->stImgOut.u32X;
    uint32_t u32OutY = pstRgaAttr->stImgOut.u32Y;
    uint32_t u32OutWidth = pstRgaAttr->stImgOut.u32Width;
    uint32_t u32OutHeight = pstRgaAttr->stImgOut.u32Height;
    uint32_t u32OutVirWidth = pstRgaAttr->stImgOut.u32HorStride;
    uint32_t u32OutVirHeight = pstRgaAttr->stImgOut.u32VirStride;
    std::string OutPixelFmt = ImageTypeToString(pstRgaAttr->stImgOut.imgType);

    bool bEnableBp = pstRgaAttr->bEnBufPool;
    uint16_t u16Rotaion = pstRgaAttr->u16Rotaion;
    uint16_t u16BufPoolCnt = pstRgaAttr->u16BufPoolCnt;

    if ((u16Rotaion != 0) && (u16Rotaion != 90) && (u16Rotaion != 180) && (u16Rotaion != 270)) {
        DRM_MEDIA_LOGE("rga rotation only support: 0/90/180/270");
        return -3;
    }

    drm_rga_flip_e enFlip = pstRgaAttr->enFlip;
    if ((enFlip != RGA_FLIP_H) && (enFlip != RGA_FLIP_V) && (enFlip != RGA_FLIP_HV)) {
        enFlip = RGA_FLIP_NULL;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status != CHN_STATUS_CLOSED) {
        g_rga_mtx.unlock();
        return -4;
    }

    std::string flow_name = "filter";
    std::string flow_param = "";
    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkrga");
    PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, InPixelFmt);
    PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, OutPixelFmt);
    PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_WIDTH, u32OutWidth);
    PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_HEIGHT, u32OutHeight);
    PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_VIR_WIDTH, u32OutVirWidth);
    PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_VIR_HEIGHT, u32OutVirHeight);

    if (bEnableBp) {
        PARAM_STRING_APPEND(flow_param, DRM_KEY_MEM_TYPE, DRM_KEY_MEM_HARDWARE);
        PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_MEM_CNT, u16BufPoolCnt);
    }

    std::string filter_param = "";
    DrmImageRect src_rect = {(int)u32InX, (int)u32InY, (int)u32InWidth, (int)u32InHeight};
    DrmImageRect dst_rect = {(int)u32OutX, (int)u32OutY, (int)u32OutWidth, (int)u32OutHeight};

    std::vector<DrmImageRect> rect_vect;
    rect_vect.push_back(src_rect);
    rect_vect.push_back(dst_rect);

    PARAM_STRING_APPEND(filter_param, DRM_KEY_BUFFER_RECT, libdrm::TwoImageRectToString(rect_vect).c_str());
    PARAM_STRING_APPEND_TO(filter_param, DRM_KEY_BUFFER_ROTATE, u16Rotaion);
    PARAM_STRING_APPEND_TO(filter_param, DRM_KEY_BUFFER_FLIP, enFlip);

    flow_param = libdrm::JoinFlowParam(flow_param, 1, filter_param);
    DRM_MEDIA_LOGD("rkrga filter flow param:[%s]", flow_param.c_str());

    g_rga_chns[channel].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!g_rga_chns[channel].media_flow) {
        g_rga_mtx.unlock();
        return -5;
    }

    g_rga_chns[channel].media_flow->SetOutputCallBack(&g_rga_chns[channel], flow_output_callback);
    init_media_channel_buffer(&g_rga_chns[channel]);
    g_rga_chns[channel].status = CHN_STATUS_OPEN;
    g_rga_mtx.unlock();

    DRM_MEDIA_LOGI("Enable RGA channel:[%d], rect:[%d,%d,%d,%d] finished......", channel, pstRgaAttr->stImgIn.u32X, pstRgaAttr->stImgIn.u32Y, pstRgaAttr->stImgIn.u32Width, pstRgaAttr->stImgIn.u32Height);
    return 0;
}

int drm_mpi_rga_get_channel_attribute(int channel, drm_rga_attr_t *pstAttr)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstAttr) {
        return -2;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status < CHN_STATUS_OPEN) {
        g_rga_mtx.unlock();
        return -3;
    }

    if (!g_rga_chns[channel].media_flow) {
        g_rga_mtx.unlock();
        return -4;
    }

    DrmRgaConfig mediaRgaCfg;
    int ret = g_rga_chns[channel].media_flow->Control(libdrm::G_RGA_CFG, &mediaRgaCfg);
    if (ret) {
        g_rga_mtx.unlock();
        return -5;
    }

    g_rga_mtx.unlock();
    pstAttr->stImgIn.u32X = (uint32_t)mediaRgaCfg.src_rect.x;
    pstAttr->stImgIn.u32Y = (uint32_t)mediaRgaCfg.src_rect.y;
    pstAttr->stImgIn.u32Width = (uint32_t)mediaRgaCfg.src_rect.w;
    pstAttr->stImgIn.u32Height = (uint32_t)mediaRgaCfg.src_rect.h;
    pstAttr->stImgOut.u32X = (uint32_t)mediaRgaCfg.dst_rect.x;
    pstAttr->stImgOut.u32Y = (uint32_t)mediaRgaCfg.dst_rect.y;
    pstAttr->stImgOut.u32Width = (uint32_t)mediaRgaCfg.dst_rect.w;
    pstAttr->stImgOut.u32Height = (uint32_t)mediaRgaCfg.dst_rect.h;
    pstAttr->u16Rotaion = (uint32_t)mediaRgaCfg.rotation;
    pstAttr->enFlip = (drm_rga_flip_e)mediaRgaCfg.flip;
    pstAttr->bEnBufPool = false;
    pstAttr->u16BufPoolCnt = 0;

    return 0;
}

int drm_mpi_rga_set_channel_attribute(int channel, const drm_rga_attr_t *pstAttr)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstAttr) {
        return -2;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status < CHN_STATUS_OPEN) {
        g_rga_mtx.unlock();
        return -3;
    }

    if (!g_rga_chns[channel].media_flow) {
        g_rga_mtx.unlock();
        return -4;
    }

    DrmRgaConfig mediaRgaCfg;
    mediaRgaCfg.src_rect.x = (int)pstAttr->stImgIn.u32X;
    mediaRgaCfg.src_rect.y = (int)pstAttr->stImgIn.u32Y;
    mediaRgaCfg.src_rect.w = (int)pstAttr->stImgIn.u32Width;
    mediaRgaCfg.src_rect.h = (int)pstAttr->stImgIn.u32Height;
    mediaRgaCfg.dst_rect.x = (int)pstAttr->stImgOut.u32X;
    mediaRgaCfg.dst_rect.y = (int)pstAttr->stImgOut.u32Y;
    mediaRgaCfg.dst_rect.w = (int)pstAttr->stImgOut.u32Width;
    mediaRgaCfg.dst_rect.h = (int)pstAttr->stImgOut.u32Height;
    mediaRgaCfg.rotation = (int)pstAttr->u16Rotaion;
    mediaRgaCfg.flip = (int)pstAttr->enFlip;

    int ret = g_rga_chns[channel].media_flow->Control(libdrm::S_RGA_CFG, &mediaRgaCfg);
    g_rga_mtx.unlock();

    return ret ? -5 : 0;
}

int drm_mpi_rga_get_channel_region_luma(int channel, const drm_video_region_info_t *pstRegionInfo, uint64_t *pu64LumaData, int s32MilliSec)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRegionInfo || !pu64LumaData) {
        return -2;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status < CHN_STATUS_OPEN) {
        g_rga_mtx.unlock();
        return -3;
    }

    if (!g_rga_chns[channel].media_flow) {
        g_rga_mtx.unlock();
        return -4;
    }

    if (pstRegionInfo->u32RegionNum > REGION_LUMA_MAX) {
        g_rga_mtx.unlock();
        return -5;
    }

    DrmImageRegionLuma region_luma;
    memset(&region_luma, 0, sizeof(DrmImageRegionLuma));
    region_luma.region_num = pstRegionInfo->u32RegionNum;
    region_luma.ms = s32MilliSec;

    for (uint32_t i = 0; i < region_luma.region_num; i++) {
        region_luma.region[i].x = pstRegionInfo->pstRegion[i].s32X;
        region_luma.region[i].y = pstRegionInfo->pstRegion[i].s32Y;
        region_luma.region[i].w = pstRegionInfo->pstRegion[i].u32Width;
        region_luma.region[i].h = pstRegionInfo->pstRegion[i].u32Height;
    }

    int ret = g_rga_chns[channel].media_flow->Control(libdrm::G_RGA_REGION_LUMA, &region_luma);
    if (ret) {
        g_rga_mtx.unlock();
        return -6;
    }

    memcpy(pu64LumaData, region_luma.luma_data, sizeof(uint64_t) * region_luma.region_num);
    g_rga_mtx.unlock();

    return 0;
}

int drm_mpi_rga_region_set_cover(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_cover_info_t *pstCoverInfo)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRgnInfo) {
        return -2;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status < CHN_STATUS_OPEN) {
        g_rga_mtx.unlock();
        return -3;
    }

    if (!g_rga_chns[channel].media_flow) {
        g_rga_mtx.unlock();
        return -4;
    }

    DrmImageBorder line;
    memset(&line, 0, sizeof(DrmImageBorder));
    line.id = pstRgnInfo->enRegionId;
    line.enable = pstRgnInfo->u8Enable;
    line.x = pstRgnInfo->u32PosX;
    line.y = pstRgnInfo->u32PosY;
    line.w = pstRgnInfo->u32Width;
    line.h = pstRgnInfo->u32Height;
    if (pstCoverInfo) {
        line.color = pstCoverInfo->u32Color;
    }

    int ret = g_rga_chns[channel].media_flow->Control(libdrm::S_RGA_LINE_INFO, &line);
    if (ret) {
        g_rga_mtx.unlock();
        return -5;
    }
    g_rga_mtx.unlock();

    return 0;
}

int drm_mpi_rga_region_set_bitmap(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_bitmap_t *pstBitmap)
{
    if ((channel < DRM_RGA_CHANNEL_00) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRgnInfo || !pstBitmap) {
        return -2;
    }

    g_rga_mtx.lock();
    if (g_rga_chns[channel].status < CHN_STATUS_OPEN) {
        g_rga_mtx.unlock();
        return -3;
    }

    if (!g_rga_chns[channel].media_flow) {
        g_rga_mtx.unlock();
        return -4;
    }

    DrmImageOsd osd;
    memset(&osd, 0, sizeof(osd));
    osd.id = pstRgnInfo->enRegionId;
    osd.enable = pstRgnInfo->u8Enable;
    osd.x = pstRgnInfo->u32PosX;
    osd.y = pstRgnInfo->u32PosY;
    osd.w = pstRgnInfo->u32Width;
    osd.h = pstRgnInfo->u32Height;
    osd.data = pstBitmap->pData;
    osd.pix_fmt = get_osd_pix_format(pstBitmap->enPixelFormat);

    int ret = g_rga_chns[channel].media_flow->Control(libdrm::S_RGA_OSD_INFO, &osd);
    if (ret) {
        g_rga_mtx.unlock();
        return -5;
    }
    g_rga_mtx.unlock();

    return 0;
}

int drm_mpi_vmix_destroy_device(int device)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].bInit == false) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -2;
    }

    if (g_vmix_dev[device].u16RefCnt > 0) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    if (g_vmix_dev[device].media_flow) {
        g_vmix_dev[device].media_flow.reset();
    }

    g_vmix_dev[device].bInit = false;
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

int drm_mpi_vmix_create_device(int device, drm_vmix_dev_info_t *pstDevInfo)
{
    uint16_t u16Rotaion;
    drm_rga_flip_e enFlip;

    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if (!pstDevInfo) {
        return -2;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].bInit == true) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    DRM_MEDIA_LOGI("Enable vmix device:[%d] starting......", device);

    std::string param;
    PARAM_STRING_APPEND(param, DRM_KEY_NAME, "rkrga");
    PARAM_STRING_APPEND(param, DRM_KEK_THREAD_SYNC_MODEL, DRM_KEY_ASYNCATOMIC);
    PARAM_STRING_APPEND_TO(param, DRM_KEY_FPS, pstDevInfo->u16Fps);
    PARAM_STRING_APPEND(param, DRM_KEY_OUTPUTDATATYPE, ImageTypeToString(pstDevInfo->enImgType));
    PARAM_STRING_APPEND(param, DRM_KEY_INPUTDATATYPE, ImageTypeToString(pstDevInfo->enImgType));
    PARAM_STRING_APPEND_TO(param, DRM_KEY_BUFFER_WIDTH, pstDevInfo->u32ImgWidth);
    PARAM_STRING_APPEND_TO(param, DRM_KEY_BUFFER_HEIGHT, pstDevInfo->u32ImgHeight);
    PARAM_STRING_APPEND_TO(param, DRM_KEY_BUFFER_VIR_WIDTH, pstDevInfo->u32ImgWidth);
    PARAM_STRING_APPEND_TO(param, DRM_KEY_BUFFER_VIR_HEIGHT, pstDevInfo->u32ImgHeight);

    bool bEnableBp = pstDevInfo->bEnBufPool;
    uint16_t u16BufPoolCnt = pstDevInfo->u16BufPoolCnt;

    if (bEnableBp) {
        PARAM_STRING_APPEND(param, DRM_KEY_MEM_TYPE, DRM_KEY_MEM_HARDWARE);
        PARAM_STRING_APPEND_TO(param, DRM_KEY_MEM_CNT, u16BufPoolCnt);
    }

    for (uint16_t i = 0; i < pstDevInfo->u16ChnCnt; i++) {
        char rect_str[128] = {0};
        snprintf(rect_str, sizeof(rect_str), "(%d,%d,%u,%u)->(%d,%d,%u,%u)",
                pstDevInfo->stChnInfo[i].stInRect.s32X,
                pstDevInfo->stChnInfo[i].stInRect.s32Y,
                pstDevInfo->stChnInfo[i].stInRect.u32Width,
                pstDevInfo->stChnInfo[i].stInRect.u32Height,
                pstDevInfo->stChnInfo[i].stOutRect.s32X,
                pstDevInfo->stChnInfo[i].stOutRect.s32Y,
                pstDevInfo->stChnInfo[i].stOutRect.u32Width,
                pstDevInfo->stChnInfo[i].stOutRect.u32Height);
        param.append(" ");
        PARAM_STRING_APPEND(param, DRM_KEY_BUFFER_RECT, rect_str);

        u16Rotaion = pstDevInfo->stChnInfo[i].u16Rotaion;
        if ((u16Rotaion != 0) && (u16Rotaion != 90) && (u16Rotaion != 180) && (u16Rotaion != 270)) {
            DRM_MEDIA_LOGW("invalid rotation angle:[%d], only 0/90/180/270", u16Rotaion);
        } else {
            PARAM_STRING_APPEND_TO(param, DRM_KEY_BUFFER_ROTATE, u16Rotaion);
        }

        enFlip = pstDevInfo->stChnInfo[i].enFlip;
        if ((enFlip != RGA_FLIP_H) && (enFlip != RGA_FLIP_V) && (enFlip != RGA_FLIP_HV)) {
            DRM_MEDIA_LOGW("invalid flip type:[%d], only 0/1/2/3", enFlip);
        } else {
            PARAM_STRING_APPEND_TO(param, DRM_KEY_BUFFER_FLIP, enFlip);
        }
    }

    DRM_MEDIA_LOGD("vmix Flow:[%s]", param.c_str());

    g_vmix_dev[device].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>("filter", param.c_str());
    if (!g_vmix_dev[device].media_flow) {
        DRM_MEDIA_LOGE("Create video mixer flow failed");
        g_vmix_dev[device].VmixMtx.unlock();
        return -4;
    }

    g_vmix_dev[device].u16ChnCnt = pstDevInfo->u16ChnCnt;
    g_vmix_dev[device].bInit = true;
    g_vmix_dev[device].VmixMtx.unlock();

    DRM_MEDIA_LOGI("Enable vmix device:[%d] finished......", device);

    return 0;
}

int drm_mpi_vmix_enable_channel(int device, int channel)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].bInit == false) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    if (g_vmix_dev[device].VmixChns[channel].status != CHN_STATUS_CLOSED) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -4;
    }

    g_vmix_dev[device].VmixChns[channel].media_flow = g_vmix_dev[device].media_flow;
    g_vmix_dev[device].u16RefCnt++;
    g_vmix_dev[device].VmixChns[channel].status = CHN_STATUS_OPEN;
    if (!channel) {
        init_media_channel_buffer(&g_vmix_dev[device].VmixChns[channel]);
        g_vmix_dev[device].media_flow->SetOutputCallBack(&g_vmix_dev[device].VmixChns[channel], flow_output_callback);
    }
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

int drm_mpi_vmix_disable_channel(int device, int channel)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status == CHN_STATUS_BIND) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    if (g_vmix_dev[device].VmixChns[channel].media_flow) {
        g_vmix_dev[device].VmixChns[channel].media_flow.reset();
    }

    if (g_vmix_dev[device].u16RefCnt > 0) {
        g_vmix_dev[device].u16RefCnt--;
    }

    g_vmix_dev[device].VmixChns[channel].status = CHN_STATUS_CLOSED;
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

int drm_mpi_vmix_hide_channel(int device, int channel)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status < CHN_STATUS_OPEN) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    g_vmix_dev[device].media_flow->Control(libdrm::S_RGA_HIDE, &channel);
    g_vmix_dev[device].VmixMtx.unlock();

    return 9;
}

int drm_mpi_vmix_show_channel(int device, int channel)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status < CHN_STATUS_OPEN) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    g_vmix_dev[device].media_flow->Control(libdrm::S_RGA_SHOW, &channel);
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

int drm_mpi_vmix_set_layout_line(int device, int channel, drm_vmix_line_info_t *pstVmixLine)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    if (!pstVmixLine) {
        return -3;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status < CHN_STATUS_OPEN) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -4;
    }

    for (unsigned int i = 0; i < pstVmixLine->u32LineCnt && i < ARRAY_ELEMS(pstVmixLine->stLines); i++) {
        DrmImageBorder line;

        memset(&line, 0x00, sizeof(DrmImageBorder));
        line.id = i;
        line.enable = pstVmixLine->u8Enable[i];
        line.x = pstVmixLine->stLines[i].s32X;
        line.y = pstVmixLine->stLines[i].s32Y;
        line.w = pstVmixLine->stLines[i].u32Width;
        line.h = pstVmixLine->stLines[i].u32Height;
        line.color = pstVmixLine->u32Color;
        line.priv = channel;

        DRM_MEDIA_LOGI("device:[%d], channel:[%d], line:[%d %d %d %d]", device, line.priv, line.x, line.y, line.w, line.h);
        g_vmix_dev[device].media_flow->Control(libdrm::S_RGA_LINE_INFO, &line);
    }
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

static int vmix_get_channel_region_luma(int device, int channel, const drm_video_region_info_t *pstRegionInfo, uint64_t *pu64LumaData, int s32MilliSec, int offset)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    if (!pstRegionInfo || !pu64LumaData) {
        return -3;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status < CHN_STATUS_OPEN) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -4;
    }

    if (pstRegionInfo->u32RegionNum > REGION_LUMA_MAX) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -5;
    }

    DrmImageRegionLuma region_luma;
    memset(&region_luma, 0, sizeof(DrmImageRegionLuma));
    region_luma.priv = channel;
    region_luma.region_num = pstRegionInfo->u32RegionNum;
    region_luma.ms = s32MilliSec;
    region_luma.offset = offset;

    for (uint32_t i = 0; i < region_luma.region_num; i++) {
        region_luma.region[i].x = pstRegionInfo->pstRegion[i].s32X;
        region_luma.region[i].y = pstRegionInfo->pstRegion[i].s32Y;
        region_luma.region[i].w = pstRegionInfo->pstRegion[i].u32Width;
        region_luma.region[i].h = pstRegionInfo->pstRegion[i].u32Height;
    }

    int ret =g_vmix_dev[device].media_flow->Control(libdrm::G_RGA_REGION_LUMA, &region_luma);
    if (ret) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -6;
    }

    memcpy(pu64LumaData, region_luma.luma_data, sizeof(uint64_t) * region_luma.region_num);
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

int drm_mpi_vmix_get_device_region_luma(int device, const drm_video_region_info_t *pstRegionInfo, uint64_t *pu64LumaData, int s32MilliSec)
{
    return vmix_get_channel_region_luma(device, g_vmix_dev[device].u16RefCnt - 1, pstRegionInfo, pu64LumaData, s32MilliSec, 0);
}

int drm_mpi_vmix_get_channel_region_luma(int device, int channel, const drm_video_region_info_t *pstRegionInfo, uint64_t *pu64LumaData, int s32MilliSec)
{
    return vmix_get_channel_region_luma(device, channel, pstRegionInfo, pu64LumaData, s32MilliSec, 1);
}

int drm_mpi_vmix_region_set_cover(int device, int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_cover_info_t *pstCoverInfo)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    if (!pstRgnInfo) {
        return -3;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status < CHN_STATUS_OPEN) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -4;
    }

    DrmImageBorder line;
    memset(&line, 0, sizeof(DrmImageBorder));
    line.id = pstRgnInfo->enRegionId;
    line.enable = pstRgnInfo->u8Enable;
    line.priv = channel;
    line.x = pstRgnInfo->u32PosX;
    line.y = pstRgnInfo->u32PosY;
    line.w = pstRgnInfo->u32Width;
    line.h = pstRgnInfo->u32Height;
    if (pstCoverInfo) {
        line.color = pstCoverInfo->u32Color;
    }

    line.offset = 1;
    int ret = g_vmix_dev[device].media_flow->Control(libdrm::S_RGA_LINE_INFO, &line);
    if (ret) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -5;
    }
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

int drm_mpi_vmix_region_set_bitmap(int device, int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_bitmap_t *pstBitmap)
{
    if ((device < DRM_VMIX_DEVICE_00) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < DRM_VMIX_CHANNEL_00) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
        return -2;
    }

    g_vmix_dev[device].VmixMtx.lock();
    if (g_vmix_dev[device].VmixChns[channel].status < CHN_STATUS_OPEN) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -3;
    }

    DrmImageOsd osd;
    memset(&osd, 0, sizeof(osd));
    osd.id = pstRgnInfo->enRegionId;
    osd.enable = pstRgnInfo->u8Enable;
    osd.priv = channel;
    osd.x = pstRgnInfo->u32PosX;
    osd.y = pstRgnInfo->u32PosY;
    osd.w = pstRgnInfo->u32Width;
    osd.h = pstRgnInfo->u32Height;
    osd.data = pstBitmap->pData;
    osd.pix_fmt = get_osd_pix_format(pstBitmap->enPixelFormat);

    int ret = g_vmix_dev[device].media_flow->Control(libdrm::S_RGA_OSD_INFO, &osd);
    if (ret) {
        g_vmix_dev[device].VmixMtx.unlock();
        return -4;
    }
    g_vmix_dev[device].VmixMtx.unlock();

    return 0;
}

static int parse_split_attribute(drm_video_mode_e enVideoMode)
{
    int split = 0;

    switch (enVideoMode) {
        case VIDEO_MODE_STREAM:
            split = 1;
            break;

        case VIDEO_MODE_FRAME:
            split = 0;
            break;

        case VIDEO_MODE_COMPAT:
        default:
            split = -1;
    }

    return split;
}

int drm_mpi_vdec_destroy_channel(int channel)
{
    if ((channel < DRM_VDEC_CHANNEL_00) || (channel >= DRM_VDEC_CHANNEL_BUTT)) {
        return -1;
    }

    g_vdec_mtx.lock();
    if (g_vdec_chns[channel].status == CHN_STATUS_BIND) {
        g_vdec_mtx.unlock();
        return -2;
    }

    g_vdec_chns[channel].media_flow.reset();
    clear_media_channel_buffer(&g_vdec_chns[channel]);
    g_vdec_chns[channel].status = CHN_STATUS_CLOSED;
    g_vdec_mtx.unlock();

    return 0;
}

int drm_mpi_vdec_create_channel(int channel, const drm_vdec_chn_attr_t *pstAttr)
{
    if ((channel < DRM_VDEC_CHANNEL_00) || (channel >= DRM_VDEC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstAttr) {
        return -2;
    }

    int split = 0;
    int timeout = 0;
    std::string flow_name;
    std::string flow_param;
    std::shared_ptr<libdrm::Flow> video_decoder_flow;

    flow_name = "video_dec";
    flow_param = "";

    if (pstAttr->enDecodecMode == VIDEO_DECODEC_HADRWARE) {
        PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
    } else if (pstAttr->enDecodecMode == VIDEO_DECODEC_SOFTWARE) {
        PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkaudio_vid");
        PARAM_STRING_APPEND(flow_param, DRM_KEK_THREAD_SYNC_MODEL, DRM_KEY_SYNC);
    } else {
        return -3;
    }

    switch (pstAttr->enCodecType) {
        case DRM_CODEC_TYPE_H264:
        case DRM_CODEC_TYPE_H265:
            split = parse_split_attribute(pstAttr->enMode);
            break;

        case DRM_CODEC_TYPE_JPEG:
        case DRM_CODEC_TYPE_MJPEG:
            if (pstAttr->enMode == VIDEO_MODE_STREAM) {
                DRM_MEDIA_LOGE("JPEG/MJPEG only support VIDEO_MODE_FRAME");
                return -4;
            }
            split = parse_split_attribute(pstAttr->enMode);
            break;

        default:
            DRM_MEDIA_LOGE("Not support CodecType:[%d]", pstAttr->enCodecType);
            return -5;
    }

    if (split < 0) {
        DRM_MEDIA_LOGE("Not support split mode:[%d]", pstAttr->enMode);
        return -6;
    } else if (split == 0) {
        timeout = -1;
    }

    PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, CodecToString(pstAttr->enCodecType));
    // PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, ImageTypeToString(pstAttr->enImageType));

    std::string dec_param = "";
    PARAM_STRING_APPEND(dec_param, DRM_KEY_INPUTDATATYPE, CodecToString(pstAttr->enCodecType));

    PARAM_STRING_APPEND_TO(dec_param, DRM_KEY_MPP_SPLIT_MODE, split);
    PARAM_STRING_APPEND_TO(dec_param, DRM_KEY_OUTPUT_TIMEOUT, timeout);

    g_vdec_mtx.lock();
    if (g_vdec_chns[channel].status != CHN_STATUS_CLOSED) {
        g_vdec_mtx.unlock();
        return -7;
    }

    DRM_MEDIA_LOGI("Enable vdec channel:[%d] starting......", channel);

    memcpy(&g_vdec_chns[channel].vdec_attr, pstAttr, sizeof(drm_vdec_chn_attr_t));

    flow_param = libdrm::JoinFlowParam(flow_param, 1, dec_param);
    DRM_MEDIA_LOGD("VDEC: flow param:[%s]", flow_param.c_str());

    video_decoder_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!video_decoder_flow) {
        DRM_MEDIA_LOGE("Create flow:[%s] failed", flow_name.c_str());

        g_vdec_mtx.unlock();
        g_vdec_chns[channel].status = CHN_STATUS_CLOSED;

        return -8;
    }

    g_vdec_chns[channel].media_flow = video_decoder_flow;
    init_media_channel_buffer(&g_vdec_chns[channel]);
    g_vdec_chns[channel].status = CHN_STATUS_OPEN;
    g_vdec_chns[channel].media_flow->SetOutputCallBack(&g_vdec_chns[channel], flow_output_callback);
    g_vdec_mtx.unlock();

    DRM_MEDIA_LOGI("Enable vdec channel:[%d] finished......", channel);

    return 0;
}

static int create_media_jpeg_snap_pipeline(drm_media_channel_t *VenChn)
{
    std::shared_ptr<libdrm::Flow> video_rga_flow;
    std::shared_ptr<libdrm::Flow> video_jpeg_flow;
    std::shared_ptr<libdrm::Flow> video_encoder_flow;
    std::shared_ptr<libdrm::Flow> video_decoder_flow;

    int s32ZoomWidth = 0;
    int s32ZoomHeight = 0;
    int s32ZoomVirWidth = 0;
    int s32ZoomVirHeight = 0;
    bool bEnableRga = false;
    bool bSupportDcf = false;
    bool bEnableH265 = false;
    uint32_t u32InFpsNum = 1;
    uint32_t u32InFpsDen = 1;
    uint32_t u32OutFpsNum = 1;
    uint32_t u32OutFpsDen = 1;
    const char *pcMediaRcMode = nullptr;
    const char *pcMediaCodecType = nullptr;
    VENC_CHN_ATTR_S *stVencChnAttr = &VenChn->venc_attr.attr;
    VENC_CHN_PARAM_S *stVencChnParam = &VenChn->venc_attr.param;
    drm_venc_rotation_e enRotation = stVencChnAttr->stVencAttr.enRotation;

    int mjpeg_bps = 0;
    int pre_enc_bps = 2000000;
    int video_width = stVencChnAttr->stVencAttr.u32PicWidth;
    int video_height = stVencChnAttr->stVencAttr.u32PicHeight;
    int vir_width = stVencChnAttr->stVencAttr.u32VirWidth;
    int vir_height = stVencChnAttr->stVencAttr.u32VirHeight;

    std::string pixel_format = ImageTypeToString(stVencChnAttr->stVencAttr.imageType);

    if (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
        if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGCBR) {
            mjpeg_bps = stVencChnAttr->stRcAttr.stMjpegCbr.u32BitRate;
            u32InFpsNum = stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateNum;
            u32InFpsDen = stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateDen;
            u32OutFpsNum = stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateNum;
            u32OutFpsDen = stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateDen;
            pcMediaRcMode = DRM_KEY_CBR;
        } else if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGVBR) {
            mjpeg_bps = stVencChnAttr->stRcAttr.stMjpegVbr.u32BitRate;
            u32InFpsNum = stVencChnAttr->stRcAttr.stMjpegVbr.u32SrcFrameRateNum;
            u32InFpsDen = stVencChnAttr->stRcAttr.stMjpegVbr.u32SrcFrameRateDen;
            u32OutFpsNum = stVencChnAttr->stRcAttr.stMjpegVbr.fr32DstFrameRateNum;
            u32OutFpsDen = stVencChnAttr->stRcAttr.stMjpegVbr.fr32DstFrameRateDen;
            pcMediaRcMode = DRM_KEY_VBR;
        } else {
            DRM_MEDIA_LOGE("Invalid RcMode:[%d]", stVencChnAttr->stRcAttr.enRcMode);
            return -1;
        }

        if ((mjpeg_bps < 2000) || (mjpeg_bps > 100000000)) {
            DRM_MEDIA_LOGE("Invalid BitRate[%d], should be:[2000, 100000000]", mjpeg_bps);
            return -2;
        }

        if (!u32InFpsNum) {
            DRM_MEDIA_LOGE("Invalid src frame rate:[%d/%d]", u32InFpsNum, u32InFpsDen);
            return -3;
        }

        if (!u32OutFpsNum) {
            DRM_MEDIA_LOGE("Invalid dst frame rate:[%d/%d]", u32OutFpsNum, u32OutFpsDen);
            return -4;
        }

        pcMediaCodecType = DRM_VIDEO_MJPEG;
        s32ZoomWidth = stVencChnAttr->stVencAttr.stAttrMjpege.u32ZoomWidth;
        s32ZoomHeight = stVencChnAttr->stVencAttr.stAttrMjpege.u32ZoomHeight;
        s32ZoomVirWidth = stVencChnAttr->stVencAttr.stAttrMjpege.u32ZoomVirWidth;
        s32ZoomVirHeight = stVencChnAttr->stVencAttr.stAttrMjpege.u32ZoomVirHeight;
    } else {
        pcMediaCodecType = DRM_IMAGE_JPEG;
        s32ZoomWidth = stVencChnAttr->stVencAttr.stAttrJpege.u32ZoomWidth;
        s32ZoomHeight = stVencChnAttr->stVencAttr.stAttrJpege.u32ZoomHeight;
        s32ZoomVirWidth = stVencChnAttr->stVencAttr.stAttrJpege.u32ZoomVirWidth;
        s32ZoomVirHeight = stVencChnAttr->stVencAttr.stAttrJpege.u32ZoomVirHeight;
        bSupportDcf = stVencChnAttr->stVencAttr.stAttrJpege.bSupportDCF;
    }

    std::string enc_param = "";
    std::string flow_param = "";
    std::string flow_name = "video_enc";

    std::string str_fps_in;
    str_fps_in.append(std::to_string(u32InFpsNum)).append("/").append(std::to_string(u32InFpsDen));

    std::string str_fps_out;
    str_fps_out.append(std::to_string(u32OutFpsNum)).append("/").append(std::to_string(u32OutFpsDen));

    if ((stVencChnAttr->stVencAttr.imageType == DRM_IMAGE_TYPE_FBC0)
        || (stVencChnAttr->stVencAttr.imageType == DRM_IMAGE_TYPE_FBC2)
        || (stVencChnAttr->stVencAttr.imageType == DRM_IMAGE_TYPE_NV16))
    {
        PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
        PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, pixel_format);
        PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, DRM_VIDEO_H265);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_WIDTH, video_width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_HEIGHT, video_height);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_WIDTH, vir_width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_HEIGHT, vir_height);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, video_width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, video_height);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, pre_enc_bps);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, pre_enc_bps);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MIN, pre_enc_bps);
        PARAM_STRING_APPEND(enc_param, DRM_KEY_VIDEO_GOP, "1");
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps_out);
 
        PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_FIXQP);
        PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_QP_INIT, "15");
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_ROTATION, enRotation);

        flow_param = libdrm::JoinFlowParam(flow_param, 1, enc_param);
        DRM_MEDIA_LOGD("JPEG: Pre encoder flow param:[%s]", flow_param.c_str());

        video_encoder_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
        if (!video_encoder_flow) {
            DRM_MEDIA_LOGE("Create flow:[%s] failed", flow_name.c_str());
            return -5;
        }

        flow_name = "video_dec";
        flow_param = "";
        PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
        PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, DRM_VIDEO_H265);
        PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, DRM_IMAGE_NV12);

        std::string dec_param = "";
        PARAM_STRING_APPEND(dec_param, DRM_KEY_INPUTDATATYPE, DRM_VIDEO_H265);
        PARAM_STRING_APPEND_TO(dec_param, DRM_KEY_MPP_SPLIT_MODE, 0);
        PARAM_STRING_APPEND_TO(dec_param, DRM_KEY_OUTPUT_TIMEOUT, -1);

        flow_param = libdrm::JoinFlowParam(flow_param, 1, dec_param);
        DRM_MEDIA_LOGD("JPEG: Pre decoder flow param:[%s]", flow_param.c_str());

        video_decoder_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
        if (!video_decoder_flow) {
            DRM_MEDIA_LOGE("Create flow:[%s] failed", flow_name.c_str());
            return -6;
        }

        bEnableH265 = true;
    }

    int jpeg_width = video_width;
    int jpeg_height = video_height;
    int s32RgaWidth = s32ZoomWidth;
    int s32RgaHeight = s32ZoomHeight;
    int s32RgaVirWidht = DRM_UPALIGNTO16(s32ZoomVirWidth);
    int s32RgaVirHeight = DRM_UPALIGNTO16(s32ZoomVirHeight);

    if ((enRotation == VENC_ROTATION_90) || (enRotation == VENC_ROTATION_270)) {
        jpeg_width = video_height;
        jpeg_height = video_width;
        s32RgaWidth = s32ZoomHeight;
        s32RgaHeight = s32ZoomWidth;
        s32RgaVirWidht = DRM_UPALIGNTO16(s32ZoomVirHeight);
        s32RgaVirHeight = DRM_UPALIGNTO16(s32ZoomVirWidth);
    }

    int jpeg_vir_height = DRM_UPALIGNTO(jpeg_height, 8);
    int jpeg_vir_width = DRM_UPALIGNTO(jpeg_width, 256);
    if (((jpeg_vir_width / 256) % 2) == 0) {
        jpeg_vir_width += 256;
    }

    if ((s32RgaWidth > 0) && (s32RgaHeight > 0) && (s32RgaVirWidht > 0)
        && (s32RgaVirHeight > 0)
        && ((s32RgaWidth != video_width) || (s32RgaHeight != video_height)
        || (s32RgaVirWidht != vir_width) || (s32RgaVirHeight != vir_height)))
    {
        flow_name = "filter";
        flow_param = "";
        PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkrga");

        if (!bEnableH265) {
            PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, pixel_format);
        } else {
            PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, DRM_IMAGE_NV12);
        }

        PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, DRM_IMAGE_NV12);
        PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_WIDTH, s32RgaWidth);
        PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_HEIGHT, s32RgaHeight);
        PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_VIR_WIDTH, s32RgaVirWidht);
        PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_BUFFER_VIR_HEIGHT, s32RgaVirHeight);
        PARAM_STRING_APPEND(flow_param, DRM_KEY_MEM_TYPE, DRM_KEY_MEM_HARDWARE);
        PARAM_STRING_APPEND_TO(flow_param, DRM_KEY_MEM_CNT, 2);

        std::string filter_param = "";
        DrmImageRect src_rect = {0, 0, jpeg_width, jpeg_height};
        DrmImageRect dst_rect = {0, 0, s32RgaWidth, s32RgaHeight};

        std::vector<DrmImageRect> rect_vect;
        rect_vect.push_back(src_rect);
        rect_vect.push_back(dst_rect);

        PARAM_STRING_APPEND(filter_param, DRM_KEY_BUFFER_RECT, libdrm::TwoImageRectToString(rect_vect).c_str());
        PARAM_STRING_APPEND_TO(filter_param, DRM_KEY_BUFFER_ROTATE, 0);

        flow_param = libdrm::JoinFlowParam(flow_param, 1, filter_param);
        DRM_MEDIA_LOGD("JPEG: Pre process flow param:[%s]", flow_param.c_str());

        video_rga_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
        if (!video_rga_flow) {
            DRM_MEDIA_LOGE("Create flow filter:rga failed");
            return -7;
        }

        bEnableRga = true;
        jpeg_width = s32RgaWidth;
        jpeg_height = s32RgaHeight;
        jpeg_vir_width = s32RgaVirWidht;
        jpeg_vir_height = s32RgaVirHeight;
    }

    flow_name = "video_enc";
    flow_param = "";
    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
    PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, DRM_IMAGE_NV12);
    PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, pcMediaCodecType);

    enc_param = "";
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_WIDTH, jpeg_width);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_HEIGHT, jpeg_height);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_WIDTH, jpeg_vir_width);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_HEIGHT, jpeg_vir_height);

    if (stVencChnParam->stCropCfg.bEnable) {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, stVencChnParam->stCropCfg.stRect.s32X);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, stVencChnParam->stCropCfg.stRect.s32Y);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, stVencChnParam->stCropCfg.stRect.u32Width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, stVencChnParam->stCropCfg.stRect.u32Height);
    } else {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, jpeg_width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, jpeg_height);
    }

    if (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
        if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGCBR) {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, mjpeg_bps);
        } else {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, mjpeg_bps);
        }

        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps_out);
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_out);
        PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, pcMediaRcMode);
    } else {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_QFACTOR, 50);

        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_ENABLE_JPEG_DCF, bSupportDcf);
        if (stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum > 0) {
            PARAM_STRING_APPEND_TO( enc_param, DRM_KEY_JPEG_MPF_CNT, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF0_W, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[0].u32Width);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF0_H, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[0].u32Height);
        }

        if (stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum > 1) {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF1_W, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[1].u32Width);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF1_H, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[1].u32Height);
        }
    }

    flow_param = libdrm::JoinFlowParam(flow_param, 1, enc_param);
    DRM_MEDIA_LOGD("JPEG: Type:[%s] encoder flow param:[%s]", pcMediaCodecType, flow_param.c_str());

    video_jpeg_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!video_jpeg_flow) {
        DRM_MEDIA_LOGE("Create flow:[%s] failed", flow_name.c_str());
        return -8;
    }

    if (bEnableH265) {
        video_encoder_flow->SetFlowTag("JpegPreEncoder");
        video_decoder_flow->SetFlowTag("JpegPreDecoder");
    }

    video_jpeg_flow->SetFlowTag("JpegEncoder");
    if (bEnableRga) {
        video_rga_flow->SetFlowTag("JpegRgaFilter");
    }

    if (bEnableRga) {
        video_rga_flow->AddDownFlow(video_jpeg_flow, 0, 0);
        if (bEnableH265) {
            video_decoder_flow->AddDownFlow(video_rga_flow, 0, 0);
        }
    } else {
        video_decoder_flow->AddDownFlow(video_jpeg_flow, 0, 0);
    }

    if (bEnableH265) {
        video_encoder_flow->AddDownFlow(video_decoder_flow, 0, 0);
    }

    init_media_channel_buffer(VenChn);
    video_jpeg_flow->SetOutputCallBack(VenChn, flow_output_callback);

    if (bEnableH265) {
        VenChn->media_flow = video_encoder_flow;
        VenChn->media_flow_list.push_back(video_decoder_flow);
    } else {
        VenChn->media_flow = video_rga_flow;
    }

    if (bEnableRga) {
        VenChn->media_flow_list.push_back(video_rga_flow);
    }
    VenChn->media_flow_list.push_back(video_jpeg_flow);

    if (pipe2(VenChn->wake_fd, O_CLOEXEC) == -1) {
        VenChn->wake_fd[0] = 0;
        VenChn->wake_fd[1] = 0;
        DRM_MEDIA_LOGW("Create pipe failed");
    }

    VenChn->status = CHN_STATUS_OPEN;
    VenChn->venc_attr.bFullFunc = true;

    return 0;
}

static int create_media_jpeg_light(drm_media_channel_t *VenChn)
{
    VENC_CHN_ATTR_S *stVencChnAttr = &VenChn->venc_attr.attr;
    VENC_CHN_PARAM_S *stVencChnParam = &VenChn->venc_attr.param;

    if ((stVencChnAttr->stVencAttr.enType != DRM_CODEC_TYPE_JPEG) && (stVencChnAttr->stVencAttr.enType != DRM_CODEC_TYPE_MJPEG)) {
        return -1;
    }

    int mjpeg_bps = 0;
    uint32_t u32InFpsNum = 1;
    uint32_t u32InFpsDen = 1;
    uint32_t u32OutFpsNum = 1;
    uint32_t u32OutFpsDen = 1;
    bool bSupprtDcf = false;
    const char *pcMediaRcMode = nullptr;
    const char *pcMediaCodecType = nullptr;
    std::shared_ptr<libdrm::Flow> video_jpeg_flow;
    int video_width = stVencChnAttr->stVencAttr.u32PicWidth;
    int video_height = stVencChnAttr->stVencAttr.u32PicHeight;
    int vir_width = stVencChnAttr->stVencAttr.u32VirWidth;
    int vir_height = stVencChnAttr->stVencAttr.u32VirHeight;
    std::string pixel_format = ImageTypeToString(stVencChnAttr->stVencAttr.imageType);
    drm_venc_rotation_e enRotation = stVencChnAttr->stVencAttr.enRotation;

    if (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
        if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGCBR) {
            mjpeg_bps = stVencChnAttr->stRcAttr.stMjpegCbr.u32BitRate;
            u32InFpsNum = stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateNum;
            u32InFpsDen = stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateDen;
            u32OutFpsNum = stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateNum;
            u32OutFpsDen = stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateDen;
            pcMediaRcMode = DRM_KEY_CBR;
        } else if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGVBR) {
            mjpeg_bps = stVencChnAttr->stRcAttr.stMjpegVbr.u32BitRate;
            u32InFpsNum = stVencChnAttr->stRcAttr.stMjpegVbr.u32SrcFrameRateNum;
            u32InFpsDen = stVencChnAttr->stRcAttr.stMjpegVbr.u32SrcFrameRateDen;
            u32OutFpsNum = stVencChnAttr->stRcAttr.stMjpegVbr.fr32DstFrameRateNum;
            u32OutFpsDen = stVencChnAttr->stRcAttr.stMjpegVbr.fr32DstFrameRateDen;
            pcMediaRcMode = DRM_KEY_VBR;
        } else {
            DRM_MEDIA_LOGE("Invalid RcMode:[%d]", stVencChnAttr->stRcAttr.enRcMode);
            return -2;
        }

        if ((mjpeg_bps < 2000) || (mjpeg_bps > 100000000)) {
            DRM_MEDIA_LOGE("Invalid BitRate:[%d], should be:[2000, 100000000]", mjpeg_bps);
            return -3;
        }

        if (!u32InFpsNum) {
            DRM_MEDIA_LOGE("Invalid src frame rate:[%d/%d]", u32InFpsNum, u32InFpsDen);
            return -4;
        }

        if (!u32OutFpsNum) {
            DRM_MEDIA_LOGE("Invalid dst frame rate:[%d/%d]", u32OutFpsNum, u32OutFpsDen);
            return -5;
        }

        pcMediaCodecType = DRM_VIDEO_MJPEG;
    } else {
        bSupprtDcf = stVencChnAttr->stVencAttr.stAttrJpege.bSupportDCF;
        pcMediaCodecType = DRM_IMAGE_JPEG;
    }

    std::string enc_param = "";
    std::string flow_param = "";
    std::string flow_name = "video_enc";

    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
    PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, pixel_format);
    PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, pcMediaCodecType);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_WIDTH, video_width);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_HEIGHT, video_height);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_WIDTH, vir_width);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_HEIGHT, vir_height);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_ROTATION, enRotation);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_ENABLE_JPEG_DCF, bSupprtDcf);

    if (stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum > 0) {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF_CNT, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF0_W, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[0].u32Width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF0_H, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[0].u32Height);
    }

    if (stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum > 1) {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF1_W, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[1].u32Width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_MPF1_H, stVencChnAttr->stVencAttr.stAttrJpege.stMPFCfg.astLargeThumbNailSize[1].u32Height);
    }

    if (stVencChnParam->stCropCfg.bEnable) {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, stVencChnParam->stCropCfg.stRect.s32X);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, stVencChnParam->stCropCfg.stRect.s32Y);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, stVencChnParam->stCropCfg.stRect.u32Width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, stVencChnParam->stCropCfg.stRect.u32Height);
    } else {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, video_width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, video_height);
    }

    if (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
        std::string str_fps_in;
        str_fps_in.append(std::to_string(u32InFpsNum)).append("/").append(std::to_string(u32InFpsDen));
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

        std::string str_fps_out;
        str_fps_out.append(std::to_string(u32OutFpsNum)).append("/").append(std::to_string(u32OutFpsDen));
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps_out);

        if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGCBR) {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, mjpeg_bps);
        } else {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, mjpeg_bps);
        }

        PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, pcMediaRcMode);
    } else {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_QFACTOR, 50);
    }

    flow_param = libdrm::JoinFlowParam(flow_param, 1, enc_param);
    DRM_MEDIA_LOGD("JPEG-LT: Type:[%s] encoder flow param:[%s]", pcMediaCodecType, flow_param.c_str());

    video_jpeg_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!video_jpeg_flow) {
        DRM_MEDIA_LOGE("Create flow:[%s] failed", flow_name.c_str());
        g_venc_mtx.unlock();
        return -6;
    }

    video_jpeg_flow->SetFlowTag("JpegLightEncoder");
    init_media_channel_buffer(VenChn);
    video_jpeg_flow->SetOutputCallBack(VenChn, flow_output_callback);

    VenChn->media_flow = video_jpeg_flow;
    VenChn->media_flow_list.push_back(video_jpeg_flow);
    if (pipe2(VenChn->wake_fd, O_CLOEXEC) == -1) {
        VenChn->wake_fd[0] = 0;
        VenChn->wake_fd[1] = 0;

        DRM_MEDIA_LOGW("Create pipe failed");
    }

    VenChn->status = CHN_STATUS_OPEN;
    VenChn->venc_attr.bFullFunc = false;

    return 0;
}

int drm_mpi_venc_destroy_channel(int channel)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    g_venc_mtx.lock();
    if (g_venc_chns[channel].status == CHN_STATUS_BIND) {
        g_venc_mtx.unlock();
        return -2;
    }

    while (g_venc_chns[channel].chn_ref_cnt) {
        g_venc_mtx.unlock();
        DRM_MEDIA_LOGI("wait ven channel:[%d] chn_ref_cnt to be zero...", channel);

        libdrm::msleep(100);
        g_venc_mtx.lock();
    }

    DRM_MEDIA_LOGI("Disable venc channel:[%d] starting......", channel);

    std::shared_ptr<libdrm::Flow> ptrFlowHead = NULL;
    while (!g_venc_chns[channel].media_flow_list.empty()) {
        auto ptrMediaFlow0 = g_venc_chns[channel].media_flow_list.back();
        g_venc_chns[channel].media_flow_list.pop_back();

        if (!g_venc_chns[channel].media_flow_list.empty()) {
            auto ptrMediaFlow1 = g_venc_chns[channel].media_flow_list.back();
            ptrMediaFlow1->RemoveDownFlow(ptrMediaFlow0);
        } else {
            ptrFlowHead = ptrMediaFlow0;
            break;
        }

        ptrMediaFlow0.reset();
    }

    if (g_venc_chns[channel].media_flow) {
        if (ptrFlowHead) {
            g_venc_chns[channel].media_flow->RemoveDownFlow(ptrFlowHead);
            ptrFlowHead.reset();
        }

        g_venc_chns[channel].media_flow.reset();
    }

    clear_media_channel_buffer(&g_venc_chns[channel]);
    g_venc_chns[channel].status = CHN_STATUS_CLOSED;

    if (g_venc_chns[channel].wake_fd[0] > 0) {
        close(g_venc_chns[channel].wake_fd[0]);
        g_venc_chns[channel].wake_fd[0] = 0;
    }

    if (g_venc_chns[channel].wake_fd[1] > 0) {
        close(g_venc_chns[channel].wake_fd[1]);
        g_venc_chns[channel].wake_fd[1] = 0;
    }
    g_venc_mtx.unlock();

    DRM_MEDIA_LOGI("Disable venc channl:[%d] finished......", channel);

    return 0;
}

int drm_mpi_venc_create_channel(int channel, drm_venc_chn_attr_t *stVencChnAttr)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!stVencChnAttr) {
        return -2;
    }

    g_venc_mtx.lock();
    if (g_venc_chns[channel].status != CHN_STATUS_CLOSED) {
        g_venc_mtx.unlock();
        return -3;
    }

    DRM_MEDIA_LOGI("Enable venc channel:[%d], type:[%d] starting......", channel, stVencChnAttr->stVencAttr.enType);

    if ((stVencChnAttr->stVencAttr.enRotation != VENC_ROTATION_0)
        && (stVencChnAttr->stVencAttr.enRotation != VENC_ROTATION_90)
        && (stVencChnAttr->stVencAttr.enRotation != VENC_ROTATION_180)
        && (stVencChnAttr->stVencAttr.enRotation != VENC_ROTATION_270))
    {
        DRM_MEDIA_LOGW("venc channel:[%d] rotation invalid, use default 0", channel);
        stVencChnAttr->stVencAttr.enRotation = VENC_ROTATION_0;
    }

    memset(&g_venc_chns[channel].venc_attr, 0x00, sizeof(venc_channel_attr_t));
    memcpy(&g_venc_chns[channel].venc_attr.attr, stVencChnAttr, sizeof(drm_venc_chn_attr_t));

    if ((stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_JPEG) || (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG)) {
        int ret;
        if ((stVencChnAttr->stVencAttr.imageType == DRM_IMAGE_TYPE_FBC0)
            || (stVencChnAttr->stVencAttr.imageType == DRM_IMAGE_TYPE_FBC2)
            || (stVencChnAttr->stVencAttr.imageType == DRM_IMAGE_TYPE_NV16)
            || ((stVencChnAttr->stVencAttr.stAttrJpege.u32ZoomWidth)
            && (stVencChnAttr->stVencAttr.stAttrJpege.u32ZoomWidth != stVencChnAttr->stVencAttr.u32PicWidth)))
        {
            ret = create_media_jpeg_snap_pipeline(&g_venc_chns[channel]);
        } else {
            ret = create_media_jpeg_light(&g_venc_chns[channel]);
        }
        g_venc_mtx.unlock();

        DRM_MEDIA_LOGI("Enable venc channel:[%d], type:[%d] finished......", channel, stVencChnAttr->stVencAttr.enType);
        return ret;
    }

    std::string flow_param;
    std::string flow_name = "video_enc";

    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
    PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, ImageTypeToString(stVencChnAttr->stVencAttr.imageType));
    PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, CodecToString(stVencChnAttr->stVencAttr.enType));

    std::string enc_param;
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_WIDTH, stVencChnAttr->stVencAttr.u32PicWidth);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_HEIGHT, stVencChnAttr->stVencAttr.u32PicHeight);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_WIDTH, stVencChnAttr->stVencAttr.u32VirWidth);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_HEIGHT, stVencChnAttr->stVencAttr.u32VirHeight);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_ROTATION, stVencChnAttr->stVencAttr.enRotation);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_FULL_RANGE, stVencChnAttr->stVencAttr.bFullRange);

    if (g_venc_chns[channel].venc_attr.param.stCropCfg.bEnable) {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, g_venc_chns[channel].venc_attr.param.stCropCfg.stRect.s32X);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, g_venc_chns[channel].venc_attr.param.stCropCfg.stRect.s32Y);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, g_venc_chns[channel].venc_attr.param.stCropCfg.stRect.u32Width);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, g_venc_chns[channel].venc_attr.param.stCropCfg.stRect.u32Height);
    } else {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_X, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_Y, 0);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_W, stVencChnAttr->stVencAttr.u32PicWidth);
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_RECT_H, stVencChnAttr->stVencAttr.u32PicHeight);
    }

    switch (stVencChnAttr->stVencAttr.enType) {
        case DRM_CODEC_TYPE_H264:
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_PROFILE, stVencChnAttr->stVencAttr.u32Profile);
            break;

        case DRM_CODEC_TYPE_H265:
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_SCALING_LIST, stVencChnAttr->stVencAttr.stAttrH265e.bScaleList);

        default:
            break;
    }

    std::string str_fps_in, str_fps;

    switch (stVencChnAttr->stRcAttr.enRcMode) {
        case VENC_RC_MODE_H264CBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_CBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_VIDEO_GOP, stVencChnAttr->stRcAttr.stH264Cbr.u32Gop);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, stVencChnAttr->stRcAttr.stH264Cbr.u32BitRate);
            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stH264Cbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH264Cbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stH264Cbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH264Cbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        case VENC_RC_MODE_H264VBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_VBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_VIDEO_GOP, stVencChnAttr->stRcAttr.stH264Vbr.u32Gop);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, stVencChnAttr->stRcAttr.stH264Vbr.u32MaxBitRate);
            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stH264Vbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH264Vbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        case VENC_RC_MODE_H264AVBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_AVBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_VIDEO_GOP, stVencChnAttr->stRcAttr.stH264Avbr.u32Gop);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, stVencChnAttr->stRcAttr.stH264Avbr.u32MaxBitRate);
            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stH264Avbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH264Avbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        case VENC_RC_MODE_H265CBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_CBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_VIDEO_GOP, stVencChnAttr->stRcAttr.stH265Cbr.u32Gop);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, stVencChnAttr->stRcAttr.stH265Cbr.u32BitRate);
            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stH265Cbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH265Cbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stH265Cbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH265Cbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        case VENC_RC_MODE_H265VBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_VBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_VIDEO_GOP, stVencChnAttr->stRcAttr.stH265Vbr.u32Gop);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, stVencChnAttr->stRcAttr.stH265Vbr.u32MaxBitRate);

            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stH265Vbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH265Vbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stH265Vbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH265Vbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        case VENC_RC_MODE_H265AVBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_AVBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_VIDEO_GOP, stVencChnAttr->stRcAttr.stH265Avbr.u32Gop);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, stVencChnAttr->stRcAttr.stH265Avbr.u32MaxBitRate);

            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stH265Avbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH265Avbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stH265Avbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stH265Avbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        case VENC_RC_MODE_MJPEGCBR:
            PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, DRM_KEY_CBR);
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, stVencChnAttr->stRcAttr.stMjpegCbr.u32BitRate);
            str_fps_in.append(std::to_string(stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

            str_fps.append(std::to_string(stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateNum)).append("/").append(std::to_string(stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateDen));
            PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps);
            break;

        default:
            break;
    }

    // PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_REF_FRM_CFG, stVencChnAttr->stGopAttr.enGopMode);

    flow_param = libdrm::JoinFlowParam(flow_param, 1, enc_param);
    g_venc_chns[channel].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>("video_enc", flow_param.c_str());
    if (!g_venc_chns[channel].media_flow) {
        g_venc_mtx.unlock();
        return -4;
    }

    init_media_channel_buffer(&g_venc_chns[channel]);
    g_venc_chns[channel].media_flow->SetOutputCallBack(&g_venc_chns[channel], flow_output_callback);
    if (pipe2(g_venc_chns[channel].wake_fd, O_CLOEXEC) == -1) {
        g_venc_chns[channel].wake_fd[0] = 0;
        g_venc_chns[channel].wake_fd[1] = 0;

        DRM_MEDIA_LOGW("Create pipe failed");
    }

    g_venc_chns[channel].status = CHN_STATUS_OPEN;
    g_venc_mtx.unlock();
    if (stVencChnAttr->stGopAttr.enGopMode > VENC_GOPMODE_NORMALP) {
        drm_mpi_venc_set_gop_mode(channel, &stVencChnAttr->stGopAttr);
    }

    DRM_MEDIA_LOGI("Enable venc channel:[%d], type:[%d] finished......", channel, stVencChnAttr->stVencAttr.enType);

    return 0;
}

int drm_mpi_venc_set_gop(int channel, uint32_t u32Gop)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_venc_mtx.lock();
    int ret = video_encoder_set_gop_size(g_venc_chns[channel].media_flow, u32Gop);
    if (!ret) {
        g_venc_chns[channel].venc_attr.attr.stGopAttr.u32GopSize = u32Gop;
        switch (g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode) {
            case VENC_RC_MODE_H264CBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.u32Gop = u32Gop;
                break;

            case VENC_RC_MODE_H264VBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.u32Gop = u32Gop;
                break;

            case VENC_RC_MODE_H264AVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.u32Gop = u32Gop;
                break;

            case VENC_RC_MODE_MJPEGCBR:
                break;

            case VENC_RC_MODE_MJPEGVBR:
                break;

            case VENC_RC_MODE_H265CBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.u32Gop = u32Gop;
                break;

            case VENC_RC_MODE_H265VBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.u32Gop = u32Gop;
                break;

            case VENC_RC_MODE_H265AVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.u32Gop = u32Gop;
                break;

            case VENC_RC_MODE_BUTT:
                break;
        }
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_gop_mode(int channel, drm_venc_gop_attr_t *pstGopModeAttr)
{
    if (!pstGopModeAttr) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -3;
    }

    DrmEncGopModeParam media_param;
    memset(&media_param, 0x00, sizeof(media_param));

    switch (pstGopModeAttr->enGopMode) {
        case VENC_GOPMODE_NORMALP:
            media_param.mode = GOP_MODE_NORMALP;
            media_param.ip_qp_delta = pstGopModeAttr->s32IPQpDelta;
            break;

        case VENC_GOPMODE_TSVC:
            media_param.mode = GOP_MODE_TSVC3;
            break;

        case VENC_GOPMODE_SMARTP:
            media_param.mode = GOP_MODE_SMARTP;
            media_param.ip_qp_delta = pstGopModeAttr->s32IPQpDelta;
            media_param.interval = pstGopModeAttr->u32BgInterval;
            media_param.gop_size = pstGopModeAttr->u32GopSize;
            media_param.vi_qp_delta = pstGopModeAttr->s32ViQpDelta;
            break;

        default:
            DRM_MEDIA_LOGE("invalid gop mode:[%d]", pstGopModeAttr->enGopMode);
            return -4;
    }

    g_venc_mtx.lock();
    int ret = libdrm::video_encoder_set_gop_mode(g_venc_chns[channel].media_flow, &media_param);
    if (!ret) {
        memcpy(&g_venc_chns[channel].venc_attr.attr.stGopAttr, pstGopModeAttr, sizeof(drm_venc_gop_attr_t));
    }
    g_venc_mtx.unlock();

    return ret;
}

int drm_mpi_venc_set_bitrate(int channel, uint32_t u32BitRate, uint32_t u32MinBitRate, uint32_t u32MaxBitRate)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_venc_mtx.lock();
    std::shared_ptr<libdrm::Flow> target_flow;

    if (!g_venc_chns[channel].media_flow_list.empty()) {
        target_flow = g_venc_chns[channel].media_flow_list.back();
    } else if (g_venc_chns[channel].media_flow) {
        target_flow = g_venc_chns[channel].media_flow;
    }

    int ret = video_encoder_set_bps(target_flow, u32BitRate, u32MinBitRate, u32MaxBitRate);
    if (!ret) {
        switch (g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode) {
            case VENC_RC_MODE_H264CBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.u32BitRate = u32BitRate;
                break;

            case VENC_RC_MODE_H264VBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.u32MaxBitRate = u32MaxBitRate;
                break;

            case VENC_RC_MODE_H264AVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.u32MaxBitRate = u32MaxBitRate;
                break;

            case VENC_RC_MODE_MJPEGCBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.u32BitRate = u32BitRate;
                break;

            case VENC_RC_MODE_MJPEGVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.u32BitRate = u32MaxBitRate;
                break;

            case VENC_RC_MODE_H265CBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.u32BitRate = u32BitRate;
                break;

            case VENC_RC_MODE_H265VBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.u32MaxBitRate = u32MaxBitRate;
                break;

            case VENC_RC_MODE_H265AVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.u32MaxBitRate = u32MaxBitRate;
                break;

            case VENC_RC_MODE_BUTT:
                break;
        }
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_fps(int channel, uint8_t u8OutNum, uint8_t u8OutDen, uint8_t u8InNum, uint8_t u8InDen)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_venc_mtx.lock();
    int ret = video_encoder_set_fps(g_venc_chns[channel].media_flow, u8OutNum, u8OutDen, u8InNum, u8InDen);
    if (!ret) {
        switch (g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode) {
            case VENC_RC_MODE_H264CBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_H264VBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_H264AVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_MJPEGCBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_MJPEGVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_H265CBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_H265VBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_H265AVBR:
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.u32SrcFrameRateNum = u8InNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.u32SrcFrameRateDen = u8InDen;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.fr32DstFrameRateNum = u8OutNum;
                g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.fr32DstFrameRateDen = u8OutDen;
                break;

            case VENC_RC_MODE_BUTT:
                break;
        }
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_avc_profile(int channel, uint32_t u32Profile, uint32_t u32Level)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType != DRM_CODEC_TYPE_H264) {
        return -3;
    }

    g_venc_mtx.lock();
    int ret = video_encoder_set_avc_profile(g_venc_chns[channel].media_flow, u32Profile, u32Level);
    if (!ret) {
        g_venc_chns[channel].venc_attr.attr.stVencAttr.u32Profile = u32Profile;
        g_venc_chns[channel].venc_attr.attr.stVencAttr.stAttrH264e.u32Level = u32Level;
    }
    g_venc_mtx.unlock();

    return ret;
}

int drm_mpi_venc_set_resolution(int channel, drm_venc_resolution_param_t stResolutionParam)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (g_venc_chns[channel].venc_attr.bFullFunc) {
        return -3;
    }

    g_venc_mtx.lock();
    DrmVideoResolutionCfg vid_cfg;
    vid_cfg.width = stResolutionParam.u32Width;
    vid_cfg.height = stResolutionParam.u32Height;
    vid_cfg.vir_width = stResolutionParam.u32VirWidth;
    vid_cfg.vir_height = stResolutionParam.u32VirHeight;
    vid_cfg.x = 0;
    vid_cfg.y = 0;
    vid_cfg.w = vid_cfg.width;
    vid_cfg.h = vid_cfg.height;

    int ret = video_encoder_set_resolution(g_venc_chns[channel].media_flow, &vid_cfg);
    if (!ret) {
        g_venc_chns[channel].venc_attr.attr.stVencAttr.u32VirWidth = stResolutionParam.u32VirWidth;
        g_venc_chns[channel].venc_attr.attr.stVencAttr.u32VirHeight = stResolutionParam.u32VirHeight;
        g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicWidth = stResolutionParam.u32Width;
        g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicHeight = stResolutionParam.u32Height;
        g_venc_chns[channel].venc_attr.param.stCropCfg.bEnable = false;
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_rotation(int channel, drm_venc_rotation_e rotation_rate)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (g_venc_chns[channel].venc_attr.bFullFunc) {
        return -3;
    }

    g_venc_mtx.lock();
    int ret = video_encoder_set_rotation(g_venc_chns[channel].media_flow, rotation_rate);
    if (!ret) {
        g_venc_chns[channel].venc_attr.attr.stVencAttr.enRotation = rotation_rate;
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_rc_mode(int channel, drm_venc_rc_mode_e RcMode)
{
    int ret = 0;
    const char *key_value = NULL;

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    switch (RcMode) {
        case VENC_RC_MODE_H264CBR:
        case VENC_RC_MODE_MJPEGCBR:
        case VENC_RC_MODE_H265CBR:
            key_value = DRM_KEY_CBR;
            break;

        case VENC_RC_MODE_H264VBR:
        case VENC_RC_MODE_H265VBR:
        case VENC_RC_MODE_MJPEGVBR:
            key_value = DRM_KEY_VBR;
            break;

        case VENC_RC_MODE_H264AVBR:
        case VENC_RC_MODE_H265AVBR:
            key_value = DRM_KEY_AVBR;
            break;

        default:
            break;
    }

    if (!key_value) {
        return -3;
    }

    g_venc_mtx.lock();
    if (g_venc_chns[channel].media_flow) {
        ret = video_encoder_set_rc_mode(g_venc_chns[channel].media_flow, key_value);
        ret = ret ? -4 : 0;
    } else {
        ret = -5;
    }

    if (!ret) {
        g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode = RcMode;
    }
    g_venc_mtx.unlock();

    return ret;
}

int drm_mpi_venc_set_rc_quality(int channel, drm_venc_rc_quality_e RcQuality)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN)
        return -2;

    g_venc_mtx.lock();
    switch (RcQuality) {
        case VENC_RC_QUALITY_HIGHEST:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_HIGHEST);
            break;

        case VENC_RC_QUALITY_HIGHER:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_HIGHER);
            break;

        case VENC_RC_QUALITY_HIGH:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_HIGH);
            break;

        case VENC_RC_QUALITY_MEDIUM:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_MEDIUM);
            break;

        case VENC_RC_QUALITY_LOW:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_LOW);
            break;

        case VENC_RC_QUALITY_LOWER:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_LOWER);
            break;

        case VENC_RC_QUALITY_LOWEST:
            video_encoder_set_rc_quality(g_venc_chns[channel].media_flow, DRM_KEY_LOWEST);
            break;

        default:
            break;
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_request_idr(int channel, bool bInstant)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_venc_mtx.lock();
    video_encoder_force_idr(g_venc_chns[channel].media_flow);
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_get_channel_attribute(int channel, drm_venc_chn_attr_t *stVencChnAttr)
{
    if (!stVencChnAttr) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    g_venc_mtx.lock();
    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        g_venc_mtx.unlock();
        return -3;
    }

    memcpy(stVencChnAttr, &g_venc_chns[channel].venc_attr.attr, sizeof(drm_venc_chn_attr_t));
    g_venc_mtx.unlock();

    return 0;
}

static int venc_set_avc_profile_if_change(int channel, drm_venc_attr_t *stVencAttr)
{
    int ret = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType != DRM_CODEC_TYPE_H264) {
        return ret;
    }

    if (stVencAttr->u32Profile != g_venc_chns[channel].venc_attr.attr.stVencAttr.u32Profile
        || stVencAttr->stAttrH264e.u32Level != g_venc_chns[channel].venc_attr.attr.stVencAttr.stAttrH264e.u32Level)
    {
        ret = drm_mpi_venc_set_avc_profile(channel, stVencAttr->u32Profile, stVencAttr->stAttrH264e.u32Level);
    }

    return ret;
}

static int venc_set_resolution_if_change(int channel, drm_venc_attr_t *stVencAttr)
{
    int ret = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.u32VirWidth != stVencAttr->u32VirWidth
        || g_venc_chns[channel].venc_attr.attr.stVencAttr.u32VirHeight != stVencAttr->u32VirHeight
        || g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicWidth != stVencAttr->u32PicWidth
        || g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicHeight != stVencAttr->u32PicHeight)
    {
        drm_venc_resolution_param_t solution_attr;
        solution_attr.u32VirWidth = stVencAttr->u32VirWidth;
        solution_attr.u32VirHeight = stVencAttr->u32VirHeight;
        solution_attr.u32Width = stVencAttr->u32PicWidth;
        solution_attr.u32Height = stVencAttr->u32PicHeight;

        ret = drm_mpi_venc_set_resolution(channel, solution_attr);
        if (ret) {
            return ret;
        }
    }

    return ret;
}

static int venc_set_rotation_if_change(int channel, drm_venc_attr_t *stVencAttr)
{
    int ret = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enRotation != stVencAttr->enRotation) {
        ret = drm_mpi_venc_set_rotation(channel, stVencAttr->enRotation);
    }

    return ret;
}

static int venc_set_rc_mode_if_change(int channel, drm_venc_rc_attr_t *stRcAttr)
{
    int ret = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_JPEG) {
        return ret;
    }

    if (stRcAttr->enRcMode != g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode) {
        ret = drm_mpi_venc_set_rc_mode(channel, stRcAttr->enRcMode);
    }

    return ret;
}

static int venc_set_bitrate_if_change(int channel, drm_venc_rc_attr_t *stRcAttr)
{
    int ret = 0;
    uint32_t u32BitRate = 0;
    bool bRateChange = false;
    uint32_t u32MinBitRate = 0;
    uint32_t u32MaxBitRate = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_JPEG) {
        return ret;
    }

    switch (g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode) {
        case VENC_RC_MODE_H264CBR:
            u32BitRate = stRcAttr->stH264Cbr.u32BitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_H264VBR:
            u32MaxBitRate = stRcAttr->stH264Vbr.u32MaxBitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_H264AVBR:
            u32MaxBitRate = stRcAttr->stH264Avbr.u32MaxBitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_MJPEGCBR:
            u32BitRate = stRcAttr->stMjpegCbr.u32BitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_MJPEGVBR:
            u32MaxBitRate = stRcAttr->stMjpegVbr.u32BitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_H265CBR:
            u32BitRate = stRcAttr->stH265Cbr.u32BitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_H265VBR:
            u32MaxBitRate = stRcAttr->stH265Vbr.u32MaxBitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_H265AVBR:
            u32MaxBitRate = stRcAttr->stH265Avbr.u32MaxBitRate;
            bRateChange = true;
            break;

        case VENC_RC_MODE_BUTT:
            break;
    }

    if (u32BitRate != 0 || u32MinBitRate != 0 || u32MaxBitRate != 0) {
        ret = drm_mpi_venc_set_bitrate(channel, u32BitRate, u32MinBitRate, u32MaxBitRate);
    } else if (bRateChange) {
        return -1;
    }

    return ret;
}

static int venc_set_fps_if_change(int channel, drm_venc_rc_attr_t *stRcAttr)
{
    int ret = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_JPEG) {
        return ret;
    }

    switch (g_venc_chns[channel].venc_attr.attr.stRcAttr.enRcMode) {
        case VENC_RC_MODE_H264CBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum != stRcAttr->stH264Cbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen != stRcAttr->stH264Cbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum != stRcAttr->stH264Cbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen != stRcAttr->stH264Cbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stH264Cbr.fr32DstFrameRateNum, stRcAttr->stH264Cbr.fr32DstFrameRateDen, stRcAttr->stH264Cbr.u32SrcFrameRateNum, stRcAttr->stH264Cbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_H264VBR:
            if (g_venc_chns[channel] .venc_attr.attr.stRcAttr.stH264Vbr.u32SrcFrameRateNum != stRcAttr->stH264Vbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.u32SrcFrameRateDen != stRcAttr->stH264Vbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.fr32DstFrameRateNum != stRcAttr->stH264Vbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Vbr.fr32DstFrameRateDen != stRcAttr->stH264Vbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stH264Vbr.fr32DstFrameRateNum, stRcAttr->stH264Vbr.fr32DstFrameRateDen, stRcAttr->stH264Vbr.u32SrcFrameRateNum, stRcAttr->stH264Vbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_H264AVBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.u32SrcFrameRateNum != stRcAttr->stH264Avbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.u32SrcFrameRateDen != stRcAttr->stH264Avbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.fr32DstFrameRateNum != stRcAttr->stH264Avbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH264Avbr.fr32DstFrameRateDen != stRcAttr->stH264Avbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stH264Avbr.fr32DstFrameRateNum, stRcAttr->stH264Avbr.fr32DstFrameRateDen, stRcAttr->stH264Avbr.u32SrcFrameRateNum, stRcAttr->stH264Avbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_MJPEGCBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.u32SrcFrameRateNum != stRcAttr->stMjpegCbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.u32SrcFrameRateDen != stRcAttr->stMjpegCbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.fr32DstFrameRateNum != stRcAttr->stMjpegCbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegCbr.fr32DstFrameRateDen != stRcAttr->stMjpegCbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stMjpegCbr.fr32DstFrameRateNum, stRcAttr->stMjpegCbr.fr32DstFrameRateDen, stRcAttr->stMjpegCbr.u32SrcFrameRateNum, stRcAttr->stMjpegCbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_MJPEGVBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.u32SrcFrameRateNum != stRcAttr->stMjpegVbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.u32SrcFrameRateDen != stRcAttr->stMjpegVbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.fr32DstFrameRateNum != stRcAttr->stMjpegVbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stMjpegVbr.fr32DstFrameRateDen != stRcAttr->stMjpegVbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stMjpegVbr.fr32DstFrameRateNum, stRcAttr->stMjpegVbr.fr32DstFrameRateDen, stRcAttr->stMjpegVbr.u32SrcFrameRateNum, stRcAttr->stMjpegVbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_H265CBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.u32SrcFrameRateNum != stRcAttr->stH265Cbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.u32SrcFrameRateDen != stRcAttr->stH265Cbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.fr32DstFrameRateNum != stRcAttr->stH265Cbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Cbr.fr32DstFrameRateDen != stRcAttr->stH265Cbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stH265Cbr.fr32DstFrameRateNum, stRcAttr->stH265Cbr.fr32DstFrameRateDen, stRcAttr->stH265Cbr.u32SrcFrameRateNum, stRcAttr->stH265Cbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_H265VBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.u32SrcFrameRateNum != stRcAttr->stH265Vbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.u32SrcFrameRateDen != stRcAttr->stH265Vbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.fr32DstFrameRateNum != stRcAttr->stH265Vbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Vbr.fr32DstFrameRateDen != stRcAttr->stH265Vbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stH265Vbr.fr32DstFrameRateNum, stRcAttr->stH265Vbr.fr32DstFrameRateDen, stRcAttr->stH265Vbr.u32SrcFrameRateNum, stRcAttr->stH265Vbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_H265AVBR:
            if (g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.u32SrcFrameRateNum != stRcAttr->stH265Avbr.u32SrcFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.u32SrcFrameRateDen != stRcAttr->stH265Avbr.u32SrcFrameRateDen
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.fr32DstFrameRateNum != stRcAttr->stH265Avbr.fr32DstFrameRateNum
                || g_venc_chns[channel].venc_attr.attr.stRcAttr.stH265Avbr.fr32DstFrameRateDen != stRcAttr->stH265Avbr.fr32DstFrameRateDen)
            {
                ret = drm_mpi_venc_set_fps(channel, stRcAttr->stH265Avbr.fr32DstFrameRateNum, stRcAttr->stH265Avbr.fr32DstFrameRateDen, stRcAttr->stH265Avbr.u32SrcFrameRateNum, stRcAttr->stH265Avbr.u32SrcFrameRateDen);
                if (ret) {
                    return ret;
                }
            }
            break;

        case VENC_RC_MODE_BUTT:
            break;
    }

    return ret;
}

static int venc_set_gop_mode_if_change(int channel, drm_venc_gop_attr_t *stGopAttr)
{
    int ret = 0;

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType == DRM_CODEC_TYPE_JPEG) {
        return ret;
    }

    if (stGopAttr->enGopMode != g_venc_chns[channel].venc_attr.attr.stGopAttr.enGopMode
        || stGopAttr->u32GopSize != g_venc_chns[channel].venc_attr.attr.stGopAttr.u32GopSize
        || stGopAttr->s32IPQpDelta != g_venc_chns[channel].venc_attr.attr.stGopAttr.s32IPQpDelta
        || stGopAttr->u32BgInterval != g_venc_chns[channel].venc_attr.attr.stGopAttr.u32BgInterval
        || stGopAttr->s32ViQpDelta != g_venc_chns[channel].venc_attr.attr.stGopAttr.s32ViQpDelta)
    {
        ret = drm_mpi_venc_set_gop_mode(channel, stGopAttr);
    }

    return ret;
}

int drm_mpi_venc_set_channel_attribute(int channel, drm_venc_chn_attr_t *stVencChnAttr)
{
    if (!stVencChnAttr) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    int ret = venc_set_avc_profile_if_change(channel, &stVencChnAttr->stVencAttr);
    if (ret) {
        return ret;
    }

    ret = venc_set_resolution_if_change(channel, &stVencChnAttr->stVencAttr);
    if (ret) {
        return ret;
    }

    ret = venc_set_rotation_if_change(channel, &stVencChnAttr->stVencAttr);
    if (ret) {
        return ret;
    }

    ret = venc_set_rc_mode_if_change(channel, &stVencChnAttr->stRcAttr);
    if (ret) {
        return ret;
    }

    ret = venc_set_bitrate_if_change(channel, &stVencChnAttr->stRcAttr);
    if (ret) {
        return ret;
    }

    ret = venc_set_fps_if_change(channel, &stVencChnAttr->stRcAttr);
    if (ret) {
        return ret;
    }

    ret = venc_set_gop_mode_if_change(channel, &stVencChnAttr->stGopAttr);
    if (ret) {
        return ret;
    }

    return ret;
}

int drm_mpi_venc_create_jpeg_light_channel(int channel, drm_venc_chn_attr_t *stVencChnAttr)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!stVencChnAttr) {
        return -2;
    }

    if ((stVencChnAttr->stVencAttr.enType != DRM_CODEC_TYPE_JPEG) && (stVencChnAttr->stVencAttr.enType != DRM_CODEC_TYPE_MJPEG)) {
        return -3;
    }

    if (g_venc_chns[channel].status != CHN_STATUS_CLOSED) {
        return -4;
    }

    DRM_MEDIA_LOGI("Enable venc channel:[%d], type:[%d] starting......", channel, stVencChnAttr->stVencAttr.enType);

    uint32_t u32InFpsNum = 1;
    uint32_t u32InFpsDen = 1;
    uint32_t u32OutFpsNum = 1;
    uint32_t u32OutFpsDen = 1;
    const char *pcMediaRcMode = nullptr;
    const char *pcMediaCodecType = nullptr;
    std::shared_ptr<libdrm::Flow> video_jpeg_flow;

    int mjpeg_bps = 0;
    int video_width = stVencChnAttr->stVencAttr.u32PicWidth;
    int video_height = stVencChnAttr->stVencAttr.u32PicHeight;
    int vir_width = stVencChnAttr->stVencAttr.u32VirWidth;
    int vir_height = stVencChnAttr->stVencAttr.u32VirHeight;
    std::string pixel_format = ImageTypeToString(stVencChnAttr->stVencAttr.imageType);
    drm_venc_rotation_e enRotation = stVencChnAttr->stVencAttr.enRotation;

    if (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
        if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGCBR) {
            mjpeg_bps = stVencChnAttr->stRcAttr.stMjpegCbr.u32BitRate;
            u32InFpsNum = stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateNum;
            u32InFpsDen = stVencChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRateDen;
            u32OutFpsNum = stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateNum;
            u32OutFpsDen = stVencChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRateDen;
            pcMediaRcMode = DRM_KEY_CBR;
        } else if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGVBR) {
            mjpeg_bps = stVencChnAttr->stRcAttr.stMjpegVbr.u32BitRate;
            u32InFpsNum = stVencChnAttr->stRcAttr.stMjpegVbr.u32SrcFrameRateNum;
            u32InFpsDen = stVencChnAttr->stRcAttr.stMjpegVbr.u32SrcFrameRateDen;
            u32OutFpsNum = stVencChnAttr->stRcAttr.stMjpegVbr.fr32DstFrameRateNum;
            u32OutFpsDen = stVencChnAttr->stRcAttr.stMjpegVbr.fr32DstFrameRateDen;
            pcMediaRcMode = DRM_KEY_VBR;
        } else {
            DRM_MEDIA_LOGE("Invalid RcMode:[%d]", stVencChnAttr->stRcAttr.enRcMode);
            return -5;
        }

        if ((mjpeg_bps < 2000) || (mjpeg_bps > 100000000)) {
            DRM_MEDIA_LOGE("Invalid BitRate:[%d], should be:[2000, 100000000]", mjpeg_bps);
            return -6;
        }

        if (!u32InFpsNum) {
            DRM_MEDIA_LOGE("Invalid src frame rate:[%d/%d]", u32InFpsNum, u32InFpsDen);
            return -7;
        }

        if (!u32OutFpsNum) {
            DRM_MEDIA_LOGE("Invalid dst frame rate:[%d/%d]", u32OutFpsNum, u32OutFpsDen);
            return -8;
        }

        pcMediaCodecType = DRM_VIDEO_MJPEG;
    } else {
        pcMediaCodecType = DRM_IMAGE_JPEG;
    }

    g_venc_mtx.lock();
    if (g_venc_chns[channel].status != CHN_STATUS_CLOSED) {
        g_venc_mtx.unlock();
        return -9;
    }

    memcpy(&g_venc_chns[channel].venc_attr.attr, stVencChnAttr, sizeof(drm_venc_chn_attr_t));

    std::string enc_param = "";
    std::string flow_param = "";
    std::string flow_name = "video_enc";

    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "rkmpp");
    PARAM_STRING_APPEND(flow_param, DRM_KEY_INPUTDATATYPE, DRM_IMAGE_NV12);
    PARAM_STRING_APPEND(flow_param, DRM_KEY_OUTPUTDATATYPE, pcMediaCodecType);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_WIDTH, video_width);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_HEIGHT, video_height);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_WIDTH, vir_width);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_BUFFER_VIR_HEIGHT, vir_height);
    PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_ROTATION, enRotation);

    if (stVencChnAttr->stVencAttr.enType == DRM_CODEC_TYPE_MJPEG) {
        std::string str_fps_in;
        str_fps_in.append(std::to_string(u32InFpsNum)).append("/").append(std::to_string(u32InFpsDen));
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS_IN, str_fps_in);

        std::string str_fps_out;
        str_fps_out.append(std::to_string(u32OutFpsNum)).append("/").append(std::to_string(u32OutFpsDen));
        PARAM_STRING_APPEND(enc_param, DRM_KEY_FPS, str_fps_out);

        if (stVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_MJPEGCBR) {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE, mjpeg_bps);
        } else {
            PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_COMPRESS_BITRATE_MAX, mjpeg_bps);
        }

        PARAM_STRING_APPEND(enc_param, DRM_KEY_COMPRESS_RC_MODE, pcMediaRcMode);
    } else {
        PARAM_STRING_APPEND_TO(enc_param, DRM_KEY_JPEG_QFACTOR, 50);
    }

    flow_param = libdrm::JoinFlowParam(flow_param, 1, enc_param);
    DRM_MEDIA_LOGD("JPEG-LT: Type:[%s] encoder flow param:[%s]", pcMediaCodecType, flow_param.c_str());

    video_jpeg_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!video_jpeg_flow) {
        DRM_MEDIA_LOGE("Create flow:[%s] failed", flow_name.c_str());
        g_venc_mtx.unlock();
        return -10;
    }

    video_jpeg_flow->SetFlowTag("JpegLightEncoder");

    init_media_channel_buffer(&g_venc_chns[channel]);
    video_jpeg_flow->SetOutputCallBack(&g_venc_chns[channel], flow_output_callback);

    g_venc_chns[channel].media_flow = video_jpeg_flow;
    g_venc_chns[channel].media_flow_list.push_back(video_jpeg_flow);

    if (pipe2(g_venc_chns[channel].wake_fd, O_CLOEXEC) == -1) {
        g_venc_chns[channel].wake_fd[0] = 0;
        g_venc_chns[channel].wake_fd[1] = 0;
        DRM_MEDIA_LOGW("Create pipe failed");
    }
    g_venc_chns[channel].status = CHN_STATUS_OPEN;
    g_venc_mtx.unlock();

    g_venc_chns[channel].venc_attr.bFullFunc = false;
    return 0;
}

int drm_mpi_venc_get_channel_parameter(int channel, drm_venc_chn_param_t *stVencChnParam)
{
    if (!stVencChnParam) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    g_venc_mtx.lock();
    memcpy(stVencChnParam, &g_venc_chns[channel].venc_attr.param, sizeof(drm_venc_chn_param_t));
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_channel_parameter(int channel, drm_venc_chn_param_t *stVencChnParam)
{
    if (!stVencChnParam) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    g_venc_mtx.lock();
    if (g_venc_chns[channel].status == CHN_STATUS_CLOSED) {
        g_venc_mtx.unlock();
        return -3;
    }

    memcpy(&g_venc_chns[channel].venc_attr.param, stVencChnParam, sizeof(drm_venc_chn_param_t));

    DrmVideoResolutionCfg vid_cfg;
    std::shared_ptr<libdrm::Flow> flow;
    drm_codec_type_e enCodecType = g_venc_chns[channel].venc_attr.attr.stVencAttr.enType;

    if (((enCodecType == DRM_CODEC_TYPE_MJPEG) || (enCodecType == DRM_CODEC_TYPE_JPEG)) && (g_venc_chns[channel].venc_attr.bFullFunc)) {
        vid_cfg.width = g_venc_chns[channel].venc_attr.attr.stVencAttr.stAttrJpege.u32ZoomWidth;
        vid_cfg.height = g_venc_chns[channel].venc_attr.attr.stVencAttr.stAttrJpege.u32ZoomHeight;
        vid_cfg.vir_width = g_venc_chns[channel].venc_attr.attr.stVencAttr.stAttrJpege.u32ZoomVirWidth;
        vid_cfg.vir_height = g_venc_chns[channel] .venc_attr.attr.stVencAttr.stAttrJpege.u32ZoomVirHeight;
        flow = g_venc_chns[channel].media_flow_list.back();
    } else {
        vid_cfg.width = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicWidth;
        vid_cfg.height = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicHeight;
        vid_cfg.vir_width = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32VirWidth;
        vid_cfg.vir_height = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32VirHeight;
        flow = g_venc_chns[channel].media_flow;
    }

    vid_cfg.x = stVencChnParam->stCropCfg.stRect.s32X;
    vid_cfg.y = stVencChnParam->stCropCfg.stRect.s32Y;
    vid_cfg.w = stVencChnParam->stCropCfg.stRect.u32Width;
    vid_cfg.h = stVencChnParam->stCropCfg.stRect.u32Height;

    int ret = video_encoder_set_resolution(flow, &vid_cfg);
    if (ret) {
        g_venc_mtx.unlock();
        return -4;
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_get_rc_parameter(int channel, drm_venc_rc_param_t *pstRcParam)
{
    if (!pstRcParam) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -3;
    }

    DrmVideoEncoderQp stVencQp;

    g_venc_mtx.lock();
    int ret = video_encoder_get_qp(g_venc_chns[channel].media_flow, stVencQp);
    if (!ret) {
        memcpy(pstRcParam->u32ThrdI, stVencQp.thrd_i, RC_TEXTURE_THR_SIZE * sizeof(uint32_t));
        memcpy(pstRcParam->u32ThrdP, stVencQp.thrd_p, RC_TEXTURE_THR_SIZE * sizeof(uint32_t));
        pstRcParam->u32RowQpDeltaI = stVencQp.row_qp_delta_i;
        pstRcParam->u32RowQpDeltaP = stVencQp.row_qp_delta_p;

        pstRcParam->bEnableHierQp = (bool)stVencQp.hier_qp_en;
        memcpy(pstRcParam->s32HierQpDelta, stVencQp.hier_qp_delta, RC_HEIR_SIZE * sizeof(int));
        memcpy(pstRcParam->s32HierFrameNum, stVencQp.hier_frame_num, RC_HEIR_SIZE * sizeof(int));

        pstRcParam->s32FirstFrameStartQp = stVencQp.qp_init;
        switch (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType) {
            case DRM_CODEC_TYPE_H264:
                pstRcParam->stParamH264.u32MaxQp = stVencQp.qp_max;
                pstRcParam->stParamH264.u32MinQp = stVencQp.qp_min;
                pstRcParam->stParamH264.u32MaxIQp = stVencQp.qp_max_i;
                pstRcParam->stParamH264.u32MinIQp = stVencQp.qp_min_i;
                break;

            case DRM_CODEC_TYPE_H265:
                pstRcParam->stParamH265.u32MaxQp = stVencQp.qp_max;
                pstRcParam->stParamH265.u32MinQp = stVencQp.qp_min;
                pstRcParam->stParamH265.u32MaxIQp = stVencQp.qp_max_i;
                pstRcParam->stParamH265.u32MinIQp = stVencQp.qp_min_i;
                break;

            case DRM_CODEC_TYPE_JPEG:
                break;

            default:
                break;
        }
    } else {
        memcpy(pstRcParam, &g_venc_chns[channel].venc_attr.stRcPara, sizeof(drm_venc_rc_param_t));
    }
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_rc_parameter(int channel, drm_venc_rc_param_t *pstRcParam)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_venc_mtx.lock();
    DrmVideoEncoderQp stVencQp;

    memcpy(stVencQp.thrd_i, pstRcParam->u32ThrdI, RC_TEXTURE_THR_SIZE * sizeof(uint32_t));
    memcpy(stVencQp.thrd_p, pstRcParam->u32ThrdP, RC_TEXTURE_THR_SIZE * sizeof(uint32_t));
    stVencQp.row_qp_delta_i = pstRcParam->u32RowQpDeltaI;
    stVencQp.row_qp_delta_p = pstRcParam->u32RowQpDeltaP;

    stVencQp.hier_qp_en = (bool)pstRcParam->bEnableHierQp;
    memcpy(stVencQp.hier_qp_delta, pstRcParam->s32HierQpDelta, RC_HEIR_SIZE * sizeof(int));
    memcpy(stVencQp.hier_frame_num, pstRcParam->s32HierFrameNum, RC_HEIR_SIZE * sizeof(int));

    stVencQp.qp_init = pstRcParam->s32FirstFrameStartQp;
    switch (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType) {
        case DRM_CODEC_TYPE_H264:
            stVencQp.qp_step = pstRcParam->stParamH264.u32StepQp;
            stVencQp.qp_max = pstRcParam->stParamH264.u32MaxQp;
            stVencQp.qp_min = pstRcParam->stParamH264.u32MinQp;
            stVencQp.qp_max_i = pstRcParam->stParamH264.u32MaxIQp;
            stVencQp.qp_min_i = pstRcParam->stParamH264.u32MinIQp;
            break;

        case DRM_CODEC_TYPE_H265:
            stVencQp.qp_step = pstRcParam->stParamH265.u32StepQp;
            stVencQp.qp_max = pstRcParam->stParamH265.u32MaxQp;
            stVencQp.qp_min = pstRcParam->stParamH265.u32MinQp;
            stVencQp.qp_max_i = pstRcParam->stParamH265.u32MaxIQp;
            stVencQp.qp_min_i = pstRcParam->stParamH265.u32MinIQp;
            break;

        case DRM_CODEC_TYPE_JPEG:
            break;

        default:
            break;
    }

    int ret = video_encoder_set_qp(g_venc_chns[channel].media_flow, stVencQp);
    if (!ret) {
        memcpy(&g_venc_chns[channel].venc_attr.stRcPara, pstRcParam, sizeof(drm_venc_rc_param_t));
    }
    g_venc_mtx.unlock();

    return ret;
}

int drm_mpi_venc_set_jpeg_parameter(int channel, drm_venc_jpeg_param_t *pstJpegParam)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstJpegParam) {
        return -2;
    }

    if (pstJpegParam->u32Qfactor > 99 || !pstJpegParam->u32Qfactor) {
        DRM_MEDIA_LOGE("u32Qfactor:[%d] is invalid, should be:[1, 99]", pstJpegParam->u32Qfactor);
        return -3;
    }

    if ((g_venc_chns[channel].status < CHN_STATUS_OPEN) || g_venc_chns[channel].media_flow_list.empty()) {
        return -4;
    }

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType != DRM_CODEC_TYPE_JPEG) {
        return -5;
    }

    std::shared_ptr<libdrm::Flow> media_flow = g_venc_chns[channel].media_flow_list.back();
    int ret = libdrm::jpeg_encoder_set_qfactor(media_flow, pstJpegParam->u32Qfactor);

    return ret;
}

int drm_mpi_venc_insert_userdata(int channel, uint8_t *pu8Data, uint32_t u32Len)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_venc_mtx.lock();
    video_encoder_set_userdata(g_venc_chns[channel].media_flow, pu8Data, u32Len);
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_get_roi_attribute(int channel, drm_venc_roi_attr_t *pstRoiAttr, int roi_index07)
{
    if (!pstRoiAttr) {
        return -1;
    }

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -2;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -3;
    }

    if ((roi_index07 < 0) || (roi_index07 > 7)) {
        return -4;
    }

    g_venc_mtx.lock();
    memcpy(pstRoiAttr, &g_venc_chns[channel].venc_attr.astRoiAttr[roi_index07], sizeof(drm_venc_roi_attr_t));
    g_venc_mtx.unlock();

    return 0;
}

int drm_mpi_venc_set_roi_attribute(int channel, const drm_venc_roi_attr_t *pstRoiAttr, int region_cnt)
{
    int ret = 0;

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (((pstRoiAttr == nullptr) && (region_cnt > 0)) || (region_cnt > 8)) {
        return -3;
    }

    if (g_venc_chns[channel].venc_attr.attr.stVencAttr.enType != DRM_CODEC_TYPE_H264
        && g_venc_chns[channel].venc_attr.attr.stVencAttr.enType != DRM_CODEC_TYPE_H265)
    {
        return -4;
    }

    int img_width;
    int img_height;
    int x_offset = 0;
    int y_offset = 0;
    int valid_rgn_cnt = 0;
    DrmEncROIRegion regions[region_cnt];

    if ((g_venc_chns[channel].venc_attr.attr.stVencAttr.enRotation == VENC_ROTATION_90)
        || (g_venc_chns[channel].venc_attr.attr.stVencAttr.enRotation == VENC_ROTATION_270))
    {
        img_width = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicHeight;
        img_height = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicWidth;
    } else {
        img_width = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicWidth;
        img_height = g_venc_chns[channel].venc_attr.attr.stVencAttr.u32PicHeight;
    }

    memset(regions, 0x00, sizeof(DrmEncROIRegion) * region_cnt);
    for (int i = 0; i < region_cnt; i++) {
        if (!pstRoiAttr[i].bEnable) {
            continue;
        }

        regions[valid_rgn_cnt].x = pstRoiAttr[i].stRect.s32X;
        regions[valid_rgn_cnt].y = pstRoiAttr[i].stRect.s32Y;
        regions[valid_rgn_cnt].w = pstRoiAttr[i].stRect.u32Width;
        regions[valid_rgn_cnt].h = pstRoiAttr[i].stRect.u32Height;
        regions[valid_rgn_cnt].intra = pstRoiAttr[i].bIntra;
        regions[valid_rgn_cnt].abs_qp_en = pstRoiAttr[i].bAbsQp;
        regions[valid_rgn_cnt].qp_area_idx = pstRoiAttr[i].u32Index;
        regions[valid_rgn_cnt].quality = pstRoiAttr[i].s32Qp;
        regions[valid_rgn_cnt].area_map_en = 1;

        x_offset = pstRoiAttr[i].stRect.s32X + pstRoiAttr[i].stRect.u32Width;
        y_offset = pstRoiAttr[i].stRect.s32Y + pstRoiAttr[i].stRect.u32Height;
        if ((x_offset > img_width) || (y_offset > img_height)) {
            return -5;
        }

        valid_rgn_cnt++;
    }

    g_venc_mtx.lock();
    ret = video_encoder_set_roi_regions(g_venc_chns[channel].media_flow, regions, valid_rgn_cnt);
    if (!ret) {
        for (int i = 0; i < region_cnt; i++) {
            memcpy(&g_venc_chns[channel].venc_attr.astRoiAttr[i], &pstRoiAttr[i], sizeof(drm_venc_roi_attr_t));
        }
    }
    g_venc_mtx.unlock();

    return ret ? -6 : 0;
}

int drm_mpi_venc_get_fd(int channel)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    int rcv_fd = 0;

    g_venc_mtx.lock();
    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        g_venc_mtx.unlock();
        return -2;
    }
    rcv_fd = g_venc_chns[channel].wake_fd[0];
    g_venc_mtx.unlock();

    return rcv_fd;
}

int drm_mpi_venc_query_status(int channel, drm_venc_chn_status_t *pstStatus)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstStatus) {
        return -2;
    }

    g_venc_mtx.lock();
    if ((g_venc_chns[channel].status < CHN_STATUS_OPEN) || (!g_venc_chns[channel].media_flow)) {
        g_venc_mtx.unlock();
        return -3;
    }

    uint32_t u32BufferUsedCnt = 0;
    uint32_t u32BufferTotalCnt = 0;

    g_venc_chns[channel].media_flow->GetCachedBufferNum(u32BufferTotalCnt, u32BufferUsedCnt);
    g_venc_mtx.unlock();

    pstStatus->u32LeftFrames = u32BufferUsedCnt;
    pstStatus->u32TotalFrames = u32BufferTotalCnt;

    g_venc_chns[channel].buffer_list_mtx.lock();
    pstStatus->u32TotalPackets = g_venc_chns[channel].buffer_depth;
    pstStatus->u32LeftPackets = g_venc_chns[channel].buffer_list.size();
    g_venc_chns[channel].buffer_list_mtx.unlock();

    return 0;
}

int drm_mpi_venc_get_super_frame_strategy(int channel, drm_venc_superframe_cfg_t *pstSuperFrmParam)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstSuperFrmParam) {
        return -2;
    }

    if (g_venc_chns[channel].status != CHN_STATUS_OPEN) {
        return -3;
    }

    if (!g_venc_chns[channel].media_flow) {
        return -4;
    }

    // TODO:

    return 0;
}

int drm_mpi_venc_set_super_frame_strategy(int channel, const drm_venc_superframe_cfg_t *pstSuperFrmParam)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstSuperFrmParam) {
        return -2;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -3;
    }

    if (!g_venc_chns[channel].media_flow) {
        return -4;
    }

    DrmVencSuperFrmCfg media_super_frm_cfg;
    switch (pstSuperFrmParam->enSuperFrmMode) {
        case SUPERFRM_NONE:
            media_super_frm_cfg.SuperFrmMode = DRM_MEDIA_SUPERFRM_NONE;
            break;

        case SUPERFRM_DISCARD:
            media_super_frm_cfg.SuperFrmMode = DRM_MEDIA_SUPERFRM_DISCARD;
            break;

        case SUPERFRM_REENCODE:
            media_super_frm_cfg.SuperFrmMode = DRM_MEDIA_SUPERFRM_REENCODE;
            break;

        default:
            return -5;
    }

    switch (pstSuperFrmParam->enRcPriority) {
        case VENC_RC_PRIORITY_BITRATE_FIRST:
            media_super_frm_cfg.RcPriority = DRM_MEDIA_VENC_RC_PRIORITY_BITRATE_FIRST;
            break;

        case VENC_RC_PRIORITY_FRAMEBITS_FIRST:
            media_super_frm_cfg.RcPriority = DRM_MEDIA_VENC_RC_PRIORITY_FRAMEBITS_FIRST;
            break;

        default:
            return -6;
    }

    media_super_frm_cfg.SuperIFrmBitsThr = pstSuperFrmParam->u32SuperIFrmBitsThr;
    media_super_frm_cfg.SuperPFrmBitsThr = pstSuperFrmParam->u32SuperPFrmBitsThr;

    int ret = libdrm::video_encoder_set_super_frame(g_venc_chns[channel].media_flow, &media_super_frm_cfg);
    if (ret) {
        ret = -7;
    }

    return 0;
}

int drm_mpi_venc_start_recv_frame(int channel, const drm_recv_pic_param_t *pstRecvParam)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRecvParam) {
        return -2;
    }

    if (!g_venc_chns[channel].media_flow) {
        return -3;
    }

    g_venc_chns[channel].media_flow->SetRunTimes(pstRecvParam->s32RecvPicNum);
    return 0;
}

int drm_mpi_venc_region_set_palette_id(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_osd_color_palette_buf_t *pstColPalBuf)
{
    int ret = 0;

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if ((g_venc_chns[channel].status < CHN_STATUS_OPEN) || (g_venc_chns[channel].bColorTblInit == false)) {
        return -2;
    }

    drm_codec_type_e enCodecType = g_venc_chns[channel].venc_attr.attr.stVencAttr.enType;
    if (((enCodecType == DRM_CODEC_TYPE_MJPEG) || (enCodecType == DRM_CODEC_TYPE_JPEG)) && (!g_venc_chns[channel].venc_attr.bFullFunc)) {
        DRM_MEDIA_LOGE("Jpeg lt don't support SetPaletteId");
        return -3;
    }

    if (pstRgnInfo && !pstRgnInfo->u8Enable) {
        DrmOsdRegionData media_osd_rgn;

        memset(&media_osd_rgn, 0x00, sizeof(media_osd_rgn));
        media_osd_rgn.region_id = pstRgnInfo->enRegionId;
        media_osd_rgn.enable = pstRgnInfo->u8Enable;
        media_osd_rgn.region_type = REGION_TYPE_OVERLAY;

        ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn);
        if (ret) {
            ret = -4;
        }

        return ret;
    }

    if (!pstColPalBuf || !pstColPalBuf->pIdBuf) {
        return -5;
    }

    if (!pstRgnInfo || !pstRgnInfo->u32Width || !pstRgnInfo->u32Height) {
        return -6;
    }

    if ((pstRgnInfo->u32PosX % 16) || (pstRgnInfo->u32PosY % 16) || (pstRgnInfo->u32Width % 16) || (pstRgnInfo->u32Height % 16)) {
        DRM_MEDIA_LOGE("<x, y, w, h> = <%d, %d, %d, %d> must be 16 aligned", pstRgnInfo->u32PosX, pstRgnInfo->u32PosY, pstRgnInfo->u32Width, pstRgnInfo->u32Height);
        return -7;
    }

    if ((pstRgnInfo->u32Width != pstColPalBuf->u32Width) || (pstRgnInfo->u32Height != pstColPalBuf->u32Height)) {
        DRM_MEDIA_LOGE("RgnInfo:[%dx%d] and ColorPaletteBuf:[%dx%d] not equal", pstRgnInfo->u32Width, pstRgnInfo->u32Height, pstColPalBuf->u32Width, pstColPalBuf->u32Height);
        return -8;
    }

    DrmOsdRegionData media_osd_rgn;

    media_osd_rgn.buffer = (uint8_t *)pstColPalBuf->pIdBuf;
    media_osd_rgn.region_id = pstRgnInfo->enRegionId;
    media_osd_rgn.pos_x = pstRgnInfo->u32PosX;
    media_osd_rgn.pos_y = pstRgnInfo->u32PosY;
    media_osd_rgn.width = pstRgnInfo->u32Width;
    media_osd_rgn.height = pstRgnInfo->u32Height;
    media_osd_rgn.inverse = pstRgnInfo->u8Inverse;
    media_osd_rgn.enable = pstRgnInfo->u8Enable;
    media_osd_rgn.region_type = REGION_TYPE_OVERLAY;

    ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn);
    if (ret) {
        ret = -9;
    }

    return ret;
}

int drm_mpi_venc_region_init(int channel, drm_venc_color_tbl_t *stColorTbl)
{
    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("venc should be opened before init osd");
        return -2;
    }

    drm_codec_type_e enCodecType = g_venc_chns[channel].venc_attr.attr.stVencAttr.enType;
    if (((enCodecType == DRM_CODEC_TYPE_MJPEG) || (enCodecType == DRM_CODEC_TYPE_JPEG))) {
        g_venc_mtx.lock();
        g_venc_chns[channel].bColorTblInit = true;
        g_venc_mtx.unlock();

        return 0;
    }

    int ret = 0;
    const uint32_t *pu32ArgbColorTbl = NULL;
    uint32_t u32AVUYColorTbl[VENC_RGN_COLOR_NUM] = {0};

    if (stColorTbl) {
        if (stColorTbl->bColorDichotomyEnable) {
            DRM_MEDIA_LOGI("venc channel:[%d] user define color table(Dichotomy:[true])", channel);

            std::sort(stColorTbl->u32ArgbTbl, stColorTbl->u32ArgbTbl + VENC_RGN_COLOR_NUM);
            g_venc_chns[channel].bColorDichotomyEnable = true;
        } else {
            DRM_MEDIA_LOGI("venc channel:[%d] user define color table(Dichotomy:[false])", channel);
            g_venc_chns[channel].bColorDichotomyEnable = false;
        }

        pu32ArgbColorTbl = stColorTbl->u32ArgbTbl;
    } else {
        DRM_MEDIA_LOGI("venc channel:[%d] default color table(Dichotomy:[true])", channel);

        g_venc_chns[channel].bColorDichotomyEnable = true;
        pu32ArgbColorTbl = u32DftARGB8888ColorTbl;
    }

    color_tbl_argb_to_avuy(pu32ArgbColorTbl, u32AVUYColorTbl);

    g_venc_mtx.lock();
    ret = libdrm::video_encoder_set_osd_plt(g_venc_chns[channel].media_flow, u32AVUYColorTbl);
    if (ret) {
        g_venc_mtx.unlock();
        return -3;
    }

    memcpy(g_venc_chns[channel].u32ArgbColorTbl, pu32ArgbColorTbl, VENC_RGN_COLOR_NUM * 4);
    g_venc_chns[channel].bColorTblInit = true;
    g_venc_mtx.unlock();

    return 0;
}

static void argb8888_to_region_data(int channel, const drm_bitmap_t *pstBitmap, uint8_t *data, uint32_t canvasWidth, uint32_t canvasHeight)
{
    uint32_t ColorValue;
    uint8_t *CanvasLineStart;
    uint8_t TransColorId = 0;
    uint32_t *BitmapLineStart;
    uint32_t TransColor = 0x00000000;
    uint32_t TargetWidth, TargetHeight;

    TargetWidth = (pstBitmap->u32Width > canvasWidth) ? canvasWidth : pstBitmap->u32Width;
    TargetHeight = (pstBitmap->u32Height > canvasHeight) ? canvasHeight : pstBitmap->u32Height;

    DRM_MEDIA_LOGD("bitmap:[%d, %d] -> canvas:[%d, %d], target:[%d, %d]", pstBitmap->u32Width, pstBitmap->u32Height, canvasWidth, canvasHeight, TargetWidth, TargetHeight);

    memset(data, TransColorId, canvasWidth * canvasHeight);

    for (uint32_t i = 0; i < TargetHeight; i++) {
        CanvasLineStart = data + i * canvasWidth;
        BitmapLineStart = (uint32_t *)pstBitmap->pData + i * pstBitmap->u32Width;

        for (uint32_t j = 0; j < TargetWidth; j++) {
            ColorValue = *(BitmapLineStart + j);

            if (ColorValue == TransColor) {
                continue;
            }

            if (g_venc_chns[channel].bColorDichotomyEnable) {
                *(CanvasLineStart + j) = find_argb_color_tbl_by_dichotomy(g_venc_chns[channel].u32ArgbColorTbl, PALETTE_TABLE_LEN, ColorValue);
            } else {
                *(CanvasLineStart + j) = find_argb_color_tbl_by_order(g_venc_chns[channel].u32ArgbColorTbl, PALETTE_TABLE_LEN, ColorValue);
            }
        }
    }
}

int drm_mpi_venc_region_set_bitmap(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_bitmap_t *pstBitmap)
{
    int ret = 0;
    uint8_t *media_osd_data = NULL;

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    g_venc_mtx.lock();
    if ((g_venc_chns[channel].status < CHN_STATUS_OPEN) || (g_venc_chns[channel].bColorTblInit == false)) {
        g_venc_mtx.unlock();
        return -2;
    } else {
        g_venc_chns[channel].chn_ref_cnt++;
    }
    g_venc_mtx.unlock();

    uint8_t u8Align = 16;
    uint8_t u8PlaneCnt = 1;
    bool bIsJpegLight = false;
    drm_region_type_e region_type = REGION_TYPE_OVERLAY;
    drm_codec_type_e enCodecType = g_venc_chns[channel].venc_attr.attr.stVencAttr.enType;

    if (((enCodecType == DRM_CODEC_TYPE_MJPEG) || (enCodecType == DRM_CODEC_TYPE_JPEG))) {
        bIsJpegLight = true;
        u8Align = 2;
        u8PlaneCnt = 4;
        region_type = REGION_TYPE_OVERLAY_EX;
    }

    if (pstRgnInfo && !pstRgnInfo->u8Enable) {
        DrmOsdRegionData media_osd_rgn;

        memset(&media_osd_rgn, 0x00, sizeof(media_osd_rgn));
        media_osd_rgn.region_id = pstRgnInfo->enRegionId;
        media_osd_rgn.enable = pstRgnInfo->u8Enable;
        media_osd_rgn.region_type = region_type;

        if (g_venc_chns[channel].venc_attr.bFullFunc) {
            ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow_list.back(), &media_osd_rgn, u8PlaneCnt);
        } else {
            ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn);
        }

        if (ret) {
            ret = -3;
        }

        goto deref;
    }

    if (!pstBitmap || !pstBitmap->pData || !pstBitmap->u32Width || !pstBitmap->u32Height) {
        ret = -4;
        goto deref;
    }

    if (!pstRgnInfo || !pstRgnInfo->u32Width || !pstRgnInfo->u32Height) {
        ret = -5;
        goto deref;
    }

    if ((pstRgnInfo->u32PosX % u8Align) || (pstRgnInfo->u32PosY % u8Align) || (pstRgnInfo->u32Width % u8Align) || (pstRgnInfo->u32Height % u8Align)) {
        DRM_MEDIA_LOGE("<x, y, w, h> = <%d, %d, %d, %d> must be %u aligned", pstRgnInfo->u32PosX, pstRgnInfo->u32PosY, pstRgnInfo->u32Width, pstRgnInfo->u32Height, u8Align);

        ret = -6;
        goto deref;
    }

    if (!bIsJpegLight) {
        uint32_t total_pix_num = pstRgnInfo->u32Width * pstRgnInfo->u32Height;
        media_osd_data = (uint8_t *)malloc(total_pix_num);
        if (!media_osd_data) {
            DRM_MEDIA_LOGE("no space left, RgnInfo pixels:[%d]", total_pix_num);

            ret = -7;
            goto deref;
        }

        switch (pstBitmap->enPixelFormat) {
            case PIXEL_FORMAT_ARGB_8888:
                argb8888_to_region_data(channel, pstBitmap, media_osd_data, pstRgnInfo->u32Width, pstRgnInfo->u32Height);
                break;

            default:
                DRM_MEDIA_LOGE("not support bitmap pixel format:[%d]", pstBitmap->enPixelFormat);
                ret = -8;
                goto deref;
        }
    } else {
        if ((pstBitmap->u32Width != pstRgnInfo->u32Width) || (pstBitmap->u32Height != pstRgnInfo->u32Height)) {
            DRM_MEDIA_LOGE("JpegLight:RgnInfo:[%ux%u] should be equal to BitMap:[%ux%u]", pstRgnInfo->u32Width, pstRgnInfo->u32Height, pstBitmap->u32Width, pstBitmap->u32Height);
            ret = -9;
            goto deref;
        }

        media_osd_data = (uint8_t *)pstBitmap->pData;
    }

    DrmOsdRegionData media_osd_rgn;
    media_osd_rgn.buffer = media_osd_data;
    media_osd_rgn.region_id = pstRgnInfo->enRegionId;
    media_osd_rgn.pos_x = pstRgnInfo->u32PosX;
    media_osd_rgn.pos_y = pstRgnInfo->u32PosY;
    media_osd_rgn.width = pstRgnInfo->u32Width;
    media_osd_rgn.height = pstRgnInfo->u32Height;
    media_osd_rgn.inverse = pstRgnInfo->u8Inverse;
    media_osd_rgn.enable = pstRgnInfo->u8Enable;
    media_osd_rgn.region_type = region_type;

    if (g_venc_chns[channel].venc_attr.bFullFunc) {
        ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow_list.back(), &media_osd_rgn, u8PlaneCnt);
    } else {
        ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn, u8PlaneCnt);
    }

    if (ret) {
        ret = -10;
    }

    if (!bIsJpegLight && media_osd_data) {
        free(media_osd_data);
    }

deref:
    g_venc_mtx.lock();
    g_venc_chns[channel].chn_ref_cnt--;
    g_venc_mtx.unlock();

    return ret;
}

int drm_mpi_venc_region_set_cover(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_cover_info_t *pstCoverInfo)
{
    int ret = 0;
    uint8_t color_id = 0xFF;
    uint32_t total_pix_num = 0;
    uint8_t *media_cover_data = NULL;

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if ((g_venc_chns[channel].status < CHN_STATUS_OPEN) || (g_venc_chns[channel].bColorTblInit == false)) {
        return -2;
    }

    uint8_t u8Align = 16;
    uint8_t u8PlaneCnt = 1;
    bool bIsJpegLight = false;
    drm_region_type_e region_type = REGION_TYPE_OVERLAY;
    drm_codec_type_e enCodecType = g_venc_chns[channel].venc_attr.attr.stVencAttr.enType;

    if (((enCodecType == DRM_CODEC_TYPE_MJPEG) || (enCodecType == DRM_CODEC_TYPE_JPEG)) && (!g_venc_chns[channel].venc_attr.bFullFunc)) {
        bIsJpegLight = true;
        u8Align = 2;
        u8PlaneCnt = 0;
        region_type = REGION_TYPE_COVER_EX;
    }

    if (pstRgnInfo && !pstRgnInfo->u8Enable) {
        DrmOsdRegionData media_osd_rgn;

        memset(&media_osd_rgn, 0x00, sizeof(media_osd_rgn));
        media_osd_rgn.region_id = pstRgnInfo->enRegionId;
        media_osd_rgn.enable = pstRgnInfo->u8Enable;
        media_osd_rgn.region_type = region_type;

        ret = libdrm::video_encoder_set_osd_region( g_venc_chns[channel].media_flow, &media_osd_rgn);
        if (ret) {
            ret = -3;
        }

        return ret;
    }

    if (!pstCoverInfo) {
        return -4;
    }

    if (!pstRgnInfo || !pstRgnInfo->u32Width || !pstRgnInfo->u32Height) {
        return -5;
    }

    if ((pstRgnInfo->u32PosX % u8Align) || (pstRgnInfo->u32PosY % u8Align) || (pstRgnInfo->u32Width % u8Align) || (pstRgnInfo->u32Height % u8Align)) {
        DRM_MEDIA_LOGE("<x, y, w, h> = <%d, %d, %d, %d> must be %d aligned", pstRgnInfo->u32PosX, pstRgnInfo->u32PosY, pstRgnInfo->u32Width, pstRgnInfo->u32Height, u8Align);
        return -6;
    }

    if (pstCoverInfo->enPixelFormat != PIXEL_FORMAT_ARGB_8888) {
        DRM_MEDIA_LOGE("not support cover pixel format:[%d]", pstCoverInfo->enPixelFormat);
        return -7;
    }

    if (!bIsJpegLight) {
        total_pix_num = pstRgnInfo->u32Width * pstRgnInfo->u32Height;
        media_cover_data = (uint8_t *)malloc(total_pix_num);
        if (!media_cover_data) {
            DRM_MEDIA_LOGE("no space left, RgnInfo pixels:[%d]", total_pix_num);
            return -8;
        }

        color_id = find_argb_color_tbl_by_order(g_venc_chns[channel].u32ArgbColorTbl, PALETTE_TABLE_LEN, pstCoverInfo->u32Color);
        memset(media_cover_data, color_id, total_pix_num);
    }

    DrmOsdRegionData media_osd_rgn;
    media_osd_rgn.buffer = media_cover_data;
    media_osd_rgn.region_id = pstRgnInfo->enRegionId;
    media_osd_rgn.pos_x = pstRgnInfo->u32PosX;
    media_osd_rgn.pos_y = pstRgnInfo->u32PosY;
    media_osd_rgn.width = pstRgnInfo->u32Width;
    media_osd_rgn.height = pstRgnInfo->u32Height;
    media_osd_rgn.inverse = pstRgnInfo->u8Inverse;
    media_osd_rgn.enable = pstRgnInfo->u8Enable;
    media_osd_rgn.region_type = region_type;
    media_osd_rgn.cover_color = pstCoverInfo->u32Color;

    ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn, u8PlaneCnt);
    if (ret) {
        ret = -9;
    }

    if (media_cover_data) {
        free(media_cover_data);
    }

    return ret;
}

int drm_mpi_venc_region_set_coverEx(int channel, const drm_osd_region_info_t *pstRgnInfo, const drm_cover_info_t *pstCoverInfo)
{
    int ret = 0;

    if ((channel < DRM_VENC_CHANNEL_00) || (channel >= DRM_VENC_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_venc_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (pstRgnInfo && !pstRgnInfo->u8Enable) {
        DrmOsdRegionData media_osd_rgn;

        memset(&media_osd_rgn, 0x00, sizeof(media_osd_rgn));
        media_osd_rgn.region_id = pstRgnInfo->enRegionId;
        media_osd_rgn.enable = pstRgnInfo->u8Enable;
        media_osd_rgn.region_type = REGION_TYPE_COVER_EX;

        ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn);
        if (ret) {
            ret = -3;
        }

        return ret;
    }

    if (!pstCoverInfo) {
        return -4;
    }

    if (!pstRgnInfo || !pstRgnInfo->u32Width || !pstRgnInfo->u32Height) {
        return -5;
    }

    uint8_t u8Align = 2;
    uint8_t u8PlaneCnt = 0;

    if ((pstRgnInfo->u32PosX % u8Align) || (pstRgnInfo->u32PosY % u8Align) || (pstRgnInfo->u32Width % u8Align) || (pstRgnInfo->u32Height % u8Align)) {
        DRM_MEDIA_LOGE("<x, y, w, h> = <%d, %d, %d, %d> must be %d aligned", pstRgnInfo->u32PosX, pstRgnInfo->u32PosY, pstRgnInfo->u32Width, pstRgnInfo->u32Height, u8Align);
        return -6;
    }

    if (pstCoverInfo->enPixelFormat != PIXEL_FORMAT_ARGB_8888) {
        DRM_MEDIA_LOGE("Not support cover pixel format:[%d]", pstCoverInfo->enPixelFormat);
        return -7;
    }

    DrmOsdRegionData media_osd_rgn;

    media_osd_rgn.buffer = NULL;
    media_osd_rgn.region_id = pstRgnInfo->enRegionId;
    media_osd_rgn.pos_x = pstRgnInfo->u32PosX;
    media_osd_rgn.pos_y = pstRgnInfo->u32PosY;
    media_osd_rgn.width = pstRgnInfo->u32Width;
    media_osd_rgn.height = pstRgnInfo->u32Height;
    media_osd_rgn.inverse = pstRgnInfo->u8Inverse;
    media_osd_rgn.enable = pstRgnInfo->u8Enable;
    media_osd_rgn.region_type = REGION_TYPE_COVER_EX;
    media_osd_rgn.cover_color = pstCoverInfo->u32Color;

    ret = libdrm::video_encoder_set_osd_region(g_venc_chns[channel].media_flow, &media_osd_rgn, u8PlaneCnt);
    if (ret) {
        ret = -8;
    }

    return ret;
}

#ifdef __cplusplus
}
#endif
