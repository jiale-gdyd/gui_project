#ifndef LV_HAL_H
#define LV_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_hal_disp.h"
#include "lv_hal_tick.h"
#include "lv_hal_indev.h"

#define _LV_DPX_CALC(dpi, n)    ((n) == 0 ? 0 :LV_MAX((( (dpi) * (n) + 80) / 160), 1))
#define LV_DPX(n)               _LV_DPX_CALC(lv_disp_get_dpi(NULL), n)

#ifdef __cplusplus
}
#endif

#endif
