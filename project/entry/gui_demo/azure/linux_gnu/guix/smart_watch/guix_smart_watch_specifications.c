#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "guix_smart_watch_resources.h"
#include "guix_smart_watch_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
SCREEN_SLIDE_PARENT_CONTROL_BLOCK screen_slide_parent;
HOME_BUTTON_CONTROL_BLOCK home_button;
PAGE_3_CONTAINER_SCREEN_CONTROL_BLOCK page_3_container_screen;
PAGE_2_CONTAINER_SCREEN_CONTROL_BLOCK page_2_container_screen;
PAGE_1_CONTAINER_SCREEN_CONTROL_BLOCK page_1_container_screen;
MAIN_SCREEN_CONTROL_BLOCK main_screen;
WEATHER_SCREEN_CONTROL_BLOCK weather_screen;
MUSIC_SCREEN_CONTROL_BLOCK music_screen;
MESSAGE_SCREEN_CONTROL_BLOCK message_screen;
SANFRANCISCO_WEATHER_SCREEN_CONTROL_BLOCK SanFrancisco_weather_screen;
LOSANGELES_WEATHER_SCREEN_CONTROL_BLOCK LosAngeles_weather_screen;
SANDIEGO_WEATHER_SCREEN_CONTROL_BLOCK SanDiego_weather_screen;
EKG_SCREEN_CONTROL_BLOCK ekg_screen;
YOGA_SCREEN_CONTROL_BLOCK yoga_screen;
STAND_SCREEN_CONTROL_BLOCK stand_screen;
RUN_SCREEN_CONTROL_BLOCK run_screen;
CALORIES_SCREEN_CONTROL_BLOCK calories_screen;
FITNESS_SCREEN_CONTROL_BLOCK fitness_screen;
CLOCK_2_SCREEN_CONTROL_BLOCK clock_2_screen;
CLOCK_1_SCREEN_CONTROL_BLOCK clock_1_screen;
CLOCK_5_SCREEN_CONTROL_BLOCK clock_5_screen;
CLOCK_3_SCREEN_CONTROL_BLOCK clock_3_screen;
CLOCK_4_SCREEN_CONTROL_BLOCK clock_4_screen;
HOME_SCREEN_CONTROL_BLOCK home_screen;
GX_DISPLAY display_1_control_block;
GX_WINDOW_ROOT display_1_root_window;
GX_CANVAS  display_1_canvas_control_block;
ULONG      display_1_canvas_memory[307200];

extern GX_CONST GX_THEME *display_1_theme_table[];
extern GX_CONST GX_STRING *display_1_language_table[];

GX_STUDIO_DISPLAY_INFO guix_smart_watch_display_table[1] =
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


UINT gx_studio_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_BUTTON *button = (GX_BUTTON *) control_block;
    status = gx_button_create(button, info->widget_name, parent, info->style, info->widget_id, &info->size);
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

UINT gx_studio_sprite_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_SPRITE *sprite = (GX_SPRITE *) control_block;
    GX_SPRITE_PROPERTIES *props = (GX_SPRITE_PROPERTIES *) info->properties;
    status = gx_sprite_create(sprite, info->widget_name, parent,
               props->frame_list, props->frame_count,
               info->style, info->widget_id, &info->size);
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

UINT gx_studio_multi_line_text_view_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_MULTI_LINE_TEXT_VIEW *view = (GX_MULTI_LINE_TEXT_VIEW *) control_block;
    GX_ML_TEXT_VIEW_PROPERTIES *props = (GX_ML_TEXT_VIEW_PROPERTIES *) info->properties;
    status = gx_multi_line_text_view_create(view, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_multi_line_text_view_font_set(view, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_multi_line_text_view_text_color_set(view, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_multi_line_text_view_text_color_set(view, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
        gx_multi_line_text_view_whitespace_set(view, props->whitespace);
        gx_multi_line_text_view_line_space_set(view, props->line_space);
    }
    return status;
}

UINT gx_studio_vertical_scrollbar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_SCROLLBAR *scroll = (GX_SCROLLBAR *) control_block;
    GX_SCROLLBAR_APPEARANCE *appearance = (GX_SCROLLBAR_APPEARANCE *) info->properties;
    status = gx_vertical_scrollbar_create(scroll, info->widget_name, parent, appearance, info->style);
    return status;
}

UINT gx_studio_circular_gauge_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_CIRCULAR_GAUGE *gauge = (GX_CIRCULAR_GAUGE *) control_block;
    GX_CIRCULAR_GAUGE_PROPERTIES *props = (GX_CIRCULAR_GAUGE_PROPERTIES *) info->properties;
    GX_CIRCULAR_GAUGE_INFO gauge_info;
    gauge_info.gx_circular_gauge_info_needle_pixelmap = props->needle_pixelmap_id;
    gauge_info.gx_circular_gauge_info_needle_xpos = props->needle_xpos;
    gauge_info.gx_circular_gauge_info_needle_ypos = props->needle_ypos;
    gauge_info.gx_circular_gauge_info_needle_xcor = props->needle_xcor;
    gauge_info.gx_circular_gauge_info_needle_ycor = props->needle_ycor;
    gauge_info.gx_circular_gauge_info_animation_steps = props->animation_steps;
    gauge_info.gx_circular_gauge_info_animation_delay = props->animation_delay;
    gauge->gx_icon_normal_pixelmap = props->normal_pixelmap_id;
    gauge->gx_icon_selected_pixelmap = props->selected_pixelmap_id;
    status = gx_circular_gauge_create(gauge,
                    info->widget_name,
                    parent,
                    &gauge_info,
                    props->normal_pixelmap_id,
                    info->style,
                    info->widget_id,
                    info->size.gx_rectangle_left,
                    info->size.gx_rectangle_top);
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
GX_WINDOW_PROPERTIES screen_slide_parent_properties =
{
    0                                        /* wallpaper pixelmap id          */
};

GX_CONST GX_STUDIO_WIDGET screen_slide_parent_define =
{
    "screen_slide_parent",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SCREEN_SLIDE_PARENT_CONTROL_BLOCK), /* control block size           */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 257, 288},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &screen_slide_parent_properties /* extended properties            */
};
GX_PIXELMAP_BUTTON_PROPERTIES home_button_properties =
{
    GX_PIXELMAP_ID_WATCH_HOME_BUTTON,        /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET home_button_define =
{
    "home_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_HOME_BUTTON,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(HOME_BUTTON_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) root_home_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {444, 214, 455, 267},                    /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &home_button_properties         /* extended properties            */
};
GX_WINDOW_PROPERTIES page_3_container_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};

GX_CONST GX_STUDIO_WIDGET page_3_container_screen_define =
{
    "page_3_container_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_PAGE_3_CONTAINER_SCREEN,              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(PAGE_3_CONTAINER_SCREEN_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &page_3_container_screen_properties /* extended properties        */
};
GX_WINDOW_PROPERTIES page_2_container_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};

GX_CONST GX_STUDIO_WIDGET page_2_container_screen_define =
{
    "page_2_container_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_PAGE_2_CONTAINER_SCREEN,              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(PAGE_2_CONTAINER_SCREEN_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &page_2_container_screen_properties /* extended properties        */
};
GX_WINDOW_PROPERTIES page_1_container_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};

GX_CONST GX_STUDIO_WIDGET page_1_container_screen_define =
{
    "page_1_container_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_PAGE_1_CONTAINER_SCREEN,              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(PAGE_1_CONTAINER_SCREEN_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &page_1_container_screen_properties /* extended properties        */
};
GX_WINDOW_PROPERTIES main_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
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
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) main_screen_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &main_screen_properties         /* extended properties            */
};
GX_WINDOW_PROPERTIES weather_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_RADIAL_PROGRESS_BAR_INFO weather_screen_calorie_burned_progress_properties =
{
    239,                                     /* xcenter                        */
    330,                                     /* ycenter                        */
    28,                                      /* radius                         */
    -300,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    10,                                      /* normal brush width             */
    10,                                      /* selected brush width           */
    GX_COLOR_ID_GREEN_BG,                    /* normal brush color             */
    GX_COLOR_ID_GREEN,                       /* selected brush color           */
};
GX_ICON_PROPERTIES weather_screen_calorie_icon_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_CALORIES_GREEN, /* normal pixelmap id          */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_screen_calorie_burned_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_screen_prompt_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO weather_screen_running_progress_properties =
{
    318,                                     /* xcenter                        */
    330,                                     /* ycenter                        */
    28,                                      /* radius                         */
    -300,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    10,                                      /* normal brush width             */
    10,                                      /* selected brush width           */
    GX_COLOR_ID_YELLOW_BG,                   /* normal brush color             */
    GX_COLOR_ID_YELLOW,                      /* selected brush color           */
};
GX_ICON_PROPERTIES weather_screen_icon_5_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_RUN_YELLOW,  /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_screen_running_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_screen_prompt_9_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO weather_screen_ekg_progress_properties =
{
    400,                                     /* xcenter                        */
    330,                                     /* ycenter                        */
    28,                                      /* radius                         */
    -300,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    10,                                      /* normal brush width             */
    10,                                      /* selected brush width           */
    GX_COLOR_ID_ORANGE_BG,                   /* normal brush color             */
    GX_COLOR_ID_ORANGE,                      /* selected brush color           */
};
GX_ICON_PROPERTIES weather_screen_icon_6_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_HEART_RED,   /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_screen_ekg_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_screen_prompt_11_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_screen_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES weather_screen_weather_icon_properties =
{
    GX_PIXELMAP_ID_LANDING_TOP_BIG_ICON_PARTLY_CLOUDY, /* normal pixelmap id   */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_screen_temperature_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_19,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_BLUE,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_BLUE                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_screen_day_of_week_properties =
{
    GX_STRING_ID_STRING_20,                  /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_BLUE,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_BLUE                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_screen_date_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_BLUE,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_BLUE                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_screen_city_properties =
{
    GX_STRING_ID_SAN_DIEGO,                  /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_BLUE,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_BLUE                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_screen_second_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_BLUE,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_BLUE                   /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    32                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_screen_am_pm_properties =
{
    GX_STRING_ID_AM,                         /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_BLUE,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_BLUE                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES weather_screen_bell_button_properties =
{
    GX_PIXELMAP_ID_LANDING_TOP_ICON_BELL,    /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES weather_screen_singer_properties =
{
    GX_STRING_ID_STRING_31,                  /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_screen_song_properties =
{
    GX_STRING_ID_STRING_30,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO weather_screen_music_play_progress_properties =
{
    384,                                     /* xcenter                        */
    246,                                     /* ycenter                        */
    37,                                      /* radius                         */
    0,                                       /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    20,                                      /* normal brush width             */
    20,                                      /* selected brush width           */
    GX_COLOR_ID_STONE_BLUE,                  /* normal brush color             */
    GX_COLOR_ID_WHITE,                       /* selected brush color           */
};
GX_PIXELMAP_BUTTON_PROPERTIES weather_screen_play_button_properties =
{
    GX_PIXELMAP_ID_LANDING_TOP_PLAYER_ICON_PLAY, /* normal pixelmap id         */
    GX_PIXELMAP_ID_LANDING_TOP_PLAYER_ICON_PAUSE_ACTIVE, /* selected pixelmap id */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_prompt_define =
{
    "prompt",
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
    {248, 333, 258, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_prompt), /* control block */
    (void *) &weather_screen_prompt_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_calorie_burned_progress_val_define =
{
    "calorie_burned_progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {224, 333, 249, 353},                    /* widget size                    */
    &weather_screen_prompt_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_calorie_burned_progress_val), /* control block */
    (void *) &weather_screen_calorie_burned_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_calorie_icon_define =
{
    "calorie_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {233, 308, 248, 330},                    /* widget size                    */
    &weather_screen_calorie_burned_progress_val_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_calorie_icon), /* control block */
    (void *) &weather_screen_calorie_icon_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_calorie_burned_progress_define =
{
    "calorie_burned_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {206, 297, 272, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &weather_screen_calorie_icon_define,     /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_calorie_burned_progress), /* control block */
    (void *) &weather_screen_calorie_burned_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_prompt_9_define =
{
    "prompt_9",
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
    {327, 333, 337, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_prompt_9), /* control block */
    (void *) &weather_screen_prompt_9_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_running_progress_val_define =
{
    "running_progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {303, 333, 328, 353},                    /* widget size                    */
    &weather_screen_prompt_9_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_running_progress_val), /* control block */
    (void *) &weather_screen_running_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_icon_5_define =
{
    "icon_5",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {305, 311, 328, 331},                    /* widget size                    */
    &weather_screen_running_progress_val_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_icon_5), /* control block */
    (void *) &weather_screen_icon_5_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_running_progress_define =
{
    "running_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {285, 297, 351, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &weather_screen_icon_5_define,           /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_running_progress), /* control block */
    (void *) &weather_screen_running_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_prompt_11_define =
{
    "prompt_11",
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
    {410, 333, 420, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_prompt_11), /* control block */
    (void *) &weather_screen_prompt_11_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_ekg_progress_val_define =
{
    "ekg_progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {386, 333, 411, 353},                    /* widget size                    */
    &weather_screen_prompt_11_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_ekg_progress_val), /* control block */
    (void *) &weather_screen_ekg_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_icon_6_define =
{
    "icon_6",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_ORANGE,                      /* normal color id                */
    GX_COLOR_ID_ORANGE,                      /* selected color id              */
    GX_COLOR_ID_ORANGE,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {389, 313, 409, 330},                    /* widget size                    */
    &weather_screen_ekg_progress_val_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_icon_6), /* control block */
    (void *) &weather_screen_icon_6_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_ekg_progress_define =
{
    "ekg_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {367, 297, 433, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &weather_screen_icon_6_define,           /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_ekg_progress), /* control block */
    (void *) &weather_screen_ekg_progress_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_bell_button_define =
{
    "bell_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BELL_BUTTON,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {403, 110, 430, 137},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_bell_button), /* control block */
    (void *) &weather_screen_bell_button_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_am_pm_define =
{
    "am_pm",
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
    {374, 119, 396, 136},                    /* widget size                    */
    &weather_screen_bell_button_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_am_pm), /* control block */
    (void *) &weather_screen_am_pm_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_minute_define =
{
    "minute",
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
    {341, 117, 366, 137},                    /* widget size                    */
    &weather_screen_am_pm_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_minute), /* control block */
    (void *) &weather_screen_minute_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_hour_define =
{
    "hour",
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
    {311, 117, 336, 137},                    /* widget size                    */
    &weather_screen_minute_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_hour), /* control block */
    (void *) &weather_screen_hour_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_second_define =
{
    "second",
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
    {336, 115, 342, 135},                    /* widget size                    */
    &weather_screen_hour_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_second), /* control block */
    (void *) &weather_screen_second_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_city_define =
{
    "city",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {337, 169, 427, 185},                    /* widget size                    */
    &weather_screen_second_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_city), /* control block */
    (void *) &weather_screen_city_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_date_define =
{
    "date",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {321, 143, 427, 160},                    /* widget size                    */
    &weather_screen_city_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_date), /* control block */
    (void *) &weather_screen_date_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_day_of_week_define =
{
    "day_of_week",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {282, 143, 321, 159},                    /* widget size                    */
    &weather_screen_date_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_day_of_week), /* control block */
    (void *) &weather_screen_day_of_week_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_prompt_1_define =
{
    "prompt_1",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_LIGHT_BLUE,                  /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {277, 177, 288, 194},                    /* widget size                    */
    &weather_screen_day_of_week_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_prompt_1), /* control block */
    (void *) &weather_screen_prompt_1_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_temperature_val_define =
{
    "temperature_val",
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
    {211, 179, 274, 216},                    /* widget size                    */
    &weather_screen_prompt_1_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_temperature_val), /* control block */
    (void *) &weather_screen_temperature_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_weather_icon_define =
{
    "weather_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {202, 112, 282, 168},                    /* widget size                    */
    &weather_screen_temperature_val_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_weather_icon), /* control block */
    (void *) &weather_screen_weather_icon_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_play_button_define =
{
    "play_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PLAY_BUTTON,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {371, 233, 397, 260},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_play_button), /* control block */
    (void *) &weather_screen_play_button_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_music_play_progress_define =
{
    "music_play_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {337, 199, 431, 293},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &weather_screen_play_button_define,      /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_music_play_progress), /* control block */
    (void *) &weather_screen_music_play_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_song_define =
{
    "song",
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
    {192, 275, 336, 287},                    /* widget size                    */
    &weather_screen_music_play_progress_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_song), /* control block */
    (void *) &weather_screen_song_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_singer_define =
{
    "singer",
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
    {209, 252, 315, 268},                    /* widget size                    */
    &weather_screen_song_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_singer), /* control block */
    (void *) &weather_screen_singer_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_window_define =
{
    "window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_DARK_BLUE,                   /* normal color id                */
    GX_COLOR_ID_DARK_BLUE,                   /* selected color id              */
    GX_COLOR_ID_DARK_BLUE,                   /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {185, 96, 442, 240},                     /* widget size                    */
    &weather_screen_singer_define,           /* next widget definition         */
    &weather_screen_weather_icon_define,     /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_window), /* control block */
    (void *) &weather_screen_window_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_ekg_button_define =
{
    "ekg_button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_EKG_BUTTON,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {367, 297, 433, 363},                    /* widget size                    */
    &weather_screen_window_define,           /* next widget definition         */
    &weather_screen_ekg_progress_define,     /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_ekg_button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_running_button_define =
{
    "running_button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_RUNNING_BUTTON,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {285, 297, 351, 363},                    /* widget size                    */
    &weather_screen_ekg_button_define,       /* next widget definition         */
    &weather_screen_running_progress_define, /* child widget definition        */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_running_button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_calorie_button_define =
{
    "calorie_button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_CALORIE_BUTTON,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {206, 297, 272, 363},                    /* widget size                    */
    &weather_screen_running_button_define,   /* next widget definition         */
    &weather_screen_calorie_burned_progress_define, /* child widget definition */
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_calorie_button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_define =
{
    "weather_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) weather_screen_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &weather_screen_calorie_button_define,   /* child widget                   */
    0,                                       /* control block                  */
    (void *) &weather_screen_properties      /* extended properties            */
};
GX_WINDOW_PROPERTIES clock_screen_template_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_CIRCULAR_GAUGE_PROPERTIES clock_screen_template_gauge_properties =
{
    30,                                      /* start angle                    */
    0,                                       /* animation steps                */
    1,                                       /* animation delay                */
    115,                                     /* needle xpos                    */
    130,                                     /* needle yos                     */
    1,                                       /* needle xcor                    */
    87,                                      /* needle ycor                    */
    GX_PIXELMAP_ID_W_DIAL_BLACK,             /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    GX_PIXELMAP_ID_WATCH_HAND_SECOND         /* needle pixelmap id             */
};

GX_CONST GX_STUDIO_WIDGET clock_screen_template_gauge_define =
{
    "gauge",
    GX_TYPE_CIRCULAR_GAUGE,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CIRCULAR_GAUGE),               /* control block size             */
    GX_COLOR_ID_WATCH_HAND_SECOND,           /* normal color id                */
    GX_COLOR_ID_WATCH_HAND_SECOND,           /* selected color id              */
    GX_COLOR_ID_WATCH_HAND_SECOND,           /* disabled color id              */
    gx_studio_circular_gauge_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {199, 110, 428, 370},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK, clock_screen_template_gauge), /* control block */
    (void *) &clock_screen_template_gauge_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET clock_screen_template_define =
{
    "clock_screen_template",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) clock_screen_template_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &clock_screen_template_gauge_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &clock_screen_template_properties /* extended properties          */
};
GX_WINDOW_PROPERTIES message_template_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES message_template_message_bar_properties =
{
    GX_PIXELMAP_ID_MESSAGE_BAR               /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES message_template_second_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES message_template_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    4                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES message_template_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    37                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES message_template_am_pm_properties =
{
    GX_STRING_ID_PM,                         /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES message_template_day_of_week_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES message_template_comma_properties =
{
    GX_STRING_ID_STRING_28,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_ML_TEXT_VIEW_PROPERTIES message_template_text_view_properties =
{
    GX_STRING_ID_STRING_29,                  /* string id                      */
    GX_FONT_ID_TEXT_INPUT,                   /* font id                        */
    GX_COLOR_ID_TEXT_INPUT_TEXT,             /* normal text color              */
    GX_COLOR_ID_TEXT_INPUT_TEXT,             /* selected text color            */
    GX_COLOR_ID_TEXT_INPUT_TEXT,             /* disabled text color            */
    8,                                       /* whitespace                     */
    0                                        /* line_space                     */
};

GX_CONST GX_STUDIO_WIDGET message_template_comma_define =
{
    "comma",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {151, 6, 154, 23},                       /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_comma), /* control block */
    (void *) &message_template_comma_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET message_template_day_of_week_define =
{
    "day_of_week",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {73, 7, 150, 24},                        /* widget size                    */
    &message_template_comma_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_day_of_week), /* control block */
    (void *) &message_template_day_of_week_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET message_template_am_pm_define =
{
    "am_pm",
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
    {196, 7, 218, 24},                       /* widget size                    */
    &message_template_day_of_week_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_am_pm), /* control block */
    (void *) &message_template_am_pm_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET message_template_minute_define =
{
    "minute",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {173, 7, 194, 23},                       /* widget size                    */
    &message_template_am_pm_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_minute), /* control block */
    (void *) &message_template_minute_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET message_template_hour_define =
{
    "hour",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {147, 7, 168, 23},                       /* widget size                    */
    &message_template_minute_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_hour), /* control block */
    (void *) &message_template_hour_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET message_template_second_define =
{
    "second",
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
    {167, 6, 172, 22},                       /* widget size                    */
    &message_template_hour_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_second), /* control block */
    (void *) &message_template_second_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET message_template_text_view_define =
{
    "text_view",
    GX_TYPE_MULTI_LINE_TEXT_VIEW,            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TEXT_LEFT,   /* style flags                  */
    0,                                       /* status flags                   */
    sizeof(GX_MULTI_LINE_TEXT_VIEW),         /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_multi_line_text_view_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 27, 227, 58},                        /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_text_view), /* control block */
    (void *) &message_template_text_view_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET message_template_message_bar_define =
{
    "message_bar",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 227, 26},                         /* widget size                    */
    &message_template_text_view_define,      /* next widget definition         */
    &message_template_second_define,         /* child widget definition        */
    offsetof(MESSAGE_TEMPLATE_CONTROL_BLOCK, message_template_message_bar), /* control block */
    (void *) &message_template_message_bar_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET message_template_define =
{
    "message_template",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(MESSAGE_TEMPLATE_CONTROL_BLOCK),  /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 227, 58},                         /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &message_template_message_bar_define,    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &message_template_properties    /* extended properties            */
};
GX_WINDOW_PROPERTIES weather_info_template_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_info_template_day_of_week_properties =
{
    GX_STRING_ID_WEDNESDAY,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_info_template_prompt_6_properties =
{
    GX_STRING_ID_STRING_24,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES weather_info_template_weather_icon_properties =
{
    GX_PIXELMAP_ID_WEATHER_ICON_PARTLY_CLOUDY, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_info_template_high_temperature_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    79                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_info_template_low_temperature_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    73                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_info_template_prompt_1_properties =
{
    GX_STRING_ID_STRING_24,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_prompt_1_define =
{
    "prompt_1",
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
    {421, 165, 431, 181},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK, weather_info_template_prompt_1), /* control block */
    (void *) &weather_info_template_prompt_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_low_temperature_define =
{
    "low_temperature",
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
    {404, 170, 421, 187},                    /* widget size                    */
    &weather_info_template_prompt_1_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK, weather_info_template_low_temperature), /* control block */
    (void *) &weather_info_template_low_temperature_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_high_temperature_define =
{
    "high_temperature",
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
    {368, 170, 385, 187},                    /* widget size                    */
    &weather_info_template_low_temperature_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK, weather_info_template_high_temperature), /* control block */
    (void *) &weather_info_template_high_temperature_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_weather_icon_define =
{
    "weather_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {299, 156, 353, 193},                    /* widget size                    */
    &weather_info_template_high_temperature_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK, weather_info_template_weather_icon), /* control block */
    (void *) &weather_info_template_weather_icon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_prompt_6_define =
{
    "prompt_6",
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
    {384, 165, 394, 181},                    /* widget size                    */
    &weather_info_template_weather_icon_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK, weather_info_template_prompt_6), /* control block */
    (void *) &weather_info_template_prompt_6_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_day_of_week_define =
{
    "day_of_week",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {206, 171, 292, 188},                    /* widget size                    */
    &weather_info_template_prompt_6_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK, weather_info_template_day_of_week), /* control block */
    (void *) &weather_info_template_day_of_week_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_info_template_define =
{
    "weather_info_template",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {202, 154, 432, 200},                    /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &weather_info_template_day_of_week_define, /* child widget                 */
    0,                                       /* control block                  */
    (void *) &weather_info_template_properties /* extended properties          */
};
GX_WINDOW_PROPERTIES screen_template_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES screen_template_title_bar_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES screen_template_title_properties =
{
    GX_STRING_ID_STRING_12,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES screen_template_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES screen_template_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    32                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES screen_template_second_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES screen_template_icon_properties =
{
    GX_PIXELMAP_ID_ICON_LOCATION,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET screen_template_icon_define =
{
    "icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {10, 20, 23, 33},                        /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_TEMPLATE_CONTROL_BLOCK, screen_template_icon), /* control block */
    (void *) &screen_template_icon_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET screen_template_second_define =
{
    "second",
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
    {205, 18, 211, 38},                      /* widget size                    */
    &screen_template_icon_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_TEMPLATE_CONTROL_BLOCK, screen_template_second), /* control block */
    (void *) &screen_template_second_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET screen_template_minute_define =
{
    "minute",
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
    {210, 20, 235, 40},                      /* widget size                    */
    &screen_template_second_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_TEMPLATE_CONTROL_BLOCK, screen_template_minute), /* control block */
    (void *) &screen_template_minute_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET screen_template_hour_define =
{
    "hour",
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
    {180, 20, 205, 40},                      /* widget size                    */
    &screen_template_minute_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_TEMPLATE_CONTROL_BLOCK, screen_template_hour), /* control block */
    (void *) &screen_template_hour_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET screen_template_title_define =
{
    "title",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {29, 20, 133, 37},                       /* widget size                    */
    &screen_template_hour_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_TEMPLATE_CONTROL_BLOCK, screen_template_title), /* control block */
    (void *) &screen_template_title_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET screen_template_title_bar_define =
{
    "title_bar",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_DARK_BLUE,                   /* normal color id                */
    GX_COLOR_ID_DARK_BLUE,                   /* selected color id              */
    GX_COLOR_ID_DARK_BLUE,                   /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 257, 54},                         /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &screen_template_title_define,           /* child widget definition        */
    offsetof(SCREEN_TEMPLATE_CONTROL_BLOCK, screen_template_title_bar), /* control block */
    (void *) &screen_template_title_bar_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET screen_template_define =
{
    "screen_template",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SCREEN_TEMPLATE,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SCREEN_TEMPLATE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) screen_template_event_process, /* event function override */
    {0, 0, 257, 288},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &screen_template_title_bar_define,       /* child widget                   */
    0,                                       /* control block                  */
    (void *) &screen_template_properties     /* extended properties            */
};
GX_TEMPLATE_PROPERTIES ekg_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_RADIAL_PROGRESS_BAR_INFO ekg_screen_progress_properties =
{
    322,                                     /* xcenter                        */
    251,                                     /* ycenter                        */
    98,                                      /* radius                         */
    -288,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    30,                                      /* normal brush width             */
    30,                                      /* selected brush width           */
    GX_COLOR_ID_ORANGE_BG,                   /* normal brush color             */
    GX_COLOR_ID_ORANGE,                      /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES ekg_screen_systole_blood_pressure_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_MEDIUM,                /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    120                                      /* numeric prompt value           */
};
GX_ICON_PROPERTIES ekg_screen_heart_icon_properties =
{
    GX_PIXELMAP_ID_ICON_HEART_FRONT,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES ekg_screen_diastole_blood_pressure_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_MEDIUM,                /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    78                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES ekg_screen_prompt_21_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES ekg_screen_prompt_21_1_properties =
{
    GX_STRING_ID_STRING_15,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_ICON_PROPERTIES ekg_screen_icon_21_properties =
{
    GX_PIXELMAP_ID_BALL_RANGE_EKG,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES ekg_screen_heart_rate_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES ekg_screen_prompt_20_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_prompt_21_1_define =
{
    "prompt_21_1",
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
    {295, 297, 350, 314},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_prompt_21_1), /* control block */
    (void *) &ekg_screen_prompt_21_1_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_prompt_21_define =
{
    "prompt_21",
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
    {323, 247, 332, 277},                    /* widget size                    */
    &ekg_screen_prompt_21_1_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_prompt_21), /* control block */
    (void *) &ekg_screen_prompt_21_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_diastole_blood_pressure_define =
{
    "diastole_blood_pressure",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {335, 248, 384, 277},                    /* widget size                    */
    &ekg_screen_prompt_21_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_diastole_blood_pressure), /* control block */
    (void *) &ekg_screen_diastole_blood_pressure_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_heart_icon_define =
{
    "heart_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_ORANGE,                      /* normal color id                */
    GX_COLOR_ID_ORANGE,                      /* selected color id              */
    GX_COLOR_ID_ORANGE,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {290, 186, 344, 229},                    /* widget size                    */
    &ekg_screen_diastole_blood_pressure_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_heart_icon), /* control block */
    (void *) &ekg_screen_heart_icon_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_systole_blood_pressure_define =
{
    "systole_blood_pressure",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {248, 248, 322, 277},                    /* widget size                    */
    &ekg_screen_heart_icon_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_systole_blood_pressure), /* control block */
    (void *) &ekg_screen_systole_blood_pressure_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_prompt_20_define =
{
    "prompt_20",
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
    {223, 354, 257, 371},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_prompt_20), /* control block */
    (void *) &ekg_screen_prompt_20_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_heart_rate_define =
{
    "heart_rate",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {220, 325, 257, 355},                    /* widget size                    */
    &ekg_screen_prompt_20_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_heart_rate), /* control block */
    (void *) &ekg_screen_heart_rate_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_icon_21_define =
{
    "icon_21",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 309, 274, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &ekg_screen_heart_rate_define,           /* child widget definition        */
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_icon_21), /* control block   */
    (void *) &ekg_screen_icon_21_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_progress_define =
{
    "progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 126, 433, 352},                    /* widget size                    */
    &ekg_screen_icon_21_define,              /* next widget definition         */
    &ekg_screen_systole_blood_pressure_define, /* child widget definition      */
    offsetof(EKG_SCREEN_CONTROL_BLOCK, ekg_screen_progress), /* control block  */
    (void *) &ekg_screen_progress_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET ekg_screen_define =
{
    "ekg_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(EKG_SCREEN_CONTROL_BLOCK),        /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) ekg_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &ekg_screen_progress_define,             /* child widget                   */
    0,                                       /* control block                  */
    (void *) &ekg_screen_properties          /* extended properties            */
};
GX_TEMPLATE_PROPERTIES yoga_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_RADIAL_PROGRESS_BAR_INFO yoga_screen_progress_properties =
{
    322,                                     /* xcenter                        */
    251,                                     /* ycenter                        */
    98,                                      /* radius                         */
    -288,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    30,                                      /* normal brush width             */
    30,                                      /* selected brush width           */
    GX_COLOR_ID_PURPLE_BG,                   /* normal brush color             */
    GX_COLOR_ID_PURPLE,                      /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES yoga_screen_calories_burned_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    480                                      /* numeric prompt value           */
};
GX_ICON_PROPERTIES yoga_screen_divide_line_properties =
{
    GX_PIXELMAP_ID_LINE_CALORIES_GREEN,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES yoga_screen_yoga_icon_properties =
{
    GX_PIXELMAP_ID_ICON_YOGA_PURPLE,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES yoga_screen_prompt_22_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES yoga_screen_yoga_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    time_format,                             /* format function                */
    28                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES yoga_screen_yoga_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    52                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES yoga_screen_calorie_label_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_ICON_PROPERTIES yoga_screen_calorie_icon_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_CALORIES_GREEN, /* normal pixelmap id          */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES yoga_screen_icon_18_properties =
{
    GX_PIXELMAP_ID_BALL_RANGE_YOGA,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES yoga_screen_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES yoga_screen_prompt_18_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_calorie_icon_define =
{
    "calorie_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_PURPLE,                      /* normal color id                */
    GX_COLOR_ID_PURPLE,                      /* selected color id              */
    GX_COLOR_ID_PURPLE,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {373, 242, 388, 264},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_calorie_icon), /* control block */
    (void *) &yoga_screen_calorie_icon_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_calorie_label_define =
{
    "calorie_label",
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
    {368, 266, 396, 283},                    /* widget size                    */
    &yoga_screen_calorie_icon_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_calorie_label), /* control block */
    (void *) &yoga_screen_calorie_label_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_yoga_hour_define =
{
    "yoga_hour",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {284, 294, 321, 324},                    /* widget size                    */
    &yoga_screen_calorie_label_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_yoga_hour), /* control block */
    (void *) &yoga_screen_yoga_hour_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_yoga_minute_define =
{
    "yoga_minute",
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
    {323, 294, 360, 324},                    /* widget size                    */
    &yoga_screen_yoga_hour_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_yoga_minute), /* control block */
    (void *) &yoga_screen_yoga_minute_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_prompt_22_define =
{
    "prompt_22",
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
    {317, 292, 326, 322},                    /* widget size                    */
    &yoga_screen_yoga_minute_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_prompt_22), /* control block */
    (void *) &yoga_screen_prompt_22_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_yoga_icon_define =
{
    "yoga_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_PURPLE,                      /* normal color id                */
    GX_COLOR_ID_PURPLE,                      /* selected color id              */
    GX_COLOR_ID_PURPLE,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {293, 174, 342, 233},                    /* widget size                    */
    &yoga_screen_prompt_22_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_yoga_icon), /* control block */
    (void *) &yoga_screen_yoga_icon_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_divide_line_define =
{
    "divide_line",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_PURPLE,                      /* normal color id                */
    GX_COLOR_ID_PURPLE,                      /* selected color id              */
    GX_COLOR_ID_PURPLE,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {293, 286, 345, 287},                    /* widget size                    */
    &yoga_screen_yoga_icon_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_divide_line), /* control block */
    (void *) &yoga_screen_divide_line_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_calories_burned_define =
{
    "calories_burned",
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
    {272, 233, 367, 284},                    /* widget size                    */
    &yoga_screen_divide_line_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_calories_burned), /* control block */
    (void *) &yoga_screen_calories_burned_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_prompt_18_define =
{
    "prompt_18",
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
    {255, 330, 269, 347},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_prompt_18), /* control block */
    (void *) &yoga_screen_prompt_18_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_progress_val_define =
{
    "progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {219, 332, 256, 362},                    /* widget size                    */
    &yoga_screen_prompt_18_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_progress_val), /* control block */
    (void *) &yoga_screen_progress_val_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_icon_18_define =
{
    "icon_18",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 309, 274, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &yoga_screen_progress_val_define,        /* child widget definition        */
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_icon_18), /* control block */
    (void *) &yoga_screen_icon_18_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_progress_define =
{
    "progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 120, 433, 346},                    /* widget size                    */
    &yoga_screen_icon_18_define,             /* next widget definition         */
    &yoga_screen_calories_burned_define,     /* child widget definition        */
    offsetof(YOGA_SCREEN_CONTROL_BLOCK, yoga_screen_progress), /* control block */
    (void *) &yoga_screen_progress_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET yoga_screen_define =
{
    "yoga_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(YOGA_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) yoga_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &yoga_screen_progress_define,            /* child widget                   */
    0,                                       /* control block                  */
    (void *) &yoga_screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES stand_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_RADIAL_PROGRESS_BAR_INFO stand_screen_progress_properties =
{
    322,                                     /* xcenter                        */
    251,                                     /* ycenter                        */
    98,                                      /* radius                         */
    -288,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    30,                                      /* normal brush width             */
    30,                                      /* selected brush width           */
    GX_COLOR_ID_DARK_BLUE,                   /* normal brush color             */
    GX_COLOR_ID_BLUE,                        /* selected brush color           */
};
GX_ICON_PROPERTIES stand_screen_divide_line_properties =
{
    GX_PIXELMAP_ID_LINE_CALORIES_GREEN,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES stand_screen_stand_icon_properties =
{
    GX_PIXELMAP_ID_ICON_STAND_BLUE,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES stand_screen_min_label_properties =
{
    GX_STRING_ID_STRING_17,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_ICON_PROPERTIES stand_screen_clock_icon_properties =
{
    GX_PIXELMAP_ID_ICON_CLOCK,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES stand_screen_stand_minute_target_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    time_format,                             /* format function                */
    28                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES stand_screen_prompt_22_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES stand_screen_stand_hour_target_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    52                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES stand_screen_icon_8_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES stand_screen_stand_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    4                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES stand_screen_stand_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    65                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES stand_screen_icon_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES stand_screen_icon_15_properties =
{
    GX_PIXELMAP_ID_BALL_RANGE_STAND,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES stand_screen_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES stand_screen_prompt_16_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_icon_define =
{
    "icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {297, 251, 301, 255},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_icon), /* control block  */
    (void *) &stand_screen_icon_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_stand_minute_define =
{
    "stand_minute",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {304, 240, 367, 281},                    /* widget size                    */
    &stand_screen_icon_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_stand_minute), /* control block */
    (void *) &stand_screen_stand_minute_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_stand_hour_define =
{
    "stand_hour",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {252, 241, 294, 282},                    /* widget size                    */
    &stand_screen_stand_minute_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_stand_hour), /* control block */
    (void *) &stand_screen_stand_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_icon_8_define =
{
    "icon_8",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {297, 268, 301, 272},                    /* widget size                    */
    &stand_screen_stand_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_icon_8), /* control block */
    (void *) &stand_screen_icon_8_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_stand_hour_target_define =
{
    "stand_hour_target",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {282, 290, 319, 320},                    /* widget size                    */
    &stand_screen_icon_8_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_stand_hour_target), /* control block */
    (void *) &stand_screen_stand_hour_target_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_prompt_22_define =
{
    "prompt_22",
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
    {316, 290, 325, 320},                    /* widget size                    */
    &stand_screen_stand_hour_target_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_prompt_22), /* control block */
    (void *) &stand_screen_prompt_22_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_stand_minute_target_define =
{
    "stand_minute_target",
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
    {322, 290, 359, 320},                    /* widget size                    */
    &stand_screen_prompt_22_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_stand_minute_target), /* control block */
    (void *) &stand_screen_stand_minute_target_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_clock_icon_define =
{
    "clock_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {372, 240, 387, 255},                    /* widget size                    */
    &stand_screen_stand_minute_target_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_clock_icon), /* control block */
    (void *) &stand_screen_clock_icon_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_min_label_define =
{
    "min_label",
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
    {367, 264, 395, 281},                    /* widget size                    */
    &stand_screen_clock_icon_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_min_label), /* control block */
    (void *) &stand_screen_min_label_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_stand_icon_define =
{
    "stand_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {304, 175, 329, 235},                    /* widget size                    */
    &stand_screen_min_label_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_stand_icon), /* control block */
    (void *) &stand_screen_stand_icon_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_divide_line_define =
{
    "divide_line",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {293, 284, 345, 285},                    /* widget size                    */
    &stand_screen_stand_icon_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_divide_line), /* control block */
    (void *) &stand_screen_divide_line_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_prompt_16_define =
{
    "prompt_16",
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
    {255, 330, 269, 347},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_prompt_16), /* control block */
    (void *) &stand_screen_prompt_16_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_progress_val_define =
{
    "progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {219, 332, 256, 362},                    /* widget size                    */
    &stand_screen_prompt_16_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_progress_val), /* control block */
    (void *) &stand_screen_progress_val_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_icon_15_define =
{
    "icon_15",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 309, 274, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &stand_screen_progress_val_define,       /* child widget definition        */
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_icon_15), /* control block */
    (void *) &stand_screen_icon_15_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_progress_define =
{
    "progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 120, 433, 346},                    /* widget size                    */
    &stand_screen_icon_15_define,            /* next widget definition         */
    &stand_screen_divide_line_define,        /* child widget definition        */
    offsetof(STAND_SCREEN_CONTROL_BLOCK, stand_screen_progress), /* control block */
    (void *) &stand_screen_progress_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET stand_screen_define =
{
    "stand_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_STAND_SCREEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(STAND_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) stand_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &stand_screen_progress_define,           /* child widget                   */
    0,                                       /* control block                  */
    (void *) &stand_screen_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES run_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_RADIAL_PROGRESS_BAR_INFO run_screen_progress_properties =
{
    322,                                     /* xcenter                        */
    251,                                     /* ycenter                        */
    98,                                      /* radius                         */
    -288,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    30,                                      /* normal brush width             */
    30,                                      /* selected brush width           */
    GX_COLOR_ID_YELLOW_BG,                   /* normal brush color             */
    GX_COLOR_ID_YELLOW,                      /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES run_screen_running_distance_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    4                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES run_screen_running_minutes_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    52                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES run_screen_divide_line_properties =
{
    GX_PIXELMAP_ID_LINE_CALORIES_GREEN,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES run_screen_running_icon_properties =
{
    GX_PIXELMAP_ID_ICON_RUN_YELLOW,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES run_screen_running_seconds_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    28                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES run_screen_second_label_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_ICON_PROPERTIES run_screen_distance_icon_properties =
{
    GX_PIXELMAP_ID_ICON_DISTANCE,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES run_screen_mile_label_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES run_screen_running_distance_fraction_part_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    65                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES run_screen_icon_8_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES run_screen_icon_12_properties =
{
    GX_PIXELMAP_ID_BALL_RANGE_RUN,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES run_screen_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES run_screen_prompt_14_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET run_screen_icon_8_define =
{
    "icon_8",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {299, 270, 303, 274},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_icon_8), /* control block    */
    (void *) &run_screen_icon_8_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET run_screen_running_distance_fraction_part_define =
{
    "running_distance_fraction_part",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {304, 235, 367, 276},                    /* widget size                    */
    &run_screen_icon_8_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_running_distance_fraction_part), /* control block */
    (void *) &run_screen_running_distance_fraction_part_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET run_screen_mile_label_define =
{
    "mile_label",
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
    {367, 262, 391, 282},                    /* widget size                    */
    &run_screen_running_distance_fraction_part_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_mile_label), /* control block */
    (void *) &run_screen_mile_label_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET run_screen_distance_icon_define =
{
    "distance_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {367, 234, 382, 255},                    /* widget size                    */
    &run_screen_mile_label_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_distance_icon), /* control block */
    (void *) &run_screen_distance_icon_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET run_screen_second_label_define =
{
    "second_label",
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
    {317, 288, 326, 318},                    /* widget size                    */
    &run_screen_distance_icon_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_second_label), /* control block */
    (void *) &run_screen_second_label_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET run_screen_running_seconds_define =
{
    "running_seconds",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {323, 290, 360, 320},                    /* widget size                    */
    &run_screen_second_label_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_running_seconds), /* control block */
    (void *) &run_screen_running_seconds_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET run_screen_running_icon_define =
{
    "running_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {291, 177, 347, 230},                    /* widget size                    */
    &run_screen_running_seconds_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_running_icon), /* control block */
    (void *) &run_screen_running_icon_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET run_screen_divide_line_define =
{
    "divide_line",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {293, 281, 345, 282},                    /* widget size                    */
    &run_screen_running_icon_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_divide_line), /* control block */
    (void *) &run_screen_divide_line_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET run_screen_running_minutes_define =
{
    "running_minutes",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {284, 290, 321, 320},                    /* widget size                    */
    &run_screen_divide_line_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_running_minutes), /* control block */
    (void *) &run_screen_running_minutes_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET run_screen_running_distance_define =
{
    "running_distance",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {257, 236, 299, 277},                    /* widget size                    */
    &run_screen_running_minutes_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_running_distance), /* control block */
    (void *) &run_screen_running_distance_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET run_screen_prompt_14_define =
{
    "prompt_14",
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
    {255, 330, 269, 347},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_prompt_14), /* control block */
    (void *) &run_screen_prompt_14_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET run_screen_progress_val_define =
{
    "progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {219, 332, 256, 362},                    /* widget size                    */
    &run_screen_prompt_14_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_progress_val), /* control block */
    (void *) &run_screen_progress_val_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET run_screen_icon_12_define =
{
    "icon_12",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 309, 274, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &run_screen_progress_val_define,         /* child widget definition        */
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_icon_12), /* control block   */
    (void *) &run_screen_icon_12_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET run_screen_progress_define =
{
    "progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 120, 433, 346},                    /* widget size                    */
    &run_screen_icon_12_define,              /* next widget definition         */
    &run_screen_running_distance_define,     /* child widget definition        */
    offsetof(RUN_SCREEN_CONTROL_BLOCK, run_screen_progress), /* control block  */
    (void *) &run_screen_progress_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET run_screen_define =
{
    "run_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_RUN_SCREEN,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(RUN_SCREEN_CONTROL_BLOCK),        /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) run_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &run_screen_progress_define,             /* child widget                   */
    0,                                       /* control block                  */
    (void *) &run_screen_properties          /* extended properties            */
};
GX_TEMPLATE_PROPERTIES calories_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_RADIAL_PROGRESS_BAR_INFO calories_screen_progress_properties =
{
    322,                                     /* xcenter                        */
    251,                                     /* ycenter                        */
    98,                                      /* radius                         */
    -288,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    30,                                      /* normal brush width             */
    30,                                      /* selected brush width           */
    GX_COLOR_ID_GREEN_BG,                    /* normal brush color             */
    GX_COLOR_ID_GREEN,                       /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES calories_screen_calories_burned_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    480                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES calories_screen_calories_burned_target_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    600                                      /* numeric prompt value           */
};
GX_ICON_PROPERTIES calories_screen_divide_line_properties =
{
    GX_PIXELMAP_ID_LINE_CALORIES_GREEN,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES calories_screen_calories_icon_properties =
{
    GX_PIXELMAP_ID_ICON_CALORIES_GREEN,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES calories_screen_icon_7_properties =
{
    GX_PIXELMAP_ID_BALL_RANGE_CALORIES,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES calories_screen_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES calories_screen_percent_label_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_calories_icon_define =
{
    "calories_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {303, 186, 339, 237},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_calories_icon), /* control block */
    (void *) &calories_screen_calories_icon_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_divide_line_define =
{
    "divide_line",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {295, 291, 347, 292},                    /* widget size                    */
    &calories_screen_calories_icon_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_divide_line), /* control block */
    (void *) &calories_screen_divide_line_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_calories_burned_target_define =
{
    "calories_burned_target",
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
    {293, 296, 349, 326},                    /* widget size                    */
    &calories_screen_divide_line_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_calories_burned_target), /* control block */
    (void *) &calories_screen_calories_burned_target_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_calories_burned_define =
{
    "calories_burned",
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
    {274, 241, 369, 292},                    /* widget size                    */
    &calories_screen_calories_burned_target_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_calories_burned), /* control block */
    (void *) &calories_screen_calories_burned_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_percent_label_define =
{
    "percent_label",
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
    {255, 330, 269, 347},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_percent_label), /* control block */
    (void *) &calories_screen_percent_label_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_progress_val_define =
{
    "progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {218, 332, 255, 362},                    /* widget size                    */
    &calories_screen_percent_label_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_progress_val), /* control block */
    (void *) &calories_screen_progress_val_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_icon_7_define =
{
    "icon_7",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 309, 274, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &calories_screen_progress_val_define,    /* child widget definition        */
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_icon_7), /* control block */
    (void *) &calories_screen_icon_7_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_progress_define =
{
    "progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {209, 138, 435, 364},                    /* widget size                    */
    &calories_screen_icon_7_define,          /* next widget definition         */
    &calories_screen_calories_burned_define, /* child widget definition        */
    offsetof(CALORIES_SCREEN_CONTROL_BLOCK, calories_screen_progress), /* control block */
    (void *) &calories_screen_progress_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET calories_screen_define =
{
    "calories_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_CALORIES_SCREEN,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CALORIES_SCREEN_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) calories_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &calories_screen_progress_define,        /* child widget                   */
    0,                                       /* control block                  */
    (void *) &calories_screen_properties     /* extended properties            */
};
GX_TEMPLATE_PROPERTIES fitness_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {185, 96, 442, 384}                      /* widget size                    */
};
GX_ICON_PROPERTIES fitness_screen_icon_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_STEPS,       /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES fitness_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_DISTANCE,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES fitness_screen_icon_2_properties =
{
    GX_PIXELMAP_ID_EYE_NEEDLE,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES fitness_screen_icon_3_properties =
{
    GX_PIXELMAP_ID_EYE_NEEDLE,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES fitness_screen_running_steps_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    11278                                    /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES fitness_screen_running_distance_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    465                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES fitness_screen_prompt_3_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    68                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES fitness_screen_prompt_4_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES fitness_screen_prompt_5_properties =
{
    GX_STRING_ID_STRING_10,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES fitness_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_ICON_PROPERTIES fitness_screen_ekd_dot_guide_bg_properties =
{
    GX_PIXELMAP_ID_LANDING_EKG_DOT_GUIDE_BG, /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES fitness_screen_ekg_dot_icon_properties =
{
    GX_PIXELMAP_ID_LANDING_EKG_DOT,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES fitness_screen_ekg_waveform_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_RADIAL_PROGRESS_BAR_INFO fitness_screen_calorie_burned_progress_properties =
{
    239,                                     /* xcenter                        */
    330,                                     /* ycenter                        */
    28,                                      /* radius                         */
    -300,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    10,                                      /* normal brush width             */
    10,                                      /* selected brush width           */
    GX_COLOR_ID_GREEN_BG,                    /* normal brush color             */
    GX_COLOR_ID_GREEN,                       /* selected brush color           */
};
GX_ICON_PROPERTIES fitness_screen_calorie_icon_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_CALORIES_GREEN, /* normal pixelmap id          */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES fitness_screen_calorie_burned_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES fitness_screen_prompt_7_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO fitness_screen_standing_progress_properties =
{
    400,                                     /* xcenter                        */
    330,                                     /* ycenter                        */
    28,                                      /* radius                         */
    -300,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    10,                                      /* normal brush width             */
    10,                                      /* selected brush width           */
    GX_COLOR_ID_DARK_BLUE,                   /* normal brush color             */
    GX_COLOR_ID_BLUE,                        /* selected brush color           */
};
GX_ICON_PROPERTIES fitness_screen_icon_6_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_MAN_BLUE,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES fitness_screen_standing_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES fitness_screen_prompt_11_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO fitness_screen_running_progress_properties =
{
    318,                                     /* xcenter                        */
    330,                                     /* ycenter                        */
    28,                                      /* radius                         */
    -300,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    10,                                      /* normal brush width             */
    10,                                      /* selected brush width           */
    GX_COLOR_ID_YELLOW_BG,                   /* normal brush color             */
    GX_COLOR_ID_YELLOW,                      /* selected brush color           */
};
GX_ICON_PROPERTIES fitness_screen_icon_5_properties =
{
    GX_PIXELMAP_ID_LANDING_ICON_RUN_YELLOW,  /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES fitness_screen_running_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES fitness_screen_prompt_9_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_ekg_dot_icon_define =
{
    "ekg_dot_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_RED,                         /* normal color id                */
    GX_COLOR_ID_RED,                         /* selected color id              */
    GX_COLOR_ID_RED,                         /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {210, 242, 218, 250},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_ekg_dot_icon), /* control block */
    (void *) &fitness_screen_ekg_dot_icon_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_7_define =
{
    "prompt_7",
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
    {249, 333, 259, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_7), /* control block */
    (void *) &fitness_screen_prompt_7_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_calorie_burned_progress_val_define =
{
    "calorie_burned_progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {225, 333, 250, 353},                    /* widget size                    */
    &fitness_screen_prompt_7_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_calorie_burned_progress_val), /* control block */
    (void *) &fitness_screen_calorie_burned_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_calorie_icon_define =
{
    "calorie_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {232, 309, 247, 331},                    /* widget size                    */
    &fitness_screen_calorie_burned_progress_val_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_calorie_icon), /* control block */
    (void *) &fitness_screen_calorie_icon_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_calorie_burned_progress_define =
{
    "calorie_burned_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {206, 297, 272, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &fitness_screen_calorie_icon_define,     /* child widget definition        */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_calorie_burned_progress), /* control block */
    (void *) &fitness_screen_calorie_burned_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_11_define =
{
    "prompt_11",
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
    {411, 333, 421, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_11), /* control block */
    (void *) &fitness_screen_prompt_11_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_standing_progress_val_define =
{
    "standing_progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {387, 333, 412, 353},                    /* widget size                    */
    &fitness_screen_prompt_11_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_standing_progress_val), /* control block */
    (void *) &fitness_screen_standing_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_icon_6_define =
{
    "icon_6",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {395, 310, 404, 333},                    /* widget size                    */
    &fitness_screen_standing_progress_val_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_icon_6), /* control block */
    (void *) &fitness_screen_icon_6_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_standing_progress_define =
{
    "standing_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {367, 297, 433, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &fitness_screen_icon_6_define,           /* child widget definition        */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_standing_progress), /* control block */
    (void *) &fitness_screen_standing_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_9_define =
{
    "prompt_9",
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
    {328, 333, 338, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_9), /* control block */
    (void *) &fitness_screen_prompt_9_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_running_progress_val_define =
{
    "running_progress_val",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {304, 333, 329, 353},                    /* widget size                    */
    &fitness_screen_prompt_9_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_running_progress_val), /* control block */
    (void *) &fitness_screen_running_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_icon_5_define =
{
    "icon_5",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {305, 311, 328, 331},                    /* widget size                    */
    &fitness_screen_running_progress_val_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_icon_5), /* control block */
    (void *) &fitness_screen_icon_5_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_running_progress_define =
{
    "running_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {285, 297, 351, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &fitness_screen_icon_5_define,           /* child widget definition        */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_running_progress), /* control block */
    (void *) &fitness_screen_running_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_running_button_define =
{
    "running_button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_RUNNING_BUTTON,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {285, 297, 351, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &fitness_screen_running_progress_define, /* child widget definition        */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_running_button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_standing_button_define =
{
    "standing_button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_STANDING_BUTTON,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {367, 297, 433, 363},                    /* widget size                    */
    &fitness_screen_running_button_define,   /* next widget definition         */
    &fitness_screen_standing_progress_define, /* child widget definition       */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_standing_button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_calorie_button_define =
{
    "calorie_button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    ID_CALORIE_BUTTON,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {206, 297, 272, 363},                    /* widget size                    */
    &fitness_screen_standing_button_define,  /* next widget definition         */
    &fitness_screen_calorie_burned_progress_define, /* child widget definition */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_calorie_button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_ekg_waveform_win_define =
{
    "ekg_waveform_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) ekg_waveform_win_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {219, 216, 428, 265},                    /* widget size                    */
    &fitness_screen_calorie_button_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_ekg_waveform_win), /* control block */
    (void *) &fitness_screen_ekg_waveform_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_ekd_dot_guide_bg_define =
{
    "ekd_dot_guide_bg",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {208, 216, 218, 274},                    /* widget size                    */
    &fitness_screen_ekg_waveform_win_define, /* next widget definition         */
    &fitness_screen_ekg_dot_icon_define,     /* child widget definition        */
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_ekd_dot_guide_bg), /* control block */
    (void *) &fitness_screen_ekd_dot_guide_bg_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_2_define =
{
    "prompt_2",
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
    {408, 181, 427, 198},                    /* widget size                    */
    &fitness_screen_ekd_dot_guide_bg_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_2), /* control block */
    (void *) &fitness_screen_prompt_2_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_5_define =
{
    "prompt_5",
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
    {328, 268, 357, 285},                    /* widget size                    */
    &fitness_screen_prompt_2_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_5), /* control block */
    (void *) &fitness_screen_prompt_5_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_4_define =
{
    "prompt_4",
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
    {393, 268, 427, 285},                    /* widget size                    */
    &fitness_screen_prompt_5_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_4), /* control block */
    (void *) &fitness_screen_prompt_4_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_prompt_3_define =
{
    "prompt_3",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {365, 265, 390, 285},                    /* widget size                    */
    &fitness_screen_prompt_4_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_prompt_3), /* control block */
    (void *) &fitness_screen_prompt_3_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_running_distance_define =
{
    "running_distance",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {364, 178, 407, 198},                    /* widget size                    */
    &fitness_screen_prompt_3_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_running_distance), /* control block */
    (void *) &fitness_screen_running_distance_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_running_steps_define =
{
    "running_steps",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {237, 177, 313, 197},                    /* widget size                    */
    &fitness_screen_running_distance_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_running_steps), /* control block */
    (void *) &fitness_screen_running_steps_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_icon_3_define =
{
    "icon_3",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {334, 196, 442, 205},                    /* widget size                    */
    &fitness_screen_running_steps_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_icon_3), /* control block */
    (void *) &fitness_screen_icon_3_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_icon_2_define =
{
    "icon_2",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {208, 195, 316, 204},                    /* widget size                    */
    &fitness_screen_icon_3_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_icon_2), /* control block */
    (void *) &fitness_screen_icon_2_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_icon_1_define =
{
    "icon_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {328, 159, 351, 192},                    /* widget size                    */
    &fitness_screen_icon_2_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_icon_1), /* control block */
    (void *) &fitness_screen_icon_1_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_icon_define =
{
    "icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_YELLOW,                      /* normal color id                */
    GX_COLOR_ID_YELLOW,                      /* selected color id              */
    GX_COLOR_ID_YELLOW,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {195, 159, 223, 193},                    /* widget size                    */
    &fitness_screen_icon_1_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(FITNESS_SCREEN_CONTROL_BLOCK, fitness_screen_icon), /* control block */
    (void *) &fitness_screen_icon_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET fitness_screen_define =
{
    "fitness_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(FITNESS_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) fitness_screen_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &fitness_screen_icon_define,             /* child widget                   */
    0,                                       /* control block                  */
    (void *) &fitness_screen_properties      /* extended properties            */
};
GX_TEMPLATE_PROPERTIES clock_2_screen_properties =
{
    &clock_screen_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET clock_2_screen_define =
{
    "clock_2_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_CLOCK_2_SCREEN,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLOCK_2_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &clock_2_screen_properties      /* extended properties            */
};
GX_TEMPLATE_PROPERTIES clock_1_screen_properties =
{
    &clock_screen_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET clock_1_screen_define =
{
    "clock_1_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_CLOCK_1_SCREEN,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLOCK_1_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &clock_1_screen_properties      /* extended properties            */
};
GX_WINDOW_PROPERTIES clock_5_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_NUMERIC_PROMPT_PROPERTIES clock_5_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_LARGE,                 /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    12                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES clock_5_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_LARGE,                 /* font id                        */
    GX_COLOR_ID_YELLOW,                      /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_YELLOW,                      /* disabled text color            */
    GX_NULL,                                 /* format function                */
    12                                       /* numeric prompt value           */
};

GX_CONST GX_STUDIO_WIDGET clock_5_screen_minute_define =
{
    "minute",
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
    {251, 258, 418, 348},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_5_SCREEN_CONTROL_BLOCK, clock_5_screen_minute), /* control block */
    (void *) &clock_5_screen_minute_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET clock_5_screen_hour_define =
{
    "hour",
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
    {206, 132, 373, 222},                    /* widget size                    */
    &clock_5_screen_minute_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_5_SCREEN_CONTROL_BLOCK, clock_5_screen_hour), /* control block */
    (void *) &clock_5_screen_hour_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET clock_5_screen_define =
{
    "clock_5_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLOCK_5_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) clock_5_screen_draw, /* drawing function override  */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) clock_5_screen_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &clock_5_screen_hour_define,             /* child widget                   */
    0,                                       /* control block                  */
    (void *) &clock_5_screen_properties      /* extended properties            */
};
GX_WINDOW_PROPERTIES clock_3_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_NUMERIC_PROMPT_PROPERTIES clock_3_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_LARGE,                 /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    12                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES clock_3_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_LARGE,                 /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    49                                       /* numeric prompt value           */
};

GX_CONST GX_STUDIO_WIDGET clock_3_screen_minute_define =
{
    "minute",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {228, 259, 395, 349},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_3_SCREEN_CONTROL_BLOCK, clock_3_screen_minute), /* control block */
    (void *) &clock_3_screen_minute_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET clock_3_screen_hour_define =
{
    "hour",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {228, 133, 395, 223},                    /* widget size                    */
    &clock_3_screen_minute_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_3_SCREEN_CONTROL_BLOCK, clock_3_screen_hour), /* control block */
    (void *) &clock_3_screen_hour_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET clock_3_screen_define =
{
    "clock_3_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLOCK_3_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) clock_3_screen_draw, /* drawing function override  */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) clock_3_screen_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &clock_3_screen_hour_define,             /* child widget                   */
    0,                                       /* control block                  */
    (void *) &clock_3_screen_properties      /* extended properties            */
};
GX_WINDOW_PROPERTIES clock_4_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_NUMERIC_PROMPT_PROPERTIES clock_4_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_LARGE,                 /* font id                        */
    GX_COLOR_ID_YELLOW,                      /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_YELLOW,                      /* disabled text color            */
    GX_NULL,                                 /* format function                */
    12                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES clock_4_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER_LARGE,                 /* font id                        */
    GX_COLOR_ID_BLUE,                        /* normal text color              */
    GX_COLOR_ID_BLUE,                        /* selected text color            */
    GX_COLOR_ID_BLUE,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    12                                       /* numeric prompt value           */
};

GX_CONST GX_STUDIO_WIDGET clock_4_screen_minute_define =
{
    "minute",
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
    {227, 258, 394, 348},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_4_SCREEN_CONTROL_BLOCK, clock_4_screen_minute), /* control block */
    (void *) &clock_4_screen_minute_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET clock_4_screen_hour_define =
{
    "hour",
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
    {257, 132, 424, 222},                    /* widget size                    */
    &clock_4_screen_minute_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLOCK_4_SCREEN_CONTROL_BLOCK, clock_4_screen_hour), /* control block */
    (void *) &clock_4_screen_hour_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET clock_4_screen_define =
{
    "clock_4_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLOCK_4_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) clock_4_screen_event_process, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &clock_4_screen_hour_define,             /* child widget                   */
    0,                                       /* control block                  */
    (void *) &clock_4_screen_properties      /* extended properties            */
};
GX_WINDOW_PROPERTIES home_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_SPRITE_FRAME home_screen_sprite_frame_list[19] =
{
    {
        GX_PIXELMAP_ID_EARTH_258X289_F0,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F1,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F2,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F3,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F4,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F5,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F6,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F7,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F8,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F9,     /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F10,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F11,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F12,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F13,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F14,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F15,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F16,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F17,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_EARTH_258X289_F18,    /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        8,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    }
};

GX_SPRITE_PROPERTIES home_screen_sprite_properties =
{
    home_screen_sprite_frame_list,           /* address of frame list          */
    19,                                      /* frame count                    */
};
GX_NUMERIC_PROMPT_PROPERTIES home_screen_time_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    12                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES home_screen_time_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NUMBER,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_format,                             /* format function                */
    55                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES home_screen_time_second_up_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES home_screen_time_second_down_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES home_screen_am_pm_properties =
{
    GX_STRING_ID_PM,                         /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES home_screen_day_of_week_properties =
{
    GX_STRING_ID_WEDNESDAY,                  /* string id                      */
    GX_FONT_ID_NORMAL_BOLD,                  /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES home_screen_date_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET home_screen_date_define =
{
    "date",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {306, 307, 412, 324},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_date), /* control block    */
    (void *) &home_screen_date_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET home_screen_day_of_week_define =
{
    "day_of_week",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {203, 307, 306, 323},                    /* widget size                    */
    &home_screen_date_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_day_of_week), /* control block */
    (void *) &home_screen_day_of_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET home_screen_am_pm_define =
{
    "am_pm",
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
    {355, 268, 377, 285},                    /* widget size                    */
    &home_screen_day_of_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_am_pm), /* control block   */
    (void *) &home_screen_am_pm_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET home_screen_time_second_down_define =
{
    "time_second_down",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {304, 246, 308, 250},                    /* widget size                    */
    &home_screen_am_pm_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_time_second_down), /* control block */
    (void *) &home_screen_time_second_down_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET home_screen_time_second_up_define =
{
    "time_second_up",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {304, 230, 308, 234},                    /* widget size                    */
    &home_screen_time_second_down_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_time_second_up), /* control block */
    (void *) &home_screen_time_second_up_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET home_screen_time_minute_define =
{
    "time_minute",
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
    {314, 217, 377, 268},                    /* widget size                    */
    &home_screen_time_second_up_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_time_minute), /* control block */
    (void *) &home_screen_time_minute_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET home_screen_time_hour_define =
{
    "time_hour",
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
    {235, 217, 298, 268},                    /* widget size                    */
    &home_screen_time_minute_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_time_hour), /* control block */
    (void *) &home_screen_time_hour_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET home_screen_sprite_define =
{
    "sprite",
    GX_TYPE_SPRITE,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_SPRITE_LOOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_SPRITE),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_sprite_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &home_screen_time_hour_define,           /* child widget definition        */
    offsetof(HOME_SCREEN_CONTROL_BLOCK, home_screen_sprite), /* control block  */
    (void *) &home_screen_sprite_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET home_screen_define =
{
    "home_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(HOME_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) home_screen_event_handler, /* event function override */
    {185, 96, 442, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &home_screen_sprite_define,              /* child widget                   */
    0,                                       /* control block                  */
    (void *) &home_screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES music_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_RADIAL_PROGRESS_BAR_INFO music_screen_play_progress_properties =
{
    319,                                     /* xcenter                        */
    260,                                     /* ycenter                        */
    52,                                      /* radius                         */
    -180,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    20,                                      /* normal brush width             */
    20,                                      /* selected brush width           */
    GX_COLOR_ID_BLUE,                        /* normal brush color             */
    GX_COLOR_ID_WHITE,                       /* selected brush color           */
};
GX_PIXELMAP_BUTTON_PROPERTIES music_screen_play_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_ICON_PLAY,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_PLAYER_ICON_PAUSE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES music_screen_singer_properties =
{
    GX_STRING_ID_STRING_21,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES music_screen_song_properties =
{
    GX_STRING_ID_STRING_30,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES music_screen_reverse_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_ICON_REVERSE,      /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES music_screen_forward_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_ICON_FORWARD,      /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES music_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    time_format,                             /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES music_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    time_format,                             /* format function                */
    32                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES music_screen_second_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES music_screen_play_mode_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES music_screen_play_list_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_MENU_LIST,         /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES music_screen_play_randon_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_MENU_RANDOM,       /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES music_screen_play_repeat_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_MENU_REPEAT,       /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET music_screen_play_button_define =
{
    "play_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PLAY_BUTTON,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {299, 239, 338, 280},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_play_button), /* control block */
    (void *) &music_screen_play_button_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET music_screen_play_repeat_button_define =
{
    "play_repeat_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {367, 336, 411, 369},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_play_repeat_button), /* control block */
    (void *) &music_screen_play_repeat_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET music_screen_play_randon_button_define =
{
    "play_randon_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {296, 336, 339, 365},                    /* widget size                    */
    &music_screen_play_repeat_button_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_play_randon_button), /* control block */
    (void *) &music_screen_play_randon_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET music_screen_play_list_button_define =
{
    "play_list_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {226, 337, 269, 366},                    /* widget size                    */
    &music_screen_play_randon_button_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_play_list_button), /* control block */
    (void *) &music_screen_play_list_button_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET music_screen_play_mode_win_define =
{
    "play_mode_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {226, 331, 418, 371},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &music_screen_play_list_button_define,   /* child widget definition        */
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_play_mode_win), /* control block */
    (void *) &music_screen_play_mode_win_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET music_screen_second_define =
{
    "second",
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
    {401, 199, 404, 216},                    /* widget size                    */
    &music_screen_play_mode_win_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_second), /* control block */
    (void *) &music_screen_second_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET music_screen_minute_define =
{
    "minute",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {406, 200, 423, 217},                    /* widget size                    */
    &music_screen_second_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_minute), /* control block */
    (void *) &music_screen_minute_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET music_screen_hour_define =
{
    "hour",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {383, 200, 400, 217},                    /* widget size                    */
    &music_screen_minute_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_hour), /* control block  */
    (void *) &music_screen_hour_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET music_screen_forward_button_define =
{
    "forward_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_FORWARD_BUTTON,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_DARK_BLUE,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {400, 247, 437, 274},                    /* widget size                    */
    &music_screen_hour_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_forward_button), /* control block */
    (void *) &music_screen_forward_button_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET music_screen_reverse_button_define =
{
    "reverse_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_REVERSE_BUTTON,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_DARK_BLUE,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {205, 247, 242, 274},                    /* widget size                    */
    &music_screen_forward_button_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_reverse_button), /* control block */
    (void *) &music_screen_reverse_button_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET music_screen_song_define =
{
    "song",
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
    {231, 168, 410, 185},                    /* widget size                    */
    &music_screen_reverse_button_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_song), /* control block  */
    (void *) &music_screen_song_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET music_screen_singer_define =
{
    "singer",
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
    {275, 139, 389, 159},                    /* widget size                    */
    &music_screen_song_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_singer), /* control block */
    (void *) &music_screen_singer_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET music_screen_play_progress_define =
{
    "play_progress",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {257, 198, 381, 322},                    /* widget size                    */
    &music_screen_singer_define,             /* next widget definition         */
    &music_screen_play_button_define,        /* child widget definition        */
    offsetof(MUSIC_SCREEN_CONTROL_BLOCK, music_screen_play_progress), /* control block */
    (void *) &music_screen_play_progress_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET music_screen_define =
{
    "music_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MUSIC_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) music_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &music_screen_play_progress_define,      /* child widget                   */
    0,                                       /* control block                  */
    (void *) &music_screen_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES message_screen_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_WINDOW_PROPERTIES message_screen_message_frame_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_SCROLLBAR_APPEARANCE  message_screen_vertical_scroll_1_properties =
{
    10,                                      /* scroll width                   */
    10,                                      /* thumb width                    */
    0,                                       /* thumb travel min               */
    0,                                       /* thumb travel max               */
    4,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    GX_PIXELMAP_ID_SCROLL_BAR,               /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb color             */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb border color      */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll button color            */
};

GX_CONST GX_STUDIO_WIDGET message_screen_vertical_scroll_1_define =
{
    "vertical_scroll_1",
    GX_TYPE_VERTICAL_SCROLL,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_SCROLLBAR_VERTICAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_SCROLL_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_vertical_scrollbar_create,     /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {451, 150, 460, 382},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MESSAGE_SCREEN_CONTROL_BLOCK, message_screen_vertical_scroll_1), /* control block */
    (void *) &message_screen_vertical_scroll_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET message_screen_message_frame_define =
{
    "message_frame",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 149, 448, 383},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &message_screen_vertical_scroll_1_define, /* child widget definition       */
    offsetof(MESSAGE_SCREEN_CONTROL_BLOCK, message_screen_message_frame), /* control block */
    (void *) &message_screen_message_frame_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET message_screen_define =
{
    "message_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MESSAGE_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) message_screen_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &message_screen_message_frame_define,    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &message_screen_properties      /* extended properties            */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_properties =
{
    &screen_template_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};
GX_WINDOW_PROPERTIES weather_screen_template_weather_frame_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_SCROLLBAR_APPEARANCE  weather_screen_template_vertical_scroll_properties =
{
    10,                                      /* scroll width                   */
    10,                                      /* thumb width                    */
    0,                                       /* thumb travel min               */
    0,                                       /* thumb travel max               */
    4,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    GX_PIXELMAP_ID_SCROLL_BAR,               /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb color             */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb border color      */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll button color            */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_7_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 425, 434, 471}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_6_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 379, 434, 425}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_5_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 333, 434, 379}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_4_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 287, 434, 333}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_3_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 241, 434, 287}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_2_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 195, 434, 241}                     /* widget size                    */
};
GX_TEMPLATE_PROPERTIES weather_screen_template_day_win_1_properties =
{
    &weather_info_template_define,           /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {204, 149, 434, 195}                     /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_1_define =
{
    "day_win_1",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 149, 434, 195},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_1), /* control block */
    (void *) &weather_screen_template_day_win_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_2_define =
{
    "day_win_2",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 195, 434, 241},                    /* widget size                    */
    &weather_screen_template_day_win_1_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_2), /* control block */
    (void *) &weather_screen_template_day_win_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_3_define =
{
    "day_win_3",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 241, 434, 287},                    /* widget size                    */
    &weather_screen_template_day_win_2_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_3), /* control block */
    (void *) &weather_screen_template_day_win_3_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_4_define =
{
    "day_win_4",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 287, 434, 333},                    /* widget size                    */
    &weather_screen_template_day_win_3_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_4), /* control block */
    (void *) &weather_screen_template_day_win_4_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_5_define =
{
    "day_win_5",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 333, 434, 379},                    /* widget size                    */
    &weather_screen_template_day_win_4_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_5), /* control block */
    (void *) &weather_screen_template_day_win_5_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_6_define =
{
    "day_win_6",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 379, 434, 425},                    /* widget size                    */
    &weather_screen_template_day_win_5_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_6), /* control block */
    (void *) &weather_screen_template_day_win_6_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_day_win_7_define =
{
    "day_win_7",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_INFO_TEMPLATE_CONTROL_BLOCK), /* control block size         */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {204, 425, 434, 471},                    /* widget size                    */
    &weather_screen_template_day_win_6_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_day_win_7), /* control block */
    (void *) &weather_screen_template_day_win_7_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_vertical_scroll_define =
{
    "vertical_scroll",
    GX_TYPE_VERTICAL_SCROLL,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_SCROLLBAR_VERTICAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_SCROLL_FILL,                 /* normal color id                */
    GX_COLOR_ID_SCROLL_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_vertical_scrollbar_create,     /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {438, 150, 447, 382},                    /* widget size                    */
    &weather_screen_template_day_win_7_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_vertical_scroll), /* control block */
    (void *) &weather_screen_template_vertical_scroll_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_weather_frame_define =
{
    "weather_frame",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 149, 448, 383},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &weather_screen_template_vertical_scroll_define, /* child widget definition */
    offsetof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK, weather_screen_template_weather_frame), /* control block */
    (void *) &weather_screen_template_weather_frame_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_template_define =
{
    "weather_screen_template",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_TEMPLATE,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) weather_screen_template_event_process, /* event function override */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &weather_screen_template_weather_frame_define, /* child widget             */
    0,                                       /* control block                  */
    (void *) &weather_screen_template_properties /* extended properties        */
};
GX_TEMPLATE_PROPERTIES SanFrancisco_weather_screen_properties =
{
    &weather_screen_template_define,         /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET SanFrancisco_weather_screen_define =
{
    "SanFrancisco_weather_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_SAN_FRANCISCO_SCREEN,                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SANFRANCISCO_WEATHER_SCREEN_CONTROL_BLOCK), /* control block size   */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &SanFrancisco_weather_screen_properties /* extended properties    */
};
GX_TEMPLATE_PROPERTIES LosAngeles_weather_screen_properties =
{
    &weather_screen_template_define,         /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET LosAngeles_weather_screen_define =
{
    "LosAngeles_weather_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_LOS_ANGELES_SCREEN,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOSANGELES_WEATHER_SCREEN_CONTROL_BLOCK), /* control block size     */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &LosAngeles_weather_screen_properties /* extended properties      */
};
GX_TEMPLATE_PROPERTIES SanDiego_weather_screen_properties =
{
    &weather_screen_template_define,         /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {191, 95, 448, 383}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET SanDiego_weather_screen_define =
{
    "SanDiego_weather_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_SAN_DIEGO_SCREEN,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SANDIEGO_WEATHER_SCREEN_CONTROL_BLOCK), /* control block size       */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 95, 448, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &SanDiego_weather_screen_properties /* extended properties        */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY guix_smart_watch_widget_table[] =
{
    { &screen_slide_parent_define, (GX_WIDGET *) &screen_slide_parent },
    { &home_button_define, (GX_WIDGET *) &home_button },
    { &page_3_container_screen_define, (GX_WIDGET *) &page_3_container_screen },
    { &page_2_container_screen_define, (GX_WIDGET *) &page_2_container_screen },
    { &page_1_container_screen_define, (GX_WIDGET *) &page_1_container_screen },
    { &main_screen_define, (GX_WIDGET *) &main_screen },
    { &weather_screen_define, (GX_WIDGET *) &weather_screen },
    { &music_screen_define, (GX_WIDGET *) &music_screen },
    { &message_screen_define, (GX_WIDGET *) &message_screen },
    { &SanFrancisco_weather_screen_define, (GX_WIDGET *) &SanFrancisco_weather_screen },
    { &LosAngeles_weather_screen_define, (GX_WIDGET *) &LosAngeles_weather_screen },
    { &SanDiego_weather_screen_define, (GX_WIDGET *) &SanDiego_weather_screen },
    { &ekg_screen_define, (GX_WIDGET *) &ekg_screen },
    { &yoga_screen_define, (GX_WIDGET *) &yoga_screen },
    { &stand_screen_define, (GX_WIDGET *) &stand_screen },
    { &run_screen_define, (GX_WIDGET *) &run_screen },
    { &calories_screen_define, (GX_WIDGET *) &calories_screen },
    { &fitness_screen_define, (GX_WIDGET *) &fitness_screen },
    { &clock_2_screen_define, (GX_WIDGET *) &clock_2_screen },
    { &clock_1_screen_define, (GX_WIDGET *) &clock_1_screen },
    { &clock_5_screen_define, (GX_WIDGET *) &clock_5_screen },
    { &clock_3_screen_define, (GX_WIDGET *) &clock_3_screen },
    { &clock_4_screen_define, (GX_WIDGET *) &clock_4_screen },
    { &home_screen_define, (GX_WIDGET *) &home_screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = guix_smart_watch_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &guix_smart_watch_display_table[display];


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
