#ifndef LIBDRM_MEDIA_BUFFER_H
#define LIBDRM_MEDIA_BUFFER_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MB_FLAG_NOCACHED                    0x01
#define MB_FLAG_PHY_ADDR_CONSECUTIVE        0x02

#define MB_TYPE_IMAGE_MASK                  0x0100
#define MB_TYPE_VIDEO_MASK                  0x0200
#define MB_TYPE_AUDIO_MASK                  0x0400

typedef void *media_buffer_t;
typedef void *media_buffer_pool_t;

typedef void (*OutCallbackFunction)(media_buffer_t mb);
typedef void (*EventCallbackFunction)(void *pHandle, void *event);
typedef void (*OutCallbackFunctionEx)(media_buffer_t mb, void *pHandle);

typedef enum MB_TYPE {
    MB_TYPE_COMMON = 0,
    MB_TYPE_IMAGE  = MB_TYPE_IMAGE_MASK | 0x0000,
    MB_TYPE_VIDEO  = MB_TYPE_VIDEO_MASK | 0x0000,
    MB_TYPE_H264   = MB_TYPE_VIDEO_MASK | 0x0001,
    MB_TYPE_H265   = MB_TYPE_VIDEO_MASK | 0x0002,
    MB_TYPE_JPEG   = MB_TYPE_VIDEO_MASK | 0x0003,
    MB_TYPE_MJPEG  = MB_TYPE_VIDEO_MASK | 0x0004,
    MB_TYPE_AUDIO  = MB_TYPE_AUDIO_MASK | 0x0000,
} mb_type_e;

typedef struct MB_IMAGE_INFO {
    uint32_t         u32Width;
    uint32_t         u32Height;
    uint32_t         u32HorStride;
    uint32_t         u32VerStride;
    drm_image_type_e enImgType;
} mb_image_info_t;

typedef struct MB_AUDIO_INFO {
    uint32_t            u32Channels;
    uint32_t            u32SampleRate;
    uint32_t            u32NBSamples;
    drm_sample_format_e enSampleFmt;
} mb_audio_info_t;

typedef struct MB_POOL_PARAM_S {
    mb_type_e           enMediaType;
    uint32_t            u32Cnt;
    uint32_t            u32Size;
    bool                bHardWare;
    uint16_t            u16Flag;
    union {
        mb_image_info_t stImageInfo;
    };
} mb_pool_param_t;

typedef struct HANDLE_MB {
    void *handle;
    void *mb;
} handle_mb_t;

int drm_mpi_mb_get_fd(media_buffer_t mb);
void *drm_mpi_mb_get_ptr(media_buffer_t mb);
int drm_mpi_mb_get_handle(media_buffer_t mb);
size_t drm_mpi_mb_get_size(media_buffer_t mb);
int drm_mpi_mb_get_device_fd(media_buffer_t mb);

mod_id_e drm_mpi_mb_get_mode_id(media_buffer_t mb);
int16_t drm_mpi_mb_get_channel_id(media_buffer_t mb);
uint64_t drm_mpi_mb_get_timestamp(media_buffer_t mb);

int drm_mpi_mb_release_buffer(media_buffer_t mb);
media_buffer_t drm_mpi_mb_create_buffer(uint32_t u32Size, bool boolHardWare, uint8_t u8Flag);

media_buffer_t drm_mpi_mb_convert_to_audio_buffer(media_buffer_t mb);
media_buffer_t drm_mpi_mb_convert_to_image_buffer(media_buffer_t mb, mb_image_info_t *pstImageInfo);
media_buffer_t drm_mpi_mb_convert_to_audio_bufferEx(media_buffer_t mb, mb_audio_info_t *pstAudioInfo);

media_buffer_t drm_mpi_mb_create_audio_buffer(uint32_t u32BufferSize, bool boolHardWare);
media_buffer_t drm_mpi_mb_create_image_buffer(mb_image_info_t *pstImageInfo, bool boolHardWare, uint8_t u8Flag);
media_buffer_t drm_mpi_mb_create_audio_bufferEx(mb_audio_info_t *pstAudioInfo, bool boolHardWare, uint8_t u8Flag);

int drm_mpi_mb_set_size(media_buffer_t mb, uint32_t size);
int drm_mpi_mb_set_timestamp(media_buffer_t mb, uint64_t timestamp);

int drm_mpi_mb_get_flag(media_buffer_t mb);
int drm_mpi_mb_get_tsvc_level(media_buffer_t mb);

int drm_mpi_mb_get_image_info(media_buffer_t mb, mb_image_info_t *pstImageInfo);
int drm_mpi_mb_get_audio_info(media_buffer_t mb, mb_audio_info_t *pstAudioInfo);

int drm_mpi_mb_start_cpu_access(media_buffer_t mb, bool bReadonly);
int drm_mpi_mb_finish_cpu_access(media_buffer_t mb, bool bReadonly);

int drm_mpi_mb_timestamp_node_dump(media_buffer_t mb);
media_buffer_t drm_mpi_mb_copy(media_buffer_t mb, bool bZeroCopy);

int drm_mpi_mb_pool_destroy(media_buffer_pool_t MBPHandle);
media_buffer_pool_t drm_mpi_mb_pool_create(mb_pool_param_t *pstPoolParam);
media_buffer_t drm_mpi_mb_pool_get_buffer(media_buffer_pool_t MBPHandle, bool bIsBlock);

#ifdef __cplusplus
}
#endif

#endif
