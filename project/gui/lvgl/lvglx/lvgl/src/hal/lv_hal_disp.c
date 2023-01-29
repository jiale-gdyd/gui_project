#include <stdint.h>
#include <stddef.h>

#include "lv_hal.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_mem.h"
#include "../core/lv_obj.h"
#include "../core/lv_refr.h"
#include "../core/lv_theme.h"
#include "../misc/lv_assert.h"
#include "../draw/sdl/lv_draw_sdl.h"
#include "../draw/sw/lv_draw_sw.h"

#if LV_USE_GPU_STM32_DMA2D
#include "../draw/stm32_dma2d/lv_gpu_stm32_dma2d.h"
#endif

#if LV_USE_GPU_GD32_IPA
#include "../draw/gd32_ipa/lv_gpu_gd32_ipa.h"
#endif

#if LV_USE_GPU_SWM341_DMA2D
#include "../draw/swm341_dma2d/lv_gpu_swm341_dma2d.h"
#endif

#if LV_USE_GPU_ARM2D
#include "../draw/arm2d/lv_gpu_arm2d.h"
#endif

#if LV_USE_GPU_NXP_VG_LITE
#include "../draw/nxp/vglite/lv_draw_vglite.h"
#endif

#if LV_USE_GPU_NXP_PXP
#include "../draw/nxp/pxp/lv_draw_pxp.h"
#endif

#if LV_USE_THEME_DEFAULT
#include "../themes/default/lv_theme_default.h"
#endif

static lv_disp_t *disp_def;
static lv_obj_tree_walk_res_t invalidate_layout_cb(lv_obj_t *obj, void *user_data);

void lv_disp_drv_init(lv_disp_drv_t *driver)
{
    lv_memzero(driver, sizeof(lv_disp_drv_t));

    driver->hor_res = 320;
    driver->ver_res = 240;
    driver->physical_hor_res = -1;
    driver->physical_ver_res = -1;
    driver->offset_x = 0;
    driver->offset_y = 0;
    driver->antialiasing = LV_COLOR_DEPTH > 8 ? 1: 0;
    driver->screen_transp = 0;
    driver->dpi = LV_DPI_DEF;
    driver->color_chroma_key = LV_COLOR_CHROMA_KEY;

#if LV_COLOR_DEPTH == 1
    driver->color_format = LV_COLOR_FORMAT_L1;
#elif LV_COLOR_DEPTH == 8
    driver->color_format = LV_COLOR_FORMAT_L8;
#else
    driver->color_format = LV_COLOR_FORMAT_NATIVE;
#endif

#if LV_USE_GPU_STM32_DMA2D
    driver->draw_ctx_init = lv_draw_stm32_dma2d_ctx_init;
    driver->draw_ctx_deinit = lv_draw_stm32_dma2d_ctx_deinit;
    driver->draw_ctx_size = sizeof(lv_draw_stm32_dma2d_ctx_t);
#elif LV_USE_GPU_GD32_IPA
    driver->draw_ctx_init = lv_draw_gd32_ipa_ctx_init;
    driver->draw_ctx_deinit = lv_draw_gd32_ipa_ctx_deinit;
    driver->draw_ctx_size = sizeof(lv_draw_gd32_ipa_ctx_t);
#elif LV_USE_GPU_SWM341_DMA2D
    driver->draw_ctx_init = lv_draw_swm341_dma2d_ctx_init;
    driver->draw_ctx_deinit = lv_draw_swm341_dma2d_ctx_deinit;
    driver->draw_ctx_size = sizeof(lv_draw_swm341_dma2d_ctx_t);
#elif LV_USE_GPU_NXP_VG_LITE
    driver->draw_ctx_init = lv_draw_vglite_ctx_init;
    driver->draw_ctx_deinit = lv_draw_vglite_ctx_deinit;
    driver->draw_ctx_size = sizeof(lv_draw_vglite_ctx_t);
#elif LV_USE_GPU_NXP_PXP
    driver->draw_ctx_init = lv_draw_pxp_ctx_init;
    driver->draw_ctx_deinit = lv_draw_pxp_ctx_deinit;
    driver->draw_ctx_size = sizeof(lv_draw_pxp_ctx_t);
#elif LV_USE_DRAW_SDL
    driver->draw_ctx_init = lv_draw_sdl_init_ctx;
    driver->draw_ctx_deinit = lv_draw_sdl_deinit_ctx;
    driver->draw_ctx_size = sizeof(lv_draw_sdl_ctx_t);
#elif LV_USE_GPU_ARM2D
    driver->draw_ctx_init = lv_draw_arm2d_ctx_init;
    driver->draw_ctx_deinit = lv_draw_arm2d_ctx_deinit;
    driver->draw_ctx_size = sizeof(lv_draw_arm2d_ctx_t);
#else
    driver->draw_ctx_init = lv_draw_sw_init_ctx;
    driver->draw_ctx_deinit = lv_draw_sw_deinit_ctx;
    driver->draw_ctx_size = sizeof(lv_draw_sw_ctx_t);
#endif
}

void lv_disp_draw_buf_init(lv_disp_draw_buf_t *draw_buf, void *buf1, void *buf2, uint32_t size_in_px_cnt)
{
    lv_memzero(draw_buf, sizeof(lv_disp_draw_buf_t));

    draw_buf->buf1 = buf1;
    draw_buf->buf2 = buf2;
    draw_buf->buf_act = draw_buf->buf1;
    draw_buf->size = size_in_px_cnt;
}

lv_disp_t *lv_disp_drv_register(lv_disp_drv_t *driver)
{
    lv_disp_t *disp = (lv_disp_t *)_lv_ll_ins_head(&LV_GC_ROOT(_lv_disp_ll));
    LV_ASSERT_MALLOC(disp);
    if (!disp) {
        return NULL;
    }

    if (driver->draw_ctx == NULL) {
        lv_draw_ctx_t *draw_ctx = (lv_draw_ctx_t *)lv_malloc(driver->draw_ctx_size);
        LV_ASSERT_MALLOC(draw_ctx);
        if (draw_ctx == NULL) {
            return NULL;
        }

        driver->draw_ctx_init(driver, draw_ctx);
        driver->draw_ctx = draw_ctx;
    }

    driver->draw_ctx->color_format = driver->color_format;
    driver->draw_ctx->render_with_alpha = driver->screen_transp;

    lv_memzero(disp, sizeof(lv_disp_t));
    disp->driver = driver;
    disp->inv_en_cnt = 1;

    lv_disp_t *disp_def_tmp = disp_def;
    disp_def = disp;                    // 临时更改默认屏幕以在新显示器上创建默认屏幕

    // 创建刷新计时器
    disp->refr_timer = lv_timer_create(_lv_disp_refr_timer, LV_DEF_REFR_PERIOD, disp);
    LV_ASSERT_MALLOC(disp->refr_timer);
    if (disp->refr_timer == NULL) {
        lv_free(disp);
        return NULL;
    }

    if (driver->full_refresh && (driver->draw_buf->size < ((uint32_t)driver->hor_res * driver->ver_res))) {
        driver->full_refresh = 0;
        LV_LOG_WARN("full_refresh requires at least screen sized draw buffer(s)");
    }

    disp->bg_color = lv_color_white();
    disp->bg_opa = LV_OPA_COVER;

#if LV_USE_THEME_DEFAULT
    if (lv_theme_default_is_inited() == false) {
        disp->theme = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    } else {
        disp->theme = lv_theme_default_get();
    }
#endif

    disp->act_scr = lv_obj_create(NULL);        // 在显示器上创建默认屏幕
    disp->top_layer = lv_obj_create(NULL);      // 在显示器上创建顶层
    disp->sys_layer = lv_obj_create(NULL);      // 在显示器上创建sys层
    lv_obj_remove_style_all(disp->top_layer);
    lv_obj_remove_style_all(disp->sys_layer);
    lv_obj_clear_flag(disp->top_layer, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(disp->sys_layer, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_scrollbar_mode(disp->top_layer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(disp->sys_layer, LV_SCROLLBAR_MODE_OFF);

    lv_obj_invalidate(disp->act_scr);

    // 恢复默认显示
    disp_def = disp_def_tmp;
    if (disp_def == NULL) {
        // 初始化默认显示
        disp_def = disp;
    }

    // 确保屏幕会在启动时立即刷新
    lv_timer_ready(disp->refr_timer);

    return disp;
}

void lv_disp_drv_update(lv_disp_t *disp, lv_disp_drv_t *new_drv)
{
    disp->driver = new_drv;

    if (disp->driver->full_refresh && (disp->driver->draw_buf->size < ((uint32_t)disp->driver->hor_res * disp->driver->ver_res))) {
        disp->driver->full_refresh = 0;
        LV_LOG_WARN("full_refresh requires at least screen sized draw buffer(s)");
    }

    uint32_t i;
    lv_coord_t w = lv_disp_get_hor_res(disp);
    lv_coord_t h = lv_disp_get_ver_res(disp);
    
    for (i = 0; i < disp->screen_cnt; i++) {
        lv_area_t prev_coords;
        lv_obj_get_coords(disp->screens[i], &prev_coords);
        lv_area_set_width(&disp->screens[i]->coords, w);
        lv_area_set_height(&disp->screens[i]->coords, h);
        lv_event_send(disp->screens[i], LV_EVENT_SIZE_CHANGED, &prev_coords);
    }

    // 这个方法通常在方向改变时调用，因此屏幕现在是不同的尺寸。该对象使其先前的区域无效。该区域现在超出了屏幕区域，因此我们重置所有无效区域并仅使活动屏幕的新区域无效
    lv_memzero(disp->inv_areas, sizeof(disp->inv_areas));
    lv_memzero(disp->inv_area_joined, sizeof(disp->inv_area_joined));
    
    disp->inv_p = 0;
    if (disp->act_scr != NULL) {
        lv_obj_invalidate(disp->act_scr);
    }
    lv_obj_tree_walk(NULL, invalidate_layout_cb, NULL);

    if (disp->driver->drv_update_cb) {
        disp->driver->drv_update_cb(disp->driver);
    }
}

void lv_disp_remove(lv_disp_t *disp)
{
    bool was_default = false;
    if (disp == lv_disp_get_default()) {
        was_default = true;
    }

    // 分离输入设备
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev) {
        if (indev->driver->disp == disp) {
            indev->driver->disp = NULL;
        }
        indev = lv_indev_get_next(indev);
    }

    // 删除屏幕和其他对象
    if (disp->sys_layer) {
        lv_obj_del(disp->sys_layer);
        disp->sys_layer = NULL;
    }

    if (disp->top_layer) {
        lv_obj_del(disp->top_layer);
        disp->top_layer = NULL;
    }

    while (disp->screen_cnt != 0) {
        // 删除屏幕截图
        lv_obj_del(disp->screens[0]);
    }

    _lv_ll_remove(&LV_GC_ROOT(_lv_disp_ll), disp);
    if (disp->refr_timer) {
        lv_timer_del(disp->refr_timer);
    }
    lv_free(disp);

    if (was_default) {
        lv_disp_set_default((lv_disp_t *)_lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll)));
    }
}

void lv_disp_set_default(lv_disp_t *disp)
{
    disp_def = disp;
}

lv_disp_t *lv_disp_get_default(void)
{
    return disp_def;
}

lv_coord_t lv_disp_get_hor_res(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return 0;
    } else {
        switch (disp->driver->rotated) {
            case LV_DISP_ROT_90:
            case LV_DISP_ROT_270:
                return disp->driver->ver_res;

            default:
                return disp->driver->hor_res;
        }
    }
}

lv_coord_t lv_disp_get_ver_res(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return 0;
    } else {
        switch (disp->driver->rotated) {
            case LV_DISP_ROT_90:
            case LV_DISP_ROT_270:
                return disp->driver->hor_res;

            default:
                return disp->driver->ver_res;
        }
    }
}

lv_coord_t lv_disp_get_physical_hor_res(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return 0;
    } else {
        switch (disp->driver->rotated) {
            case LV_DISP_ROT_90:
            case LV_DISP_ROT_270:
                return disp->driver->physical_ver_res > 0 ? disp->driver->physical_ver_res : disp->driver->ver_res;

            default:
                return disp->driver->physical_hor_res > 0 ? disp->driver->physical_hor_res : disp->driver->hor_res;
        }
    }
}

lv_coord_t lv_disp_get_physical_ver_res(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return 0;
    } else {
        switch (disp->driver->rotated) {
            case LV_DISP_ROT_90:
            case LV_DISP_ROT_270:
                return disp->driver->physical_hor_res > 0 ? disp->driver->physical_hor_res : disp->driver->hor_res;

            default:
                return disp->driver->physical_ver_res > 0 ? disp->driver->physical_ver_res : disp->driver->ver_res;
        }
    }
}

lv_coord_t lv_disp_get_offset_x(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return 0;
    } else {
        switch (disp->driver->rotated) {
            case LV_DISP_ROT_90:
                return disp->driver->offset_y;

            case LV_DISP_ROT_180:
                return lv_disp_get_physical_hor_res(disp) - disp->driver->offset_x;

            case LV_DISP_ROT_270:
                return lv_disp_get_physical_hor_res(disp) - disp->driver->offset_y;

            default:
                return disp->driver->offset_x;
        }
    }
}

lv_coord_t lv_disp_get_offset_y(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return 0;
    } else {
        switch (disp->driver->rotated) {
            case LV_DISP_ROT_90:
                return disp->driver->offset_x;

            case LV_DISP_ROT_180:
                return lv_disp_get_physical_ver_res(disp) - disp->driver->offset_y;

            case LV_DISP_ROT_270:
                return lv_disp_get_physical_ver_res(disp) - disp->driver->offset_x;

            default:
                return disp->driver->offset_y;
        }
    }
}

bool lv_disp_get_antialiasing(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return false;
    }

    return disp->driver->antialiasing ? true : false;
}

lv_coord_t lv_disp_get_dpi(const lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        // 不要返回0因为它可能是一个分隔符
        return LV_DPI_DEF;
    }

    return disp->driver->dpi;
}

LV_ATTRIBUTE_FLUSH_READY void lv_disp_flush_ready(lv_disp_drv_t *disp_drv)
{
    disp_drv->draw_buf->flushing = 0;
    disp_drv->draw_buf->flushing_last = 0;
}

LV_ATTRIBUTE_FLUSH_READY bool lv_disp_flush_is_last(lv_disp_drv_t *disp_drv)
{
    return disp_drv->draw_buf->flushing_last;
}

lv_disp_t *lv_disp_get_next(lv_disp_t *disp)
{
    if (disp == NULL) {
        return (lv_disp_t *)_lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
    } else {
        return (lv_disp_t *)_lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), disp);
    }
}

lv_disp_draw_buf_t *lv_disp_get_draw_buf(lv_disp_t *disp)
{
    return disp->driver->draw_buf;
}

void lv_disp_set_rotation(lv_disp_t *disp, lv_disp_rot_t rotation)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return;
    }

    disp->driver->rotated = rotation;
    lv_disp_drv_update(disp, disp->driver);
}

lv_disp_rot_t lv_disp_get_rotation(lv_disp_t *disp)
{
    if (disp == NULL) {
        disp = lv_disp_get_default();
    }

    if (disp == NULL) {
        return LV_DISP_ROT_NONE;
    }

    return (lv_disp_rot_t)(disp->driver->rotated);
}

static lv_obj_tree_walk_res_t invalidate_layout_cb(lv_obj_t *obj, void *user_data)
{
    LV_UNUSED(user_data);
    lv_obj_mark_layout_as_dirty(obj);

    return LV_OBJ_TREE_WALK_NEXT;
}
