#ifndef LVGL_DEMO_BENCHMARK_H
#define LVGL_DEMO_BENCHMARK_H

#define LV_USE_DEMO_BENCHMARK           1

#if LV_USE_DEMO_BENCHMARK
// 使用16位色深的RGB565A8图像而不是ARGB8565
#define LV_DEMO_BENCHMARK_RGB565A8      0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

LV_IMG_DECLARE(img_benchmark_cogwheel_rgb);
LV_IMG_DECLARE(img_benchmark_cogwheel_alpha256);
#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
LV_IMG_DECLARE(img_benchmark_cogwheel_rgb565a8);
#else
LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
#endif
LV_IMG_DECLARE(img_benchmark_cogwheel_indexed16);
LV_IMG_DECLARE(img_benchmark_cogwheel_chroma_keyed);

#if LV_USE_FONT_COMPRESSED
LV_FONT_DECLARE(lv_font_benchmark_montserrat_12_compr_az);
LV_FONT_DECLARE(lv_font_benchmark_montserrat_16_compr_az);
LV_FONT_DECLARE(lv_font_benchmark_montserrat_28_compr_az);
#endif

typedef enum {
    // 渲染场景并将它们显示在显示器上。测量渲染时间，但它可能包含LVGL等待驱动程序的额外时间。运行每个场景几秒钟，这样性能也可以用肉眼看到。由于仅测量渲染时间并将其转换为FPS，因此可能会有非常高的值(例如1000FPS）
    LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER,

    // 与RENDER_AND_DRIVER类似，但不是测量渲染时间，而是测量系统的真实FPS。例如。即使一个场景在1毫秒内渲染，但屏幕仅每100毫秒重绘一次，结果将是10FPS
    LV_DEMO_BENCHMARK_MODE_REAL,

    // 临时显示`flush_cb`以便测量纯渲染时间。在基准测试期间不更新显示，仅在显示摘要表时更新。从每个场景渲染给定数量的帧并从中计算FPS
    LV_DEMO_BENCHMARK_MODE_RENDER_ONLY,
} lv_demo_benchmark_mode_t;

int lvgl_demo_benchmark(int argc, char *argv[]);
void lvgl_demo_benchmark_run_scene(lv_demo_benchmark_mode_t mode, uint16_t scene_no);

#ifdef __cplusplus
}
#endif

#endif
