#ifndef UNITTEST_ROCKCHIP_RV11XX_UNITTEST_PRIVATE_H
#define UNITTEST_ROCKCHIP_RV11XX_UNITTEST_PRIVATE_H

#ifndef unittest_print
#define unittest_print(msg, ...)        fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef unittest_error
#define unittest_error(msg, ...)        unittest_print("\033[1;31m[TEST][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef unittest_warn
#define unittest_warn(msg, ...)         unittest_print("\033[1;33m[TEST][W]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef unittest_info
#define unittest_info(msg, ...)         unittest_print("\033[1;32m[TEST][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef unittest_debug
#define unittest_debug(msg, ...)        {}
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* libdrm使用c++显示案例 */
int rv11xx_unittest_libdrm_display_exit(void);
int rv11xx_unittest_libdrm_display_init(int argc, char *argv[]);

/* libdrm捕获摄像头显示 */
int rv11xx_unittest_camera_libdrm_display_exit(void);
int rv11xx_unittest_camera_libdrm_display_init(int argc, char *argv[]);

#if defined(__cplusplus)
}
#endif

#endif
