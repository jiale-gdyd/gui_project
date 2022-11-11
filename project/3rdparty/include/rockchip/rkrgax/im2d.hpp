#ifndef ROCKCHIP_RKRGAX_IM2D_HPP
#define ROCKCHIP_RKRGAX_IM2D_HPP

#include "im2d.h"
#include "RgaUtils.h"

rga_buffer_handle_t importbuffer_fd(int fd, int width, int height, int format);
rga_buffer_handle_t importbuffer_virtualaddr(void *va, int width, int height, int format);
rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, int width, int height, int format);

#undef wrapbuffer_handle
rga_buffer_t wrapbuffer_handle(rga_buffer_handle_t handle, int width, int height, int format);
rga_buffer_t wrapbuffer_handle(rga_buffer_handle_t handle, int width, int height, int format, int wstride, int hstride);

#undef imresize
IM_STATUS imresize(const rga_buffer_t src, rga_buffer_t dst, double fx = 0, double fy = 0, int interpolation = 0, int sync = 1, int *release_fence_fd = NULL);

#undef imcrop
IM_STATUS imcrop(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync = 1, int *release_fence_fd = NULL);

#undef imrotate
IM_STATUS imrotate(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync = 1, int *release_fence_fd = NULL);

#undef imflip
IM_STATUS imflip(const rga_buffer_t src, rga_buffer_t dst, int mode, int sync = 1, int *release_fence_fd = NULL);

#undef imfill
IM_STATUS imfill(rga_buffer_t dst, im_rect rect, int color, int sync = 1, int *release_fence_fd = NULL);

#undef impalette
IM_STATUS impalette(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync = 1, int *release_fence_fd = NULL);

#undef imtranslate
IM_STATUS imtranslate(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync = 1, int *release_fence_fd = NULL);

#undef imcopy
IM_STATUS imcopy(const rga_buffer_t src, rga_buffer_t dst, int sync = 1, int *release_fence_fd = NULL);

#undef imblend
IM_STATUS imblend(const rga_buffer_t src, rga_buffer_t dst, int mode = IM_ALPHA_BLEND_SRC_OVER, int sync = 1, int *release_fence_fd = NULL);

#undef imcomposite
IM_STATUS imcomposite(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode = IM_ALPHA_BLEND_SRC_OVER, int sync = 1, int *release_fence_fd = NULL);

#undef imcolorkey
IM_STATUS imcolorkey(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode = IM_ALPHA_COLORKEY_NORMAL, int sync = 1, int *release_fence_fd = NULL);

#undef imcvtcolor
IM_STATUS imcvtcolor(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode = IM_COLOR_SPACE_DEFAULT, int sync = 1, int *release_fence_fd = NULL);

#undef imquantize
IM_STATUS imquantize(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync = 1, int *release_fence_fd = NULL);

#undef imrop
IM_STATUS imrop(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync = 1, int *release_fence_fd = NULL);
IM_STATUS immosaic(const rga_buffer_t image, im_rect rect, int mosaic_mode, int sync = 1, int *release_fence_fd = NULL);
IM_STATUS imosd(const rga_buffer_t osd,const rga_buffer_t dst, const im_rect osd_rect, im_osd_t *osd_config, int sync = 1, int *release_fence_fd = NULL);

IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt_ptr, int usage);
IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt, int usage, im_ctx_id_t ctx_id);

#endif
