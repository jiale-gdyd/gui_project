#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "guix_shapes_resources.h"
#include "guix_shapes_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
TEXT_ROTAION_WINDOW_CONTROL_BLOCK Text_Rotaion_Window;
SHAPES_SCREEN_CONTROL_BLOCK Shapes_Screen;
GX_DISPLAY display_1_control_block;
GX_WINDOW_ROOT display_1_root_window;
GX_CANVAS  display_1_canvas_control_block;
ULONG      display_1_canvas_memory[307200];

extern GX_CONST GX_THEME *display_1_theme_table[];
extern GX_CONST GX_STRING *display_1_language_table[];

GX_STUDIO_DISPLAY_INFO guix_shapes_display_table[1] =
{
    {
    "display_1",
    "display_1_canvas",
    display_1_theme_table,
    display_1_language_table,
    DISPLAY_1_THEME_TABLE_SIZE,
    DISPLAY_1_LANGUAGE_TABLE_SIZE,
    DISPLAY_1_STRING_TABLE_SIZE,
    640,                                     /* x resolution                   */
    480,                                     /* y resolution                   */
    &display_1_control_block,
    &display_1_canvas_control_block,
    &display_1_root_window,
    display_1_canvas_memory,                 /* canvas memory area             */
    1228800,                                 /* canvas memory size in bytes    */
    GX_SCREEN_ROTATION_NONE                  /* rotation angle                 */
    }
};


UINT gx_studio_checkbox_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_CHECKBOX *button = (GX_CHECKBOX *) control_block;
    GX_TEXT_BUTTON *text_button = (GX_TEXT_BUTTON *) button;
    GX_CHECKBOX_PROPERTIES *props = (GX_CHECKBOX_PROPERTIES *) info->properties;
    status = gx_checkbox_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set(text_button, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_text_button_text_color_set(text_button, props->normal_text_color_id, props->selected_text_color_id);
 #else
        gx_text_button_text_color_set(text_button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif

        if (props->unchecked_pixelmap_id ||
            props->checked_pixelmap_id ||
            props->unchecked_disabled_pixelmap_id ||
            props->checked_disabled_pixelmap_id)
        {
            gx_checkbox_pixelmap_set(button,
                                     props->unchecked_pixelmap_id,
                                     props->checked_pixelmap_id,
                                     props->unchecked_disabled_pixelmap_id,
                                     props->checked_disabled_pixelmap_id);
        }
    }
    return status;
}

UINT gx_studio_radio_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_RADIO_BUTTON *button = (GX_RADIO_BUTTON *) control_block;
    GX_TEXT_BUTTON *text_button = (GX_TEXT_BUTTON *) button;
    GX_RADIO_BUTTON_PROPERTIES *props = (GX_RADIO_BUTTON_PROPERTIES *) info->properties;
    status = gx_radio_button_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set(text_button, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_text_button_text_color_set(text_button, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_text_button_text_color_set(text_button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif

        if (props->off_pixelmap_id ||
            props->on_pixelmap_id ||
            props->off_disabled_pixelmap_id ||
            props->on_disabled_pixelmap_id)
        {
            gx_radio_button_pixelmap_set(button,
                                     props->off_pixelmap_id,
                                     props->on_pixelmap_id,
                                     props->off_disabled_pixelmap_id,
                                     props->on_disabled_pixelmap_id);
        }
    }
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

UINT gx_studio_pixelmap_slider_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PIXELMAP_SLIDER *slider = (GX_PIXELMAP_SLIDER *) control_block;
    GX_PIXELMAP_SLIDER_PROPERTIES *props = (GX_PIXELMAP_SLIDER_PROPERTIES *) info->properties;
    GX_PIXELMAP_SLIDER_INFO pixelmap_info;
    GX_SLIDER_INFO slider_info;
    slider_info.gx_slider_info_min_val = props->min_val;
    slider_info.gx_slider_info_max_val = props->max_val;
    slider_info.gx_slider_info_current_val = props->current_val;
    slider_info.gx_slider_info_increment = props->increment;
    slider_info.gx_slider_info_min_travel = props->min_travel;
    slider_info.gx_slider_info_max_travel = props->max_travel;
    slider_info.gx_slider_info_needle_width = props->needle_width;
    slider_info.gx_slider_info_needle_height = props->needle_height;
    slider_info.gx_slider_info_needle_inset = props->needle_inset;
    slider_info.gx_slider_info_needle_hotspot_offset = props->needle_hotspot;
    pixelmap_info.gx_pixelmap_slider_info_lower_background_pixelmap = props->lower_pixelmap;
    pixelmap_info.gx_pixelmap_slider_info_upper_background_pixelmap = props->upper_pixelmap;
    pixelmap_info.gx_pixelmap_slider_info_needle_pixelmap = props->needle_pixelmap;
    status = gx_pixelmap_slider_create(slider,
                    info->widget_name,
                    parent,
                    &slider_info,
                    &pixelmap_info,
                    info->style,
                    info->widget_id,
                    &info->size);
    return status;
}

UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PROMPT *prompt = (GX_PROMPT *) control_block;
    GX_PROMPT_PROPERTIES *props = (GX_PROMPT_PROPERTIES *) info->properties;
    status = gx_prompt_create(prompt, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_prompt_font_set(prompt, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
    }
    return status;
}

UINT gx_studio_pixelmap_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PIXELMAP_PROMPT *pix_prompt = (GX_PIXELMAP_PROMPT *) control_block;
    GX_PROMPT *prompt = (GX_PROMPT *) pix_prompt;
    GX_PIXELMAP_PROMPT_PROPERTIES *props = (GX_PIXELMAP_PROMPT_PROPERTIES *) info->properties;
    status = gx_pixelmap_prompt_create(pix_prompt, info->widget_name, parent,
               props->string_id,
               props->fill_map_id,
               info->style, info->widget_id, &info->size);

    if (status == GX_SUCCESS)
    {
        gx_pixelmap_prompt_pixelmap_set(pix_prompt,
                                        props->left_map_id,
                                        props->fill_map_id,
                                        props->right_map_id,
                                        props->selected_left_map_id,
                                        props->selected_fill_map_id,
                                        props->selected_right_map_id);
        gx_prompt_font_set(prompt, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
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
GX_WINDOW_PROPERTIES Text_Rotaion_Window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Text_Rotaion_Window_Text_Window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Text_Rotaion_Window_text_window_8bpp_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Text_Rotaion_Window_text_window_4bpp_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Text_Rotaion_Window_text_window_1bpp_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Text_Rotaion_Window_dynamic_text_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES Text_Rotaion_Window_prompt_rotate_angle_properties =
{
    GX_STRING_ID_STRING_75,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PIXELMAP_PROMPT_PROPERTIES Text_Rotaion_Window_rotation_angle_value_properties =
{
    GX_STRING_ID_STRING_84,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_VAL_BACKGROUND,           /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0                                        /* selected right pixelmap id     */
};
GX_PIXELMAP_SLIDER_PROPERTIES Text_Rotaion_Window_Rotation_Angle_slider_properties =
{
    0,                                       /* minimum value                  */
    359,                                     /* maximum value                  */
    30,                                      /* current value                  */
    5,                                       /* increment                      */
    14,                                      /* minimum travel                 */
    14,                                      /* maximum travel                 */
    22,                                      /* needle width                   */
    24,                                      /* needle height                  */
    1,                                       /* needle inset                   */
    11,                                      /* needle hotspot                 */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_ACTIVE_HORIZONTAL, /* lower pixelmap id      */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_HORIZONTAL, /* upper pixelmap id             */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id        */
};
GX_CHECKBOX_PROPERTIES Text_Rotaion_Window_checkbox_auto_rotate_properties =
{
    GX_STRING_ID_STRING_76,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_CHECKBOX_PROPERTIES Text_Rotaion_Window_checkbox_auto_shift_properties =
{
    GX_STRING_ID_STRING_77,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_WINDOW_PROPERTIES Text_Rotaion_Window_color_group_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES Text_Rotaion_Window_prompt_text_color_properties =
{
    GX_STRING_ID_STRING_78,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_RADIO_BUTTON_PROPERTIES Text_Rotaion_Window_radio_button_red_properties =
{
    GX_STRING_ID_STRING_79,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Text_Rotaion_Window_radio_button_blue_properties =
{
    GX_STRING_ID_STRING_81,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Text_Rotaion_Window_radio_button_greeb_properties =
{
    GX_STRING_ID_STRING_82,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_dynamic_text_window_define =
{
    "dynamic_text_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    DYNAMIC_TEXT_WINDOW,                     /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) text_window_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {201, 80, 400, 279},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_dynamic_text_window), /* control block */
    (void *) &Text_Rotaion_Window_dynamic_text_window_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_text_window_1bpp_define =
{
    "text_window_1bpp",
    GX_TYPE_WINDOW,                          /* widget type                    */
    TEXT_WIN_1BPP,                           /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) text_window_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {69, 195, 268, 394},                     /* widget size                    */
    &Text_Rotaion_Window_dynamic_text_window_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_text_window_1bpp), /* control block */
    (void *) &Text_Rotaion_Window_text_window_1bpp_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_text_window_4bpp_define =
{
    "text_window_4bpp",
    GX_TYPE_WINDOW,                          /* widget type                    */
    TEXT_WIN_4BPP,                           /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) text_window_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {71, 65, 270, 264},                      /* widget size                    */
    &Text_Rotaion_Window_text_window_1bpp_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_text_window_4bpp), /* control block */
    (void *) &Text_Rotaion_Window_text_window_4bpp_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_text_window_8bpp_define =
{
    "text_window_8bpp",
    GX_TYPE_WINDOW,                          /* widget type                    */
    TEXT_WIN_8BPP,                           /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) text_window_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {98, 116, 402, 372},                     /* widget size                    */
    &Text_Rotaion_Window_text_window_4bpp_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_text_window_8bpp), /* control block */
    (void *) &Text_Rotaion_Window_text_window_8bpp_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_radio_button_greeb_define =
{
    "radio_button_greeb",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_GREEN_TEXT,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 363, 576, 387},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_radio_button_greeb), /* control block */
    (void *) &Text_Rotaion_Window_radio_button_greeb_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_radio_button_blue_define =
{
    "radio_button_blue",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_BLUE_TEXT,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 329, 576, 353},                    /* widget size                    */
    &Text_Rotaion_Window_radio_button_greeb_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_radio_button_blue), /* control block */
    (void *) &Text_Rotaion_Window_radio_button_blue_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_radio_button_red_define =
{
    "radio_button_red",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_RED_TEXT,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 295, 576, 319},                    /* widget size                    */
    &Text_Rotaion_Window_radio_button_blue_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_radio_button_red), /* control block */
    (void *) &Text_Rotaion_Window_radio_button_red_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_prompt_text_color_define =
{
    "prompt_text_color",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 267, 563, 290},                    /* widget size                    */
    &Text_Rotaion_Window_radio_button_red_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_prompt_text_color), /* control block */
    (void *) &Text_Rotaion_Window_prompt_text_color_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_color_group_window_define =
{
    "color_group_window",
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
    {462, 254, 613, 421},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Text_Rotaion_Window_prompt_text_color_define, /* child widget definition  */
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_color_group_window), /* control block */
    (void *) &Text_Rotaion_Window_color_group_window_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_checkbox_auto_shift_define =
{
    "checkbox_auto_shift",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    ID_AUTO_SHIFT,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CHECKBOX),                     /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_checkbox_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 202, 588, 225},                    /* widget size                    */
    &Text_Rotaion_Window_color_group_window_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_checkbox_auto_shift), /* control block */
    (void *) &Text_Rotaion_Window_checkbox_auto_shift_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_checkbox_auto_rotate_define =
{
    "checkbox_auto_rotate",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    ID_AUTO_ROTATE,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CHECKBOX),                     /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_checkbox_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 162, 588, 185},                    /* widget size                    */
    &Text_Rotaion_Window_checkbox_auto_shift_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_checkbox_auto_rotate), /* control block */
    (void *) &Text_Rotaion_Window_checkbox_auto_rotate_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_Rotation_Angle_slider_define =
{
    "Rotation_Angle_slider",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_ROTATION_ANGLE_SLIDER,                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_RAISED|GX_STYLE_ENABLED|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 100, 618, 126},                    /* widget size                    */
    &Text_Rotaion_Window_checkbox_auto_rotate_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_Rotation_Angle_slider), /* control block */
    (void *) &Text_Rotaion_Window_Rotation_Angle_slider_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_rotation_angle_value_define =
{
    "rotation_angle_value",
    GX_TYPE_PIXELMAP_PROMPT,                 /* widget type                    */
    ID_ROTATION_ANGLE_VAL,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TEXT_CENTER,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_PROMPT),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_prompt_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {573, 65, 622, 93},                      /* widget size                    */
    &Text_Rotaion_Window_Rotation_Angle_slider_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_rotation_angle_value), /* control block */
    (void *) &Text_Rotaion_Window_rotation_angle_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_prompt_rotate_angle_define =
{
    "prompt_rotate_angle",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 68, 563, 91},                      /* widget size                    */
    &Text_Rotaion_Window_rotation_angle_value_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_prompt_rotate_angle), /* control block */
    (void *) &Text_Rotaion_Window_prompt_rotate_angle_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_Text_Window_define =
{
    "Text_Window",
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
    {62, 62, 454, 426},                      /* widget size                    */
    &Text_Rotaion_Window_prompt_rotate_angle_define, /* next widget definition */
    &Text_Rotaion_Window_text_window_8bpp_define, /* child widget definition   */
    offsetof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK, Text_Rotaion_Window_Text_Window), /* control block */
    (void *) &Text_Rotaion_Window_Text_Window_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Rotaion_Window_define =
{
    "Text_Rotaion_Window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEXT_ROTAION_WINDOW_CONTROL_BLOCK), /* control block size           */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) Text_Rotation_Window_event_handler, /* event function override */
    {2, 51, 641, 426},                       /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Text_Rotaion_Window_Text_Window_define, /* child widget                   */
    0,                                       /* control block                  */
    (void *) &Text_Rotaion_Window_properties /* extended properties            */
};
GX_WINDOW_PROPERTIES Shapes_Screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Shapes_Screen_shape_type_container_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_circle_properties =
{
    GX_STRING_ID_STRING_36,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_arc_properties =
{
    GX_STRING_ID_STRING_52,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SCROLL_BUTTON,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_pie_properties =
{
    GX_STRING_ID_STRING_53,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_polygon_properties =
{
    GX_STRING_ID_STRING_54,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_ellipse_properties =
{
    GX_STRING_ID_STRING_58,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_rectangle_properties =
{
    GX_STRING_ID_STRING_65,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Shapes_Screen_radio_button_text_properties =
{
    GX_STRING_ID_STRING_74,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_WINDOW_PROPERTIES Shapes_Screen_Shapes_Window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Shapes_Screen_graphics_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES Shapes_Screen_prompt_radius_properties =
{
    GX_STRING_ID_STRING_71,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PIXELMAP_PROMPT_PROPERTIES Shapes_Screen_radius_val_properties =
{
    GX_STRING_ID_STRING_68,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_VAL_BACKGROUND,           /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0                                        /* selected right pixelmap id     */
};
GX_PIXELMAP_SLIDER_PROPERTIES Shapes_Screen_radius_slider_properties =
{
    5,                                       /* minimum value                  */
    200,                                     /* maximum value                  */
    160,                                     /* current value                  */
    1,                                       /* increment                      */
    14,                                      /* minimum travel                 */
    14,                                      /* maximum travel                 */
    22,                                      /* needle width                   */
    24,                                      /* needle height                  */
    1,                                       /* needle inset                   */
    11,                                      /* needle hotspot                 */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_ACTIVE_HORIZONTAL, /* lower pixelmap id      */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_HORIZONTAL, /* upper pixelmap id             */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id        */
};
GX_PROMPT_PROPERTIES Shapes_Screen_prompt_brush_width_properties =
{
    GX_STRING_ID_STRING_70,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PIXELMAP_PROMPT_PROPERTIES Shapes_Screen_brush_width_val_properties =
{
    GX_STRING_ID_STRING_64,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_VAL_BACKGROUND,           /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0                                        /* selected right pixelmap id     */
};
GX_PIXELMAP_SLIDER_PROPERTIES Shapes_Screen_brush_width_slider_properties =
{
    0,                                       /* minimum value                  */
    20,                                      /* maximum value                  */
    2,                                       /* current value                  */
    1,                                       /* increment                      */
    14,                                      /* minimum travel                 */
    14,                                      /* maximum travel                 */
    22,                                      /* needle width                   */
    24,                                      /* needle height                  */
    0,                                       /* needle inset                   */
    11,                                      /* needle hotspot                 */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_ACTIVE_HORIZONTAL, /* lower pixelmap id      */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_HORIZONTAL, /* upper pixelmap id             */
    GX_PIXELMAP_ID_GX_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id        */
};
GX_PROMPT_PROPERTIES Shapes_Screen_prompt_brush_style_properties =
{
    GX_STRING_ID_STRING_72,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_TEAL,                        /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_CHECKBOX_PROPERTIES Shapes_Screen_checkbox_anti_alised_properties =
{
    GX_STRING_ID_STRING_20,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_CHECKBOX_PROPERTIES Shapes_Screen_checkbox_round_end_properties =
{
    GX_STRING_ID_STRING_67,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_CHECKBOX_PROPERTIES Shapes_Screen_checkbox_fill_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_CHECKBOX_PROPERTIES Shapes_Screen_checkbox_pixelmap_fill_properties =
{
    GX_STRING_ID_STRING_38,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_DARK_TEAL,                   /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_ICON_PROPERTIES Shapes_Screen_express_icon_properties =
{
    GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_text_define =
{
    "radio_button_text",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_TEXT,                                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {559, 429, 627, 453},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_text), /* control block */
    (void *) &Shapes_Screen_radio_button_text_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_rectangle_define =
{
    "radio_button_rectangle",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_RECTANGLE,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {333, 429, 438, 453},                    /* widget size                    */
    &Shapes_Screen_radio_button_text_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_rectangle), /* control block */
    (void *) &Shapes_Screen_radio_button_rectangle_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_ellipse_define =
{
    "radio_button_ellipse",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_ELLIPSE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {98, 429, 183, 453},                     /* widget size                    */
    &Shapes_Screen_radio_button_rectangle_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_ellipse), /* control block */
    (void *) &Shapes_Screen_radio_button_ellipse_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_polygon_define =
{
    "radio_button_polygon",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_POLYGON,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {450, 429, 547, 453},                    /* widget size                    */
    &Shapes_Screen_radio_button_ellipse_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_polygon), /* control block */
    (void *) &Shapes_Screen_radio_button_polygon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_pie_define =
{
    "radio_button_pie",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PIE,                                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {264, 429, 321, 453},                    /* widget size                    */
    &Shapes_Screen_radio_button_polygon_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_pie), /* control block */
    (void *) &Shapes_Screen_radio_button_pie_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_arc_define =
{
    "radio_button_arc",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_ARC,                                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {195, 429, 252, 453},                    /* widget size                    */
    &Shapes_Screen_radio_button_pie_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_arc), /* control block */
    (void *) &Shapes_Screen_radio_button_arc_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radio_button_circle_define =
{
    "radio_button_circle",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_CIRCLE,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {12, 426, 86, 457},                      /* widget size                    */
    &Shapes_Screen_radio_button_arc_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radio_button_circle), /* control block */
    (void *) &Shapes_Screen_radio_button_circle_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_checkbox_pixelmap_fill_define =
{
    "checkbox_pixelmap_fill",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    ID_PIXELMAP_FILL,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CHECKBOX),                     /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_checkbox_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 369, 596, 392},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_checkbox_pixelmap_fill), /* control block */
    (void *) &Shapes_Screen_checkbox_pixelmap_fill_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_checkbox_fill_define =
{
    "checkbox_fill",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    ID_SOLID_FILL,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CHECKBOX),                     /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_checkbox_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 333, 591, 356},                    /* widget size                    */
    &Shapes_Screen_checkbox_pixelmap_fill_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_checkbox_fill), /* control block */
    (void *) &Shapes_Screen_checkbox_fill_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_checkbox_round_end_define =
{
    "checkbox_round_end",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    ID_ROUND_END,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CHECKBOX),                     /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_checkbox_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 297, 591, 320},                    /* widget size                    */
    &Shapes_Screen_checkbox_fill_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_checkbox_round_end), /* control block */
    (void *) &Shapes_Screen_checkbox_round_end_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_checkbox_anti_alised_define =
{
    "checkbox_anti_alised",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    ID_ANTI_ALIASED,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CHECKBOX),                     /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_checkbox_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 261, 591, 284},                    /* widget size                    */
    &Shapes_Screen_checkbox_round_end_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_checkbox_anti_alised), /* control block */
    (void *) &Shapes_Screen_checkbox_anti_alised_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_prompt_brush_style_define =
{
    "prompt_brush_style",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {466, 225, 557, 248},                    /* widget size                    */
    &Shapes_Screen_checkbox_anti_alised_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_prompt_brush_style), /* control block */
    (void *) &Shapes_Screen_prompt_brush_style_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_brush_width_slider_define =
{
    "brush_width_slider",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_BRUSH_WIDTH,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 170, 619, 196},                    /* widget size                    */
    &Shapes_Screen_prompt_brush_style_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_brush_width_slider), /* control block */
    (void *) &Shapes_Screen_brush_width_slider_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_brush_width_val_define =
{
    "brush_width_val",
    GX_TYPE_PIXELMAP_PROMPT,                 /* widget type                    */
    ID_BRUSH_WIDTH_VAL,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TEXT_CENTER,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_PROMPT),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_prompt_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {572, 139, 621, 167},                    /* widget size                    */
    &Shapes_Screen_brush_width_slider_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_brush_width_val), /* control block */
    (void *) &Shapes_Screen_brush_width_val_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_prompt_brush_width_define =
{
    "prompt_brush_width",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 139, 559, 162},                    /* widget size                    */
    &Shapes_Screen_brush_width_val_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_prompt_brush_width), /* control block */
    (void *) &Shapes_Screen_prompt_brush_width_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radius_slider_define =
{
    "radius_slider",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_RADIUS_SLIDER,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_RAISED|GX_STYLE_ENABLED|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {462, 76, 617, 102},                     /* widget size                    */
    &Shapes_Screen_prompt_brush_width_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radius_slider), /* control block */
    (void *) &Shapes_Screen_radius_slider_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_radius_val_define =
{
    "radius_val",
    GX_TYPE_PIXELMAP_PROMPT,                 /* widget type                    */
    ID_RADIUS_PROMPT_VAL,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TEXT_CENTER,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_PROMPT),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_prompt_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {572, 46, 621, 74},                      /* widget size                    */
    &Shapes_Screen_radius_slider_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_radius_val), /* control block */
    (void *) &Shapes_Screen_radius_val_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_prompt_radius_define =
{
    "prompt_radius",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {463, 46, 518, 69},                      /* widget size                    */
    &Shapes_Screen_radius_val_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_prompt_radius), /* control block */
    (void *) &Shapes_Screen_prompt_radius_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_graphics_window_define =
{
    "graphics_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_GRAPHICS_WINDOW,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) graphics_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {60, 46, 452, 410},                      /* widget size                    */
    &Shapes_Screen_prompt_radius_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_graphics_window), /* control block */
    (void *) &Shapes_Screen_graphics_window_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_express_icon_define =
{
    "express_icon",
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
    {12, 12, 200, 41},                       /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_express_icon), /* control block */
    (void *) &Shapes_Screen_express_icon_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_Shapes_Window_define =
{
    "Shapes_Window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SHAPES_WINDOW,                        /* widget id                      */
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
    {0, 35, 639, 410},                       /* widget size                    */
    &Shapes_Screen_express_icon_define,      /* next widget definition         */
    &Shapes_Screen_graphics_window_define,   /* child widget definition        */
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_Shapes_Window), /* control block */
    (void *) &Shapes_Screen_Shapes_Window_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_shape_type_container_define =
{
    "shape_type_container",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {7, 419, 635, 462},                      /* widget size                    */
    &Shapes_Screen_Shapes_Window_define,     /* next widget definition         */
    &Shapes_Screen_radio_button_circle_define, /* child widget definition      */
    offsetof(SHAPES_SCREEN_CONTROL_BLOCK, Shapes_Screen_shape_type_container), /* control block */
    (void *) &Shapes_Screen_shape_type_container_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Shapes_Screen_define =
{
    "Shapes_Screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SHAPES_SCREEN,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SHAPES_SCREEN_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) ShapesScreen_event_handler, /* event function override */
    {1, 0, 639, 478},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Shapes_Screen_shape_type_container_define, /* child widget                */
    0,                                       /* control block                  */
    (void *) &Shapes_Screen_properties       /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY guix_shapes_widget_table[] =
{
    { &Text_Rotaion_Window_define, (GX_WIDGET *) &Text_Rotaion_Window },
    { &Shapes_Screen_define, (GX_WIDGET *) &Shapes_Screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = guix_shapes_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &guix_shapes_display_table[display];


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

/******************************************************************************************/
/* Calculate string length.                                                               */
/******************************************************************************************/
UINT string_length_get(GX_CONST GX_CHAR* input_string, UINT max_string_length)
{
    UINT length = 0;

    if (input_string)
    {
        /* Traverse the string.  */
        for (length = 0; input_string[length]; length++)
        {
            /* Check if the string length is bigger than the max string length.  */
            if (length >= max_string_length)
            {
                break;
            }
        }
    }

    return length;
}

#undef GUIX_STUDIO_GENERATED_FILE
