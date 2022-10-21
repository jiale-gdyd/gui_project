#ifndef LV_DEMO_MUSIC_LIST_H
#define LV_DEMO_MUSIC_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "demo_music.h"

lv_obj_t *_lv_demo_music_list_create(lv_obj_t *parent);
void _lv_demo_music_list_btn_check(uint32_t track_id, bool state);

#ifdef __cplusplus
}
#endif

#endif
