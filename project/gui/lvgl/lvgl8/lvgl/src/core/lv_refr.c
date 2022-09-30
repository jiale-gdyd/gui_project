#include <stddef.h>

#include "lv_refr.h"
#include "lv_disp.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_mem.h"
#include "../misc/lv_math.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_timer.h"
#include "../hal/lv_hal_tick.h"
#include "../hal/lv_hal_disp.h"
#include "../font/lv_font_fmt_txt.h"
#include "../others/snapshot/lv_snapshot.h"

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
#include "../widgets/label/lv_label.h"
#endif

#if LV_LOG_TRACE_DISP_REFR
#define REFR_TRACE(...)             LV_LOG_TRACE(__VA_ARGS__)
#else
#define REFR_TRACE(...)
#endif

typedef struct {
    uint32_t  perf_last_time;
    uint32_t  elaps_sum;
    uint32_t  frame_cnt;
    uint32_t  fps_sum_cnt;
    uint32_t  fps_sum_all;
#if LV_USE_LABEL
    lv_obj_t  *perf_label;
#endif
} perf_monitor_t;

typedef struct {
    uint32_t mem_last_time;
#if LV_USE_LABEL
    lv_obj_t *mem_label;
#endif
} mem_monitor_t;

static uint32_t px_num;
static lv_disp_t *disp_refr;

#if LV_USE_PERF_MONITOR
static perf_monitor_t perf_monitor;
#endif

#if LV_USE_MEM_MONITOR
static mem_monitor_t mem_monitor;
#endif

static void lv_refr_join_area(void);
static void refr_invalid_areas(void);
static void draw_buf_flush(lv_disp_t *disp);
static void refr_area(const lv_area_t *area_p);
static void refr_area_part(lv_draw_ctx_t *draw_ctx);
static void refr_obj(lv_draw_ctx_t *draw_ctx, lv_obj_t *obj);
static lv_obj_t *lv_refr_get_top_obj(const lv_area_t *area_p, lv_obj_t *obj);
static void refr_obj_and_children(lv_draw_ctx_t *draw_ctx, lv_obj_t *top_obj);
static uint32_t get_max_row(lv_disp_t *disp, lv_coord_t area_w, lv_coord_t area_h);
static void call_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);

#if LV_USE_PERF_MONITOR
static void perf_monitor_init(perf_monitor_t *perf_monitor);
#endif

#if LV_USE_MEM_MONITOR
static void mem_monitor_init(mem_monitor_t *mem_monitor);
#endif

void _lv_refr_init(void)
{
#if LV_USE_PERF_MONITOR
    perf_monitor_init(&perf_monitor);
#endif

#if LV_USE_MEM_MONITOR
    mem_monitor_init(&mem_monitor);
#endif
}

void lv_refr_now(lv_disp_t *disp)
{
    lv_anim_refr_now();

    if (disp) {
        if (disp->refr_timer) {
            _lv_disp_refr_timer(disp->refr_timer);
        }
    } else {
        lv_disp_t *d;
        d = lv_disp_get_next(NULL);
        while (d) {
            if (d->refr_timer) {
                _lv_disp_refr_timer(d->refr_timer);
            }

            d = lv_disp_get_next(d);
        }
    }
}

void lv_obj_redraw(lv_draw_ctx_t *draw_ctx, lv_obj_t *obj)
{
    lv_area_t obj_coords_ext;
    lv_area_t clip_coords_for_obj;
    lv_obj_get_coords(obj, &obj_coords_ext);
    const lv_area_t *clip_area_ori = draw_ctx->clip_area;
    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);

    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);

    bool com_clip_res = _lv_area_intersect(&clip_coords_for_obj, clip_area_ori, &obj_coords_ext);

    // 如果对象在当前剪辑区域可见或溢出可见绘制它。应该使用溢出可见绘图应用可能影响子的mask
    bool should_draw = com_clip_res || lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    if (should_draw) {
        draw_ctx->clip_area = &clip_coords_for_obj;

        lv_event_send(obj, LV_EVENT_DRAW_MAIN_BEGIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, draw_ctx);

#if LV_USE_REFR_DEBUG
        lv_color_t debug_color = lv_color_make(lv_rand(0, 0xFF), lv_rand(0, 0xFF), lv_rand(0, 0xFF));
        lv_draw_rect_dsc_t draw_dsc;

        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_color.full = debug_color.full;
        draw_dsc.bg_opa = LV_OPA_20;
        draw_dsc.border_width = 1;
        draw_dsc.border_opa = LV_OPA_30;
        draw_dsc.border_color = debug_color;
        lv_draw_rect(draw_ctx, &draw_dsc, &obj_coords_ext);
#endif
    }

    // 在溢出可见的情况下，保持之前的剪辑区域，让这个对象之外的子对象也可见。在不溢出可见限制下，剪辑将以对象的坐标来剪辑子对象
    lv_area_t clip_coords_for_children;
    bool refr_children = true;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
        clip_coords_for_children  = *clip_area_ori;
    } else {
        if (!_lv_area_intersect(&clip_coords_for_children, clip_area_ori, &obj->coords)) {
            refr_children = false;
        }
    }

    if (refr_children) {
        draw_ctx->clip_area = &clip_coords_for_children;

        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);

        for (i = 0; i < child_cnt; i++) {
            lv_obj_t *child = obj->spec_attr->children[i];
            refr_obj(draw_ctx, child);
        }
    }

    if (should_draw) {
        draw_ctx->clip_area = &clip_coords_for_obj;
        lv_event_send(obj, LV_EVENT_DRAW_POST_BEGIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_POST, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_POST_END, draw_ctx);
    }

    draw_ctx->clip_area = clip_area_ori;
}

void _lv_inv_area(lv_disp_t *disp, const lv_area_t *area_p)
{
    if (!disp) {
        disp = lv_disp_get_default();
    }

    if (!disp) {
        return;
    }

    if (!lv_disp_is_invalidation_enabled(disp)) {
        return;
    }

    if (disp->rendering_in_progress) {
        LV_LOG_ERROR("detected modifying dirty areas in render");
        return;
    }

    // 如果参数为NULL，则清除无效缓冲区
    if (area_p == NULL) {
        disp->inv_p = 0;
        return;
    }

    lv_area_t scr_area;
    scr_area.x1 = 0;
    scr_area.y1 = 0;
    scr_area.x2 = lv_disp_get_hor_res(disp) - 1;
    scr_area.y2 = lv_disp_get_ver_res(disp) - 1;

    bool suc;
    lv_area_t com_area;

    suc = _lv_area_intersect(&com_area, area_p, &scr_area);
    if (suc == false) {
        // 出画面
        return;
    }

    // 如果全刷新模式下至少有1个无效区域，则重绘整个屏幕
    if (disp->driver->full_refresh) {
        disp->inv_areas[0] = scr_area;
        disp->inv_p = 1;
        if (disp->refr_timer) {
            lv_timer_resume(disp->refr_timer);
        }

        return;
    }

    if (disp->driver->rounder_cb) {
        disp->driver->rounder_cb(disp->driver, &com_area);
    }

    // 仅当此区域不在已保存区域之一中时才保存
    uint16_t i;
    for (i = 0; i < disp->inv_p; i++) {
        if (_lv_area_is_in(&com_area, &disp->inv_areas[i], 0) != false) {
            return;
        }
    }

    // 保存区域
    if (disp->inv_p < LV_INV_BUF_SIZE) {
        lv_area_copy(&disp->inv_areas[disp->inv_p], &com_area);
    } else {   // 如果该区域没有地方添加屏幕
        disp->inv_p = 0;
        lv_area_copy(&disp->inv_areas[disp->inv_p], &scr_area);
    }

    disp->inv_p++;
    if (disp->refr_timer) {
        lv_timer_resume(disp->refr_timer);
    }
}

lv_disp_t *_lv_refr_get_disp_refreshing(void)
{
    return disp_refr;
}

void _lv_refr_set_disp_refreshing(lv_disp_t *disp)
{
    disp_refr = disp;
}

void _lv_disp_refr_timer(lv_timer_t *tmr)
{
    REFR_TRACE("begin");

    uint32_t start = lv_tick_get();

    if (tmr) {
        disp_refr = (lv_disp_t *)tmr->user_data;
#if (LV_USE_PERF_MONITOR == 0) && (LV_USE_MEM_MONITOR == 0)
        // 确保计时器不会再次自动运行。这是在刷新之前完成的，以防刷新使其他内容无效
        lv_timer_pause(tmr);
#endif
    } else {
        disp_refr = lv_disp_get_default();
    }

    uint32_t elaps = lv_tick_elaps(disp_refr->last_render_start_time);
    disp_refr->last_render_start_time = start;

#if LV_USE_PERF_MONITOR && LV_USE_LABEL
    lv_obj_t *perf_label = perf_monitor.perf_label;
    if (perf_label == NULL) {
        perf_label = lv_label_create(lv_layer_sys());
        lv_obj_set_style_bg_opa(perf_label, LV_OPA_50, 0);
        lv_obj_set_style_bg_color(perf_label, lv_color_black(), 0);
        lv_obj_set_style_text_color(perf_label, lv_color_white(), 0);
        lv_obj_set_style_pad_top(perf_label, 3, 0);
        lv_obj_set_style_pad_bottom(perf_label, 3, 0);
        lv_obj_set_style_pad_left(perf_label, 3, 0);
        lv_obj_set_style_pad_right(perf_label, 3, 0);
        lv_obj_set_style_text_align(perf_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(perf_label, "?");
        lv_obj_align(perf_label, LV_USE_PERF_MONITOR_POS, 0, 0);
        perf_monitor.perf_label = perf_label;
    }

    if (lv_tick_elaps(perf_monitor.perf_last_time) < 300) {
        if (px_num > 5000) {
            perf_monitor.elaps_sum += elaps;
            perf_monitor.frame_cnt++;
        }
    } else {
        perf_monitor.perf_last_time = lv_tick_get();

        uint32_t fps;
        uint32_t fps_limit;

        if (disp_refr->refr_timer) {
            fps_limit = 1000 / disp_refr->refr_timer->period;
        } else {
            fps_limit = 1000 / 33;
        }

        if (perf_monitor.elaps_sum == 0) {
            perf_monitor.elaps_sum = 1;
        }

        if (perf_monitor.frame_cnt == 0) {
            fps = fps_limit;
        } else {
            fps = (1000 * perf_monitor.frame_cnt) / perf_monitor.elaps_sum;
        }

        perf_monitor.elaps_sum = 0;
        perf_monitor.frame_cnt = 0;
        if (fps > fps_limit) {
            fps = fps_limit;
        }

        perf_monitor.fps_sum_all += fps;
        perf_monitor.fps_sum_cnt++;

        uint32_t cpu = 100 - lv_timer_get_idle();
        lv_label_set_text_fmt(perf_label, "%" LV_PRIu32 " FPS\n%" LV_PRIu32 "%% CPU", fps, cpu);
    }
#endif

#if LV_USE_MEM_MONITOR && LV_USE_BUILTIN_MALLOC && LV_USE_LABEL
    lv_obj_t *mem_label = mem_monitor.mem_label;

    if (mem_label == NULL) {
        mem_label = lv_label_create(lv_layer_sys());
        lv_obj_set_style_bg_opa(mem_label, LV_OPA_50, 0);
        lv_obj_set_style_bg_color(mem_label, lv_color_black(), 0);
        lv_obj_set_style_text_color(mem_label, lv_color_white(), 0);
        lv_obj_set_style_pad_top(mem_label, 3, 0);
        lv_obj_set_style_pad_bottom(mem_label, 3, 0);
        lv_obj_set_style_pad_left(mem_label, 3, 0);
        lv_obj_set_style_pad_right(mem_label, 3, 0);
        lv_label_set_text(mem_label, "?");
        lv_obj_align(mem_label, LV_USE_MEM_MONITOR_POS, 0, 0);
        mem_monitor.mem_label = mem_label;
    }

    if (lv_tick_elaps(mem_monitor.mem_last_time) > 300) {
        mem_monitor.mem_last_time = lv_tick_get();
        mem_last_time = lv_tick_get();
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);

        uint32_t used_size = mon.total_size - mon.free_size;;
        uint32_t used_kb = used_size / 1024;
        uint32_t used_kb_tenth = (used_size - (used_kb * 1024)) / 102;
        lv_label_set_text_fmt(mem_label, "%"LV_PRIu32 ".%"LV_PRIu32 " kB used (%d %%)\n%d%% frag.", used_kb, used_kb_tenth, mon.used_pct, mon.frag_pct);
    }
#endif

    lv_obj_update_layout(disp_refr->act_scr);
    if (disp_refr->prev_scr) {
        lv_obj_update_layout(disp_refr->prev_scr);
    }

    lv_obj_update_layout(disp_refr->top_layer);
    lv_obj_update_layout(disp_refr->sys_layer);

    if (disp_refr->act_scr == NULL) {
        disp_refr->inv_p = 0;
        LV_LOG_WARN("there is no active screen");
        REFR_TRACE("finished");
        return;
    }

    if (disp_refr->driver->direct_mode && (disp_refr->driver->draw_ctx->color_format != LV_COLOR_FORMAT_NATIVE)) {
        LV_LOG_WARN("In direct_mode only LV_COLOR_FORMAT_NATIVE color format is supported");
        return;
    }

    lv_refr_join_area();
    refr_invalid_areas();

    if (disp_refr->inv_p != 0) {
        if (disp_refr->driver->full_refresh) {
            lv_area_t disp_area;
            lv_area_set(&disp_area, 0, 0, lv_disp_get_hor_res(disp_refr) - 1, lv_disp_get_ver_res(disp_refr) - 1);
            disp_refr->driver->draw_ctx->buf_area = &disp_area;
            draw_buf_flush(disp_refr);
        }

        lv_memzero(disp_refr->inv_areas, sizeof(disp_refr->inv_areas));
        lv_memzero(disp_refr->inv_area_joined, sizeof(disp_refr->inv_area_joined));
        disp_refr->inv_p = 0;

        if(disp_refr->driver->monitor_cb) {
            disp_refr->driver->monitor_cb(disp_refr->driver, elaps, px_num);
        }
    }

    _lv_font_clean_up_fmt_txt();

#if LV_USE_DRAW_MASKS
    _lv_draw_mask_cleanup();
#endif

    REFR_TRACE("finished");
}

#if LV_USE_PERF_MONITOR
void lv_refr_reset_fps_counter(void)
{
    perf_monitor.fps_sum_all = 0;
    perf_monitor.fps_sum_cnt = 0;
}

uint32_t lv_refr_get_fps_avg(void)
{
    if (perf_monitor.fps_sum_cnt == 0) {
        return 0;
    }

    return perf_monitor.fps_sum_all / perf_monitor.fps_sum_cnt;
}
#endif

static void lv_refr_join_area(void)
{
    uint32_t join_in;
    uint32_t join_from;
    lv_area_t joined_area;

    for (join_in = 0; join_in < disp_refr->inv_p; join_in++) {
        if (disp_refr->inv_area_joined[join_in] != 0) {
            continue;
        }

        // 检查所有区域以在'join_in'中加入它们
        for (join_from = 0; join_from < disp_refr->inv_p; join_from++) {
            // 仅处理未连接的区域并忽略自身
            if ((disp_refr->inv_area_joined[join_from] != 0) || (join_in == join_from)) {
                continue;
            }

            // 检查区域是否相互重叠
            if (_lv_area_is_on(&disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]) == false) {
                continue;
            }

            _lv_area_join(&joined_area, &disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]);

            // 仅当连接区域尺寸较小时才连接两个区域
            if (lv_area_get_size(&joined_area) < (lv_area_get_size(&disp_refr->inv_areas[join_in]) + lv_area_get_size(&disp_refr->inv_areas[join_from]))) {
                lv_area_copy(&disp_refr->inv_areas[join_in], &joined_area);

                // 标记'join_form'加入到'join_in'
                disp_refr->inv_area_joined[join_from] = 1;
            }
        }
    }
}

static void refr_invalid_areas(void)
{
    px_num = 0;

    if (disp_refr->inv_p == 0) {
        return;
    }

    // 找到将要绘制的最后一个区域
    int32_t i;
    int32_t last_i = 0;

    for (i = disp_refr->inv_p - 1; i >= 0; i--) {
        if (disp_refr->inv_area_joined[i] == 0) {
            last_i = i;
            break;
        }
    }

    // 通知显示驱动的渲染已经开始
    if (disp_refr->driver->render_start_cb) {
        disp_refr->driver->render_start_cb(disp_refr->driver);
    }

    disp_refr->driver->draw_buf->last_area = 0;
    disp_refr->driver->draw_buf->last_part = 0;
    disp_refr->rendering_in_progress = true;

    for (i = 0; i < disp_refr->inv_p; i++) {
        // 刷新未连接的区域
        if (disp_refr->inv_area_joined[i] == 0) {
            if (i == last_i) {
                disp_refr->driver->draw_buf->last_area = 1;
            }

            disp_refr->driver->draw_buf->last_part = 0;
            refr_area(&disp_refr->inv_areas[i]);

            px_num += lv_area_get_size(&disp_refr->inv_areas[i]);
        }
    }

    disp_refr->rendering_in_progress = false;
}

static void refr_area(const lv_area_t *area_p)
{
    lv_draw_ctx_t *draw_ctx = disp_refr->driver->draw_ctx;
    draw_ctx->buf = disp_refr->driver->draw_buf->buf_act;

    if (disp_refr->driver->full_refresh || disp_refr->driver->direct_mode) {
        lv_area_t disp_area;
        lv_area_set(&disp_area, 0, 0, lv_disp_get_hor_res(disp_refr) - 1, lv_disp_get_ver_res(disp_refr) - 1);
        draw_ctx->buf_area = &disp_area;

        if (disp_refr->driver->full_refresh) {
            disp_refr->driver->draw_buf->last_part = 1;
            draw_ctx->clip_area = &disp_area;
            refr_area_part(draw_ctx);
        } else {
            disp_refr->driver->draw_buf->last_part = disp_refr->driver->draw_buf->last_area;
            draw_ctx->clip_area = area_p;
            refr_area_part(draw_ctx);
        }

        return;
    }

    lv_coord_t w = lv_area_get_width(area_p);
    lv_coord_t h = lv_area_get_height(area_p);
    lv_coord_t y2 = area_p->y2 >= lv_disp_get_ver_res(disp_refr) ? lv_disp_get_ver_res(disp_refr) - 1 : area_p->y2;
    int32_t max_row = get_max_row(disp_refr, w, h);

    lv_coord_t row;
    lv_area_t sub_area;
    lv_coord_t row_last = 0;

    for (row = area_p->y1; (row + max_row - 1) <= y2; row += max_row) {
        sub_area.x1 = area_p->x1;
        sub_area.x2 = area_p->x2;
        sub_area.y1 = row;
        sub_area.y2 = row + max_row - 1;
        draw_ctx->buf_area = &sub_area;
        draw_ctx->clip_area = &sub_area;
        draw_ctx->buf = disp_refr->driver->draw_buf->buf_act;
        if (sub_area.y2 > y2) {
            sub_area.y2 = y2;
        }

        row_last = sub_area.y2;
        if (y2 == row_last) {
            disp_refr->driver->draw_buf->last_part = 1;
        }

        refr_area_part(draw_ctx);
    }

    if (y2 != row_last) {
        sub_area.x1 = area_p->x1;
        sub_area.x2 = area_p->x2;
        sub_area.y1 = row;
        sub_area.y2 = y2;
        draw_ctx->buf_area = &sub_area;
        draw_ctx->clip_area = &sub_area;
        draw_ctx->buf = disp_refr->driver->draw_buf->buf_act;
        disp_refr->driver->draw_buf->last_part = 1;
        refr_area_part(draw_ctx);
    }
}

static void refr_area_part(lv_draw_ctx_t *draw_ctx)
{
    lv_disp_draw_buf_t *draw_buf = lv_disp_get_draw_buf(disp_refr);

    if (draw_buf->buf1 && !draw_buf->buf2) {
        while (draw_buf->flushing) {
            if (disp_refr->driver->wait_cb) {
                disp_refr->driver->wait_cb(disp_refr->driver);
            }
        }

        if (disp_refr->driver->screen_transp) {
            if (disp_refr->driver->clear_cb) {
                disp_refr->driver->clear_cb(disp_refr->driver, (uint8_t *)disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size);
            } else {
                lv_memzero(disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size * LV_IMG_PX_SIZE_ALPHA_BYTE);
            }
        }
    }

    lv_obj_t *top_act_scr = NULL;
    lv_obj_t *top_prev_scr = NULL;

    // 获取未被其他人覆盖的最顶层物体
    top_act_scr = lv_refr_get_top_obj(draw_ctx->buf_area, lv_disp_get_scr_act(disp_refr));
    if (disp_refr->prev_scr) {
        top_prev_scr = lv_refr_get_top_obj(draw_ctx->buf_area, disp_refr->prev_scr);
    }

    // 如果没有顶部对象，则绘制显示背景
    if ((top_act_scr == NULL) && (top_prev_scr == NULL)) {
        lv_area_t a;
        lv_area_set(&a, 0, 0, lv_disp_get_hor_res(disp_refr) - 1, lv_disp_get_ver_res(disp_refr) - 1);

        if (draw_ctx->draw_bg) {
            lv_draw_rect_dsc_t dsc;
            lv_draw_rect_dsc_init(&dsc);
            dsc.bg_img_src = disp_refr->bg_img;
            dsc.bg_img_opa = disp_refr->bg_opa;
            dsc.bg_color = disp_refr->bg_color;
            dsc.bg_opa = disp_refr->bg_opa;
            draw_ctx->draw_bg(draw_ctx, &dsc, &a);
        } else if (disp_refr->bg_img) {
            lv_img_header_t header;
            lv_res_t res = lv_img_decoder_get_info(disp_refr->bg_img, &header);
            if (res == LV_RES_OK) {
                lv_draw_img_dsc_t dsc;
                lv_draw_img_dsc_init(&dsc);
                dsc.opa = disp_refr->bg_opa;
                lv_draw_img(draw_ctx, &dsc, &a, disp_refr->bg_img);
            } else {
                LV_LOG_WARN("Can't draw the background image");
            }
        } else {
            lv_draw_rect_dsc_t dsc;
            lv_draw_rect_dsc_init(&dsc);
            dsc.bg_color = disp_refr->bg_color;
            dsc.bg_opa = disp_refr->bg_opa;
            lv_draw_rect(draw_ctx, &dsc, draw_ctx->buf_area);
        }
    }

    if (disp_refr->draw_prev_over_act) {
        if (top_act_scr == NULL) {
            top_act_scr = disp_refr->act_scr;
        }
        refr_obj_and_children(draw_ctx, top_act_scr);

        if (disp_refr->prev_scr) {
            if (top_prev_scr == NULL) {
                top_prev_scr = disp_refr->prev_scr;
            }
            refr_obj_and_children(draw_ctx, top_prev_scr);
        }
    } else {
        if (disp_refr->prev_scr) {
            if (top_prev_scr == NULL) {
                top_prev_scr = disp_refr->prev_scr;
            }
            refr_obj_and_children(draw_ctx, top_prev_scr);
        }

        if (top_act_scr == NULL) {
            top_act_scr = disp_refr->act_scr;
        }
        refr_obj_and_children(draw_ctx, top_act_scr);
    }

    if (top_act_scr == NULL) {
        top_act_scr = disp_refr->act_scr;
    }
    refr_obj_and_children(draw_ctx, top_act_scr);

    // 也无条件刷新top和sys层
    refr_obj_and_children(draw_ctx, lv_disp_get_layer_top(disp_refr));
    refr_obj_and_children(draw_ctx, lv_disp_get_layer_sys(disp_refr));

    // 在真正的双缓冲模式下，当所有区域都被渲染时，只刷新一次。在正常模式下冲洗每个区域
    if (disp_refr->driver->full_refresh == false) {
        draw_buf_flush(disp_refr);
    }
}

static lv_obj_t *lv_refr_get_top_obj(const lv_area_t *area_p, lv_obj_t *obj)
{
    lv_obj_t *found_p = NULL;

    if (_lv_area_is_in(area_p, &obj->coords, 0) == false) {
        return NULL;
    }

    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        return NULL;
    }

    if (_lv_obj_get_layer_type(obj) != LV_LAYER_TYPE_NONE) {
        return NULL;
    }

    lv_cover_check_info_t info;
    info.res = LV_COVER_RES_COVER;
    info.area = area_p;
    lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
    if (info.res == LV_COVER_RES_MASKED) {
        return NULL;
    }

    int32_t i;
    int32_t child_cnt = lv_obj_get_child_cnt(obj);
    for (i = child_cnt - 1; i >= 0; i--) {
        lv_obj_t *child = obj->spec_attr->children[i];
        found_p = lv_refr_get_top_obj(area_p, child);

        if (found_p != NULL) {
            break;
        }
    }

    if (found_p == NULL && (info.res == LV_COVER_RES_COVER)) {
        found_p = obj;
    }

    return found_p;
}

static void refr_obj_and_children(lv_draw_ctx_t *draw_ctx, lv_obj_t *top_obj)
{
    // 通常总是一个top_obj(至少是屏幕)但在特殊情况下(例如如果屏幕有alpha)它不会。在这种情况下直接使用屏幕
    if (top_obj == NULL) {
        top_obj = lv_disp_get_scr_act(disp_refr);
    }

    if (top_obj == NULL) {
        return;
    }

    // 刷新顶部对象及其子对象
    refr_obj(draw_ctx, top_obj);

    // 绘制'younger'兄弟对象，因为它们可以在top_obj上
    lv_obj_t *parent;
    lv_obj_t *border_p = top_obj;

    parent = lv_obj_get_parent(top_obj);

    // 直到没有到达屏幕
    while (parent != NULL) {
        uint32_t i;
        bool go = false;
        uint32_t child_cnt = lv_obj_get_child_cnt(parent);

        for (i = 0; i < child_cnt; i++) {
            lv_obj_t *child = parent->spec_attr->children[i];
            if (!go) {
                if (child == border_p) {
                    go = true;
                }
            } else {
                // 刷新对象
                refr_obj(draw_ctx, child);
            }
        }

        // 调用to对象的父对象的post draw draw函数
        lv_event_send(parent, LV_EVENT_DRAW_POST_BEGIN, (void *)draw_ctx);
        lv_event_send(parent, LV_EVENT_DRAW_POST, (void *)draw_ctx);
        lv_event_send(parent, LV_EVENT_DRAW_POST_END, (void *)draw_ctx);

        // 新边界将是最后一个父母，因此父母的'younger'兄弟将被刷新
        border_p = parent;
        // 更深一层
        parent = lv_obj_get_parent(parent);
    }
}

static lv_res_t layer_get_area(lv_draw_ctx_t *draw_ctx, lv_obj_t *obj, lv_layer_type_t layer_type, lv_area_t *layer_area_out)
{
    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_t obj_coords_ext;

    lv_obj_get_coords(obj, &obj_coords_ext);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);

    if (layer_type == LV_LAYER_TYPE_TRANSFORM) {
        // 获取变换后的区域，并将其剪辑到当前剪辑区域。此区域需要在屏幕上更新
        lv_area_t clip_coords_for_obj;
        lv_area_t tranf_coords = obj_coords_ext;
        lv_obj_get_transformed_area(obj, &tranf_coords, false, false);
        if (!_lv_area_intersect(&clip_coords_for_obj, draw_ctx->clip_area, &tranf_coords)) {
            return LV_RES_INV;
        }

        // 变换回(逆)变换的区域。它将告诉未转换小部件的哪个区域需要重绘以覆盖转换后的转换区域
        lv_area_t inverse_clip_coords_for_obj = clip_coords_for_obj;
        lv_obj_get_transformed_area(obj, &inverse_clip_coords_for_obj, false, true);
        if (!_lv_area_intersect(&inverse_clip_coords_for_obj, &inverse_clip_coords_for_obj, &obj_coords_ext)) {
            return LV_RES_INV;
        }

        *layer_area_out = inverse_clip_coords_for_obj;
    } else if (layer_type == LV_LAYER_TYPE_SIMPLE) {
        lv_area_t clip_coords_for_obj;
        if (!_lv_area_intersect(&clip_coords_for_obj, draw_ctx->clip_area, &obj_coords_ext)) {
            return LV_RES_INV;
        }

        *layer_area_out = clip_coords_for_obj;
    } else {
        LV_LOG_WARN("Unhandled intermediate layer type");
        return LV_RES_INV;
    }

    return LV_RES_OK;
}

static void layer_alpha_test(lv_obj_t *obj, lv_draw_ctx_t *draw_ctx, lv_draw_layer_ctx_t *layer_ctx, lv_draw_layer_flags_t flags)
{
    bool has_alpha;

    // 如果全局层具有alpha，则可能这个较小的部分没有(例如，不在圆角上)。如果事实证明这部分没有alpha渲染器可以选择更快的算法
    if (flags & LV_DRAW_LAYER_FLAG_HAS_ALPHA) {
        // 通过假设没有alpha来测试alpha。如果失败，则回退到使 alpha进行渲染
        has_alpha = true;
        if (_lv_area_is_in(&layer_ctx->area_act, &obj->coords, 0)) {
            lv_cover_check_info_t info;
            info.res = LV_COVER_RES_COVER;
            info.area = &layer_ctx->area_act;
            lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
            if (info.res == LV_COVER_RES_COVER) {
                has_alpha = false;
            }
        }

        if (has_alpha) {
            layer_ctx->area_act.y2 = layer_ctx->area_act.y1 + layer_ctx->max_row_with_alpha - 1;
        }
    } else {
        has_alpha = false;
    }

    if (layer_ctx->area_act.y2 > layer_ctx->area_full.y2) {
        layer_ctx->area_act.y2 = layer_ctx->area_full.y2;
    }

    lv_draw_layer_adjust(draw_ctx, layer_ctx, has_alpha ? LV_DRAW_LAYER_FLAG_HAS_ALPHA : LV_DRAW_LAYER_FLAG_NONE);
}

void refr_obj(lv_draw_ctx_t *draw_ctx, lv_obj_t *obj)
{
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        return;
    }

    lv_layer_type_t layer_type = _lv_obj_get_layer_type(obj);
    if  (layer_type == LV_LAYER_TYPE_NONE) {
        lv_obj_redraw(draw_ctx, obj);
    } else {
        lv_opa_t opa = lv_obj_get_style_opa(obj, 0);
        if (opa < LV_OPA_MIN) {
            return;
        }

        lv_area_t layer_area_full;
        lv_res_t res = layer_get_area(draw_ctx, obj, layer_type, &layer_area_full);
        if (res != LV_RES_OK) {
            return;
        }

        lv_draw_layer_flags_t flags = LV_DRAW_LAYER_FLAG_HAS_ALPHA;

        if (_lv_area_is_in(&layer_area_full, &obj->coords, 0)) {
            lv_cover_check_info_t info;
            info.res = LV_COVER_RES_COVER;
            info.area = &layer_area_full;
            lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
            if (info.res == LV_COVER_RES_COVER) {
                flags = (lv_draw_layer_flags_t)(flags & ~LV_DRAW_LAYER_FLAG_HAS_ALPHA);
            }
        }

        if (layer_type == LV_LAYER_TYPE_SIMPLE) {
            flags = (lv_draw_layer_flags_t)(flags | LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE);
        }

        lv_draw_layer_ctx_t *layer_ctx = lv_draw_layer_create(draw_ctx, &layer_area_full, flags);
        if (layer_ctx == NULL) {
            LV_LOG_WARN("Couldn't create a new layer context");
            return;
        }

        lv_point_t pivot = {
            .x = lv_obj_get_style_transform_pivot_x(obj, 0),
            .y = lv_obj_get_style_transform_pivot_y(obj, 0)
        };

        lv_draw_img_dsc_t draw_dsc;
        lv_draw_img_dsc_init(&draw_dsc);
        draw_dsc.opa = opa;
        draw_dsc.angle = lv_obj_get_style_transform_angle(obj, 0);
        if (draw_dsc.angle > 3600) {
            draw_dsc.angle -= 3600;
        } else if (draw_dsc.angle < 0) {
            draw_dsc.angle += 3600;
        }

        draw_dsc.zoom = lv_obj_get_style_transform_zoom(obj, 0);
        draw_dsc.blend_mode = lv_obj_get_style_blend_mode(obj, 0);
        draw_dsc.antialias = disp_refr->driver->antialiasing;

        if (flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) {
            layer_ctx->area_act = layer_ctx->area_full;
            layer_ctx->area_act.y2 = layer_ctx->area_act.y1 + layer_ctx->max_row_with_no_alpha - 1;
            if (layer_ctx->area_act.y2 > layer_ctx->area_full.y2) {
                layer_ctx->area_act.y2 = layer_ctx->area_full.y2;
            }
        }

        while (layer_ctx->area_act.y1 <= layer_area_full.y2) {
            if (flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) {
                layer_alpha_test(obj, draw_ctx, layer_ctx, flags);
            }

            lv_obj_redraw(draw_ctx, obj);

            draw_dsc.pivot.x = obj->coords.x1 + pivot.x - draw_ctx->buf_area->x1;
            draw_dsc.pivot.y = obj->coords.y1 + pivot.y - draw_ctx->buf_area->y1;

            // 使用LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE它也应该进入下一个块
            lv_draw_layer_blend(draw_ctx, layer_ctx, &draw_dsc);

            if ((flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) == 0) {
                break;
            }

            layer_ctx->area_act.y1 = layer_ctx->area_act.y2 + 1;
            layer_ctx->area_act.y2 = layer_ctx->area_act.y1 + layer_ctx->max_row_with_no_alpha - 1;
        }

        lv_draw_layer_destroy(draw_ctx, layer_ctx);
    }
}

static uint32_t get_max_row(lv_disp_t *disp, lv_coord_t area_w, lv_coord_t area_h)
{
    int32_t max_row = (uint32_t)disp->driver->draw_buf->size / area_w;
    if (max_row > area_h) {
        max_row = area_h;
    }

    if (disp_refr->driver->rounder_cb) {
        lv_area_t tmp;
        tmp.x1 = 0;
        tmp.x2 = 0;
        tmp.y1 = 0;

        lv_coord_t h_tmp = max_row;
        do {
            tmp.y2 = h_tmp - 1;
            disp_refr->driver->rounder_cb(disp_refr->driver, &tmp);

            if (lv_area_get_height(&tmp) <= max_row) {
                break;
            }

            h_tmp--;
        } while(h_tmp > 0);

        if (h_tmp <= 0) {
            LV_LOG_WARN("Can't set draw_buf height using the round function. (Wrong round_cb or to small draw_buf)");
            return 0;
        } else {
            max_row = tmp.y2 + 1;
        }
    }

    return max_row;
}

static void draw_buf_rotate_180(lv_disp_drv_t *drv, lv_area_t *area, lv_color_t *color_p)
{
    lv_coord_t area_w = lv_area_get_width(area);
    lv_coord_t area_h = lv_area_get_height(area);
    uint32_t total = area_w * area_h;

    // 交换开始值和结束值
    lv_color_t tmp;
    uint32_t i = total - 1, j = 0;

    while (i > j) {
        tmp = color_p[i];
        color_p[i] = color_p[j];
        color_p[j] = tmp;
        i--;
        j++;
    }

    lv_coord_t tmp_coord;
    tmp_coord = area->y2;
    area->y2 = drv->ver_res - area->y1 - 1;
    area->y1 = drv->ver_res - tmp_coord - 1;
    tmp_coord = area->x2;
    area->x2 = drv->hor_res - area->x1 - 1;
    area->x1 = drv->hor_res - tmp_coord - 1;
}

static LV_ATTRIBUTE_FAST_MEM void draw_buf_rotate_90(bool invert_i, lv_coord_t area_w, lv_coord_t area_h, lv_color_t *orig_color_p, lv_color_t *rot_buf)
{
    uint32_t invert = (area_w * area_h) - 1;
    uint32_t initial_i = ((area_w - 1) * area_h);

    for (lv_coord_t y = 0; y < area_h; y++) {
        uint32_t i = initial_i + y;
        if (invert_i) {
            i = invert - i;
        }

        for (lv_coord_t x = 0; x < area_w; x++) {
            rot_buf[i] = *(orig_color_p++);
            if (invert_i) {
                i += area_h;
            } else {
                i -= area_h;
            }
        }
    }
}

static inline void draw_buf_rotate4(lv_color_t *a, lv_color_t *b, lv_color_t *c, lv_color_t *d)
{
    lv_color_t tmp;
    tmp = *a;
    *a = *b;
    *b = *c;
    *c = *d;
    *d = tmp;
}

static void draw_buf_rotate_90_sqr(bool is_270, lv_coord_t w, lv_color_t *color_p)
{
    for (lv_coord_t i = 0; i < w/2; i++) {
        for (lv_coord_t j = 0; j < (w + 1)/2; j++) {
            lv_coord_t inv_i = (w - 1) - i;
            lv_coord_t inv_j = (w - 1) - j;
            if (is_270) {
                draw_buf_rotate4(&color_p[i * w + j], &color_p[inv_j * w + i], &color_p[inv_i * w + inv_j], &color_p[j * w + inv_i]);
            } else {
                draw_buf_rotate4(&color_p[i * w + j], &color_p[j * w + inv_i], &color_p[inv_i * w + inv_j], &color_p[inv_j * w + i]);
            }
        }
    }
}

static void draw_buf_rotate(lv_area_t *area, lv_color_t *color_p)
{
    lv_disp_drv_t *drv = disp_refr->driver;
    if (disp_refr->driver->full_refresh && drv->sw_rotate) {
        LV_LOG_ERROR("cannot rotate a full refreshed display!");
        return;
    }

    if (drv->rotated == LV_DISP_ROT_180) {
        draw_buf_rotate_180(drv, area, color_p);
        call_flush_cb(drv, area, color_p);
    } else if ((drv->rotated == LV_DISP_ROT_90) || (drv->rotated == LV_DISP_ROT_270)) {
        // 分配一个临时缓冲区来存储旋转的图像
        lv_color_t *rot_buf = NULL;
        lv_disp_draw_buf_t *draw_buf = lv_disp_get_draw_buf(disp_refr);
        lv_coord_t area_w = lv_area_get_width(area);
        lv_coord_t area_h = lv_area_get_height(area);

        // 确定一次可以旋转的最大行数
        lv_coord_t max_row = LV_MIN((lv_coord_t)((LV_DISP_ROT_MAX_BUF/sizeof(lv_color_t)) / area_w), area_h);
        lv_coord_t init_y_off;
        init_y_off = area->y1;
        if (drv->rotated == LV_DISP_ROT_90) {
            area->y2 = drv->ver_res - area->x1 - 1;
            area->y1 = area->y2 - area_w + 1;
        } else {
            area->y1 = area->x1;
            area->y2 = area->y1 + area_w - 1;
        }

        // 分块旋转屏幕，每一个刷新一次
        lv_coord_t row = 0;
        while (row < area_h) {
            lv_coord_t height = LV_MIN(max_row, area_h-row);
            draw_buf->flushing = 1;
            if ((row == 0) && (area_h >= area_w)) {
                // 将初始区域旋转为正方形
                height = area_w;
                draw_buf_rotate_90_sqr(drv->rotated == LV_DISP_ROT_270, area_w, color_p);
                if (drv->rotated == LV_DISP_ROT_90) {
                    area->x1 = init_y_off;
                    area->x2 = init_y_off + area_w - 1;
                } else {
                    area->x2 = drv->hor_res - 1 - init_y_off;
                    area->x1 = area->x2 - area_w + 1;
                }
            } else {
                // 使用最大缓冲区大小旋转其他区域
                if (rot_buf == NULL) {
                    rot_buf = (lv_color_t *)lv_malloc(LV_DISP_ROT_MAX_BUF);
                }

                draw_buf_rotate_90(drv->rotated == LV_DISP_ROT_270, area_w, height, color_p, rot_buf);

                if (drv->rotated == LV_DISP_ROT_90) {
                    area->x1 = init_y_off + row;
                    area->x2 = init_y_off + row + height - 1;
                } else {
                    area->x2 = drv->hor_res - 1 - init_y_off - row;
                    area->x1 = area->x2 - height + 1;
                }
            }

            // 原始部分(当前区域的块)在这里被拆分为更多部分。在旋转的最后一部分设置原始last_part标志
            if (((row + height) >= area_h) && draw_buf->last_area && draw_buf->last_part) {
                draw_buf->flushing_last = 1;
            } else {
                draw_buf->flushing_last = 0;
            }

            // 将完成的区域刷新到显示器
            call_flush_cb(drv, area, rot_buf == NULL ? color_p : rot_buf);

            // FIXME: 旋转强制传统行为，其中渲染和刷新是连续完成的
            while (draw_buf->flushing) {
                if (drv->wait_cb) {
                    drv->wait_cb(drv);
                }
            }

            color_p += area_w * height;
            row += height;
        }

        // 如有必要，最后释放分配的缓冲区
        if (rot_buf != NULL) {
            lv_free(rot_buf);
        }
    }
}

static void draw_buf_flush(lv_disp_t *disp)
{
    lv_disp_draw_buf_t *draw_buf = lv_disp_get_draw_buf(disp_refr);

    lv_draw_ctx_t *draw_ctx = disp->driver->draw_ctx;
    if (draw_ctx->wait_for_finish) {
        draw_ctx->wait_for_finish(draw_ctx);
    }

    if (draw_buf->buf1 && draw_buf->buf2) {
        while (draw_buf->flushing) {
            if (disp_refr->driver->wait_cb) {
                disp_refr->driver->wait_cb(disp_refr->driver);
            }
        }

        if (disp_refr->driver->screen_transp) {
            if (disp_refr->driver->clear_cb) {
                disp_refr->driver->clear_cb(disp_refr->driver, (uint8_t *)disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size);
            } else {
                lv_memzero(disp_refr->driver->draw_buf->buf_act, disp_refr->driver->draw_buf->size * LV_IMG_PX_SIZE_ALPHA_BYTE);
            }
        }
    }

    draw_buf->flushing = 1;

    if (disp_refr->driver->draw_buf->last_area && disp_refr->driver->draw_buf->last_part) {
        draw_buf->flushing_last = 1;
    } else {
        draw_buf->flushing_last = 0;
    }

    bool flushing_last = draw_buf->flushing_last;

    if (disp->driver->flush_cb) {
        // 如有必要，将缓冲区调整为显示器的原始方向
        if ((disp->driver->rotated != LV_DISP_ROT_NONE) && disp->driver->sw_rotate) {
            draw_buf_rotate(draw_ctx->buf_area, (lv_color_t *)draw_ctx->buf);
        } else {
            call_flush_cb(disp->driver, draw_ctx->buf_area, (lv_color_t *)draw_ctx->buf);
        }
    }

    if (draw_buf->buf1 && draw_buf->buf2 && (!disp->driver->direct_mode || flushing_last)) {
        if (draw_buf->buf_act == draw_buf->buf1) {
            draw_buf->buf_act = draw_buf->buf2;
        } else {
            draw_buf->buf_act = draw_buf->buf1;
        }
    }
}

static void call_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    REFR_TRACE("Calling flush_cb on (%d;%d)(%d;%d) area with %p image pointer", area->x1, area->y1, area->x2, area->y2, (void *)color_p);

    lv_area_t offset_area = {
        .x1 = (lv_coord_t)(area->x1 + drv->offset_x),
        .y1 = (lv_coord_t)(area->y1 + drv->offset_y),
        .x2 = (lv_coord_t)(area->x2 + drv->offset_x),
        .y2 = (lv_coord_t)(area->y2 + drv->offset_y)
    };

    if (drv->draw_ctx->buffer_convert) {
        drv->draw_ctx->buffer_convert(drv->draw_ctx);
    }

    drv->flush_cb(drv, &offset_area, color_p);
}

#if LV_USE_PERF_MONITOR
static void perf_monitor_init(perf_monitor_t *_perf_monitor)
{
    LV_ASSERT_NULL(_perf_monitor);
    _perf_monitor->elaps_sum = 0;
    _perf_monitor->fps_sum_all = 0;
    _perf_monitor->fps_sum_cnt = 0;
    _perf_monitor->frame_cnt = 0;
    _perf_monitor->perf_last_time = 0;
    _perf_monitor->perf_label = NULL;
}
#endif

#if LV_USE_MEM_MONITOR
static void mem_monitor_init(mem_monitor_t *_mem_monitor)
{
    LV_ASSERT_NULL(_mem_monitor);
    _mem_monitor->mem_last_time = 0;
    _mem_monitor->mem_label = NULL;
}
#endif
