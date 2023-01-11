#ifndef ROCKCHIP_RKRGAX_RGA_H
#define ROCKCHIP_RKRGAX_RGA_H

#include <stdint.h>
#include <asm/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RGA_IOC_MAGIC                                       'r'
#define RGA_IOW(nr, type)                                   _IOW(RGA_IOC_MAGIC, nr, type)
#define RGA_IOR(nr, type)                                   _IOR(RGA_IOC_MAGIC, nr, type)
#define RGA_IOWR(nr, type)                                  _IOWR(RGA_IOC_MAGIC, nr, type)

#define RGA_IOC_GET_DRVIER_VERSION                          RGA_IOR(0x1, struct rga_version_t)
#define RGA_IOC_GET_HW_VERSION                              RGA_IOR(0x2, struct rga_hw_versions_t)
#define RGA_IOC_IMPORT_BUFFER                               RGA_IOWR(0x3, struct rga_buffer_pool)
#define RGA_IOC_RELEASE_BUFFER                              RGA_IOW(0x4, struct rga_buffer_pool)
#define RGA_IOC_REQUEST_CREATE                              RGA_IOR(0x5, uint32_t)
#define RGA_IOC_REQUEST_SUBMIT                              RGA_IOWR(0x6, struct rga_user_request)
#define RGA_IOC_REQUEST_CONFIG                              RGA_IOWR(0x7, struct rga_user_request)
#define RGA_IOC_REQUEST_CANCEL                              RGA_IOWR(0x8, uint32_t)

#define RGA_BLIT_SYNC                                       0x5017
#define RGA_BLIT_ASYNC                                      0x5018
#define RGA_FLUSH                                           0x5019
#define RGA_GET_RESULT                                      0x501a
#define RGA_GET_VERSION                                     0x501b

#define RGA2_BLIT_SYNC                                      0x6017
#define RGA2_BLIT_ASYNC                                     0x6018
#define RGA2_FLUSH                                          0x6019
#define RGA2_GET_RESULT                                     0x601a
#define RGA2_GET_VERSION                                    0x601b

#define RGA_START_CONFIG                                    RGA_IOC_REQUEST_CREATE
#define RGA_END_CONFIG                                      RGA_IOC_REQUEST_SUBMIT
#define RGA_CMD_CONFIG                                      RGA_IOC_REQUEST_CONFIG
#define RGA_CANCEL_CONFIG                                   RGA_IOC_REQUEST_CANCEL

#define RGA_REG_CTRL_LEN                                    0x8
#define RGA_REG_CMD_LEN                                     0x1c
#define RGA_CMD_BUF_SIZE                                    0x700

#define RGA_TASK_NUM_MAX                                    50

enum rga_memory_type {
    RGA_DMA_BUFFER = 0,
    RGA_VIRTUAL_ADDRESS,
    RGA_PHYSICAL_ADDRESS
};

enum {
    bitblt_mode               = 0x0,
    color_palette_mode        = 0x1,
    color_fill_mode           = 0x2,
    line_point_drawing_mode   = 0x3,
    blur_sharp_filter_mode    = 0x4,
    pre_scaling_mode          = 0x5,
    update_palette_table_mode = 0x6,
    update_patten_buff_mode   = 0x7,
};

enum {
    rop_enable_mask    = 0x2,
    dither_enable_mask = 0x8,
    fading_enable_mask = 0x10,
    PD_enbale_mask     = 0x20,
};

enum {
    yuv2rgb_mode0                 = 0x0,
    yuv2rgb_mode1                 = 0x1,
    yuv2rgb_mode2                 = 0x2,

    rgb2yuv_601_full              = 0x1 << 8,
    rgb2yuv_709_full              = 0x2 << 8,
    yuv2yuv_601_limit_2_709_limit = 0x3 << 8,
    yuv2yuv_601_limit_2_709_full  = 0x4 << 8,
    yuv2yuv_709_limit_2_601_limit = 0x5 << 8,
    yuv2yuv_709_limit_2_601_full  = 0x6 << 8,
    yuv2yuv_601_full_2_709_limit  = 0x7 << 8,
    yuv2yuv_601_full_2_709_full   = 0x8 << 8,
    yuv2yuv_709_full_2_601_limit  = 0x9 << 8,
    yuv2yuv_709_full_2_601_full   = 0xa << 8,
    full_csc_mask = 0xf00,
};

enum {
    rotate_mode0 = 0x0,
    rotate_mode1 = 0x1,
    rotate_mode2 = 0x2,
    rotate_mode3 = 0x3,
};

enum {
    color_palette_mode0 = 0x0,
    color_palette_mode1 = 0x1,
    color_palette_mode2 = 0x2,
    color_palette_mode3 = 0x3,
};

enum {
    BB_BYPASS   = 0x0,
    BB_ROTATE   = 0x1,
    BB_X_MIRROR = 0x2,
    BB_Y_MIRROR = 0x3 
};

enum {
    nearby   = 0x0,
    bilinear = 0x1,
    bicubic  = 0x2,
};

#define RGA_SCHED_PRIORITY_DEFAULT                          0
#define RGA_SCHED_PRIORITY_MAX                              6

enum {
    RGA3_SCHEDULER_CORE0 = 1 << 0,
    RGA3_SCHEDULER_CORE1 = 1 << 1,
    RGA2_SCHEDULER_CORE0 = 1 << 2,
};

typedef enum _Rga_SURF_FORMAT {
    RK_FORMAT_RGBA_8888        = 0x0 << 8,
    RK_FORMAT_RGBX_8888        = 0x1 << 8,
    RK_FORMAT_RGB_888          = 0x2 << 8,
    RK_FORMAT_BGRA_8888        = 0x3 << 8,
    RK_FORMAT_RGB_565          = 0x4 << 8,
    RK_FORMAT_RGBA_5551        = 0x5 << 8,
    RK_FORMAT_RGBA_4444        = 0x6 << 8,
    RK_FORMAT_BGR_888          = 0x7 << 8,

    RK_FORMAT_YCbCr_422_SP     = 0x8 << 8,
    RK_FORMAT_YCbCr_422_P      = 0x9 << 8,
    RK_FORMAT_YCbCr_420_SP     = 0xa << 8,
    RK_FORMAT_YCbCr_420_P      = 0xb << 8,

    RK_FORMAT_YCrCb_422_SP     = 0xc << 8,
    RK_FORMAT_YCrCb_422_P      = 0xd << 8,
    RK_FORMAT_YCrCb_420_SP     = 0xe << 8,
    RK_FORMAT_YCrCb_420_P      = 0xf << 8,

    RK_FORMAT_BPP1             = 0x10 << 8,
    RK_FORMAT_BPP2             = 0x11 << 8,
    RK_FORMAT_BPP4             = 0x12 << 8,
    RK_FORMAT_BPP8             = 0x13 << 8,

    RK_FORMAT_Y4               = 0x14 << 8,
    RK_FORMAT_YCbCr_400        = 0x15 << 8,

    RK_FORMAT_BGRX_8888        = 0x16 << 8,

    RK_FORMAT_YVYU_422         = 0x18 << 8,
    RK_FORMAT_YVYU_420         = 0x19 << 8,
    RK_FORMAT_VYUY_422         = 0x1a << 8,
    RK_FORMAT_VYUY_420         = 0x1b << 8,
    RK_FORMAT_YUYV_422         = 0x1c << 8,
    RK_FORMAT_YUYV_420         = 0x1d << 8,
    RK_FORMAT_UYVY_422         = 0x1e << 8,
    RK_FORMAT_UYVY_420         = 0x1f << 8,

    RK_FORMAT_YCbCr_420_SP_10B = 0x20 << 8,
    RK_FORMAT_YCrCb_420_SP_10B = 0x21 << 8,
    RK_FORMAT_YCbCr_422_SP_10B = 0x22 << 8,
    RK_FORMAT_YCrCb_422_SP_10B = 0x23 << 8,

    RK_FORMAT_YCbCr_422_10b_SP = RK_FORMAT_YCbCr_422_SP_10B,
    RK_FORMAT_YCrCb_422_10b_SP = RK_FORMAT_YCrCb_422_SP_10B,

    RK_FORMAT_BGR_565          = 0x24 << 8,
    RK_FORMAT_BGRA_5551        = 0x25 << 8,
    RK_FORMAT_BGRA_4444        = 0x26 << 8,

    RK_FORMAT_ARGB_8888        = 0x28 << 8,
    RK_FORMAT_XRGB_8888        = 0x29 << 8,
    RK_FORMAT_ARGB_5551        = 0x2a << 8,
    RK_FORMAT_ARGB_4444        = 0x2b << 8,
    RK_FORMAT_ABGR_8888        = 0x2c << 8,
    RK_FORMAT_XBGR_8888        = 0x2d << 8,
    RK_FORMAT_ABGR_5551        = 0x2e << 8,
    RK_FORMAT_ABGR_4444        = 0x2f << 8,

    RK_FORMAT_RGBA2BPP         = 0x30 << 8,

    RK_FORMAT_UNKNOWN          = 0x100 << 8,
} RgaSURF_FORMAT;

enum {
    raster_mode = 0x1 << 0,
    fbc_mode    = 0x1 << 1,
    tile_mode   = 0x1 << 2,
};

typedef struct rga_img_info_t {
    uint64_t yrgb_addr;
    uint64_t uv_addr;
    uint64_t v_addr;

    uint32_t format;
    uint16_t act_w;
    uint16_t act_h;
    uint16_t x_offset;
    uint16_t y_offset;

    uint16_t vir_w;
    uint16_t vir_h;

    uint16_t endian_mode;
    uint16_t alpha_swap;

    uint16_t rotate_mode;
    uint16_t rd_mode;

    uint16_t is_10b_compact;
    uint16_t is_10b_endian;

    uint16_t enable;
} rga_img_info_t;

typedef struct POINT {
    uint16_t x;
    uint16_t y;
} POINT;

typedef struct RECT {
    uint16_t xmin;
    uint16_t xmax;
    uint16_t ymin;
    uint16_t ymax;
} RECT;

typedef struct MMU {
    uint8_t  mmu_en;
    uint64_t base_addr;
    uint32_t mmu_flag;
} MMU;

typedef struct COLOR_FILL {
    int16_t gr_x_a;
    int16_t gr_y_a;
    int16_t gr_x_b;
    int16_t gr_y_b;
    int16_t gr_x_g;
    int16_t gr_y_g;
    int16_t gr_x_r;
    int16_t gr_y_r;
} COLOR_FILL;

typedef struct FADING {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t res;
} FADING;

typedef struct line_draw_t {
    POINT    start_point;
    POINT    end_point;
    uint32_t color;
    uint32_t flag;
    uint32_t line_width;
} line_draw_t;

typedef struct csc_coe_t {
    int16_t r_v;
    int16_t g_y;
    int16_t b_u;
    int32_t off;
} csc_coe_t;

typedef struct full_csc_t {
    uint8_t   flag;
    csc_coe_t coe_y;
    csc_coe_t coe_u;
    csc_coe_t coe_v;
} full_csc_t;

struct rga_mosaic_info {
    uint8_t enable;
    uint8_t mode;
};

struct rga_pre_intr_info {
    uint8_t  enable;

    uint8_t  read_intr_en;
    uint8_t  write_intr_en;
    uint8_t  read_hold_en;
    uint32_t read_threshold;
    uint32_t write_start;
    uint32_t write_step;
};

struct rga_osd_invert_factor {
    uint8_t alpha_max;
    uint8_t alpha_min;
    uint8_t yg_max;
    uint8_t yg_min;
    uint8_t crb_max;
    uint8_t crb_min;
};

struct rga_color {
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
        uint32_t    value;
    };
};

struct rga_osd_bpp2 {
    uint8_t          ac_swap;
    uint8_t          endian_swap;
    struct rga_color color0;
    struct rga_color color1;
};

struct rga_osd_mode_ctrl {
    uint8_t  mode;
    uint8_t  direction_mode;
    uint8_t  width_mode;
    uint16_t block_fix_width;
    uint8_t  block_num;
    uint16_t flags_index;

    uint8_t  color_mode;
    uint8_t  invert_flags_mode;
    uint8_t  default_color_sel;
    uint8_t  invert_enable;
    uint8_t  invert_mode;
    uint8_t  invert_thresh;
    uint8_t  unfix_index;
};

struct rga_osd_info {
    uint8_t                      enable;

    struct rga_osd_mode_ctrl     mode_ctrl;
    struct rga_osd_invert_factor cal_factor;
    struct rga_osd_bpp2          bpp2_info;

    union {
        struct {
            uint32_t             last_flags1;
            uint32_t             last_flags0;
        };
        uint64_t                 last_flags;
    };

    union {
        struct {
            uint32_t             cur_flags1;
            uint32_t             cur_flags0;
        };
        uint64_t                 cur_flags;
    };
};

#define RGA_HW_SIZE                                         5
#define RGA_VERSION_SIZE                                    16

struct rga_version_t {
    uint32_t major;
    uint32_t minor;
    uint32_t revision;
    uint8_t  str[RGA_VERSION_SIZE];
};

struct rga_hw_versions_t {
    struct rga_version_t version[RGA_HW_SIZE];
    uint32_t             size;
};

struct rga_memory_parm {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t size;
};

struct rga_external_buffer {
    uint64_t               memory;
    uint32_t               type;
    uint32_t               handle;
    struct rga_memory_parm memory_info;
    uint8_t                reserve[252];
};

struct rga_buffer_pool {
    uint64_t buffers;
    uint32_t size;
};

struct rga_req {
    uint8_t                  render_mode;
    rga_img_info_t           src;
    rga_img_info_t           dst;
    rga_img_info_t           pat;
    uint64_t                 rop_mask_addr;
    uint64_t                 LUT_addr;
    RECT                     clip;
    int32_t                  sina;
    int32_t                  cosa;
    uint16_t                 alpha_rop_flag;
    uint8_t                  scale_mode;
    uint32_t                 color_key_max;
    uint32_t                 color_key_min;
    uint32_t                 fg_color;
    uint32_t                 bg_color;
    COLOR_FILL               gr_color;
    line_draw_t              line_draw_info;
    FADING                   fading;
    uint8_t                  PD_mode;
    uint8_t                  alpha_global_value;
    uint16_t                 rop_code;
    uint8_t                  bsfilter_flag;
    uint8_t                  palette_mode;
    uint8_t                  yuv2rgb_mode;
    uint8_t                  endian_mode;
    uint8_t                  rotate_mode;
    uint8_t                  color_fill_mode;
    MMU                      mmu_info;
    uint8_t                  alpha_rop_mode;
    uint8_t                  src_trans_mode;
    uint8_t                  dither_mode;
    full_csc_t               full_csc;
    int32_t                  in_fence_fd;
    uint8_t                  core;
    uint8_t                  priority;
    int32_t                  out_fence_fd;
    uint8_t                  handle_flag;
    struct rga_mosaic_info   mosaic_info;
    uint8_t                  uvhds_mode;
    uint8_t                  uvvds_mode;
    struct rga_osd_info      osd_info;
    struct rga_pre_intr_info pre_intr_info;
    uint8_t                  reservr[59];
};

struct rga_user_request {
    uint64_t task_ptr;
    uint32_t task_num;
    uint64_t cmd_ptr;
    uint32_t cmd_num;
    uint32_t id;
    uint32_t sync_mode;
    uint32_t release_fence_fd;
    uint32_t mpi_config_flags;
    uint32_t acquire_fence_fd;
    uint8_t  reservr[120];
};

#ifdef __cplusplus
}
#endif

#endif
