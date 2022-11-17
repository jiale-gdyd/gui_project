#include <linux/kconfig.h>

#include "base/idle.h"
#include "base/timer.h"
#include "base/font_manager.h"
#include "base/window_manager.h"
#include "main_loop/main_loop_simple.h"
#include "native_window/native_window_raw.h"

#include "input_thread/tslib_thread.h"
#include "input_thread/input_thread.h"
#include "input_thread/mouse_thread.h"
#include "lcd_linux/lcd_linux_fb.h"
#include "lcd_linux/lcd_linux_drm.h"

#include "main_loop_linux.h"

#ifndef FB_DEVICE_FILENAME
#define FB_DEVICE_FILENAME          "/dev/fb0"
#endif

#ifndef DRM_DEVICE_FILENAME
#define DRM_DEVICE_FILENAME         "/dev/dri/card0"
#endif

#ifndef TS_DEVICE_FILENAME
#define TS_DEVICE_FILENAME          "/dev/input/event0"
#endif

#ifndef KB_DEVICE_FILENAME
#define KB_DEVICE_FILENAME          "/dev/input/event1"
#endif

#ifndef MICE_DEVICE_FILENAME
#define MICE_DEVICE_FILENAME        "/dev/input/mouse0"
#endif

static tk_thread_t *s_kb_thread = NULL;
static tk_thread_t *s_ts_thread = NULL;
static tk_thread_t *s_mice_thread = NULL;

static ret_t main_loop_linux_destroy(main_loop_t *l)
{
    main_loop_simple_t *loop = (main_loop_simple_t*)l;

    main_loop_simple_reset(loop);
    native_window_raw_deinit();

    return RET_OK;
}

ret_t input_dispatch_to_main_loop(void *ctx, const event_queue_req_t *evt, const char *msg)
{
    main_loop_simple_t *l = (main_loop_simple_t *)ctx;
    event_queue_req_t event = *evt;
    event_queue_req_t *e = &event;

    if (l != NULL && l->base.queue_event != NULL) {
        switch (e->event.type) {
            case EVT_KEY_DOWN:
            case EVT_KEY_UP:
            case EVT_KEY_LONG_PRESS: {
                e->event.size = sizeof(e->key_event);
                break;
            }

            case EVT_CONTEXT_MENU: {
                e->event.size = sizeof(e->pointer_event);
                break;
            }

            case EVT_POINTER_DOWN: {
                l->pressed = TRUE;
                e->pointer_event.pressed = l->pressed;
                e->event.size = sizeof(e->pointer_event);
                break;
            }

            case EVT_POINTER_MOVE: {
                e->pointer_event.pressed = l->pressed;
                e->event.size = sizeof(e->pointer_event);
                break;
            }

            case EVT_POINTER_UP: {
                e->pointer_event.pressed = l->pressed;
                l->pressed = FALSE;
                e->event.size = sizeof(e->pointer_event);
                break;
            }

            case EVT_WHEEL: {
                e->event.size = sizeof(e->wheel_event);
                break;
            }

            default:
                break;
        }

        main_loop_queue_event(&(l->base), e);
        input_dispatch_print(ctx, e, msg);
    } else {
        return RET_BAD_PARAMS;
    }

    return RET_OK;
}

static void on_app_exit(void)
{
    if (s_kb_thread != NULL) {
        tk_thread_destroy(s_kb_thread);
    }

    if (s_mice_thread != NULL) {
        tk_thread_destroy(s_mice_thread);
    }

    if (s_ts_thread != NULL) {
        tk_thread_destroy(s_ts_thread);
    }

    input_thread_global_deinit();
}

main_loop_t *main_loop_init(int w, int h)
{
    main_loop_simple_t *loop = NULL;
#if defined(CONFIG_DRM_DISP_DRIVER)
    lcd_t *lcd = lcd_linux_drm_create(DRM_DEVICE_FILENAME);
#else
    lcd_t *lcd = lcd_linux_fb_create(FB_DEVICE_FILENAME);
#endif

    return_value_if_fail(lcd != NULL, NULL);

    native_window_raw_init(lcd);

    loop = main_loop_simple_init(lcd->w, lcd->h, NULL, NULL);
    loop->base.destroy = main_loop_linux_destroy;

    input_thread_global_init();

#if defined(CONFIG_TSLIB)
    s_ts_thread = tslib_thread_run(TS_DEVICE_FILENAME, input_dispatch_to_main_loop, loop, lcd->w, lcd->h);
#endif

    s_kb_thread = input_thread_run(KB_DEVICE_FILENAME, input_dispatch_to_main_loop, loop, lcd->w, lcd->h);
    s_mice_thread = mouse_thread_run(MICE_DEVICE_FILENAME, input_dispatch_to_main_loop, loop, lcd->w, lcd->h);

    atexit(on_app_exit);

    return (main_loop_t *)loop;
}
