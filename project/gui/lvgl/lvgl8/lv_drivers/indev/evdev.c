#include "evdev.h"

#if (USE_EVDEV != 0) || USE_BSD_EVDEV

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#if USE_BSD_EVDEV
#include <dev/evdev/input.h>
#else
#include <linux/input.h>
#endif

#if USE_XKB
#include "xkb.h"
#endif

int evdev_root_x;
int evdev_root_y;
int evdev_button;
int evdev_key_val;
int evdev_fd = -1;

int map(int x, int in_min, int in_max, int out_min, int out_max);

void evdev_init(void)
{
    if (!evdev_set_file(EVDEV_NAME)) {
        return;
    }

#if USE_XKB
    xkb_init();
#endif
}

bool evdev_set_file(char *dev_name)
{ 
    if (evdev_fd != -1) {
        close(evdev_fd);
    }

#if USE_BSD_EVDEV
    evdev_fd = open(dev_name, O_RDWR | O_NOCTTY);
#else
    evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
#endif

    if(evdev_fd == -1) {
        perror("unable to open evdev interface:");
        return false;
    }

#if USE_BSD_EVDEV
    fcntl(evdev_fd, F_SETFL, O_NONBLOCK);
#else
    fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);
#endif

    evdev_root_x = 0;
    evdev_root_y = 0;
    evdev_key_val = 0;
    evdev_button = LV_INDEV_STATE_REL;

    return true;
}

void evdev_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    struct input_event in;

    while (read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
        if (in.type == EV_REL) {
            if (in.code == REL_X) {
#if EVDEV_SWAP_AXES
                evdev_root_y += in.value;
#else
                evdev_root_x += in.value;
#endif
            } else if (in.code == REL_Y) {
#if EVDEV_SWAP_AXES
                evdev_root_x += in.value;
#else
                evdev_root_y += in.value;
#endif
            }
        } else if (in.type == EV_ABS) {
            if (in.code == ABS_X) {
#if EVDEV_SWAP_AXES
                evdev_root_y = in.value;
#else
                evdev_root_x = in.value;
#endif
            } else if (in.code == ABS_Y) {
#if EVDEV_SWAP_AXES
                evdev_root_x = in.value;
#else
                evdev_root_y = in.value;
#endif
            } else if (in.code == ABS_MT_POSITION_X) {
#if EVDEV_SWAP_AXES
                evdev_root_y = in.value;
#else
                evdev_root_x = in.value;
#endif
            } else if (in.code == ABS_MT_POSITION_Y) {
#if EVDEV_SWAP_AXES
                evdev_root_x = in.value;
#else
                evdev_root_y = in.value;
#endif
            } else if (in.code == ABS_MT_TRACKING_ID) {
                if (in.value == -1) {
                    evdev_button = LV_INDEV_STATE_REL;
                } else if (in.value == 0) {
                    evdev_button = LV_INDEV_STATE_PR;
                }
            }
        } else if (in.type == EV_KEY) {
            if ((in.code == BTN_MOUSE) || (in.code == BTN_TOUCH)) {
                if (in.value == 0) {
                    evdev_button = LV_INDEV_STATE_REL;
                } else if (in.value == 1) {
                    evdev_button = LV_INDEV_STATE_PR;
                }
            } else if (drv->type == LV_INDEV_TYPE_KEYPAD) {
#if USE_XKB
                data->key = xkb_process_key(in.code, in.value != 0);
#else
                switch (in.code) {
                    case KEY_BACKSPACE:
                        data->key = LV_KEY_BACKSPACE;
                        break;

                    case KEY_ENTER:
                        data->key = LV_KEY_ENTER;
                        break;

                    case KEY_PREVIOUS:
                        data->key = LV_KEY_PREV;
                        break;

                    case KEY_NEXT:
                        data->key = LV_KEY_NEXT;
                        break;

                    case KEY_UP:
                        data->key = LV_KEY_UP;
                        break;

                    case KEY_LEFT:
                        data->key = LV_KEY_LEFT;
                        break;

                    case KEY_RIGHT:
                        data->key = LV_KEY_RIGHT;
                        break;

                    case KEY_DOWN:
                        data->key = LV_KEY_DOWN;
                        break;

                    case KEY_TAB:
                        data->key = LV_KEY_NEXT;
                        break;

                    default:
                        data->key = 0;
                        break;
                }
#endif
                if (data->key != 0) {
                    data->state = (in.value) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
                }
    
                evdev_key_val = data->key;
                evdev_button = data->state;
                return;
            }
        }
    }

    if (drv->type == LV_INDEV_TYPE_KEYPAD) {
        data->key = evdev_key_val;
        data->state = (lv_indev_state_t)evdev_button;
        return;
    }

    if (drv->type != LV_INDEV_TYPE_POINTER) {
        return;
    }

#if EVDEV_CALIBRATE
    data->point.x = map(evdev_root_x, EVDEV_HOR_MIN, EVDEV_HOR_MAX, 0, drv->disp->driver.hor_res);
    data->point.y = map(evdev_root_y, EVDEV_VER_MIN, EVDEV_VER_MAX, 0, drv->disp->driver.ver_res);
#else
    data->point.x = evdev_root_x;
    data->point.y = evdev_root_y;
#endif

    data->state = (lv_indev_state_t)evdev_button;

    if (data->point.x < 0) {
        data->point.x = 0;
    }

    if (data->point.y < 0) {
        data->point.y = 0;
    }

    if (data->point.x >= drv->disp->driver->hor_res) {
        data->point.x = drv->disp->driver->hor_res - 1;
    }

    if (data->point.y >= drv->disp->driver->ver_res) {
        data->point.y = drv->disp->driver->ver_res - 1;
    }
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
