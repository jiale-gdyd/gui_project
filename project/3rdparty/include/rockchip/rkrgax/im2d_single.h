#ifndef RKRGA_IM2D_SINGLE_H
#define RKRGA_IM2D_SINGLE_H

#include "rgadbg.h"
#include "im2d_type.h"

#ifdef __cplusplus

IM_API IM_STATUS imcopy(const rga_buffer_t src, rga_buffer_t dst, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imresize(const rga_buffer_t src, rga_buffer_t dst, double fx = 0, double fy = 0, int interpolation = 0, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imcrop(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imtranslate(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imcvtcolor(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode = IM_COLOR_SPACE_DEFAULT, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imflip(const rga_buffer_t src, rga_buffer_t dst, int mode, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imrotate(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imblend(const rga_buffer_t fd_image, rga_buffer_t bg_image, int mode = IM_ALPHA_BLEND_SRC_OVER, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imcolorkey(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode = IM_ALPHA_COLORKEY_NORMAL, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imcomposite(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode = IM_ALPHA_BLEND_SRC_OVER, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imosd(const rga_buffer_t osd,const rga_buffer_t dst, const im_rect osd_rect, im_osd_t *osd_config, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imrop(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imquantize(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imfill(rga_buffer_t dst, im_rect rect, int color, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imfillArray(rga_buffer_t dst, im_rect *rect_array, int array_size, uint32_t color, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS imrectangle(rga_buffer_t dst, im_rect rect, uint32_t color, int thickness, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS imrectangleArray(rga_buffer_t dst, im_rect *rect_array, int array_size, uint32_t color, int thickness, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS immosaic(const rga_buffer_t image, im_rect rect, int mosaic_mode, int sync = 1, int *release_fence_fd = NULL);
IM_API IM_STATUS immosaicArray(const rga_buffer_t image, im_rect *rect_array, int array_size, int mosaic_mode, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS impalette(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync = 1, int *release_fence_fd = NULL);

IM_API IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt_ptr, int usage);

IM_API IM_STATUS immakeBorder(rga_buffer_t src, rga_buffer_t dst, int top, int bottom, int left, int right, int border_type, int value = 0, int sync = 1, int acquir_fence_fd = -1, int *release_fence_fd = NULL);
#endif

IM_C_API IM_STATUS immosaic(const rga_buffer_t image, im_rect rect, int mosaic_mode, int sync);
IM_C_API IM_STATUS imosd(const rga_buffer_t osd,const rga_buffer_t dst, const im_rect osd_rect, im_osd_t *osd_config, int sync);
IM_C_API IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int usage);

IM_C_API IM_STATUS imcopy_t(const rga_buffer_t src, rga_buffer_t dst, int sync);
IM_C_API IM_STATUS imresize_t(const rga_buffer_t src, rga_buffer_t dst, double fx, double fy, int interpolation, int sync);
IM_C_API IM_STATUS imcrop_t(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync);
IM_C_API IM_STATUS imtranslate_t(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync);
IM_C_API IM_STATUS imcvtcolor_t(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode, int sync);
IM_C_API IM_STATUS imrotate_t(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync);
IM_C_API IM_STATUS imflip_t (const rga_buffer_t src, rga_buffer_t dst, int mode, int sync);
IM_C_API IM_STATUS imblend_t(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode, int sync);
IM_C_API IM_STATUS imcolorkey_t(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode, int sync);
IM_C_API IM_STATUS imquantize_t(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync);
IM_C_API IM_STATUS imrop_t(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync);
IM_C_API IM_STATUS imfill_t(rga_buffer_t dst, im_rect rect, int color, int sync);
IM_C_API IM_STATUS impalette_t(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync);

#ifndef __cplusplus
#define RGA_GET_MIN(n1, n2)         ((n1) < (n2) ? (n1) : (n2))

#define imcopy(src, dst, ...)                                                                                                       \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imcopy_t(src, dst, 1);                                                                                          \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imcopy_t(src, dst, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                        \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imresize(src, dst, ...)                                                                                                                                                     \
    ({                                                                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                                                                        \
        double __args[] = {__VA_ARGS__};                                                                                                                                            \
        int __argc = sizeof(__args) / sizeof(double);                                                                                                                               \
                                                                                                                                                                                    \
        if (__argc == 0) {                                                                                                                                                          \
            __ret = imresize_t(src, dst, 0, 0, INTER_LINEAR, 1);                                                                                                                    \
        } else if (__argc == 2) {                                                                                                                                                   \
            __ret = imresize_t(src, dst, __args[RGA_GET_MIN(__argc, 0)], __args[RGA_GET_MIN(__argc, 1)], INTER_LINEAR, 1);                                                          \
        } else if (__argc == 3) {                                                                                                                                                   \
            __ret = imresize_t(src, dst, __args[RGA_GET_MIN(__argc, 0)], __args[RGA_GET_MIN(__argc, 1)], (int)__args[RGA_GET_MIN(__argc, 2)], 1);                                   \
        } else if (__argc == 4) {                                                                                                                                                   \
            __ret = imresize_t(src, dst, __args[RGA_GET_MIN(__argc, 0)], __args[RGA_GET_MIN(__argc, 1)], (int)__args[RGA_GET_MIN(__argc, 2)], (int)__args[RGA_GET_MIN(__argc, 3)]); \
        } else {                                                                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                                                                        \
            rga_error("invalid parameter");                                                                                                                                         \
        }                                                                                                                                                                           \
        __ret;                                                                                                                                                                      \
    })

#define impyramid(src, dst, direction) \
    imresize_t(src, dst, direction == IM_UP_SCALE ? 0.5 : 2, direction == IM_UP_SCALE ? 0.5 : 2, INTER_LINEAR, 1)

#define imcvtcolor(src, dst, sfmt, dfmt, ...)                                                                                       \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imcvtcolor_t(src, dst, sfmt, dfmt, IM_COLOR_SPACE_DEFAULT, 1);                                                  \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imcvtcolor_t(src, dst, sfmt, dfmt, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                                     \
        } else if (__argc == 2) {                                                                                                   \
            __ret = imcvtcolor_t(src, dst, sfmt, dfmt, (int)__args[RGA_GET_MIN(__argc, 0)], (int)__args[RGA_GET_MIN(__argc, 1)]);   \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imcrop(src, dst, rect, ...)                                                                                                 \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imcrop_t(src, dst, rect, 1);                                                                                    \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imcrop_t(src, dst, rect, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                  \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imtranslate(src, dst, x, y, ...)                                                                                            \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imtranslate_t(src, dst, x, y, 1);                                                                               \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imtranslate_t(src, dst, x, y, (int)__args[RGA_GET_MIN(__argc, 0)]);                                             \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imrotate(src, dst, rotation, ...)                                                                                           \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imrotate_t(src, dst, rotation, 1);                                                                              \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imrotate_t(src, dst, rotation, (int)__args[RGA_GET_MIN(__argc, 0)]);                                            \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imflip(src, dst, mode, ...)                                                                                                 \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imflip_t(src, dst, mode, 1);                                                                                    \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imflip_t(src, dst, mode, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                  \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imblend(srcA, dst, ...)                                                                                                     \
    ({                                                                                                                              \
        rga_buffer_t srcB;                                                                                                          \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        memset(&srcB, 0x00, sizeof(rga_buffer_t));                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imblend_t(srcA, srcB, dst, IM_ALPHA_BLEND_SRC_OVER, 1);                                                         \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                                             \
        } else if (__argc == 2) {                                                                                                   \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)], (int)__args[RGA_GET_MIN(__argc, 1)]);           \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imcomposite(srcA, srcB, dst, ...)                                                                                           \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imblend_t(srcA, srcB, dst, IM_ALPHA_BLEND_SRC_OVER, 1);                                                         \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                                             \
        } else if (__argc == 2) {                                                                                                   \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)], (int)__args[RGA_GET_MIN(__argc, 1)]);           \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imcolorkey(src, dst, range, ...)                                                                                            \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imcolorkey_t(src, dst, range, IM_ALPHA_COLORKEY_NORMAL, 1);                                                     \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imcolorkey_t(src, dst, range, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                                          \
        } else if (__argc == 2) {                                                                                                   \
            __ret = imcolorkey_t(src, dst, range, (int)__args[RGA_GET_MIN(__argc, 0)], (int)__args[RGA_GET_MIN(__argc, 1)]);        \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imquantize(src, dst, nn_info, ...)                                                                                          \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imquantize_t(src, dst, nn_info, 1);                                                                             \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imquantize_t(src, dst, nn_info, (int)__args[RGA_GET_MIN(__argc, 0)]);                                           \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imrop(src, dst, rop_code, ...)                                                                                              \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imrop_t(src, dst, rop_code, 1);                                                                                 \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imrop_t(src, dst, rop_code, (int)__args[RGA_GET_MIN(__argc, 0)]);                                               \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imfill(buf, rect, color, ...)                                                                                               \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imfill_t(buf, rect, color, 1);                                                                                  \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imfill_t(buf, rect, color, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imreset(buf, rect, color, ...)                                                                                              \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imfill_t(buf, rect, color, 1);                                                                                  \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imfill_t(buf, rect, color, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define imdraw(buf, rect, color, ...)                                                                                               \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = imfill_t(buf, rect, color, 1);                                                                                  \
        } else if (__argc == 1) {                                                                                                   \
            __ret = imfill_t(buf, rect, color, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })

#define impalette(src, dst, lut,  ...)                                                                                              \
    ({                                                                                                                              \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                                        \
        int __args[] = {__VA_ARGS__};                                                                                               \
        int __argc = sizeof(__args) / sizeof(int);                                                                                  \
                                                                                                                                    \
        if (__argc == 0) {                                                                                                          \
            __ret = impalette_t(src, dst, lut, 1);                                                                                  \
        } else if (__argc == 1) {                                                                                                   \
            __ret = impalette_t(src, dst, lut, (int)__args[RGA_GET_MIN(__argc, 0)]);                                                \
        } else {                                                                                                                    \
            __ret = IM_STATUS_INVALID_PARAM;                                                                                        \
            rga_error("invalid parameter");                                                                                         \
        }                                                                                                                           \
        __ret;                                                                                                                      \
    })
#endif

#endif