#ifndef ROCKCHIP_RKRGAX_IM2D_H
#define ROCKCHIP_RKRGAX_IM2D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "im2d_type.h"
#include "im2d_version.h"

#define RGA_GET_MIN(n1, n2)             ((n1) < (n2) ? (n1) : (n2))

#define imStrError(...)                                                                                         \
    ({                                                                                                          \
        const char *im2d_api_err;                                                                               \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            im2d_api_err = imStrError_t(IM_STATUS_INVALID_PARAM);                                               \
        } else if (__argc == 1) {                                                                               \
            im2d_api_err = imStrError_t((IM_STATUS)__args[0]);                                                  \
        } else {                                                                                                \
            im2d_api_err = ("Fatal error, imStrError() too many parameters\n");                                 \
            printf("Fatal error, imStrError() too many parameters\n");                                          \
        }                                                                                                       \
        im2d_api_err;                                                                                           \
    })

const char *imStrError_t(IM_STATUS status);

rga_buffer_handle_t importbuffer_fd(int fd, im_handle_param_t *param);
rga_buffer_handle_t importbuffer_virtualaddr(void *va, im_handle_param_t *param);
rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, im_handle_param_t *param);

IM_STATUS releasebuffer_handle(rga_buffer_handle_t handle);

#define wrapbuffer_handle(handle, width, height, format, ...)                                                   \
    ({                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                           \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            im2d_api_buffer = wrapbuffer_handle_t(handle, width, height, width, height, format);                \
        } else if (__argc == 2) {                                                                               \
            im2d_api_buffer = wrapbuffer_handle_t(handle, width, height, __args[0], __args[1], format);         \
        } else {                                                                                                \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        im2d_api_buffer;                                                                                        \
    })

rga_buffer_t wrapbuffer_handle_t(rga_buffer_handle_t handle, int width, int height, int wstride, int hstride, int format);

#define wrapbuffer_virtualaddr(vir_addr, width, height, format, ...)                                            \
    ({                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                           \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            im2d_api_buffer = wrapbuffer_virtualaddr_t(vir_addr, width, height, width, height, format);         \
        } else if (__argc == 2) {                                                                               \
            im2d_api_buffer = wrapbuffer_virtualaddr_t(vir_addr, width, height, __args[0], __args[1], format);  \
        } else {                                                                                                \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        im2d_api_buffer;                                                                                        \
    })

#define wrapbuffer_physicaladdr(phy_addr, width, height, format, ...)                                           \
    ({                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                           \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            im2d_api_buffer = wrapbuffer_physicaladdr_t(phy_addr, width, height, width, height, format);        \
        } else if (__argc == 2) {                                                                               \
            im2d_api_buffer = wrapbuffer_physicaladdr_t(phy_addr, width, height, __args[0], __args[1], format); \
        } else {                                                                                                \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        im2d_api_buffer;                                                                                        \
    })

#define wrapbuffer_fd(fd, width, height, format, ...)                                                           \
    ({                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                           \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            im2d_api_buffer = wrapbuffer_fd_t(fd, width, height, width, height, format);                        \
        } else if (__argc == 2) {                                                                               \
            im2d_api_buffer = wrapbuffer_fd_t(fd, width, height, __args[0], __args[1], format);                 \
        } else {                                                                                                \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        im2d_api_buffer;                                                                                        \
    })

rga_buffer_t wrapbuffer_virtualaddr_t(void* vir_addr, int width, int height, int wstride, int hstride, int format);
rga_buffer_t wrapbuffer_physicaladdr_t(void* phy_addr, int width, int height, int wstride, int hstride, int format);
rga_buffer_t wrapbuffer_fd_t(int fd, int width, int height, int wstride, int hstride, int format);

const char *querystring(int name);

#define imcheck(src, dst, src_rect, dst_rect, ...)                                                              \
    ({                                                                                                          \
        rga_buffer_t __pat;                                                                                     \
        im_rect __pat_rect;                                                                                     \
        IM_STATUS __ret = IM_STATUS_NOERROR;                                                                    \
                                                                                                                \
        memset(&__pat, 0, sizeof(rga_buffer_t));                                                                \
        memset(&__pat_rect, 0, sizeof(im_rect));                                                                \
                                                                                                                \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&__pat),         \
                (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&__pat_rect), 0);                   \
            __ret = imcheck_t(src, dst, __pat, src_rect, dst_rect, __pat_rect, 0);                              \
        } else if (__argc == 1) {                                                                               \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&__pat),         \
                (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&__pat_rect), __args[0]);           \
            __ret = imcheck_t(src, dst, __pat, src_rect, dst_rect, __pat_rect, __args[0]);                      \
        } else {                                                                                                \
            __ret = IM_STATUS_FAILED;                                                                           \
            printf("check failed\n");                                                                           \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

#define imcheck_composite(src, dst, pat, src_rect, dst_rect, pat_rect, ...)                                     \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_NOERROR;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&pat),           \
                (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&pat_rect), 0);                     \
            __ret = imcheck_t(src, dst, pat, src_rect, dst_rect, pat_rect, 0);                                  \
        } else if (__argc == 1) {                                                                               \
            rga_check_perpare((rga_buffer_t *)(&src), (rga_buffer_t *)(&dst), (rga_buffer_t *)(&pat),           \
                (im_rect *)(&src_rect), (im_rect *)(&dst_rect), (im_rect *)(&pat_rect), __args[0]);             \
            __ret = imcheck_t(src, dst, pat, src_rect, dst_rect, pat_rect, __args[0]);                          \
        } else {                                                                                                \
            __ret = IM_STATUS_FAILED;                                                                           \
            printf("check failed\n");                                                                           \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

void rga_check_perpare(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *src_rect, im_rect *dst_rect, im_rect *pat_rect, int mode_usage);
IM_STATUS imcheck_t(const rga_buffer_t src, const rga_buffer_t dst, const rga_buffer_t pat, const im_rect src_rect, const im_rect dst_rect, const im_rect pat_rect, const int mode_usage);

#define imresize(src, dst, ...)                                                                                 \
    ({                                                                                                          \
        double __args[] = {__VA_ARGS__};                                                                        \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(double);                                                           \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imresize_t(src, dst, 0, 0, INTER_LINEAR, 1);                                                \
        } else if (__argc == 2) {                                                                               \
            __ret = imresize_t(src, dst, __args[RGA_GET_MIN(__argc, 0)],                                        \
                __args[RGA_GET_MIN(__argc, 1)], INTER_LINEAR, 1);                                               \
        } else if (__argc == 3) {                                                                               \
            __ret = imresize_t(src, dst, __args[RGA_GET_MIN(__argc, 0)],                                        \
                __args[RGA_GET_MIN(__argc, 1)], (int)__args[RGA_GET_MIN(__argc, 2)], 1);                        \
        } else if (__argc == 4) {                                                                               \
            __ret = imresize_t(src, dst, __args[RGA_GET_MIN(__argc, 0)],                                        \
                __args[RGA_GET_MIN(__argc, 1)], (int)__args[RGA_GET_MIN(__argc, 2)],                            \
                (int)__args[RGA_GET_MIN(__argc, 3)]);                                                           \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

#define impyramid(src, dst, direction)                                                                          \
        imresize_t(src, dst, direction == IM_UP_SCALE ? 0.5 : 2, direction == IM_UP_SCALE ? 0.5 : 2, INTER_LINEAR, 1)

IM_STATUS imresize_t(const rga_buffer_t src, rga_buffer_t dst, double fx, double fy, int interpolation, int sync);

#define imcrop(src, dst, rect, ...)                                                                             \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imcrop_t(src, dst, rect, 1);                                                                \
        } else if (__argc == 1) {                                                                               \
            __ret = imcrop_t(src, dst, rect, (int)__args[RGA_GET_MIN(__argc, 0)]);                              \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imcrop_t(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync);

#define imrotate(src, dst, rotation, ...)                                                                       \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imrotate_t(src, dst, rotation, 1);                                                          \
        } else if (__argc == 1) {                                                                               \
            __ret = imrotate_t(src, dst, rotation, (int)__args[RGA_GET_MIN(__argc, 0)]);                        \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imrotate_t(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync);

#define imflip(src, dst, mode, ...)                                                                             \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imflip_t(src, dst, mode, 1);                                                                \
        } else if (__argc == 1) {                                                                               \
            __ret = imflip_t(src, dst, mode, (int)__args[RGA_GET_MIN(__argc, 0)]);                              \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imflip_t (const rga_buffer_t src, rga_buffer_t dst, int mode, int sync);

#define imfill(buf, rect, color, ...)                                                                           \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imfill_t(buf, rect, color, 1);                                                              \
        } else if (__argc == 1) {                                                                               \
            __ret = imfill_t(buf, rect, color, (int)__args[RGA_GET_MIN(__argc, 0)]);                            \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

#define imreset(buf, rect, color, ...)                                                                          \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imfill_t(buf, rect, color, 1);                                                              \
        } else if (__argc == 1) {                                                                               \
            __ret = imfill_t(buf, rect, color, (int)__args[RGA_GET_MIN(__argc, 0)]);                            \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

#define imdraw(buf, rect, color, ...)                                                                           \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imfill_t(buf, rect, color, 1);                                                              \
        } else if (__argc == 1) {                                                                               \
            __ret = imfill_t(buf, rect, color, (int)__args[RGA_GET_MIN(__argc, 0)]);                            \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imfill_t(rga_buffer_t dst, im_rect rect, int color, int sync);

#define impalette(src, dst, lut,  ...)                                                                          \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = impalette_t(src, dst, lut, 1);                                                              \
        } else if (__argc == 1) {                                                                               \
            __ret = impalette_t(src, dst, lut, (int)__args[RGA_GET_MIN(__argc, 0)]);                            \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS impalette_t(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync);

#define imtranslate(src, dst, x, y, ...)                                                                        \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imtranslate_t(src, dst, x, y, 1);                                                           \
        } else if (__argc == 1) {                                                                               \
            __ret = imtranslate_t(src, dst, x, y, (int)__args[RGA_GET_MIN(__argc, 0)]);                         \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imtranslate_t(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync);

#define imcopy(src, dst, ...)                                                                                   \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imcopy_t(src, dst, 1);                                                                      \
        } else if (__argc == 1) {                                                                               \
            __ret = imcopy_t(src, dst, (int)__args[RGA_GET_MIN(__argc, 0)]);                                    \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imcopy_t(const rga_buffer_t src, rga_buffer_t dst, int sync);

#define imblend(srcA, dst, ...)                                                                                 \
    ({                                                                                                          \
        rga_buffer_t srcB;                                                                                      \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
                                                                                                                \
        memset(&srcB, 0x00, sizeof(rga_buffer_t));                                                              \
                                                                                                                \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imblend_t(srcA, srcB, dst, IM_ALPHA_BLEND_SRC_OVER, 1);                                     \
        } else if (__argc == 1) {                                                                               \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                         \
        } else if (__argc == 2) {                                                                               \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)],                             \
                (int)__args[RGA_GET_MIN(__argc, 1)]);                                                           \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

#define imcomposite(srcA, srcB, dst, ...)                                                                       \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imblend_t(srcA, srcB, dst, IM_ALPHA_BLEND_SRC_OVER, 1);                                     \
        } else if (__argc == 1) {                                                                               \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                         \
        } else if (__argc == 2) {                                                                               \
            __ret = imblend_t(srcA, srcB, dst, (int)__args[RGA_GET_MIN(__argc, 0)],                             \
                (int)__args[RGA_GET_MIN(__argc, 1)]);                                                           \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imblend_t(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode, int sync);

#define imcolorkey(src, dst, range, ...)                                                                        \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imcolorkey_t(src, dst, range, IM_ALPHA_COLORKEY_NORMAL, 1);                                 \
        } else if (__argc == 1) {                                                                               \
            __ret = imcolorkey_t(src, dst, range, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                      \
        } else if (__argc == 2) {                                                                               \
            __ret = imcolorkey_t(src, dst, range, (int)__args[RGA_GET_MIN(__argc, 0)],                          \
                (int)__args[RGA_GET_MIN(__argc, 1)]);                                                           \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imcolorkey_t(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode, int sync);

#define imcvtcolor(src, dst, sfmt, dfmt, ...)                                                                   \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imcvtcolor_t(src, dst, sfmt, dfmt, IM_COLOR_SPACE_DEFAULT, 1);                              \
        } else if (__argc == 1) {                                                                               \
            __ret = imcvtcolor_t(src, dst, sfmt, dfmt, (int)__args[RGA_GET_MIN(__argc, 0)], 1);                 \
        } else if (__argc == 2) {                                                                               \
            __ret = imcvtcolor_t(src, dst, sfmt, dfmt, (int)__args[RGA_GET_MIN(__argc, 0)],                     \
                (int)__args[RGA_GET_MIN(__argc, 1)]);                                                           \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imcvtcolor_t(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode, int sync);

#define imquantize(src, dst, nn_info, ...)                                                                      \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imquantize_t(src, dst, nn_info, 1);                                                         \
        } else if (__argc == 1) {                                                                               \
            __ret = imquantize_t(src, dst, nn_info, (int)__args[RGA_GET_MIN(__argc, 0)]);                       \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imquantize_t(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync);

#define imrop(src, dst, rop_code, ...)                                                                          \
    ({                                                                                                          \
        int __args[] = {__VA_ARGS__};                                                                           \
        IM_STATUS __ret = IM_STATUS_SUCCESS;                                                                    \
        int __argc = sizeof(__args) / sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            __ret = imrop_t(src, dst, rop_code, 1);                                                             \
        } else if (__argc == 1) {                                                                               \
            __ret = imrop_t(src, dst, rop_code, (int)__args[RGA_GET_MIN(__argc, 0)]);                           \
        } else {                                                                                                \
            __ret = IM_STATUS_INVALID_PARAM;                                                                    \
            printf("invalid parameter\n");                                                                      \
        }                                                                                                       \
        __ret;                                                                                                  \
    })

IM_STATUS imrop_t(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync);
IM_STATUS immosaic(const rga_buffer_t image, im_rect rect, int mosaic_mode, int sync);
IM_STATUS imosd(const rga_buffer_t osd,const rga_buffer_t dst, const im_rect osd_rect, im_osd_t *osd_config, int sync);
IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int usage);

IM_STATUS imsync(int out_fence_fd);
IM_STATUS imconfig(IM_CONFIG_NAME name, uint64_t value);

#ifdef __cplusplus
}
#endif

#endif
