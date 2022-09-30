#include "benchmark.h"

#if LV_USE_DEMO_BENCHMARK

#define RND_NUM                     64
#define SCENE_TIME                  1000
#define RENDER_REPEAT_CNT           50
#define ANIM_TIME_MIN               ((2 * SCENE_TIME) / 10)
#define ANIM_TIME_MAX               (SCENE_TIME)
#define OBJ_NUM                     8
#define OBJ_SIZE_MIN                (LV_MAX(LV_DPI_DEF / 20, 5))
#define OBJ_SIZE_MAX                (LV_HOR_RES / 2)
#define RADIUS                      LV_MAX(LV_DPI_DEF / 15, 2)
#define BORDER_WIDTH                LV_MAX(LV_DPI_DEF / 40, 1)
#define SHADOW_WIDTH_SMALL          LV_MAX(LV_DPI_DEF / 15, 5)
#define SHADOW_OFS_X_SMALL          LV_MAX(LV_DPI_DEF / 20, 2)
#define SHADOW_OFS_Y_SMALL          LV_MAX(LV_DPI_DEF / 20, 2)
#define SHADOW_SPREAD_SMALL         LV_MAX(LV_DPI_DEF / 30, 2)
#define SHADOW_WIDTH_LARGE          LV_MAX(LV_DPI_DEF / 5,  10)
#define SHADOW_OFS_X_LARGE          LV_MAX(LV_DPI_DEF / 10, 5)
#define SHADOW_OFS_Y_LARGE          LV_MAX(LV_DPI_DEF / 10, 5)
#define SHADOW_SPREAD_LARGE         LV_MAX(LV_DPI_DEF / 30, 2)
#define IMG_WIDH                    100
#define IMG_HEIGHT                  100
#define IMG_NUM                     LV_MAX((LV_HOR_RES * LV_VER_RES) / 5 / IMG_WIDH / IMG_HEIGHT, 1)
#define IMG_ZOOM_MIN                128
#define IMG_ZOOM_MAX                (256 + 64)
#define TXT                         "hello world\nit is a multi line text to test\nthe performance of text rendering"
#define LINE_WIDTH                  LV_MAX(LV_DPI_DEF / 50, 2)
#define LINE_POINT_NUM              16
#define LINE_POINT_DIFF_MIN         (LV_DPI_DEF / 10)
#define LINE_POINT_DIFF_MAX         LV_MAX(LV_HOR_RES / (LINE_POINT_NUM + 2), LINE_POINT_DIFF_MIN * 2)
#define ARC_WIDTH_THIN              LV_MAX(LV_DPI_DEF / 50, 2)
#define ARC_WIDTH_THICK             LV_MAX(LV_DPI_DEF / 10, 5)

#ifndef dimof
#define dimof(__array)              (sizeof(__array) / sizeof(__array[0]))
#endif

typedef struct {
    const char *name;
    void (*create_cb)(void);
    uint32_t   time_sum_normal;
    uint32_t   time_sum_opa;
    uint32_t   refr_cnt_normal;
    uint32_t   refr_cnt_opa;
    uint32_t   fps_normal;
    uint32_t   fps_opa;
    uint8_t    weight;
} scene_dsc_t;

static lv_demo_benchmark_mode_t mode;
static uint32_t disp_ori_timer_period;
static uint32_t anim_ori_timer_period;

static lv_style_t style_common;
static bool scene_with_opa = true;
static uint32_t render_start_time;
static uint32_t last_flush_cb_call;

static void (*flush_cb_ori)(lv_disp_drv_t *, const lv_area_t *, lv_color_t *);

static void fall_anim(lv_obj_t *obj);
static void txt_create(lv_style_t *style);
static void arc_create(lv_style_t *style);
static void line_create(lv_style_t *style);
static void rect_create(lv_style_t *style);
static void monitor_cb(lv_disp_drv_t *drv, uint32_t time, uint32_t px);
static void img_create(lv_style_t *style, const void * src, bool rotate, bool zoom, bool aa);

static void rnd_reset(void);
static int32_t rnd_next(int32_t min, int32_t max);

static void benchmark_init(void);
static void show_scene_report(void);
static lv_res_t load_next_scene(void);
static void calc_scene_statistics(void);
static void next_scene_timer_cb(lv_timer_t *timer);
static void single_scene_finsih_timer_cb(lv_timer_t *timer);

static void generate_report(void);
static void render_start_cb(lv_disp_drv_t *drv);
static void dummy_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *colors);


static void rectangle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_bg_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void rectangle_rounded_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void rectangle_circle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_rounded_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_circle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, LV_RADIUS_CIRCLE);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_top_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_TOP);
    rect_create(&style_common);
}

static void border_left_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_LEFT);
    rect_create(&style_common);
}

static void border_top_left_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP);
    rect_create(&style_common);
}

static void border_left_right_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT);
    rect_create(&style_common);
}

static void border_top_bottom_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    rect_create(&style_common);
}

static void shadow_small_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, scene_with_opa ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_SMALL);
    rect_create(&style_common);
}

static void shadow_small_ofs_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, scene_with_opa ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_SMALL);
    lv_style_set_shadow_ofs_x(&style_common, SHADOW_OFS_X_SMALL);
    lv_style_set_shadow_ofs_y(&style_common, SHADOW_OFS_Y_SMALL);
    lv_style_set_shadow_spread(&style_common, SHADOW_SPREAD_SMALL);
    rect_create(&style_common);
}

static void shadow_large_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, scene_with_opa ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_LARGE);
    rect_create(&style_common);
}

static void shadow_large_ofs_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, scene_with_opa ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_LARGE);
    lv_style_set_shadow_ofs_x(&style_common, SHADOW_OFS_X_LARGE);
    lv_style_set_shadow_ofs_y(&style_common, SHADOW_OFS_Y_LARGE);
    lv_style_set_shadow_spread(&style_common, SHADOW_SPREAD_LARGE);
    rect_create(&style_common);
}

static void img_rgb_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, false, false);
}

static void img_argb_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, false, false, false);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, false, false);
#endif
}

static void img_ckey_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_chroma_keyed, false, false, false);
}

static void img_index_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_indexed16, false, false, false);
}

static void img_alpha_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_alpha16, false, false, false);
}

static void img_rgb_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, false, false);
}

static void img_argb_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, false, false, false);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, false, false);
#endif
}

static void img_ckey_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_chroma_keyed, false, false, false);
}

static void img_index_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_indexed16, false, false, false);
}

static void img_rgb_rot_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, true, false, false);
}

static void img_rgb_rot_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, true, false, true);
}

static void img_argb_rot_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, true, false, false);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, true, false, false);
#endif
}

static void img_argb_rot_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, true, false, true);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, true, false, true);
#endif
}

static void img_rgb_zoom_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, true, false);
}

static void img_rgb_zoom_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, true, true);
}

static void img_argb_zoom_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, false, true, false);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, true, false);
#endif
}

static void img_argb_zoom_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, false, true, true);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, true, true);
#endif
}

static void txt_small_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_small(NULL));
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);
}

static void txt_medium_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_normal(NULL));
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);
}

static void txt_large_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_large(NULL));
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);
}

static void txt_small_compr_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, &lv_font_benchmark_montserrat_12_compr_az);
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);
}

static void txt_medium_compr_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, &lv_font_benchmark_montserrat_16_compr_az);
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);
}

static void txt_large_compr_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, &lv_font_benchmark_montserrat_28_compr_az);
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);
}

static void line_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_line_width(&style_common, LINE_WIDTH);
    lv_style_set_line_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    line_create(&style_common);
}

static void arc_think_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_arc_width(&style_common, ARC_WIDTH_THIN);
    lv_style_set_arc_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    arc_create(&style_common);
}

static void arc_thick_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_arc_width(&style_common, ARC_WIDTH_THICK);
    lv_style_set_arc_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    arc_create(&style_common);
}

static void sub_rectangle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    rect_create(&style_common);
}

static void sub_border_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    rect_create(&style_common);
}

static void sub_shadow_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, scene_with_opa ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_SMALL);
    lv_style_set_shadow_spread(&style_common, SHADOW_WIDTH_SMALL);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    rect_create(&style_common);
}

static void sub_img_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);

#if LV_DEMO_BENCHMARK_RGB565A8 && (LV_COLOR_DEPTH == 16)
    img_create(&style_common, &img_benchmark_cogwheel_rgb565a8, false, false, false);
#else
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, false, false);
#endif
}

static void sub_line_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_line_width(&style_common, LINE_WIDTH);
    lv_style_set_line_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    line_create(&style_common);
}

static void sub_arc_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_arc_width(&style_common, ARC_WIDTH_THICK);
    lv_style_set_arc_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    arc_create(&style_common);
}

static void sub_text_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_normal(NULL));
    lv_style_set_text_opa(&style_common, scene_with_opa ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    txt_create(&style_common);
}

static scene_dsc_t scenes[] = {
    {.name = "Rectangle",                      .create_cb = rectangle_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 30},
    {.name = "Rectangle rounded",              .create_cb = rectangle_rounded_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},
    {.name = "Circle",                         .create_cb = rectangle_circle_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Border",                         .create_cb = border_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},
    {.name = "Border rounded",                 .create_cb = border_rounded_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 30},
    {.name = "Circle border",                  .create_cb = border_circle_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Border top",                     .create_cb = border_top_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Border left",                    .create_cb = border_left_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Border top + left",              .create_cb = border_top_left_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Border left + right",            .create_cb = border_left_right_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Border top + bottom",            .create_cb = border_top_bottom_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},

    {.name = "Shadow small",                   .create_cb = shadow_small_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Shadow small offset",            .create_cb = shadow_small_ofs_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Shadow large",                   .create_cb = shadow_large_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Shadow large offset",            .create_cb = shadow_large_ofs_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},

    {.name = "Image RGB",                      .create_cb = img_rgb_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},
    {.name = "Image ARGB",                     .create_cb = img_argb_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},
    {.name = "Image chorma keyed",             .create_cb = img_ckey_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Image indexed",                  .create_cb = img_index_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Image alpha only",               .create_cb = img_alpha_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},

    {.name = "Image RGB recolor",              .create_cb = img_rgb_recolor_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Image ARGB recolor",             .create_cb = img_argb_recolor_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},
    {.name = "Image chorma keyed recolor",     .create_cb = img_ckey_recolor_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Image indexed recolor",          .create_cb = img_index_recolor_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},

    {.name = "Image RGB rotate",               .create_cb = img_rgb_rot_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Image RGB rotate anti aliased",  .create_cb = img_rgb_rot_aa_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Image ARGB rotate",              .create_cb = img_argb_rot_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Image ARGB rotate anti aliased", .create_cb = img_argb_rot_aa_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Image RGB zoom",                 .create_cb = img_rgb_zoom_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Image RGB zoom anti aliased",    .create_cb = img_rgb_zoom_aa_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Image ARGB zoom",                .create_cb = img_argb_zoom_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Image ARGB zoom anti aliased",   .create_cb = img_argb_zoom_aa_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},

    {.name = "Text small",                     .create_cb = txt_small_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},
    {.name = "Text medium",                    .create_cb = txt_medium_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 30},
    {.name = "Text large",                     .create_cb = txt_large_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 20},

    {.name = "Text small compressed",          .create_cb = txt_small_compr_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 3},
    {.name = "Text medium compressed",         .create_cb = txt_medium_compr_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 5},
    {.name = "Text large compressed",          .create_cb = txt_large_compr_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},

    {.name = "Line",                           .create_cb = line_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},

    {.name = "Arc think",                      .create_cb = arc_think_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Arc thick",                      .create_cb = arc_thick_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},

    {.name = "Substr. rectangle",              .create_cb = sub_rectangle_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Substr. border",                 .create_cb = sub_border_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Substr. shadow",                 .create_cb = sub_shadow_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Substr. image",                  .create_cb = sub_img_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Substr. line",                   .create_cb = sub_line_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Substr. arc",                    .create_cb = sub_arc_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},
    {.name = "Substr. text",                   .create_cb = sub_text_cb, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 10},

    {.name = "", .create_cb = NULL, .time_sum_normal = 0, .time_sum_opa = 0, .refr_cnt_normal = 0, .refr_cnt_opa = 0, .fps_normal = 0, .fps_opa = 0, .weight = 0}
};

static lv_obj_t *title;
static uint32_t rnd_act;
static lv_obj_t *scene_bg;
static lv_obj_t *subtitle;
static int32_t scene_act = -1;

static const uint32_t rnd_map[] = {
    0xbd13204f, 0x67d8167f, 0x20211c99, 0xb0a7cc05,
    0x06d5c703, 0xeafb01a7, 0xd0473b5c, 0xc999aaa2,
    0x86f9d5d9, 0x294bdb29, 0x12a3c207, 0x78914d14,
    0x10a30006, 0x6134c7db, 0x194443af, 0x142d1099,
    0x376292d5, 0x20f433c5, 0x074d2a59, 0x4e74c293,
    0x072a0810, 0xdd0f136d, 0x5cca6dbc, 0x623bfdd8,
    0xb645eb2f, 0xbe50894a, 0xc9b56717, 0xe0f912c8,
    0x4f6b5e24, 0xfe44b128, 0xe12d57a8, 0x9b15c9cc,
    0xab2ae1d3, 0xb4dc5074, 0x67d457c8, 0x8e46b00c,
    0xa29a1871, 0xcee40332, 0x80f93aa1, 0x85286096,
    0x09bd6b49, 0x95072088, 0x2093924b, 0x6a27328f,
    0xa796079b, 0xc3b488bc, 0xe29bcce0, 0x07048a4c,
    0x7d81bd99, 0x27aacb30, 0x44fc7a0e, 0xa2382241,
    0x8357a17d, 0x97e9c9cc, 0xad10ff52, 0x9923fc5c,
    0x8f2c840a, 0x20356ba2, 0x7997a677, 0x9a7f1800,
    0x35c7562b, 0xd901fe51, 0x8f4e053d, 0xa5b94923,
};

static void benchmark_init(void)
{
    lv_disp_t *disp = lv_disp_get_default();

    if (mode == LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER) {
        disp->driver->render_start_cb = render_start_cb;
        flush_cb_ori = disp->driver->flush_cb;
        disp->driver->flush_cb = dummy_flush_cb;
    } else if (mode == LV_DEMO_BENCHMARK_MODE_REAL) {
        flush_cb_ori = disp->driver->flush_cb;
        disp->driver->flush_cb = dummy_flush_cb;
    } else if(mode == LV_DEMO_BENCHMARK_MODE_RENDER_ONLY) {
        disp->driver->monitor_cb = monitor_cb;
        flush_cb_ori = disp->driver->flush_cb;
        disp->driver->flush_cb = dummy_flush_cb;
        if (disp->refr_timer) {
            disp_ori_timer_period = disp->refr_timer->period;
            lv_timer_set_period(disp->refr_timer, 2);
        }

        lv_timer_t *anim_timer = lv_anim_get_timer();
        anim_ori_timer_period = anim_timer->period;
        lv_timer_set_period(anim_timer, 2);
    }

    lv_obj_t *scr = lv_scr_act();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    title = lv_label_create(scr);
    lv_obj_set_pos(title, LV_DPI_DEF / 30, LV_DPI_DEF / 30);
    lv_label_set_text(title, "");

    subtitle = lv_label_create(scr);
    lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(subtitle, "");

    scene_bg = lv_obj_create(scr);
    lv_obj_remove_style_all(scene_bg);
    lv_obj_set_size(scene_bg, lv_obj_get_width(scr), lv_obj_get_height(scr) - subtitle->coords.y2 - LV_DPI_DEF / 30);
    lv_obj_align(scene_bg, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_style_init(&style_common);

    lv_obj_update_layout(scr);
}

int lvgl_demo_benchmark(int argc, char *argv[])
{
    benchmark_init();
    mode = LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER;

    if (mode == LV_DEMO_BENCHMARK_MODE_RENDER_ONLY) {
        while (load_next_scene() == LV_RES_OK) {
            uint32_t i;
            for (i = 0; i < RENDER_REPEAT_CNT; i++) {
                uint32_t t = lv_tick_get();
                while (lv_tick_elaps(t) > 20);

                lv_refr_now(NULL);
            }

            calc_scene_statistics();
            show_scene_report();
        }

        generate_report();
    } else {
        lv_timer_t *t = lv_timer_create(next_scene_timer_cb, SCENE_TIME, NULL);
        lv_timer_ready(t);
    }

    return 0;
}

void lvgl_demo_benchmark_run_scene(lv_demo_benchmark_mode_t _mode, uint16_t scene_no)
{
    mode = _mode;
    benchmark_init();

    if (((scene_no >> 1) >= dimof(scenes))) {
        return ;
    }

    scene_with_opa = scene_no & 0x01;
    scene_act = scene_no >> 1;

    lv_obj_clean(scene_bg);

    last_flush_cb_call = 0;
    rnd_reset();
    scenes[scene_act].create_cb();

    lv_label_set_text_fmt(title, "%s%s", scenes[scene_act].name, scene_with_opa ? " + opa" : "");

    if (mode == LV_DEMO_BENCHMARK_MODE_RENDER_ONLY) {
        uint32_t i;
        for (i = 0; i < RENDER_REPEAT_CNT; i++) {
            uint32_t t = lv_tick_get();
            while (lv_tick_elaps(t) < 20);

            lv_refr_now(NULL);
        }

        single_scene_finsih_timer_cb(NULL);
    } else {
        lv_timer_t *t = lv_timer_create(single_scene_finsih_timer_cb, SCENE_TIME, NULL);
        lv_timer_set_repeat_count(t, 1);
    }
}

static void show_scene_report(void)
{
    if (scene_act < 0) {
        return;
    }

    if (scene_with_opa) {
        lv_label_set_text_fmt(subtitle, "Result: %" LV_PRId32 " FPS", scenes[scene_act].fps_opa);
        LV_LOG("Result of \"%s + opa\": %" LV_PRId32 " FPS\n", scenes[scene_act].name, scenes[scene_act].fps_opa);
    } else {
        lv_label_set_text_fmt(subtitle, "Result: %" LV_PRId32 " FPS", scenes[scene_act].fps_normal);
        LV_LOG("Result of \"%s\": %" LV_PRId32 " FPS\n", scenes[scene_act].name, scenes[scene_act].fps_normal);
    }
}

static void calc_scene_statistics(void)
{
    if (scene_act < 0) {
        return;
    }

    if (scene_with_opa) {
        if (scenes[scene_act].time_sum_opa == 0) {
            scenes[scene_act].time_sum_opa = 1;
        }

        scenes[scene_act].fps_opa = (1000 * scenes[scene_act].refr_cnt_opa) / scenes[scene_act].time_sum_opa;
    } else {
        if (scenes[scene_act].time_sum_normal == 0) {
            scenes[scene_act].time_sum_normal = 1;
        }

        scenes[scene_act].fps_normal = (1000 * scenes[scene_act].refr_cnt_normal) / scenes[scene_act].time_sum_normal;
    }
}

static lv_res_t load_next_scene(void)
{
    if ((scene_act >= 0) && (scenes[scene_act].create_cb == NULL)) {
        return LV_RES_INV;
    }

    lv_obj_clean(scene_bg);

    if (scene_with_opa) {
        scene_act++;
        scene_with_opa = false;
    } else {
        scene_with_opa = true;
    }

    if ((scene_act >= 0) && (scenes[scene_act].create_cb == NULL)) {
        return LV_RES_INV;
    }

    last_flush_cb_call = 0;
    rnd_reset();
    scenes[scene_act].create_cb();

    lv_label_set_text_fmt(title, "%s%s", scenes[scene_act].name, scene_with_opa ? " + opa" : "");
    return LV_RES_OK;
}

static void next_scene_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    calc_scene_statistics();
    show_scene_report();
    lv_res_t res = load_next_scene();

    if (res == LV_RES_INV) {
        lv_timer_del(timer);
        generate_report();
    }
}

static void single_scene_finsih_timer_cb(lv_timer_t *timer)
{
    calc_scene_statistics();

    if ((mode == LV_DEMO_BENCHMARK_MODE_RENDER_ONLY) || (mode == LV_DEMO_BENCHMARK_MODE_REAL)) {
        lv_disp_t *disp = lv_disp_get_default();
        disp->driver->flush_cb = flush_cb_ori;
    }

    show_scene_report();
    lv_obj_clean(scene_bg);
    lv_obj_invalidate(lv_scr_act());
}

static void monitor_cb(lv_disp_drv_t *drv, uint32_t time, uint32_t px)
{
    LV_UNUSED(drv);
    LV_UNUSED(px);

    if (scene_with_opa) {
        scenes[scene_act].refr_cnt_opa++;
        scenes[scene_act].time_sum_opa += time;
    } else {
        scenes[scene_act].refr_cnt_normal++;
        scenes[scene_act].time_sum_normal += time;
    }
}

static void render_start_cb(lv_disp_drv_t *drv)
{
    LV_UNUSED(drv);
    render_start_time = lv_tick_get();
}

static void dummy_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *colors)
{
    LV_UNUSED(area);
    LV_UNUSED(colors);

    if (mode == LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER) {
        bool last = lv_disp_flush_is_last(drv);
        flush_cb_ori(drv, area, colors);
        if (last) {
            uint32_t t = lv_tick_elaps(render_start_time);
            if (scene_with_opa) {
                scenes[scene_act].refr_cnt_opa++;
                scenes[scene_act].time_sum_opa += t;
            } else {
                scenes[scene_act].refr_cnt_normal++;
                scenes[scene_act].time_sum_normal += t;
            }
        }
    } else if (mode == LV_DEMO_BENCHMARK_MODE_REAL) {
        bool last = lv_disp_flush_is_last(drv);
        flush_cb_ori(drv, area, colors);
        if (last) {
            if (last_flush_cb_call != 0) {
                uint32_t t = lv_tick_elaps(last_flush_cb_call);
                if (scene_with_opa) {
                    scenes[scene_act].refr_cnt_opa ++;
                    scenes[scene_act].time_sum_opa += t;
                } else {
                    scenes[scene_act].refr_cnt_normal ++;
                    scenes[scene_act].time_sum_normal += t;
                }
            }

            last_flush_cb_call = lv_tick_get();
        }
    } else if (mode == LV_DEMO_BENCHMARK_MODE_RENDER_ONLY) {
        lv_disp_flush_ready(drv);
    }
}

static void generate_report(void)
{
    if (mode == LV_DEMO_BENCHMARK_MODE_RENDER_ONLY) {
        lv_disp_t *disp = lv_disp_get_default();
        disp->driver->flush_cb = flush_cb_ori;
    }

    uint32_t i;
    uint32_t fps_sum = 0;
    uint32_t weight_sum = 0;
    uint32_t fps_opa_sum = 0;
    uint32_t weight_opa_sum = 0;
    uint32_t fps_normal_sum = 0;
    uint32_t weight_normal_sum = 0;

    for (i = 0; scenes[i].create_cb; i++) {
        fps_normal_sum += scenes[i].fps_normal * scenes[i].weight;
        weight_normal_sum += scenes[i].weight;

        uint32_t w = LV_MAX(scenes[i].weight / 2, 1);
        fps_opa_sum += scenes[i].fps_opa * w;
        weight_opa_sum += w;
    }

    fps_sum = fps_normal_sum + fps_opa_sum;
    weight_sum = weight_normal_sum + weight_opa_sum;

    uint32_t fps_weighted = fps_sum / weight_sum;
    uint32_t fps_normal_unweighted = fps_normal_sum / weight_normal_sum;
    uint32_t fps_opa_unweighted = fps_opa_sum / weight_opa_sum;

    uint32_t opa_speed_pct = (fps_opa_unweighted * 100) / fps_normal_unweighted;

    lv_obj_clean(lv_scr_act());
    scene_bg = NULL;

    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);

    title = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(title, "Weighted FPS: %" LV_PRIu32, fps_weighted);

    subtitle = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(subtitle, "Opa. speed: %" LV_PRIu32 "%%", opa_speed_pct);

    lv_coord_t w = lv_obj_get_content_width(lv_scr_act());
    lv_obj_t *table = lv_table_create(lv_scr_act());
    lv_table_set_col_cnt(table, 2);

    lv_table_set_col_width(table, 0, (w * 3) / 4 - 3);
    lv_table_set_col_width(table, 1, w  / 4 - 3);
    lv_obj_set_width(table, lv_pct(100));

    uint16_t row = 0;
    lv_table_add_cell_ctrl(table, row, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, row, 0, "Slow but common cases");

    LV_LOG("\r\nLVGL v%d.%d.%d " LVGL_VERSION_INFO " Benchmark (in csv format)\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    LV_LOG("Weighted FPS: %" LV_PRIu32 "\n", fps_weighted);
    LV_LOG("Opa. speed: %" LV_PRIu32 "%%\n", opa_speed_pct);

    row++;
    char buf[256];
    for (i = 0; scenes[i].create_cb; i++) {
        if ((scenes[i].fps_normal < 20) && (scenes[i].weight >= 10)) {
            lv_table_set_cell_value(table, row, 0, scenes[i].name);

            lv_snprintf(buf, sizeof(buf), "%" LV_PRIu32, scenes[i].fps_normal);
            lv_table_set_cell_value(table, row, 1, buf);

            // LV_LOG("%s,%s\r\n", scenes[i].name, buf);
            row++;
        }

        if ((scenes[i].fps_opa < 20) && (LV_MAX(scenes[i].weight / 2, 1) >= 10)) {
            lv_snprintf(buf, sizeof(buf), "%s + opa", scenes[i].name);
            lv_table_set_cell_value(table, row, 0, buf);

            // LV_LOG("%s,", buf);

            lv_snprintf(buf, sizeof(buf), "%" LV_PRIu32, scenes[i].fps_opa);
            lv_table_set_cell_value(table, row, 1, buf);

            // LV_LOG("%s\r\n", buf);
            row++;
        }
    }

    if (row == 1) {
        lv_table_add_cell_ctrl(table, row, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
        lv_table_set_cell_value(table, row, 0, "All good");
        row++;
    }

    lv_table_add_cell_ctrl(table, row, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, row, 0, "All cases");
    row++;

    for (i = 0; scenes[i].create_cb; i++) {
        lv_table_set_cell_value(table, row, 0, scenes[i].name);

        lv_snprintf(buf, sizeof(buf), "%" LV_PRIu32, scenes[i].fps_normal);
        lv_table_set_cell_value(table, row, 1, buf);

        if (scenes[i].fps_normal < 10) {

        } else if(scenes[i].fps_normal < 20) {

        }

        LV_LOG("%s,%s\r\n", scenes[i].name, buf);
        row++;

        lv_snprintf(buf, sizeof(buf), "%s + opa", scenes[i].name);
        lv_table_set_cell_value(table, row, 0, buf);

        LV_LOG("%s,", buf);

        lv_snprintf(buf, sizeof(buf), "%" LV_PRIu32, scenes[i].fps_opa);
        lv_table_set_cell_value(table, row, 1, buf);

        if (scenes[i].fps_opa < 10) {

        } else if(scenes[i].fps_opa < 20) {

        }

        LV_LOG("%s\r\n", buf);
        row++;
    }
}

static void rect_create(lv_style_t * style)
{
    uint32_t i;
    for (i = 0; i < OBJ_NUM; i++) {
        lv_obj_t *obj = lv_obj_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);
        lv_obj_set_style_border_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);
        lv_obj_set_style_shadow_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        lv_obj_set_size(obj, rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX), rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX));

        fall_anim(obj);
    }
}

static void img_create(lv_style_t *style, const void *src, bool rotate, bool zoom, bool aa)
{
    uint32_t i;
    for (i = 0; i < (uint32_t)IMG_NUM; i++) {
        lv_obj_t *obj = lv_img_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_img_set_src(obj, src);
        lv_obj_set_style_img_recolor(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        if (rotate) {
            lv_img_set_angle(obj, rnd_next(0, 3599));
        }

        if (zoom) {
            lv_img_set_zoom(obj, rnd_next(IMG_ZOOM_MIN, IMG_ZOOM_MAX));
        }

        lv_img_set_antialias(obj, aa);

        fall_anim(obj);
    }
}

static void txt_create(lv_style_t *style)
{
    uint32_t i;
    for (i = 0; i < OBJ_NUM; i++) {
        lv_obj_t *obj = lv_label_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_obj_set_style_text_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        lv_label_set_text(obj, TXT);

        fall_anim(obj);
    }
}

static void line_create(lv_style_t *style)
{
    static lv_point_t points[OBJ_NUM][LINE_POINT_NUM];

    uint32_t i;
    for (i = 0; i < OBJ_NUM; i++) {
        points[i][0].x = 0;
        points[i][0].y = 0;

        uint32_t j;
        for (j = 1; j < LINE_POINT_NUM; j++) {
            points[i][j].x = points[i][j - 1].x + rnd_next(LINE_POINT_DIFF_MIN, LINE_POINT_DIFF_MAX);
            points[i][j].y = rnd_next(LINE_POINT_DIFF_MIN, LINE_POINT_DIFF_MAX);
        }

        lv_obj_t *obj = lv_line_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_obj_set_style_line_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        lv_line_set_points(obj, points[i], LINE_POINT_NUM);

        fall_anim(obj);
    }
}

static void arc_anim_end_angle_cb(void *var, int32_t v)
{
    lv_arc_set_end_angle((lv_obj_t *)var, v);
}

static void arc_create(lv_style_t *style)
{
    uint32_t i;
    for (i = 0; i < OBJ_NUM; i++) {
        lv_obj_t *obj = lv_arc_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_set_size(obj, rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX), rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX));
        lv_obj_add_style(obj, style, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), LV_PART_INDICATOR);

        lv_arc_set_start_angle(obj, 0);

        uint32_t t = rnd_next(ANIM_TIME_MIN / 4, ANIM_TIME_MAX / 4);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, arc_anim_end_angle_cb);
        lv_anim_set_values(&a, 0, 359);
        lv_anim_set_time(&a, t);
        lv_anim_set_playback_time(&a, t);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a);

        fall_anim(obj);
    }
}

static void fall_anim_y_cb(void *var, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)var, v);
}

static void fall_anim(lv_obj_t *obj)
{
    lv_obj_set_x(obj, rnd_next(0, lv_obj_get_width(scene_bg) - lv_obj_get_width(obj)));

    uint32_t t = rnd_next(ANIM_TIME_MIN, ANIM_TIME_MAX);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, fall_anim_y_cb);
    lv_anim_set_values(&a, 0, lv_obj_get_height(scene_bg) - lv_obj_get_height(obj));
    lv_anim_set_time(&a, t);
    lv_anim_set_playback_time(&a, t);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    a.act_time = a.time / 2;
    lv_anim_start(&a);
}

static void rnd_reset(void)
{
    rnd_act = 0;
}

static int32_t rnd_next(int32_t min, int32_t max)
{
    if (min == max) {
        return min;
    }
    
    if (min > max) {
        int32_t t = min;
        min = max;
        max = t;
    }

    int32_t d = max - min;
    int32_t r = (rnd_map[rnd_act] % d) + min;

    rnd_act++;
    if (rnd_act >= RND_NUM) {
        rnd_act = 0;
    }

    return r;
}

#endif
