#ifndef ROCKCHIP_RKRGAX_IM2D_COMMON_H
#define ROCKCHIP_RKRGAX_IM2D_COMMON_H

#include "im2d.h"
#include "drmrga.h"
#include "im2d_hardware.h"

#define ALIGN(val, align)           (((val) + ((align) - 1)) & ~((align) - 1))
#define DOWN_ALIGN(val, align)      ((val) & ~((align) - 1))
#define UNUSED(...)                 (void)(__VA_ARGS__)

#define RGA_GET_API_VERSION(v)      \
    {                               \
        (((v) >> 24) & 0xff),       \
        (((v) >> 16) & 0xff),       \
        (((v) >> 8) & 0xff),        \
        {0}                         \
    }

#define ERR_MSG_LEN                 300

int imSetErrorMsg(const char* format, ...);

bool rga_is_rect_valid(im_rect rect);
bool rga_is_buffer_valid(rga_buffer_t buf);

void empty_structure(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *srect, im_rect *drect, im_rect *prect, im_opt_t *opt);

IM_STATUS rga_set_buffer_info(rga_buffer_t dst, rga_info_t* dstinfo);
IM_STATUS rga_set_buffer_info(const rga_buffer_t src, rga_buffer_t dst, rga_info_t* srcinfo, rga_info_t* dstinfo);

inline void rga_apply_rect(rga_buffer_t *image, im_rect *rect)
{
    if ((rect->width > 0) && (rect->height > 0)) {
        image->width = rect->width;
        image->height = rect->height;
    }
}

IM_STATUS rga_get_info(rga_info_table_entry *return_table);
IM_STATUS rga_check_driver(void);

IM_STATUS rga_check_info(const char *name, const rga_buffer_t info, const im_rect rect, int resolution_usage);
IM_STATUS rga_check_limit(rga_buffer_t src, rga_buffer_t dst, int scale_usage, int mode_usage);
IM_STATUS rga_check_format(const char *name, rga_buffer_t info, im_rect rect, int format_usage, int mode_usgae);
IM_STATUS rga_check_align(const char *name, rga_buffer_t info, int byte_stride);
IM_STATUS rga_check_blend(rga_buffer_t src, rga_buffer_t pat, rga_buffer_t dst, int pat_enable, int mode_usage);
IM_STATUS rga_check_rotate(int mode_usage, rga_info_table_entry &table);
IM_STATUS rga_check_feature(rga_buffer_t src, rga_buffer_t pat, rga_buffer_t dst, int pat_enable, int mode_usage, int feature_usage);

IM_STATUS rga_import_buffers(struct rga_buffer_pool *buffer_pool);
IM_STATUS rga_release_buffers(struct rga_buffer_pool *buffer_pool);

rga_buffer_handle_t rga_import_buffer(uint64_t memory, int type, uint32_t size);
rga_buffer_handle_t rga_import_buffer(uint64_t memory, int type, im_handle_param_t *param);

IM_STATUS rga_release_buffer(int handle);

IM_STATUS rga_get_opt(im_opt_t *opt, void *ptr);

im_ctx_id_t rga_begin_job(uint32_t flags);
IM_STATUS rga_cancel(im_ctx_id_t id);

#endif
