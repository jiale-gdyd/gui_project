#include <awtk.h>
#include <awtk_main.inc>

#include "demo_desktop.h"
#include "../res/assets.inc"

static ret_t on_fullscreen(void *ctx, event_t *e)
{
    widget_t *btn = WIDGET(e->target);
    window_t *win = WINDOW(widget_get_window(btn));

    if (win->fullscreen) {
        window_set_fullscreen(WIDGET(win), FALSE);
        widget_set_text_utf8(btn, "Fullscreen");
    } else {
        window_set_fullscreen(WIDGET(win), TRUE);
        widget_set_text_utf8(btn, "Unfullscreen");
    }

    return RET_OK;
}

static ret_t on_click_enlarge(void *ctx, event_t *e)
{
    window_manager_resize(window_manager(), 840, 800);
    return RET_OK;
}

static ret_t on_click_close(void *ctx, event_t *e)
{
    tk_quit();
    return RET_OK;
}

ret_t awtk_application_init(void)
{
    widget_t *win = window_open("desktop");

    widget_child_on(win, "close", EVT_CLICK, on_click_close, NULL);
    widget_child_on(win, "enlarge", EVT_CLICK, on_click_enlarge, NULL);
    widget_child_on(win, "fullscreen", EVT_CLICK, on_fullscreen, win);

    return RET_OK;
}

ret_t awtk_application_exit(void)
{
    log_debug("application_exit\n");
    return RET_OK;
}

int awtk_demo_desktop_init(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <assets_path>[project/entry/gui_demo/awtk]\n", argv[0]);
        return -1;
    }

    return awtk_application_enter(800, 600, APP_DESKTOP, "awtk_gui", argv[1], "default_full");
}

int awtk_demo_desktop_exit(void)
{
    return 0;
}
