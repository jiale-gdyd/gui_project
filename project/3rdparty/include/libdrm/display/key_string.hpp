#ifndef LIBDRM_DISPLAY_KEY_STRING_HPP
#define LIBDRM_DISPLAY_KEY_STRING_HPP

#define _DRM_STR(s)                             #s
#define DRM_STR(s)                              _DRM_STR(s)

#define DRM_KEY_DEVICE                          "device"

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

#define DRM_KEY_FPS                             "framerate"
#define DRM_KEY_FPS_IN                          "framerate_in"

#define DRM_KEY_MEM_CNT                         "mem_cnt"
#define DRM_KEY_MEM_TYPE                        "mem_type"
#define DRM_KEY_MEM_ION                         "ion"
#define DRM_KEY_MEM_DRM                         "drm"
#define DRM_KEY_MEM_HARDWARE                    "hw_mem"

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

#define DRM_KEY_ALLOW_THROUGH_COUNT             "allow_through_count"

#endif
