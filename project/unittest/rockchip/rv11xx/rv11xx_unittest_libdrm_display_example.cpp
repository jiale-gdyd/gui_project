#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
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
#include <media/drm_media_api.h>

#include "private.h"

#define DISP_WIDTH_PIXELS           720
#define DISP_HEIGHT_PIXELS          720
#define SCREEN_WIDTH_PIXELS         1920
#define SCREEN_HEIGHT_PIXELS        720

enum guixCachePingPong {
    CACHE_BUF_PING = 0,
    CACHE_BUF_PONG = 1,
    CACHE_BUF_BUTT
};

static int g_dispZpos = 1;
static int g_voChannel = 0;
static size_t u32FrameSize = 0;
static size_t g_DispWidth = DISP_WIDTH_PIXELS;
static size_t g_DispHeight = DISP_HEIGHT_PIXELS;
static size_t g_dispXoffset = ((SCREEN_WIDTH_PIXELS - DISP_WIDTH_PIXELS));
static size_t g_dispYoffset = ((SCREEN_HEIGHT_PIXELS - DISP_HEIGHT_PIXELS) / 2);

static bool g_quit = false;
static int g_bufPingPong = CACHE_BUF_PING;
static drm_plane_type_e g_dispPlaneType = VO_PLANE_OVERLAY;
static drm_image_type_e g_dispImageType = DRM_IMAGE_TYPE_RGB888;
static media_buffer_t g_canvasBuffer[CACHE_BUF_BUTT] = {NULL, };

static void sigterm_handler(int signo)
{
    unittest_info("catch signal: %d", signo);
    g_quit = true;
}

int rv11xx_unittest_libdrm_display_exit(void)
{
    drm_destroy_video_output(g_voChannel);

    for (int i = 0; i < CACHE_BUF_BUTT; i++) {
        if (g_canvasBuffer[i]) {
            drm_mpi_mb_release_buffer(g_canvasBuffer[i]);
        }
    }

    return 0;
}

int rv11xx_unittest_libdrm_display_init(int argc, char *argv[])
{
    int ret = drm_mpi_system_init();
    if (ret != 0) {
        unittest_error("drm_mpi_system_init failed");
        return -1;
    }

    ret = drm_create_video_output(g_voChannel, g_dispZpos, g_dispPlaneType, g_DispWidth, g_DispHeight, g_dispXoffset, g_dispYoffset, g_dispImageType, "/dev/dri/card0");
    if (ret != 0) {
        unittest_error("drm_create_video_output failed");
        return -2;
    }

    mb_image_info_t dispImageInfo = { g_DispWidth, g_DispHeight, g_DispWidth, g_DispHeight, g_dispImageType };
    for (int i = 0; i < CACHE_BUF_BUTT; i++) {
        g_canvasBuffer[i] = drm_mpi_mb_create_image_buffer(&dispImageInfo, true, MB_FLAG_NOCACHED);
    }

    signal(SIGINT, sigterm_handler);

    int color = 0x00;
    bool bIncNumeric = true;

    while (!g_quit) {
        if (g_canvasBuffer[g_bufPingPong] != NULL) {
            size_t size = drm_mpi_mb_get_size(g_canvasBuffer[g_bufPingPong]);
            media_buffer_t ptr = drm_mpi_mb_get_ptr(g_canvasBuffer[g_bufPingPong]);

            memset(ptr, color, size);

            ret = drm_send_frame_video_output(g_voChannel, g_canvasBuffer[g_bufPingPong]);
            if (g_bufPingPong == CACHE_BUF_PING) {
                g_bufPingPong = CACHE_BUF_PONG;
            } else {
                g_bufPingPong = CACHE_BUF_PING;
            }

            if (bIncNumeric) {
                color += 0x02;
                if (color >= 0xFF) {
                    color = 0xFF;
                    bIncNumeric = false;
                }
            } else {
                color -= 0x02;
                if (color <= 0x00) {
                    color = 0x00;
                    bIncNumeric = true;
                }
            }
        }

        usleep(50 * 1000);
    }

    return 0;
}
