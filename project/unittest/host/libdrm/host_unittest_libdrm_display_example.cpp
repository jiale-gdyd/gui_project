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
#include <libdrm/drm_media_api.h>

#include "private.h"

enum guixCachePingPong {
    CACHE_BUF_PING = 0,
    CACHE_BUF_PONG = 1,
    CACHE_BUF_BUTT
};

static bool g_quit = false;

static int g_dispZpos = 1;
static int g_voChannel = 0;
static size_t u32FrameSize = 0;
static size_t g_dispXoffset = 0;
static size_t g_dispYoffset = 0;
static size_t g_DispWidth = 640;
static size_t g_DispHeight = 480;
static media_buffer_pool_t g_dispBufferPool = NULL;
static drm_plane_type_e g_dispPlaneType = VO_PLANE_OVERLAY;
static drm_image_type_e g_dispImageType = DRM_IMAGE_TYPE_RGB888;

static void sigterm_handler(int signo)
{
    unittest_info("catch signal: %d", signo);
    g_quit = true;
}

int host_unittest_libdrm_display_exit(void)
{
    drm_destroy_video_output(g_voChannel);

    if (g_dispBufferPool != NULL) {
        drm_mpi_mb_pool_destroy(g_dispBufferPool);
    }

    return 0;
}

int host_unittest_libdrm_display_init(int argc, char *argv[])
{
    int ret = drm_mpi_system_init();
    if (ret != 0) {
        unittest_error("drm_mpi_system_init failed");
        return -1;
    }

    ret = drm_create_video_output(g_voChannel, g_dispZpos, g_dispPlaneType, g_DispWidth, g_DispHeight, g_dispXoffset, g_dispYoffset, g_dispImageType, "/dev/dri/card0");
    if (ret != 0) {
        unittest_error("drm_create_video_output failed");
        return -1;
    }

    mb_pool_param_t stBufPoolParam;
    stBufPoolParam.u32Cnt = 3;
    stBufPoolParam.u32Size = 0;
    stBufPoolParam.enMediaType = MB_TYPE_VIDEO;
    stBufPoolParam.bHardWare = true;
    stBufPoolParam.u16Flag = MB_FLAG_NOCACHED;
    stBufPoolParam.stImageInfo.enImgType = g_dispImageType;
    stBufPoolParam.stImageInfo.u32Width = g_DispWidth;
    stBufPoolParam.stImageInfo.u32Height = g_DispHeight;
    stBufPoolParam.stImageInfo.u32HorStride = g_DispWidth;
    stBufPoolParam.stImageInfo.u32VerStride = g_DispHeight;

    g_dispBufferPool = drm_mpi_mb_pool_create(&stBufPoolParam);
    if (!g_dispBufferPool) {
        unittest_error("Couldn't create buffer pool");
        return -1;
    }

    float fltImgRatio = 0.0f;
    if (g_dispImageType == DRM_IMAGE_TYPE_RGB888) {
        fltImgRatio = 3.0;
    } else if (g_dispImageType == DRM_IMAGE_TYPE_ABGR8888) {
        fltImgRatio = 4.0;
    } else if (g_dispImageType == DRM_IMAGE_TYPE_RGB565) {
        fltImgRatio = 2.0;
    }

    u32FrameSize = (size_t)(g_DispWidth * g_DispHeight * fltImgRatio);

    int frameCount = 0;
    bool bSet0xFF = false;
    media_buffer_t frame = NULL;

    signal(SIGINT, sigterm_handler);

    while (!g_quit) {
        if (g_dispBufferPool != NULL) {
            frame = drm_mpi_mb_pool_get_buffer(g_dispBufferPool, true);
            if (frame != NULL) {
                memset(drm_mpi_mb_get_ptr(frame), bSet0xFF ? 0xFF : 0x00, u32FrameSize);
                drm_mpi_mb_set_size(frame, u32FrameSize);

            }

            unittest_info("frame:[%04d] output 0x%02X, frameSize:[%u]", frameCount, bSet0xFF ? 0xFF : 0x00, u32FrameSize);

            bSet0xFF = !bSet0xFF;
            ret = drm_send_frame_video_output(g_voChannel, frame);
            if (ret) {
                break;
            }
        }

        frameCount++;
        sleep(5);
    }

    if (frame) {
        drm_mpi_mb_release_buffer(frame);
    }

    return 0;
}
