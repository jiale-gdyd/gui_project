#ifndef RKRGA_RGA_H
#define RKRGA_RGA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define RGA_BLIT_SYNC                           0x5017
#define RGA_BLIT_ASYNC                          0x5018
#define RGA_FLUSH                               0x5019
#define RGA_GET_RESULT                          0x501a
#define RGA_GET_VERSION                         0x501b

#define RGA2_BLIT_SYNC                          0x6017
#define RGA2_BLIT_ASYNC                         0x6018
#define RGA2_FLUSH                              0x6019
#define RGA2_GET_RESULT                         0x601a
#define RGA2_GET_VERSION                        0x601b
#define RGA2_GET_VERSION                        0x601b

#define RGA_REG_CTRL_LEN                        0x8
#define RGA_REG_CMD_LEN                         0x1c
#define RGA_CMD_BUF_SIZE                        0x700

#ifndef ENABLE
#define ENABLE                                  1
#endif

#ifndef DISABLE
#define DISABLE                                 0
#endif

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
    full_csc_mask                 = 0xf00,
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
    RK_FORMAT_YCbCr_422_10b_SP = 0x22 << 8,
    RK_FORMAT_YCrCb_422_10b_SP = 0x23 << 8,

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

    RK_FORMAT_UNKNOWN          = 0x100 << 8,
} RgaSURF_FORMAT;

typedef struct rga_img_info_t {
#if defined(__arm64__) || defined(__aarch64__)
    unsigned long  yrgb_addr;
    unsigned long  uv_addr;
    unsigned long  v_addr;
#else
    unsigned int   yrgb_addr;
    unsigned int   uv_addr;
    unsigned int   v_addr;
#endif
    unsigned int   format;
    unsigned short act_w;
    unsigned short act_h;
    unsigned short x_offset;
    unsigned short y_offset;

    unsigned short vir_w;
    unsigned short vir_h;

    unsigned short endian_mode;
    unsigned short alpha_swap;
} rga_img_info_t;


typedef struct mdp_img_act {
    unsigned short w;
    unsigned short h;
    short          x_off;
    short          y_off;
} mdp_img_act;

typedef struct RANGE {
    unsigned short min;
    unsigned short max;
} RANGE;

typedef struct POINT {
    unsigned short x;
    unsigned short y;
} POINT;

typedef struct RECT {
    unsigned short xmin;
    unsigned short xmax;
    unsigned short ymin;
    unsigned short ymax;
} RECT;

typedef struct RGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char res;
}RGB;

typedef struct MMU {
    unsigned char mmu_en;
#if defined(__arm64__) || defined(__aarch64__)
    unsigned long base_addr;
#else
    unsigned int  base_addr;
#endif
    unsigned int  mmu_flag;
} MMU;

typedef struct COLOR_FILL {
    short gr_x_a;
    short gr_y_a;
    short gr_x_b;
    short gr_y_b;
    short gr_x_g;
    short gr_y_g;
    short gr_x_r;
    short gr_y_r;
} COLOR_FILL;

typedef struct FADING {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char res;
} FADING;

typedef struct line_draw_t {
    POINT        start_point;
    POINT        end_point;
    unsigned int color;
    unsigned int flag;
    unsigned int line_width;
} line_draw_t;

typedef struct csc_coe_t {
    int16_t r_v;
    int16_t g_y;
    int16_t b_u;
    int32_t off;
} csc_coe_t;

typedef struct full_csc_t {
    unsigned char flag;
    csc_coe_t     coe_y;
    csc_coe_t     coe_u;
    csc_coe_t     coe_v;
} full_csc_t;

struct rga_req {
    unsigned char  render_mode;

    rga_img_info_t src;
    rga_img_info_t dst;
    rga_img_info_t pat;

#if defined(__arm64__) || defined(__aarch64__)
    unsigned long  rop_mask_addr;
    unsigned long  LUT_addr;
#else
    unsigned int   rop_mask_addr;
    unsigned int   LUT_addr;
#endif

    RECT           clip;
    int            sina;
    int            cosa;
    unsigned short alpha_rop_flag;
    unsigned char  scale_mode;
    unsigned int   color_key_max;
    unsigned int   color_key_min;
    unsigned int   fg_color;
    unsigned int   bg_color;
    COLOR_FILL     gr_color;
    line_draw_t    line_draw_info;
    FADING         fading;
    unsigned char  PD_mode;
    unsigned char  alpha_global_value;
    unsigned short rop_code;
    unsigned char  bsfilter_flag;
    unsigned char  palette_mode;
    unsigned char  yuv2rgb_mode;
    unsigned char  endian_mode;
    unsigned char  rotate_mode;
    unsigned char  color_fill_mode;
    MMU            mmu_info;
    unsigned char  alpha_rop_mode;
    unsigned char  src_trans_mode;
    unsigned char  dither_mode;
    full_csc_t     full_csc;
    unsigned char  CMD_fin_int_enable;

    void (*complete)(int retval);
};

int RGA_set_src_act_info(struct rga_req *req, unsigned int width, unsigned int height, unsigned int x_off, unsigned int y_off);

#if defined(__arm64__) || defined(__aarch64__)
int RGA_set_src_vir_info(struct rga_req *req, unsigned long yrgb_addr, unsigned long uv_addr, unsigned long v_addr, unsigned int vir_w, unsigned int vir_h, unsigned char format, unsigned char a_swap_en);
#else
int RGA_set_src_vir_info(struct rga_req *req, unsigned int yrgb_addr, unsigned int uv_addr, unsigned int v_addr, unsigned int vir_w, unsigned int vir_h, unsigned char format, unsigned char a_swap_en);
#endif

int RGA_set_dst_act_info(struct rga_req *req, unsigned int width, unsigned int height, unsigned int x_off, unsigned int y_off);

#if defined(__arm64__) || defined(__aarch64__)
int RGA_set_dst_vir_info(struct rga_req *msg, unsigned long yrgb_addr, unsigned long uv_addr, unsigned long v_addr, unsigned int vir_w, unsigned int vir_h, RECT *clip, unsigned char format, unsigned char a_swap_en);
#else
int RGA_set_dst_vir_info(struct rga_req *msg, unsigned int yrgb_addr, unsigned int uv_addr, unsigned int v_addr, unsigned int vir_w, unsigned int vir_h, RECT *clip, unsigned char format, unsigned char a_swap_en);
#endif

int RGA_set_pat_info(struct rga_req *msg, unsigned int width, unsigned int height, unsigned int x_off, unsigned int y_off, unsigned int pat_format);

#if defined(__arm64__) || defined(__aarch64__)
int RGA_set_rop_mask_info(struct rga_req *msg, unsigned long rop_mask_addr, unsigned int rop_mask_endian_mode);
#else
int RGA_set_rop_mask_info(struct rga_req *msg, unsigned int rop_mask_addr, unsigned int rop_mask_endian_mode);
#endif

int RGA_set_alpha_en_info(struct rga_req *msg, unsigned int alpha_cal_mode, unsigned int alpha_mode, unsigned int global_a_value, unsigned int PD_en, unsigned int PD_mode, unsigned int dst_alpha_en);

int RGA_set_rop_en_info(struct rga_req *msg, unsigned int ROP_mode, unsigned int ROP_code, unsigned int color_mode, unsigned int solid_color);

int RGA_set_fading_en_info(struct rga_req *msg, unsigned char r, unsigned char g, unsigned char b);

int RGA_set_src_trans_mode_info(struct rga_req *msg, unsigned char trans_mode, unsigned char a_en, unsigned char b_en, unsigned char g_en, unsigned char r_en, unsigned char color_key_min, unsigned char color_key_max, unsigned char zero_mode_en);

int RGA_set_bitblt_mode(struct rga_req *msg, unsigned char scale_mode, unsigned char rotate_mode, unsigned int angle, unsigned int dither_en, unsigned int AA_en, unsigned int yuv2rgb_mode);

int RGA_set_color_palette_mode(struct rga_req *msg, unsigned char palette_mode, unsigned char endian_mode, unsigned int bpp1_0_color, unsigned int bpp1_1_color);

int RGA_set_color_fill_mode(struct rga_req *msg, COLOR_FILL  *gr_color, unsigned char gr_satur_mode, unsigned char cf_mode, unsigned int color, unsigned short pat_width, unsigned short pat_height, unsigned char pat_x_off, unsigned char pat_y_off, unsigned char aa_en);

int RGA_set_line_point_drawing_mode(struct rga_req *msg, POINT sp, POINT ep, unsigned int color, unsigned int line_width, unsigned char AA_en, unsigned char last_point_en);

int RGA_set_blur_sharp_filter_mode(struct rga_req *msg, unsigned char filter_mode, unsigned char filter_type, unsigned char dither_en);

int RGA_set_pre_scaling_mode(struct rga_req *msg, unsigned char dither_en);

#if defined(__arm64__) || defined(__aarch64__)
int RGA_update_palette_table_mode(struct rga_req *msg, unsigned long LUT_addr, unsigned int palette_mode);
#else
int RGA_update_palette_table_mode(struct rga_req *msg, unsigned int LUT_addr, unsigned int palette_mode);
#endif

int RGA_set_update_patten_buff_mode(struct rga_req *msg, unsigned int pat_addr, unsigned int w, unsigned int h, unsigned int format);

#if defined(__arm64__) || defined(__aarch64__)
int RGA_set_mmu_info(struct rga_req *msg, unsigned char mmu_en, unsigned char src_flush, unsigned char dst_flush, unsigned char cmd_flush, unsigned long base_addr, unsigned char page_size);
#else
int RGA_set_mmu_info(struct rga_req *msg, unsigned char mmu_en, unsigned char src_flush, unsigned char dst_flush, unsigned char cmd_flush, unsigned int base_addr, unsigned char page_size);
#endif

void rga_set_fds_offsets(struct rga_req *rga_request, unsigned short src_fd, unsigned short dst_fd, unsigned int src_offset, unsigned int dst_offset);

int RGA_set_src_fence_flag(struct rga_req *msg, int acq_fence, int src_flag);
int RGA_set_dst_fence_flag(struct rga_req *msg, int dst_flag);

int RGA_get_dst_fence(struct rga_req *msg);

#ifdef __cplusplus
}
#endif

#endif
