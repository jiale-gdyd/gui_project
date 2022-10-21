#ifndef LIBDRM_V4L2_STREAM_H
#define LIBDRM_V4L2_STREAM_H

#include <assert.h>
#include <stdint.h>

#include <mutex>

#include <media/utils/stream.h>
#include <media/utils/buffer.h>

#include "v4l2_utils.h"

namespace libdrm {
class V4L2Context {
public:
    V4L2Context(enum v4l2_buf_type cap_type, v4l2_io_t io_func, const std::string device);
    ~V4L2Context();

    int GetDeviceFd() {
        return fd;
    }

    void SetCapType(enum v4l2_buf_type type) {
        capture_type = type;
    }

    bool SetStarted(bool val);
    int IoCtrl(unsigned long int request, void *arg);

private:
    int                fd;
    enum v4l2_buf_type capture_type;
    v4l2_io_t          vio;
    std::mutex         mtx;
    volatile bool      started;
    std::string        path;
};

class V4L2MediaCtl {
public:
    V4L2MediaCtl();
    ~V4L2MediaCtl();

    int InitHwInfos();
    int SetupLink(std::string devname, bool enable);

#if defined(CONFIG_RKAIQ)
    RKAiqMedia media_ctl_infos;
#endif
};

class V4L2Stream : public Stream {
public:
    V4L2Stream(const char *param);
    virtual ~V4L2Stream() {
        V4L2Stream::Close();
    }

    virtual size_t Read(void *ptr _DRM_UNUSED, size_t size _DRM_UNUSED, size_t nmemb _DRM_UNUSED) final {
        return 0;
    }

    virtual int Seek(int64_t offset _DRM_UNUSED, int whence _DRM_UNUSED) final {
        return -1;
    }

    virtual long Tell() final {
        return -1;
    }

    virtual size_t Write(const void *ptr _DRM_UNUSED, size_t size _DRM_UNUSED, size_t nmemb _DRM_UNUSED) final {
        return 0;
    }

    virtual int IoCtrl(unsigned long int request, ...) override;

#define v4l2_open   vio.open_f
#define v4l2_close  vio.close_f
#define v4l2_dup    vio.dup_f
#define v4l2_ioctl  vio.ioctl_f
#define v4l2_read   vio.read_f
#define v4l2_mmap   vio.mmap_f
#define v4l2_munmap vio.munmap_f

protected:
    virtual int Open() override;
    virtual int Close() override;

    bool                          use_libv4l2;
    v4l2_io_t                     vio;
    std::string                   devname;
    std::string                   device;
    std::string                   sub_device;
    int                           camera_id;
    int                           fd;

    enum v4l2_buf_type            capture_type;
    enum v4l2_buf_type            output_type;
    int                           plane_cnt;
    std::shared_ptr<V4L2Context>  v4l2_ctx;
    std::shared_ptr<V4L2MediaCtl> v4l2_medctl;
    std::shared_ptr<MediaBuffer>  user_picture;
    int                           enable_user_picture;
    ConditionLockMutex            param_mtx;
    std::vector<DrmImageBorder>   lines;
    std::map<int, DrmOsdInfo>     osds;
    int64_t                       recent_time;
};
}

#endif
