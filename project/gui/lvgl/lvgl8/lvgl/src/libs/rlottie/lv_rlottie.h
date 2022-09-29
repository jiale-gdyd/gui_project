#ifndef LV_RLOTTIE_H
#define LV_RLOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

#if LV_USE_RLOTTIE

typedef enum {
    LV_RLOTTIE_CTRL_FORWARD  = 0,
    LV_RLOTTIE_CTRL_BACKWARD = 1,
    LV_RLOTTIE_CTRL_PAUSE    = 2,
    LV_RLOTTIE_CTRL_PLAY     = 0,
    LV_RLOTTIE_CTRL_LOOP     = 8,
} lv_rlottie_ctrl_t;

struct Lottie_Animation_S;
typedef struct {
    lv_img_t                  img_ext;
    struct Lottie_Animation_S *animation;
    lv_timer_t                *task;
    lv_img_dsc_t              imgdsc;
    size_t                    total_frames;
    size_t                    current_frame;
    size_t                    framerate;
    uint32_t                  *allocated_buf;
    size_t                    allocated_buffer_size;
    size_t                    scanline_width;
    lv_rlottie_ctrl_t         play_ctrl;
    size_t                    dest_frame;
} lv_rlottie_t;

extern const lv_obj_class_t lv_rlottie_class;

lv_obj_t *lv_rlottie_create_from_file(lv_obj_t *parent,lv_coord_t width, lv_coord_t height, const char *path);
lv_obj_t *lv_rlottie_create_from_raw(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, const char *rlottie_desc);

void lv_rlottie_set_play_mode(lv_obj_t *rlottie, const lv_rlottie_ctrl_t ctrl);
void lv_rlottie_set_current_frame(lv_obj_t *rlottie, const size_t goto_frame);

#endif

#ifdef __cplusplus
}
#endif

#endif
