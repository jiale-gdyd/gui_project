#ifndef RKRGA_IM2D_BUFFER_H
#define RKRGA_IM2D_BUFFER_H

#include "rgadbg.h"
#include "im2d_type.h"

#ifdef __cplusplus
IM_API rga_buffer_handle_t importbuffer_fd(int fd, int size);
IM_API rga_buffer_handle_t importbuffer_virtualaddr(void *va, int size);
IM_API rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, int size);
#endif

#ifdef __cplusplus
IM_API rga_buffer_handle_t importbuffer_fd(int fd, int width, int height, int format);
IM_API rga_buffer_handle_t importbuffer_virtualaddr(void *va, int width, int height, int format);
IM_API rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, int width, int height, int format);
#endif

IM_EXPORT_API rga_buffer_handle_t importbuffer_fd(int fd, im_handle_param_t *param);
IM_EXPORT_API rga_buffer_handle_t importbuffer_virtualaddr(void *va, im_handle_param_t *param);
IM_EXPORT_API rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, im_handle_param_t *param);

IM_EXPORT_API IM_STATUS releasebuffer_handle(rga_buffer_handle_t handle);

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
            memset(&im2d_api_buffer, 0x0, sizeof(im2d_api_buffer));                                             \
            rga_error("invalid parameter");                                                                     \
        }                                                                                                       \
        im2d_api_buffer;                                                                                        \
    })

#define wrapbuffer_virtualaddr(vir_addr, width, height, format, ...)                                            \
    ({                                                                                                          \
        rga_buffer_t im2d_api_buffer;                                                                           \
        int __args[] = {__VA_ARGS__};                                                                           \
        int __argc = sizeof(__args )/ sizeof(int);                                                              \
                                                                                                                \
        if (__argc == 0) {                                                                                      \
            im2d_api_buffer = wrapbuffer_virtualaddr_t(vir_addr, width, height, width, height, format);         \
        } else if (__argc == 2) {                                                                               \
            im2d_api_buffer = wrapbuffer_virtualaddr_t(vir_addr, width, height, __args[0], __args[1], format);  \
        } else {                                                                                                \
            memset(&im2d_api_buffer, 0x0, sizeof(im2d_api_buffer));                                             \
            rga_error("invalid parameter");                                                                     \
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
            memset(&im2d_api_buffer, 0x0, sizeof(im2d_api_buffer));                                             \
            rga_error("invalid parameter");                                                                     \
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
            memset(&im2d_api_buffer, 0x0, sizeof(im2d_api_buffer));                                             \
            rga_error("invalid parameter");                                                                     \
        }                                                                                                       \
        im2d_api_buffer;                                                                                        \
    })

IM_C_API rga_buffer_t wrapbuffer_fd_t(int fd, int width, int height, int wstride, int hstride, int format);
IM_C_API rga_buffer_t wrapbuffer_virtualaddr_t(void* vir_addr, int width, int height, int wstride, int hstride, int format);
IM_C_API rga_buffer_t wrapbuffer_physicaladdr_t(void* phy_addr, int width, int height, int wstride, int hstride, int format);

IM_C_API rga_buffer_t wrapbuffer_handle_t(rga_buffer_handle_t handle, int width, int height, int wstride, int hstride, int format);

#ifdef __cplusplus
#undef wrapbuffer_handle
IM_API rga_buffer_t wrapbuffer_handle(rga_buffer_handle_t  handle, int width, int height, int format);
IM_API rga_buffer_t wrapbuffer_handle(rga_buffer_handle_t  handle, int width, int height, int format, int wstride, int hstride);
#endif

#endif
