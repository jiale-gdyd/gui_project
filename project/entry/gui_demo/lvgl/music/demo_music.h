#ifndef LV_DEMO_MUSIC_H
#define LV_DEMO_MUSIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

#define LV_DEMO_MUSIC_SQUARE        1
#define LV_DEMO_MUSIC_LANDSCAPE     1
#define LV_DEMO_MUSIC_ROUND         1
#define LV_DEMO_MUSIC_LARGE         1
#define LV_DEMO_MUSIC_AUTO_PLAY     0

#if LV_DEMO_MUSIC_LARGE
#define LV_DEMO_MUSIC_HANDLE_SIZE   40
#else
#define LV_DEMO_MUSIC_HANDLE_SIZE   20
#endif

const char *_lv_demo_music_get_title(uint32_t track_id);
const char *_lv_demo_music_get_artist(uint32_t track_id);
const char *_lv_demo_music_get_genre(uint32_t track_id);
uint32_t _lv_demo_music_get_track_length(uint32_t track_id);

#ifdef __cplusplus
}
#endif

#endif
