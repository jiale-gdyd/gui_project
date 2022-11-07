#include <linux/kconfig.h>
#include <media/utils/buffer.h>
#include <media/utils/control.h>

#include "drm_stream.h"

namespace libdrm {
#define USING_ASYNC_COMMIT      1

struct plane_property_ids {
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t src_x;
    uint32_t src_y;
    uint32_t src_w;
    uint32_t src_h;
    uint32_t crtc_x;
    uint32_t crtc_y;
    uint32_t crtc_w;
    uint32_t crtc_h;
    uint32_t zpos;
    uint32_t feature;
    uint32_t async_commit;
};

class DRMDisplayBuffer {
public:
    DRMDisplayBuffer(std::shared_ptr<ImageBuffer> buffer, const std::shared_ptr<DRMDevice> &dev, uint32_t drm_fmt, int num = 1, int den = 1)
        : ib(buffer), drm_dev(dev), drm_fd(dev->GetDeviceFd()), handle(0), fb_id(0)
    {
        int fd = buffer->GetFD();
        int ret = drmPrimeFDToHandle(drm_fd, fd, &handle);
        if (ret) {
            DRM_MEDIA_LOGE("failed to drmPrimeFDToHandle, return:[%d], errstr:[%m]", ret);
            return;
        }

        int w = buffer->GetVirWidth() * num / den;
        int h = buffer->GetVirHeight();
        uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};

        switch (drm_fmt) {
            case DRM_FORMAT_NV12:
            case DRM_FORMAT_NV16:
                handles[0] = handle;
                pitches[0] = w;
                offsets[0] = 0;
                handles[1] = handle;
                pitches[1] = pitches[0];
                offsets[1] = pitches[0] * h;
                break;

            case DRM_FORMAT_RGB332:
                handles[0] = handle;
                pitches[0] = w;
                offsets[0] = 0;
                break;

            case DRM_FORMAT_RGB565:
            case DRM_FORMAT_BGR565:
                handles[0] = handle;
                pitches[0] = w * 2;
                offsets[0] = 0;
                break;

            case DRM_FORMAT_RGB888:
            case DRM_FORMAT_BGR888:
                handles[0] = handle;
                pitches[0] = w * 3;
                offsets[0] = 0;
                break;

            case DRM_FORMAT_ARGB8888:
            case DRM_FORMAT_ABGR8888:
                handles[0] = handle;
                pitches[0] = w * 4;
                offsets[0] = 0;
                break;

            case DRM_FORMAT_XRGB8888:
            case DRM_FORMAT_XBGR8888:
            case DRM_FORMAT_RGBX8888:
            case DRM_FORMAT_BGRX8888:
                handles[0] = handle;
                pitches[0] = w * 4;
                offsets[0] = 0;
                break;

            default:
                DRM_MEDIA_LOGE("TODO: format for drm format:[%c%c%c%c]", DRM_DUMP_FOURCC(drm_fmt));
                return;
        }

        ret = drmModeAddFB2(drm_fd, w, h, drm_fmt, handles, pitches, offsets, &fb_id, 0);
        if (ret) {
            DRM_MEDIA_LOGE("drmModeAddFB2 failed, return:[%d], errstr:[%m]", ret);
            DRM_MEDIA_LOGI("num/den:[%d/%d], w:[%d], h:[%d], drm_fmt:[%c%c%c%c]", num, den, w, h, DRM_DUMP_FOURCC(drm_fmt));
        }
    }

    ~DRMDisplayBuffer() {
        if (fb_id > 0) {
            drmModeRmFB(drm_fd, fb_id);
        }

        if (handle > 0) {
            struct drm_mode_destroy_dumb data = {
                .handle = handle,
            };

            int ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &data);
            if (ret) {
                DRM_MEDIA_LOGE("failed to free drm handle:[%d], errstr:[%m]", handle);
            }
        }
    }

    uint32_t GetFBID() {
        return fb_id;
    }

private:
    std::shared_ptr<ImageBuffer> ib;
    std::shared_ptr<DRMDevice>   drm_dev;
    int                          drm_fd;
    uint32_t                     handle;
    uint32_t                     fb_id;
};

class DRMOutPutStream : public DRMStream {
public:
    DRMOutPutStream(const char *param);
    virtual ~DRMOutPutStream() {
        DRMOutPutStream::Close();
    }

    static const char *GetStreamName() {
        return "drm_output_stream";
    }

    virtual int Open() final;
    virtual int Close() final;
    virtual bool Write(std::shared_ptr<MediaBuffer>) override;
    virtual int IoCtrl(unsigned long int request, ...) override;

private:
#if defined(CONFIG_ROCKCHIP)
    static const DrmPixelFormat       defaultPixFmt = DRM_PIX_FMT_ARGB8888;
#else
    static const DrmPixelFormat       defaultPixFmt = DRM_PIX_FMT_XRGB8888;
#endif
    struct plane_property_ids         plane_prop_ids;
    int                               zindex;
    bool                              support_scale;

    bool                              plane_set;
    std::shared_ptr<DRMDisplayBuffer> disp_buffer;
    DrmImageRect                      src_rect;
    DrmImageRect                      dst_rect;
    ConditionLockMutex                rect_mtx;
};

DRMOutPutStream::DRMOutPutStream(const char *param) : DRMStream(param, true), zindex(-1), support_scale(false), plane_set(false)
{
    if (device.empty()) {
        return;
    }

    memset(&plane_prop_ids, 0x00, sizeof(plane_prop_ids));
    memset(&src_rect, 0x00, sizeof(src_rect));
    memset(&dst_rect, 0x00, sizeof(dst_rect));

    std::string str_zindex;
    std::map<std::string, std::string> params;
    std::list<std::pair<const std::string, std::string &>> req_list;

    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_ZPOS, str_zindex));
    int ret = parse_media_param_match(param, params, req_list);
    if (ret == 0) {
        return;
    }

    if (!str_zindex.empty()) {
        zindex = std::stoi(str_zindex);
    }
}

#define DRM_ATOMIC_ADD_PROP(object_id, property_id, value)                                                                  \
    do {                                                                                                                    \
        ret = drmModeAtomicAddProperty(req, object_id, property_id, value);                                                 \
        if (ret < 0) {                                                                                                      \
            DRM_MEDIA_LOGE("failed to add prop value:[%d] property_id:[%d], return:[%d]", (int)value, property_id, ret);    \
        }                                                                                                                   \
    } while (0)

#define DRM_ATOMIC_ADD_PROP_EXTRA(errvalue, addcode)                                                                        \
    uint32_t flags = 0;                                                                                                     \
    drmModeAtomicReq *req = drmModeAtomicAlloc();                                                                           \
    if (!req) {                                                                                                             \
        return errvalue;                                                                                                    \
    }                                                                                                                       \
                                                                                                                            \
    addcode                                                                                                                 \
    ret = drmModeAtomicCommit(fd, req, flags, NULL);                                                                        \
    if (ret) {                                                                                                              \
        DRM_MEDIA_LOGE("failed to atomic commit, return:[%d], errstr:[%m]", ret);                                           \
    }                                                                                                                       \
    drmModeAtomicFree(req);

int DRMOutPutStream::Open()
{
    if (data_type.empty()) {
        data_type = PixFmtToString(defaultPixFmt);
    }

    int ret = DRMStream::Open();
    if (ret) {
        return ret;
    }

    if (!GetAgreeableIDSet()) {
        DRM_MEDIA_LOGE("GetAgreeableIDSet failed");
        return -1;
    }

    {
        uint32_t blob_id;
        drmModeAtomicReq *req;
        uint32_t property_active;
        uint32_t property_crtc_id;
        uint32_t property_mode_id;
        uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_ALLOW_MODESET;

        property_crtc_id = get_property_id(res, DRM_MODE_OBJECT_CONNECTOR, connector_id, "CRTC_ID");
        property_active = get_property_id(res, DRM_MODE_OBJECT_CRTC, crtc_id, "ACTIVE");
        property_mode_id = get_property_id(res, DRM_MODE_OBJECT_CRTC, crtc_id, "MODE_ID");

        ret = drmModeCreatePropertyBlob(fd, &cur_mode, sizeof(cur_mode), &blob_id);
        if (ret != 0) {
            DRM_MEDIA_LOGE("drmModeCreatePropertyBlob failed, return:[%d], errstr:[%m]", ret);
        }

        req = drmModeAtomicAlloc();
        if (!req) {
            DRM_MEDIA_LOGE("drmModeAtomicAlloc failed, errstr:[%m]");
            return -1;
        }

        ret = drmModeAtomicAddProperty(req, crtc_id, property_active, 1);
        if (ret < 0) {
            DRM_MEDIA_LOGE("drmModeAtomicAddProperty crtc_id:[%u], property_active:[%u] failed, errstr:[%m]", crtc_id, property_active);
        }

        ret = drmModeAtomicAddProperty(req, crtc_id, property_mode_id, blob_id);
        if (ret < 0) {
            DRM_MEDIA_LOGE("drmModeAtomicAddProperty crtc_id:[%u], property_mode_id:[%u] failed, errstr:[%m]", crtc_id, property_mode_id);
        }

        ret = drmModeAtomicAddProperty(req, connector_id, property_crtc_id, crtc_id);
        if (ret < 0) {
            DRM_MEDIA_LOGE("drmModeAtomicAddProperty connector_id:[%u], property_crtc_id:[%u] failed, errstr:[%m]", crtc_id, property_crtc_id);
        }

        ret = drmModeAtomicCommit(fd, req, flags, NULL);
        drmModeAtomicFree(req);
        if (ret != 0) {
            DRM_MEDIA_LOGE("drmModeAtomicCommit set crtc failed, return:[%d], errstr:[%m]", ret);
            return ret;
        }
    }

    std::map<const char *, uint32_t *> plane_prop_id_map = {
        {DRM_KEY_CRTC_ID,      &plane_prop_ids.crtc_id},
        {DRM_KEY_FB_ID,        &plane_prop_ids.fb_id},
        {DRM_KEY_SRC_X,        &plane_prop_ids.src_x},
        {DRM_KEY_SRC_Y,        &plane_prop_ids.src_y},
        {DRM_KEY_SRC_W,        &plane_prop_ids.src_w},
        {DRM_KEY_SRC_H,        &plane_prop_ids.src_h},
        {DRM_KEY_CRTC_X,       &plane_prop_ids.crtc_x},
        {DRM_KEY_CRTC_Y,       &plane_prop_ids.crtc_y},
        {DRM_KEY_CRTC_W,       &plane_prop_ids.crtc_w},
        {DRM_KEY_CRTC_H,       &plane_prop_ids.crtc_h},
        {DRM_KEY_ZPOS,         &plane_prop_ids.zpos},
        {DRM_KEY_FEATURE,      &plane_prop_ids.feature},
        {DRM_KEY_ASYNC_COMMIT, &plane_prop_ids.async_commit},
    };

    for (auto &m : plane_prop_id_map) {
        uint64_t value = 0;
        *m.second = get_property_id(res, DRM_MODE_OBJECT_PLANE, plane_id, m.first, &value);
        if (*m.second <= 0) {
            return -1;
        }

        if (!strcmp(m.first, DRM_KEY_ZPOS)) {
            if (zindex >= 0 && zindex == (int)value) {
                zindex = -1;
            }
        } else if (!strcmp(m.first, DRM_KEY_FEATURE)) {
            support_scale = !!(value & 0x1);
        }
    }

    if (zindex >= 0) {
        DRM_ATOMIC_ADD_PROP_EXTRA(-1, DRM_ATOMIC_ADD_PROP(plane_id, plane_prop_ids.zpos, zindex);)
        if (ret) {
            DRM_MEDIA_LOGE("set zpos:[%d] failed, return:[%d]", zindex, ret);
            return ret;
        }
    }

    DrmImageRect ir = {0, 0, img_info.vir_width, img_info.vir_height};
    ret = IoCtrl(S_SOURCE_RECT, &ir);
    if (ret) {
        DRM_MEDIA_LOGE("failed to set display source rect, return:[%d]", ret);
        return -1;
    }

    ret = IoCtrl(S_DESTINATION_RECT, &ir);
    if (ret) {
        DRM_MEDIA_LOGE("failed to set display destination rect, return:[%d]", ret);
        return -1;
    }

#if USING_ASYNC_COMMIT
    uint32_t async_commit = 1;
    drmModeAtomicReqPtr req = drmModeAtomicAlloc();
    if (!req) {
        DRM_MEDIA_LOGE("drmModeAtomicAlloc failed, errstr:[%m]");
        return -1;
    }

    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.async_commit, async_commit);
    ret = drmModeAtomicCommit(fd, req, DRM_MODE_ATOMIC_ALLOW_MODESET, NULL);
    drmModeAtomicFree(req);
    if (ret) {
        DRM_MEDIA_LOGE("set async commit failed, drmModeAtomicCommit, return:[%d], errstr:[%m]", ret);
        return ret;
    }
#endif

    return 0;
}

int DRMOutPutStream::Close()
{
    int ret = DRMStream::Close();
    disp_buffer = nullptr;
    return ret;
}

int DRMOutPutStream::IoCtrl(unsigned long int request, ...)
{
    va_list vl;

    va_start(vl, request);
    void *arg = va_arg(vl, void *);
    va_end(vl);

    if (!arg) {
        return -1;
    }

    int ret = 0;

    switch (request) {
        case S_SOURCE_RECT: {
            DrmImageRect *rect = (static_cast<DrmImageRect *>(arg));
            if (!support_scale && ((dst_rect.w != 0 && rect->w != dst_rect.w) || (dst_rect.h != 0 && rect->h != dst_rect.h))) {
                DRM_MEDIA_LOGE("plane id:[%d] do not support scale, the source rect should the same to the destination rect", plane_id);
                return -1;
            }

            rect_mtx.lock();
            src_rect = *rect;
            rect_mtx.unlock();
        }
        break;

        case S_DESTINATION_RECT: {
            DrmImageRect *rect = (static_cast<DrmImageRect *>(arg));
            if (rect->x > img_info.width || rect->y > img_info.height) {
                return -EINVAL;
            }

            rect_mtx.lock();
            dst_rect = *rect;
            rect_mtx.unlock();
        }
        break;

        case S_SRC_DST_RECT: {
            DrmImageRect *rect = (static_cast<DrmImageRect *>(arg));
            if (!support_scale && (rect[0].w != rect[1].w || rect[0].h != rect[1].h)) {
                DRM_MEDIA_LOGE("plane id:[%d] do not support scale, input source rect and destination rect should be the same", plane_id);
                return -1;
            }

            rect_mtx.lock();
            src_rect = rect[0];
            dst_rect = rect[1];
            rect_mtx.unlock();
        }
        break;

        case G_SRC_DST_RECT: {
            DrmImageRect *rect = (static_cast<DrmImageRect *>(arg));

            rect_mtx.lock();
            rect[0] = src_rect;
            rect[1] = dst_rect;
            rect_mtx.unlock();
        }
        break;

        case G_PLANE_IMAGE_INFO: {
            *(static_cast<DrmImageInfo *>(arg)) = img_info;
        }
        break;

        case G_PLANE_SUPPORT_SCALE: {
            *((int *)arg) = support_scale ? 1 : 0;
        }
        break;

        case S_CRTC_PROPERTY:
        case S_CONNECTOR_PROPERTY: {
            uint32_t object_id = 0;
            uint32_t object_type = 0;

            switch (request) {
                case S_CRTC_PROPERTY:
                    object_type = DRM_MODE_OBJECT_CRTC;
                    object_id = crtc_id;
                    break;

                case S_CONNECTOR_PROPERTY:
                    object_type = DRM_MODE_OBJECT_CONNECTOR;
                    object_id = connector_id;
                    break;
            }

            DRMPropertyArg *prop_arg = (static_cast<DRMPropertyArg *>(arg));
            assert(prop_arg->name);

            ret = set_property(res, object_type, object_id, prop_arg->name, prop_arg->value);
            if (ret < 0) {
                DRM_MEDIA_LOGE("failed to set propery name:[%s], value:[%d] on object id:[%d]", prop_arg->name, (int)prop_arg->value, object_id);
            }
        }
        break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

bool DRMOutPutStream::Write(std::shared_ptr<MediaBuffer> input)
{
    if (input->GetType() != DrmType::Image) {
        return false;
    }

    int num = 1, den = 1;
    auto input_img = std::static_pointer_cast<ImageBuffer>(input);

    if (img_info.pix_fmt != input_img->GetPixelFormat()) {
        int drm_num = 0, drm_den = 0;
        GetPixFmtNumDen(img_info.pix_fmt, drm_num, drm_den);

        int in_num = 0, in_den = 0;
        GetPixFmtNumDen(input_img->GetPixelFormat(), in_num, in_den);

        num = in_num * drm_den;
        den = in_den * drm_num;
    }

    DrmImageRect src_rect_tmp;
    DrmImageRect dst_rect_tmp;

    rect_mtx.lock();
    src_rect_tmp = src_rect;
    dst_rect_tmp = dst_rect;
    rect_mtx.unlock();

    if ((input_img->GetVirWidth() < (src_rect_tmp.x + src_rect_tmp.w)) || (input_img->GetVirHeight() < (src_rect_tmp.y + src_rect_tmp.h))) {
        DRM_MEDIA_LOGE("InBuf:[%dx%d] does not match the imgRect:[%d,%d,%d,%d]", input_img->GetVirWidth(), input_img->GetVirHeight(), src_rect_tmp.x, src_rect_tmp.y, src_rect_tmp.w, src_rect_tmp.h);
        return false;
    }

    uint32_t disp_fb_id = 0;
    auto disp = std::make_shared<DRMDisplayBuffer>(input_img, dev, drm_fmt, num, den);
    if (!disp || (disp_fb_id = disp->GetFBID()) == 0) {
        return false;
    }

    int ret = 0;

#if USING_ASYNC_COMMIT
    uint32_t flags = 0;
    ret = drmModeSetPlane(fd, plane_id, crtc_id, disp_fb_id, flags, dst_rect_tmp.x, dst_rect_tmp.y, dst_rect_tmp.w, dst_rect_tmp.h, src_rect_tmp.x << 16, src_rect_tmp.y << 16, src_rect_tmp.w << 16, src_rect_tmp.h << 16);

    drmVBlank vbl;
    memset(&vbl, 0x00, sizeof(vbl));
    vbl.request.type = DRM_VBLANK_RELATIVE;
    vbl.request.sequence = 1;

    int vbl_ret = drmWaitVBlank(fd, &vbl);
    if (vbl_ret != 0) {
        DRM_MEDIA_LOGE("drmWaitVBlank (relative) failed return:[%d], errstr:[%m]", ret);
    }
#else
    drmModeAtomicReq *req = drmModeAtomicAlloc();
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.crtc_id, crtc_id);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.fb_id, disp_fb_id);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.crtc_x, dst_rect_tmp.x);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.crtc_y, dst_rect_tmp.y);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.crtc_w, dst_rect_tmp.w);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.crtc_h, dst_rect_tmp.h);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.src_x, src_rect_tmp.x << 16);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.src_y, src_rect_tmp.y << 16);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.src_w, src_rect_tmp.w << 16);
    drmModeAtomicAddProperty(req, plane_id, plane_prop_ids.src_h, src_rect_tmp.h << 16);

    ret = drmModeAtomicCommit(fd, req, 0, NULL);
    drmModeAtomicFree(req);
#endif
    if (!ret) {
        disp_buffer = disp;
        return true;
    }

    DRM_MEDIA_LOGE("crtc_id:[%d], plane_id:[%d], Imgbuf:[%d,%d,%d,%d] display with [%d,%d,%d,%d] failed", crtc_id, plane_id, src_rect_tmp.x, src_rect_tmp.y, src_rect_tmp.w, src_rect_tmp.h, dst_rect_tmp.x, dst_rect_tmp.y, dst_rect_tmp.w, dst_rect_tmp.h);
    return false;
}

DEFINE_STREAM_FACTORY(DRMOutPutStream, Stream)

const char *FACTORY(DRMOutPutStream)::ExpectedInputDataType()
{
    return GetStringOfDRMFmts().c_str();
}

const char *FACTORY(DRMOutPutStream)::OutPutDataType()
{
    return DRM_TYPE_NOTHING;
}
}
