#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "demo_guix_widget_animation_resources.h"
#include "demo_guix_widget_animation_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
MAIN_SCREEN_CONTROL_BLOCK main_screen;
GX_DISPLAY Primary_control_block;
GX_WINDOW_ROOT Primary_root_window;
GX_CANVAS  Primary_canvas_control_block;
ULONG      Primary_canvas_memory[307200];

extern GX_CONST GX_THEME *Primary_theme_table[];
extern GX_CONST GX_STRING *Primary_language_table[];

GX_STUDIO_DISPLAY_INFO demo_guix_widget_animation_display_table[1] =
{
    {
    "Primary",
    "Primary_canvas",
    Primary_theme_table,
    Primary_language_table,
    PRIMARY_THEME_TABLE_SIZE,
    PRIMARY_LANGUAGE_TABLE_SIZE,
    PRIMARY_STRING_TABLE_SIZE,
    640,                                     /* x resolution                   */
    480,                                     /* y resolution                   */
    &Primary_control_block,
    &Primary_canvas_control_block,
    &Primary_root_window,
    Primary_canvas_memory,                   /* canvas memory area             */
    1228800,                                 /* canvas memory size in bytes    */
    GX_SCREEN_ROTATION_NONE                  /* rotation angle                 */
    }
};


UINT gx_studio_icon_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_ICON_BUTTON *button = (GX_ICON_BUTTON *) control_block;
    GX_ICON_BUTTON_PROPERTIES *props = (GX_ICON_BUTTON_PROPERTIES *) info->properties;
    status = gx_icon_button_create(button, info->widget_name, parent, props->pixelmap_id, info->style, info->widget_id, &info->size);
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

UINT gx_studio_drop_list_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_DROP_LIST *list = (GX_DROP_LIST *) control_block;
    GX_DROP_LIST_PROPERTIES *props = (GX_DROP_LIST_PROPERTIES *) info->properties;
    status = gx_drop_list_create(list, info->widget_name, parent,
                                 props->total_rows, props->open_height,
                                 props->callback, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->pixelmap_id)
        {
            gx_drop_list_pixelmap_set(list, props->pixelmap_id);
        }
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW *)&list->gx_drop_list_popup.gx_popup_list_list, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }
    return status;
}
GX_WINDOW_PROPERTIES main_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES main_screen_title_properties =
{
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_SLATE_GRAY             /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_logo_properties =
{
    GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_DROP_LIST_PROPERTIES main_screen_easing_function_drop_list_properties =
{
    0,                                       /* widget pixelmap id             */
    0,                                       /* popup list wallpaper pixelmap id */
    GX_NULL,                                 /* callback function              */
    31,                                      /* total rows                     */
    250                                      /* open height                    */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_drop_list_button_properties =
{
    GX_PIXELMAP_ID_ICON_DROPDOWN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_DROPDOWN_ACTIVE,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_label_1_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_SLATE_GRAY             /* disabled text color            */
};
GX_WINDOW_PROPERTIES main_screen_easing_function_chart_win_properties =
{
    GX_PIXELMAP_ID_LINEAR                    /* wallpaper pixelmap id          */
};
GX_ICON_BUTTON_PROPERTIES main_screen_animation_button_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL  /* pixelmap id                  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_drop_list_button_define =
{
    "drop_list_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_DROP_LIST_BUTTON,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {430, 108, 477, 150},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_drop_list_button), /* control block */
    (void *) &main_screen_drop_list_button_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET main_screen_animation_button_define =
{
    "animation_button",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* normal color id                */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* selected color id              */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {89, 350, 110, 373},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_animation_button), /* control block */
    (void *) &main_screen_animation_button_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET main_screen_easing_function_chart_win_define =
{
    "easing_function_chart_win",
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
    {129, 158, 516, 437},                    /* widget size                    */
    &main_screen_animation_button_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_easing_function_chart_win), /* control block */
    (void *) &main_screen_easing_function_chart_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_label_1_define =
{
    "label_1",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {128, 74, 302, 91},                      /* widget size                    */
    &main_screen_easing_function_chart_win_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_label_1), /* control block */
    (void *) &main_screen_label_1_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_easing_function_drop_list_define =
{
    "easing_function_drop_list",
    GX_TYPE_DROP_LIST,                       /* widget type                    */
    ID_EASING_FUNCTION_DROP_LIST,            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK,                   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_DROP_LIST),                    /* control block size             */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* normal color id                */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* selected color id              */
    GX_COLOR_ID_LIGHT_SLATE_GRAY,            /* disabled color id              */
    gx_studio_drop_list_create,              /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {128, 107, 477, 150},                    /* widget size                    */
    &main_screen_label_1_define,             /* next widget definition         */
    &main_screen_drop_list_button_define,    /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_easing_function_drop_list), /* control block */
    (void *) &main_screen_easing_function_drop_list_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_logo_define =
{
    "logo",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {10, 10, 198, 39},                       /* widget size                    */
    &main_screen_easing_function_drop_list_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_logo), /* control block    */
    (void *) &main_screen_logo_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_title_define =
{
    "title",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {246, 25, 426, 45},                      /* widget size                    */
    &main_screen_logo_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_title), /* control block   */
    (void *) &main_screen_title_properties   /* extended properties            */
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
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) main_screen_event_process, /* event function override */
    {0, 0, 799, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &main_screen_title_define,               /* child widget                   */
    0,                                       /* control block                  */
    (void *) &main_screen_properties         /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY demo_guix_widget_animation_widget_table[] =
{
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = demo_guix_widget_animation_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &demo_guix_widget_animation_display_table[display];


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
