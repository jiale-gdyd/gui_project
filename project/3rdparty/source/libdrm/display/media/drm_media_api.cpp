#include <mutex>
#include <string>
#include <sstream>
#include <algorithm>
#include <condition_variable>

#include <fcntl.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <libdrm/drm_media_vi.h>
#include <libdrm/drm_media_vo.h>
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

static std::mutex g_vi_mtx;
static std::mutex g_vo_mtx;

static drm_media_channel_t g_vi_chns[DRM_VI_CHANNEL_BUTT];
static drm_media_channel_t g_vo_chns[DRM_VO_CHANNEL_BUTT];

static unsigned char g_sys_init = 0;

extern std::mutex g_handle_mb_mutex;
extern std::list<handle_mb_t *> g_handle_mb;

static const char *mod_tag_list[MOD_ID_BUTT] = {
    "UNKNOW", "SYS", "VO", "VI",
};

static const char *ModIdToString(mod_id_e mod_id)
{
    if ((mod_id < MOD_ID_UNKNOW) || (mod_id >= MOD_ID_BUTT)) {
        DRM_MEDIA_LOGE("mod_id is incorrect");
        return mod_tag_list[MOD_ID_UNKNOW];
    }

    return mod_tag_list[mod_id];
}

static mb_type_e get_buffer_type(drm_media_channel_t *target_chn)
{
    mb_type_e type = (mb_type_e)0;

    if (!target_chn) {
        return type;
    }

    switch (target_chn->mode_id) {
        case MOD_ID_VI:
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

        default:
            return -2;
    }

    if (!src_chn || (src_chn->status < CHN_STATUS_OPEN) || !src) {
        DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d] is not ready", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId);
        return -3;
    }

    switch (pstDstChn->enModId) {
        case MOD_ID_VO:
            sink = g_vo_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vo_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vo_mtx;
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

    if ((src_chn->media_out_cb_status == CHN_OUT_CB_INIT) || (src_chn->media_out_cb_status == CHN_OUT_CB_CLOSE)) {
        DRM_MEDIA_LOGD("disable media flow output callback");
        src_chn->media_out_cb_status = CHN_OUT_CB_CLOSE;
        src->SetOutputCallBack(NULL, NULL);
        clear_media_channel_buffer(src_chn);
    }

    src_chn->status = CHN_STATUS_BIND;
    src_chn->bind_ref_nxt++;

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

        default:
            return -1;
    }

    if (!src) {
        DRM_MEDIA_LOGE("src mode:[%s]-chn:[%d]: flow is null", ModIdToString(pstSrcChn->enModId), pstSrcChn->s32ChnId);
        return -2;
    }

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

    switch (pstDstChn->enModId) {
        case MOD_ID_VO:
            sink = g_vo_chns[pstDstChn->s32ChnId].media_flow;
            dst_chn = &g_vo_chns[pstDstChn->s32ChnId];
            dst_mutex = &g_vo_mtx;
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
    src_chn->bind_ref_nxt--;
    if ((src_chn->bind_ref_nxt <= 0) && (src_chn->bind_ref_pre <= 0)) {
        src_chn->status = CHN_STATUS_OPEN;
        src_chn->bind_ref_pre = 0;
        src_chn->bind_ref_nxt = 0;
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

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -3;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("channel:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -4;
    }

    int ret = target_chn->media_flow->SetRunTimes(pstRecvParam->s32RecvPicNum);
    target_mutex->unlock();
    if (ret != pstRecvParam->s32RecvPicNum) {
        DRM_MEDIA_LOGE("SetRunTimes failed, return:[%d]", ret);
        return -5;
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

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]\n", enModID);
            return -2;
        }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -3;
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

        default:
            DRM_MEDIA_LOGE("unsupport mod");
            return -2;
    }

    media_buffer_impl_t *mb = (media_buffer_impl_t *)buffer;
    target_mutex->lock();
    if (target_chn->media_flow) {
        target_chn->media_flow->SendInput(mb->media_mb, 0);
    } else {
        target_mutex->unlock();
        DRM_MEDIA_LOGE("media flow null");
        return -3;
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

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -2;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -3;
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

        default:
            DRM_MEDIA_LOGE("invalid mode:[%d]", enModID);
            return -2;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -3;
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

int drm_mpi_create_vo_channel(int channel, const drm_vo_chn_attr_t *pstAttr)
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

int drm_mpi_destroy_vo_channel(int channel)
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

int drm_mpi_get_vo_channel_attribute(int channel, drm_vo_chn_attr_t *pstAttr)
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

int drm_mpi_set_vo_channel_attribute(int channel, const drm_vo_chn_attr_t *pstAttr)
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

#ifdef __cplusplus
}
#endif
