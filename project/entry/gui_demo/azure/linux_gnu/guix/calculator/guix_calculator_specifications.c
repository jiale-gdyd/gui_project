#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "guix_calculator_resources.h"
#include "guix_calculator_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
FRAME_CONTROL_BLOCK frame;
GX_DISPLAY Main_Screen_control_block;
GX_WINDOW_ROOT Main_Screen_root_window;
GX_CANVAS  Main_Screen_canvas_control_block;
ULONG      Main_Screen_canvas_memory[44800];

extern GX_CONST GX_THEME *Main_Screen_theme_table[];
extern GX_CONST GX_STRING *Main_Screen_language_table[];

GX_STUDIO_DISPLAY_INFO guix_calculator_display_table[1] =
{
    {
    "Main_Screen",
    "Main_Screen_canvas",
    Main_Screen_theme_table,
    Main_Screen_language_table,
    MAIN_SCREEN_THEME_TABLE_SIZE,
    MAIN_SCREEN_LANGUAGE_TABLE_SIZE,
    MAIN_SCREEN_STRING_TABLE_SIZE,
    280,                                     /* x resolution                   */
    320,                                     /* y resolution                   */
    &Main_Screen_control_block,
    &Main_Screen_canvas_control_block,
    &Main_Screen_root_window,
    Main_Screen_canvas_memory,               /* canvas memory area             */
    179200,                                  /* canvas memory size in bytes    */
    GX_SCREEN_ROTATION_NONE                  /* rotation angle                 */
    }
};


UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PIXELMAP_BUTTON *button = (GX_PIXELMAP_BUTTON *) control_block;
    GX_PIXELMAP_BUTTON_PROPERTIES *props = (GX_PIXELMAP_BUTTON_PROPERTIES *) info->properties;
    status = gx_pixelmap_button_create(button, info->widget_name, parent,
               props->normal_pixelmap_id,
               props->selected_pixelmap_id,
               props->disabled_pixelmap_id,
               info->style, info->widget_id, &info->size);
    return status;
}

UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_ICON *icon = (GX_ICON *) control_block;
    GX_ICON_PROPERTIES *props = (GX_ICON_PROPERTIES *) info->properties;
    status = gx_icon_create(icon, info->widget_name, parent, props->normal_pixelmap_id, info->style, info->widget_id, info->size.gx_rectangle_left, info->size.gx_rectangle_top);
    if (props->selected_pixelmap_id)
    {
        gx_icon_pixelmap_set(icon, props->normal_pixelmap_id, props->selected_pixelmap_id);
    }
    else
    {
        gx_widget_resize((GX_WIDGET *)icon, (GX_RECTANGLE *)&info->size);
    }
    return status;
}

UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_WINDOW *window = (GX_WINDOW *) control_block;
    GX_WINDOW_PROPERTIES *props = (GX_WINDOW_PROPERTIES *) info->properties;
    status = gx_window_create(window, info->widget_name, parent, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set(window, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }
    return status;
}

UINT gx_studio_text_input_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_STRING text;
    GX_SINGLE_LINE_TEXT_INPUT *input = (GX_SINGLE_LINE_TEXT_INPUT *) control_block;
    GX_PROMPT *prompt = (GX_PROMPT *) input;
    GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES *props = (GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES *) info->properties;
    status = gx_single_line_text_input_create(input, info->widget_name, parent, props->buffer, props->buffer_size, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_prompt_font_set(prompt, props->font_id);
        gx_single_line_text_input_text_color_set(input, props->normal_text_color_id, props->selected_text_color_id,
                                                 props->disabled_text_color_id, props->readonly_text_color_id);
        gx_single_line_text_input_fill_color_set(input, input->gx_widget_normal_fill_color, input->gx_widget_selected_fill_color,
                                                 input->gx_widget_disabled_fill_color, props->readonly_fill_color_id);
        if (props->buffer && props->buffer_size > 0 && props->string_id)
        {
             gx_display_string_get_ext(guix_calculator_display_table[0].display, props->string_id, &text);

             if (text.gx_string_ptr)
             {
                 gx_single_line_text_input_text_set_ext(input, &text);
             }
        }
    }
    return status;
}
GX_WINDOW_PROPERTIES frame_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES frame_icon_1_properties =
{
    GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES frame_calculator_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_result_properties =
{
    GX_PIXELMAP_ID_B_RESULT,                 /* normal pixelmap id             */
    GX_PIXELMAP_ID_B_RESULT_H,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_19_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_10_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_11_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_divide_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_multi_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_minus_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_plus_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_percent_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_dot_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_12_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_13_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_14_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_15_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_16_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_17_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_18_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_c_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_mc_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_mr_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES frame_pixelmap_button_del_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_H,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES frame_text_bg_win_2_properties =
{
    GX_PIXELMAP_ID_TEXTBG                    /* wallpaper pixelmap id          */
};
GX_CHAR frame_display_result_buffer[100];
GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES frame_display_result_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_TEXT_INPUT,                   /* font id                        */
    GX_COLOR_ID_DARK_GRAY,                   /* normal text color              */
    GX_COLOR_ID_DARK_GRAY,                   /* selected text color            */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled text color            */
    GX_COLOR_ID_READONLY_FILL,               /* readonly fill color            */
    GX_COLOR_ID_READONLY_TEXT,               /* readonly text color            */
    frame_display_result_buffer,             /* buffer                         */
    100,                                     /* buffer size                    */
};

GX_CONST GX_STUDIO_WIDGET frame_display_result_define =
{
    "display_result",
    GX_TYPE_SINGLE_LINE_TEXT_INPUT,          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SINGLE_LINE_TEXT_INPUT),       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_text_input_create,             /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 35, 265, 69},                       /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_display_result), /* control block      */
    (void *) &frame_display_result_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET frame_text_bg_win_2_define =
{
    "text_bg_win_2",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {38, 31, 269, 73},                       /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &frame_display_result_define,            /* child widget definition        */
    offsetof(FRAME_CONTROL_BLOCK, frame_text_bg_win_2), /* control block       */
    (void *) &frame_text_bg_win_2_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_del_define =
{
    "pixelmap_button_del",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_DEL,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {213, 79, 267, 118},                     /* widget size                    */
    &frame_text_bg_win_2_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_del), /* control block */
    (void *) &frame_pixelmap_button_del_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_mr_define =
{
    "pixelmap_button_mr",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_MR,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {154, 78, 208, 117},                     /* widget size                    */
    &frame_pixelmap_button_del_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_mr), /* control block  */
    (void *) &frame_pixelmap_button_mr_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_mc_define =
{
    "pixelmap_button_mc",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_MC,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {95, 78, 149, 117},                      /* widget size                    */
    &frame_pixelmap_button_mr_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_mc), /* control block  */
    (void *) &frame_pixelmap_button_mc_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_c_define =
{
    "pixelmap_button_c",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_C,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {36, 78, 90, 117},                       /* widget size                    */
    &frame_pixelmap_button_mc_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_c), /* control block   */
    (void *) &frame_pixelmap_button_c_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_18_define =
{
    "pixelmap_button_18",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_FOUR,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {36, 157, 90, 196},                      /* widget size                    */
    &frame_pixelmap_button_c_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_18), /* control block  */
    (void *) &frame_pixelmap_button_18_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_17_define =
{
    "pixelmap_button_17",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_ONE,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {36, 196, 90, 235},                      /* widget size                    */
    &frame_pixelmap_button_18_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_17), /* control block  */
    (void *) &frame_pixelmap_button_17_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_16_define =
{
    "pixelmap_button_16",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_ZERO,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {36, 235, 90, 274},                      /* widget size                    */
    &frame_pixelmap_button_17_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_16), /* control block  */
    (void *) &frame_pixelmap_button_16_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_15_define =
{
    "pixelmap_button_15",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_THREE,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {154, 196, 208, 235},                    /* widget size                    */
    &frame_pixelmap_button_16_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_15), /* control block  */
    (void *) &frame_pixelmap_button_15_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_14_define =
{
    "pixelmap_button_14",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_SIX,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {154, 157, 208, 196},                    /* widget size                    */
    &frame_pixelmap_button_15_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_14), /* control block  */
    (void *) &frame_pixelmap_button_14_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_13_define =
{
    "pixelmap_button_13",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_TWO,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {95, 196, 149, 235},                     /* widget size                    */
    &frame_pixelmap_button_14_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_13), /* control block  */
    (void *) &frame_pixelmap_button_13_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_12_define =
{
    "pixelmap_button_12",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_FIVE,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {95, 157, 149, 196},                     /* widget size                    */
    &frame_pixelmap_button_13_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_12), /* control block  */
    (void *) &frame_pixelmap_button_12_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_dot_define =
{
    "pixelmap_button_dot",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_DOT,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {95, 235, 149, 274},                     /* widget size                    */
    &frame_pixelmap_button_12_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_dot), /* control block */
    (void *) &frame_pixelmap_button_dot_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_percent_define =
{
    "pixelmap_button_percent",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_PERCENT,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {154, 235, 208, 274},                    /* widget size                    */
    &frame_pixelmap_button_dot_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_percent), /* control block */
    (void *) &frame_pixelmap_button_percent_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_plus_define =
{
    "pixelmap_button_plus",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_PLUS,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {213, 235, 267, 274},                    /* widget size                    */
    &frame_pixelmap_button_percent_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_plus), /* control block */
    (void *) &frame_pixelmap_button_plus_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_minus_define =
{
    "pixelmap_button_minus",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_MINUS,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {213, 196, 267, 235},                    /* widget size                    */
    &frame_pixelmap_button_plus_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_minus), /* control block */
    (void *) &frame_pixelmap_button_minus_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_multi_define =
{
    "pixelmap_button_multi",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_MULTI,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {213, 157, 267, 196},                    /* widget size                    */
    &frame_pixelmap_button_minus_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_multi), /* control block */
    (void *) &frame_pixelmap_button_multi_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_divide_define =
{
    "pixelmap_button_divide",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_DIVIDE,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {213, 118, 267, 157},                    /* widget size                    */
    &frame_pixelmap_button_multi_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_divide), /* control block */
    (void *) &frame_pixelmap_button_divide_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_11_define =
{
    "pixelmap_button_11",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_NINE,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {154, 118, 208, 157},                    /* widget size                    */
    &frame_pixelmap_button_divide_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_11), /* control block  */
    (void *) &frame_pixelmap_button_11_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_10_define =
{
    "pixelmap_button_10",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_EIGHT,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {95, 118, 149, 157},                     /* widget size                    */
    &frame_pixelmap_button_11_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_10), /* control block  */
    (void *) &frame_pixelmap_button_10_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_19_define =
{
    "pixelmap_button_19",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_SEVEN,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {36, 118, 90, 157},                      /* widget size                    */
    &frame_pixelmap_button_10_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_19), /* control block  */
    (void *) &frame_pixelmap_button_19_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET frame_pixelmap_button_result_define =
{
    "pixelmap_button_result",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CALCULATOR_RESULT,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {175, 277, 263, 306},                    /* widget size                    */
    &frame_pixelmap_button_19_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_pixelmap_button_result), /* control block */
    (void *) &frame_pixelmap_button_result_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET frame_calculator_define =
{
    "calculator",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) calculator_event_handler, /* event function override */
    {34, 25, 273, 311},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &frame_pixelmap_button_result_define,    /* child widget definition        */
    offsetof(FRAME_CONTROL_BLOCK, frame_calculator), /* control block          */
    (void *) &frame_calculator_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET frame_icon_1_define =
{
    "icon_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {5, 5, 130, 24},                         /* widget size                    */
    &frame_calculator_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FRAME_CONTROL_BLOCK, frame_icon_1), /* control block              */
    (void *) &frame_icon_1_properties        /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET frame_define =
{
    "frame",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(FRAME_CONTROL_BLOCK),             /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 279, 319},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &frame_icon_1_define,                    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &frame_properties               /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY guix_calculator_widget_table[] =
{
    { &frame_define, (GX_WIDGET *) &frame },
    {GX_NULL, GX_NULL}
};

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent)
{
    UINT status = GX_SUCCESS;
    GX_WIDGET *widget = GX_NULL;
    GX_VALUE   list_count = 0;
    GX_VALUE   list_total_count = 0;

    if(parent && (parent->gx_widget_type == GX_TYPE_MENU))
    {
        list_total_count = ((GX_MENU *)parent)->gx_menu_list_total_count;
    }

    while(definition && status == GX_SUCCESS)
    {
        if (definition->create_function)
        {
            if (definition->style & GX_STYLE_DYNAMICALLY_ALLOCATED)
            {
                status = gx_widget_allocate(&widget, definition->control_block_size);
                if (status != GX_SUCCESS)
                {
                    return GX_NULL;
                }
            }
            else
            {
                if (control == GX_NULL)
                {
                    return GX_NULL;
                }
                widget = (GX_WIDGET *) (control + definition->control_block_offset);
            }

            status = definition->create_function(definition, widget, parent);

            if(list_count < list_total_count)
            {
                gx_menu_insert((GX_MENU *)parent, widget);
                ((GX_MENU *)parent)->gx_menu_list_total_count--;
                list_count++;
            }

            if (status == GX_SUCCESS)
            {
                if (definition->widget_type != GX_TYPE_TEMPLATE)
                {
#if defined(GUIX_5_4_0_COMPATIBILITY)
                    gx_widget_fill_color_set(widget, definition->normal_fill_color_id, definition->selected_fill_color_id);
#else
                    gx_widget_fill_color_set(widget, definition->normal_fill_color_id, definition->selected_fill_color_id, definition->disabled_fill_color_id);
#endif
                }

                if (!(definition->status & GX_STATUS_ACCEPTS_FOCUS))
                {
                    gx_widget_status_remove(widget, GX_STATUS_ACCEPTS_FOCUS);
                }

                if (definition->draw_function)
                {
                    gx_widget_draw_set(widget, definition->draw_function);
                }
                if (definition->event_function)
                {
                    gx_widget_event_process_set(widget, definition->event_function);
                }

                #if defined(GX_WIDGET_USER_DATA)
                widget->gx_widget_user_data = definition->user_data;
                #endif

                if (definition->child_widget)
                {
                    gx_studio_nested_widget_create(control, definition->child_widget, widget);
                }
            }
            definition = definition->next_widget;
        }
    }
    return widget;
}

GX_WIDGET *gx_studio_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent)
{
    GX_WIDGET *widget;
    widget = gx_studio_nested_widget_create(control, definition, GX_NULL);

    if (parent && widget)
    {
        gx_widget_attach(parent, widget);
    }
    return widget;
}

UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget)
{
    UINT status = GX_FAILURE;
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = guix_calculator_widget_table;
    GX_WIDGET *widget = GX_NULL;

    while(entry->widget_information)
    {
        if (!strcmp(name, entry->widget_information->widget_name))
        {
            widget = gx_studio_widget_create((GX_BYTE *) entry->widget, entry->widget_information, parent);
            if (widget)
            {
                status = GX_SUCCESS;
            }
            break;
        }
        entry++;
    }

    if (new_widget)
    {
        *new_widget = widget;
    }
    return status;
}


UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *),
    GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root)
{
    GX_CONST GX_THEME *theme_ptr;
    GX_RECTANGLE size;

    GX_STUDIO_DISPLAY_INFO *display_info = &guix_calculator_display_table[display];


/* create the requested display                                                */

    gx_display_create(display_info->display,
                      display_info->name,
                      driver,
                      (GX_VALUE) display_info->x_resolution,
                      (GX_VALUE) display_info->y_resolution);


/* install the request theme                                                   */

    if(display_info->theme_table)
    {
        theme_ptr = display_info->theme_table[theme];
        if(theme_ptr)
        {
            gx_display_color_table_set(display_info->display, theme_ptr->theme_color_table, theme_ptr->theme_color_table_size);
            
/* install the color palette if required                                       */
            if (display_info->display->gx_display_driver_palette_set &&
                theme_ptr->theme_palette != NULL)
            {
                display_info->display->gx_display_driver_palette_set(display_info->display, theme_ptr->theme_palette, theme_ptr->theme_palette_size);
            }

            gx_display_font_table_set(display_info->display, theme_ptr->theme_font_table, theme_ptr->theme_font_table_size);
            gx_display_pixelmap_table_set(display_info->display, theme_ptr->theme_pixelmap_table, theme_ptr->theme_pixelmap_table_size);
            gx_system_scroll_appearance_set(theme_ptr->theme_vertical_scroll_style, (GX_SCROLLBAR_APPEARANCE *) &theme_ptr->theme_vertical_scrollbar_appearance);
            gx_system_scroll_appearance_set(theme_ptr->theme_horizontal_scroll_style, (GX_SCROLLBAR_APPEARANCE *) &theme_ptr->theme_horizontal_scrollbar_appearance);
        }
    }

/* Install the language table.                                                 */

    if(display_info->language_table)
    {
        gx_display_language_table_set_ext(display_info->display, display_info->language_table, (GX_UBYTE) display_info->language_table_size, display_info->string_table_size);
        gx_display_active_language_set(display_info->display, language);
    }

/* Set screen rotation angle.                                                  */

    display_info->display->gx_display_rotation_angle = display_info->rotation_angle;

/* create the canvas for this display                                          */

    gx_canvas_create(display_info->canvas,
                     display_info->canvas_name,
                     display_info->display,
                     GX_CANVAS_MANAGED | GX_CANVAS_VISIBLE,
                     display_info->x_resolution,
                     display_info->y_resolution,
                     display_info->canvas_memory,
                     display_info->canvas_memory_size);

/* Create the root window for this canvas                                      */

    gx_utility_rectangle_define(&size,
                                0, 0,
                                (GX_VALUE) (display_info->x_resolution - 1),
                                (GX_VALUE) (display_info->y_resolution - 1));

    gx_window_root_create(display_info->root_window,
                          display_info->name,
                          display_info->canvas, GX_STYLE_NONE, 0, &size);
    if (return_root)
    {
        *return_root = display_info->root_window;
    }
    return GX_SUCCESS;
}
#undef GUIX_STUDIO_GENERATED_FILE
