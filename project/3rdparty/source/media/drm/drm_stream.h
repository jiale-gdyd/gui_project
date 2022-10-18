#ifndef LIBDRM_DISPLAY_DRM_STREAM_H
#define LIBDRM_DISPLAY_DRM_STREAM_H

#include <assert.h>
#include <vector>

#include <media/utils/image.h>
#include <media/utils/stream.h>

#include "drm_utils.h"

namespace libdrm {
class DRMStream : public Stream {
public:
    DRMStream(const char *param, bool accept_scale = false);

    virtual ~DRMStream() {
        DRMStream::Close();
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

protected:
    virtual int Open() override;
    virtual int Close() override;

    void SetModeInfo(const drmModeModeInfo &mode);
    bool GetAgreeableIDSet();

    bool                       accept_scale;
    std::string                device;
    std::string                data_type;
    DrmImageInfo               img_info;
    int                        fps;
    uint32_t                   connector_id;
    uint32_t                   crtc_id;
    uint32_t                   encoder_id;
    std::string                plane_type;
    uint32_t                   plane_id;
    std::vector<uint32_t>      skip_plane_ids;
    bool                       active;
    drmModeModeInfo            cur_mode;
    uint32_t                   drm_fmt;
    bool                       find_strict_match_wh;

    int                        fd;
    struct resources           *res;
    std::shared_ptr<DRMDevice> dev;
};
}

#endif
