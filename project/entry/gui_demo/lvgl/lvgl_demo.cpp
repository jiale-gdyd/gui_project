#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <linux/kconfig.h>

#include "lvgl_demo.h"

#if defined(CONFIG_LVGL_V8)
#include "stress/stress.h"
#include "benchmark/benchmark.h"
#elif defined(CONFIG_LVGL_V7)
#include "printer/printer.h"
#endif

#define DISP_BUF_SIZE           (810 * 1440)

static lv_color_t buf0[DISP_BUF_SIZE];
static lv_color_t buf1[DISP_BUF_SIZE];

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

int lvgl_demo_init(int argc, char *argv[])
{
    lv_init();

    drm_init();
    evdev_init();

#if defined(CONFIG_LVGL_V7)
    lv_coord_t width, height;
    static lv_disp_drv_t disp_drv;
    static lv_disp_buf_t disp_buf;
    static lv_indev_drv_t indev_drv;

    lv_disp_drv_init(&disp_drv);

    drm_get_sizes(&width, &height, NULL);
    lv_disp_buf_init(&disp_buf, buf0, buf1, DISP_BUF_SIZE);

    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = drm_flush;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    static lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv); 

    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = evdev_read;
    lv_indev_drv_register(&indev_drv); 

    lvgl_demo_printer(argc, argv);

#elif defined(CONFIG_LVGL_V8)
    lv_coord_t width, height;
    static lv_disp_drv_t disp_drv;
    static lv_indev_drv_t indev_drv;
    static lv_disp_draw_buf_t disp_buf;

    lv_disp_drv_init(&disp_drv);

    drm_get_sizes(&width, &height, NULL);
    lv_disp_draw_buf_init(&disp_buf, buf0, buf1, DISP_BUF_SIZE);

    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = drm_flush;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    static lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.disp = disp;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    static lv_indev_t *indev = lv_indev_drv_register(&indev_drv);

#if defined(CONFIG_LVGL_STRESS_TEST)
    lvgl_demo_stress(argc, argv);
#elif defined(CONFIG_LVGL_BENCHMARK_TEST)
    lvgl_demo_benchmark(argc, argv);
#endif
#endif

    while (1) {
        lv_task_handler();
        usleep(1);
        lv_tick_inc(1);
    }

    return 0;
}

int lvgl_demo_exit(void)
{
    drm_exit();
    lv_deinit();
}
