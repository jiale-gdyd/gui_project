#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
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

#include <gx_api.h>
#include <gx_display.h>

#include "fbdev_display_driver.h"

#ifndef FBDEV_PATH
#define FBDEV_PATH                      "/dev/fb0"
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d)              (((n) + (d) - 1) / (d))
#endif

#define _FBDEV_CONCAT3(x, y, z)         x ## y ## z
#define FBDEV_CONCAT3(x, y, z)          _FBDEV_CONCAT3(x, y, z)

#define FBDEV_COLOR_DEPTH               32

#ifndef fbdev_print
#define fbdev_print(msg, ...)           fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef fbdev_error
#define fbdev_error(msg, ...)           fbdev_print("[FBDEV][E]: " msg "\n", ##__VA_ARGS__)
#endif

#ifndef fbdev_info
#define fbdev_info(msg, ...)            fbdev_print("[FBDEV][I]: " msg "\n", ##__VA_ARGS__)
#endif

#ifndef fbdev_debug
#define fbdev_debug(msg, ...)           {}
#endif

#if defined(CONFIG_FBDEV_DISP_DRIVER)
struct bsd_fb_var_info {
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t xres;
    uint32_t yres;
    int      bits_per_pixel;
};

struct bsd_fb_fix_info {
    long int line_length;
    long int smem_len;
};

typedef union {
    uint8_t     full;                   // 必须首先声明以通过初始化列表设置字节的所有位
    union {
        uint8_t blue : 1;
        uint8_t green : 1;
        uint8_t red : 1;
    } ch;
} fbdev_color1_t;

typedef union {
    struct {
        uint8_t blue : 2;
        uint8_t green : 3;
        uint8_t red : 3;
    } ch;
    uint8_t     full;
} fbdev_color8_t;

typedef union {
    struct {
        uint16_t blue : 5;
        uint16_t green : 6;
        uint16_t red : 5;
    } ch;
    uint16_t     full;
} fbdev_color16_t;

typedef union {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t    full;
} fbdev_color32_t;

typedef FBDEV_CONCAT3(fbdev_color, FBDEV_COLOR_DEPTH, _t) fbdev_color_t;

static int fbfd = 0;
static char *fbp = 0;
static long int screensize = 0;
static unsigned int g_colorSize = 32;
static char *gx_canvas_memory = NULL;

#if USE_BSD_FBDEV
static struct bsd_fb_var_info vinfo;
static struct bsd_fb_fix_info finfo;
#else
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
#endif

static int fbdev_init(void)
{
    fbfd = open(FBDEV_PATH, O_RDWR);
    if (fbfd == -1) {
        fbdev_error("cannot open framebuffer device, errstr:[%s]", strerror(errno));
        return -1;
    }

    fbdev_info("The framebuffer device was opened successfully");

#if USE_BSD_FBDEV
    struct fbtype fb;
    unsigned line_length;

    // 获取fb类型
    if (ioctl(fbfd, FBIOGTYPE, &fb) != 0) {
        fbdev_error("ioctl(FBIOGTYPE), errstr:[%s]", strerror(errno));
        return -1;
    }

    // 获取屏幕宽度
    if (ioctl(fbfd, FBIO_GETLINEWIDTH, &line_length) != 0) {
        fbdev_error("ioctl(FBIO_GETLINEWIDTH), errstr:[%s]", strerror(errno));
        return -1;
    }

    vinfo.xres = (unsigned)fb.fb_width;
    vinfo.yres = (unsigned)fb.fb_height;
    vinfo.bits_per_pixel = fb.fb_depth + 8;
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    finfo.line_length = line_length;
    finfo.smem_len = finfo.line_length * vinfo.yres;
#else
    // 确保显示器开着
    if (ioctl(fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
        fbdev_error("ioctl(FBIOBLANK) failed, errstr:[%s]", strerror(errno));
        // 不要返回。一些像efifb或simplefb这样的帧缓冲驱动程序不实现FBIOBLANK
    }

    // 获取固定屏幕信息
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        fbdev_error("reading fixed information failed, errstr:[%s]", strerror(errno));
        return -1;
    }

    // 获取可变屏幕信息
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        fbdev_error("reading variable information failed, errstr:[%s]", strerror(errno));
        return -1;
    }
#endif

    fbdev_info("xres:[%d], yres:[%d], bits_per_pixel:[%d]bpp", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // 用字节计算屏幕的大小
    screensize = finfo.smem_len;

    // 将设备映射到内存
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == MAP_FAILED) {
        fbdev_error("failed to map framebuffer device to memory, errstr:[%s]", strerror(errno));
        return -1;
    }

    // 不要初始化内存以保留当前显示的内容/避免清除屏幕。这对于只提取整个帧缓冲区的一部分的应用程序非常重要

    fbdev_info("The framebuffer device was mapped to memory successfully");
    return 0;
}

static void fbdev_exit(void)
{
    munmap(fbp, finfo.smem_len);
    close(fbfd);
}

static void fbdev_get_sizes(uint32_t *width, uint32_t *height, uint32_t *dpi)
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

static void fbdev_set_offset(uint32_t xoffset, uint32_t yoffset)
{
    vinfo.xoffset = xoffset;
    vinfo.yoffset = yoffset;
}

static void fbdev_flush(const char *gx_canvas_memory, struct GX_CANVAS_STRUCT *canvas)
{
    uint32_t width = canvas->gx_canvas_x_resolution;
    uint32_t height = canvas->gx_canvas_y_resolution;
    int y1 = canvas->gx_canvas_dirty_area.gx_rectangle_top;
    int x1 = canvas->gx_canvas_dirty_area.gx_rectangle_left;
    int x2 = canvas->gx_canvas_dirty_area.gx_rectangle_right;
    int y2 = canvas->gx_canvas_dirty_area.gx_rectangle_bottom;

    if ((fbp == NULL) || (x2 < 0) || (y2 < 0) || (x1 > ((uint32_t)vinfo.xres - 1)) || (y1 > ((uint32_t)vinfo.yres - 1))) {
        fbdev_error("Invalid parameter");
        return;
    }

    // 截断屏幕上的区域
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : x2;
    int32_t act_y2 = y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : y2;

    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;
    int16_t w = (act_x2 - act_x1 + 1);

    if (vinfo.bits_per_pixel == 32) {                                   // 每像素32位
        int32_t y;
        uint32_t *fbp32 = (uint32_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;
            memcpy(&fbp32[location], (uint32_t *)gx_canvas_memory, (act_x2 - act_x1 + 1) * 4);
            gx_canvas_memory += w;
        }
        // UINT wHorzQuant = width * sizeof(GX_COLOR);
        // memcpy(fbp, gx_canvas_memory, wHorzQuant * height);
    } else if ((vinfo.bits_per_pixel == 24) && (g_colorSize == 32)) {   // 每像素24位
        int16_t x;
        int32_t y;
        uint8_t *pixel;
        uint8_t *fbp8 = (uint8_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 3;
            for (x = 0; x < w; ++x) {
                pixel = (uint8_t *)(&gx_canvas_memory[x]);
                fbp8[3 * (location + x) + 0] = pixel[0];
                fbp8[3 * (location + x) + 1] = pixel[1];
                fbp8[3 * (location + x) + 2] = pixel[2];
            }

            gx_canvas_memory += w;
        }
    } else if (vinfo.bits_per_pixel == 16) {                            // 每像素16位
        int32_t y;
        uint16_t *fbp16 = (uint16_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 2;
            memcpy(&fbp16[location], (uint32_t *)gx_canvas_memory, (act_x2 - act_x1 + 1) * 2);
            gx_canvas_memory += w;
        }
    } else if (vinfo.bits_per_pixel == 8) {                             // 每像素8位
        int32_t y;
        uint8_t *fbp8 = (uint8_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length;
            memcpy(&fbp8[location], (uint32_t *)gx_canvas_memory, (act_x2 - act_x1 + 1));
            gx_canvas_memory += w;
        }
    } else if (vinfo.bits_per_pixel == 1) {                             // 每像素1位
        int32_t x;
        int32_t y;
        uint8_t *fbp8 = (uint8_t *)fbp;

        for (y = act_y1; y <= act_y2; y++) {
            for (x = act_x1; x <= act_x2; x++) {
                location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * vinfo.xres;
                byte_location = location / 8;       // 找到我们需要更改的字节
                bit_location = location % 8;        // 在找到的字节中，找到我们需要更改的位
                fbp8[byte_location] &= ~(((uint8_t)(1)) << bit_location);
                fbp8[byte_location] |= ((uint8_t)(*gx_canvas_memory)) << bit_location;
                gx_canvas_memory++;
            }

            gx_canvas_memory += x2 - act_x2;
        }
    }

    // 可能需要一些直接的更新命令
    // ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));
}

static void _gx_copy_canvas_to_buffer_332rgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    GX_COLOR *put_ptr, *put_row;
    GX_UBYTE *read_ptr, *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标的起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源的起始地址
    read_row = (GX_UBYTE *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            red = (*read_ptr) & 0xe0;
            if (red & 0x20) {
                red |= 0x1f;
            }

            green = ((*read_ptr) & 0x1c) << 3;
            if (green & 0x20) {
                green |= 0x1f;
            }

            blue = ((*read_ptr) & 0x03) << 6;
            if (blue & 0x40) {
                blue |= 0x3f;
            }

            *put_ptr++ = ASSEMBLECOLOR_32BPP(red, green, blue);
            read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void _gx_copy_canvas_to_buffer_565rgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    USHORT *read_ptr, *read_row;
    GX_COLOR *put_ptr, *put_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标的起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源的起始地址
    read_row = (USHORT *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            red = REDVAL_16BPP(*read_ptr) << 3;
            if (red & 0x08) {
                red |= 0x07;
            }

            green = GREENVAL_16BPP(*read_ptr) << 2;
            if (green & 0x04) {
                green |= 0x03;
            }

            blue = BLUEVAL_16BPP(*read_ptr) << 3;
            if (blue & 0x08) {
                blue |= 0x07;
            }

            *put_ptr++ = ASSEMBLECOLOR_32BPP(red, green, blue);
            read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void _gx_copy_canvas_to_buffer_24xrgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    GX_COLOR *put_ptr, *put_row;
    GX_COLOR *read_ptr, *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源起始地址
    read_row = (GX_COLOR *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            *put_ptr++ = *read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void gx_fbdev_buffer_toggle(struct GX_CANVAS_STRUCT *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE format;
    GX_VALUE width, height;
    GX_RECTANGLE limitRect, overLapArea;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    gx_utility_rectangle_define(&limitRect, 0, 0, width - 1, height - 1);
    gx_utility_rectangle_overlap_detect(&limitRect, &canvas->gx_canvas_dirty_area, &overLapArea);

    if (canvas->gx_canvas_display) {
        format = canvas->gx_canvas_display->gx_display_color_format;
    }

    switch (format) {
        case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
            _gx_copy_canvas_to_buffer_332rgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        case GX_COLOR_FORMAT_565RGB:
            _gx_copy_canvas_to_buffer_565rgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            _gx_copy_canvas_to_buffer_24xrgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        default:
            printf("[FBDEV] Unknown color format:[%d]\n", format);
            return;
    }

    // 刷新缓存到FBDEV
    fbdev_flush(gx_canvas_memory, canvas);
}

static GX_BOOL _gx_allocate_x_canvas(GX_VALUE width, GX_VALUE height)
{
    if ((width > GX_MAX_DISPLAY_RESOLUTION) || (height > GX_MAX_DISPLAY_RESOLUTION)) {
        printf("[FBDEV] Invalid canvas size\n");
        return GX_FALSE;
    }

    UINT wHorzQuant = width * sizeof(GX_COLOR);
    gx_canvas_memory = (char *)malloc(wHorzQuant * height);
    if (!gx_canvas_memory) {
        printf("[FBDEV] Failed to allocate memory\n");
        return GX_FALSE;
    }

    return GX_TRUE;
}

static void _gx_x11_graphics_driver_setup(GX_DISPLAY *display)
{
    GX_VALUE width, height;

    width = display->gx_display_width;
    height = display->gx_display_height;

    _gx_allocate_x_canvas(width, height);
}
#endif

UINT gx_fbdev_graphics_driver_setup_24xrgb(GX_DISPLAY *display)
{
#if defined(CONFIG_FBDEV_DISP_DRIVER)
    static int fbdev_init_flags = 0;

    if (fbdev_init_flags == 0) {
        g_colorSize = 32;
        int ret = fbdev_init();
        if (ret == 0) {
            fbdev_init_flags = 1;
        }
    }

    _gx_display_driver_24xrgb_setup(display, GX_NULL, gx_fbdev_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}

UINT gx_fbdev_graphics_driver_setup_565rgb(GX_DISPLAY *display)
{
#if defined(CONFIG_FBDEV_DISP_DRIVER)
    static int fbdev_init_flags = 0;

    if (fbdev_init_flags == 0) {
        g_colorSize = 16;
        int ret = fbdev_init();
        if (ret == 0) {
            fbdev_init_flags = 1;
        }
    }

    _gx_display_driver_565rgb_setup(display, GX_NULL, gx_fbdev_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}

UINT gx_fbdev_graphics_driver_setup_332rgb(GX_DISPLAY *display)
{
#if defined(CONFIG_FBDEV_DISP_DRIVER)
    static int fbdev_init_flags = 0;

    if (fbdev_init_flags == 0) {
        g_colorSize = 8;
        int ret = fbdev_init();
        if (ret == 0) {
            fbdev_init_flags = 1;
        }
    }

    _gx_display_driver_332rgb_setup(display, GX_NULL, gx_fbdev_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}
