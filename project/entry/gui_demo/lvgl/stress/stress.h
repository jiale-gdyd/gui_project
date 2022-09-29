#ifndef LVGL_DEMO_STRESS_H
#define LVGL_DEMO_STRESS_H

#ifdef __cplusplus
extern "C" {
#endif

#define LV_USE_DEMO_STRESS          1
#define LV_DEMO_STRESS_TIME_STEP    50

#include <lvgl/lvgl.h>

int lvgl_demo_stress(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
