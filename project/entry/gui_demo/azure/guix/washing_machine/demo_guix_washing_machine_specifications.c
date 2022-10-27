#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "demo_guix_washing_machine_resources.h"
#include "demo_guix_washing_machine_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
MODE_SELECT_WINDOW_CONTROL_BLOCK mode_select_window;
WATER_LEVEL_WINDOW_CONTROL_BLOCK water_level_window;
TEMPERATURE_WINDOW_CONTROL_BLOCK temperature_window;
WASH_CYCLE_WINDOW_CONTROL_BLOCK wash_cycle_window;
MAIN_SCREEN_CONTROL_BLOCK main_screen;
GX_DISPLAY main_display_control_block;
GX_WINDOW_ROOT main_display_root_window;
GX_CANVAS  main_display_canvas_control_block;
ULONG      main_display_canvas_memory[307200];

extern GX_CONST GX_THEME *main_display_theme_table[];
extern GX_CONST GX_STRING *main_display_language_table[];

GX_STUDIO_DISPLAY_INFO demo_guix_washing_machine_display_table[1] =
{
    {
    "main_display",
    "main_display_canvas",
    main_display_theme_table,
    main_display_language_table,
    MAIN_DISPLAY_THEME_TABLE_SIZE,
    MAIN_DISPLAY_LANGUAGE_TABLE_SIZE,
    MAIN_DISPLAY_STRING_TABLE_SIZE,
    640,                                     /* x resolution                   */
    480,                                     /* y resolution                   */
    &main_display_control_block,
    &main_display_canvas_control_block,
    &main_display_root_window,
    main_display_canvas_memory,              /* canvas memory area             */
    1228800,                                 /* canvas memory size in bytes    */
    GX_SCREEN_ROTATION_NONE                  /* rotation angle                 */
    }
};


UINT gx_studio_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_BUTTON *button = (GX_BUTTON *) control_block;
    status = gx_button_create(button, info->widget_name, parent, info->style, info->widget_id, &info->size);
    return status;
}

UINT gx_studio_text_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_TEXT_BUTTON *button = (GX_TEXT_BUTTON *) control_block;
    GX_TEXT_BUTTON_PROPERTIES *props = (GX_TEXT_BUTTON_PROPERTIES *) info->properties;
    status = gx_text_button_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set(button, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_text_button_text_color_set(button, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_text_button_text_color_set(button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
    }
    return status;
}

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

UINT gx_studio_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PROGRESS_BAR *bar = (GX_PROGRESS_BAR *) control_block;
    GX_PROGRESS_BAR_INFO *bar_info = (GX_PROGRESS_BAR_INFO *) info->properties;
    status = gx_progress_bar_create(bar,
                    info->widget_name,
                    parent,
                    bar_info,
                    info->style,
                    info->widget_id,
                    &info->size);
    return status;
}

UINT gx_studio_radial_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_RADIAL_PROGRESS_BAR *bar = (GX_RADIAL_PROGRESS_BAR *) control_block;
    GX_RADIAL_PROGRESS_BAR_INFO *bar_info = (GX_RADIAL_PROGRESS_BAR_INFO *) info->properties;
    status = gx_radial_progress_bar_create(bar,
                    info->widget_name,
                    parent,
                    bar_info,
                    info->style,
                    info->widget_id);
    return status;
}

UINT gx_studio_radial_slider_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_RADIAL_SLIDER *slider = (GX_RADIAL_SLIDER *) control_block;
    GX_RADIAL_SLIDER_INFO slider_info;
    GX_RADIAL_SLIDER_PROPERTIES *props = (GX_RADIAL_SLIDER_PROPERTIES *)info->properties;
    memset(&slider_info, 0, sizeof(GX_RADIAL_SLIDER_INFO));
    slider_info.gx_radial_slider_info_xcenter = props->xcenter;
    slider_info.gx_radial_slider_info_ycenter = props->ycenter;
    slider_info.gx_radial_slider_info_radius = props->radius;
    slider_info.gx_radial_slider_info_track_width = props->track_width;
    slider_info.gx_radial_slider_info_needle_offset = props->needle_offset;
    slider_info.gx_radial_slider_info_current_angle = props->current_angle;
    slider_info.gx_radial_slider_info_min_angle = props->min_angle;
    slider_info.gx_radial_slider_info_max_angle = props->max_angle;
    slider_info.gx_radial_slider_info_background_pixelmap = props->background_pixelmap;
    slider_info.gx_radial_slider_info_needle_pixelmap = props->needle_pixelmap;
    status = gx_radial_slider_create(slider,
                    info->widget_name,
                    parent,
                    &slider_info,
                    info->style,
                    info->widget_id,
                    &info->size);
    gx_radial_slider_animation_set(slider, props->animation_total_steps, props->animation_delay, props->animation_style, props->animation_update_callback);
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

UINT gx_studio_numeric_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_NUMERIC_PROMPT *prompt = (GX_NUMERIC_PROMPT *) control_block;
    GX_NUMERIC_PROMPT_PROPERTIES *props = (GX_NUMERIC_PROMPT_PROPERTIES *) info->properties;
    status = gx_numeric_prompt_create(prompt, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_prompt_font_set((GX_PROMPT *)prompt, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_prompt_text_color_set((GX_PROMPT *)prompt, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_prompt_text_color_set((GX_PROMPT *)prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
        if(!props->string_id)
        {
            gx_numeric_prompt_value_set(prompt, props->numeric_prompt_value);
        }
        if(props->format_func)
        {
            gx_numeric_prompt_format_function_set(prompt, props->format_func);
        }
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

UINT gx_studio_template_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status = GX_SUCCESS;
    GX_STUDIO_WIDGET derived;
    GX_TEMPLATE_PROPERTIES *props = (GX_TEMPLATE_PROPERTIES *) info->properties;

                                             /* create base widget and children */ 
    derived =            *props->base_info;
    derived.widget_id =   info->widget_id;
    derived.widget_name = info->widget_name;

    if (info->style & GX_STYLE_ENABLED)
    {
        derived.style |= GX_STYLE_ENABLED;
    }
    else
    {
        derived.style &= ~GX_STYLE_ENABLED;
    }
    if (info->status & GX_STATUS_ACCEPTS_FOCUS)
    {
        derived.status |= GX_STATUS_ACCEPTS_FOCUS;
    }
    else
    {
        derived.status &= ~GX_STATUS_ACCEPTS_FOCUS;
    }

    #if defined(GX_WIDGET_USER_DATA)
    derived.user_data =   info->user_data;
    #endif

    control_block = gx_studio_widget_create((GX_BYTE *) control_block, &derived, parent);
    if (control_block)
    {
        if (info->style & GX_STYLE_DYNAMICALLY_ALLOCATED)
        {
            control_block->gx_widget_status |= GX_STATUS_DYNAMICALLY_ALLOCATED;
        }
        if (derived.size.gx_rectangle_left != info->size.gx_rectangle_left ||
            derived.size.gx_rectangle_top != info->size.gx_rectangle_top)
        {
            gx_widget_shift(control_block,
                info->size.gx_rectangle_left - derived.size.gx_rectangle_left,
                info->size.gx_rectangle_top - derived.size.gx_rectangle_top, GX_FALSE);
        }
    }
    else
    {
        status = GX_FAILURE;
    }
    return status;
}
GX_PIXELMAP_BUTTON_PROPERTIES template_mode_btn_small_properties =
{
    GX_PIXELMAP_ID_CYCLE_QUICK_SMALL,        /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES template_mode_btn_small_mode_label_properties =
{
    GX_STRING_ID_QUICK,                      /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES template_mode_btn_small_mode_label_4_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES template_mode_btn_small_total_time_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    30                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES template_mode_btn_small_mode_label_5_properties =
{
    GX_STRING_ID_STRING_12,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_small_mode_label_5_define =
{
    "mode_label_5",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {118, 161, 146, 178},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK, template_mode_btn_small_mode_label_5), /* control block */
    (void *) &template_mode_btn_small_mode_label_5_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_small_total_time_define =
{
    "total_time",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {81, 157, 110, 181},                     /* widget size                    */
    &template_mode_btn_small_mode_label_5_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK, template_mode_btn_small_total_time), /* control block */
    (void *) &template_mode_btn_small_total_time_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_small_mode_label_4_define =
{
    "mode_label_4",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {80, 126, 146, 150},                     /* widget size                    */
    &template_mode_btn_small_total_time_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK, template_mode_btn_small_mode_label_4), /* control block */
    (void *) &template_mode_btn_small_mode_label_4_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_small_mode_label_define =
{
    "mode_label",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {62, 68, 160, 103},                      /* widget size                    */
    &template_mode_btn_small_mode_label_4_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK, template_mode_btn_small_mode_label), /* control block */
    (void *) &template_mode_btn_small_mode_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_small_define =
{
    "template_mode_btn_small",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 223, 223},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &template_mode_btn_small_mode_label_define, /* child widget                */
    0,                                       /* control block                  */
    (void *) &template_mode_btn_small_properties /* extended properties        */
};
GX_PIXELMAP_BUTTON_PROPERTIES template_mode_btn_properties =
{
    GX_PIXELMAP_ID_CYCLE_QUICK,              /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES template_mode_btn_mode_label_properties =
{
    GX_STRING_ID_QUICK,                      /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES template_mode_btn_wash_label_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES template_mode_btn_min_label_properties =
{
    GX_STRING_ID_STRING_12,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES template_mode_btn_total_time_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    30                                       /* numeric prompt value           */
};
GX_RADIAL_PROGRESS_BAR_INFO template_mode_btn_mode_progress_bar_properties =
{
    159,                                     /* xcenter                        */
    158,                                     /* ycenter                        */
    122,                                     /* radius                         */
    -270,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    5,                                       /* normal brush width             */
    5,                                       /* selected brush width           */
    GX_COLOR_ID_SLIDER_NEEDLE_FILL,          /* normal brush color             */
    GX_COLOR_ID_SHINE,                       /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES template_mode_btn_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    60                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES template_mode_btn_second_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    60                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES template_mode_btn_prompt_3_properties =
{
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES template_mode_btn_prompt_2_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_prompt_2_define =
{
    "prompt_2",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {174, 57, 198, 74},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_prompt_2), /* control block */
    (void *) &template_mode_btn_prompt_2_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_prompt_3_define =
{
    "prompt_3",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {146, 55, 149, 72},                      /* widget size                    */
    &template_mode_btn_prompt_2_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_prompt_3), /* control block */
    (void *) &template_mode_btn_prompt_3_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_second_define =
{
    "second",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {152, 56, 169, 73},                      /* widget size                    */
    &template_mode_btn_prompt_3_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_second), /* control block */
    (void *) &template_mode_btn_second_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_minute_define =
{
    "minute",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {127, 56, 144, 73},                      /* widget size                    */
    &template_mode_btn_second_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_minute), /* control block */
    (void *) &template_mode_btn_minute_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_mode_progress_bar_define =
{
    "mode_progress_bar",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_NO_BACKTRACK|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {34, 33, 284, 283},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &template_mode_btn_minute_define,        /* child widget definition        */
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_mode_progress_bar), /* control block */
    (void *) &template_mode_btn_mode_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_total_time_define =
{
    "total_time",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {114, 227, 157, 262},                    /* widget size                    */
    &template_mode_btn_mode_progress_bar_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_total_time), /* control block */
    (void *) &template_mode_btn_total_time_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_min_label_define =
{
    "min_label",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {166, 233, 210, 257},                    /* widget size                    */
    &template_mode_btn_total_time_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_min_label), /* control block */
    (void *) &template_mode_btn_min_label_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_wash_label_define =
{
    "wash_label",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {112, 178, 207, 213},                    /* widget size                    */
    &template_mode_btn_min_label_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_wash_label), /* control block */
    (void *) &template_mode_btn_wash_label_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_mode_label_define =
{
    "mode_label",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {72, 87, 241, 148},                      /* widget size                    */
    &template_mode_btn_wash_label_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_MODE_BTN_CONTROL_BLOCK, template_mode_btn_mode_label), /* control block */
    (void *) &template_mode_btn_mode_label_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET template_mode_btn_define =
{
    "template_mode_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 316, 317},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &template_mode_btn_mode_label_define,    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &template_mode_btn_properties   /* extended properties            */
};
GX_WINDOW_PROPERTIES mode_select_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES mode_select_window_btn_quick_properties =
{
    &template_mode_btn_define,               /* base info                      */
    gx_studio_pixelmap_button_create,        /* base create function           */
    {8, 82, 324, 399}                        /* widget size                    */
};
GX_TEMPLATE_PROPERTIES mode_select_window_btn_deep_properties =
{
    &template_mode_btn_define,               /* base info                      */
    gx_studio_pixelmap_button_create,        /* base create function           */
    {316, 82, 632, 399}                      /* widget size                    */
};
GX_TEMPLATE_PROPERTIES mode_select_window_btn_quick_small_properties =
{
    &template_mode_btn_small_define,         /* base info                      */
    gx_studio_pixelmap_button_create,        /* base create function           */
    {9, 129, 232, 352}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES mode_select_window_btn_deep_small_properties =
{
    &template_mode_btn_small_define,         /* base info                      */
    gx_studio_pixelmap_button_create,        /* base create function           */
    {411, 129, 634, 352}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES mode_select_window_btn_eco_properties =
{
    &template_mode_btn_define,               /* base info                      */
    gx_studio_pixelmap_button_create,        /* base create function           */
    {163, 82, 479, 399}                      /* widget size                    */
};
GX_TEMPLATE_PROPERTIES mode_select_window_btn_eco_small_properties =
{
    &template_mode_btn_small_define,         /* base info                      */
    gx_studio_pixelmap_button_create,        /* base create function           */
    {210, 129, 433, 352}                     /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_btn_eco_small_define =
{
    "btn_eco_small",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BTN_ECO_SMALL,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {210, 129, 433, 352},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MODE_SELECT_WINDOW_CONTROL_BLOCK, mode_select_window_btn_eco_small), /* control block */
    (void *) &mode_select_window_btn_eco_small_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_btn_eco_define =
{
    "btn_eco",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BTN_ECO,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {163, 82, 479, 399},                     /* widget size                    */
    &mode_select_window_btn_eco_small_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MODE_SELECT_WINDOW_CONTROL_BLOCK, mode_select_window_btn_eco), /* control block */
    (void *) &mode_select_window_btn_eco_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_btn_deep_small_define =
{
    "btn_deep_small",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BTN_DEEP_SMALL,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {411, 129, 634, 352},                    /* widget size                    */
    &mode_select_window_btn_eco_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MODE_SELECT_WINDOW_CONTROL_BLOCK, mode_select_window_btn_deep_small), /* control block */
    (void *) &mode_select_window_btn_deep_small_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_btn_quick_small_define =
{
    "btn_quick_small",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BTN_QUICK_SMALL,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {9, 129, 232, 352},                      /* widget size                    */
    &mode_select_window_btn_deep_small_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MODE_SELECT_WINDOW_CONTROL_BLOCK, mode_select_window_btn_quick_small), /* control block */
    (void *) &mode_select_window_btn_quick_small_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_btn_deep_define =
{
    "btn_deep",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BTN_DEEP,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {316, 82, 632, 399},                     /* widget size                    */
    &mode_select_window_btn_quick_small_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MODE_SELECT_WINDOW_CONTROL_BLOCK, mode_select_window_btn_deep), /* control block */
    (void *) &mode_select_window_btn_deep_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_btn_quick_define =
{
    "btn_quick",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BTN_QUICK,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_MODE_BTN_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {8, 82, 324, 399},                       /* widget size                    */
    &mode_select_window_btn_deep_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MODE_SELECT_WINDOW_CONTROL_BLOCK, mode_select_window_btn_quick), /* control block */
    (void *) &mode_select_window_btn_quick_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET mode_select_window_define =
{
    "mode_select_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MODE_SELECT_WINDOW_CONTROL_BLOCK), /* control block size            */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) mode_select_window_event_process, /* event function override */
    {0, 82, 639, 399},                       /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &mode_select_window_btn_quick_define,    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &mode_select_window_properties  /* extended properties            */
};
GX_WINDOW_PROPERTIES water_level_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_RADIAL_SLIDER_PROPERTIES water_level_window_water_level_slider_properties =
{
    159,                                     /* xcenter                        */
    159,                                     /* ycenter                        */
    140,                                     /* radius                         */
    20,                                      /* track width                    */
    0,                                       /* needle offset                  */
    -20,                                     /* current angle                  */
    -40,                                     /* min angle                      */
    40,                                      /* max angle                      */
    GX_PIXELMAP_ID_DISC_WATER_LEVEL,         /* background pixelmap            */
    GX_PIXELMAP_ID_METALIC_DIAL,             /* needle pixelmap                */
    10,                                      /* animation total steps          */
    2,                                       /* animation delay                */
    GX_ANIMATION_CIRC_EASE_IN_OUT,           /* animation style                */
    GX_NULL,                                 /* animation update callback func */
};
GX_NUMERIC_PROMPT_PROPERTIES water_level_window_water_level_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARG_NUMBER,                  /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    35                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES water_level_window_percentage_flag_properties =
{
    GX_STRING_ID_STRING_38,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES water_level_window_btn_max_properties =
{
    GX_STRING_ID_STRING_21,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES water_level_window_btn_high_properties =
{
    GX_STRING_ID_STRING_20,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES water_level_window_btn_medium_properties =
{
    GX_STRING_ID_STRING_22,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES water_level_window_btn_low_properties =
{
    GX_STRING_ID_STRING_23,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES water_level_window_btn_min_properties =
{
    GX_STRING_ID_STRING_24,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_percentage_flag_define =
{
    "percentage_flag",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {374, 201, 396, 225},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_percentage_flag), /* control block */
    (void *) &water_level_window_percentage_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_water_level_value_define =
{
    "water_level_value",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {255, 207, 372, 278},                    /* widget size                    */
    &water_level_window_percentage_flag_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_water_level_value), /* control block */
    (void *) &water_level_window_water_level_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_btn_min_define =
{
    "btn_min",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_MIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {443, 343, 485, 367},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_btn_min), /* control block */
    (void *) &water_level_window_btn_min_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_btn_low_define =
{
    "btn_low",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_LOW,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {477, 287, 524, 311},                    /* widget size                    */
    &water_level_window_btn_min_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_btn_low), /* control block */
    (void *) &water_level_window_btn_low_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_btn_medium_define =
{
    "btn_medium",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_MEDIUM,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {485, 232, 580, 256},                    /* widget size                    */
    &water_level_window_btn_low_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_btn_medium), /* control block */
    (void *) &water_level_window_btn_medium_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_btn_high_define =
{
    "btn_high",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_HIGH,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {478, 177, 532, 201},                    /* widget size                    */
    &water_level_window_btn_medium_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_btn_high), /* control block */
    (void *) &water_level_window_btn_high_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_btn_max_define =
{
    "btn_max",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_MAX,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {443, 121, 495, 145},                    /* widget size                    */
    &water_level_window_btn_high_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_btn_max), /* control block */
    (void *) &water_level_window_btn_max_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_water_level_slider_define =
{
    "water_level_slider",
    GX_TYPE_RADIAL_SLIDER,                   /* widget type                    */
    ID_WATER_LEVEL_SLIDER,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_SLIDER),                /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_slider_create,          /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {160, 80, 477, 397},                     /* widget size                    */
    &water_level_window_btn_max_define,      /* next widget definition         */
    &water_level_window_water_level_value_define, /* child widget definition   */
    offsetof(WATER_LEVEL_WINDOW_CONTROL_BLOCK, water_level_window_water_level_slider), /* control block */
    (void *) &water_level_window_water_level_slider_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET water_level_window_define =
{
    "water_level_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WATER_LEVEL_WINDOW_CONTROL_BLOCK), /* control block size            */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) water_level_window_event_process, /* event function override */
    {160, 80, 628, 397},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &water_level_window_water_level_slider_define, /* child widget             */
    0,                                       /* control block                  */
    (void *) &water_level_window_properties  /* extended properties            */
};
GX_WINDOW_PROPERTIES temperature_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_RADIAL_SLIDER_PROPERTIES temperature_window_temperature_slider_properties =
{
    159,                                     /* xcenter                        */
    159,                                     /* ycenter                        */
    120,                                     /* radius                         */
    40,                                      /* track width                    */
    20,                                      /* needle offset                  */
    30,                                      /* current angle                  */
    -30,                                     /* min angle                      */
    210,                                     /* max angle                      */
    GX_PIXELMAP_ID_DISC_TEMPERATURE_TEXT,    /* background pixelmap            */
    GX_PIXELMAP_ID_METALIC_DIAL,             /* needle pixelmap                */
    10,                                      /* animation total steps          */
    2,                                       /* animation delay                */
    GX_ANIMATION_CIRC_EASE_IN_OUT,           /* animation style                */
    temperature_slider_animation_callback,   /* animation update callback func */
};
GX_NUMERIC_PROMPT_PROPERTIES temperature_window_temperature_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARG_NUMBER,                  /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES temperature_window_temperature_flag_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET temperature_window_temperature_flag_define =
{
    "temperature_flag",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {373, 204, 387, 228},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPERATURE_WINDOW_CONTROL_BLOCK, temperature_window_temperature_flag), /* control block */
    (void *) &temperature_window_temperature_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET temperature_window_temperature_value_define =
{
    "temperature_value",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {259, 219, 372, 286},                    /* widget size                    */
    &temperature_window_temperature_flag_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPERATURE_WINDOW_CONTROL_BLOCK, temperature_window_temperature_value), /* control block */
    (void *) &temperature_window_temperature_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET temperature_window_temperature_slider_define =
{
    "temperature_slider",
    GX_TYPE_RADIAL_SLIDER,                   /* widget type                    */
    ID_TEMPERATURE_SLIDER,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_SLIDER),                /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_slider_create,          /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {160, 80, 477, 397},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &temperature_window_temperature_value_define, /* child widget definition   */
    offsetof(TEMPERATURE_WINDOW_CONTROL_BLOCK, temperature_window_temperature_slider), /* control block */
    (void *) &temperature_window_temperature_slider_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET temperature_window_define =
{
    "temperature_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPERATURE_WINDOW_CONTROL_BLOCK), /* control block size            */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) temperature_window_draw, /* drawing function override */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) temperature_window_event_process, /* event function override */
    {160, 80, 477, 397},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &temperature_window_temperature_slider_define, /* child widget             */
    0,                                       /* control block                  */
    (void *) &temperature_window_properties  /* extended properties            */
};
GX_WINDOW_PROPERTIES wash_cycle_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_RADIAL_SLIDER_PROPERTIES wash_cycle_window_wash_cycle_slider_properties =
{
    159,                                     /* xcenter                        */
    159,                                     /* ycenter                        */
    139,                                     /* radius                         */
    20,                                      /* track width                    */
    0,                                       /* needle offset                  */
    100,                                     /* current angle                  */
    -17,                                     /* min angle                      */
    196,                                     /* max angle                      */
    GX_PIXELMAP_ID_DISC_CYCLE_WITH_PROGRESS_BG_TEXT, /* background pixelmap    */
    0,                                       /* needle pixelmap                */
    1,                                       /* animation total steps          */
    0,                                       /* animation delay                */
    GX_ANIMATION_CIRC_EASE_IN_OUT,           /* animation style                */
    GX_NULL,                                 /* animation update callback func */
};
GX_NUMERIC_PROMPT_PROPERTIES wash_cycle_window_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    35                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES wash_cycle_window_second_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    35                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES wash_cycle_window_icon_1_properties =
{
    GX_PIXELMAP_ID_CYCLE_PROGRESS_DOT,       /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES wash_cycle_window_icon_2_properties =
{
    GX_PIXELMAP_ID_CYCLE_PROGRESS_DOT,       /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET wash_cycle_window_icon_2_define =
{
    "icon_2",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {314, 248, 318, 252},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WASH_CYCLE_WINDOW_CONTROL_BLOCK, wash_cycle_window_icon_2), /* control block */
    (void *) &wash_cycle_window_icon_2_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET wash_cycle_window_icon_1_define =
{
    "icon_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {314, 227, 318, 231},                    /* widget size                    */
    &wash_cycle_window_icon_2_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WASH_CYCLE_WINDOW_CONTROL_BLOCK, wash_cycle_window_icon_1), /* control block */
    (void *) &wash_cycle_window_icon_1_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET wash_cycle_window_second_define =
{
    "second",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 213, 399, 274},                    /* widget size                    */
    &wash_cycle_window_icon_1_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WASH_CYCLE_WINDOW_CONTROL_BLOCK, wash_cycle_window_second), /* control block */
    (void *) &wash_cycle_window_second_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET wash_cycle_window_minute_define =
{
    "minute",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {236, 212, 311, 273},                    /* widget size                    */
    &wash_cycle_window_second_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WASH_CYCLE_WINDOW_CONTROL_BLOCK, wash_cycle_window_minute), /* control block */
    (void *) &wash_cycle_window_minute_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET wash_cycle_window_wash_cycle_slider_define =
{
    "wash_cycle_slider",
    GX_TYPE_RADIAL_SLIDER,                   /* widget type                    */
    ID_WASH_CYCLE_SLIDER,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_SLIDER),                /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_slider_create,          /* create function                */
    (VOID (*)(GX_WIDGET *)) wash_cycle_slider_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {160, 80, 477, 397},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &wash_cycle_window_minute_define,        /* child widget definition        */
    offsetof(WASH_CYCLE_WINDOW_CONTROL_BLOCK, wash_cycle_window_wash_cycle_slider), /* control block */
    (void *) &wash_cycle_window_wash_cycle_slider_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET wash_cycle_window_define =
{
    "wash_cycle_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WASH_CYCLE_WINDOW_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_TEXT,                        /* selected color id              */
    GX_COLOR_ID_TEXT,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) wash_cycle_window_draw, /* drawing function override */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) wash_cycle_window_event_process, /* event function override */
    {160, 80, 477, 397},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &wash_cycle_window_wash_cycle_slider_define, /* child widget               */
    0,                                       /* control block                  */
    (void *) &wash_cycle_window_properties   /* extended properties            */
};
GX_WINDOW_PROPERTIES main_screen_properties =
{
    GX_PIXELMAP_ID_BG                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES main_screen_logo_properties =
{
    GX_PIXELMAP_ID_MICROSOFT_AZURE_LOGO,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_wash_cycle_mode_properties =
{
    GX_STRING_ID_STRING_2,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_wash_cycle_lable_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_temperature_lable_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_temperature_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_4_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_water_level_label_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_water_level_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    35                                       /* numeric prompt value           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_btn_play_properties =
{
    GX_PIXELMAP_ID_PLAY,                     /* normal pixelmap id             */
    GX_PIXELMAP_ID_PAUSE,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROGRESS_BAR_INFO main_screen_wash_cycle_progress_bar_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    17,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    0                                        /* fill pixelmap                  */
};
GX_PROMPT_PROPERTIES main_screen_label_1_soak_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_label_2_spin_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_label_3_wash_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_label_4_spin_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_label_5_rinse_properties =
{
    GX_STRING_ID_STRING_15,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_label_6_spin_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_wash_cycle_lable_define =
{
    "wash_cycle_lable",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_text_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {376, 65, 444, 77},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_wash_cycle_lable), /* control block */
    (void *) &main_screen_wash_cycle_lable_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET main_screen_wash_cycle_mode_define =
{
    "wash_cycle_mode",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_text_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {397, 41, 439, 58},                      /* widget size                    */
    &main_screen_wash_cycle_lable_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_wash_cycle_mode), /* control block */
    (void *) &main_screen_wash_cycle_mode_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET main_screen_temperature_value_define =
{
    "temperature_value",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_numeric_text_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {489, 41, 510, 62},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_temperature_value), /* control block */
    (void *) &main_screen_temperature_value_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_temperature_lable_define =
{
    "temperature_lable",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_text_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {453, 65, 530, 77},                      /* widget size                    */
    &main_screen_temperature_value_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_temperature_lable), /* control block */
    (void *) &main_screen_temperature_lable_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_2_define =
{
    "prompt_2",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_text_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {509, 40, 515, 52},                      /* widget size                    */
    &main_screen_temperature_lable_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_2), /* control block */
    (void *) &main_screen_prompt_2_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_water_level_value_define =
{
    "water_level_value",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_numeric_text_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {566, 41, 587, 62},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_water_level_value), /* control block */
    (void *) &main_screen_water_level_value_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_water_level_label_define =
{
    "water_level_label",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_text_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {537, 64, 607, 76},                      /* widget size                    */
    &main_screen_water_level_value_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_water_level_label), /* control block */
    (void *) &main_screen_water_level_label_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_4_define =
{
    "prompt_4",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) btn_text_draw,   /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {587, 42, 601, 59},                      /* widget size                    */
    &main_screen_water_level_label_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_4), /* control block */
    (void *) &main_screen_prompt_4_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_6_spin_define =
{
    "label_6_spin",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {574, 428, 600, 440},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_6_spin), /* control block */
    (void *) &main_screen_label_6_spin_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_5_rinse_define =
{
    "label_5_rinse",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {532, 428, 563, 440},                    /* widget size                    */
    &main_screen_label_6_spin_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_5_rinse), /* control block */
    (void *) &main_screen_label_5_rinse_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_4_spin_define =
{
    "label_4_spin",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {494, 428, 520, 440},                    /* widget size                    */
    &main_screen_label_5_rinse_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_4_spin), /* control block */
    (void *) &main_screen_label_4_spin_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_3_wash_define =
{
    "label_3_wash",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {451, 428, 483, 440},                    /* widget size                    */
    &main_screen_label_4_spin_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_3_wash), /* control block */
    (void *) &main_screen_label_3_wash_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_2_spin_define =
{
    "label_2_spin",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {414, 428, 440, 440},                    /* widget size                    */
    &main_screen_label_3_wash_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_2_spin), /* control block */
    (void *) &main_screen_label_2_spin_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_1_soak_define =
{
    "label_1_soak",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {374, 428, 402, 440},                    /* widget size                    */
    &main_screen_label_2_spin_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_1_soak), /* control block */
    (void *) &main_screen_label_1_soak_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_wash_cycle_progress_bar_define =
{
    "wash_cycle_progress_bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_PROGRESS_PERCENT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    (VOID (*)(GX_WIDGET *)) wash_cycle_progress_bar_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {372, 416, 597, 420},                    /* widget size                    */
    &main_screen_label_1_soak_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_wash_cycle_progress_bar), /* control block */
    (void *) &main_screen_wash_cycle_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_btn_play_define =
{
    "btn_play",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BTN_PLAY,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {55, 324, 125, 396},                     /* widget size                    */
    &main_screen_wash_cycle_progress_bar_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_btn_play), /* control block */
    (void *) &main_screen_btn_play_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_btn_water_level_define =
{
    "btn_water_level",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_BTN_WATER_LEVEL,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {533, 33, 612, 81},                      /* widget size                    */
    &main_screen_btn_play_define,            /* next widget definition         */
    &main_screen_prompt_4_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_btn_water_level), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET main_screen_btn_temperature_define =
{
    "btn_temperature",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_BTN_TEMPERATURE,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {452, 33, 531, 81},                      /* widget size                    */
    &main_screen_btn_water_level_define,     /* next widget definition         */
    &main_screen_prompt_2_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_btn_temperature), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET main_screen_btn_wash_cycle_define =
{
    "btn_wash_cycle",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_BTN_WASH_CYCLE,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {371, 33, 450, 81},                      /* widget size                    */
    &main_screen_btn_temperature_define,     /* next widget definition         */
    &main_screen_wash_cycle_mode_define,     /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_btn_wash_cycle), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET main_screen_logo_define =
{
    "logo",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {43, 35, 145, 74},                       /* widget size                    */
    &main_screen_btn_wash_cycle_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_logo), /* control block    */
    (void *) &main_screen_logo_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_define =
{
    "main_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MAIN_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) main_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &main_screen_logo_define,                /* child widget                   */
    0,                                       /* control block                  */
    (void *) &main_screen_properties         /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY demo_guix_washing_machine_widget_table[] =
{
    { &mode_select_window_define, (GX_WIDGET *) &mode_select_window },
    { &water_level_window_define, (GX_WIDGET *) &water_level_window },
    { &temperature_window_define, (GX_WIDGET *) &temperature_window },
    { &wash_cycle_window_define, (GX_WIDGET *) &wash_cycle_window },
    { &main_screen_define, (GX_WIDGET *) &main_screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = demo_guix_washing_machine_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &demo_guix_washing_machine_display_table[display];


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
