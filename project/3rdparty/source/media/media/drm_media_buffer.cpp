#include <stdlib.h>
#include <media/utils/image.h>
#include <media/drm_media_buffer.h>

#include "media_utils.h"
#include "drm_media_buffer_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

std::mutex g_handle_mb_mutex;
std::list<handle_mb_t *> g_handle_mb;

void *drm_mpi_mb_get_ptr(media_buffer_t mb)
{
    if (!mb) {
        return NULL;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->ptr;
}

int drm_mpi_mb_get_fd(media_buffer_t mb)
{
    if (!mb) {
        return 0;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->fd;
}

int drm_mpi_mb_get_handle(media_buffer_t mb)
{
    if (!mb) {
        return 0;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->handle;
}

int drm_mpi_mb_get_device_fd(media_buffer_t mb)
{
    if (!mb) {
        return 0;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->dev_fd;
}

size_t drm_mpi_mb_get_size(media_buffer_t mb)
{
    if (!mb) {
        return 0;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->size;
}

mod_id_e drm_mpi_mb_get_mode_id(media_buffer_t mb)
{
    if (!mb) {
        return MOD_ID_UNKNOW;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->mode_id;
}

int16_t drm_mpi_mb_get_channel_id(media_buffer_t mb)
{
    if (!mb) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->chn_id;
}

uint64_t drm_mpi_mb_get_timestamp(media_buffer_t mb)
{
    if (!mb) {
        return 0;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->timestamp;
}

int drm_mpi_mb_release_buffer(media_buffer_t mb)
{
    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb) {
        return -1;
    }

    if (mb_impl->media_mb) {
        mb_impl->media_mb.reset();
    }

    delete mb_impl;

    g_handle_mb_mutex.lock();
    for (auto it = g_handle_mb.begin(); it != g_handle_mb.end(); ++it) {
        auto p = *it;
        if (p->mb == mb) {
            g_handle_mb.erase(it);
            free(p);
            break;
        }
    }
    g_handle_mb_mutex.unlock();

    return 0;
}

int drm_mpi_mb_start_cpu_access(media_buffer_t mb, bool bReadonly)
{
    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb) {
        return -1;
    }

    if (mb_impl->media_mb) {
        mb_impl->media_mb->BeginCPUAccess(bReadonly);
    }

    return 0;
}

int drm_mpi_mb_finish_cpu_access(media_buffer_t mb, bool bReadonly)
{
    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb) {
        return -1;
    }

    if (mb_impl->media_mb) {
        mb_impl->media_mb->EndCPUAccess(bReadonly);
    }

    return 0;
}

media_buffer_t drm_mpi_mb_create_audio_bufferEx(mb_audio_info_t *pstAudioInfo, bool boolHardWare, uint8_t u8Flag)
{
    if (!pstAudioInfo || !pstAudioInfo->u32Channels || !pstAudioInfo->u32SampleRate || !pstAudioInfo->u32NBSamples) {
        return NULL;
    }

    std::string strSampleFormat = SampleFormatToString(pstAudioInfo->enSampleFmt);
    DrmSampleFormat mediaSampleFormat = StringToSampleFmt(strSampleFormat.c_str());
    if (mediaSampleFormat == DRM_SAMPLE_FMT_NONE) {
        DRM_MEDIA_LOGE("unsupport sample format");
        return NULL;
    }

    DrmSampleInfo mediaSampleInfo = {
        mediaSampleFormat, (int)pstAudioInfo->u32Channels,
        (int)pstAudioInfo->u32SampleRate, (int)pstAudioInfo->u32NBSamples
    };

    uint32_t buf_size = GetSampleSize(mediaSampleInfo) * pstAudioInfo->u32NBSamples;
    if (buf_size == 0) {
        return NULL;
    }

    media_buffer_impl_t *mb = new media_buffer_impl_t;
    if (!mb) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    uint32_t u32mediaBufFlag = 2;
    if (u8Flag == MB_FLAG_NOCACHED) {
        u32mediaBufFlag = 0;
    } else if (u8Flag == MB_FLAG_PHY_ADDR_CONSECUTIVE) {
        u32mediaBufFlag = 1;
    }

    auto &&media_mb = libdrm::MediaBuffer::Alloc(buf_size, boolHardWare ? libdrm::MediaBuffer::MemType::MEM_HARD_WARE : libdrm::MediaBuffer::MemType::MEM_COMMON, u32mediaBufFlag);
    if (!media_mb) {
        delete mb;
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    mb->media_mb = std::make_shared<libdrm::SampleBuffer>(*(media_mb.get()), mediaSampleInfo);
    mb->media_mb->SetValidSize(buf_size);
    mb->ptr = mb->media_mb->GetPtr();
    mb->fd = mb->media_mb->GetFD();
    mb->size = buf_size;
    mb->type = MB_TYPE_AUDIO;
    mb->stAudioInfo = *pstAudioInfo;
    mb->timestamp = 0;
    mb->mode_id = MOD_ID_UNKNOW;
    mb->chn_id = 0;
    mb->flag = 0;
    mb->tsvc_level = 0;

    return mb;
}

media_buffer_t drm_mpi_mb_create_audio_buffer(uint32_t u32BufferSize, bool boolHardWare)
{
    std::shared_ptr<libdrm::MediaBuffer> media_mb;
    if (u32BufferSize == 0) {
        media_mb = std::make_shared<libdrm::MediaBuffer>();
    } else {
        media_mb = libdrm::MediaBuffer::Alloc(u32BufferSize, boolHardWare ? libdrm::MediaBuffer::MemType::MEM_HARD_WARE : libdrm::MediaBuffer::MemType::MEM_COMMON);
    }

    media_buffer_impl_t *mb = new media_buffer_impl_t;
    if (!mb) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    if (!media_mb) {
        delete mb;
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    mb->media_mb = media_mb;
    mb->media_mb->SetValidSize(u32BufferSize);
    mb->ptr = media_mb->GetPtr();
    mb->fd = media_mb->GetFD();
    mb->handle = media_mb->GetHandle();
    mb->dev_fd = media_mb->GetDevFD();
    mb->size = u32BufferSize;
    mb->type = MB_TYPE_AUDIO;
    mb->timestamp = 0;
    mb->mode_id = MOD_ID_UNKNOW;
    mb->chn_id = 0;
    mb->flag = 0;
    mb->tsvc_level = 0;

    return mb;
}

media_buffer_t drm_mpi_mb_create_image_buffer(mb_image_info_t *pstImageInfo, bool boolHardWare, uint8_t u8Flag)
{
    if (!pstImageInfo || !pstImageInfo->u32Height || !pstImageInfo->u32Width || !pstImageInfo->u32VerStride || !pstImageInfo->u32HorStride) {
        return NULL;
    }

    std::string strPixFormat = ImageTypeToString(pstImageInfo->enImgType);
    DrmPixelFormat mediaPixFormat = StringToPixFmt(strPixFormat.c_str());
    if (mediaPixFormat == DRM_PIX_FMT_NONE) {
        DRM_MEDIA_LOGE("unsupport pixformat");
        return NULL;
    }

    uint32_t buf_size = CalPixFmtSize(mediaPixFormat, pstImageInfo->u32HorStride, pstImageInfo->u32VerStride, 16);
    if (buf_size == 0) {
        return NULL;
    }

    media_buffer_impl_t *mb = new media_buffer_impl_t;
    if (!mb) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    uint32_t u32mediaBufFlag = 2;
    if (u8Flag == MB_FLAG_NOCACHED) {
        u32mediaBufFlag = 0;
    } else if (u8Flag == MB_FLAG_PHY_ADDR_CONSECUTIVE) {
        u32mediaBufFlag = 1;
    }

    auto &&media_mb = libdrm::MediaBuffer::Alloc(buf_size, boolHardWare ? libdrm::MediaBuffer::MemType::MEM_HARD_WARE : libdrm::MediaBuffer::MemType::MEM_COMMON, u32mediaBufFlag);
    if (!media_mb) {
        delete mb;
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    DrmImageInfo mediaImageInfo = {
        mediaPixFormat, (int)pstImageInfo->u32Width,
        (int)pstImageInfo->u32Height,
        (int)pstImageInfo->u32HorStride,
        (int)pstImageInfo->u32VerStride
    };

    mb->media_mb = std::make_shared<libdrm::ImageBuffer>(*(media_mb.get()), mediaImageInfo);
    mb->media_mb->SetValidSize(buf_size);
    mb->ptr = mb->media_mb->GetPtr();
    mb->fd = mb->media_mb->GetFD();
    mb->handle = mb->media_mb->GetHandle();
    mb->dev_fd = mb->media_mb->GetDevFD();
    mb->size = buf_size;
    mb->type = MB_TYPE_IMAGE;
    mb->stImageInfo = *pstImageInfo;
    mb->timestamp = 0;
    mb->mode_id = MOD_ID_UNKNOW;
    mb->chn_id = 0;
    mb->flag = 0;
    mb->tsvc_level = 0;

    return mb;
}

int drm_mpi_mb_set_size(media_buffer_t mb, uint32_t size)
{
    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb_impl || !mb_impl->media_mb) {
        return -1;
    }

    mb_impl->size = size;
    if (mb_impl->media_mb) {
        mb_impl->media_mb->SetValidSize(size);
    }

    return 0;
}

int drm_mpi_mb_set_timestamp(media_buffer_t mb, uint64_t timestamp)
{
    if (!mb) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    mb_impl->timestamp = timestamp;
    if (mb_impl->media_mb) {
        mb_impl->media_mb->SetUSTimeStamp(timestamp);
    }

    return 0;
}

int drm_mpi_mb_get_flag(media_buffer_t mb)
{
    if (!mb) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->flag;
}

int drm_mpi_mb_get_tsvc_level(media_buffer_t mb)
{
    if (!mb) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    return mb_impl->tsvc_level;
}

media_buffer_t drm_mpi_mb_create_buffer(uint32_t u32Size, bool boolHardWare, uint8_t u8Flag)
{
    if (!u32Size) {
        DRM_MEDIA_LOGE("Invalid buffer size");
        return NULL;
    }

    media_buffer_impl_t *mb = new media_buffer_impl_t;
    if (!mb) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    uint32_t u32mediaBufFlag = 2;
    if (u8Flag == MB_FLAG_NOCACHED) {
        u32mediaBufFlag = 0;
    } else if (u8Flag == MB_FLAG_PHY_ADDR_CONSECUTIVE) {
        u32mediaBufFlag = 1;
    }

    mb->media_mb = libdrm::MediaBuffer::Alloc(u32Size, boolHardWare ? libdrm::MediaBuffer::MemType::MEM_HARD_WARE : libdrm::MediaBuffer::MemType::MEM_COMMON, u32mediaBufFlag);
    if (!mb->media_mb) {
        delete mb;
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    mb->media_mb->SetValidSize(u32Size);
    mb->ptr = mb->media_mb->GetPtr();
    mb->fd = mb->media_mb->GetFD();
    mb->handle = mb->media_mb->GetHandle();
    mb->dev_fd = mb->media_mb->GetDevFD();
    mb->size = u32Size;
    mb->type = MB_TYPE_COMMON;
    mb->timestamp = 0;
    mb->mode_id = MOD_ID_UNKNOW;
    mb->chn_id = 0;
    mb->flag = 0;
    mb->tsvc_level = 0;

    return mb;
}

media_buffer_t drm_mpi_mb_convert_to_image_buffer(media_buffer_t mb, mb_image_info_t *pstImageInfo)
{
    if (!mb || !pstImageInfo || !pstImageInfo->u32Height || !pstImageInfo->u32Width || !pstImageInfo->u32VerStride || !pstImageInfo->u32HorStride) {
        DRM_MEDIA_LOGE("invalid args");
        return NULL;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb_impl->media_mb) {
        DRM_MEDIA_LOGE("mediabuffer not init yet");
        return NULL;
    }

    std::string strPixFormat = ImageTypeToString(pstImageInfo->enImgType);
    DrmPixelFormat mediaPixFormat = StringToPixFmt(strPixFormat.c_str());
    if (mediaPixFormat == DRM_PIX_FMT_NONE) {
        DRM_MEDIA_LOGE("unsupport pixformat");
        return NULL;
    }

    uint32_t buf_size = CalPixFmtSize(mediaPixFormat, pstImageInfo->u32HorStride, pstImageInfo->u32VerStride, 1);
    if (buf_size > mb_impl->media_mb->GetSize()) {
        DRM_MEDIA_LOGE("buffer size:%zu do not match imgInfo(%dx%d, %s)", mb_impl->media_mb->GetSize(), pstImageInfo->u32HorStride, pstImageInfo->u32VerStride, strPixFormat.c_str());
        return NULL;
    }

    DrmImageInfo mediaImageInfo = {
        mediaPixFormat,
        (int)pstImageInfo->u32Width,
        (int)pstImageInfo->u32Height,
        (int)pstImageInfo->u32HorStride,
        (int)pstImageInfo->u32VerStride
    };

    mb_impl->media_mb = std::make_shared<libdrm::ImageBuffer>(*(mb_impl->media_mb.get()), mediaImageInfo);
    mb_impl->type = MB_TYPE_IMAGE;
    mb_impl->stImageInfo = *pstImageInfo;

    return mb_impl;
}

media_buffer_t drm_mpi_mb_convert_to_audio_bufferEx(media_buffer_t mb, mb_audio_info_t *pstAudioInfo)
{
    if (!mb || !pstAudioInfo || !pstAudioInfo->u32Channels || !pstAudioInfo->u32NBSamples || !pstAudioInfo->u32SampleRate) {
        DRM_MEDIA_LOGE("invalid args");
        return NULL;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb_impl->media_mb) {
        DRM_MEDIA_LOGE("mediabuffer not init yet");
        return NULL;
    }

    std::string strSampleFormat = SampleFormatToString(pstAudioInfo->enSampleFmt);
    DrmSampleFormat mediaSampleFormat = StringToSampleFmt(strSampleFormat.c_str());
    if (mediaSampleFormat == DRM_SAMPLE_FMT_NONE) {
        DRM_MEDIA_LOGE("unsupport sample format");
        return NULL;
    }

    DrmSampleInfo mediaSampleInfo = {
        mediaSampleFormat, (int)pstAudioInfo->u32Channels,
        (int)pstAudioInfo->u32SampleRate, (int)pstAudioInfo->u32NBSamples
    };

    uint32_t buf_size = GetSampleSize(mediaSampleInfo) * pstAudioInfo->u32NBSamples;
    if (buf_size == 0) {
        return NULL;
    }

    if (buf_size > mb_impl->media_mb->GetSize()) {
        DRM_MEDIA_LOGE("buffer size:%zu do not match AudioInfo(%dx%d%d, %s)", mb_impl->media_mb->GetSize(), pstAudioInfo->u32Channels, pstAudioInfo->u32SampleRate, pstAudioInfo->u32NBSamples, strSampleFormat.c_str());
        return NULL;
    }

    mb_impl->media_mb = std::make_shared<libdrm::SampleBuffer>(*(mb_impl->media_mb.get()), mediaSampleInfo);
    mb_impl->type = MB_TYPE_AUDIO;
    mb_impl->stAudioInfo = *pstAudioInfo;

    return mb_impl;
}

media_buffer_t drm_mpi_mb_convert_to_audio_buffer(media_buffer_t mb)
{
    if (!mb) {
        DRM_MEDIA_LOGE("invalid args");
        return NULL;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (!mb_impl->media_mb) {
        DRM_MEDIA_LOGE("mediabuffer not init yet");
        return NULL;
    }

    mb_impl->type = MB_TYPE_AUDIO;
    return mb_impl;
}

int drm_mpi_mb_get_image_info(media_buffer_t mb, mb_image_info_t *pstImageInfo)
{
    if (!mb || !pstImageInfo) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (mb_impl->type != MB_TYPE_IMAGE) {
        return -1;
    }

    *pstImageInfo = mb_impl->stImageInfo;
    return 0;
}

int drm_mpi_mb_get_audio_info(media_buffer_t mb, mb_audio_info_t *pstAudioInfo)
{
    if (!mb || !pstAudioInfo) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (mb_impl->type != MB_TYPE_IMAGE) {
        return -1;
    }

    *pstAudioInfo = mb_impl->stAudioInfo;
    return 0;
}

int drm_mpi_mb_timestamp_node_dump(media_buffer_t mb)
{
    if (!mb) {
        return -1;
    }

    media_buffer_impl_t *mb_impl = (media_buffer_impl_t *)mb;
    if (mb_impl->media_mb) {
        mb_impl->media_mb->TimeStampDump();
    }

    return 0;
}

media_buffer_t drm_mpi_mb_copy(media_buffer_t mb, bool bZeroCopy)
{
    media_buffer_impl_t *mb_old = (media_buffer_impl_t *)mb;
    media_buffer_impl_t *mb_new = new media_buffer_impl_t;
    if (!mb_new) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    if (bZeroCopy) {
        *mb_new = *mb_old;
    } else {
        DRM_MEDIA_LOGE("not support DeepCopy");
        delete mb_new;
        return NULL;
    }

    return mb_new;
}

media_buffer_pool_t drm_mpi_mb_pool_create(mb_pool_param_t *pstPoolParam)
{
    uint32_t u32Cnt;
    uint32_t u32Size;
    bool bIsHardWare;
    uint16_t u16Flag;
    mb_type_e enMediaType;
    mb_image_info_t stImageInfo;
    DrmPixelFormat mediaPixFormat;

    if (!pstPoolParam) {
        DRM_MEDIA_LOGE("param is NULL");
        return NULL;
    }

    u32Cnt = pstPoolParam->u32Cnt;
    u32Size = pstPoolParam->u32Size;
    bIsHardWare = pstPoolParam->bHardWare;
    u16Flag = pstPoolParam->u16Flag;
    enMediaType = pstPoolParam->enMediaType;

    if (enMediaType & MB_TYPE_VIDEO) {
        stImageInfo = pstPoolParam->stImageInfo;
        if ((stImageInfo.enImgType <= DRM_IMAGE_TYPE_UNKNOW)
            || (stImageInfo.enImgType > DRM_IMAGE_TYPE_BUTT)
            || (!stImageInfo.u32Width)
            || (!stImageInfo.u32Height)
            || (stImageInfo.u32HorStride < stImageInfo.u32Width)
            || (stImageInfo.u32VerStride < stImageInfo.u32Height))
        {
            DRM_MEDIA_LOGE("Invalid param");
            return NULL;
        }

        std::string strPixFormat = ImageTypeToString(stImageInfo.enImgType);
        mediaPixFormat = StringToPixFmt(strPixFormat.c_str());
        if (mediaPixFormat == DRM_PIX_FMT_NONE) {
            DRM_MEDIA_LOGE("unsupport pixformat");
            return NULL;
        }

        u32Size = CalPixFmtSize(mediaPixFormat, stImageInfo.u32HorStride, stImageInfo.u32VerStride, 16);
    }

    if (!u32Cnt || !u32Size) {
        DRM_MEDIA_LOGE("Invalid cnt(%d) or size(%d)", u32Cnt, u32Size);
        return NULL;
    }

    media_buffer_pool_impl_t *pstMBPImpl = new media_buffer_pool_impl_t;
    if (!pstMBPImpl) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    uint32_t u32mediaBufFlag = 2;
    if (u16Flag == MB_FLAG_NOCACHED) {
        u32mediaBufFlag = 0;
    } else if (u16Flag == MB_FLAG_PHY_ADDR_CONSECUTIVE) {
        u32mediaBufFlag = 1;
    }

    pstMBPImpl->media_mbp = std::make_shared<libdrm::BufferPool>(u32Cnt, u32Size, bIsHardWare ? libdrm::MediaBuffer::MemType::MEM_HARD_WARE : libdrm::MediaBuffer::MemType::MEM_COMMON, u32mediaBufFlag);

    pstMBPImpl->enType = enMediaType;
    pstMBPImpl->u32Size = u32Size;
    pstMBPImpl->u32Cnt = u32Cnt;
    if (enMediaType & MB_TYPE_VIDEO) {
        pstMBPImpl->stImageInfo = stImageInfo;
        pstMBPImpl->mediaImgInfo.pix_fmt = mediaPixFormat;
        pstMBPImpl->mediaImgInfo.width = stImageInfo.u32Width;
        pstMBPImpl->mediaImgInfo.height = stImageInfo.u32Height;
        pstMBPImpl->mediaImgInfo.vir_width = stImageInfo.u32HorStride;
        pstMBPImpl->mediaImgInfo.vir_height = stImageInfo.u32VerStride;
    }

    return (media_buffer_pool_t)pstMBPImpl;
}

media_buffer_t drm_mpi_mb_pool_get_buffer(media_buffer_pool_t MBPHandle, bool bIsBlock)
{
    if (!MBPHandle) {
        DRM_MEDIA_LOGE("handle is null");
        return NULL;
    }

    media_buffer_pool_impl_t *pstMBPImpl;
    pstMBPImpl = (media_buffer_pool_impl_t *)MBPHandle;

    if (!pstMBPImpl->media_mbp) {
        DRM_MEDIA_LOGE("invalid handle");
        return NULL;
    }

    bool boolBlock = bIsBlock;
    auto media_mb = pstMBPImpl->media_mbp->GetBuffer(boolBlock);
    if (!media_mb) {
        DRM_MEDIA_LOGE("no buffer available");
        return NULL;
    }

    media_buffer_impl_t *mb = new media_buffer_impl_t;
    if (!mb) {
        DRM_MEDIA_LOGE("no space left");
        return NULL;
    }

    if (pstMBPImpl->enType & MB_TYPE_VIDEO) {
        auto img_mb = std::make_shared<libdrm::ImageBuffer>(*(media_mb.get()), pstMBPImpl->mediaImgInfo);
        mb->type = pstMBPImpl->enType;
        mb->stImageInfo = pstMBPImpl->stImageInfo;
        mb->media_mb = img_mb;
    } else if (pstMBPImpl->enType & MB_TYPE_AUDIO) {
        mb->type = pstMBPImpl->enType;
        mb->media_mb = media_mb;
    } else {
        mb->type = MB_TYPE_COMMON;
        mb->media_mb = media_mb;
    }

    mb->ptr = mb->media_mb->GetPtr();
    mb->fd = mb->media_mb->GetFD();
    mb->handle = mb->media_mb->GetHandle();
    mb->dev_fd = mb->media_mb->GetDevFD();
    mb->size = mb->media_mb->GetSize();
    mb->timestamp = 0;
    mb->mode_id = MOD_ID_UNKNOW;
    mb->chn_id = 0;
    mb->flag = 0;
    mb->tsvc_level = 0;

    return mb;
}

int drm_mpi_mb_pool_destroy(media_buffer_pool_t MBPHandle)
{
    if (!MBPHandle) {
        return -1;
    }

    media_buffer_pool_impl_t *pstMBPImpl;
    pstMBPImpl = (media_buffer_pool_impl_t *)MBPHandle;

    if (pstMBPImpl->media_mbp) {
        pstMBPImpl->media_mbp.reset();
    }

    delete pstMBPImpl;
    return 0;
}

#ifdef __cplusplus
}
#endif
