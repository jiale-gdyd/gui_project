#include "demo_flex_layout.h"
#include "demo_flex_layout_main.h"

static view_t view;

int lvgl_demo_flex_layout(int argc, char *argv[])
{
    view_create(lv_scr_act(), &view);
    ctrl_pad_attach(&view);
    return 0;
}
