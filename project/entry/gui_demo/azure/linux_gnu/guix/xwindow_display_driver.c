#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include <gx_api.h>
#include <gx_display.h>

#include "xwindow_display_driver.h"

#ifdef CONFIG_X11_SERVER
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

GC gx_x11_context;
int gx_x11_screen;
Window gx_x11_window;
char *gx_canvas_memory;
Display *gx_x11_display;
XVisualInfo gx_x11_visual;
XImage *gx_x11_image = NULL;
int iEventMask = ExposureMask | StructureNotifyMask | SubstructureNotifyMask | SubstructureRedirectMask;

static void _gx_x11_graphics_driver_setup(GX_DISPLAY *display)
{
    INT iX = 0;
    INT iY = 0;
    XGCValues tValues;
    UINT wBorderWidth = 0;
    GX_RECTANGLE tWinRect;
    INT width = display->gx_display_width;
    INT height = display->gx_display_height;

    tWinRect.gx_rectangle_left = 0;
    tWinRect.gx_rectangle_top = 0;
    tWinRect.gx_rectangle_right = width - 1;
    tWinRect.gx_rectangle_bottom = height - 1;

    XSizeHints tSizeHints;
    char *gx_x11_displayName = NULL;
    const char *pIconName = "GXDevWin";
    const char *pWindowName = "GUIX Development Window";

    Status tXStatus = XInitThreads();
    if (!tXStatus) {
        printf("\nThe X Server does not support threading. Bailing...\n");
        exit(-1);
    }

    // 连接到X服务器
    if ((gx_x11_display = XOpenDisplay(gx_x11_displayName)) == NULL) {
        // 没有连接
        printf("\nFailed to connect to the X server. Bailing...\n");
        exit(-1);
    }

    // 从显示器上获取屏幕
    gx_x11_screen = DefaultScreen(gx_x11_display);

    INT ixviNum;
    XVisualInfo xvi, *xviList;
    XSetWindowAttributes xswa;

    xvi.screen = gx_x11_screen;
    xvi.depth = DefaultDepth(gx_x11_display, gx_x11_screen);
    printf("xvi.depth: %d\n", xvi.depth);

    // xvi.depth = 24;
    // xvi.depth = 32;

    xviList = XGetVisualInfo(gx_x11_display, VisualScreenMask | VisualDepthMask, &xvi, &ixviNum);
    if (!xviList) {
        xvi.depth = DefaultDepth(gx_x11_display, gx_x11_screen);
        xviList = XGetVisualInfo(gx_x11_display, VisualDepthMask, &xvi, &ixviNum);
    }

    memcpy(&xvi, xviList, sizeof(XVisualInfo));
    XFree(xviList);

    xswa.border_pixel = BlackPixel(gx_x11_display, gx_x11_screen);
    xswa.background_pixel = BlackPixel(gx_x11_display, gx_x11_screen);

    gx_x11_window = XCreateWindow(gx_x11_display, RootWindow(gx_x11_display, gx_x11_screen), iX, iY, width, height, wBorderWidth, xvi.depth, CopyFromParent, xvi.visual, CWBorderPixel | CWBackPixel, &xswa);

    // Tell the window manager that we would like to participate in WM_PROTOCOLS messaging

    Atom tAtomDelete = XInternAtom(gx_x11_display, "WM_DELETE_WINDOW", GX_FALSE);
    if (!XSetWMProtocols(gx_x11_display, gx_x11_window, &tAtomDelete, 1)) {
        printf("\ngx_x11_start: Unable to set WM_DELETE_WINDOW procotol with the X server");
    }

    // Setup hints for the wm

    tSizeHints.flags = PSize | PMinSize | PMaxSize;

    // x, y, width and height are obsolete, but we will leave them in for compatibility
    tSizeHints.x = iX;
    tSizeHints.y = iY;
    tSizeHints.width = width;
    tSizeHints.height = height;

    tSizeHints.min_width = width;
    tSizeHints.min_height = height;

    tSizeHints.max_width = width;
    tSizeHints.max_height = height;
 
    tSizeHints.base_width = width;
    tSizeHints.base_height = height;

    // Set the properties for the wm
    XmbSetWMProperties(gx_x11_display, gx_x11_window, pWindowName, pIconName, GX_NULL, 0, &tSizeHints, NULL, NULL);

    // We want to know about these events
    iEventMask |= ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    iEventMask |= KeyPressMask | KeyReleaseMask;
    XSelectInput(gx_x11_display, gx_x11_window, iEventMask);

    gx_allocate_x_canvas(width, height);

    XMapWindow(gx_x11_display, gx_x11_window);

    gx_x11_context = XCreateGC(gx_x11_display, gx_x11_window, 0, &tValues);
    XSetForeground(gx_x11_display, gx_x11_context, WhitePixel(gx_x11_display, gx_x11_screen));
    XSetBackground(gx_x11_display, gx_x11_context, WhitePixel(gx_x11_display, gx_x11_screen));

    XSync(gx_x11_display, 0);
}

static void _gx_copy_canvas_to_buffer_24xrgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    int x, y;
    int width;
    int height;
    GX_COLOR *put;
    GX_COLOR *read;
    GX_COLOR *put_row;
    GX_COLOR *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    /* Calculate dst start ptr*/
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    /* Calculate src start ptr*/
    read_row = (GX_COLOR *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put = put_row;
        read = read_row;
        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            *put++ = *read++;
        }

        put_row += width;
        read_row += width;
    }
}

static void gx_x11_buffer_toggle(struct GX_CANVAS_STRUCT *canvas, GX_RECTANGLE *dirty_area)
{
    int width;
    int height;
    GX_VALUE format;
    GX_RECTANGLE Limit;
    GX_RECTANGLE Copy;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    gx_utility_rectangle_define(&Limit, 0, 0, canvas->gx_canvas_x_resolution - 1, canvas->gx_canvas_y_resolution - 1);
    gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy);

    if (canvas->gx_canvas_display) {
        format = canvas->gx_canvas_display->gx_display_color_format;
    }

    switch (format) {
        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            _gx_copy_canvas_to_buffer_24xrgb(gx_canvas_memory, canvas, &Copy);
            break;

        default:
            break;
    }

    // these lines were just for testing
    // block_fill(0, width, 20, 0xffffff);
    // block_fill(40, width, 20, 0xff0000);
    // block_fill(80, width, 20, 0x00ff00);
    // block_fill(120, width, 20, 0x0000ff);

    XPutImage(gx_x11_display, gx_x11_window, gx_x11_context, gx_x11_image, 0, 0, 0, 0, width, height);
    XFlush(gx_x11_display);
}
#endif

UINT gx_x11_graphics_driver_setup_24xrgb(GX_DISPLAY *display)
{
#ifdef CONFIG_X11_SERVER
    _gx_display_driver_24xrgb_setup(display, GX_NULL, gx_x11_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}
