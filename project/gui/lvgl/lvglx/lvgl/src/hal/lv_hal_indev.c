#include "lv_hal_disp.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_mem.h"
#include "../core/lv_indev.h"
#include "../misc/lv_assert.h"
#include "../hal/lv_hal_indev.h"

#if LV_LOG_TRACE_INDEV
#define INDEV_TRACE(...)    LV_LOG_TRACE(__VA_ARGS__)
#else
#define INDEV_TRACE(...)
#endif

void lv_indev_drv_init(lv_indev_drv_t *driver)
{
    lv_memzero(driver, sizeof(lv_indev_drv_t));

    driver->type = LV_INDEV_TYPE_NONE;
    driver->scroll_limit = LV_INDEV_DEF_SCROLL_LIMIT;
    driver->scroll_throw = LV_INDEV_DEF_SCROLL_THROW;
    driver->long_press_time = LV_INDEV_DEF_LONG_PRESS_TIME;
    driver->long_press_repeat_time = LV_INDEV_DEF_LONG_PRESS_REP_TIME;
    driver->gesture_limit = LV_INDEV_DEF_GESTURE_LIMIT;
    driver->gesture_min_velocity = LV_INDEV_DEF_GESTURE_MIN_VELOCITY;
}

lv_indev_t *lv_indev_drv_register(lv_indev_drv_t *driver)
{
    if (driver->disp == NULL) {
        driver->disp = lv_disp_get_default();
    }

    if (driver->disp == NULL) {
        LV_LOG_WARN("no display registered hence can't attach the indev to a display");
        return NULL;
    }

    lv_indev_t *indev = (lv_indev_t *)_lv_ll_ins_head(&LV_GC_ROOT(_lv_indev_ll));
    LV_ASSERT_MALLOC(indev);
    if (!indev) {
        return NULL;
    }

    lv_memzero(indev, sizeof(lv_indev_t));
    indev->driver = driver;

    indev->proc.reset_query = 1;
    indev->driver->read_timer = lv_timer_create(lv_indev_read_timer_cb, LV_DEF_REFR_PERIOD, indev);

    return indev;
}

void lv_indev_drv_update(lv_indev_t *indev, lv_indev_drv_t *new_drv)
{
    LV_ASSERT_NULL(indev);
    LV_ASSERT_NULL(indev->driver);
    LV_ASSERT_NULL(indev->driver->read_timer);

    lv_timer_del(indev->driver->read_timer);

    LV_ASSERT_NULL(new_drv);
    if (new_drv->disp == NULL) {
        new_drv->disp = lv_disp_get_default();
    }

    if (new_drv->disp == NULL) {
        LV_LOG_WARN("no display registered hence can't attach the indev to a display");
        indev->proc.disabled = true;
        return;
    }

    indev->driver = new_drv;
    indev->driver->read_timer = lv_timer_create(lv_indev_read_timer_cb, LV_DEF_REFR_PERIOD, indev);
    indev->proc.reset_query = 1;
}

void lv_indev_delete(lv_indev_t *indev)
{
    LV_ASSERT_NULL(indev);
    LV_ASSERT_NULL(indev->driver);
    LV_ASSERT_NULL(indev->driver->read_timer);

    lv_timer_del(indev->driver->read_timer);
    _lv_ll_remove(&LV_GC_ROOT(_lv_indev_ll), indev);

    lv_free(indev);
}

lv_indev_t *lv_indev_get_next(lv_indev_t *indev)
{
    if (indev == NULL) {
        return (lv_indev_t *)_lv_ll_get_head(&LV_GC_ROOT(_lv_indev_ll));
    } else {
        return (lv_indev_t *)_lv_ll_get_next(&LV_GC_ROOT(_lv_indev_ll), indev);
    }
}

void _lv_indev_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    lv_memzero(data, sizeof(lv_indev_data_t));

    // ???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
    if (indev->driver->type == LV_INDEV_TYPE_POINTER) {
        data->point.x = indev->proc.types.pointer.last_raw_point.x;
        data->point.y = indev->proc.types.pointer.last_raw_point.y;
    } else if (indev->driver->type == LV_INDEV_TYPE_KEYPAD) {   // ????????????????????????????????????????????????????????????????????????
        data->key = indev->proc.types.keypad.last_key;
    } else if (indev->driver->type == LV_INDEV_TYPE_ENCODER) {  // ????????????????????????????????????????????????(???????????????)
        data->key = LV_KEY_ENTER;
    }

    if (indev->driver->read_cb) {
        INDEV_TRACE("calling indev_read_cb");
        indev->driver->read_cb(indev->driver, data);
    } else {
        LV_LOG_WARN("indev_read_cb is not registered");
    }
}
