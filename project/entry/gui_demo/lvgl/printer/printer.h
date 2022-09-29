#ifndef LVGL_DEMO_PRINTER_H
#define LVGL_DEMO_PRINTER_H

#define LV_USE_DEMO_PRINTER         1

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

LV_IMG_DECLARE(lv_demo_printer_icon_wifi);
LV_IMG_DECLARE(lv_demo_printer_icon_tel);
LV_IMG_DECLARE(lv_demo_printer_icon_eco);
LV_IMG_DECLARE(lv_demo_printer_icon_pc);

LV_IMG_DECLARE(lv_demo_printer_img_usb);
LV_IMG_DECLARE(lv_demo_printer_img_mobile);
LV_IMG_DECLARE(lv_demo_printer_img_internet);
LV_IMG_DECLARE(lv_demo_printer_img_btn_bg_1);
LV_IMG_DECLARE(lv_demo_printer_img_btn_bg_2);
LV_IMG_DECLARE(lv_demo_printer_img_btn_bg_3);
LV_IMG_DECLARE(lv_demo_printer_img_btn_bg_4);

LV_IMG_DECLARE(lv_demo_printer_img_copy);
LV_IMG_DECLARE(lv_demo_printer_img_scan);
LV_IMG_DECLARE(lv_demo_printer_img_print);
LV_IMG_DECLARE(lv_demo_printer_img_setup);

LV_IMG_DECLARE(lv_demo_printer_img_ready);

LV_IMG_DECLARE(lv_demo_printer_img_scan_example);
LV_IMG_DECLARE(lv_demo_printer_icon_bright);
LV_IMG_DECLARE(lv_demo_printer_icon_hue);

LV_IMG_DECLARE(lv_demo_printer_img_printer2);
LV_IMG_DECLARE(lv_demo_printer_img_wave);
LV_IMG_DECLARE(lv_demo_printer_img_phone);

LV_IMG_DECLARE(lv_demo_printer_img_no_internet);
LV_IMG_DECLARE(lv_demo_printer_img_cloud);

int lvgl_demo_printer(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
