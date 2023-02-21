#include <linux/kconfig.h>

#include <time.h>
#include <poll.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "lvgl_demo.h"

#if defined(CONFIG_LVGL_MUSIC_TEST)
#include "music/music.h"
#elif defined(CONFIG_LVGL_STRESS_TEST)
#include "stress/stress.h"
#elif defined(CONFIG_LVGL_WIDGET_TEST)
#include "widget/widget.h"
#elif defined(CONFIG_LVGL_BENCHMARK_TEST)
#include "benchmark/benchmark.h"
#elif defined(CONFIG_LVGL_KEYPAD_ENCODER_TEST)
#include "keypad/keypad_encoder.h"
#endif

#if defined(CONFIG_IMX6ULL)
#define SCREEN_WIDTH            (800)
#define SCREEN_HEIGHT           (480)
#elif defined(CONFIG_RV1126)
#define SCREEN_WIDTH            (1440)
#define SCREEN_HEIGHT           (810)
#else
#define SCREEN_WIDTH            (1440)
#define SCREEN_HEIGHT           (810)
#endif

#define DISP_BUF_SIZE           (SCREEN_WIDTH * SCREEN_HEIGHT)

static lv_color_t disp_buff[DISP_BUF_SIZE];

uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

static void mouse_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    
}

static void keypad_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{

}

static void encoder_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{

}

#if defined(CONFIG_WAYLAND_DISP_DRIVER)
static int wayland_sleep;
static struct pollfd wayland_pfd;
static lv_disp_t *wayland_disp = NULL;
static uint32_t wayland_time_till_next;

static void wayland_handle_init(void)
{
    lv_coord_t hor_res = 640;
    lv_coord_t ver_res = 480;
    wayland_disp = lv_wayland_create_window(hor_res, ver_res, "wayland", NULL);
}
#endif

int lvgl_demo_init(int argc, char *argv[])
{
    lv_init();

#if defined(CONFIG_DRM_DISP_DRIVER)
    drm_init();
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
    fbdev_init();
#elif defined(CONFIG_WAYLAND_DISP_DRIVER)
    lv_wayland_init();
#endif

#if !defined(CONFIG_WAYLAND_DISP_DRIVER)
    evdev_init();
#endif

#if defined(CONFIG_WAYLAND_DISP_DRIVER)
    wayland_handle_init();
#else
    lv_coord_t width = SCREEN_WIDTH, height = SCREEN_HEIGHT;

#if defined(CONFIG_DRM_DISP_DRIVER)
    drm_get_sizes(&width, &height, NULL);
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
    fbdev_get_sizes(&width, &height, NULL);
#endif

    lv_disp_t *disp = lv_disp_create(width, height);
    lv_disp_set_draw_buffers(disp, disp_buff, NULL, width * height, LV_DISP_RENDER_MODE_FULL);

#if defined(CONFIG_DRM_DISP_DRIVER)
    lv_disp_set_flush_cb(disp, drm_flush);
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
    lv_disp_set_flush_cb(disp, fbdev_flush);
#endif

    lv_indev_t *mouse_indev = lv_indev_create();
    lv_indev_set_type(mouse_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(mouse_indev, mouse_read_cb);

    lv_indev_t *keypad_indev = lv_indev_create();
    lv_indev_set_type(keypad_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keypad_indev, keypad_read_cb);

    lv_indev_t *encoder_indev = lv_indev_create();
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, encoder_read_cb);
#endif

#if defined(CONFIG_LVGL_MUSIC_TEST)
    lvgl_demo_music(argc, argv);
#elif defined(CONFIG_LVGL_WIDGET_TEST)
    lvgl_demo_widget(argc, argv);
#elif defined(CONFIG_LVGL_STRESS_TEST)
    lvgl_demo_stress(argc, argv);
#elif defined(CONFIG_LVGL_BENCHMARK_TEST)
    lvgl_demo_benchmark(argc, argv);
#elif defined(CONFIG_LVGL_KEYPAD_ENCODER_TEST)
    lvgl_demo_keypad_encoder(argc, argv);
#endif

    while (1) {
#if defined(CONFIG_WAYLAND_DISP_DRIVER)
        wayland_time_till_next = lv_wayland_timer_handler();
        if (!lv_wayland_window_is_open(wayland_disp)) {
            break;
        }

        if (wayland_time_till_next == LV_NO_TIMER_READY) {
            wayland_sleep = -1;
        } else if (wayland_time_till_next > INT_MAX) {
            wayland_sleep = INT_MAX;
        } else {
            wayland_sleep = wayland_time_till_next;
        }

        while ((poll(&wayland_pfd, 1, wayland_sleep) < 0) && (errno == EINTR));
#else
        lv_task_handler();
        usleep(5000);
        lv_tick_inc(1);
#endif
    }

    return 0;
}

int lvgl_demo_exit(void)
{
#if defined(CONFIG_DRM_DISP_DRIVER)
    drm_exit();
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
    fbdev_exit();
#elif defined(CONFIG_WAYLAND_DISP_DRIVER)
    lv_wayland_deinit();
#endif

    lv_deinit();
}
