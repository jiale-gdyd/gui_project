#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>
#include <libdrm/util/common.h>
#include <libdrm/util/buffer.h>
#include <libdrm/util/cursor.h>

#define MAX_CURSORS             8

struct cursor {
    int      fd;
    uint32_t bo_handle;
    uint32_t crtc_id;
    uint32_t crtc_w, crtc_h;
    uint32_t w, h;
    uint32_t enabled, x, y;
    int32_t  dx, dy;
};

struct cursor_step {
    void (*run)(struct cursor *cursor, const struct cursor_step *step);
    uint32_t msec;
    uint32_t repeat;
    int      arg;
};

static int ncursors;
static int cursor_running;
static uint32_t indx, count;
static pthread_t cursor_thread;
static struct cursor cursors[MAX_CURSORS];

static void set_cursor(struct cursor *cursor, const struct cursor_step *step)
{
    uint32_t handle = 0;
    int enabled = (step->arg ^ count) & 0x1;

    if (enabled) {
        handle = cursor->bo_handle;
    }
    cursor->enabled = enabled;

    drmModeSetCursor(cursor->fd, cursor->crtc_id, handle, cursor->w, cursor->h);
}

static void move_cursor(struct cursor *cursor, const struct cursor_step *step)
{
    int x = cursor->x;
    int y = cursor->y;

    if (!cursor->enabled) {
        drmModeSetCursor(cursor->fd, cursor->crtc_id, cursor->bo_handle, cursor->w, cursor->h);
    }

    x += cursor->dx * step->arg;
    y += cursor->dy * step->arg;

    if (x < 0) {
        x = 0;
        cursor->dx = 1;
    } else if (x > (int)cursor->crtc_w) {
        x = cursor->crtc_w - 1;
        cursor->dx = -1;
    }

    if (y < 0) {
        y = 0;
        cursor->dy = 1;
    } else if (y > (int)cursor->crtc_h) {
        y = cursor->crtc_h - 1;
        cursor->dy = -1;
    }

    cursor->x = x;
    cursor->y = y;

    drmModeMoveCursor(cursor->fd, cursor->crtc_id, x, y);
}

static const struct cursor_step steps[] = {
    {  set_cursor, 10,   0,  1 },
    { move_cursor,  1, 100,  1 },
    { move_cursor,  1,  10, 10 },
    {  set_cursor,  1, 100,  0 },
    { move_cursor,  1,  10, 10 },
    { move_cursor,  9, 100,  1 },
    { move_cursor, 11, 100,  5 },
    {  set_cursor, 17,  10,  0 },
    { move_cursor,  9, 100,  1 },
    {  set_cursor, 13,  10,  0 },
    { move_cursor,  9, 100,  1 },
    {  set_cursor, 13,  10,  0 },
    {  set_cursor, 10,   0,  0 },
};

static void *cursor_thread_func(void *data)
{
    while (cursor_running) {
        int i;
        const struct cursor_step *step = &steps[indx % ARRAY_SIZE(steps)];

        for (i = 0; i < ncursors; i++) {
            struct cursor *cursor = &cursors[i];
            step->run(cursor, step);
        }

        if (count < step->repeat) {
            count++;
        } else {
            count = 0;
            indx++;
        }

        usleep(1000 * step->msec);
    }

    return NULL;
}

int cursor_init(int fd, uint32_t bo_handle, uint32_t crtc_id, uint32_t crtc_w, uint32_t crtc_h, uint32_t w, uint32_t h)
{
    struct cursor *cursor = &cursors[ncursors];

    assert(ncursors < MAX_CURSORS);

    cursor->fd = fd;
    cursor->bo_handle = bo_handle;
    cursor->crtc_id = crtc_id;
    cursor->crtc_w = crtc_w;
    cursor->crtc_h = crtc_h;
    cursor->w = w;
    cursor->h = h;

    cursor->enabled = 0;
    cursor->x = w/2;
    cursor->y = h/2;
    cursor->dx = 1;
    cursor->dy = 1;

    ncursors++;

    return 0;
}

int cursor_start(void)
{
    cursor_running = 1;
    pthread_create(&cursor_thread, NULL, cursor_thread_func, NULL);
    printf("starting cursor\n");
    return 0;
}

int cursor_stop(void)
{
    cursor_running = 0;
    pthread_join(cursor_thread, NULL);
    printf("cursor stopped\n");
    return 0;
}
