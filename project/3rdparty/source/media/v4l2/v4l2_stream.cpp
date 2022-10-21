#include <fcntl.h>
#include <string.h>
#include <media/utils/control.h>

#include "v4l2_stream.h"

namespace libdrm {
V4L2Context::V4L2Context(enum v4l2_buf_type cap_type, v4l2_io_t io_func, const std::string device)
    : fd(-1), capture_type(cap_type), vio(io_func), started(false),path(device)
{
    const char *dev = device.c_str();
    fd = v4l2_open(dev, O_RDWR | O_CLOEXEC, 0);
    if (fd < 0) {
        DRM_MEDIA_LOGE("V4L2Ctx: open:[%s] failed, errstr:[%m]", dev);
    } else {
        DRM_MEDIA_LOGI("V4L2Ctx: open:[%s], fd:[%d]", dev, fd);
    }
}

V4L2Context::~V4L2Context()
{
    if (fd >= 0) {
        SetStarted(false);
        v4l2_close(fd);
        DRM_MEDIA_LOGI("V4L2Ctx: close:[%s], fd:[%d]", path.c_str(), fd);
    }
}

bool V4L2Context::SetStarted(bool val)
{
    std::lock_guard<std::mutex> _lk(mtx);
    if (started == val) {
        return true;
    }

    enum v4l2_buf_type cap_type = capture_type;
    unsigned int request = val ? VIDIOC_STREAMON : VIDIOC_STREAMOFF;
    if (IoCtrl(request, &cap_type) < 0) {
        DRM_MEDIA_LOGE("ioctl(%d) failed, errstr:[%m]", (int)request);
        return false;
    }

    started = val;
    return true;
}

int V4L2Context::IoCtrl(unsigned long int request, void *arg)
{
    if (fd < 0) {
        errno = EINVAL;
        return -1;
    }

    return V4L2IoCtl(&vio, fd, request, arg);
}

V4L2MediaCtl::V4L2MediaCtl()
{

}

V4L2MediaCtl::~V4L2MediaCtl()
{

}

V4L2Stream::V4L2Stream(const char *param)
    : use_libv4l2(false), camera_id(0), fd(-1), capture_type(V4L2_BUF_TYPE_VIDEO_CAPTURE), plane_cnt(1), enable_user_picture(0), recent_time(0)
{
    memset(&vio, 0, sizeof(vio));

    std::string str_libv4l2;
    std::map<std::string, std::string> params;
    std::list<std::pair<const std::string, std::string &>> req_list;

    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_USE_LIBV4L2, str_libv4l2));
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_DEVICE, device));

    std::string str_camera_id;
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_CAMERA_ID, str_camera_id));
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_SUB_DEVICE, sub_device));

    std::string cap_type;
    req_list.push_back(std::pair<const std::string, std::string &>(DRM_KEY_V4L2_CAP_TYPE, cap_type));

    int ret = parse_media_param_match(param, params, req_list);
    if (ret == 0) {
        return;
    }

    if (!str_camera_id.empty()) {
        camera_id = std::stoi(str_camera_id);
    }

    if (!str_libv4l2.empty()) {
        use_libv4l2 = !!std::stoi(str_libv4l2);
    }

    if (!cap_type.empty()) {
        capture_type = static_cast<enum v4l2_buf_type>(GetV4L2Type(cap_type.c_str()));
    }
    v4l2_medctl = std::make_shared<V4L2MediaCtl>();

    DRM_MEDIA_LOGI("V4l2Stream: camra id:[%d], device:[%s]", camera_id, device.c_str());
}

int V4L2Stream::Open()
{
    if (!SetV4L2IoFunction(&vio, use_libv4l2)) {
        return -EINVAL;
    }

    if (!sub_device.empty()) {
        // TODO:
    }

    if (!strcmp(device.c_str(), MB_ENTITY_NAME)
      || !strcmp(device.c_str(), S0_ENTITY_NAME)
      || !strcmp(device.c_str(), S1_ENTITY_NAME)
      || !strcmp(device.c_str(), S2_ENTITY_NAME))
    {
#if defined(CONFIG_RKAIQ)
        devname = v4l2_medctl->media_ctl_infos.GetVideoNode(camera_id, device.c_str());
#else
        DRM_MEDIA_LOGE("V4l2Stream: videoNode:[%s] is invalid without librkaiq", device.c_str());
        return -EINVAL;
#endif
    } else {
        devname = device;
    }

    DRM_MEDIA_LOGI("V4l2Stream: camera id:[%d], videoNode:[%s]", camera_id, devname.c_str());

    v4l2_ctx = std::make_shared<V4L2Context>(capture_type, vio, devname);
    if (!v4l2_ctx) {
        return -ENOMEM;
    }

    fd = v4l2_ctx->GetDeviceFd();
    if (fd < 0) {
        v4l2_ctx = nullptr;
        return -1;
    }

    user_picture = nullptr;
    return 0;
}

int V4L2Stream::Close()
{
    if (v4l2_ctx) {
        v4l2_ctx->SetStarted(false);
        v4l2_ctx = nullptr;
        DRM_MEDIA_LOGI("V4L2Stream: v4l2 ctx reset to nullptr");
    }

    fd = -1;
    return 0;
}

int V4L2Stream::IoCtrl(unsigned long int request, ...)
{
    va_list vl;

    switch (request) {
        case S_SUB_REQUEST: {
            va_start(vl, request);
            void *arg = va_arg(vl, void *);
            va_end(vl);

            auto sub = (SubRequest *)arg;
            return V4L2IoCtl(&vio, fd, sub->sub_request, sub->arg);
        }

        case S_STREAM_OFF: {
            return v4l2_ctx->SetStarted(false) ? 0 : -1;
        }

        case G_STREAM_RECENT_TIME: {
            va_start(vl, request);
            int64_t *arg = va_arg(vl, int64_t *);
            va_end(vl);

            *arg = recent_time;
            return 0;
        }

        case S_INSERT_USER_PICTURE: {
            va_start(vl, request);
            UserPicArg *arg = va_arg(vl, UserPicArg *);
            va_end(vl);

            if (arg->data && (arg->size > 0)) {
                DRM_MEDIA_LOGI("V4L2: Insert user picture, ptr:[%p], size:[%d]", arg->data, (int)arg->size);
                if (user_picture == nullptr) {
                    auto mb = MediaBuffer::Alloc((size_t)arg->size, MediaBuffer::MemType::MEM_HARD_WARE);
                    if (!mb) {
                        DRM_MEDIA_LOGE("V4L2: Enable user picture: no mem left");
                        return -1;
                    }

                    user_picture = mb;
                }

                user_picture->BeginCPUAccess(true);
                memcpy(user_picture->GetPtr(), arg->data, arg->size);
                user_picture->EndCPUAccess(true);
                user_picture->SetValidSize(arg->size);
            } else {
                DRM_MEDIA_LOGI("V4L2: Reset user picture");
                user_picture.reset();
            }

            return 0;
        }

        case S_ENABLE_USER_PICTURE: {
            if (!user_picture || !user_picture->GetValidSize()) {
                DRM_MEDIA_LOGE("V4L2: Enable user picture failed, Do insert first");
                return -1;
            }

            DRM_MEDIA_LOGI("V4L2: Enable user picture");
            enable_user_picture = 1;
            return 0;
        }

        case S_DISABLE_USER_PICTURE: {
            DRM_MEDIA_LOGI("V4L2: Disable user picture");
            enable_user_picture = 0;
            return 0;
        }

        case S_RGA_LINE_INFO: {
            va_start(vl, request);
            void *arg = va_arg(vl, void *);
            va_end(vl);

            DrmImageBorder *line = (DrmImageBorder *)arg;
            param_mtx.lock();
            int exist = 0;
            for (auto it = lines.begin(); it != lines.end(); ++it) {
            DrmImageBorder &l = *it;
                if (l.id == line->id) {
                    exist = 1;
                    memcpy(&l, line, sizeof(DrmImageBorder));
                    break;
                }
            }

            if (!exist) {
                lines.push_back(*line);
            }
            param_mtx.unlock();
            return 0;
        }

        case S_RGA_OSD_INFO: {
            va_start(vl, request);
            void *arg = va_arg(vl, void *);
            va_end(vl);

            DrmImageOsd *osd = (DrmImageOsd *)arg;
            DrmOsdInfo info = {osd->x, osd->y, osd->w, osd->h, osd->data, osd->pix_fmt, osd->enable};

            param_mtx.lock();
            osds[osd->id] = info;
            param_mtx.unlock();
            return 0;
        }
    }

    return -1;
}
}
