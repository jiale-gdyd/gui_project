#ifndef LIBDRM_MEDIA_API_H
#define LIBDRM_MEDIA_API_H

#include "drm_media_vo.h"
#include "drm_media_common.h"
#include "drm_media_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

int drm_mpi_system_init(void);

int drm_mpi_system_send_media_buffer(mod_id_e enModID, int s32ChnId, media_buffer_t buffer);

int drm_mpi_system_set_framerate(mod_id_e enModID, int s32ChnId, drm_fps_attr_t *pstFpsAttr);
int drm_mpi_system_start_recv_frame(mod_id_e enModID, int s32ChnId, const drm_recv_pic_param_t *pstRecvParam);

int drm_mpi_system_register_output_callback(const drm_chn_t *pstChn, OutCallbackFunction callback);
int drm_mpi_system_register_output_callbackEx(const drm_chn_t *pstChn, OutCallbackFunctionEx callback, void *handle);

int drm_mpi_destroy_vo_channel(int channel);
int drm_mpi_create_vo_channel(int channel, const drm_vo_chn_attr_t *pstAttr);

int drm_mpi_get_vo_channel_attribute(int channel, drm_vo_chn_attr_t *pstAttr);
int drm_mpi_set_vo_channel_attribute(int channel, const drm_vo_chn_attr_t *pstAttr);

int drm_destroy_video_output(int channel);
int drm_create_video_output(int channel, int zpos, drm_plane_type_e layer, size_t dispWidth, size_t dispHeight, size_t dispXoffset, size_t dispYoffset, drm_image_type_e imageType, const char *card);

int drm_send_frame_video_output(int channel, media_buffer_t frame);

#ifdef __cplusplus
}
#endif

#endif