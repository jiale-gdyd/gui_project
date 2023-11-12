/**
 * @file lv_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "lv_draw_label.h"
#include "../misc/lv_math.h"
#include "../core/lv_obj_event.h"
#include "../misc/lv_bidi.h"
#include "../misc/lv_assert.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                        const lv_font_t * font, uint32_t letter, lv_draw_letter_cb_t cb);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_label_dsc_t));
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
    dsc->font = LV_FONT_DEFAULT;
    dsc->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_end = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_color = lv_color_black();
    dsc->sel_bg_color = lv_palette_main(LV_PALETTE_BLUE);
    dsc->bidi_dir = LV_BASE_DIR_LTR;
}

void lv_draw_letter_dsc_init(lv_draw_glyph_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_glyph_dsc_t));
}

LV_ATTRIBUTE_FAST_MEM void lv_draw_label(lv_layer_t * layer, const lv_draw_label_dsc_t * dsc,
                                         const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;
    if(dsc->font == NULL) {
        LV_LOG_WARN("dsc->font == NULL");
        return;
    }

    lv_draw_task_t * t = lv_draw_add_task(layer, coords);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_LABEL;

    /*The text is stored in a local variable so malloc memory for it*/
    if(dsc->text_local) {
        lv_draw_label_dsc_t * new_dsc = t->draw_dsc;
        new_dsc->text = lv_strdup(dsc->text);
    }

    lv_draw_finalize_task_creation(layer, t);
}

LV_ATTRIBUTE_FAST_MEM void lv_draw_letter(lv_layer_t * layer, lv_draw_label_dsc_t * dsc,
                                          const lv_point_t * point, uint32_t unicode_letter)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->font == NULL) {
        LV_LOG_WARN("dsc->font == NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    lv_font_get_glyph_dsc(dsc->font, &g, unicode_letter, 0);

    lv_area_t a;
    a.x1 = point->x;
    a.y1 = point->y;
    a.x2 = a.x1 + g.adv_w;
    a.y2 = a.y1 + lv_font_get_line_height(g.resolved_font ? g.resolved_font : dsc->font);

    /*lv_draw_label needs UTF8 text so convert the Unicode character to an UTF8 string */
    uint32_t letter_buf[2];
    letter_buf[0] = _lv_text_unicode_to_encoded(unicode_letter);
    letter_buf[1] = '\0';

    const char * letter_buf_char = (const char *)letter_buf;

#if LV_BIG_ENDIAN_SYSTEM
    while(*letter_buf_char == 0) ++letter_buf_char;
#endif

    dsc->text = letter_buf_char;
    dsc->text_local = 1;

    lv_draw_label(layer, dsc, &a);
}

void lv_draw_label_iterate_letters(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                                   const lv_area_t * coords,
                                   lv_draw_letter_cb_t cb)
{
    const lv_font_t * font = dsc->font;
    int32_t w;

    lv_area_t clipped_area;
    bool clip_ok = _lv_area_intersect(&clipped_area, coords, draw_unit->clip_area);
    if(!clip_ok) return;

    lv_text_align_t align = dsc->align;
    lv_base_dir_t base_dir = dsc->bidi_dir;

    lv_bidi_calculate_align(&align, &base_dir, dsc->text);

    if((dsc->flag & LV_TEXT_FLAG_EXPAND) == 0) {
        /*Normally use the label's width as width*/
        w = lv_area_get_width(coords);
    }
    else {
        /*If EXPAND is enabled then not limit the text's width to the object's width*/
        lv_point_t p;
        lv_text_get_size(&p, dsc->text, dsc->font, dsc->letter_space, dsc->line_space, LV_COORD_MAX,
                         dsc->flag);
        w = p.x;
    }

    int32_t line_height_font = lv_font_get_line_height(font);
    int32_t line_height = line_height_font + dsc->line_space;

    /*Init variables for the first line*/
    int32_t line_width = 0;
    lv_point_t pos;
    pos.x = coords->x1;
    pos.y = coords->y1;

    int32_t x_ofs = 0;
    int32_t y_ofs = 0;
    x_ofs = dsc->ofs_x;
    y_ofs = dsc->ofs_y;
    pos.y += y_ofs;

    uint32_t line_start     = 0;
    int32_t last_line_start = -1;

    /*Check the hint to use the cached info*/
    if(dsc->hint && y_ofs == 0 && coords->y1 < 0) {
        /*If the label changed too much recalculate the hint.*/
        if(LV_ABS(dsc->hint->coord_y - coords->y1) > LV_LABEL_HINT_UPDATE_TH - 2 * line_height) {
            dsc->hint->line_start = -1;
        }
        last_line_start = dsc->hint->line_start;
    }

    /*Use the hint if it's valid*/
    if(dsc->hint && last_line_start >= 0) {
        line_start = last_line_start;
        pos.y += dsc->hint->y;
    }

    uint32_t line_end = line_start + _lv_text_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL,
                                                            dsc->flag);

    /*Go the first visible line*/
    while(pos.y + line_height_font < draw_unit->clip_area->y1) {
        /*Go to next line*/
        line_start = line_end;
        line_end += _lv_text_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
        pos.y += line_height;

        /*Save at the threshold coordinate*/
        if(dsc->hint && pos.y >= -LV_LABEL_HINT_UPDATE_TH && dsc->hint->line_start < 0) {
            dsc->hint->line_start = line_start;
            dsc->hint->y          = pos.y - coords->y1;
            dsc->hint->coord_y    = coords->y1;
        }

        if(dsc->text[line_start] == '\0') return;
    }

    /*Align to middle*/
    if(align == LV_TEXT_ALIGN_CENTER) {
        line_width = lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        line_width = lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);
        pos.x += lv_area_get_width(coords) - line_width;
    }

    uint32_t sel_start = dsc->sel_start;
    uint32_t sel_end = dsc->sel_end;
    if(sel_start > sel_end) {
        uint32_t tmp = sel_start;
        sel_start = sel_end;
        sel_end = tmp;
    }

    lv_area_t bg_coords;
    lv_draw_glyph_dsc_t draw_letter_dsc;
    lv_draw_letter_dsc_init(&draw_letter_dsc);
    draw_letter_dsc.opa = dsc->opa;
    draw_letter_dsc.bg_coords = &bg_coords;
    draw_letter_dsc.color = dsc->color;

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.opa = dsc->opa;
    int32_t underline_width = font->underline_thickness ? font->underline_thickness : 1;
    int32_t line_start_x;
    uint32_t i;
    int32_t letter_w;

    /*Write out all lines*/
    while(dsc->text[line_start] != '\0') {
        pos.x += x_ofs;
        line_start_x = pos.x;

        /*Write all letter of a line*/
        i = 0;
#if LV_USE_BIDI
        char * bidi_txt = lv_malloc(line_end - line_start + 1);
        LV_ASSERT_MALLOC(bidi_txt);
        _lv_bidi_process_paragraph(dsc->text + line_start, bidi_txt, line_end - line_start, base_dir, NULL, 0);
#else
        const char * bidi_txt = dsc->text + line_start;
#endif

        while(i < line_end - line_start) {
            uint32_t logical_char_pos = 0;
            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
#if LV_USE_BIDI
                logical_char_pos = _lv_text_encoded_get_char_id(dsc->text, line_start);
                uint32_t t = _lv_text_encoded_get_char_id(bidi_txt, i);
                logical_char_pos += _lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, t, NULL);
#else
                logical_char_pos = _lv_text_encoded_get_char_id(dsc->text, line_start + i);
#endif
            }

            uint32_t letter;
            uint32_t letter_next;
            _lv_text_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &i);

            letter_w = lv_font_get_glyph_width(font, letter, letter_next);

            /*Always set the bg_coordinates for placeholder drawing*/
            bg_coords.x1 = pos.x;
            bg_coords.y1 = pos.y;
            bg_coords.x2 = pos.x + letter_w + dsc->letter_space - 1;
            bg_coords.y2 = pos.y + line_height - 1;

            if(i >= line_end - line_start) {
                if(dsc->decor & LV_TEXT_DECOR_UNDERLINE) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + font->line_height - font->base_line - font->underline_position;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    cb(draw_unit, NULL, &fill_dsc, &fill_area);
                }
                if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
                    lv_area_t fill_area;
                    fill_area.x1 = line_start_x;
                    fill_area.x2 = pos.x + letter_w - 1;
                    fill_area.y1 = pos.y + (font->line_height - font->base_line) * 2 / 3 + font->underline_thickness / 2;
                    fill_area.y2 = fill_area.y1 + underline_width - 1;

                    fill_dsc.color = dsc->color;
                    cb(draw_unit, NULL, &fill_dsc, &fill_area);
                }
            }

            if(sel_start != 0xFFFF && sel_end != 0xFFFF && logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                draw_letter_dsc.color = dsc->sel_color;
                fill_dsc.color = dsc->sel_bg_color;
                cb(draw_unit, NULL, &fill_dsc, &bg_coords);
            }
            else {
                draw_letter_dsc.color = dsc->color;
            }

            draw_letter(draw_unit, &draw_letter_dsc, &pos, font, letter, cb);

            if(letter_w > 0) {
                pos.x += letter_w + dsc->letter_space;
            }
        }

#if LV_USE_BIDI
        lv_free(bidi_txt);
        bidi_txt = NULL;
#endif
        /*Go to next line*/
        line_start = line_end;
        line_end += _lv_text_get_next_line(&dsc->text[line_start], font, dsc->letter_space, w, NULL, dsc->flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(align == LV_TEXT_ALIGN_CENTER) {
            line_width =
                lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;
        }
        /*Align to the right*/
        else if(align == LV_TEXT_ALIGN_RIGHT) {
            line_width =
                lv_text_get_width(&dsc->text[line_start], line_end - line_start, font, dsc->letter_space);
            pos.x += lv_area_get_width(coords) - line_width;
        }

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > draw_unit->clip_area->y2) break;
    }
    lv_draw_buf_free(draw_letter_dsc._bitmap_buf_unaligned);

    LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                        const lv_font_t * font, uint32_t letter, lv_draw_letter_cb_t cb)
{
    lv_font_glyph_dsc_t g;

    LV_PROFILER_BEGIN;
    bool g_ret = lv_font_get_glyph_dsc(font, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
           letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
           letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%" LV_PRIX32, letter);
        }
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) {
        LV_PROFILER_END;
        return;
    }

    lv_area_t letter_coords;
    letter_coords.x1 = pos->x + g.ofs_x;
    letter_coords.x2 = letter_coords.x1 + g.box_w - 1;
    letter_coords.y1 = pos->y + (font->line_height - font->base_line) - g.box_h - g.ofs_y;
    letter_coords.y2 = letter_coords.y1 + g.box_h - 1;

    /*If the letter is completely out of mask don't draw it*/
    if(_lv_area_is_out(&letter_coords, draw_unit->clip_area, 0)) {
        LV_PROFILER_END;
        return;
    }

    uint32_t bitmap_size = lv_draw_buf_width_to_stride(g.box_w, LV_COLOR_FORMAT_A8) * g.box_h;
    bitmap_size = (bitmap_size + 63) &
                  (~63);   /*Round up to avoid many allocations if the next buffer is just slightly larger*/
    if(dsc->_bitmap_buf_size < bitmap_size) {
        lv_draw_buf_free(dsc->_bitmap_buf_unaligned);
        dsc->_bitmap_buf_unaligned = lv_draw_buf_malloc(bitmap_size, LV_COLOR_FORMAT_A8);
        LV_ASSERT_MALLOC(dsc->_bitmap_buf_unaligned);
        dsc->bitmap_buf = lv_draw_buf_align(dsc->_bitmap_buf_unaligned, LV_COLOR_FORMAT_A8);
        dsc->_bitmap_buf_size = bitmap_size;
    }

    if(g.resolved_font) {
        dsc->bitmap = lv_font_get_glyph_bitmap(g.resolved_font, letter, dsc->bitmap_buf);
    }
    else {
        dsc->bitmap = NULL;
    }
    dsc->letter_coords = &letter_coords;
    if(g.bpp == LV_IMGFONT_BPP) dsc->format = LV_DRAW_LETTER_BITMAP_FORMAT_IMAGE;
    else dsc->format = LV_DRAW_LETTER_BITMAP_FORMAT_A8;

    cb(draw_unit, dsc, NULL, NULL);
    LV_PROFILER_END;
}
