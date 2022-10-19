#ifndef LIBDRM_MEDIA_VENC_H
#define LIBDRM_MEDIA_VENC_H

#include "drm_media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RC_HEIR_SIZE            4
#define RC_TEXTURE_THR_SIZE     16
#define VENC_RGN_COLOR_NUM      256

enum DRM_VENC_CHANNELS {
    DRM_VENC_CHANNEL_00 = 0,
    DRM_VENC_CHANNEL_02,
    DRM_VENC_CHANNEL_03,
    DRM_VENC_CHANNEL_04,
    DRM_VENC_CHANNEL_05,
    DRM_VENC_CHANNEL_06,
    DRM_VENC_CHANNEL_07,
    DRM_VENC_CHANNEL_08,
    DRM_VENC_CHANNEL_09,
    DRM_VENC_CHANNEL_10,
    DRM_VENC_CHANNEL_11,
    DRM_VENC_CHANNEL_12,
    DRM_VENC_CHANNEL_13,
    DRM_VENC_CHANNEL_14,
    DRM_VENC_CHANNEL_15,
    DRM_VENC_CHANNEL_BUTT
};

typedef enum VENC_RC_QUALITY_E {
    VENC_RC_QUALITY_HIGHEST,
    VENC_RC_QUALITY_HIGHER,
    VENC_RC_QUALITY_HIGH,
    VENC_RC_QUALITY_MEDIUM,
    VENC_RC_QUALITY_LOW,
    VENC_RC_QUALITY_LOWER,
    VENC_RC_QUALITY_LOWEST,
    VENC_RC_QUALITY_BUTT,
} drm_venc_rc_quality_e;

typedef enum VENC_RC_MODE_E {
    VENC_RC_MODE_H264CBR = 1,
    VENC_RC_MODE_H264VBR,
    VENC_RC_MODE_H264AVBR,
    VENC_RC_MODE_MJPEGCBR,
    VENC_RC_MODE_MJPEGVBR,
    VENC_RC_MODE_H265CBR,
    VENC_RC_MODE_H265VBR,
    VENC_RC_MODE_H265AVBR,
    VENC_RC_MODE_BUTT,
} drm_venc_rc_mode_e;

typedef enum VENC_NALU_TYPE_E {
    VENC_NALU_BSLICE = 1,
    VENC_NALU_PSLICE,
    VENC_NALU_ISLICE,
    VENC_NALU_IDRSLICE,
    VENC_NALU_SEI,
    VENC_NALU_VPS,
    VENC_NALU_SPS,
    VENC_NALU_PPS,
    VENC_NALU_BUTT
} drm_venc_nalu_type_e;

typedef struct VENC_H264_CBR_S {
    uint32_t u32Gop;
    uint32_t u32SrcFrameRateNum;
    uint32_t u32SrcFrameRateDen;
    uint32_t fr32DstFrameRateNum;
    uint32_t fr32DstFrameRateDen;
    uint32_t u32BitRate;
} drm_venc_h264_cbr_t, drm_venc_h265_cbr_t;

typedef struct VENC_H264_VBR_S {
    uint32_t u32Gop;
    uint32_t u32SrcFrameRateNum;
    uint32_t u32SrcFrameRateDen;
    uint32_t fr32DstFrameRateNum;
    uint32_t fr32DstFrameRateDen;
    uint32_t u32MaxBitRate;
} drm_venc_h264_vbr_t, drm_venc_h265_vbr_t;

typedef struct VENC_H264_AVBR_S {
    uint32_t u32Gop;
    uint32_t u32SrcFrameRateNum;
    uint32_t u32SrcFrameRateDen;
    uint32_t fr32DstFrameRateNum;
    uint32_t fr32DstFrameRateDen;
    uint32_t u32MaxBitRate;
} drm_venc_h264_avbr_t, drm_venc_h265_avbr_t;

typedef struct VENC_MJPEG_CBR_S {
    uint32_t u32SrcFrameRateNum;
    uint32_t u32SrcFrameRateDen;
    uint32_t fr32DstFrameRateNum;
    uint32_t fr32DstFrameRateDen;
    uint32_t u32BitRate;
} drm_venc_mjpeg_cbr_t;

typedef struct VENC_MJPEG_VBR_S {
    uint32_t u32SrcFrameRateNum;
    uint32_t u32SrcFrameRateDen;
    uint32_t fr32DstFrameRateNum;
    uint32_t fr32DstFrameRateDen;
    uint32_t u32BitRate;
} drm_venc_mjpeg_vbr_t;

typedef struct VENC_RC_ATTR_S {
    drm_venc_rc_mode_e       enRcMode;
    union {
        drm_venc_h264_cbr_t  stH264Cbr;
        drm_venc_h264_vbr_t  stH264Vbr;
        drm_venc_h264_avbr_t stH264Avbr;

        drm_venc_mjpeg_cbr_t stMjpegCbr;
        drm_venc_mjpeg_vbr_t stMjpegVbr;

        drm_venc_h265_cbr_t  stH265Cbr;
        drm_venc_h265_vbr_t  stH265Vbr;
        drm_venc_h265_avbr_t stH265Avbr;
    };
} drm_venc_rc_attr_t;

typedef enum VENC_GOP_MODE_E {
    VENC_GOPMODE_NORMALP = 0,
    VENC_GOPMODE_TSVC,
    VENC_GOPMODE_SMARTP,
    VENC_GOPMODE_BUTT,
} drm_venc_gop_mode_e;

typedef enum VENC_ROTATION_E {
    VENC_ROTATION_0   = 0,
    VENC_ROTATION_90  = 90,
    VENC_ROTATION_180 = 180,
    VENC_ROTATION_270 = 270,
    VENC_ROTATION_BUTT
} drm_venc_rotation_e;

typedef struct VENC_MPF_CFG_S {
    uint8_t    u8LargeThumbNailNum;
    drm_size_t astLargeThumbNailSize[2];
} drm_venc_mpf_cfg_t;

typedef enum VENC_PIC_RECEIVE_MODE_E {
    VENC_PIC_RECEIVE_SINGLE = 0,
    VENC_PIC_RECEIVE_MULTI,
    VENC_PIC_RECEIVE_BUTT
} drm_venc_pic_receive_mode_e;

typedef struct VENC_ATTR_JPEG_S {
    uint32_t                    u32ZoomWidth;
    uint32_t                    u32ZoomHeight;
    uint32_t                    u32ZoomVirWidth;
    uint32_t                    u32ZoomVirHeight;
    bool                        bSupportDCF;
    drm_venc_mpf_cfg_t          stMPFCfg;
    drm_venc_pic_receive_mode_e enReceiveMode;
} drm_venc_attr_jpeg_t;

typedef struct VENC_ATTR_MJPEG_S {
    uint32_t u32ZoomWidth;
    uint32_t u32ZoomHeight;
    uint32_t u32ZoomVirWidth;
    uint32_t u32ZoomVirHeight;
} drm_venc_attr_mjpeg_t;

typedef struct VENC_ATTR_H264_S {
    uint32_t u32Level;
} drm_venc_attr_h264_t;

typedef struct VENC_ATTR_H265_S {
    bool bScaleList;
} drm_venc_attr_h265_t;

typedef struct VENC_ATTR_S {
    drm_codec_type_e          enType;
    drm_image_type_e          imageType;
    uint32_t                  u32VirWidth;
    uint32_t                  u32VirHeight;
    uint32_t                  u32Profile;
    bool                      bByFrame;
    uint32_t                  u32PicWidth;
    uint32_t                  u32PicHeight;
    drm_venc_rotation_e       enRotation;
    bool                      bFullRange;
    union {
        drm_venc_attr_h264_t  stAttrH264e;
        drm_venc_attr_h265_t  stAttrH265e;
        drm_venc_attr_mjpeg_t stAttrMjpege;
        drm_venc_attr_jpeg_t  stAttrJpege;
    };
} drm_venc_attr_t;

typedef struct VENC_GOP_ATTR_S {
    drm_venc_gop_mode_e enGopMode;
    uint32_t            u32GopSize;
    int                 s32IPQpDelta;
    uint32_t            u32BgInterval;
    int                 s32ViQpDelta;
} drm_venc_gop_attr_t;

typedef struct VENC_CHN_ATTR_S {
    drm_venc_attr_t     stVencAttr;
    drm_venc_rc_attr_t  stRcAttr;
    drm_venc_gop_attr_t stGopAttr;
} drm_venc_chn_attr_t;

typedef struct VENC_CROP_INFO_S {
    bool       bEnable;
    drm_rect_t stRect;
} drm_venc_crop_info_t;

typedef struct VENC_FRAME_RATE_S {
    int s32SrcFrmRate;
    int s32DstFrmRate;
} drm_venc_frame_rate_t;

typedef struct VENC_CHN_PARAM_S {
    bool                  bColor2Grey;
    uint32_t              u32Priority;
    uint32_t              u32MaxStrmCnt;
    uint32_t              u32PollWakeUpFrmCnt;
    drm_venc_crop_info_t  stCropCfg;
    drm_venc_frame_rate_t stFrameRate;
} drm_venc_chn_param_t;

typedef struct VENC_PARAM_H264_S {
    uint32_t u32StepQp;
    uint32_t u32MaxQp;
    uint32_t u32MinQp;
    uint32_t u32MaxIQp;
    uint32_t u32MinIQp;
} drm_venc_param_h264_t;

typedef struct VENC_PARAM_H265_S {
    uint32_t u32StepQp;
    uint32_t u32MaxQp;
    uint32_t u32MinQp;
    uint32_t u32MaxIQp;
    uint32_t u32MinIQp;
} drm_venc_param_h265_t;

typedef struct VENC_PARAM_MJPEG_S {

} drm_venc_param_mjpeg_t;

typedef struct VENC_RC_PARAM_S {
    uint32_t                   u32ThrdI[RC_TEXTURE_THR_SIZE];
    uint32_t                   u32ThrdP[RC_TEXTURE_THR_SIZE];
    uint32_t                   u32RowQpDeltaI;
    uint32_t                   u32RowQpDeltaP;

    bool                       bEnableHierQp;
    int                        s32HierQpDelta[RC_HEIR_SIZE];
    int                        s32HierFrameNum[RC_HEIR_SIZE];

    uint32_t                   s32FirstFrameStartQp;
    union {
        drm_venc_param_h264_t  stParamH264;
        drm_venc_param_h265_t  stParamH265;
        drm_venc_param_mjpeg_t stParamMjpeg;
    };
} drm_venc_rc_param_t;

typedef struct VENC_ROI_ATTR_S {
    uint32_t   u32Index;
    bool       bEnable;
    bool       bAbsQp;
    int        s32Qp;
    bool       bIntra;
    drm_rect_t stRect;
} drm_venc_roi_attr_t;

typedef struct VENC_COLOR_TBL {
    uint32_t u32ArgbTbl[VENC_RGN_COLOR_NUM];
    bool     bColorDichotomyEnable;
} drm_venc_color_tbl_t;

typedef enum OSD_REGION_ID_E {
    REGION_ID_0 = 0,
    REGION_ID_1,
    REGION_ID_2,
    REGION_ID_3,
    REGION_ID_4,
    REGION_ID_5,
    REGION_ID_6,
    REGION_ID_7
} drm_osd_region_id_e;

typedef enum OSD_PIXEL_FORMAT_E {
    PIXEL_FORMAT_ARGB_8888 = 0,
    PIXEL_FORMAT_BUTT
} drm_osd_pixel_format_e;

typedef struct BITMAP_S {
    drm_osd_pixel_format_e enPixelFormat;
    uint32_t               u32Width;
    uint32_t               u32Height;
    void                   *pData;
} drm_bitmap_t;

typedef struct COVER_INFO_S {
    drm_osd_pixel_format_e enPixelFormat;
    uint32_t               u32Color;
} drm_cover_info_t;

typedef struct OSD_COLOR_PALETTE_BUF_S {
    uint32_t u32Width;
    uint32_t u32Height;
    void     *pIdBuf;
} drm_osd_color_palette_buf_t;

typedef struct OSD_REGION_INFO_S {
    drm_osd_region_id_e enRegionId;
    uint32_t            u32PosX;
    uint32_t            u32PosY;
    uint32_t            u32Width;
    uint32_t            u32Height;
    uint8_t             u8Inverse;
    uint8_t             u8Enable;
} drm_osd_region_info_t;

typedef struct VENC_JPEG_PARAM_S {
    uint32_t u32Qfactor;
    uint8_t  u8YQt[64];
    uint8_t  u8CbQt[64];
    uint8_t  u8CrQt[64];
    uint32_t u32MCUPerECS;
} drm_venc_jpeg_param_t;

typedef struct VENC_RESOLUTION_PARAM_S {
    uint32_t u32Width;
    uint32_t u32Height;
    uint32_t u32VirWidth;
    uint32_t u32VirHeight;
} drm_venc_resolution_param_t;

typedef struct VENC_CHN_STATUS_S {
    uint32_t u32LeftFrames;
    uint32_t u32TotalFrames;
    uint32_t u32LeftPackets;
    uint32_t u32TotalPackets;
} drm_venc_chn_status_t;

typedef enum RC_SUPERFRM_MODE_E {
    SUPERFRM_NONE = 0,
    SUPERFRM_DISCARD,
    SUPERFRM_REENCODE,
    SUPERFRM_BUTT
} drm_venc_superfrm_mode_e;

typedef enum VENC_RC_PRIORITY_E {
    VENC_RC_PRIORITY_BITRATE_FIRST = 1,
    VENC_RC_PRIORITY_FRAMEBITS_FIRST,
    VENC_RC_PRIORITY_BUTT,
} drm_venc_rc_priority_e;

typedef struct VENC_SUPERFRAME_CFG_S {
    drm_venc_superfrm_mode_e enSuperFrmMode;
    uint32_t                 u32SuperIFrmBitsThr;
    uint32_t                 u32SuperPFrmBitsThr;
    drm_venc_rc_priority_e   enRcPriority;
} drm_venc_superframe_cfg_t;

#ifdef __cplusplus
}
#endif

#endif
