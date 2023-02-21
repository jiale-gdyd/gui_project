#include "fbdev.h"

#if USE_FBDEV || USE_BSD_FBDEV

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#if USE_BSD_FBDEV
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/consio.h>
#include <sys/fbio.h>
#else
#include <linux/fb.h>
#endif

#ifndef FBDEV_PATH
#define FBDEV_PATH  "/dev/fb0"
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d)                  (((n) + (d) - 1) / (d))
#endif

#ifndef fbdev_print
#define fbdev_print(msg, ...)               fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef fbdev_error
#define fbdev_error(msg, ...)               fbdev_print("\033[1;31m[FBDEV][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef fbdev_info
#define fbdev_info(msg, ...)                fbdev_print("\033[1;32m[FBDEV][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef fbdev_debug
#define fbdev_debug(msg, ...)               {}//fbdev_print("\033[1;34m[FBDEV][D]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

struct bsd_fb_var_info{
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t xres;
    uint32_t yres;
    int      bits_per_pixel;
};

struct bsd_fb_fix_info{
    long int line_length;
    long int smem_len;
};

#if USE_BSD_FBDEV
static struct bsd_fb_var_info vinfo;
static struct bsd_fb_fix_info finfo;
#else
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
#endif

static int fbfd = 0;
static char *fbp = 0;
static long int screensize = 0;

#if USE_BSD_FBDEV
#define FBIOBLANK FBIO_BLANK
#endif

void fbdev_init(void)
{
    fbfd = open(FBDEV_PATH, O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }

    LV_LOG_INFO("The framebuffer device was opened successfully.\n");

    if (ioctl(fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
        perror("ioctl(FBIOBLANK)");
    }

#if USE_BSD_FBDEV
    struct fbtype fb;
    unsigned line_length;

    if (ioctl(fbfd, FBIOGTYPE, &fb) != 0) {
        perror("ioctl(FBIOGTYPE)");
        return;
    }

    if (ioctl(fbfd, FBIO_GETLINEWIDTH, &line_length) != 0) {
        perror("ioctl(FBIO_GETLINEWIDTH)");
        return;
    }

    vinfo.xres = (unsigned) fb.fb_width;
    vinfo.yres = (unsigned) fb.fb_height;
    vinfo.bits_per_pixel = fb.fb_depth;
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    finfo.line_length = line_length;
    finfo.smem_len = finfo.line_length * vinfo.yres;
#else
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        return;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }
#endif

    LV_LOG_INFO("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    screensize =  finfo.smem_len;

    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((intptr_t)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return;
    }

    LV_LOG_INFO("The framebuffer device was mapped to memory successfully.\n");
}

void fbdev_exit(void)
{
    close(fbfd);
}

void fbdev_flush(lv_disp_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    if ((fbp == NULL) || (area->x2 < 0) || (area->y2 < 0) || (area->x1 > ((int32_t)vinfo.xres - 1)) || (area->y1 > ((int32_t)vinfo.yres - 1))) {
        lv_disp_flush_ready(drv);
        return;
    }

    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;

    lv_coord_t w = (act_x2 - act_x1 + 1);
    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;

    if (vinfo.bits_per_pixel == 32) {
        int32_t y;
        uint32_t *fbp32 = (uint32_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;
            memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
            color_p += w;
        }
    } else if ((vinfo.bits_per_pixel == 24) && (LV_COLOR_DEPTH == 32)) {
        int32_t y;
        lv_coord_t x;
        uint8_t *pixel;
        uint8_t *fbp8 = (uint8_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 3;
            for (x = 0; x < w; ++x) {
                pixel = (uint8_t *)(&color_p[x]);
                fbp8[3 * (location + x) + 0] = pixel[0];
                fbp8[3 * (location + x) + 1] = pixel[1];
                fbp8[3 * (location + x) + 2] = pixel[2];
            }

            color_p += w;
        }
    } else if (vinfo.bits_per_pixel == 16) {
        int32_t y;
        uint16_t *fbp16 = (uint16_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 2;
            memcpy(&fbp16[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 2);
            color_p += w;
        }
    } else if (vinfo.bits_per_pixel == 8) {
        int32_t y;
        uint8_t *fbp8 = (uint8_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length;
            memcpy(&fbp8[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1));
            color_p += w;
        }
    } else if (vinfo.bits_per_pixel == 1) {
        int32_t x;
        int32_t y;
        uint8_t *fbp8 = (uint8_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            for (x = act_x1; x <= act_x2; x++) {
                location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * vinfo.xres;
                byte_location = location / 8;
                bit_location = location % 8;
                fbp8[byte_location] &= ~(((uint8_t)(1)) << bit_location);

                fbp8[byte_location] |= ((uint8_t)(color_p->alpha << 24 | color_p->red << 16 | color_p->green << 8 | color_p->blue)) << bit_location;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

    lv_disp_flush_ready(drv);
}

void fbdev_get_sizes(lv_coord_t *width, lv_coord_t *height, lv_coord_t *dpi)
{
    if (width) {
        *width = vinfo.xres;
    }

    if (height) {
        *height = vinfo.yres;
    }

    if (dpi && vinfo.height) {
        *dpi = DIV_ROUND_UP(vinfo.xres * 254, vinfo.width * 10);
    }
}

void fbdev_set_offset(uint32_t xoffset, uint32_t yoffset)
{
    vinfo.xoffset = xoffset;
    vinfo.yoffset = yoffset;
}

#endif
