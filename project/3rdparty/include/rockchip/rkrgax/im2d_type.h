#ifndef RKRGA_IM2D_TYPE_H
#define RKRGA_IM2D_TYPE_H

#include <stdint.h>
#include "rga.h"

#define IM_API

#ifdef __cplusplus
#define IM_C_API                    extern "C"
#define IM_EXPORT_API               extern "C"
#else
#define IM_C_API
#define IM_EXPORT_API
#endif

#ifdef __cplusplus
#define DEFAULT_INITIALIZER(x)      = x
#else
#define DEFAULT_INITIALIZER(x)
#endif

typedef uint32_t im_ctx_id_t;
typedef uint32_t im_job_handle_t;
typedef uint32_t im_api_version_t;
typedef uint32_t rga_buffer_handle_t;

typedef enum {
    IM_HAL_TRANSFORM_ROT_90    = 1 << 0,
    IM_HAL_TRANSFORM_ROT_180   = 1 << 1,
    IM_HAL_TRANSFORM_ROT_270   = 1 << 2,
    IM_HAL_TRANSFORM_FLIP_H    = 1 << 3,
    IM_HAL_TRANSFORM_FLIP_V    = 1 << 4,
    IM_HAL_TRANSFORM_FLIP_H_V  = 1 << 5,
    IM_HAL_TRANSFORM_MASK      = 0x3f,
    IM_ALPHA_BLEND_SRC_OVER    = 1 << 6,
    IM_ALPHA_BLEND_SRC         = 1 << 7,
    IM_ALPHA_BLEND_DST         = 1 << 8,
    IM_ALPHA_BLEND_SRC_IN      = 1 << 9,
    IM_ALPHA_BLEND_DST_IN      = 1 << 10,
    IM_ALPHA_BLEND_SRC_OUT     = 1 << 11,
    IM_ALPHA_BLEND_DST_OUT     = 1 << 12,
    IM_ALPHA_BLEND_DST_OVER    = 1 << 13,
    IM_ALPHA_BLEND_SRC_ATOP    = 1 << 14,
    IM_ALPHA_BLEND_DST_ATOP    = 1 << 15,
    IM_ALPHA_BLEND_XOR         = 1 << 16,
    IM_ALPHA_BLEND_MASK        = 0x1ffc0,
    IM_ALPHA_COLORKEY_NORMAL   = 1 << 17,
    IM_ALPHA_COLORKEY_INVERTED = 1 << 18,
    IM_ALPHA_COLORKEY_MASK     = 0x60000,
    IM_SYNC                    = 1 << 19,
    IM_CROP                    = 1 << 20,
    IM_COLOR_FILL              = 1 << 21,
    IM_COLOR_PALETTE           = 1 << 22,
    IM_NN_QUANTIZE             = 1 << 23,
    IM_ROP                     = 1 << 24,
    IM_ALPHA_BLEND_PRE_MUL     = 1 << 25,
    IM_ASYNC                   = 1 << 26,
    IM_MOSAIC                  = 1 << 27,
    IM_OSD                     = 1 << 28,
    IM_PRE_INTR                = 1 << 29,
} IM_USAGE;

typedef enum {
    IM_RASTER_MODE = 1 << 0,
    IM_FBC_MODE    = 1 << 1,
    IM_TILE_MODE   = 1 << 2,
} IM_RD_MODE;

typedef enum {
    IM_SCHEDULER_RGA3_CORE0   = 1 << 0,
    IM_SCHEDULER_RGA3_CORE1   = 1 << 1,
    IM_SCHEDULER_RGA2_CORE0   = 1 << 2,
    IM_SCHEDULER_RGA3_DEFAULT = IM_SCHEDULER_RGA3_CORE0,
    IM_SCHEDULER_RGA2_DEFAULT = IM_SCHEDULER_RGA2_CORE0,
    IM_SCHEDULER_MASK         = 0x7,
    IM_SCHEDULER_DEFAULT      = 0,
} IM_SCHEDULER_CORE;

typedef enum {
    IM_ROP_AND     = 0x88,
    IM_ROP_OR      = 0xee,
    IM_ROP_NOT_DST = 0x55,
    IM_ROP_NOT_SRC = 0x33,
    IM_ROP_XOR     = 0xf6,
    IM_ROP_NOT_XOR = 0xf9,
} IM_ROP_CODE;

typedef enum {
    IM_MOSAIC_8   = 0x0,
    IM_MOSAIC_16  = 0x1,
    IM_MOSAIC_32  = 0x2,
    IM_MOSAIC_64  = 0x3,
    IM_MOSAIC_128 = 0x4,
} IM_MOSAIC_MODE;

typedef enum {
    IM_BORDER_CONSTANT = 0,
    IM_BORDER_REFLECT  = 2,
    IM_BORDER_WRAP     = 3,
} IM_BORDER_TYPE;

typedef enum {
    IM_YUV_TO_RGB_BT601_LIMIT = 1 << 0,
    IM_YUV_TO_RGB_BT601_FULL  = 2 << 0,
    IM_YUV_TO_RGB_BT709_LIMIT = 3 << 0,
    IM_YUV_TO_RGB_MASK        = 3 << 0,
    IM_RGB_TO_YUV_BT601_FULL  = 1 << 2,
    IM_RGB_TO_YUV_BT601_LIMIT = 2 << 2,
    IM_RGB_TO_YUV_BT709_LIMIT = 3 << 2,
    IM_RGB_TO_YUV_MASK        = 3 << 2,
    IM_RGB_TO_Y4              = 1 << 4,
    IM_RGB_TO_Y4_DITHER       = 2 << 4,
    IM_RGB_TO_Y1_DITHER       = 3 << 4,
    IM_Y4_MASK                = 3 << 4,
    IM_RGB_FULL               = 1 << 8,
    IM_RGB_CLIP               = 2 << 8,
    IM_YUV_BT601_LIMIT_RANGE  = 3 << 8,
    IM_YUV_BT601_FULL_RANGE   = 4 << 8,
    IM_YUV_BT709_LIMIT_RANGE  = 5 << 8,
    IM_YUV_BT709_FULL_RANGE   = 6 << 8,
    IM_FULL_CSC_MASK          = 0xf << 8,
    IM_COLOR_SPACE_DEFAULT    = 0,
} IM_COLOR_SPACE_MODE;

typedef enum {
    IM_UP_SCALE,
    IM_DOWN_SCALE,
} IM_SCALE;

typedef enum {
    INTER_NEAREST,
    INTER_LINEAR,
    INTER_CUBIC,
} IM_SCALE_MODE;

typedef enum {
    IM_CONFIG_SCHEDULER_CORE,
    IM_CONFIG_PRIORITY,
    IM_CONFIG_CHECK,
} IM_CONFIG_NAME;

typedef enum {
    IM_OSD_MODE_STATISTICS  = 0x1 << 0,
    IM_OSD_MODE_AUTO_INVERT = 0x1 << 1,
} IM_OSD_MODE;

typedef enum {
    IM_OSD_INVERT_CHANNEL_NONE  = 0x0,
    IM_OSD_INVERT_CHANNEL_Y_G   = 0x1 << 0,
    IM_OSD_INVERT_CHANNEL_C_RB  = 0x1 << 1,
    IM_OSD_INVERT_CHANNEL_ALPHA = 0x1 << 2,
    IM_OSD_INVERT_CHANNEL_COLOR = IM_OSD_INVERT_CHANNEL_Y_G | IM_OSD_INVERT_CHANNEL_C_RB,
    IM_OSD_INVERT_CHANNEL_BOTH  = IM_OSD_INVERT_CHANNEL_COLOR | IM_OSD_INVERT_CHANNEL_ALPHA,
} IM_OSD_INVERT_CHANNEL;

typedef enum {
    IM_OSD_FLAGS_INTERNAL = 0,
    IM_OSD_FLAGS_EXTERNAL,
} IM_OSD_FLAGS_MODE;

typedef enum {
    IM_OSD_INVERT_USE_FACTOR,
    IM_OSD_INVERT_USE_SWAP,
} IM_OSD_INVERT_MODE;

typedef enum {
    IM_OSD_BACKGROUND_DEFAULT_BRIGHT = 0,
    IM_OSD_BACKGROUND_DEFAULT_DARK,
} IM_OSD_BACKGROUND_DEFAULT;

typedef enum {
    IM_OSD_BLOCK_MODE_NORMAL = 0,
    IM_OSD_BLOCK_MODE_DIFFERENT,
} IM_OSD_BLOCK_WIDTH_MODE;

typedef enum {
    IM_OSD_MODE_HORIZONTAL,
    IM_OSD_MODE_VERTICAL,
} IM_OSD_DIRECTION;

typedef enum {
    IM_OSD_COLOR_PIXEL,
    IM_OSD_COLOR_EXTERNAL,
} IM_OSD_COLOR_MODE;

typedef enum {
    IM_INTR_READ_INTR  = 1 << 0,
    IM_INTR_READ_HOLD  = 1 << 1,
    IM_INTR_WRITE_INTR = 1 << 2,
} IM_PRE_INTR_FLAGS;

typedef enum {
    IM_CONTEXT_NONE            = 0x0,
    IM_CONTEXT_SRC_FIX_ENABLE  = 0x1 << 0,
    IM_CONTEXT_SRC_CACHE_INFO  = 0x1 << 1,
    IM_CONTEXT_SRC1_FIX_ENABLE = 0x1 << 2,
    IM_CONTEXT_SRC1_CACHE_INFO = 0x1 << 3,
    IM_CONTEXT_DST_FIX_ENABLE  = 0x1 << 4,
    IM_CONTEXT_DST_CACHE_INFO  = 0x1 << 5,
} IM_CONTEXT_FLAGS;

typedef enum {
    RGA_VENDOR = 0,
    RGA_VERSION,
    RGA_MAX_INPUT,
    RGA_MAX_OUTPUT,
    RGA_BYTE_STRIDE,
    RGA_SCALE_LIMIT,
    RGA_INPUT_FORMAT,
    RGA_OUTPUT_FORMAT,
    RGA_FEATURE,
    RGA_EXPECTED,
    RGA_ALL,
} IM_INFORMATION;

typedef enum {
    IM_STATUS_NOERROR       =  2,
    IM_STATUS_SUCCESS       =  1,
    IM_STATUS_NOT_SUPPORTED = -1,
    IM_STATUS_OUT_OF_MEMORY = -2,
    IM_STATUS_INVALID_PARAM = -3,
    IM_STATUS_ILLEGAL_PARAM = -4,
    IM_STATUS_ERROR_VERSION = -5,
    IM_STATUS_FAILED        =  0,
} IM_STATUS;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} im_rect;

typedef struct {
    int max;
    int min;
} im_colorkey_range;


typedef struct im_nn {
    int scale_r;
    int scale_g;
    int scale_b;
    int offset_r;
    int offset_g;
    int offset_b;
} im_nn_t;

typedef struct {
    void                *vir_addr;
    void                *phy_addr;
    int                 fd;
    int                 width;
    int                 height;
    int                 wstride;
    int                 hstride;
    int                 format;
    int                 color_space_mode;
    int                 global_alpha;
    int                 rd_mode;
    int                 color;
    im_colorkey_range   colorkey_range; 
    im_nn_t             nn;
    int                 rop_code;
    rga_buffer_handle_t handle;
} rga_buffer_t;

typedef struct im_color {
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
        uint32_t value;
    };
} im_color_t;

typedef struct im_osd_invert_factor {
    uint8_t alpha_max;
    uint8_t alpha_min;
    uint8_t yg_max;
    uint8_t yg_min;
    uint8_t crb_max;
    uint8_t crb_min;
} im_osd_invert_factor_t;

typedef struct im_osd_bpp2 {
    uint8_t    ac_swap;
    uint8_t    endian_swap;
    im_color_t color0;
    im_color_t color1;
} im_osd_bpp2_t;

typedef struct im_osd_block {
    int        width_mode;
    union {
        int    width;
        int    width_index;
    };
    int        block_count;
    int        background_config;
    int        direction;
    int        color_mode;
    im_color_t normal_color;
    im_color_t invert_color;
} im_osd_block_t;

typedef struct im_osd_invert {
    int                    invert_channel;
    int                    flags_mode;
    int                    flags_index;
    uint64_t               invert_flags;
    uint64_t               current_flags;
    int                    invert_mode;
    im_osd_invert_factor_t factor;
    int                    threash;
} im_osd_invert_t;

typedef struct im_osd {
    int             osd_mode;
    im_osd_block_t  block_parm;
    im_osd_invert_t invert_config;
    im_osd_bpp2_t   bpp2_info;
} im_osd_t;

typedef struct im_intr_config {
    uint32_t flags;
    int      read_threshold;
    int      write_start;
    int      write_step;
} im_intr_config_t;

typedef struct im_opt {
    im_api_version_t  version DEFAULT_INITIALIZER(RGA_CURRENT_API_HEADER_VERSION);
    int               color;
    im_colorkey_range colorkey_range;
    im_nn_t           nn;
    int               rop_code;
    int               priority;
    int               core;
    int               mosaic_mode;
    im_osd_t          osd_config;
    im_intr_config_t  intr_config;
    char              reserve[128];
} im_opt_t;

typedef struct im_handle_param {
    uint32_t width;
    uint32_t height;
    uint32_t format;
} im_handle_param_t;

#endif
