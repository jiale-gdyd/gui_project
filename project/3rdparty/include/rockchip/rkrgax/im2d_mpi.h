#ifndef RKRGA_IM2D_MPI_H
#define RKRGA_IM2D_MPI_H

#include "im2d_type.h"

IM_EXPORT_API im_ctx_id_t imbegin(uint32_t flags);
IM_EXPORT_API IM_STATUS imcancel(im_ctx_id_t id);

#ifdef __cplusplus
IM_API IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt, int usage, im_ctx_id_t ctx_id);
#endif

IM_EXPORT_API IM_STATUS improcess_ctx(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt, int usage, im_ctx_id_t ctx_id);

#endif
