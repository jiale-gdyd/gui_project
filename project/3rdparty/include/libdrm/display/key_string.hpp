#ifndef LIBDRM_DISPLAY_KEY_STRING_HPP
#define LIBDRM_DISPLAY_KEY_STRING_HPP

#define _DRM_STR(s)                             #s
#define DRM_STR(s)                              _DRM_STR(s)

#define DRM_KEY_PATH                            "path"
#define DRM_KEY_OPEN_MODE                       "mode"
#define DRM_KEY_SAVE_MODE                       "save_mode"
#define DRM_KEY_SAVE_MODE_SINGLE                "single_frame"
#define DRM_KEY_SAVE_MODE_CONTIN                "continuous_frame"
#define DRM_KEY_DEVICE                          "device"
#define DRM_KEY_CAMERA_ID                       "camera_id"

#define DRM_KEY_NAME                            "name"
#define DRM_KEY_INPUTDATATYPE                   "input_data_type"
#define DRM_KEY_OUTPUTDATATYPE                  "output_data_type"

#define DRM_KEY_PIXFMT                          "pixel_fomat"
#define DRM_KEY_BUFFER_WIDTH                    "width"
#define DRM_KEY_BUFFER_HEIGHT                   "height"
#define DRM_KEY_BUFFER_VIR_WIDTH                "virtual_width"
#define DRM_KEY_BUFFER_VIR_HEIGHT               "virtual_height"
#define DRM_KEY_CODECTYPE                       "codec_type"

#define DRM_KEY_RECT_X                          "rect_x"
#define DRM_KEY_RECT_Y                          "rect_y"
#define DRM_KEY_RECT_W                          "rect_w"
#define DRM_KEY_RECT_H                          "rect_h"

#define DRM_KEY_RIGHT_DIRECTION                 "->"
#define DRM_KEY_BUFFER_RECT                     "rect"
#define DRM_KEY_BUFFER_ROTATE                   "rotate"
#define DRM_KEY_BUFFER_FLIP                     "flip"

#define DRM_KEY_COMPRESS_QP_INIT                "qp_init"
#define DRM_KEY_COMPRESS_QP_STEP                "qp_step"
#define DRM_KEY_COMPRESS_QP_MIN                 "qp_min"
#define DRM_KEY_COMPRESS_QP_MAX                 "qp_max"
#define DRM_KEY_COMPRESS_QP_MAX_I               "qp_max_i"
#define DRM_KEY_COMPRESS_QP_MIN_I               "qp_min_i"
#define DRM_KEY_COMPRESS_BITRATE                "bitrate"
#define DRM_KEY_COMPRESS_BITRATE_MAX            "bitrate_max"
#define DRM_KEY_COMPRESS_BITRATE_MIN            "bitrate_min"

#define DRM_KEY_FPS                             "framerate"
#define DRM_KEY_FPS_IN                          "framerate_in"
#define DRM_KEY_LEVEL                           "level"
#define DRM_KEY_VIDEO_GOP                       "gop"
#define DRM_KEY_PROFILE                         "profile"
#define DRM_KEY_ROTATION                        "rotation"
#define DRM_KEY_ENABLE_JPEG_DCF                 "jpeg_en_dcf"
#define DRM_KEY_JPEG_MPF_CNT                    "jpeg_mpf_cnt"
#define DRM_KEY_JPEG_MPF0_W                     "jpeg_mpf0_w"
#define DRM_KEY_JPEG_MPF0_H                     "jpeg_mpf0_h"
#define DRM_KEY_JPEG_MPF1_W                     "jpeg_mpf1_w"
#define DRM_KEY_JPEG_MPF1_H                     "jpeg_mpf1_h"
#define DRM_KEY_COMPRESS_RC_QUALITY             "rc_quality"
#define DRM_KEY_COMPRESS_RC_MODE                "rc_mode"
#define DRM_KEY_NEED_EXTRA_OUTPUT               "need_extra_output"
#define DRM_KEY_NEED_EXTRA_MERGE                "need_extra_merge"
#define DRM_KEY_FULL_RANGE                      "full_range"
#define DRM_KEY_H264_TRANS_8x8                  "h264_trans_8x8"
#define DRM_KEY_SCALING_LIST                    "scaling_list"

#define DRM_KEY_JPEG_QFACTOR                    "jpeg_qfactor"
#define DRM_KEY_ROI_REGIONS                     "roi_regions"

#define DRM_KEY_LOWEST                          "lowest"
#define DRM_KEY_LOWER                           "lower"
#define DRM_KEY_LOW                             "low"
#define DRM_KEY_MEDIUM                          "medium"
#define DRM_KEY_HIGH                            "high"
#define DRM_KEY_HIGHER                          "higher"
#define DRM_KEY_HIGHEST                         "highest"

#define DRM_KEY_FIXQP                           "fixqp"
#define DRM_KEY_VBR                             "vbr"
#define DRM_KEY_CBR                             "cbr"
#define DRM_KEY_AVBR                            "avbr"

#define DRM_KEY_REF_FRM_CFG                     "reference_frame_config"

#define DRM_KEY_MPP_GROUP_MAX_FRAMES            "fg_max_frames"
#define DRM_KEY_MPP_SPLIT_MODE                  "split_mode"
#define DRM_KEY_OUTPUT_TIMEOUT                  "output_timeout"

#define DRM_KEY_MD_SINGLE_REF                   "md_single_ref"
#define DRM_KEY_MD_ORI_WIDTH                    "md_orignal_width"
#define DRM_KEY_MD_ORI_HEIGHT                   "md_orignal_height"
#define DRM_KEY_MD_DS_WIDTH                     "md_down_scale_width"
#define DRM_KEY_MD_DS_HEIGHT                    "md_down_scale_height"
#define DRM_KEY_MD_ROI_CNT                      "md_roi_cnt"
#define DRM_KEY_MD_ROI_RECT                     "md_roi_rect"
#define DRM_KEY_MD_SENSITIVITY                  "md_sensitivity"

#define DRM_KEY_OD_WIDTH                        "od_width"
#define DRM_KEY_OD_HEIGHT                       "od_height"
#define DRM_KEY_OD_ROI_CNT                      "od_roi_cnt"
#define DRM_KEY_OD_ROI_RECT                     "od_roi_rect"
#define DRM_KEY_OD_SENSITIVITY                  "od_sensitivity"

#define DRM_KEY_SAMPLE_FMT                      "sample_format"
#define DRM_KEY_CHANNELS                        "channel_num"
#define DRM_KEY_SAMPLE_RATE                     "sample_rate"
#define DRM_KEY_FRAMES                          "frame_num"
#define DRM_KEY_FLOAT_QUALITY                   "compress_quality"
#define DRM_KEY_LAYOUT                          "layout"
#define DRM_KEY_VQE_ENABLE                      "vqe_enable"
#define DRM_KEY_VQE_MODE                        "vqe_mode"
#define DRM_KEY_VQE_OPEN_MASK                   "vqe_open_mask"
#define DRM_KEY_VQE_WORK_SAMPLE_RATE            "vqe_work_sample_rate"
#define DRM_KEY_VQE_FRAME_SAMPLE                "vqe_frame_sample"
#define DRM_KEY_VQE_PARAM_FILE_PATH             "vqe_param_file_path"
#define DRM_KEY_ANR_POST_ADD_GAIN               "anr_post_add_gain"
#define DRM_KEY_ANR_GMIN                        "gmin"
#define DRM_KEY_ANR_NOISE_FACTOR                "noise_factor"

#define DRM_KEY_USE_LIBV4L2                     "use_libv4l2"
#define DRM_KEY_SUB_DEVICE                      "sub_device"
#define DRM_KEY_V4L2_CAP_TYPE                   "v4l2_capture_type"
#define DRM_KEY_V4L2_C_TYPE(t)                  DRM_STR(t)
#define DRM_KEY_V4L2_MEM_TYPE                   "v4l2_mem_type"
#define DRM_KEY_V4L2_M_TYPE(t)                  DRM_STR(t)
#define DRM_KEY_V4L2_COLORSPACE                 "v4l2_colorspace"
#define DRM_KEY_V4L2_QUANTIZATION               "v4l2_quantization"
#define DRM_KEY_V4L2_CS(t) STR(t)

#define DRM_KEY_PORT_NUM                        "portnum"
#define DRM_KEY_USERNAME                        "username"
#define DRM_KEY_USERPASSWORD                    "userpwd"
#define DRM_KEY_CHANNEL_NAME                    "channel_name"

#define DRM_KEY_MEM_CNT                         "mem_cnt"
#define DRM_KEY_MEM_TYPE                        "mem_type"
#define DRM_KEY_MEM_ION                         "ion"
#define DRM_KEY_MEM_DRM                         "drm"
#define DRM_KEY_MEM_HARDWARE                    "hw_mem"

#define DRM_KEY_MEM_SIZE_PERTIME                "size_pertime"
#define DRM_KEY_LOOP_TIME                       "loop_time"

#define DRM_KEK_THREAD_SYNC_MODEL               "thread_model"
#define DRM_KEY_ASYNCCOMMON                     "asynccommon"
#define DRM_KEY_ASYNCATOMIC                     "asyncatomic"
#define DRM_KEY_SYNC                            "sync"

#define DRM_KEK_INPUT_MODEL                     "input_model"
#define DRM_KEY_BLOCKING                        "blocking"
#define DRM_KEY_DROPFRONT                       "dropfront"
#define DRM_KEY_DROPCURRENT                     "dropcurrent"

#define DRM_KEY_INPUT_CACHE_NUM                 "input_cache_num"
#define DRM_KEY_OUTPUT_CACHE_NUM                "output_cache_num"

#define DRM_KEY_OUTPUT_HOLD_INPUT               "output_hold_input"

#define DRM_KEY_FILE_PREFIX                     "file_prefix"
#define DRM_KEY_FILE_SUFFIX                     "file_suffix"
#define DRM_KEY_FILE_DURATION                   "file_duration"
#define DRM_KEY_FILE_INDEX                      "file_index"
#define DRM_KEY_FILE_TIME                       "file_time"
#define DRM_KEY_MUXER_ID                        "muxer_id"
#define DRM_KEY_MUXER_RKAUDIO_AVDICTIONARY      "muxer_rkaudio_avdictionary"
#define DRM_KEY_ENABLE_STREAMING                "enable_streaming"
#define DRM_KEY_PRE_RECORD_TIME                 "pre_record_time"
#define DRM_KEY_PRE_RECORD_CACHE_TIME           "pre_record_cache_time"
#define DRM_KEY_PRE_RECORD_MODE                 "pre_record_mode"
#define DRM_KEY_LAPSE_RECORD                    "lapse_record"

#define DRM_KEY_CONNECTOR_ID                    "connector_id"
#define DRM_KEY_CRTC_ID                         "crtc_id"
#define DRM_KEY_ENCODER_ID                      "encoder_id"
#define DRM_KEY_PLANE_ID                        "plane_id"
#define DRM_KEY_SKIP_PLANE_IDS                  "skip_plane_ids"
#define DRM_KEY_PLANE_TYPE                      "plane_type"
#define DRM_KEY_OVERLAY                         "Overlay"
#define DRM_KEY_PRIMARY                         "Primary"
#define DRM_KEY_CURSOR                          "Cursor"

#define DRM_KEY_FB_ID                           "FB_ID"
#define DRM_KEY_CRTC_X                          "CRTC_X"
#define DRM_KEY_CRTC_Y                          "CRTC_Y"
#define DRM_KEY_CRTC_W                          "CRTC_W"
#define DRM_KEY_CRTC_H                          "CRTC_H"
#define DRM_KEY_SRC_X                           "SRC_X"
#define DRM_KEY_SRC_Y                           "SRC_Y"
#define DRM_KEY_SRC_W                           "SRC_W"
#define DRM_KEY_SRC_H                           "SRC_H"
#define DRM_KEY_ZPOS                            "ZPOS"
#define DRM_KEY_FEATURE                         "FEATURE"
#define DRM_KEY_ASYNC_COMMIT                    "ASYNC_COMMIT"

#define DRM_KEY_DETECT_RECT                     "detect_rect"
#define DRM_KEY_OUTPUT_WANT_FLOAT               "rknn_output_want_float"
#define DRM_KEY_TENSOR_TYPE                     "tensor_type"
#define DRM_KEY_TENSOR_FMT                      "tensor_fmt"
#define DRM_KEY_NCHW                            "NCHW"
#define DRM_KEY_NHWC                            "NHWC"
#define DRM_KEY_FACE_DETECT_TRACK_FRAME         "track_frame"
#define DRM_KEY_FACE_DETECT_LANDMARK            "detect_landmark"
#define DRM_KEY_NEED_HW_DRAW                    "need_hw_draw"
#define DRM_KEY_DRAW_RECT_THICK                 "draw_rect_thick"
#define DRM_KEY_DRAW_MIN_RECT                   "min_rect"
#define DRM_KEY_DRAW_OFFSET_X                   "offset_x"
#define DRM_KEY_DRAW_OFFSET_Y                   "offset_y"
#define DRM_KEY_FRAME_INTERVAL                  "frame_interval"
#define DRM_KEY_SCORE_THRESHOD                  "score_threshod"
#define DRM_KEY_FRAME_RATE                      "frame_rate"
#define DRM_KEY_FRAME_CACHES                    "frame_caches"
#define DRM_KEY_ENABLE                          "enable"
#define DRM_KEY_BODY_PERCENTAGE                 "percentage"
#define DRM_KEY_BODY_DURATION                   "duration"
#define DRM_KEY_ENBALE_FACE_DETECT              "enable_face_detect"
#define DRM_KEY_ENABLE_FACE_REG                 "enable_face_reg"
#define DRM_KEY_CACHE_SIZE                      "cache_size"
#define DRM_KEY_CLOCK_DELTA                     "clock_delta"

#define DRM_KEY_ROCKX_MODEL                     "rockx_model"
#define DRM_KEY_DB_PATH                         "db_path"

#define DRM_KEY_ALLOW_THROUGH_COUNT             "allow_through_count"

#define DRM_KEY_UVC_EVENT_CODE                  "uvc_event_code"
#define DRM_KEY_UVC_WIDTH                       "uvc_width"
#define DRM_KEY_UVC_HEIGHT                      "uvc_height"
#define DRM_KEY_UVC_FORMAT                      "uvc_format"

#define DRM_KEY_ROCKX_ASYNC_CALLBACK            "aysnc_callback_enable"

#endif
