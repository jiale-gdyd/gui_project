#ifndef _FIMG2D_H_
#define _FIMG2D_H_

#define G2D_PLANE_MAX_NR                    2

enum e_g2d_color_mode {
    G2D_COLOR_FMT_XRGB8888,
    G2D_COLOR_FMT_ARGB8888,
    G2D_COLOR_FMT_RGB565,
    G2D_COLOR_FMT_XRGB1555,
    G2D_COLOR_FMT_ARGB1555,
    G2D_COLOR_FMT_XRGB4444,
    G2D_COLOR_FMT_ARGB4444,
    G2D_COLOR_FMT_PRGB888,
    G2D_COLOR_FMT_YCbCr444,
    G2D_COLOR_FMT_YCbCr422,
    G2D_COLOR_FMT_YCbCr420,
    G2D_COLOR_FMT_A8,
    G2D_COLOR_FMT_L8,
    G2D_COLOR_FMT_A1,
    G2D_COLOR_FMT_A4,
    G2D_COLOR_FMT_MASK,

    G2D_ORDER_AXRGB          = (0 << 4),
    G2D_ORDER_RGBAX          = (1 << 4),
    G2D_ORDER_AXBGR          = (2 << 4),
    G2D_ORDER_BGRAX          = (3 << 4),
    G2D_ORDER_MASK           = (3 << 4),

    G2D_YCbCr_1PLANE         = (0 << 8),
    G2D_YCbCr_2PLANE         = (1 << 8),
    G2D_YCbCr_PLANE_MASK     = (3 << 8),

    G2D_YCbCr_ORDER_CrY1CbY0 = (0 << 12),
    G2D_YCbCr_ORDER_CbY1CrY0 = (1 << 12),
    G2D_YCbCr_ORDER_Y1CrY0Cb = (2 << 12),
    G2D_YCbCr_ORDER_Y1CbY0Cr = (3 << 12),
    G2D_YCbCr_ORDER_CrCb     = G2D_YCbCr_ORDER_CrY1CbY0,
    G2D_YCbCr_ORDER_CbCr     = G2D_YCbCr_ORDER_CbY1CrY0,
    G2D_YCbCr_ORDER_MASK     = (3 < 12),

    G2D_CSC_601              = (0 << 16),
    G2D_CSC_709              = (1 << 16),
    G2D_CSC_MASK             = (1 << 16),

    G2D_YCbCr_RANGE_NARROW   = (0 << 17),
    G2D_YCbCr_RANGE_WIDE     = (1 << 17),
    G2D_YCbCr_RANGE_MASK     = (1 << 17),

    G2D_COLOR_MODE_MASK      = 0xFFFFFFFF,
};

enum e_g2d_select_mode {
    G2D_SELECT_MODE_NORMAL  = (0 << 0),
    G2D_SELECT_MODE_FGCOLOR = (1 << 0),
    G2D_SELECT_MODE_BGCOLOR = (2 << 0),
};

enum e_g2d_repeat_mode {
    G2D_REPEAT_MODE_REPEAT,
    G2D_REPEAT_MODE_PAD,
    G2D_REPEAT_MODE_REFLECT,
    G2D_REPEAT_MODE_CLAMP,
    G2D_REPEAT_MODE_NONE,
};

enum e_g2d_scale_mode {
    G2D_SCALE_MODE_NONE = 0,
    G2D_SCALE_MODE_NEAREST,
    G2D_SCALE_MODE_BILINEAR,
    G2D_SCALE_MODE_MAX,
};

enum e_g2d_buf_type {
    G2D_IMGBUF_COLOR,
    G2D_IMGBUF_GEM,
    G2D_IMGBUF_USERPTR,
};

enum e_g2d_rop3_type {
    G2D_ROP3_DST  = 0xAA,
    G2D_ROP3_SRC  = 0xCC,
    G2D_ROP3_3RD  = 0xF0,
    G2D_ROP3_MASK = 0xFF,
};

enum e_g2d_select_alpha_src {
    G2D_SELECT_SRC_FOR_ALPHA_BLEND,
    G2D_SELECT_ROP_FOR_ALPHA_BLEND,
};

enum e_g2d_transparent_mode {
    G2D_TRANSPARENT_MODE_OPAQUE,
    G2D_TRANSPARENT_MODE_TRANSPARENT,
    G2D_TRANSPARENT_MODE_BLUESCREEN,
    G2D_TRANSPARENT_MODE_MAX,
};

enum e_g2d_color_key_mode {
    G2D_COLORKEY_MODE_DISABLE   = 0,
    G2D_COLORKEY_MODE_SRC_RGBA  = (1<<0),
    G2D_COLORKEY_MODE_DST_RGBA  = (1<<1),
    G2D_COLORKEY_MODE_SRC_YCbCr = (1<<2),
    G2D_COLORKEY_MODE_DST_YCbCr = (1<<3),
    G2D_COLORKEY_MODE_MASK      = 15,
};

enum e_g2d_alpha_blend_mode {
    G2D_ALPHA_BLEND_MODE_DISABLE,
    G2D_ALPHA_BLEND_MODE_ENABLE,
    G2D_ALPHA_BLEND_MODE_FADING,
    G2D_ALPHA_BLEND_MODE_MAX,
};

enum e_g2d_op {
    G2D_OP_CLEAR          = 0x00,
    G2D_OP_SRC            = 0x01,
    G2D_OP_DST            = 0x02,
    G2D_OP_OVER           = 0x03,
    G2D_OP_INTERPOLATE    = 0x04,
    G2D_OP_DISJOINT_CLEAR = 0x10,
    G2D_OP_DISJOINT_SRC   = 0x11,
    G2D_OP_DISJOINT_DST   = 0x12,
    G2D_OP_CONJOINT_CLEAR = 0x20,
    G2D_OP_CONJOINT_SRC   = 0x21,
    G2D_OP_CONJOINT_DST   = 0x22,
};

enum e_g2d_coeff_mode {
    G2D_COEFF_MODE_ONE,
    G2D_COEFF_MODE_ZERO,
    G2D_COEFF_MODE_SRC_ALPHA,
    G2D_COEFF_MODE_SRC_COLOR,
    G2D_COEFF_MODE_DST_ALPHA,
    G2D_COEFF_MODE_DST_COLOR,
    G2D_COEFF_MODE_GB_ALPHA,
    G2D_COEFF_MODE_GB_COLOR,
    G2D_COEFF_MODE_DISJOINT_S,
    G2D_COEFF_MODE_DISJOINT_D,
    G2D_COEFF_MODE_CONJOINT_S,
    G2D_COEFF_MODE_CONJOINT_D,
    G2D_COEFF_MODE_MASK
};

enum e_g2d_acoeff_mode {
    G2D_ACOEFF_MODE_A,
    G2D_ACOEFF_MODE_APGA,
    G2D_ACOEFF_MODE_AMGA,
    G2D_ACOEFF_MODE_MASK
};

union g2d_point_val {
    unsigned int     val;
    struct {
        unsigned int x:16;
        unsigned int y:16;
    } data;
};

union g2d_rop4_val {
    unsigned int             val;
    struct {
        enum e_g2d_rop3_type unmasked_rop3:8;
        enum e_g2d_rop3_type masked_rop3:8;
        unsigned int         reserved:16;
    } data;
};

union g2d_bitblt_cmd_val {
    unsigned int                    val;
    struct {
        unsigned int                mask_rop4_en : 1;
        unsigned int                masking_en : 1;
        enum e_g2d_select_alpha_src rop4_alpha_en : 1;
        unsigned int                dither_en : 1;
        unsigned int                resolved1 : 4;
        unsigned int                cw_en : 4;
        enum e_g2d_transparent_mode transparent_mode : 4;
        enum e_g2d_color_key_mode   color_key_mode : 4;
        enum e_g2d_alpha_blend_mode alpha_blend_mode : 4;
        unsigned int                src_pre_multiply : 1;
        unsigned int                pat_pre_multiply : 1;
        unsigned int                dst_pre_multiply : 1;
        unsigned int                dst_depre_multiply : 1;
        unsigned int                fast_solid_color_fill_en : 1;
        unsigned int                reserved : 3;
    } data;
};

union g2d_blend_func_val {
    unsigned int               val;
    struct {
        enum e_g2d_coeff_mode  src_coeff : 4;
        enum e_g2d_acoeff_mode src_coeff_src_a : 2;
        enum e_g2d_acoeff_mode src_coeff_dst_a : 2;
        enum e_g2d_coeff_mode  dst_coeff : 4;
        enum e_g2d_acoeff_mode dst_coeff_src_a : 2;
        enum e_g2d_acoeff_mode dst_coeff_dst_a : 2;
        unsigned int           inv_src_color_coeff : 1;
        unsigned int           resoled1 : 1;
        unsigned int           inv_dst_color_coeff : 1;
        unsigned int           resoled2 : 1;
        unsigned int           lighten_en : 1;
        unsigned int           darken_en : 1;
        unsigned int           win_ce_src_over_en : 2;
        unsigned int           reserved : 8;
    } data;
};

struct g2d_image {
    enum e_g2d_select_mode        select_mode;
    enum e_g2d_color_mode         color_mode;
    enum e_g2d_repeat_mode        repeat_mode;
    enum e_g2d_scale_mode         scale_mode;
    unsigned int                  xscale;
    unsigned int                  yscale;
    unsigned char                 rotate_90;
    unsigned char                 x_dir;
    unsigned char                 y_dir;
    unsigned char                 component_alpha;
    unsigned int                  width;
    unsigned int                  height;
    unsigned int                  stride;
    unsigned int                  need_free;
    unsigned int                  color;
    enum e_g2d_buf_type           buf_type;
    unsigned int                  bo[G2D_PLANE_MAX_NR];
    struct drm_exynos_g2d_userptr user_ptr[G2D_PLANE_MAX_NR];
    void                          *mapped_ptr[G2D_PLANE_MAX_NR];
};

struct g2d_context;

struct g2d_context *g2d_init(int fd);
void g2d_fini(struct g2d_context *ctx);

void g2d_config_event(struct g2d_context *ctx, void *userdata);
int g2d_exec(struct g2d_context *ctx);
int g2d_solid_fill(struct g2d_context *ctx, struct g2d_image *img, unsigned int x, unsigned int y, unsigned int w, unsigned int h);

int g2d_copy(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int dst_x, unsigned int dst_y, unsigned int w, unsigned int h);
int g2d_move(struct g2d_context *ctx, struct g2d_image *img, unsigned int src_x, unsigned int src_y, unsigned int dst_x, unsigned dst_y, unsigned int w, unsigned int h);

int g2d_copy_with_scale(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int src_w, unsigned int src_h, unsigned int dst_x, unsigned int dst_y, unsigned int dst_w, unsigned int dst_h, unsigned int negative);

int g2d_blend(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int dst_x, unsigned int dst_y, unsigned int w, unsigned int h, enum e_g2d_op op);
int g2d_scale_and_blend(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int src_w, unsigned int src_h, unsigned int dst_x, unsigned int dst_y, unsigned int dst_w, unsigned int dst_h, enum e_g2d_op op);

#endif
