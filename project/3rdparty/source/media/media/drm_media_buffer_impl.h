#ifndef LIBDRM_MEDIA_BUFFER_IMPL_H
#define LIBDRM_MEDIA_BUFFER_IMPL_H

#include <media/utils/flow.h>
#include <media/utils/buffer.h>
#include <media/drm_media_common.h>
#include <media/drm_media_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MEDIA_BUFFER_S {
    mb_type_e                            type;
    void                                 *ptr;
    int                                  fd;
    int                                  handle;
    int                                  dev_fd;
    size_t                               size;
    mod_id_e                             mode_id;
    uint16_t                             chn_id;
    uint64_t                             timestamp;
    uint32_t                             flag;
    uint32_t                             tsvc_level;
    std::shared_ptr<libdrm::MediaBuffer> media_mb;
    union {
        mb_image_info_t                  stImageInfo;
        mb_audio_info_t                  stAudioInfo;
    };
} media_buffer_impl_t;

typedef struct _MEDIA_BUFFER_POOL_S {
    mb_type_e                           enType;
    uint32_t                            u32Cnt;
    uint32_t                            u32Size;
    std::shared_ptr<libdrm::BufferPool> media_mbp;

    union {
        mb_image_info_t                 stImageInfo;
        mb_audio_info_t                 stAudioInfo;
    };

    union {
        DrmImageInfo                    mediaImgInfo;
        DrmSampleInfo                   mediaSampleInfo;
    };
} media_buffer_pool_impl_t;

#ifdef __cplusplus
}
#endif

#endif
