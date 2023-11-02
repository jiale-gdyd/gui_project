#ifndef LVGL_DEMO_RENDER_H
#define LVGL_DEMO_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

typedef enum {
    LV_DEMO_RENDER_SCENE_FILL,
    LV_DEMO_RENDER_SCENE_BORDER,
    LV_DEMO_RENDER_SCENE_BOX_SHADOW,
    LV_DEMO_RENDER_SCENE_TEXT,
    LV_DEMO_RENDER_SCENE_IMAGE_NORMAL,
    LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR,
    LV_DEMO_RENDER_SCENE_LINE,
    LV_DEMO_RENDER_SCENE_ARC_NORMAL,
    LV_DEMO_RENDER_SCENE_ARC_IMAGE,
    LV_DEMO_RENDER_SCENE_TRIANGLE,
    LV_DEMO_RENDER_SCENE_LAYER_NORMAL,
    _LV_DEMO_RENDER_SCENE_NUM,
} lvgl_demo_render_scene_t;

/**
 * Run the render verification for a scenario
 * @param id   ID of the scenario to run. Element of `lv_demo_render_scene_t`
 * @param opa  set this opacity for each object
 */
int lv_demo_render(lvgl_demo_render_scene_t id, lv_opa_t opa);

const char *lv_demo_render_get_scene_name(lvgl_demo_render_scene_t id);

int lvgl_demo_render(int argc, char *argv[]);

#endif

#ifdef __cplusplus
}
#endif

#endif
