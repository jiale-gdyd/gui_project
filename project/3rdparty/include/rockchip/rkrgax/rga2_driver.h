#ifndef RKRGA_RGA2_DRIVER_H
#define RKRGA_RGA2_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__arm64__) || defined(__aarch64__)
typedef unsigned long arch_type_t;
#else
typedef unsigned int arch_type_t;
#endif

typedef struct rga2_img_info_t {
    arch_type_t    yrgb_addr;
    arch_type_t    uv_addr;
    arch_type_t    v_addr;
    unsigned int   format;
    unsigned short act_w;
    unsigned short act_h;
    unsigned short x_offset;
    unsigned short y_offset;
    unsigned short vir_w;
    unsigned short vir_h;
    unsigned short endian_mode;
    unsigned short alpha_swap;
} rga2_img_info_t;

typedef struct rga2_rect {
    unsigned short xmin;
    unsigned short xmax;
    unsigned short ymin;
    unsigned short ymax;
} rga2_rect_t;

typedef struct rga2_color_fill {
    short gr_x_a;
    short gr_y_a;
    short gr_x_b;
    short gr_y_b;
    short gr_x_g;
    short gr_y_g;
    short gr_x_r;
    short gr_y_r;
} rga2_color_fill_t;

typedef struct rga2_point {
    unsigned short x;
    unsigned short y;
} rga2_point_t;

typedef struct rga2_line_draw {
    rga2_point_t start_point;
    rga2_point_t end_point;
    unsigned int color;
    unsigned int flag;
    unsigned int line_width;
} rga2_line_draw_t;

typedef struct rga2_fading {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char res;
} rga2_fading_t;

typedef struct rga2_mmu {
    unsigned char mmu_en;
    arch_type_t   base_addr;
    unsigned int  mmu_flag;
} rga2_mmu_t;

typedef struct rga2_csc_coe {
    int16_t r_v;
    int16_t g_y;
    int16_t b_u;
    int32_t off;
} rga2_csc_coe_t;

typedef struct rga2_full_csc {
    unsigned char  flag;
    rga2_csc_coe_t coe_y;
    rga2_csc_coe_t coe_u;
    rga2_csc_coe_t coe_v;
} rga2_full_csc_t;

struct rga2_req {
    unsigned char     render_mode;
    rga2_img_info_t   src;
    rga2_img_info_t   dst;
    rga2_img_info_t   pat;
    arch_type_t       rop_mask_addr;
    arch_type_t       LUT_addr;
    rga2_rect_t       clip;
    int               sina;
    int               cosa;
    unsigned short    alpha_rop_flag;
    unsigned char     scale_mode;
    unsigned int      color_key_max;
    unsigned int      color_key_min;
    unsigned int      fg_color;
    unsigned int      bg_color;
    rga2_color_fill_t gr_color;
    rga2_line_draw_t  line_draw_info;
    rga2_fading_t     fading;
    unsigned char     PD_mode;
    unsigned char     alpha_global_value;
    unsigned short    rop_code;
    unsigned char     bsfilter_flag;
    unsigned char     palette_mode;
    unsigned char     yuv2rgb_mode;
    unsigned char     endian_mode;
    unsigned char     rotate_mode;
    unsigned char     color_fill_mode;
    rga2_mmu_t        mmu_info;
    unsigned char     alpha_rop_mode;
    unsigned char     src_trans_mode;
    unsigned char     dither_mode;
    rga2_full_csc_t   full_csc;
    unsigned char     CMD_fin_int_enable;

    void (*complete)(int retval);
};

#ifdef __cplusplus
}
#endif

#endif
