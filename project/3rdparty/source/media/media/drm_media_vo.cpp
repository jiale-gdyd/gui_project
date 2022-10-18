#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <media/utils/utils.h>
#include <media/drm_media_api.h>

#ifdef __cplusplus
extern "C" {
#endif

static bool g_voChnStated[DRM_VO_CHANNEL_BUTT] = {false, };

int drm_destroy_video_output(int channel)
{
    if ((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT)) {
        DRM_MEDIA_LOGE("vo channel:[%d] invalid , only:[%d, %d)", channel, DRM_VO_CHANNEL_00, DRM_VO_CHANNEL_BUTT);
        return -1;
    }

    if (g_voChnStated[channel] == false) {
        DRM_MEDIA_LOGE("vo channel:[%d] had destroy", channel);
        return 0;
    }

    int ret = drm_mpi_vo_destroy_channel(channel);
    g_voChnStated[channel] = ret == 0 ? false : true;

    return ret;
}

int drm_create_video_output(int channel, int zpos, drm_plane_type_e layer, size_t dispWidth, size_t dispHeight, size_t dispXoffset, size_t dispYoffset, drm_image_type_e imageType, const char *card)
{
    if ((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT)) {
        DRM_MEDIA_LOGE("vo channel:[%d] invalid , only:[%d, %d)", channel, DRM_VO_CHANNEL_00, DRM_VO_CHANNEL_BUTT);
        return -1;
    }

    if (g_voChnStated[channel]) {
        DRM_MEDIA_LOGE("vo channel:[%d] had started", channel);
        return 0;
    }

    int ret = 0;
    drm_vo_chn_attr_t stVoAttr;

    memset(&stVoAttr, 0x00, sizeof(stVoAttr));
    stVoAttr.u16Zpos = zpos;
    stVoAttr.pcDevNode = card;
    stVoAttr.enImgType = imageType;
    stVoAttr.emPlaneType = layer;
    stVoAttr.stImgRect.s32X = 0;
    stVoAttr.stImgRect.s32Y = 0;
    stVoAttr.stImgRect.u32Width = dispWidth;
    stVoAttr.stImgRect.u32Height = dispHeight;
    stVoAttr.stDispRect.s32X = dispXoffset;
    stVoAttr.stDispRect.s32Y = dispYoffset;
    stVoAttr.stDispRect.u32Width = dispWidth;
    stVoAttr.stDispRect.u32Height = dispHeight;

    ret = drm_mpi_vo_create_channel(channel, (const drm_vo_chn_attr_t *)&stVoAttr);
    if (ret) {
        DRM_MEDIA_LOGE("create vo channel:[%d] failed, return:[%d]", channel, ret);
        return ret;
    }

    g_voChnStated[channel] = true;
    return 0;
}

int drm_send_frame_video_output(int channel, media_buffer_t frame)
{
    if ((channel < DRM_VO_CHANNEL_00) || (channel >= DRM_VO_CHANNEL_BUTT)) {
        DRM_MEDIA_LOGE("vo channel:[%d] invalid , only:[%d, %d)", channel, DRM_VO_CHANNEL_00, DRM_VO_CHANNEL_BUTT);
        return -1;
    }

    if (g_voChnStated[channel] == false) {
        DRM_MEDIA_LOGE("vo channel:[%d] not start", channel);
        return -2;
    }

    return drm_mpi_system_send_media_buffer(MOD_ID_VO, channel, frame);
}

#ifdef __cplusplus
}
#endif
