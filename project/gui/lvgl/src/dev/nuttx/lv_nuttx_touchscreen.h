/**
 * @file lv_nuttx_touchscreen.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef LV_NUTTX_TOUCHSCREEN_H
#define LV_NUTTX_TOUCHSCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_entry.h"

#if LV_USE_NUTTX_TOUCHSCREEN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_nuttx_touchscreen_create(const char * dev_path);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX_TOUCHSCREEN */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_TOUCHSCREEN_H */
