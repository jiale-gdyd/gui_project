#ifndef LV_DEMO_MUSIC_MAIN_H
#define LV_DEMO_MUSIC_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "demo_music.h"

void _lv_demo_music_resume(void);
void _lv_demo_music_pause(void);
void _lv_demo_music_play(uint32_t id);
void _lv_demo_music_album_next(bool next);

lv_obj_t *_lv_demo_music_main_create(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif
