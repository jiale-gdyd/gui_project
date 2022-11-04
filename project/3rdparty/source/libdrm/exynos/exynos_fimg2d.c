#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <linux/stddef.h>

#include <libdrm/xf86drm.h>
#include <libdrm/libdrm_macros.h>
#include <libdrm/drm/exynos_drm.h>
#include <libdrm/drm/exynos_fimg2d.h>

#include "fimg2d_reg.h"

#define SET_BF(val, sc, si, scsa, scda, dc, di, dcsa, dcda) \
            val.data.src_coeff = sc;                        \
            val.data.inv_src_color_coeff = si;              \
            val.data.src_coeff_src_a = scsa;                \
            val.data.src_coeff_dst_a = scda;                \
            val.data.dst_coeff = dc;                        \
            val.data.inv_dst_color_coeff = di;              \
            val.data.dst_coeff_src_a = dcsa;                \
            val.data.dst_coeff_dst_a = dcda;

#define MIN(a, b)                   ((a) < (b) ? (a) : (b))

#define MSG_PREFIX                  "exynos/fimg2d: "

#define G2D_MAX_CMD_NR              64
#define G2D_MAX_GEM_CMD_NR          64
#define G2D_MAX_CMD_LIST_NR         64

struct g2d_context {
    int                       fd;
    unsigned int              major;
    unsigned int              minor;
    struct drm_exynos_g2d_cmd cmd[G2D_MAX_CMD_NR];
    struct drm_exynos_g2d_cmd cmd_buf[G2D_MAX_GEM_CMD_NR];
    unsigned int              cmd_nr;
    unsigned int              cmd_buf_nr;
    unsigned int              cmdlist_nr;
    void                      *event_userdata;
};

enum g2d_base_addr_reg {
    g2d_dst = 0,
    g2d_src
};

enum e_g2d_dir_mode {
    G2D_DIR_MODE_POSITIVE = 0,
    G2D_DIR_MODE_NEGATIVE = 1
};

union g2d_direction_val {
    unsigned int            val[2];
    struct {
        enum e_g2d_dir_mode src_x_direction:1;
        enum e_g2d_dir_mode src_y_direction:1;

        unsigned int        reversed1:2;

        enum e_g2d_dir_mode mask_x_direction:1;
        enum e_g2d_dir_mode mask_y_direction:1;

        unsigned int        padding1:26;

        enum e_g2d_dir_mode dst_x_direction:1;
        enum e_g2d_dir_mode dst_y_direction:1;

        unsigned int        reversed2:2;

        enum e_g2d_dir_mode pat_x_direction:1;
        enum e_g2d_dir_mode pat_y_direction:1;

        unsigned int        padding2:26;
    } data;
};

static unsigned int g2d_get_scaling(unsigned int src, unsigned int dst)
{
    return ((src << 16) / dst);
}

static unsigned int g2d_get_blend_op(enum e_g2d_op op)
{
    union g2d_blend_func_val val;

    val.val = 0;

    switch (op) {
        case G2D_OP_CLEAR:
        case G2D_OP_DISJOINT_CLEAR:
        case G2D_OP_CONJOINT_CLEAR:
            SET_BF(val, G2D_COEFF_MODE_ZERO, 0, 0, 0, G2D_COEFF_MODE_ZERO, 0, 0, 0);
            break;

        case G2D_OP_SRC:
        case G2D_OP_DISJOINT_SRC:
        case G2D_OP_CONJOINT_SRC:
            SET_BF(val, G2D_COEFF_MODE_ONE, 0, 0, 0, G2D_COEFF_MODE_ZERO, 0, 0, 0);
            break;

        case G2D_OP_DST:
        case G2D_OP_DISJOINT_DST:
        case G2D_OP_CONJOINT_DST:
            SET_BF(val, G2D_COEFF_MODE_ZERO, 0, 0, 0, G2D_COEFF_MODE_ONE, 0, 0, 0);
            break;

        case G2D_OP_OVER:
            SET_BF(val, G2D_COEFF_MODE_ONE, 0, 0, 0, G2D_COEFF_MODE_SRC_ALPHA, 1, 0, 0);
            break;

        case G2D_OP_INTERPOLATE:
            SET_BF(val, G2D_COEFF_MODE_SRC_ALPHA, 0, 0, 0, G2D_COEFF_MODE_SRC_ALPHA, 1, 0, 0);
            break;
    }

    return val.val;
}

static unsigned int g2d_check_space(const struct g2d_context *ctx, unsigned int num_cmds, unsigned int num_gem_cmds)
{
    if (((ctx->cmd_nr + num_cmds) >= G2D_MAX_CMD_NR) || ((ctx->cmd_buf_nr + num_gem_cmds) >= G2D_MAX_GEM_CMD_NR)) {
        return 1;
    } else {
        return 0;
    }
}

static int g2d_validate_select_mode(enum e_g2d_select_mode mode)
{
    switch (mode) {
        case G2D_SELECT_MODE_NORMAL:
        case G2D_SELECT_MODE_FGCOLOR:
        case G2D_SELECT_MODE_BGCOLOR:
            return 1;
    }

    return 0;
}

static int g2d_validate_blending_op(enum e_g2d_op operation)
{
    switch (operation) {
        case G2D_OP_CLEAR:
        case G2D_OP_SRC:
        case G2D_OP_DST:
        case G2D_OP_OVER:
        case G2D_OP_INTERPOLATE:
        case G2D_OP_DISJOINT_CLEAR:
        case G2D_OP_DISJOINT_SRC:
        case G2D_OP_DISJOINT_DST:
        case G2D_OP_CONJOINT_CLEAR:
        case G2D_OP_CONJOINT_SRC:
        case G2D_OP_CONJOINT_DST:
            return 1;
    }

    return 0;
}

static void g2d_add_cmd(struct g2d_context *ctx, unsigned long cmd, unsigned long value)
{
    switch (cmd & ~(G2D_BUF_USERPTR)) {
        case SRC_BASE_ADDR_REG:
        case SRC_PLANE2_BASE_ADDR_REG:
        case DST_BASE_ADDR_REG:
        case DST_PLANE2_BASE_ADDR_REG:
        case PAT_BASE_ADDR_REG:
        case MASK_BASE_ADDR_REG:
            assert(ctx->cmd_buf_nr < G2D_MAX_GEM_CMD_NR);

            ctx->cmd_buf[ctx->cmd_buf_nr].offset = cmd;
            ctx->cmd_buf[ctx->cmd_buf_nr].data = value;
            ctx->cmd_buf_nr++;
            break;

        default:
            assert(ctx->cmd_nr < G2D_MAX_CMD_NR);

            ctx->cmd[ctx->cmd_nr].offset = cmd;
            ctx->cmd[ctx->cmd_nr].data = value;
            ctx->cmd_nr++;
            break;
    }
}

static void g2d_add_base_addr(struct g2d_context *ctx, struct g2d_image *img, enum g2d_base_addr_reg reg)
{
    const unsigned long cmd = (reg == g2d_dst) ? DST_BASE_ADDR_REG : SRC_BASE_ADDR_REG;

    if (img->buf_type == G2D_IMGBUF_USERPTR) {
        g2d_add_cmd(ctx, cmd | G2D_BUF_USERPTR, (unsigned long)&img->user_ptr[0]);
    } else {
        g2d_add_cmd(ctx, cmd, img->bo[0]);
    }
}

static void g2d_set_direction(struct g2d_context *ctx, const union g2d_direction_val *dir)
{
    g2d_add_cmd(ctx, SRC_MASK_DIRECT_REG, dir->val[0]);
    g2d_add_cmd(ctx, DST_PAT_DIRECT_REG, dir->val[1]);
}

static int g2d_flush(struct g2d_context *ctx)
{
    int ret;
    struct drm_exynos_g2d_set_cmdlist cmdlist = {0};

    if ((ctx->cmd_nr == 0) && (ctx->cmd_buf_nr == 0)) {
        return 0;
    }

    if (ctx->cmdlist_nr >= G2D_MAX_CMD_LIST_NR) {
        fprintf(stderr, MSG_PREFIX "command list overflow.\n");
        return -EINVAL;
    }

    cmdlist.cmd = (uint64_t)(uintptr_t)&ctx->cmd[0];
    cmdlist.cmd_buf = (uint64_t)(uintptr_t)&ctx->cmd_buf[0];
    cmdlist.cmd_nr = ctx->cmd_nr;
    cmdlist.cmd_buf_nr = ctx->cmd_buf_nr;

    if (ctx->event_userdata) {
        cmdlist.event_type = G2D_EVENT_NONSTOP;
        cmdlist.user_data = (uint64_t)(uintptr_t)(ctx->event_userdata);
        ctx->event_userdata = NULL;
    } else {
        cmdlist.event_type = G2D_EVENT_NOT;
        cmdlist.user_data = 0;
    }

    ctx->cmd_nr = 0;
    ctx->cmd_buf_nr = 0;

    ret = drmIoctl(ctx->fd, DRM_IOCTL_EXYNOS_G2D_SET_CMDLIST, &cmdlist);
    if (ret < 0) {
        fprintf(stderr, MSG_PREFIX "failed to set cmdlist.\n");
        return ret;
    }

    ctx->cmdlist_nr++;
    return ret;
}

drm_public struct g2d_context *g2d_init(int fd)
{
    int ret;
    struct g2d_context *ctx;
    struct drm_exynos_g2d_get_ver ver;

    ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        fprintf(stderr, MSG_PREFIX "failed to allocate context.\n");
        return NULL;
    }

    ctx->fd = fd;

    ret = drmIoctl(fd, DRM_IOCTL_EXYNOS_G2D_GET_VER, &ver);
    if (ret < 0) {
        fprintf(stderr, MSG_PREFIX "failed to get version.\n");
        free(ctx);
        return NULL;
    }

    ctx->major = ver.major;
    ctx->minor = ver.minor;

    printf(MSG_PREFIX "G2D version (%d.%d).\n", ctx->major, ctx->minor);
    return ctx;
}

drm_public void g2d_fini(struct g2d_context *ctx)
{
    free(ctx);
}

drm_public void g2d_config_event(struct g2d_context *ctx, void *userdata)
{
    ctx->event_userdata = userdata;
}

drm_public int g2d_exec(struct g2d_context *ctx)
{
    int ret;
    struct drm_exynos_g2d_exec exec;

    if (ctx->cmdlist_nr == 0) {
        return -EINVAL;
    }

    exec.async = 0;

    ret = drmIoctl(ctx->fd, DRM_IOCTL_EXYNOS_G2D_EXEC, &exec);
    if (ret < 0) {
        fprintf(stderr, MSG_PREFIX "failed to execute.\n");
        return ret;
    }

    ctx->cmdlist_nr = 0;
    return ret;
}

drm_public int g2d_solid_fill(struct g2d_context *ctx, struct g2d_image *img, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    union g2d_point_val pt;
    union g2d_bitblt_cmd_val bitblt;

    if (g2d_check_space(ctx, 7, 1)) {
        return -ENOSPC;
    }

    g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_NORMAL);
    g2d_add_cmd(ctx, DST_COLOR_MODE_REG, img->color_mode);
    g2d_add_base_addr(ctx, img, g2d_dst);
    g2d_add_cmd(ctx, DST_STRIDE_REG, img->stride);

    if ((x + w) > img->width) {
        w = img->width - x;
    }

    if ((y + h) > img->height) {
        h = img->height - y;
    }

    pt.data.x = x;
    pt.data.y = y;
    g2d_add_cmd(ctx, DST_LEFT_TOP_REG, pt.val);

    pt.data.x = x + w;
    pt.data.y = y + h;
    g2d_add_cmd(ctx, DST_RIGHT_BOTTOM_REG, pt.val);

    g2d_add_cmd(ctx, SF_COLOR_REG, img->color);

    bitblt.val = 0;
    bitblt.data.fast_solid_color_fill_en = 1;
    g2d_add_cmd(ctx, BITBLT_COMMAND_REG, bitblt.val);

    return g2d_flush(ctx);
}

drm_public int g2d_copy(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int dst_x, unsigned dst_y, unsigned int w, unsigned int h)
{
    union g2d_point_val pt;
    union g2d_rop4_val rop4;
    unsigned int src_w, src_h, dst_w, dst_h;

    src_w = w;
    src_h = h;

    if ((src_x + src->width) > w) {
        src_w = src->width - src_x;
    }

    if ((src_y + src->height) > h) {
        src_h = src->height - src_y;
    }

    dst_w = w;
    dst_h = w;

    if ((dst_x + dst->width) > w) {
        dst_w = dst->width - dst_x;
    }

    if ((dst_y + dst->height) > h) {
        dst_h = dst->height - dst_y;
    }

    w = MIN(src_w, dst_w);
    h = MIN(src_h, dst_h);

    if ((w <= 0) || (h <= 0)) {
        fprintf(stderr, MSG_PREFIX "invalid width or height.\n");
        return -EINVAL;
    }

    if (g2d_check_space(ctx, 11, 2)) {
        return -ENOSPC;
    }

    g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_BGCOLOR);
    g2d_add_cmd(ctx, DST_COLOR_MODE_REG, dst->color_mode);
    g2d_add_base_addr(ctx, dst, g2d_dst);
    g2d_add_cmd(ctx, DST_STRIDE_REG, dst->stride);

    g2d_add_cmd(ctx, SRC_SELECT_REG, G2D_SELECT_MODE_NORMAL);
    g2d_add_cmd(ctx, SRC_COLOR_MODE_REG, src->color_mode);
    g2d_add_base_addr(ctx, src, g2d_src);
    g2d_add_cmd(ctx, SRC_STRIDE_REG, src->stride);

    pt.data.x = src_x;
    pt.data.y = src_y;
    g2d_add_cmd(ctx, SRC_LEFT_TOP_REG, pt.val);
    pt.data.x = src_x + w;
    pt.data.y = src_y + h;
    g2d_add_cmd(ctx, SRC_RIGHT_BOTTOM_REG, pt.val);

    pt.data.x = dst_x;
    pt.data.y = dst_y;
    g2d_add_cmd(ctx, DST_LEFT_TOP_REG, pt.val);
    pt.data.x = dst_x + w;
    pt.data.y = dst_y + h;
    g2d_add_cmd(ctx, DST_RIGHT_BOTTOM_REG, pt.val);

    rop4.val = 0;
    rop4.data.unmasked_rop3 = G2D_ROP3_SRC;
    g2d_add_cmd(ctx, ROP4_REG, rop4.val);

    return g2d_flush(ctx);
}

drm_public int g2d_move(struct g2d_context *ctx, struct g2d_image *img, unsigned int src_x, unsigned int src_y, unsigned int dst_x, unsigned dst_y, unsigned int w, unsigned int h)
{
    union g2d_point_val pt;
    union g2d_rop4_val rop4;
    union g2d_direction_val dir;
    unsigned int src_w, src_h, dst_w, dst_h;

    src_w = w;
    src_h = h;

    if ((src_x + img->width) > w) {
        src_w = img->width - src_x;
    }

    if ((src_y + img->height) > h) {
        src_h = img->height - src_y;
    }

    dst_w = w;
    dst_h = w;

    if ((dst_x + img->width) > w) {
        dst_w = img->width - dst_x;
    }

    if ((dst_y + img->height) > h) {
        dst_h = img->height - dst_y;
    }

    w = MIN(src_w, dst_w);
    h = MIN(src_h, dst_h);

    if ((w == 0) || (h == 0)) {
        fprintf(stderr, MSG_PREFIX "invalid width or height.\n");
        return -EINVAL;
    }

    if (g2d_check_space(ctx, 13, 2)) {
        return -ENOSPC;
    }

    g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_BGCOLOR);
    g2d_add_cmd(ctx, SRC_SELECT_REG, G2D_SELECT_MODE_NORMAL);

    g2d_add_cmd(ctx, DST_COLOR_MODE_REG, img->color_mode);
    g2d_add_cmd(ctx, SRC_COLOR_MODE_REG, img->color_mode);

    g2d_add_base_addr(ctx, img, g2d_dst);
    g2d_add_base_addr(ctx, img, g2d_src);

    g2d_add_cmd(ctx, DST_STRIDE_REG, img->stride);
    g2d_add_cmd(ctx, SRC_STRIDE_REG, img->stride);

    dir.val[0] = dir.val[1] = 0;

    if (dst_x >= src_x) {
        dir.data.src_x_direction = dir.data.dst_x_direction = 1;
    }

    if (dst_y >= src_y) {
        dir.data.src_y_direction = dir.data.dst_y_direction = 1;
    }

    g2d_set_direction(ctx, &dir);

    pt.data.x = src_x;
    pt.data.y = src_y;
    g2d_add_cmd(ctx, SRC_LEFT_TOP_REG, pt.val);
    pt.data.x = src_x + w;
    pt.data.y = src_y + h;
    g2d_add_cmd(ctx, SRC_RIGHT_BOTTOM_REG, pt.val);

    pt.data.x = dst_x;
    pt.data.y = dst_y;
    g2d_add_cmd(ctx, DST_LEFT_TOP_REG, pt.val);
    pt.data.x = dst_x + w;
    pt.data.y = dst_y + h;
    g2d_add_cmd(ctx, DST_RIGHT_BOTTOM_REG, pt.val);

    rop4.val = 0;
    rop4.data.unmasked_rop3 = G2D_ROP3_SRC;
    g2d_add_cmd(ctx, ROP4_REG, rop4.val);

    return g2d_flush(ctx);
}

drm_public int g2d_copy_with_scale(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int src_w, unsigned int src_h, unsigned int dst_x, unsigned int dst_y, unsigned int dst_w, unsigned int dst_h, unsigned int negative)
{
    union g2d_point_val pt;
    union g2d_rop4_val rop4;
    unsigned int scale_x, scale_y;
    unsigned int scale, repeat_pad;

    if (negative) {
        negative = 1;
    }

    if ((src_w == dst_w) && (src_h == dst_h)) {
        scale = 0;
    } else {
        scale = 1;
        scale_x = g2d_get_scaling(src_w, dst_w);
        scale_y = g2d_get_scaling(src_h, dst_h);
    }

    repeat_pad = src->repeat_mode == G2D_REPEAT_MODE_PAD ? 1 : 0;

    if ((src_x + src_w) > src->width) {
        src_w = src->width - src_x;
    }

    if ((src_y + src_h) > src->height) {
        src_h = src->height - src_y;
    }

    if ((dst_x + dst_w) > dst->width) {
        dst_w = dst->width - dst_x;
    }

    if ((dst_y + dst_h) > dst->height) {
        dst_h = dst->height - dst_y;
    }

    if ((src_w <= 0) || (src_h <= 0) || (dst_w <= 0) || (dst_h <= 0)) {
        fprintf(stderr, MSG_PREFIX "invalid width or height.\n");
        return -EINVAL;
    }

    if (g2d_check_space(ctx, 12 + scale * 3 + negative + repeat_pad, 2)) {
        return -ENOSPC;
    }

    g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_BGCOLOR);
    g2d_add_cmd(ctx, DST_COLOR_MODE_REG, dst->color_mode);
    g2d_add_base_addr(ctx, dst, g2d_dst);
    g2d_add_cmd(ctx, DST_STRIDE_REG, dst->stride);

    g2d_add_cmd(ctx, SRC_SELECT_REG, G2D_SELECT_MODE_NORMAL);
    g2d_add_cmd(ctx, SRC_COLOR_MODE_REG, src->color_mode);

    g2d_add_cmd(ctx, SRC_REPEAT_MODE_REG, src->repeat_mode);
    if (repeat_pad) {
        g2d_add_cmd(ctx, SRC_PAD_VALUE_REG, dst->color);
    }

    g2d_add_base_addr(ctx, src, g2d_src);
    g2d_add_cmd(ctx, SRC_STRIDE_REG, src->stride);

    rop4.val = 0;
    rop4.data.unmasked_rop3 = G2D_ROP3_SRC;

    if (negative) {
        g2d_add_cmd(ctx, BG_COLOR_REG, 0x00FFFFFF);
        rop4.data.unmasked_rop3 ^= G2D_ROP3_DST;
    }

    g2d_add_cmd(ctx, ROP4_REG, rop4.val);

    if (scale) {
        g2d_add_cmd(ctx, SRC_SCALE_CTRL_REG, G2D_SCALE_MODE_BILINEAR);
        g2d_add_cmd(ctx, SRC_XSCALE_REG, scale_x);
        g2d_add_cmd(ctx, SRC_YSCALE_REG, scale_y);
    }

    pt.data.x = src_x;
    pt.data.y = src_y;
    g2d_add_cmd(ctx, SRC_LEFT_TOP_REG, pt.val);
    pt.data.x = src_x + src_w;
    pt.data.y = src_y + src_h;
    g2d_add_cmd(ctx, SRC_RIGHT_BOTTOM_REG, pt.val);

    pt.data.x = dst_x;
    pt.data.y = dst_y;
    g2d_add_cmd(ctx, DST_LEFT_TOP_REG, pt.val);
    pt.data.x = dst_x + dst_w;
    pt.data.y = dst_y + dst_h;
    g2d_add_cmd(ctx, DST_RIGHT_BOTTOM_REG, pt.val);

    return g2d_flush(ctx);
}

drm_public int g2d_blend(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int dst_x, unsigned int dst_y, unsigned int w, unsigned int h, enum e_g2d_op op)
{
    union g2d_point_val pt;
    unsigned int gem_space;
    union g2d_blend_func_val blend;
    union g2d_bitblt_cmd_val bitblt;
    unsigned int src_w, src_h, dst_w, dst_h;

    src_w = w;
    src_h = h;

    if ((src_x + w) > src->width) {
        src_w = src->width - src_x;
    }

    if ((src_y + h) > src->height) {
        src_h = src->height - src_y;
    }

    dst_w = w;
    dst_h = h;

    if ((dst_x + w) > dst->width) {
        dst_w = dst->width - dst_x;
    }

    if ((dst_y + h) > dst->height) {
        dst_h = dst->height - dst_y;
    }

    w = MIN(src_w, dst_w);
    h = MIN(src_h, dst_h);

    if ((w <= 0) || (h <= 0)) {
        fprintf(stderr, MSG_PREFIX "invalid width or height.\n");
        return -EINVAL;
    }

    if (!g2d_validate_select_mode(src->select_mode)) {
        fprintf(stderr , MSG_PREFIX "invalid select mode for source.\n");
        return -EINVAL;
    }

    if (!g2d_validate_blending_op(op)) {
        fprintf(stderr , MSG_PREFIX "unsupported blending operation.\n");
        return -EINVAL;
    }

    gem_space = src->select_mode == G2D_SELECT_MODE_NORMAL ? 2 : 1;
    if (g2d_check_space(ctx, 12, gem_space)) {
        return -ENOSPC;
    }

    bitblt.val = 0;
    blend.val = 0;

    if ((op == G2D_OP_SRC) || (op == G2D_OP_CLEAR)) {
        g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_BGCOLOR);
    } else {
        g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_NORMAL);
    }

    g2d_add_cmd(ctx, DST_COLOR_MODE_REG, dst->color_mode);
    g2d_add_base_addr(ctx, dst, g2d_dst);
    g2d_add_cmd(ctx, DST_STRIDE_REG, dst->stride);

    g2d_add_cmd(ctx, SRC_SELECT_REG, src->select_mode);
    g2d_add_cmd(ctx, SRC_COLOR_MODE_REG, src->color_mode);

    switch (src->select_mode) {
        case G2D_SELECT_MODE_NORMAL:
            g2d_add_base_addr(ctx, src, g2d_src);
            g2d_add_cmd(ctx, SRC_STRIDE_REG, src->stride);
            break;

        case G2D_SELECT_MODE_FGCOLOR:
            g2d_add_cmd(ctx, FG_COLOR_REG, src->color);
            break;

        case G2D_SELECT_MODE_BGCOLOR:
            g2d_add_cmd(ctx, BG_COLOR_REG, src->color);
            break;
    }

    bitblt.data.alpha_blend_mode = G2D_ALPHA_BLEND_MODE_ENABLE;
    blend.val = g2d_get_blend_op(op);
    g2d_add_cmd(ctx, BITBLT_COMMAND_REG, bitblt.val);
    g2d_add_cmd(ctx, BLEND_FUNCTION_REG, blend.val);

    pt.data.x = src_x;
    pt.data.y = src_y;
    g2d_add_cmd(ctx, SRC_LEFT_TOP_REG, pt.val);
    pt.data.x = src_x + w;
    pt.data.y = src_y + h;
    g2d_add_cmd(ctx, SRC_RIGHT_BOTTOM_REG, pt.val);

    pt.data.x = dst_x;
    pt.data.y = dst_y;
    g2d_add_cmd(ctx, DST_LEFT_TOP_REG, pt.val);
    pt.data.x = dst_x + w;
    pt.data.y = dst_y + h;
    g2d_add_cmd(ctx, DST_RIGHT_BOTTOM_REG, pt.val);

    return g2d_flush(ctx);
}

drm_public int g2d_scale_and_blend(struct g2d_context *ctx, struct g2d_image *src, struct g2d_image *dst, unsigned int src_x, unsigned int src_y, unsigned int src_w, unsigned int src_h, unsigned int dst_x, unsigned int dst_y, unsigned int dst_w, unsigned int dst_h, enum e_g2d_op op)
{
    union g2d_point_val pt;
    unsigned int scale, gem_space;
    unsigned int scale_x, scale_y;
    union g2d_blend_func_val blend;
    union g2d_bitblt_cmd_val bitblt;

    if ((src_w == dst_w) && (src_h == dst_h)) {
        scale = 0;
    } else {
        scale = 1;
        scale_x = g2d_get_scaling(src_w, dst_w);
        scale_y = g2d_get_scaling(src_h, dst_h);
    }

    if ((src_x + src_w) > src->width) {
        src_w = src->width - src_x;
    }

    if ((src_y + src_h) > src->height) {
        src_h = src->height - src_y;
    }

    if ((dst_x + dst_w) > dst->width) {
        dst_w = dst->width - dst_x;
    }

    if ((dst_y + dst_h) > dst->height) {
        dst_h = dst->height - dst_y;
    }

    if ((src_w <= 0) || (src_h <= 0) || (dst_w <= 0) || (dst_h <= 0)) {
        fprintf(stderr, MSG_PREFIX "invalid width or height.\n");
        return -EINVAL;
    }

    if (!g2d_validate_select_mode(src->select_mode)) {
        fprintf(stderr , MSG_PREFIX "invalid select mode for source.\n");
        return -EINVAL;
    }

    if (!g2d_validate_blending_op(op)) {
        fprintf(stderr , MSG_PREFIX "unsupported blending operation.\n");
        return -EINVAL;
    }

    gem_space = src->select_mode == G2D_SELECT_MODE_NORMAL ? 2 : 1;
    if (g2d_check_space(ctx, 12 + scale * 3, gem_space)) {
        return -ENOSPC;
    }

    bitblt.val = 0;
    blend.val = 0;

    if ((op == G2D_OP_SRC) || (op == G2D_OP_CLEAR)) {
        g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_BGCOLOR);
    } else {
        g2d_add_cmd(ctx, DST_SELECT_REG, G2D_SELECT_MODE_NORMAL);
    }

    g2d_add_cmd(ctx, DST_COLOR_MODE_REG, dst->color_mode);
    g2d_add_base_addr(ctx, dst, g2d_dst);
    g2d_add_cmd(ctx, DST_STRIDE_REG, dst->stride);

    g2d_add_cmd(ctx, SRC_SELECT_REG, src->select_mode);
    g2d_add_cmd(ctx, SRC_COLOR_MODE_REG, src->color_mode);

    switch (src->select_mode) {
        case G2D_SELECT_MODE_NORMAL:
            g2d_add_base_addr(ctx, src, g2d_src);
            g2d_add_cmd(ctx, SRC_STRIDE_REG, src->stride);
            break;

        case G2D_SELECT_MODE_FGCOLOR:
            g2d_add_cmd(ctx, FG_COLOR_REG, src->color);
            break;

        case G2D_SELECT_MODE_BGCOLOR:
            g2d_add_cmd(ctx, BG_COLOR_REG, src->color);
            break;
    }

    if (scale) {
        g2d_add_cmd(ctx, SRC_SCALE_CTRL_REG, G2D_SCALE_MODE_BILINEAR);
        g2d_add_cmd(ctx, SRC_XSCALE_REG, scale_x);
        g2d_add_cmd(ctx, SRC_YSCALE_REG, scale_y);
    }

    bitblt.data.alpha_blend_mode = G2D_ALPHA_BLEND_MODE_ENABLE;
    blend.val = g2d_get_blend_op(op);
    g2d_add_cmd(ctx, BITBLT_COMMAND_REG, bitblt.val);
    g2d_add_cmd(ctx, BLEND_FUNCTION_REG, blend.val);

    pt.data.x = src_x;
    pt.data.y = src_y;
    g2d_add_cmd(ctx, SRC_LEFT_TOP_REG, pt.val);
    pt.data.x = src_x + src_w;
    pt.data.y = src_y + src_h;
    g2d_add_cmd(ctx, SRC_RIGHT_BOTTOM_REG, pt.val);

    pt.data.x = dst_x;
    pt.data.y = dst_y;
    g2d_add_cmd(ctx, DST_LEFT_TOP_REG, pt.val);
    pt.data.x = dst_x + dst_w;
    pt.data.y = dst_y + dst_h;
    g2d_add_cmd(ctx, DST_RIGHT_BOTTOM_REG, pt.val);

    return g2d_flush(ctx);
}
