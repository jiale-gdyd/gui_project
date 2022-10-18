#include <sstream>
#include <strings.h>

#include <media/utils/utils.h>
#include <media/utils/encoder.h>
#include <media/utils/key_string.h>
#include <media/utils/media_type.h>
#include <media/utils/media_config.h>

namespace libdrm {
const char *rc_quality_strings[7] = {
    DRM_KEY_LOWEST, DRM_KEY_LOWER, DRM_KEY_LOW,
    DRM_KEY_MEDIUM, DRM_KEY_HIGH, DRM_KEY_HIGHER,
    DRM_KEY_HIGHEST
};

const char *rc_mode_strings[4] = {
    DRM_KEY_VBR,
    DRM_KEY_CBR,
    DRM_KEY_FIXQP,
    DRM_KEY_AVBR
};

static const char *convert2constchar(const std::string &s, const char *array[], size_t array_len)
{
    for (size_t i = 0; i < array_len; i++) {
        if (!strcasecmp(s.c_str(), array[i])) {
            return array[i];
        }
    }

    return nullptr;
}

const char *ConvertRcQuality(const std::string &s)
{
    return convert2constchar(s, rc_quality_strings, ARRAY_ELEMS(rc_quality_strings));
}

const char *ConvertRcMode(const std::string &s)
{
    return convert2constchar(s, rc_mode_strings, ARRAY_ELEMS(rc_mode_strings));
}

static int ParseMediaConfigFps(std::map<std::string, std::string> &params, DrmVideoConfig &vid_cfg)
{
    char *num = NULL;
    char *den = NULL;
    std::string value = params[DRM_KEY_FPS];

    if (value.empty()) {
        DRM_MEDIA_LOGE("MediaCfg: fps: KEY_FPS is null");
        return -1;
    }

    num = strtok((char *)value.c_str(), "/");
    den = strtok(NULL, "/");
    if (!num || !den || (strlen(num) > 2) || (strlen(den) > 2)) {
        DRM_MEDIA_LOGE("MediaCfg: fps: KEY_FPS=%s is invalid", value.c_str());
        return -1;
    }

    vid_cfg.frame_rate = std::atoi(num);
    vid_cfg.frame_rate_den = std::atoi(den);

    value = params[DRM_KEY_FPS_IN];
    if (value.empty()) {
        DRM_MEDIA_LOGE("MediaCfg: fps: KEY_FPS_IN is null");
        return -1;
    }

    num = strtok((char *)value.c_str(), "/");
    den = strtok(NULL, "/");
    if (!num || !den || (strlen(num) > 2) || (strlen(den) > 2)) {
        DRM_MEDIA_LOGE("MediaCfg: fps: KEY_FPS_IN(%s) is null", value.c_str());
        return -1;
    }

    vid_cfg.frame_in_rate = std::atoi(num);
    vid_cfg.frame_in_rate_den = std::atoi(den);

    return 0;
}

bool ParseMediaConfigFromMap(std::map<std::string, std::string> &params, DrmMediaConfig &mc)
{
    std::string value = params[DRM_KEY_OUTPUTDATATYPE];
    if (value.empty()) {
        DRM_MEDIA_LOGE("%s: miss %s", __func__, DRM_KEY_OUTPUTDATATYPE);
        return false;
    }

    bool image_in = string_start_withs(value, DRM_IMAGE_PREFIX);
    bool video_in = string_start_withs(value, DRM_VIDEO_PREFIX);
    bool audio_in = string_start_withs(value, DRM_AUDIO_PREFIX);
    if (!image_in && !video_in && !audio_in) {
        DRM_MEDIA_LOGE("%s: unsupport outtype %s", __func__, value.c_str());
        return false;
    }

    DrmImageInfo info;
    DrmCodecType codec_type = StringToCodecType(value.c_str());
    if (codec_type == DRM_CODEC_TYPE_NONE) {
        DRM_MEDIA_LOGE("%s: unsupport outtype %s", __func__, value.c_str());
        return false;
    }

    if (image_in || video_in) {
        if (!ParseImageInfoFromMap(params, info)) {
            return false;
        }

        DrmImageConfig &img_cfg = mc.img_cfg;
        GET_STRING_TO_INT(img_cfg.rect_info.x, params, DRM_KEY_RECT_X, 0);
        GET_STRING_TO_INT(img_cfg.rect_info.y, params, DRM_KEY_RECT_Y, 0);
        GET_STRING_TO_INT(img_cfg.rect_info.w, params, DRM_KEY_RECT_W, 0);
        GET_STRING_TO_INT(img_cfg.rect_info.h, params, DRM_KEY_RECT_H, 0);
        DRM_MEDIA_LOGI("%s: rect_x = %d, rect_y = %d, rect.w = %d, rect.h = %d", __func__, img_cfg.rect_info.x, img_cfg.rect_info.y, img_cfg.rect_info.w, img_cfg.rect_info.h);
    } else {
        DrmAudioConfig &aud_cfg = mc.aud_cfg;
        if (!ParseSampleInfoFromMap(params, aud_cfg.sample_info)) {
            return false;
        }

        CHECK_EMPTY(value, params, DRM_KEY_COMPRESS_BITRATE)
        aud_cfg.bit_rate = std::stoi(value);

        CHECK_EMPTY(value, params, DRM_KEY_FLOAT_QUALITY)
        aud_cfg.quality = std::stof(value);

        aud_cfg.codec_type = codec_type;
        mc.type = DrmType::Audio;
        return true;
    }

    if (image_in) {
        DrmImageConfig &img_cfg = mc.img_cfg;
        img_cfg.image_info = info;
        GET_STRING_TO_INT(img_cfg.qfactor, params, DRM_KEY_JPEG_QFACTOR, 0);
        GET_STRING_TO_INT(img_cfg.dcf, params, DRM_KEY_ENABLE_JPEG_DCF, 0);
        GET_STRING_TO_INT(img_cfg.mpf_cnt, params, DRM_KEY_JPEG_MPF_CNT, 0);
        GET_STRING_TO_INT(img_cfg.mpfw[0], params, DRM_KEY_JPEG_MPF0_W, 0);
        GET_STRING_TO_INT(img_cfg.mpfw[1], params, DRM_KEY_JPEG_MPF1_W, 0);
        GET_STRING_TO_INT(img_cfg.mpfh[0], params, DRM_KEY_JPEG_MPF0_H, 0);
        GET_STRING_TO_INT(img_cfg.mpfh[1], params, DRM_KEY_JPEG_MPF1_H, 0);
        img_cfg.codec_type = codec_type;
        mc.type = DrmType::Image;

        DrmVideoConfig &vid_cfg = mc.vid_cfg;
        GET_STRING_TO_INT(vid_cfg.rotation, params, DRM_KEY_ROTATION, 0)
    } else if (video_in) {
        DrmVideoConfig &vid_cfg = mc.vid_cfg;
        DrmImageConfig &img_cfg = vid_cfg.image_cfg;
        DrmVideoEncoderQp &qp = mc.vid_cfg.encode_qp;

        img_cfg.image_info = info;
        img_cfg.codec_type = codec_type;

        GET_STRING_TO_INT(img_cfg.qfactor, params, DRM_KEY_JPEG_QFACTOR, 0)
        GET_STRING_TO_INT(qp.qp_init, params, DRM_KEY_COMPRESS_QP_INIT, 0)
        GET_STRING_TO_INT(qp.qp_step, params, DRM_KEY_COMPRESS_QP_STEP, 0)
        GET_STRING_TO_INT(qp.qp_min, params, DRM_KEY_COMPRESS_QP_MIN, 0)
        GET_STRING_TO_INT(qp.qp_max, params, DRM_KEY_COMPRESS_QP_MAX, 0)
        GET_STRING_TO_INT(vid_cfg.bit_rate, params, DRM_KEY_COMPRESS_BITRATE, 0)
        GET_STRING_TO_INT(vid_cfg.bit_rate_min, params, DRM_KEY_COMPRESS_BITRATE_MIN, 0)
        GET_STRING_TO_INT(vid_cfg.bit_rate_max, params, DRM_KEY_COMPRESS_BITRATE_MAX, 0)
        GET_STRING_TO_INT(qp.qp_max_i, params, DRM_KEY_COMPRESS_QP_MAX_I, 0)
        GET_STRING_TO_INT(qp.qp_min_i, params, DRM_KEY_COMPRESS_QP_MIN_I, 0)
        GET_STRING_TO_INT(vid_cfg.trans_8x8, params, DRM_KEY_H264_TRANS_8x8, 0)
        GET_STRING_TO_INT(vid_cfg.level, params, DRM_KEY_LEVEL, 0)
        GET_STRING_TO_INT(vid_cfg.gop_size, params, DRM_KEY_VIDEO_GOP, 0)
        GET_STRING_TO_INT(vid_cfg.profile, params, DRM_KEY_PROFILE, 0)
        GET_STRING_TO_INT(vid_cfg.full_range, params, DRM_KEY_FULL_RANGE, 0)
        GET_STRING_TO_INT(vid_cfg.ref_frm_cfg, params, DRM_KEY_REF_FRM_CFG, 0)
        GET_STRING_TO_INT(vid_cfg.rotation, params, DRM_KEY_ROTATION, 0)
        GET_STRING_TO_INT(vid_cfg.scaling_list, params, DRM_KEY_SCALING_LIST, 0)

        if (ParseMediaConfigFps(params, vid_cfg) < 0) {
            return false;
        }

        const std::string rc_q = params[DRM_KEY_COMPRESS_RC_QUALITY];
        if (rc_q.empty()) {
            vid_cfg.rc_quality = NULL;
        } else {
            vid_cfg.rc_quality = ConvertRcQuality(rc_q);
        }

        const std::string rc_m = params[DRM_KEY_COMPRESS_RC_MODE];
        if (rc_m.empty()) {
            vid_cfg.rc_mode = NULL;
        } else {
            vid_cfg.rc_mode = ConvertRcMode(rc_m);
        }

        mc.type = DrmType::Video;
    }

    return true;
}

std::vector<DrmEncROIRegion> StringToRoiRegions(const std::string &str_regions)
{
    const char *start = nullptr;
    std::vector<DrmEncROIRegion> ret;

    if (str_regions.empty()) {
        return ret;
    }

    start = str_regions.c_str();
    while (start) {
        DrmEncROIRegion region = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        start = strstr(start, "(");
        if (!start) {
            break;
        }

        const char *end = strstr(start, ")");
        if (!end) {
            DRM_MEDIA_LOGE("RoiRegions string is invalid! Value:%s", str_regions.c_str());
            break;
        }

        int commas_cnt = 0;
        const char *commas_str = start;

        while (commas_str && (commas_str < end)) {
            commas_str = strstr(commas_str, ",");
            if (!commas_str) {
                break;
            } else if (commas_str < end) {
                commas_cnt++;
            }

            commas_str++;
        }

        if (commas_cnt != 8) {
            DRM_MEDIA_LOGE("RoiRegions string is invalid! Value:%s", str_regions.c_str());
            break;
        }

        int x, y, w, h, intra, quality, qp_area_idx, area_map_en, abs_qp_en;
        int r = sscanf(start, "(%d,%d,%d,%d,%d,%d,%d,%d,%d)", &x, &y, &w, &h, &intra, &quality, &qp_area_idx, &area_map_en, &abs_qp_en);
        if (r != 9) {
            DRM_MEDIA_LOGE("Fail to sscanf(ret=%d) : %m", r);
            ret.clear();
            return ret;
        }

        region.x = (uint16_t)x;
        region.y = (uint16_t)y;
        region.w = (uint16_t)w;
        region.h = (uint16_t)h;
        region.intra = (uint16_t)intra;
        region.quality = (int16_t)quality;
        region.qp_area_idx = (uint16_t)qp_area_idx;
        region.area_map_en = (uint8_t)area_map_en;
        region.abs_qp_en = (uint8_t)abs_qp_en;
        ret.push_back(std::move(region));
        start = end;
    }

    return ret;
}

std::string to_param_string(const DrmImageConfig &img_cfg)
{
    std::string ret = to_param_string(img_cfg.image_info);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_JPEG_QFACTOR, img_cfg.qfactor);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_CODECTYPE, img_cfg.codec_type);

    return ret;
}

std::string to_param_string(const DrmVideoConfig &vid_cfg)
{
    const DrmImageConfig &img_cfg = vid_cfg.image_cfg;
    const DrmVideoEncoderQp &qp = vid_cfg.encode_qp;

    std::string ret = to_param_string(img_cfg);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_QP_INIT, qp.qp_init);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_QP_STEP, qp.qp_step);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_QP_MIN, qp.qp_min);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_QP_MAX, qp.qp_max);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_BITRATE, vid_cfg.bit_rate);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_BITRATE_MAX, vid_cfg.bit_rate_max);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_BITRATE_MIN, vid_cfg.bit_rate_min);

    std::stringstream str_stream;
    std::string fps;
    str_stream << vid_cfg.frame_rate;
    str_stream << "/";
    str_stream << vid_cfg.frame_rate_den;
    str_stream >> fps;
    PARAM_STRING_APPEND(ret, DRM_KEY_FPS, fps);

    str_stream.clear();
    str_stream << vid_cfg.frame_in_rate;
    str_stream << "/";
    str_stream << vid_cfg.frame_in_rate_den;
    str_stream >> fps;
    PARAM_STRING_APPEND(ret, DRM_KEY_FPS_IN, fps);

    PARAM_STRING_APPEND_TO(ret, DRM_KEY_LEVEL, vid_cfg.level);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_VIDEO_GOP, vid_cfg.gop_size);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_PROFILE, vid_cfg.profile);

    if (vid_cfg.rc_quality) {
        PARAM_STRING_APPEND(ret, DRM_KEY_COMPRESS_RC_QUALITY, vid_cfg.rc_quality);
    }

    if (vid_cfg.rc_mode) {
        PARAM_STRING_APPEND(ret, DRM_KEY_COMPRESS_RC_MODE, vid_cfg.rc_mode);
    }

    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_QP_MAX_I, qp.qp_max_i);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_QP_MIN_I, qp.qp_min_i);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_H264_TRANS_8x8, vid_cfg.trans_8x8);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_FULL_RANGE, vid_cfg.full_range);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_REF_FRM_CFG, vid_cfg.ref_frm_cfg);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_ROTATION, vid_cfg.rotation);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_SCALING_LIST, vid_cfg.scaling_list);

    return ret;
}

std::string to_param_string(const DrmAudioConfig &aud_cfg)
{
    std::string ret = to_param_string(aud_cfg.sample_info);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_COMPRESS_BITRATE, aud_cfg.bit_rate);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_FLOAT_QUALITY, aud_cfg.quality);
    PARAM_STRING_APPEND_TO(ret, DRM_KEY_CODECTYPE, aud_cfg.codec_type);

    return ret;
}

std::string to_param_string(const DrmMediaConfig &mc, const std::string &out_type)
{
    std::string ret;
    DrmMediaConfig mc_temp = mc;

    bool image_in = string_start_withs(out_type, DRM_IMAGE_PREFIX);
    bool video_in = string_start_withs(out_type, DRM_VIDEO_PREFIX);
    bool audio_in = string_start_withs(out_type, DRM_AUDIO_PREFIX);

    if (!image_in && !video_in && !audio_in) {
        DRM_MEDIA_LOGE("unsupport outtype %s", out_type.c_str());
        return ret;
    }

    PARAM_STRING_APPEND(ret, DRM_KEY_OUTPUTDATATYPE, out_type);

    if (image_in) {
        mc_temp.img_cfg.codec_type = StringToCodecType(out_type.c_str());
        ret.append(to_param_string(mc_temp.img_cfg));
    }

    if (video_in) {
        mc_temp.vid_cfg.image_cfg.codec_type = StringToCodecType(out_type.c_str());
        ret.append(to_param_string(mc_temp.vid_cfg));
    }

    if (audio_in) {
        mc_temp.aud_cfg.codec_type = StringToCodecType(out_type.c_str());
        ret.append(to_param_string(mc_temp.aud_cfg));
    }

    return ret;
}

std::string get_video_encoder_config_string(const DrmImageInfo &info, const DrmVideoEncoderCfg &cfg)
{
    if (!info.width || !info.height || (info.pix_fmt >= DRM_PIX_FMT_NB) || (info.pix_fmt <= DRM_PIX_FMT_NONE)) {
        DRM_MEDIA_LOGE("%s image info is wrong", __func__);
        return NULL;
    }

    if (StringToCodecType(cfg.type) < 0) {
        DRM_MEDIA_LOGE("%s not support enc type:%s", __func__, cfg.type);
        return NULL;
    }

    if (cfg.rc_quality
        && strcmp(cfg.rc_quality, DRM_KEY_HIGHEST)
        && strcmp(cfg.rc_quality, DRM_KEY_HIGHER)
        && strcmp(cfg.rc_quality, DRM_KEY_HIGH)
        && strcmp(cfg.rc_quality, DRM_KEY_MEDIUM)
        && strcmp(cfg.rc_quality, DRM_KEY_LOW)
        && strcmp(cfg.rc_quality, DRM_KEY_LOWER)
        && strcmp(cfg.rc_quality, DRM_KEY_LOWEST))
    {
        DRM_MEDIA_LOGE("%s rc_quality is invalid! should be [KEY_LOWEST, KEY_HIGHEST]", __func__);
        return NULL;
    }

    if (cfg.rc_mode
        && strcmp(cfg.rc_mode, DRM_KEY_VBR)
        && strcmp(cfg.rc_mode, DRM_KEY_CBR)
        && strcmp(cfg.rc_mode, DRM_KEY_AVBR)
        && strcmp(cfg.rc_mode, DRM_KEY_FIXQP))
    {
        DRM_MEDIA_LOGE("%s rc_mode(%s) is invalid", __func__, cfg.rc_mode);
        return NULL;
    }

    DrmMediaConfig enc_config;
    memset(&enc_config, 0, sizeof(enc_config));
    DrmVideoConfig &vid_cfg = enc_config.vid_cfg;
    DrmImageConfig &img_cfg = vid_cfg.image_cfg;
    img_cfg.image_info.pix_fmt = info.pix_fmt;
    img_cfg.image_info.width = info.width;
    img_cfg.image_info.height = info.height;
    img_cfg.image_info.vir_width = info.vir_width;
    img_cfg.image_info.vir_height = info.vir_height;
    img_cfg.codec_type = StringToCodecType(cfg.type);

    if (cfg.fps) {
        vid_cfg.frame_rate = vid_cfg.frame_in_rate = cfg.fps;
    } else {
        vid_cfg.frame_rate = vid_cfg.frame_in_rate = 30;
        DRM_MEDIA_LOGI("%s: frame rate use defalut value:30", __func__);
    }

    vid_cfg.gop_size = cfg.gop;

    if (cfg.max_bps) {
        vid_cfg.bit_rate_max = cfg.max_bps;
    } else {
        int den, num;
        GetPixFmtNumDen(info.pix_fmt, num, den);

        int wh_product = info.width * info.height;
        if (wh_product > 2073600) {
            vid_cfg.bit_rate_max = wh_product * vid_cfg.frame_rate * num / den / 20;
        } else if (wh_product > 921600) {
            vid_cfg.bit_rate_max = wh_product * vid_cfg.frame_rate * num / den / 17;
        } else if (wh_product > 101376) {
            vid_cfg.bit_rate_max = wh_product * vid_cfg.frame_rate * num / den / 15;
        } else {
            vid_cfg.bit_rate_max = wh_product * vid_cfg.frame_rate * num / den / 8;
        }

        DRM_MEDIA_LOGI("%s: maxbps use defalut value:%d", __func__, vid_cfg.bit_rate_max);
    }

    vid_cfg.rc_quality = cfg.rc_quality;
    vid_cfg.rc_mode = cfg.rc_mode;

    std::string enc_param = "";
    enc_param.append(libdrm::to_param_string(enc_config, cfg.type));

    return enc_param;
}

int video_encoder_set_bps(std::shared_ptr<Flow> &enc_flow, unsigned int target, unsigned int min, unsigned int max)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    int *bps_array = (int *)malloc(3 * sizeof(int));
    bps_array[0] = min;
    bps_array[1] = target;
    bps_array[2] = max;

    pbuff->SetPtr(bps_array, 3 * sizeof(int));
    enc_flow->Control(VideoEncoder::kBitRateChange, pbuff);

    return 0;
}

int video_encoder_set_rc_quality(std::shared_ptr<Flow> &enc_flow, const char *rc_quality)
{
    if (!enc_flow || !rc_quality) {
        return -EINVAL;
    }

    if (strcmp(rc_quality, DRM_KEY_HIGHEST)
        && strcmp(rc_quality, DRM_KEY_HIGHER)
        && strcmp(rc_quality, DRM_KEY_HIGH)
        && strcmp(rc_quality, DRM_KEY_MEDIUM)
        && strcmp(rc_quality, DRM_KEY_LOW)
        && strcmp(rc_quality, DRM_KEY_LOWER)
        && strcmp(rc_quality, DRM_KEY_LOWEST))
    {
        DRM_MEDIA_LOGE("%s rc_quality:%s is invalid! should be [KEY_LOWEST, KEY_HIGHEST]", __func__, rc_quality);
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    int str_len = strlen(rc_quality);
    char *quality = (char *)malloc(str_len + 1);

    memcpy(quality, rc_quality, strlen(rc_quality));
    quality[str_len] = '\0';
    pbuff->SetPtr(quality, strlen(rc_quality));
    enc_flow->Control(VideoEncoder::kRcQualityChange, pbuff);

    return 0;
}

int video_encoder_set_rc_mode(std::shared_ptr<Flow> &enc_flow, const char *rc_mode)
{
    if (!enc_flow || !rc_mode) {
        return -EINVAL;
    }

    if (strcmp(rc_mode, DRM_KEY_VBR)
        && strcmp(rc_mode, DRM_KEY_CBR)
        && strcmp(rc_mode, DRM_KEY_AVBR)
        && strcmp(rc_mode, DRM_KEY_FIXQP))
    {
        DRM_MEDIA_LOGE("%s rc_mode(%s) is invalid", __func__, rc_mode);
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    int str_len = strlen(rc_mode);
    char *mode = (char *)malloc(str_len + 1);

    memcpy(mode, rc_mode, strlen(rc_mode));
    mode[str_len] = '\0';
    pbuff->SetPtr(mode, strlen(rc_mode));
    enc_flow->Control(VideoEncoder::kRcModeChange, pbuff);

    return 0;
}

int video_encoder_set_qp(std::shared_ptr<Flow> &enc_flow, DrmVideoEncoderQp &qps)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    if ((qps.qp_max && ((qps.qp_max > 51)
        || (qps.qp_max < 8)))
        || (qps.qp_max_i && ((qps.qp_max_i > 51)
        || (qps.qp_max_i < 8)))
        || (qps.qp_min < 0) || (qps.qp_min > 48) || (qps.qp_min_i < 0)
        || (qps.qp_min_i > 48) || (qps.qp_min > qps.qp_max)
        || (qps.qp_min_i > qps.qp_max_i))
    {
        DRM_MEDIA_LOGE("qp range error. qp_min:[0, 48]; qp_max:[8, 51]");
        return -EINVAL;
    }

    if ((qps.qp_init != -1) && ((qps.qp_init > qps.qp_max) || (qps.qp_init < qps.qp_min))) {
        DRM_MEDIA_LOGE("qp_init should be within [qp_min, qp_max] or equal -1.\n");
        return -EINVAL;
    }

    if (!qps.qp_step || (qps.qp_step > (qps.qp_max - qps.qp_min))) {
        DRM_MEDIA_LOGE("qp_step should be within (0, qp_max - qp_min]\n");
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    DrmVideoEncoderQp *qp_struct = (DrmVideoEncoderQp *)malloc(sizeof(DrmVideoEncoderQp));

    memcpy(qp_struct, &qps, sizeof(DrmVideoEncoderQp));
    pbuff->SetPtr(qp_struct, sizeof(DrmVideoEncoderQp));
    enc_flow->Control(VideoEncoder::kQPChange, pbuff);

    return 0;
}

int video_encoder_get_qp(std::shared_ptr<Flow> &enc_flow, DrmVideoEncoderQp &qps)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    DrmVideoEncoderQp *qp_struct = (DrmVideoEncoderQp *)malloc(sizeof(DrmVideoEncoderQp));
    pbuff->SetPtr(qp_struct, sizeof(DrmVideoEncoderQp));
    enc_flow->Control(VideoEncoder::kQPChange | VideoEncoder::kGetFlag, pbuff);
    memcpy(&qps, qp_struct, sizeof(DrmVideoEncoderQp));

    return 0;
}

int jpeg_encoder_set_qfactor(std::shared_ptr<Flow> &enc_flow, int qfactor)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    if ((qfactor > 99) || (qfactor < 1)) {
        DRM_MEDIA_LOGE("%s: qfactor should be within [1, 99]", __func__);
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetValue(qfactor);
    enc_flow->Control(VideoEncoder::kQPChange, pbuff);

    return 0;
}

int video_encoder_force_idr(std::shared_ptr<Flow> &enc_flow)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    enc_flow->Control(VideoEncoder::kForceIdrFrame, pbuff);

    return 0;
}

int video_encoder_set_fps(std::shared_ptr<Flow> &enc_flow, uint8_t out_num, uint8_t out_den, uint8_t in_num, uint8_t in_den)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    if (!out_den || !out_num || (out_den > 16) || (out_num > 120) || (in_den > 16) || (in_num > 120)) {
        DRM_MEDIA_LOGE("fps(%d/%d) is invalid! num:[1,120], den:[1, 16]", out_num, out_den);
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    uint8_t *fps_array = (uint8_t *)malloc(4 * sizeof(uint8_t));
    fps_array[0] = in_num;
    fps_array[1] = in_den;
    fps_array[2] = out_num;
    fps_array[3] = out_den;

    pbuff->SetPtr(fps_array, 4);
    enc_flow->Control(VideoEncoder::kFrameRateChange, pbuff);

    return 0;
}

int video_encoder_set_osd_plt(std::shared_ptr<Flow> &enc_flow, const uint32_t *yuv_plt)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    uint32_t *plt = (uint32_t *)malloc(256 * sizeof(uint32_t));
    memcpy(plt, yuv_plt, 256 * sizeof(uint32_t));

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(plt, 256 * sizeof(uint32_t));
    enc_flow->Control(VideoEncoder::kOSDPltChange, pbuff);

    return 0;
}

int video_encoder_set_gop_size(std::shared_ptr<Flow> &enc_flow, int gop)
{
    if (!enc_flow || (gop < 0)) {
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetValue(gop);
    enc_flow->Control(VideoEncoder::kGopChange, pbuff);

    return 0;
}

int video_encoder_set_osd_region(std::shared_ptr<Flow> &enc_flow, DrmOsdRegionData *region_data, int plane_cnt)
{
    if (!enc_flow || !region_data) {
        return -EINVAL;
    }

    if (region_data->enable && ((region_data->width % 2) || (region_data->height % 2))) {
        DRM_MEDIA_LOGE("osd region size must be a multiple of 2x2");
        return -EINVAL;
    }

    int buffer_size = region_data->width * region_data->height * plane_cnt;
    DrmOsdRegionData *rdata = (DrmOsdRegionData *)malloc(sizeof(DrmOsdRegionData) + buffer_size);
    memcpy((void *)rdata, (void *)region_data, sizeof(DrmOsdRegionData));
    if (buffer_size) {
        rdata->buffer = (uint8_t *)rdata + sizeof(DrmOsdRegionData);
        memcpy(rdata->buffer, region_data->buffer, buffer_size);
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(rdata, sizeof(DrmOsdRegionData) + buffer_size);
    enc_flow->Control(VideoEncoder::kOSDDataChange, pbuff);

    return 0;
}

int video_encoder_set_move_detection(std::shared_ptr<Flow> &enc_flow, std::shared_ptr<Flow> &md_flow)
{
    int ret = 0;
    void **rdata = (void **)malloc(sizeof(void *));
    *rdata = md_flow.get();

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(rdata, sizeof(sizeof(void *)));
    ret = enc_flow->Control(libdrm::VideoEncoder::kMoveDetectionFlow, pbuff);

    return ret;
}

int video_encoder_set_roi_regions(std::shared_ptr<Flow> &enc_flow, DrmEncROIRegion *regions, int region_cnt)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    int rsize = 0;
    void *rdata = NULL;

    if (regions && region_cnt) {
        rsize = sizeof(DrmEncROIRegion) * region_cnt;
        rdata = (void *)malloc(rsize);
        memcpy(rdata, (void *)regions, rsize);
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(rdata, rsize);
    enc_flow->Control(VideoEncoder::kROICfgChange, pbuff);

    return 0;
}

int video_encoder_set_roi_regions(std::shared_ptr<Flow> &enc_flow, std::string roi_param)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    auto regions = StringToRoiRegions(roi_param);
    int region_cnt = regions.size();
    if (!region_cnt) {
        return -EINVAL;
    }

    DrmEncROIRegion *rdata = NULL;
    int rsize = sizeof(DrmEncROIRegion) * region_cnt;
    rdata = (DrmEncROIRegion *)malloc(rsize);
    if (!rdata) {
        return -EINVAL;
    }

    int i = 0;
    for (auto iter : regions) {
        memcpy((void *)&rdata[i++], (void *)&iter, sizeof(DrmEncROIRegion));
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(rdata, rsize);
    enc_flow->Control(VideoEncoder::kROICfgChange, pbuff);

    return 0;
}

int video_move_detect_set_rects(std::shared_ptr<Flow> &md_flow, DrmImageRect *rects, int rect_cnt)
{
    if (!md_flow || !rects || !rect_cnt) {
        return -EINVAL;
    }

    return md_flow->Control(libdrm::S_MD_ROI_RECTS, rects, rect_cnt);
}

int video_move_detect_set_rects(std::shared_ptr<Flow> &md_flow, std::string rects_param)
{
    if (!md_flow) {
        return -EINVAL;
    }

    std::vector<DrmImageRect> rect_vector;
    rect_vector = StringToImageRect(rects_param);
    if (rect_vector.empty()) {
        return -EINVAL;
    }

    int rect_cnt = (int)rect_vector.size();
    DrmImageRect *rects = (DrmImageRect *)malloc(rect_cnt * sizeof(DrmImageRect));
    if (!rects) {
        return -ENOSPC;
    }

    int i = 0;
    for (auto iter : rect_vector) {
        memcpy((void *)&rects[i++], (void *)&iter, sizeof(DrmImageRect));
    }

    int ret = md_flow->Control(libdrm::S_MD_ROI_RECTS, rects, rect_cnt);
    free(rects);

    return ret;
}

int video_encoder_set_split(std::shared_ptr<Flow> &enc_flow, unsigned int mode, unsigned int size)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    uint32_t *param = (uint32_t *)malloc(2 * sizeof(uint32_t));
    *param = mode;
    *(param + 1) = size;

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(param, 2 * sizeof(uint32_t));
    enc_flow->Control(VideoEncoder::kSplitChange, pbuff);

    return 0;
}

int video_encoder_set_gop_mode(std::shared_ptr<Flow> &enc_flow, DrmEncGopModeParam *mode_params)
{
    if (!enc_flow || !mode_params) {
        return -EINVAL;
    }

    uint8_t *param = (uint8_t *)malloc(sizeof(DrmEncGopModeParam));
    if (!param) {
        return -ENOSPC;
    }

    memcpy(param, mode_params, sizeof(DrmEncGopModeParam));
    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(param, sizeof(DrmEncGopModeParam));
    enc_flow->Control(VideoEncoder::kGopModeChange, pbuff);

    return 0;
}

int video_encoder_set_avc_profile(std::shared_ptr<Flow> &enc_flow, int profile_idc, int level)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    if ((profile_idc != 66) && (profile_idc != 77) && (profile_idc != 100)) {
        DRM_MEDIA_LOGE("%s profile_idc:%d is invalid! Only supprot: 66:Baseline, 77:Main Profile, 100: High Profile", __func__, profile_idc);
        return -EINVAL;
    }

    int *param = (int *)malloc(2 * sizeof(int));
    *param = profile_idc;
    *(param + 1) = level;

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(param, 2 * sizeof(int));
    enc_flow->Control(VideoEncoder::kProfileChange, pbuff);

    return 0;
}

int video_encoder_set_userdata(std::shared_ptr<Flow> &enc_flow, void *data, int len, int all_frames)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    if (!data && len) {
        DRM_MEDIA_LOGE("invalid userdata size");
        return -EINVAL;
    }

    uint8_t *param = (uint8_t *)malloc(len + 1);
    *param = all_frames ? 1 : 0;
    if (len) {
        memcpy(param + 1, data, len);
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(param, len + 1);
    enc_flow->Control(VideoEncoder::kUserDataChange, pbuff);

    return 0;
}

int video_encoder_set_resolution(std::shared_ptr<Flow> &enc_flow, DrmVideoResolutionCfg *vid_cfg)
{
    if (!enc_flow || !vid_cfg) {
        return -EINVAL;
    }

    uint8_t *cfg = (uint8_t *)malloc(sizeof(DrmVideoResolutionCfg));
    if (!cfg) {
        return -ENOSPC;
    }

    memcpy(cfg, vid_cfg, sizeof(DrmVideoResolutionCfg));
    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(cfg, sizeof(DrmVideoResolutionCfg));
    enc_flow->Control(VideoEncoder::kResolutionChange, pbuff);

    return 0;
}

int video_encoder_set_rotation(std::shared_ptr<Flow> &enc_flow, int rotation_cfg)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    int *param = (int *)malloc(sizeof(int));
    if (!param) {
        return -ENOSPC;
    }

    *param = rotation_cfg;
    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(param, sizeof(int));
    enc_flow->Control(VideoEncoder::kRotationChange, pbuff);

    return 0;
}

int video_encoder_enable_statistics(std::shared_ptr<Flow> &enc_flow, int enable)
{
    if (!enc_flow) {
        return -EINVAL;
    }

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetValue(enable);
    enc_flow->Control(VideoEncoder::kEnableStatistics, pbuff);

    return 0;
}

int video_encoder_set_super_frame(std::shared_ptr<Flow> &enc_flow, DrmVencSuperFrmCfg *super_frm_cfg)
{
    if (!enc_flow || !super_frm_cfg) {
        return -EINVAL;
    }

    DrmVencSuperFrmCfg *cfg = (DrmVencSuperFrmCfg *)malloc(sizeof(DrmVencSuperFrmCfg));
    memcpy((void *)cfg, (void *)super_frm_cfg, sizeof(DrmVencSuperFrmCfg));

    auto pbuff = std::make_shared<ParameterBuffer>(0);
    pbuff->SetPtr(cfg, sizeof(DrmVencSuperFrmCfg));
    enc_flow->Control(VideoEncoder::kSuperFrmChange, pbuff);

    return 0;
}
}
