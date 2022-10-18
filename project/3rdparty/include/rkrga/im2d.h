#ifndef RKRGA_IM2D_H
#define RKRGA_IM2D_H

#include "rgadbg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IM_HAL_TRANSFORM_ROT_90     = 1 << 0,
    IM_HAL_TRANSFORM_ROT_180    = 1 << 1,
    IM_HAL_TRANSFORM_ROT_270    = 1 << 2,
    IM_HAL_TRANSFORM_FLIP_H     = 1 << 3,
    IM_HAL_TRANSFORM_FLIP_V     = 1 << 4,
    IM_HAL_TRANSFORM_FLIP_H_V   = 1 << 5,
    IM_HAL_TRANSFORM_MASK       = 0x3f,

    IM_ALPHA_BLEND_SRC_OVER     = 1 << 6,
    IM_ALPHA_BLEND_SRC          = 1 << 7,
    IM_ALPHA_BLEND_DST          = 1 << 8,
    IM_ALPHA_BLEND_SRC_IN       = 1 << 9,
    IM_ALPHA_BLEND_DST_IN       = 1 << 10,
    IM_ALPHA_BLEND_SRC_OUT      = 1 << 11,
    IM_ALPHA_BLEND_DST_OUT      = 1 << 12,
    IM_ALPHA_BLEND_DST_OVER     = 1 << 13,
    IM_ALPHA_BLEND_SRC_ATOP     = 1 << 14,
    IM_ALPHA_BLEND_DST_ATOP     = 1 << 15,
    IM_ALPHA_BLEND_XOR          = 1 << 16,
    IM_ALPHA_BLEND_MASK         = 0x1ffc0,

    IM_ALPHA_COLORKEY_NORMAL    = 1 << 17,
    IM_ALPHA_COLORKEY_INVERTED  = 1 << 18,
    IM_ALPHA_COLORKEY_MASK      = 0x60000,

    IM_SYNC                     = 1 << 19,
    IM_ASYNC                    = 1 << 26,
    IM_CROP                     = 1 << 20,
    IM_COLOR_FILL               = 1 << 21,
    IM_COLOR_PALETTE            = 1 << 22,
    IM_NN_QUANTIZE              = 1 << 23,
    IM_ROP                      = 1 << 24,
    IM_ALPHA_BLEND_PRE_MUL      = 1 << 25,
} IM_USAGE;

typedef enum {
    IM_ROP_AND                  = 0x88,
    IM_ROP_OR                   = 0xee,
    IM_ROP_NOT_DST              = 0x55,
    IM_ROP_NOT_SRC              = 0x33,
    IM_ROP_XOR                  = 0xf6,
    IM_ROP_NOT_XOR              = 0xf9,
} IM_ROP_CODE;

typedef enum {
    IM_RGA_SUPPORT_FORMAT_ERROR_INDEX = 0,
    IM_RGA_SUPPORT_FORMAT_RGB_INDEX,
    IM_RGA_SUPPORT_FORMAT_RGB_OTHER_INDEX,
    IM_RGA_SUPPORT_FORMAT_BPP_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUV_8_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUV_10_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUYV_420_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUYV_422_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUV_400_INDEX,
    IM_RGA_SUPPORT_FORMAT_Y4_INDEX,
    IM_RGA_SUPPORT_FORMAT_MASK_INDEX,
} IM_RGA_SUPPORT_FORMAT_INDEX;

typedef enum {
    IM_RGA_SUPPORT_FORMAT_ERROR     = 1 << IM_RGA_SUPPORT_FORMAT_ERROR_INDEX,
    IM_RGA_SUPPORT_FORMAT_RGB       = 1 << IM_RGA_SUPPORT_FORMAT_RGB_INDEX,
    IM_RGA_SUPPORT_FORMAT_RGB_OTHER = 1 << IM_RGA_SUPPORT_FORMAT_RGB_OTHER_INDEX,
    IM_RGA_SUPPORT_FORMAT_BPP       = 1 << IM_RGA_SUPPORT_FORMAT_BPP_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUV_8     = 1 << IM_RGA_SUPPORT_FORMAT_YUV_8_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUV_10    = 1 << IM_RGA_SUPPORT_FORMAT_YUV_10_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUYV_420  = 1 << IM_RGA_SUPPORT_FORMAT_YUYV_420_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUYV_422  = 1 << IM_RGA_SUPPORT_FORMAT_YUYV_422_INDEX,
    IM_RGA_SUPPORT_FORMAT_YUV_400   = 1 << IM_RGA_SUPPORT_FORMAT_YUV_400_INDEX,
    IM_RGA_SUPPORT_FORMAT_Y4        = 1 << IM_RGA_SUPPORT_FORMAT_Y4_INDEX,
    IM_RGA_SUPPORT_FORMAT_MASK      = ~((~(unsigned int)0x0 << IM_RGA_SUPPORT_FORMAT_MASK_INDEX) | 1),
} IM_RGA_SUPPORT_FORMAT;

typedef enum {
    IM_RGA_SUPPORT_FEATURE_ERROR_INDEX = 0,
    IM_RGA_SUPPORT_FEATURE_COLOR_FILL_INDEX,
    IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE_INDEX,
    IM_RGA_SUPPORT_FEATURE_ROP_INDEX,
    IM_RGA_SUPPORT_FEATURE_QUANTIZE_INDEX,
    IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC_INDEX,
    IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC_INDEX,
    IM_RGA_SUPPORT_FEATURE_MASK_INDEX,
} IM_RGA_SUPPORT_FEATURE_INDEX;

typedef enum {
    IM_RGA_SUPPORT_FEATURE_ERROR         = 1 << IM_RGA_SUPPORT_FEATURE_ERROR_INDEX,
    IM_RGA_SUPPORT_FEATURE_COLOR_FILL    = 1 << IM_RGA_SUPPORT_FEATURE_COLOR_FILL_INDEX,
    IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE = 1 << IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE_INDEX,
    IM_RGA_SUPPORT_FEATURE_ROP           = 1 << IM_RGA_SUPPORT_FEATURE_ROP_INDEX,
    IM_RGA_SUPPORT_FEATURE_QUANTIZE      = 1 << IM_RGA_SUPPORT_FEATURE_QUANTIZE_INDEX,
    IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC  = 1 << IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC_INDEX,
    IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC  = 1 << IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC_INDEX,
    IM_RGA_SUPPORT_FEATURE_MASK          = ~((~(unsigned int)0x0 << IM_RGA_SUPPORT_FEATURE_MASK_INDEX) | 1),
} IM_RGA_SUPPORT_FEATURE;

typedef enum {
    IM_STATUS_NOERROR       =  2,
    IM_STATUS_SUCCESS       =  1,
    IM_STATUS_NOT_SUPPORTED = -1,
    IM_STATUS_OUT_OF_MEMORY = -2,
    IM_STATUS_INVALID_PARAM = -3,
    IM_STATUS_ILLEGAL_PARAM = -4,
    IM_STATUS_FAILED        =  0,
} IM_STATUS;

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
    RGA_VENDOR = 0,
    RGA_VERSION,
    RGA_MAX_INPUT,
    RGA_MAX_OUTPUT,
    RGA_SCALE_LIMIT,
    RGA_INPUT_FORMAT,
    RGA_OUTPUT_FORMAT,
    RGA_FEATURE,
    RGA_EXPECTED,
    RGA_ALL,
} IM_INFORMATION;

typedef enum {
    RGA_V_ERR     = 0x0,
    RGA_1         = 0x1,
    RGA_1_PLUS    = 0x2,
    RGA_2         = 0x3,
    RGA_2_LITE0   = 0x4,
    RGA_2_LITE1   = 0x5,
    RGA_2_ENHANCE = 0x6,
} RGA_VERSION_NUM;

typedef struct {
    RGA_VERSION_NUM version;
    unsigned int    input_resolution;
    unsigned int    output_resolution;
    unsigned int    scale_limit;
    unsigned int    performance;
    unsigned int    input_format;
    unsigned int    output_format;
    unsigned int    feature;
    char            reserved[28];
} rga_info_table_entry;

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
    void              *vir_addr;
    void              *phy_addr;
    int               fd;
    int               width;
    int               height;
    int               wstride;
    int               hstride;
    int               format;
    int               color_space_mode;
    int               color;
    int               global_alpha;
    im_colorkey_range colorkey_range;
    im_nn_t           nn;
    int               rop_code;
} rga_buffer_t;

#define imStrError(...)                                                                 \
    ({                                                                                  \
        const char *im2d_api_err;                                                       \
        int im2d_api_args[] = {__VA_ARGS__};                                            \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                        \
                                                                                        \
        if (im2d_api_argc == 0) {                                                       \
            im2d_api_err = imStrError_t(IM_STATUS_INVALID_PARAM);                       \
        } else if (im2d_api_argc == 1) {                                                \
            im2d_api_err = imStrError_t((IM_STATUS)im2d_api_args[0]);                   \
        } else {                                                                        \
            im2d_api_err = ("Fatal error, imStrError() too many parameters\n");         \
            rga_error("Fatal error, imStrError() too many parameters");                 \
        }                                                                               \
        im2d_api_err;                                                                   \
    })

const char *imStrError_t(IM_STATUS status);

#define wrapbuffer_virtualaddr(vir_addr, width, height, format, ...)                                                            \
    ({                                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                                           \
        int im2d_api_args[] = {__VA_ARGS__};                                                                                    \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                                                \
                                                                                                                                \
        if (im2d_api_argc == 0) {                                                                                               \
            im2d_api_buffer = wrapbuffer_virtualaddr_t(vir_addr, width, height, width, height, format);                         \
        } else if (im2d_api_argc == 2) {                                                                                        \
            im2d_api_buffer = wrapbuffer_virtualaddr_t(vir_addr, width, height, im2d_api_args[0], im2d_api_args[1], format);    \
        } else {                                                                                                                \
            rga_error("invalid parameter");                                                                                     \
        }                                                                                                                       \
        im2d_api_buffer;                                                                                                        \
    })

#define wrapbuffer_physicaladdr(phy_addr, width, height, format, ...)                                                           \
    ({                                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                                           \
        int im2d_api_args[] = {__VA_ARGS__};                                                                                    \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                                                \
                                                                                                                                \
        if (im2d_api_argc == 0) {                                                                                               \
            im2d_api_buffer = wrapbuffer_physicaladdr_t(phy_addr, width, height, width, height, format);                        \
        } else if (im2d_api_argc == 2) {                                                                                        \
            im2d_api_buffer = wrapbuffer_physicaladdr_t(phy_addr, width, height, im2d_api_args[0], im2d_api_args[1], format);   \
        } else {                                                                                                                \
            rga_error("invalid parameter\n");                                                                                   \
        }                                                                                                                       \
        im2d_api_buffer;                                                                                                        \
    })

#define wrapbuffer_fd(fd, width, height, format, ...)                                                                           \
    ({                                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                                           \
        int im2d_api_args[] = {__VA_ARGS__};                                                                                    \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                                                \
                                                                                                                                \
        if (im2d_api_argc == 0) {                                                                                               \
            im2d_api_buffer = wrapbuffer_fd_t(fd, width, height, width, height, format);                                        \
        } else if (im2d_api_argc == 2) {                                                                                        \
            im2d_api_buffer = wrapbuffer_fd_t(fd, width, height, im2d_api_args[0], im2d_api_args[1], format);                   \
        } else {                                                                                                                \
            rga_error("invalid parameter");                                                                                     \
        }                                                                                                                       \
        im2d_api_buffer;                                                                                                        \
    })

rga_buffer_t wrapbuffer_fd_t(int fd, int width, int height, int wstride, int hstride, int format);
rga_buffer_t wrapbuffer_virtualaddr_t(void *vir_addr, int width, int height, int wstride, int hstride, int format);
rga_buffer_t wrapbuffer_physicaladdr_t(void *phy_addr, int width, int height, int wstride, int hstride, int format);

IM_STATUS rga_get_info(rga_info_table_entry *return_table);

const char *querystring(int name);

#define imcheck(src, dst, src_rect, dst_rect, ...)                                                                                                                                                                  \
    ({                                                                                                                                                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_NOERROR;                                                                                                                                                                 \
        rga_buffer_t im2d_api_pat;                                                                                                                                                                                  \
        im_rect im2d_api_pat_rect;                                                                                                                                                                                  \
        memset(&im2d_api_pat, 0, sizeof(rga_buffer_t));                                                                                                                                                             \
        memset(&im2d_api_pat_rect, 0, sizeof(im_rect));                                                                                                                                                             \
        int im2d_api_args[] = {__VA_ARGS__};                                                                                                                                                                        \
        int im2d_api_argc = sizeof(im2d_api_args )/ sizeof(int);                                                                                                                                                    \
                                                                                                                                                                                                                    \
        if (im2d_api_argc == 0) {                                                                                                                                                                                   \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&im2d_api_pat), (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&im2d_api_pat_rect), 0);                 \
            im2d_api_ret = imcheck_t(src, dst, im2d_api_pat, src_rect, dst_rect, im2d_api_pat_rect, 0);                                                                                                             \
        } else if (im2d_api_argc == 1) {                                                                                                                                                                            \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&im2d_api_pat), (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&im2d_api_pat_rect), im2d_api_args[0]);  \
            im2d_api_ret = imcheck_t(src, dst, im2d_api_pat, src_rect, dst_rect, im2d_api_pat_rect, im2d_api_args[0]);                                                                                              \
        } else {                                                                                                                                                                                                    \
            im2d_api_ret = IM_STATUS_FAILED;                                                                                                                                                                        \
            rga_error("check failed");                                                                                                                                                                              \
        }                                                                                                                                                                                                           \
        im2d_api_ret;                                                                                                                                                                                               \
    })

#define imcheck_composite(src, dst, pat, src_rect, dst_rect, pat_rect, ...)                                                                                                                         \
    ({                                                                                                                                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_NOERROR;                                                                                                                                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                                                                                                                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                                                                                                                    \
                                                                                                                                                                                                    \
        if (im2d_api_argc == 0) {                                                                                                                                                                   \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&pat), (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&pat_rect), 0);                   \
            im2d_api_ret = imcheck_t(src, dst, pat, src_rect, dst_rect, pat_rect, 0);                                                                                                               \
        } else if (im2d_api_argc == 1) {                                                                                                                                                            \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&pat), (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&pat_rect), im2d_api_args[0]);    \
            im2d_api_ret = imcheck_t(src, dst, pat, src_rect, dst_rect, pat_rect, im2d_api_args[0]);                                                                                                \
        } else {                                                                                                                                                                                    \
            im2d_api_ret = IM_STATUS_FAILED;                                                                                                                                                        \
            rga_error("check failed");                                                                                                                                                              \
        }                                                                                                                                                                                           \
        im2d_api_ret;                                                                                                                                                                               \
    })

void rga_check_perpare(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *src_rect, im_rect *dst_rect, im_rect *pat_rect, int mode_usage);
IM_STATUS imcheck_t(const rga_buffer_t src, const rga_buffer_t dst, const rga_buffer_t pat, const im_rect src_rect, const im_rect dst_rect, const im_rect pat_rect, const int mode_usage);

#define imresize(src, dst, ...)                                                                                                     \
    ({                                                                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                                                                 \
        double im2d_api_args[] = {__VA_ARGS__};                                                                                     \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(double);                                                                 \
                                                                                                                                    \
        if (im2d_api_argc == 0) {                                                                                                   \
            im2d_api_ret = imresize_t(src, dst, 0, 0, INTER_LINEAR, 1);                                                             \
        } else if (im2d_api_argc == 2) {                                                                                            \
            im2d_api_ret = imresize_t(src, dst, im2d_api_args[0], im2d_api_args[1], INTER_LINEAR, 1);                               \
        } else if (im2d_api_argc == 3) {                                                                                            \
            im2d_api_ret = imresize_t(src, dst, im2d_api_args[0], im2d_api_args[1], (int)im2d_api_args[2], 1);                      \
        } else if (im2d_api_argc == 4) {                                                                                            \
            im2d_api_ret = imresize_t(src, dst, im2d_api_args[0], im2d_api_args[1], (int)im2d_api_args[2], (int)im2d_api_args[3]);  \
        } else {                                                                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                                                                 \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        im2d_api_ret;                                                                                                               \
    })

#define impyramid(src, dst, direction) \
    imresize_t(src, dst, direction == IM_UP_SCALE ? 0.5 : 2, direction == IM_UP_SCALE ? 0.5 : 2, INTER_LINEAR, 1)

IM_STATUS imresize_t(const rga_buffer_t src, rga_buffer_t dst, double fx, double fy, int interpolation, int sync);

#define imcrop(src, dst, rect, ...)                                                 \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imcrop_t(src, dst, rect, 1);                             \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imcrop_t(src, dst, rect, im2d_api_args[0]);              \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS imcrop_t(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync);

#define imrotate(src, dst, rotation, ...)                                           \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imrotate_t(src, dst, rotation, 1);                       \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imrotate_t(src, dst, rotation, im2d_api_args[0]);        \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS imrotate_t(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync);

#define imflip(src, dst, mode, ...)                                                 \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imflip_t(src, dst, mode, 1);                             \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imflip_t(src, dst, mode, im2d_api_args[0]);              \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS imflip_t (const rga_buffer_t src, rga_buffer_t dst, int mode, int sync);

#define imfill(buf, rect, color, ...)                                               \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imfill_t(buf, rect, color, 1);                           \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imfill_t(buf, rect, color, im2d_api_args[0]);            \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

#define imreset(buf, rect, color, ...)                                              \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imfill_t(buf, rect, color, 1);                           \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imfill_t(buf, rect, color, im2d_api_args[0]);            \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

#define imdraw(buf, rect, color, ...)                                               \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imfill_t(buf, rect, color, 1);                           \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imfill_t(buf, rect, color, im2d_api_args[0]);            \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS imfill_t(rga_buffer_t dst, im_rect rect, int color, int sync);

#define impalette(src, dst, lut,  ...)                                              \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = impalette_t(src, dst, lut, 1);                           \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = impalette_t(src, dst, lut, im2d_api_args[0]);            \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS impalette_t(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync);

#define imtranslate(src, dst, x, y, ...)                                            \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imtranslate_t(src, dst, x, y, 1);                        \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imtranslate_t(src, dst, x, y, im2d_api_args[0]);         \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS imtranslate_t(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync);

#define imcopy(src, dst, ...)                                                       \
    ({                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                    \
                                                                                    \
        if (im2d_api_argc == 0) {                                                   \
            im2d_api_ret = imcopy_t(src, dst, 1);                                   \
        } else if (im2d_api_argc == 1) {                                            \
            im2d_api_ret = imcopy_t(src, dst, im2d_api_args[0]);                    \
        } else {                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                 \
            rga_error("invalid parameter");                                         \
        }                                                                           \
        im2d_api_ret;                                                               \
    })

IM_STATUS imcopy_t(const rga_buffer_t src, rga_buffer_t dst, int sync);

#define imblend(srcA, dst, ...)                                                             \
    ({                                                                                      \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                         \
        rga_buffer_t srcB;                                                                  \
        memset(&srcB, 0x00, sizeof(rga_buffer_t));                                          \
        int im2d_api_args[] = {__VA_ARGS__};                                                \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                            \
                                                                                            \
        if (im2d_api_argc == 0) {                                                           \
            im2d_api_ret = imblend_t(srcA, srcB, dst, IM_ALPHA_BLEND_SRC_OVER, 1);          \
        } else if (im2d_api_argc == 1) {                                                    \
            im2d_api_ret = imblend_t(srcA, srcB, dst, im2d_api_args[0], 1);                 \
        } else if (im2d_api_argc == 2) {                                                    \
            im2d_api_ret = imblend_t(srcA, srcB, dst, im2d_api_args[0], im2d_api_args[1]);  \
        } else {                                                                            \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                         \
            rga_error("invalid parameter");                                                 \
        }                                                                                   \
        im2d_api_ret;                                                                       \
    })

#define imcomposite(srcA, srcB, dst, ...)                                                   \
    ({                                                                                      \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                         \
        int im2d_api_args[] = {__VA_ARGS__};                                                \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                            \
                                                                                            \
        if (im2d_api_argc == 0) {                                                           \
            im2d_api_ret = imblend_t(srcA, srcB, dst, IM_ALPHA_BLEND_SRC_OVER, 1);          \
        } else if (im2d_api_argc == 1) {                                                    \
            im2d_api_ret = imblend_t(srcA, srcB, dst, im2d_api_args[0], 1);                 \
        } else if (im2d_api_argc == 2) {                                                    \
            im2d_api_ret = imblend_t(srcA, srcB, dst, im2d_api_args[0], im2d_api_args[1]);  \
        } else {                                                                            \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                         \
            rga_error("invalid parameter");                                                 \
        }                                                                                   \
        im2d_api_ret;                                                                       \
    })

IM_STATUS imblend_t(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode, int sync);

#define imcolorkey(src, dst, range, ...)                                                        \
    ({                                                                                          \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                             \
        int im2d_api_args[] = {__VA_ARGS__};                                                    \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                \
                                                                                                \
        if (im2d_api_argc == 0) {                                                               \
            im2d_api_ret = imcolorkey_t(src, dst, range, IM_ALPHA_COLORKEY_NORMAL, 1);          \
        } else if (im2d_api_argc == 1) {                                                        \
            im2d_api_ret = imcolorkey_t(src, dst, range, im2d_api_args[0], 1);                  \
        } else if (im2d_api_argc == 2) {                                                        \
            im2d_api_ret = imcolorkey_t(src, dst, range, im2d_api_args[0], im2d_api_args[1]);   \
        } else {                                                                                \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                             \
            rga_error("invalid parameter");                                                     \
        }                                                                                       \
        im2d_api_ret;                                                                           \
    })

IM_STATUS imcolorkey_t(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode, int sync);

#define imcvtcolor(src, dst, sfmt, dfmt, ...)                                                       \
    ({                                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                    \
                                                                                                    \
        if (im2d_api_argc == 0) {                                                                   \
            im2d_api_ret = imcvtcolor_t(src, dst, sfmt, dfmt, IM_COLOR_SPACE_DEFAULT, 1);           \
        } else if (im2d_api_argc == 1) {                                                            \
            im2d_api_ret = imcvtcolor_t(src, dst, sfmt, dfmt, im2d_api_args[0], 1);                 \
        } else if (im2d_api_argc == 2) {                                                            \
            im2d_api_ret = imcvtcolor_t(src, dst, sfmt, dfmt, im2d_api_args[0], im2d_api_args[1]);  \
        } else {                                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                                 \
            rga_error("invalid parameter");                                                         \
        }                                                                                           \
        im2d_api_ret;                                                                               \
    })

IM_STATUS imcvtcolor_t(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode, int sync);

#define imquantize(src, dst, nn_info, ...)                                                          \
    ({                                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                    \
                                                                                                    \
        if (im2d_api_argc == 0) {                                                                   \
            im2d_api_ret = imquantize_t(src, dst, nn_info, 1);                                      \
        } else if (im2d_api_argc == 1) {                                                            \
            im2d_api_ret = imquantize_t(src, dst, nn_info, im2d_api_args[0]);                       \
        } else {                                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                                 \
            rga_error("invalid parameter");                                                         \
        }                                                                                           \
        im2d_api_ret;                                                                               \
    })

IM_STATUS imquantize_t(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync);

#define imrop(src, dst, rop_code, ...)                                                              \
    ({                                                                                              \
        IM_STATUS im2d_api_ret = IM_STATUS_SUCCESS;                                                 \
        int im2d_api_args[] = {__VA_ARGS__};                                                        \
        int im2d_api_argc = sizeof(im2d_api_args) / sizeof(int);                                    \
                                                                                                    \
        if (im2d_api_argc == 0) {                                                                   \
            im2d_api_ret = imrop_t(src, dst, rop_code, 1);                                          \
        } else if (im2d_api_argc == 1) {                                                            \
            im2d_api_ret = imrop_t(src, dst, rop_code, im2d_api_args[0]);                           \
        } else {                                                                                    \
            im2d_api_ret = IM_STATUS_INVALID_PARAM;                                                 \
            rga_error("invalid parameter");                                                         \
        }                                                                                           \
        im2d_api_ret;                                                                               \
    })

IM_STATUS imrop_t(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync);

IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int usage);
IM_STATUS imsync(void);

#ifdef __cplusplus
}
#endif

#endif
