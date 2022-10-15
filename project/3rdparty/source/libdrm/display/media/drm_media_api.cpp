#include <mutex>
#include <string>
#include <sstream>
#include <algorithm>
#include <condition_variable>

#include <fcntl.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <libdrm/drm_media_vo.h>
#include <libdrm/drm_media_api.h>
#include <libdrm/display/utils.hpp>
#include <libdrm/display/image.hpp>
#include <libdrm/display/stream.hpp>
#include <libdrm/drm_media_buffer.h>
#include <libdrm/display/control.hpp>
#include <libdrm/display/encoder.hpp>
#include <libdrm/display/message.hpp>
#include <libdrm/display/key_string.hpp>
#include <libdrm/display/media_type.hpp>
#include <libdrm/display/media_config.hpp>

#include "media_utils.hpp"
#include "drm_media_buffer_impl.hpp"

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
    drm_vo_chn_attr_t                        vo_attr;

    uint16_t                                 chn_ref_cnt;
    uint16_t                                 bind_ref_pre;
    uint16_t                                 bind_ref_nxt;
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

std::mutex g_vo_mtx;
drm_media_channel_t g_vo_chns[DRM_VO_CHANNEL_BUTT];

static unsigned char g_sys_init = 0;

// static inline void media_push_pip_fd(int fd)
// {
//     int i = 0;
//     ssize_t count = write(fd, &i, sizeof(i));
//     if (count < 0) {
//         printf("%s: write(%d) failed: %s\n", __func__, fd, strerror(errno));
//     }
// }

// static inline void media_pop_pip_fd(int fd)
// {
//     int i = 0;
//     ssize_t read_size = (ssize_t)sizeof(i);
//     ssize_t ret = read(fd, &i, sizeof(i));
//     if (ret != read_size) {
//         printf("%s: Read(%d) failed: %s\n", __func__, fd, strerror(errno));
//     }
// }

// static int media_channel_push_buffer(drm_media_channel_t *ptrChn, media_buffer_t buffer)
// {
//     if (!ptrChn || !buffer) {
//         return -1;
//     }

//     ptrChn->buffer_list_mtx.lock();
//     if (!ptrChn->buffer_list_quit) {
//         ptrChn->buffer_list.push_back(buffer);
//         if (ptrChn->wake_fd[1] > 0)
//         media_push_pip_fd(ptrChn->wake_fd[1]);
//     } else {
//         drm_mpi_mb_release_buffer(buffer);
//     }

//     ptrChn->buffer_list_cond.notify_all();
//     ptrChn->buffer_list_mtx.unlock();
//     pthread_yield();

//     return 0;
// }

// static media_buffer_t media_channel_pop_buffer(drm_media_channel_t *ptrChn, int s32MilliSec)
// {
//     if (!ptrChn) {
//         return NULL;
//     }

//     std::unique_lock<std::mutex> lck_pop(ptrChn->buffer_list_pop_mtx);
//     std::unique_lock<std::mutex> lck(ptrChn->buffer_list_mtx);
//     if (ptrChn->buffer_list.empty()) {
//         if (s32MilliSec < 0 && !ptrChn->buffer_list_quit) {
//             ptrChn->buffer_list_cond.wait(lck);
//         } else if (s32MilliSec > 0) {
//             if (ptrChn->buffer_list_cond.wait_for(lck, std::chrono::milliseconds(s32MilliSec)) == std::cv_status::timeout) {
//                 printf("INFO: %s: Chn[%d] get mediabuffer timeout!\n", __func__, ptrChn->chn_id);
//                 return NULL;
//             }
//         } else {
//             return NULL;
//         }
//     }

//     media_buffer_t mb = NULL;
//     if (!ptrChn->buffer_list.empty()) {
//         if (ptrChn->wake_fd[0] > 0) {
//             media_pop_pip_fd(ptrChn->wake_fd[0]);
//         }

//         mb = ptrChn->buffer_list.front();
//         ptrChn->buffer_list.pop_front();
//     }

//     return mb;
// }

// static void media_channel_init_buffer(drm_media_channel_t *ptrChn)
// {
//     if (!ptrChn) {
//         return;
//     }

//     printf("#%p Chn[%d] clear media buffer start...\n", ptrChn, ptrChn->chn_id);

//     media_buffer_t mb = NULL;
//     ptrChn->buffer_list_mtx.lock();
//     while (!ptrChn->buffer_list.empty()) {
//         mb = ptrChn->buffer_list.front();
//         ptrChn->buffer_list.pop_front();
//         drm_mpi_mb_release_buffer(mb);
//     }

//     ptrChn->buffer_list_quit = true;
//     ptrChn->buffer_list_cond.notify_all();
//     ptrChn->buffer_list_mtx.unlock();

//     printf("#%p Chn[%d] clear media buffer end...\n", ptrChn, ptrChn->chn_id);
// }

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
        //memset(tbl[i].u32ArgbColorTbl, 0, 256);
        tbl[i].buffer_depth = 2;
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

    reset_channel_table(g_vo_chns, DRM_VO_CHANNEL_BUTT, MOD_ID_VO);
    g_sys_init = 1;

    return 0;
}

int drm_mpi_system_set_framerate(mod_id_e enModID, int s32ChnId, drm_fps_attr_t *pstFpsAttr)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    if (!pstFpsAttr) {
        return -1;
    }

    int s32FpsInRatio = pstFpsAttr->s32FpsInNum * pstFpsAttr->s32FpsOutDen;
    int s32FpsOutRatio = pstFpsAttr->s32FpsOutNum * pstFpsAttr->s32FpsInDen;

    if (s32FpsInRatio < s32FpsOutRatio) {
        return -2;
    }

    switch (enModID) {
        case MOD_ID_VO:
            if ((s32ChnId < 0) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                return -3;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        default:
            return -4;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("chn:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -5;
    }

    int ret = 0;
    ret = target_chn->media_flow->SetInputFpsControl(s32FpsInRatio, s32FpsOutRatio);
    target_mutex->unlock();
    if (ret) {
        return -6;
    }

    return 0;
}

int drm_mpi_system_start_recv_frame(mod_id_e enModID, int s32ChnId, const drm_recv_pic_param_t *pstRecvParam)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    if (!pstRecvParam) {
        return -1;
    }

    switch (enModID) {
        case MOD_ID_VO:
            if ((s32ChnId < 0) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                return -2;
            }
            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        default:
            return -3;
    }

    target_mutex->lock();
    if (target_chn->status < CHN_STATUS_OPEN) {
        DRM_MEDIA_LOGE("chn:[%d] in status:[%d], this operation is not allowed", s32ChnId, target_chn->status);
        target_mutex->unlock();
        return -4;
    }

    int ret = target_chn->media_flow->SetRunTimes(pstRecvParam->s32RecvPicNum);
    target_mutex->unlock();
    if (ret != pstRecvParam->s32RecvPicNum) {
        return -5;
    }

    return 0;
}

int drm_mpi_system_send_media_buffer(mod_id_e enModID, int s32ChnId, media_buffer_t buffer)
{
    std::mutex *target_mutex = NULL;
    drm_media_channel_t *target_chn = NULL;

    switch (enModID) {
        case MOD_ID_VO:
            if ((s32ChnId < 0) || (s32ChnId >= DRM_VO_CHANNEL_BUTT)) {
                return -1;
            }

            target_chn = &g_vo_chns[s32ChnId];
            target_mutex = &g_vo_mtx;
            break;

        default:
            return -2;
    }

    media_buffer_impl_t *mb = (media_buffer_impl_t *)buffer;
    target_mutex->lock();
    if (target_chn->media_flow) {
        target_chn->media_flow->SendInput(mb->media_mb, 0);
    } else {
        target_mutex->unlock();
        return -3;
    }

    target_mutex->unlock();
    return 0;
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
            return -1;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (!target_chn->media_flow_list.empty()) {
        flow = target_chn->media_flow_list.back();
    } else if (target_chn->media_flow) {
        flow = target_chn->media_flow;
    }

    if (!flow) {
        DRM_MEDIA_LOGE("<ModeID:%d ChnID:%d> fatal error! Status does not match the resource", pstChn->enModId, pstChn->s32ChnId);
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
            return -1;
    }

    if (target_chn->status < CHN_STATUS_OPEN) {
        return -2;
    }

    if (!target_chn->media_flow_list.empty()) {
        flow = target_chn->media_flow_list.back();
    } else if (target_chn->media_flow) {
        flow = target_chn->media_flow;
    }

    if (!flow) {
        DRM_MEDIA_LOGE("<ModeID:%d ChnID:%d> fatal error! Status does not match the resource", pstChn->enModId, pstChn->s32ChnId);
        return -3;
    }

    target_chn->out_ex_cb = callback;
    target_chn->out_handle = handle;

    return 0;
}

int drm_mpi_create_vo_channel(int channel, const drm_vo_chn_attr_t *pstAttr)
{
    const char *pcPlaneType = NULL;

    if (((channel < 0) || (channel >= DRM_VO_CHANNEL_BUTT)) || !pstAttr) {
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
        return -2;
    }

    g_vo_mtx.lock();
    if (g_vo_chns[channel].status != CHN_STATUS_CLOSED) {
        g_vo_mtx.unlock();
        return -3;
    }

    printf("Enable vo channel:[%d] starting......\n", channel);

    std::string flow_name = "output_stream";
    std::string flow_param = "";
    PARAM_STRING_APPEND(flow_param, DRM_KEY_NAME, "drm_output_stream");

    std::string stream_param = "";
    if (!pstAttr->pcDevNode) {
        printf("use default DevNode:/dev/dri/card0\n");
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
    printf("DrmDisplay flow params:\n%s\n", flow_param.c_str());

    g_vo_chns[channel].media_flow = libdrm::REFLECTOR(Flow)::Create<libdrm::Flow>(flow_name.c_str(), flow_param.c_str());
    if (!g_vo_chns[channel].media_flow) {
        g_vo_mtx.unlock();
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
    printf("Enable vo channel:[%d] finished......\n", channel);

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