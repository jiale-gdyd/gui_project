#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <sys/types.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>
#include <libdrm/drm/drm_fourcc.h>
#include <libdrm/drm_media_api.h>

#include <gx_api.h>
#include <gx_display.h>

#include "drm_display_driver.h"

enum guixCachePingPong {
    CACHE_BUF_PING = 0,
    CACHE_BUF_PONG = 1,
    CACHE_BUF_BUTT
};

static int g_dispVoChannel = 0;
static bool g_driverInied = false;
static int g_bufPingPong = CACHE_BUF_PING;
static media_buffer_t g_canvasBuffer[CACHE_BUF_BUTT] = {NULL, };

static void _gx_copy_canvas_to_buffer_332rgb(void *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
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

static void _gx_copy_canvas_to_buffer_565rgb(void *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
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

static void _gx_copy_canvas_to_buffer_24xrgb(void *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
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

static void _gx_copy_canvas_to_buffer_4444argb(void *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    GX_COLOR *put_ptr, *put_row;
    USHORT *read_ptr, *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源起始地址
    read_row = (USHORT *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            red = ((*read_ptr) & 0x0F00) >> 4;
            red |= red >> 4;

            green = (*read_ptr) & 0xF0;
            green |= green >> 4;

            blue = ((*read_ptr) & 0x0F);
            blue |= blue << 4;

            *put_ptr++ = ASSEMBLECOLOR_32BPP(red, green, blue);
            read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void _gx_copy_canvas_to_buffer_1555xrgb(void *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    GX_COLOR *put_ptr, *put_row;
    USHORT *read_ptr, *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    //  计算源起始地址
    read_row = (USHORT *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            red = ((*read_ptr) & 0x7c00) >> 7;
            if (red & 0x08) {
                red |= 0x07;
            }

            green = ((*read_ptr) & 0x03e0) >> 2;
            if (green & 0x08) {
                green |= 0x03;
            }

            blue = ((*read_ptr) & 0x1f) << 3;
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

static void gx_drm_buffer_toggle(struct GX_CANVAS_STRUCT *canvas, GX_RECTANGLE *dirty_area)
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

    if (canvas->gx_canvas_display) {
        format = canvas->gx_canvas_display->gx_display_color_format;
    }

    void *gx_canvas_memory = drm_mpi_mb_get_ptr(g_canvasBuffer[g_bufPingPong]);
    if (gx_canvas_memory == NULL) {
        return;
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

        case GX_COLOR_FORMAT_4444ARGB:
            _gx_copy_canvas_to_buffer_4444argb(gx_canvas_memory, canvas, &overLapArea);
            break;

        case GX_COLOR_FORMAT_1555XRGB:
            _gx_copy_canvas_to_buffer_1555xrgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        default:
            printf("Unknown color format:[%d]\n", format);
            return;
    }

    // 发送数据帧到VO
    int ret = drm_send_frame_video_output(g_dispVoChannel, g_canvasBuffer[g_bufPingPong]);
    if (g_bufPingPong == CACHE_BUF_PING) {
        g_bufPingPong = CACHE_BUF_PONG;
    } else {
        g_bufPingPong = CACHE_BUF_PING;
    }
}

static void _gx_x11_graphics_driver_setup(GX_DISPLAY *display)
{

}

UINT gx_drm_graphics_driver_setup_24xrgb(GX_DISPLAY *display)
{
    if (!g_driverInied) {
        return GX_FAILURE;
    }

    _gx_display_driver_24xrgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
}

UINT gx_drm_graphics_driver_setup_565rgb(GX_DISPLAY *display)
{
    if (!g_driverInied) {
        return GX_FAILURE;
    }

    _gx_display_driver_565rgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
}

UINT gx_drm_graphics_driver_setup_332rgb(GX_DISPLAY *display)
{
    if (!g_driverInied) {
        return GX_FAILURE;
    }

    _gx_display_driver_332rgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
}

UINT gx_drm_graphics_driver_setup_1555xrgb(GX_DISPLAY *display)
{
    if (!g_driverInied) {
        return GX_FAILURE;
    }

    _gx_display_driver_1555xrgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
}

UINT gx_drm_graphics_driver_setup_4444argb(GX_DISPLAY *display)
{
    if (!g_driverInied) {
        return GX_FAILURE;
    }

    _gx_display_driver_4444argb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
}

int gx_drm_graphics_driver_setup(int channel, size_t width, size_t height, size_t xoffset, size_t yoffset, disp_image_type_e type, disp_plane_type_e dispLayer, int zpos)
{
    int ret = drm_mpi_system_init();
    if (ret != 0) {
        return -1;
    }

    ret = drm_create_video_output(channel, zpos, (drm_plane_type_e)dispLayer, width, height, xoffset, yoffset, (drm_image_type_e)type, "/dev/dri/card0");
    if (ret != 0) {
        return -1;
    }

    g_dispVoChannel = channel;
    g_bufPingPong = CACHE_BUF_PING;

    mb_image_info_t dispImageInfo = { width, height, width, height, (drm_image_type_e)type };
    for (int i = 0; i < CACHE_BUF_BUTT; i++) {
        g_canvasBuffer[i] = drm_mpi_mb_create_image_buffer(&dispImageInfo, true, MB_FLAG_NOCACHED);
        if (g_canvasBuffer[i] != NULL) {
            size_t size = drm_mpi_mb_get_size(g_canvasBuffer[i]);
            media_buffer_t ptr = drm_mpi_mb_get_ptr(g_canvasBuffer[i]);
            memset(ptr, 0x00, size);
        }
    }

    g_driverInied = true;
    return 0;
}

int gx_drm_graphics_driver_exit()
{
    g_driverInied = false;
    for (int i = 0; i < CACHE_BUF_BUTT; i++) {
        if (g_canvasBuffer[i] != NULL) {
            drm_mpi_mb_release_buffer(g_canvasBuffer[i]);
        }
    }

    drm_destroy_video_output(g_dispVoChannel);
    return 0;
}
