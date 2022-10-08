#ifndef _DEMO_GUIX_WASHING_MACHINE_H_
#define _DEMO_GUIX_WASHING_MACHINE_H_

#include <stdio.h>
#include <gx_api.h>

#include "demo_guix_washing_machine_resources.h"
#include "demo_guix_washing_machine_specifications.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID remain_time_set(INT seconds);

VOID animation_wave_draw(GX_WINDOW* window);
VOID wash_cycle_window_init(INT remain_seconds);
VOID wash_cycle_window_remain_time_update(INT remain_seconds);

VOID mode_select_window_init();
VOID mode_select_window_remain_time_update(INT remain_seconds);

VOID water_level_window_init();

#ifdef __cplusplus
}
#endif

#endif
