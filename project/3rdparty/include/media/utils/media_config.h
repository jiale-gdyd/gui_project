#ifndef LIBDRM_DISPLAY_MEDIA_CONFIG_H
#define LIBDRM_DISPLAY_MEDIA_CONFIG_H

#include "flow.h"
#include "image.h"
#include "sound.h"
#include "media_type.h"

typedef struct {
    DrmImageInfo image_info;
    DrmImageRect rect_info;
    DrmCodecType codec_type;

    int          qfactor;
    int          dcf;
    int          mpf_cnt;
    int          mpfw[2];
    int          mpfh[2];
} DrmImageConfig;

typedef struct {
    char       *type;
    uint32_t   max_bps;
    const char *rc_quality;
    const char *rc_mode;
    uint16_t   fps;
    uint16_t   gop;
    uint8_t    profile;
    uint8_t    enc_levle;
} DrmVideoEncoderCfg;

typedef struct {
    int qp_init;
    int qp_step;
    int qp_min;
    int qp_max;
    int qp_min_i;
    int qp_max_i;
    int thrd_i[16];
    int thrd_p[16];
    int row_qp_delta_i;
    int row_qp_delta_p;
    int hier_qp_en;
    int hier_qp_delta[4];
    int hier_frame_num[4];
} DrmVideoEncoderQp;

typedef struct {
    DrmImageConfig    image_cfg;
    DrmVideoEncoderQp encode_qp;
    int               bit_rate;
    int               bit_rate_max;
    int               bit_rate_min;
    int               frame_rate;
    int               frame_rate_den;
    int               frame_in_rate;
    int               frame_in_rate_den;
    int               trans_8x8;
    int               level;
    int               gop_size;
    int               profile;
    int               full_range;
    int               ref_frm_cfg;
    int               rotation;
    const char        *rc_quality;
    const char        *rc_mode;
    int               scaling_list;
} DrmVideoConfig;

typedef struct {
    DrmSampleInfo sample_info;
    DrmCodecType  codec_type;
    int           bit_rate;
    float         quality;
} DrmAudioConfig;

typedef struct {
    union {
        DrmVideoConfig vid_cfg;
        DrmImageConfig img_cfg;
        DrmAudioConfig aud_cfg;
    };
    DrmType            type;
} DrmMediaConfig;

#define OSD_REGIONS_CNT 8

typedef enum {
    REGION_TYPE_OVERLAY = 0,
    REGION_TYPE_COVER,
    REGION_TYPE_MOSAIC,
    REGION_TYPE_OVERLAY_EX,
    REGION_TYPE_COVER_EX,
} drm_region_type_e;

typedef struct {
    uint8_t           *buffer;
    uint32_t          pos_x;
    uint32_t          pos_y;
    uint32_t          width;
    uint32_t          height;
    uint32_t          inverse;
    uint32_t          region_id;
    uint8_t           enable;
    drm_region_type_e region_type;
    uint32_t          cover_color;
} DrmOsdRegionData;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t intra;
    int16_t  quality;
    uint16_t qp_area_idx;
    uint8_t  area_map_en;
    uint8_t  abs_qp_en;
} DrmEncROIRegion;

typedef enum {
    GOP_MODE_NORMALP = 0,
    GOP_MODE_TSVC2,
    GOP_MODE_TSVC3,
    GOP_MODE_TSVC4,
    GOP_MODE_SMARTP,
} DrmEncGopMode;

typedef struct {
    DrmEncGopMode mode;
    int           gop_size;
    int           ip_qp_delta;
    int           interval;
    int           vi_qp_delta;
} DrmEncGopModeParam;

typedef struct {
    int width;
    int height;
    int vir_width;
    int vir_height;
    int x;
    int y;
    int w;
    int h;
} DrmVideoResolutionCfg;

typedef enum EncSuperFrmMode_ {
    DRM_MEDIA_SUPERFRM_NONE = 0,
    DRM_MEDIA_SUPERFRM_DISCARD,
    DRM_MEDIA_SUPERFRM_REENCODE,
    DRM_MEDIA_SUPERFRM_BUTT
} DrmEncSuperFrmMode;

typedef enum VencRcPriority_ {
    DRM_MEDIA_VENC_RC_PRIORITY_BITRATE_FIRST = 0,
    DRM_MEDIA_VENC_RC_PRIORITY_FRAMEBITS_FIRST,
    DRM_MEDIA_VENC_RC_PRIORITY_BUTT,
} DrmVencRcPriority;

typedef struct VencSuperFrmCfg_ {
    DrmEncSuperFrmMode SuperFrmMode;
    unsigned int       SuperIFrmBitsThr;
    unsigned int       SuperPFrmBitsThr;
    DrmVencRcPriority  RcPriority;
} DrmVencSuperFrmCfg;

typedef struct RgaConfig_ {
    DrmImageRect src_rect;
    DrmImageRect dst_rect;
    int          rotation;
    int          flip;
} DrmRgaConfig;

#include <map>

namespace libdrm {
extern const char *rc_mode_strings[4];
extern const char *rc_quality_strings[7];

const char *ConvertRcQuality(const std::string &s);
const char *ConvertRcMode(const std::string &s);
bool ParseMediaConfigFromMap(std::map<std::string, std::string> &params, DrmMediaConfig &mc);

std::vector<DrmEncROIRegion> StringToRoiRegions(const std::string &str_regions);
std::string to_param_string(const DrmImageConfig &img_cfg);
std::string to_param_string(const DrmVideoConfig &vid_cfg);
std::string to_param_string(const DrmAudioConfig &aud_cfg);
std::string to_param_string(const DrmMediaConfig &mc, const std::string &out_type);
std::string get_video_encoder_config_string(const DrmImageInfo &info, const DrmVideoEncoderCfg &cfg);
int video_encoder_set_bps(std::shared_ptr<Flow> &enc_flow, unsigned int target, unsigned int min = 0, unsigned int max = 0);

int video_encoder_set_rc_quality(std::shared_ptr<Flow> &enc_flow, const char *rc_quality);
int video_encoder_set_rc_mode(std::shared_ptr<Flow> &enc_flow, const char *rc_mode);
int video_encoder_set_qp(std::shared_ptr<Flow> &enc_flow, DrmVideoEncoderQp &qps);
int video_encoder_get_qp(std::shared_ptr<Flow> &enc_flow, DrmVideoEncoderQp &qps);
int video_encoder_force_idr(std::shared_ptr<Flow> &enc_flow);
int video_encoder_set_fps(std::shared_ptr<Flow> &enc_flow, uint8_t out_num, uint8_t out_den, uint8_t in_num = 0, uint8_t in_den = 0);
int video_encoder_set_osd_plt(std::shared_ptr<Flow> &enc_flow, const uint32_t *yuv_plt);
int video_encoder_set_osd_region(std::shared_ptr<Flow> &enc_flow, DrmOsdRegionData *region_data, int plane_cnt = 1);
int video_encoder_set_move_detection(std::shared_ptr<Flow> &enc_flow, std::shared_ptr<Flow> &md_flow);
int video_encoder_set_roi_regions(std::shared_ptr<Flow> &enc_flow, DrmEncROIRegion *regions, int region_cnt);
int video_encoder_set_roi_regions(std::shared_ptr<Flow> &enc_flow, std::string roi_param);
int video_encoder_set_gop_size(std::shared_ptr<Flow> &enc_flow, int gop);
int video_encoder_set_resolution(std::shared_ptr<Flow> &enc_flow, DrmVideoResolutionCfg *vid_cfg);
int video_encoder_set_rotation(std::shared_ptr<Flow> &enc_flow, int rotation_cfg);
int video_move_detect_set_rects(std::shared_ptr<Flow> &md_flow, DrmImageRect *rects, int rect_cnt);
int video_move_detect_set_rects(std::shared_ptr<Flow> &md_flow, std::string rects_param);
int video_encoder_set_avc_profile(std::shared_ptr<Flow> &enc_flow, int profile_idc, int level = 0);
int video_encoder_set_super_frame(std::shared_ptr<Flow> &enc_flow, DrmVencSuperFrmCfg *super_frm_cfg);

int video_encoder_set_split(std::shared_ptr<Flow> &enc_flow, unsigned int mode, unsigned int size);
int video_encoder_set_gop_mode(std::shared_ptr<Flow> &enc_flow, DrmEncGopModeParam *params);
int video_encoder_set_userdata(std::shared_ptr<Flow> &enc_flow, void *data, int len, int all_frames = 0);
int video_encoder_enable_statistics(std::shared_ptr<Flow> &enc_flow, int enable);
int jpeg_encoder_set_qfactor(std::shared_ptr<Flow> &enc_flow, int qfactor);
}

#endif
