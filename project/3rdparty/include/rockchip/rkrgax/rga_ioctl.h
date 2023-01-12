#ifndef RKRGA_RGA_IOCTL_H
#define RKRGA_RGA_IOCTL_H

#include <stdint.h>
#include <asm/ioctl.h>

#include "rga2_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RGA_IOC_MAGIC                               'r'
#define RGA_IOW(nr, type)                           _IOW(RGA_IOC_MAGIC, nr, type)
#define RGA_IOR(nr, type)                           _IOR(RGA_IOC_MAGIC, nr, type)
#define RGA_IOWR(nr, type)                          _IOWR(RGA_IOC_MAGIC, nr, type)

#define RGA_IOC_GET_DRVIER_VERSION                  RGA_IOR(0x1, struct rga_version_t)
#define RGA_IOC_GET_HW_VERSION                      RGA_IOR(0x2, struct rga_hw_versions_t)
#define RGA_IOC_IMPORT_BUFFER                       RGA_IOWR(0x3, struct rga_buffer_pool)
#define RGA_IOC_RELEASE_BUFFER                      RGA_IOW(0x4, struct rga_buffer_pool)
#define RGA_IOC_REQUEST_CREATE                      RGA_IOR(0x5, uint32_t)
#define RGA_IOC_REQUEST_SUBMIT                      RGA_IOWR(0x6, struct rga_user_request)
#define RGA_IOC_REQUEST_CONFIG                      RGA_IOWR(0x7, struct rga_user_request)
#define RGA_IOC_REQUEST_CANCEL                      RGA_IOWR(0x8, uint32_t)

#define RGA_BLIT_SYNC                               0x5017
#define RGA_BLIT_ASYNC                              0x5018
#define RGA_FLUSH                                   0x5019
#define RGA_GET_RESULT                              0x501a
#define RGA_GET_VERSION                             0x501b

#define RGA2_BLIT_SYNC                              0x6017
#define RGA2_BLIT_ASYNC                             0x6018
#define RGA2_FLUSH                                  0x6019
#define RGA2_GET_RESULT                             0x601a
#define RGA2_GET_VERSION                            0x601b

#define RGA_START_CONFIG                            RGA_IOC_REQUEST_CREATE
#define RGA_END_CONFIG                              RGA_IOC_REQUEST_SUBMIT
#define RGA_CMD_CONFIG                              RGA_IOC_REQUEST_CONFIG
#define RGA_CANCEL_CONFIG                           RGA_IOC_REQUEST_CANCEL

#define RGA_REG_CTRL_LEN                            0x8
#define RGA_REG_CMD_LEN                             0x1c
#define RGA_CMD_BUF_SIZE                            0x700

#define RGA_TASK_NUM_MAX                            50

#define RGA_SCHED_PRIORITY_DEFAULT                  0
#define RGA_SCHED_PRIORITY_MAX                      6

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

enum {
    rotate_mode0 = 0x0,
    rotate_mode1 = 0x1,
    rotate_mode2 = 0x2,
    rotate_mode3 = 0x3,
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

typedef struct rga_mosaic_info_ioctl {
    uint8_t enable;
    uint8_t mode;
} rga_mosaic_info_t;

typedef struct rga_pre_intr_info_ioctl {
    uint8_t  enable;
    uint8_t  read_intr_en;
    uint8_t  write_intr_en;
    uint8_t  read_hold_en;
    uint32_t read_threshold;
    uint32_t write_start;
    uint32_t write_step;
} rga_pre_intr_info_t;

typedef struct rga_osd_invert_factor_ioctl {
    uint8_t alpha_max;
    uint8_t alpha_min;
    uint8_t yg_max;
    uint8_t yg_min;
    uint8_t crb_max;
    uint8_t crb_min;
} rga_osd_invert_factor_t;

typedef struct rga_color_ioctl {
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
        uint32_t    value;
    };
} rga_color_t;

typedef struct rga_osd_bpp2_ioctl {
    uint8_t     ac_swap;
    uint8_t     endian_swap;
    rga_color_t color0;
    rga_color_t color1;
} rga_osd_bpp2_t;

typedef struct rga_osd_mode_ctrl_ioctal {
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
} rga_osd_mode_ctrl_t;

typedef struct rga_osd_info_ioctl {
    uint8_t                 enable;
    rga_osd_mode_ctrl_t     mode_ctrl;
    rga_osd_invert_factor_t cal_factor;
    rga_osd_bpp2_t          bpp2_info;

    union {
        struct {
            uint32_t last_flags1;
            uint32_t last_flags0;
        };
        uint64_t     last_flags;
    };

    union {
        struct {
            uint32_t cur_flags1;
            uint32_t cur_flags0;
        };
        uint64_t     cur_flags;
    };
} rga_osd_info_t;

#define RGA_VERSION_SIZE                            16
#define RGA_HW_SIZE                                 5

struct rga_version_t {
    uint32_t major;
    uint32_t minor;
    uint32_t revision;
    uint8_t str[RGA_VERSION_SIZE];
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
    uint8_t             render_mode;
    rga_img_info_t      src;
    rga_img_info_t      dst;
    rga_img_info_t      pat;
    uint64_t            rop_mask_addr;
    uint64_t            LUT_addr;
    RECT                clip;
    int32_t             sina;
    int32_t             cosa;
    uint16_t            alpha_rop_flag;
    uint8_t             scale_mode;
    uint32_t            color_key_max;
    uint32_t            color_key_min;
    uint32_t            fg_color;
    uint32_t            bg_color;
    COLOR_FILL          gr_color;
    line_draw_t         line_draw_info;
    FADING              fading;
    uint8_t             PD_mode;
    uint8_t             alpha_global_value;
    uint16_t            rop_code;
    uint8_t             bsfilter_flag;
    uint8_t             palette_mode;
    uint8_t             yuv2rgb_mode;
    uint8_t             endian_mode;
    uint8_t             rotate_mode;
    uint8_t             color_fill_mode;
    MMU                 mmu_info;
    uint8_t             alpha_rop_mode;
    uint8_t             src_trans_mode;
    uint8_t             dither_mode;
    full_csc_t          full_csc;
    int32_t             in_fence_fd;
    uint8_t             core;
    uint8_t             priority;
    int32_t             out_fence_fd;
    uint8_t             handle_flag;
    rga_mosaic_info_t   mosaic_info;
    uint8_t             uvhds_mode;
    uint8_t             uvvds_mode;
    rga_osd_info_t      osd_info;
    rga_pre_intr_info_t pre_intr_info;
    uint8_t             reservr[59];
};

struct rga_user_request {
    uint64_t task_ptr;
    uint32_t task_num;
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
