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

#define DISP_WIDTH_PIXELS               1280
#define DISP_HEIGHT_PIXELS              720

#define SCREEN_WIDTH_PIXELS             1440/* 1920 */
#define SCREEN_HEIGHT_PIXELS            810/* 720 */

#define CAPTURE_CAMERA_WIDTH_PIXELS     1280/* 1920 */
#define CAPTURE_CAMERA_HEIGHT_PIXELS    720/* 1080 */

#define DISP_XOFFSET                    (SCREEN_WIDTH_PIXELS - DISP_WIDTH_PIXELS)
#define DISP_YOFFSET                    ((SCREEN_HEIGHT_PIXELS - DISP_HEIGHT_PIXELS) / 2)

int rv11xx_unittest_camera_libdrm_display_exit(void)
{
    return 0;
}

int rv11xx_unittest_camera_libdrm_display_init(int argc, char *argv[])
{
    int ret = drm_mpi_system_init();
    if (ret != 0) {
        unittest_error("drm_mpi_system_init failed");
        return -1;
    }

    int g_viChn = 0, g_voChn = 1;

    drm_vi_chn_attr_t stViChnAttr;
    memset(&stViChnAttr, 0, sizeof(stViChnAttr));
    stViChnAttr.pcVideoNode = "/dev/video0";
    stViChnAttr.u32BufCnt = 3;
    stViChnAttr.u32Width = CAPTURE_CAMERA_WIDTH_PIXELS;
    stViChnAttr.u32Height = CAPTURE_CAMERA_HEIGHT_PIXELS;
    stViChnAttr.enPixFmt = DRM_IMAGE_TYPE_NV12;
    stViChnAttr.enBufType = DRM_VI_CHN_BUF_TYPE_MMAP;
    stViChnAttr.enWorkMode = DRM_VI_WORK_MODE_NORMAL;

    ret = drm_mpi_vi_set_channel_attribute(g_viChn, &stViChnAttr);
    if (ret != 0) {
        unittest_error("drm_mpi_vi_set_channel_attribute failed, return:[%d]", ret);
        return -2;
    }

    ret = drm_mpi_vi_enable_channel(g_viChn);
    if (ret != 0) {
        unittest_error("drm_mpi_vi_enable_channel failed, return:[%d]", ret);
        return -3;
    }

    drm_vo_chn_attr_t stVoChnAttr;
    memset(&stVoChnAttr, 0, sizeof(stVoChnAttr));
    stVoChnAttr.pcDevNode = "/dev/dri/card0";
    stVoChnAttr.emPlaneType = VO_PLANE_OVERLAY;
    stVoChnAttr.enImgType = DRM_IMAGE_TYPE_NV12;
    stVoChnAttr.u16Zpos = 1;
    stVoChnAttr.stImgRect.s32X = 0;
    stVoChnAttr.stImgRect.s32Y = 0;
    stVoChnAttr.stImgRect.u32Width = CAPTURE_CAMERA_WIDTH_PIXELS;
    stVoChnAttr.stImgRect.u32Height = CAPTURE_CAMERA_HEIGHT_PIXELS;
    stVoChnAttr.stDispRect.s32X = DISP_XOFFSET;
    stVoChnAttr.stDispRect.s32Y = DISP_YOFFSET;
    stVoChnAttr.stDispRect.u32Width = DISP_WIDTH_PIXELS;
    stVoChnAttr.stDispRect.u32Height = DISP_HEIGHT_PIXELS;

    ret = drm_mpi_vo_create_channel(g_voChn, &stVoChnAttr);
    if (ret != 0) {
        unittest_error("drm_mpi_vo_create_channel failed, return:[%d]", ret);
        return -4;
    }

    drm_chn_t stSrcChn, stDstChn;
    stSrcChn.enModId = MOD_ID_VI;
    stSrcChn.s32ChnId = g_viChn;
    stDstChn.enModId = MOD_ID_VO;
    stDstChn.s32ChnId = g_voChn;

    ret = drm_mpi_system_bind(&stSrcChn, &stDstChn);
    if (ret != 0) {
        unittest_error("drm_mpi_system_bind failed, return:[%d]", ret);
        return -5;
    }

    static bool g_quit = false;
    signal(SIGINT, [](int) {
        g_quit = true;
    });

    while (!g_quit) {
        usleep(500000);
    }

    stSrcChn.enModId = MOD_ID_VI;
    stSrcChn.s32ChnId = g_viChn;
    stDstChn.enModId = MOD_ID_VO;
    stDstChn.s32ChnId = g_voChn;

    drm_mpi_system_unbind(&stSrcChn, &stDstChn);
    drm_mpi_vo_destroy_channel(g_voChn);
    drm_mpi_vi_disable_channel(g_viChn);

    return 0;
}
