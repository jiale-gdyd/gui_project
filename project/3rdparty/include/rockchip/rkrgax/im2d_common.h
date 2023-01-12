#ifndef RKRGA_IM2D_COMMON_H
#define RKRGA_IM2D_COMMON_H

#include "rgadbg.h"
#include "im2d_type.h"
#include "im2d_version.h"

IM_EXPORT_API const char *querystring(int name);

#define imStrError(...)                                                                         \
    ({                                                                                          \
        const char *im2d_api_err;                                                               \
        int __args[] = {__VA_ARGS__};                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                              \
                                                                                                \
        if (__argc == 0) {                                                                      \
            im2d_api_err = imStrError_t(IM_STATUS_INVALID_PARAM);                               \
        } else if (__argc == 1) {                                                               \
            im2d_api_err = imStrError_t((IM_STATUS)__args[0]);                                  \
        } else {                                                                                \
            im2d_api_err = ("Fatal error, imStrError() too many parameters\n");                 \
            rga_error("Fatal error, imStrError() too many parameters");                         \
        }                                                                                       \
        im2d_api_err;                                                                           \
    })

IM_C_API const char *imStrError_t(IM_STATUS status);

#ifdef __cplusplus
IM_API IM_STATUS imcheckHeader(im_api_version_t header_version = RGA_CURRENT_API_HEADER_VERSION);
#endif

#define imcheck(src, dst, src_rect, dst_rect, ...)                                              \
    ({                                                                                          \
        rga_buffer_t __pat;                                                                     \
        im_rect __pat_rect;                                                                     \
        IM_STATUS __ret = IM_STATUS_NOERROR;                                                    \
                                                                                                \
        memset(&__pat, 0, sizeof(rga_buffer_t));                                                \
        memset(&__pat_rect, 0, sizeof(im_rect));                                                \
                                                                                                \
        int __args[] = {__VA_ARGS__};                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                              \
                                                                                                \
        if (__argc == 0) {                                                                      \
            __ret = imcheck_t(src, dst, __pat, src_rect, dst_rect, __pat_rect, 0);              \
        } else if (__argc == 1) {                                                               \
            __ret = imcheck_t(src, dst, __pat, src_rect, dst_rect, __pat_rect, __args[0]);      \
        } else {                                                                                \
            __ret = IM_STATUS_FAILED;                                                           \
            rga_error("check failed");                                                          \
        }                                                                                       \
        __ret;                                                                                  \
    })

#define imcheck_composite(src, dst, pat, src_rect, dst_rect, pat_rect, ...)                     \
    ({                                                                                          \
        IM_STATUS __ret = IM_STATUS_NOERROR;                                                    \
        int __args[] = {__VA_ARGS__};                                                           \
        int __argc = sizeof(__args) / sizeof(int);                                              \
                                                                                                \
        if (__argc == 0) {                                                                      \
            __ret = imcheck_t(src, dst, pat, src_rect, dst_rect, pat_rect, 0);                  \
        } else if (__argc == 1) {                                                               \
            __ret = imcheck_t(src, dst, pat, src_rect, dst_rect, pat_rect, __args[0]);          \
        } else {                                                                                \
            __ret = IM_STATUS_FAILED;                                                           \
            rga_error("check failed");                                                          \
        }                                                                                       \
        __ret;                                                                                  \
    })

IM_C_API IM_STATUS imcheck_t(const rga_buffer_t src, const rga_buffer_t dst, const rga_buffer_t pat, const im_rect src_rect, const im_rect dst_rect, const im_rect pat_rect, const int mode_usage);

IM_C_API void rga_check_perpare(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *src_rect, im_rect *dst_rect, im_rect *pat_rect, int mode_usage);

IM_EXPORT_API IM_STATUS imsync(int release_fence_fd);
IM_EXPORT_API IM_STATUS imconfig(IM_CONFIG_NAME name, uint64_t value);

#endif
