#ifndef LIBDRM_MEDIA_COMMON_H
#define LIBDRM_MEDIA_COMMON_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MOD_ID_E {
    MOD_ID_UNKNOW = 0,
    MOD_ID_VB,
    MOD_ID_SYS,
    MOD_ID_VDEC,
    MOD_ID_VENC,
    MOD_ID_VO,
    MOD_ID_VI,
    MOD_ID_VP,
    MOD_ID_RGA,
    MOD_ID_VMIX,
    MOD_ID_BUTT
} mod_id_e;

typedef enum DRM_IMAGE_TYPE_E {
    DRM_IMAGE_TYPE_UNKNOW = 0,
    DRM_IMAGE_TYPE_GRAY8,
    DRM_IMAGE_TYPE_GRAY16,
    DRM_IMAGE_TYPE_YUV420P,
    DRM_IMAGE_TYPE_NV12,
    DRM_IMAGE_TYPE_NV21,
    DRM_IMAGE_TYPE_YV12,
    DRM_IMAGE_TYPE_FBC2,
    DRM_IMAGE_TYPE_FBC0,
    DRM_IMAGE_TYPE_YUV422P,
    DRM_IMAGE_TYPE_NV16,
    DRM_IMAGE_TYPE_NV61,
    DRM_IMAGE_TYPE_YV16,
    DRM_IMAGE_TYPE_YUYV422,
    DRM_IMAGE_TYPE_UYVY422,
    DRM_IMAGE_TYPE_YUV444SP,
    DRM_IMAGE_TYPE_RGB332,
    DRM_IMAGE_TYPE_RGB565,
    DRM_IMAGE_TYPE_BGR565,
    DRM_IMAGE_TYPE_RGB888,
    DRM_IMAGE_TYPE_BGR888,
    DRM_IMAGE_TYPE_ARGB8888,
    DRM_IMAGE_TYPE_ABGR8888,
    DRM_IMAGE_TYPE_RGBA8888,
    DRM_IMAGE_TYPE_BGRA8888,
    DRM_IMAGE_TYPE_JPEG,
    DRM_IMAGE_TYPE_XRGB8888,
    DRM_IMAGE_TYPE_BUTT
} drm_image_type_e;

typedef enum {
    DRM_CODEC_TYPE_NONE = -1,
    DRM_CODEC_TYPE_MP3,
    DRM_CODEC_TYPE_MP2,
    DRM_CODEC_TYPE_G711A,
    DRM_CODEC_TYPE_G711U,
    DRM_CODEC_TYPE_G726,
    DRM_CODEC_TYPE_H264,
    DRM_CODEC_TYPE_H265,
    DRM_CODEC_TYPE_JPEG,
    DRM_CODEC_TYPE_MJPEG,
    DRM_CODEC_TYPE_NB
} DrmCodecType, drm_codec_type_e;

typedef enum {
    DRM_SAMPLE_FMT_NONE = -1,
    DRM_SAMPLE_FMT_U8,
    DRM_SAMPLE_FMT_S16,
    DRM_SAMPLE_FMT_S32,
    DRM_SAMPLE_FMT_FLT,
    DRM_SAMPLE_FMT_U8P,
    DRM_SAMPLE_FMT_S16P,
    DRM_SAMPLE_FMT_S32P,
    DRM_SAMPLE_FMT_FLTP,
    DRM_SAMPLE_FMT_G711A,
    DRM_SAMPLE_FMT_G711U,
    DRM_SAMPLE_FMT_NB
} DrmSampleFormat, drm_sample_format_e;

typedef struct SIZE_S {
    uint32_t u32Width;
    uint32_t u32Height;
} drm_size_t;

typedef struct RECT_S {
    int32_t  s32X;
    int32_t  s32Y;
    uint32_t u32Width;
    uint32_t u32Height;
} drm_rect_t;

typedef struct DRM_CHN_S {
    mod_id_e enModId;
    int      s32DevId;
    int      s32ChnId;
} drm_chn_t;

typedef struct DRM_FPS_ATTR_S {
    int s32FpsInNum;
    int s32FpsInDen;
    int s32FpsOutNum;
    int s32FpsOutDen;
} drm_fps_attr_t;

typedef struct DRM_RECV_PIC_PARAM_S {
    int s32RecvPicNum;
} drm_recv_pic_param_t;

#ifdef __cplusplus
}
#endif

#endif
