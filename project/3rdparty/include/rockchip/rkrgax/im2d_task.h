#ifndef RKRGA_IM2D_TASK_H
#define RKRGA_IM2D_TASK_H

#include "im2d_type.h"

#ifdef __cplusplus
IM_API im_job_handle_t imbeginJob(uint64_t flags = 0);
IM_API IM_STATUS imcancelJob(im_job_handle_t job_handle);

IM_API IM_STATUS imendJob(im_job_handle_t job_handle, int sync_mode = IM_SYNC, int acquire_fence_fd = 0, int *release_fence_fd = NULL);

IM_API IM_STATUS imcopyTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst);
IM_API IM_STATUS imresizeTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, double fx = 0, double fy = 0, int interpolation = 0);

IM_API IM_STATUS imcropTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, im_rect rect);
IM_API IM_STATUS imtranslateTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, int x, int y);
IM_API IM_STATUS imcvtcolorTask(im_job_handle_t job_handle, rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode = IM_COLOR_SPACE_DEFAULT);

IM_API IM_STATUS imflipTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, int mode);
IM_API IM_STATUS imrotateTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, int rotation);
IM_API IM_STATUS imblendTask(im_job_handle_t job_handle, const rga_buffer_t fg_image, rga_buffer_t bg_image, int mode = IM_ALPHA_BLEND_SRC_OVER);
IM_API IM_STATUS imcompositeTask(im_job_handle_t job_handle, const rga_buffer_t fg_image, const rga_buffer_t bg_image, rga_buffer_t output_image, int mode = IM_ALPHA_BLEND_SRC_OVER);

IM_API IM_STATUS imosdTask(im_job_handle_t job_handle, const rga_buffer_t osd,const rga_buffer_t bg_image, const im_rect osd_rect, im_osd_t *osd_config);
IM_API IM_STATUS imcolorkeyTask(im_job_handle_t job_handle, const rga_buffer_t fg_image, rga_buffer_t bg_image, im_colorkey_range range, int mode = IM_ALPHA_COLORKEY_NORMAL);

IM_API IM_STATUS imropTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, int rop_code);
IM_API IM_STATUS imquantizeTask(im_job_handle_t job_handle, const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info);

IM_API IM_STATUS imfillTask(im_job_handle_t job_handle, rga_buffer_t dst, im_rect rect, uint32_t color);
IM_API IM_STATUS imfillTaskArray(im_job_handle_t job_handle, rga_buffer_t dst, im_rect *rect_array, int array_size, uint32_t color);

IM_API IM_STATUS imrectangleTask(im_job_handle_t job_handle, rga_buffer_t dst, im_rect rect, uint32_t color, int thickness);
IM_API IM_STATUS imrectangleTaskArray(im_job_handle_t job_handle, rga_buffer_t dst, im_rect *rect_array, int array_size, uint32_t color, int thickness);

IM_API IM_STATUS immosaicTask(im_job_handle_t job_handle, const rga_buffer_t image, im_rect rect, int mosaic_mode);
IM_API IM_STATUS immosaicTaskArray(im_job_handle_t job_handle, const rga_buffer_t image, im_rect *rect_array, int array_size, int mosaic_mode);

IM_API IM_STATUS impaletteTask(im_job_handle_t job_handle, rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut);
IM_API IM_STATUS improcessTask(im_job_handle_t job_handle, rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, im_opt_t *opt_ptr, int usage);
#endif

#endif
