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
#include <rkrga/rga.h>
#include <rkrga/im2d.h>
#endif

#include <libdrm/drm_media_vi.h>
#include <libdrm/drm_media_vo.h>
#include <libdrm/drm_media_rga.h>
#include <libdrm/drm_media_vmix.h>

#include <libdrm/drm_media_api.h>
#include <libdrm/display/utils.h>
#include <libdrm/display/image.h>
#include <libdrm/display/stream.h>
#include <libdrm/display/control.h>
#include <libdrm/display/encoder.h>
#include <libdrm/display/message.h>
#include <libdrm/drm_media_buffer.h>
#include <libdrm/display/key_string.h>
#include <libdrm/display/media_type.h>
#include <libdrm/display/media_config.h>

#include "media_utils.h"
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
        drm_vo_chn_attr_t                    vo_attr;
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
    std::shared_ptr<libdrm::MediaBuffer>     luma_rkmedia_buf;
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
} video_mix_device_t;

static std::mutex g_vi_mtx;
static std::mutex g_vo_mtx;
static std::mutex g_rga_mtx;

static drm_media_channel_t g_vi_chns[DRM_VI_CHANNEL_BUTT];
static drm_media_channel_t g_vo_chns[DRM_VO_CHANNEL_BUTT];
static video_mix_device_t g_vmix_dev[DRM_VMIX_DEVICE_BUTT];
static drm_media_channel_t g_rga_chns[DRM_RGA_CHANNEL_BUTT];

static unsigned char g_sys_init = 0;

extern std::mutex g_handle_mb_mutex;
extern std::list<handle_mb_t *> g_handle_mb;

static const char *mod_tag_list[MOD_ID_BUTT] = {
    "UNKNOW", "VB", "SYS", "VO", "VI", "RGA", "VMIX",
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
        {DRM_PIX_FMT_ABGR8888, RK_FORMAT_RGBA_8888}
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
        case MOD_ID_RGA:
        case MOD_ID_VMIX:
            type = MB_TYPE_IMAGE;
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
            target_chn->luma_rkmedia_buf = media_mb;
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
        mb->flag = (media_mb->GetUserFlag() & libdrm::MediaBuffer::kIntra) ? 4 : 2;
        mb->tsvc_level = media_mb->GetTsvcLevel();
    } else {
        mb->flag = 0;
        mb->tsvc_level = 0;
    }

    if (mb_type == MB_TYPE_IMAGE) {
        libdrm::ImageBuffer *rkmedia_ib = static_cast<libdrm::ImageBuffer *>(media_mb.get());
        mb->stImageInfo.u32Width = rkmedia_ib->GetWidth();
        mb->stImageInfo.u32Height = rkmedia_ib->GetHeight();
        mb->stImageInfo.u32HorStride = rkmedia_ib->GetVirWidth();
        mb->stImageInfo.u32VerStride = rkmedia_ib->GetVirHeight();
        std::string strPixFmt = PixFmtToString(rkmedia_ib->GetPixelFormat());
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

    for (int i = 0; i < MOD_ID_BUTT; i++) {
        g_level_list[i] = 3;
    }

    reset_channel_table(g_vi_chns, DRM_VI_CHANNEL_BUTT, MOD_ID_VI);
    reset_channel_table(g_vo_chns, DRM_VO_CHANNEL_BUTT, MOD_ID_VO);
    reset_channel_table(g_rga_chns, DRM_RGA_CHANNEL_BUTT, MOD_ID_RGA);

    for (int i = 0; i < DRM_VMIX_DEVICE_BUTT; i++) {
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
                DRM_MEDIA_LOGD("disable rkmedia flow output callback");

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
            if ((s32ChnId < 0) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid channel id:[%d]", s32ChnId);
                return -3;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                return -4;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -5;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("channel:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -5;
    }

    int ret = 0;
    ret = target_chn->media_flow->SetInputFpsControl(s32FpsInRatio, s32FpsOutRatio);
    target_mutex->unlock();
    if (ret) {
        DRM_MEDIA_LOGE("SetInputFpsControl failed, return:[%d]", ret);
        return -6;
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
            if ((s32ChnId < 0) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid channel id:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                return -3;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -4;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("channel:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -5;
    }

    int ret = target_chn->media_flow->SetRunTimes(pstRecvParam->s32RecvPicNum);
    target_mutex->unlock();
    if (ret != pstRecvParam->s32RecvPicNum) {
        DRM_MEDIA_LOGE("SetRunTimes failed, return:[%d]", ret);
        return -6;
    }

    return 0;
}

int drm_mpi_system_set_media_buffer_depth(mod_id_e enModID, int s32ChnId, int depth)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VI:
            if (s32ChnId < 0 || s32ChnId >= DRM_VI_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid vi channel:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vi_chns[s32ChnId];
            target_mutex = &g_vi_mtx;
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid RGA channe:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX: {
            int s32DevId = s32ChnId;
            if (s32DevId < 0 || s32DevId >= DRM_VMIX_DEVICE_BUTT) {
                return -3;
            }
            target_chn = &g_vmix_dev[s32DevId].VmixChns[0];
            target_mutex = &g_vmix_dev[s32DevId].VmixMtx;
            break;
        }

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -4;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -5;
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
            if ((s32ChnId < 0) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                DRM_MEDIA_LOGE("invalid channel id:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                return -2;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -3;
    }

    media_buffer_impl_t *mb = (media_buffer_impl_t *)buffer;
    target_mutex->lock();
    if (target_chn->media_flow) {
        target_chn->media_flow->SendInput(mb->media_mb, 0);
    } else {
        target_mutex->unlock();
        DRM_MEDIA_LOGE("media flow null");
        return -4;
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
            if (s32ChnId < 0 || s32ChnId >= DRM_VMIX_CHANNEL_BUTT || s32DevId < 0 || s32DevId >= DRM_VMIX_DEVICE_BUTT) {
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
            if (s32ChnId < 0 || s32ChnId >= DRM_VI_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid VI channel:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vi_chns[s32ChnId];
            target_mutex = &g_vi_mtx;
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid RGA channel:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -3;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -4;
    }

    clear_media_channel_buffer(target_chn);

    target_mutex->lock();
    target_chn->media_out_cb_status = CHN_OUT_CB_CLOSE;
    target_chn->media_flow->SetOutputCallBack(NULL, NULL);
    DRM_MEDIA_LOGD("disable rkmedia output callback");
    target_mutex->unlock();

    return 0;
}

int drm_mpi_system_start_get_media_buffer(mod_id_e enModID, int s32ChnId)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VI:
            if (s32ChnId < 0 || s32ChnId >= DRM_VI_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid VI channel:[%d]", s32ChnId);
                return -1;
            }
            target_chn = &g_vi_chns[s32ChnId];
            target_mutex = &g_vi_mtx;
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid RGA channel:[%d]", s32ChnId);
                return -2;
            }
            target_chn = &g_rga_chns[s32ChnId];
            target_mutex = &g_rga_mtx;
            break;

        case MOD_ID_VMIX: {
            int s32DevId = s32ChnId;
            if (s32DevId < 0 || s32DevId >= DRM_VMIX_DEVICE_BUTT) {
                return -3;
            }
            target_chn = &g_vmix_dev[s32DevId].VmixChns[0];
            target_mutex = &g_vmix_dev[s32DevId].VmixMtx;
            break;
        }

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -4;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -5;
    }

    init_media_channel_buffer(target_chn);

    target_mutex->lock();
    if (target_chn->media_out_cb_status == CHN_OUT_CB_USER) {
        target_mutex->unlock();
        return 0;
    } else if (target_chn->media_out_cb_status == CHN_OUT_CB_CLOSE) {
        DRM_MEDIA_LOGD("enable rkmedia output callback");
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
            if (s32ChnId < 0 || s32ChnId >= DRM_VO_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid vi channel:[%d]", s32ChnId);
                return NULL;
            }
            target_chn = &g_vi_chns[s32ChnId];
            break;

        case MOD_ID_RGA:
            if (s32ChnId < 0 || s32ChnId >= DRM_RGA_CHANNEL_BUTT) {
                DRM_MEDIA_LOGE("invalid RGA channel:[%d]", s32ChnId);
                return NULL;
            }
            target_chn = &g_rga_chns[s32ChnId];
            break;

        case MOD_ID_VMIX: {
            int s32DevId = s32ChnId;
            if (s32DevId < 0 || s32DevId >= DRM_VMIX_DEVICE_BUTT) {
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

int drm_mpi_vi_get_channel_attribute(int channel, drm_vi_chn_attr_t *pstChnAttr)
{
    if ((channel < 0) || (channel >= DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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

    PARAM_STRING_APPEND_TO(stream_param, DRM_KEY_USE_LIBV4L2, 0);
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    g_vi_chns[channel].luma_rkmedia_buf.reset();
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (g_vi_chns[channel].status < CHN_STATUS_OPEN) {
        return -2;
    }

    g_vi_chns[channel].luma_buf_mtx.lock();
    g_vi_chns[channel].luma_buf_start = false;
    g_vi_chns[channel].luma_rkmedia_buf.reset();
    g_vi_chns[channel].luma_buf_mtx.unlock();

    g_vi_mtx.lock();
    if (g_vi_chns[channel].media_out_cb_status == CHN_OUT_CB_LUMA) {
        DRM_MEDIA_LOGD("luma mode: disable rkmedia out callback");
        g_vi_chns[channel].media_out_cb_status = CHN_OUT_CB_CLOSE;
        g_vi_chns[channel].media_flow->SetOutputCallBack(&g_vi_chns[channel], flow_output_callback);
    }
    g_vi_mtx.unlock();

    return 0;
}

int drm_mpi_vi_start_region_luma(int channel)
{
    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
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
    int s32Ret = 0;
    uint32_t u32XOffset = 0;
    uint32_t u32YOffset = 0;
    uint32_t u32ImgWidth = 0;
    uint32_t u32ImgHeight = 0;

    if ((channel < 0) || (channel > DRM_VI_CHANNEL_BUTT)) {
        return -1;
    }

    if (!pstRegionInfo || !pstRegionInfo->u32RegionNum || !pu64LumaData) {
        return -2;
    }

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

        if (!target_chn->luma_rkmedia_buf) {
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

        if (target_chn->luma_rkmedia_buf) {
            media_mb = std::static_pointer_cast<libdrm::ImageBuffer>(target_chn->luma_rkmedia_buf);
        }

        target_chn->luma_rkmedia_buf.reset();
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
    if ((channel < 0) || (channel >= DRM_VI_CHANNEL_BUTT))
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
    if ((channel < 0) || (channel >= DRM_VI_CHANNEL_BUTT)) {
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

    if (((channel < 0) || (channel >= DRM_VO_CHANNEL_BUTT)) || !pstAttr) {
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
    if ((channel < DRM_VO_CHANNEL_0) || (channel >= DRM_VO_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_VO_CHANNEL_BUTT))
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
    if ((channel < 0) || (channel >= DRM_VO_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    //  uint32_t u32InVirWidth = pstRgaAttr->stImgIn.u32HorStride;
    //  uint32_t u32InVirHeight = pstRgaAttr->stImgIn.u32VirStride;
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
    DRM_MEDIA_LOGD("Rkrga Filter flow param:[%s]", flow_param.c_str());

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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    if ((channel < 0) || (channel >= DRM_RGA_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
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

    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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
    if ((device < 0) || (device >= DRM_VMIX_DEVICE_BUTT)) {
        return -1;
    }

    if ((channel < 0) || (channel >= DRM_VMIX_CHANNEL_BUTT)) {
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

#ifdef __cplusplus
}
#endif
