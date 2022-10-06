#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "guix_simple_resources.h"
#include "guix_simple_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);

SIMPLE_WINDOW_CONTROL_BLOCK simple_window;

GX_DISPLAY main_display_control_block;
GX_WINDOW_ROOT main_display_root_window;
GX_CANVAS main_display_canvas_control_block;
ULONG main_display_canvas_memory[MAIN_DISPLAY_X_RESOLUTION * MAIN_DISPLAY_Y_RESOLUTION];

extern GX_CONST GX_THEME *main_display_theme_table[];
extern GX_CONST GX_STRING *main_display_language_table[];

GX_STUDIO_DISPLAY_INFO guix_simple_display_table[1] = {
    {
        "main_display",
        "main_display_canvas",
        main_display_theme_table,
        main_display_language_table,
        MAIN_DISPLAY_THEME_TABLE_SIZE,
        MAIN_DISPLAY_LANGUAGE_TABLE_SIZE,
        MAIN_DISPLAY_STRING_TABLE_SIZE,
        MAIN_DISPLAY_X_RESOLUTION,              /* x resolution                   */
        MAIN_DISPLAY_Y_RESOLUTION,              /* y resolution                   */
        &main_display_control_block,
        &main_display_canvas_control_block,
        &main_display_root_window,
        main_display_canvas_memory,              /* canvas memory area             */
        /*1228800,*/                             /* canvas memory size in bytes    */
        MAIN_DISPLAY_X_RESOLUTION * MAIN_DISPLAY_Y_RESOLUTION * sizeof(ULONG),
        GX_SCREEN_ROTATION_NONE                  /* rotation angle                 */
    }
};

UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PIXELMAP_BUTTON *button = (GX_PIXELMAP_BUTTON *)control_block;
    GX_PIXELMAP_BUTTON_PROPERTIES *props = (GX_PIXELMAP_BUTTON_PROPERTIES *)info->properties;

    status = gx_pixelmap_button_create(button, info->widget_name, parent, props->normal_pixelmap_id, props->selected_pixelmap_id, props->disabled_pixelmap_id, info->style, info->widget_id, &info->size);
    return status;
}

UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_ICON *icon = (GX_ICON *)control_block;
    GX_ICON_PROPERTIES *props = (GX_ICON_PROPERTIES *)info->properties;

    status = gx_icon_create(icon, info->widget_name, parent, props->normal_pixelmap_id, info->style, info->widget_id, info->size.gx_rectangle_left, info->size.gx_rectangle_top);
    if (props->selected_pixelmap_id) {
        gx_icon_pixelmap_set(icon, props->normal_pixelmap_id, props->selected_pixelmap_id);
    } else {
        gx_widget_resize((GX_WIDGET *)icon, (GX_RECTANGLE *)&info->size);
    }

    return status;
}

UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_PROMPT *prompt = (GX_PROMPT *)control_block;
    GX_PROMPT_PROPERTIES *props = (GX_PROMPT_PROPERTIES *)info->properties;

    status = gx_prompt_create(prompt, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS) {
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
    GX_WINDOW *window = (GX_WINDOW *)control_block;
    GX_WINDOW_PROPERTIES *props = (GX_WINDOW_PROPERTIES *)info->properties;

    status = gx_window_create(window, info->widget_name, parent, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS) {
        if (props->wallpaper_id) {
            gx_window_wallpaper_set(window, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }

    return status;
}

GX_WINDOW_PROPERTIES simple_window_properties = {
    0                                        /* wallpaper pixelmap id          */
};

GX_ICON_PROPERTIES simple_window_prompt_border_properties = {
    GX_PIXELMAP_ID_PROMPT_BORDER,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_PROMPT_PROPERTIES simple_window_hello_properties = {
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};

GX_ICON_PROPERTIES simple_window_express_logo_properties = {
    GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_PIXELMAP_BUTTON_PROPERTIES simple_window_simple_button_properties = {
    GX_PIXELMAP_ID_BUTTON,                   /* normal pixelmap id             */
    GX_PIXELMAP_ID_BUTTON_ACTIVE,            /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_PROMPT_PROPERTIES simple_window_button_text_properties = {
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET simple_window_hello_define = {
    "hello",
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
    {174, 213, 306, 230},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SIMPLE_WINDOW_CONTROL_BLOCK, simple_window_hello), /* control block */
    (void *)&simple_window_hello_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET simple_window_button_text_define = {
    "button_text",
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
    {272, 355, 371, 378},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SIMPLE_WINDOW_CONTROL_BLOCK, simple_window_button_text), /* control block */
    (void *) &simple_window_button_text_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET simple_window_simple_button_define = {
    "simple_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {250, 345, 393, 392},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &simple_window_button_text_define,       /* child widget definition        */
    offsetof(SIMPLE_WINDOW_CONTROL_BLOCK, simple_window_simple_button), /* control block */
    (void *) &simple_window_simple_button_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET simple_window_express_logo_define = {
    "express_logo",
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
    {18, 18, 206, 47},                       /* widget size                    */
    &simple_window_simple_button_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SIMPLE_WINDOW_CONTROL_BLOCK, simple_window_express_logo), /* control block */
    (void *) &simple_window_express_logo_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET simple_window_prompt_border_define = {
    "prompt_border",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
#if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
#endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {166, 202, 475, 241},                    /* widget size                    */
    &simple_window_express_logo_define,      /* next widget definition         */
    &simple_window_hello_define,             /* child widget definition        */
    offsetof(SIMPLE_WINDOW_CONTROL_BLOCK, simple_window_prompt_border), /* control block */
    (void *) &simple_window_prompt_border_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET simple_window_define = {
    "simple_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
#if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
#endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SIMPLE_WINDOW_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &simple_window_prompt_border_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &simple_window_properties       /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET_ENTRY guix_simple_widget_table[] = {
    { &simple_window_define, (GX_WIDGET *)&simple_window },
    {GX_NULL, GX_NULL}
};

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent)
{
    GX_VALUE list_count = 0;
    UINT status = GX_SUCCESS;
    GX_WIDGET *widget = GX_NULL;
    GX_VALUE list_total_count = 0;

    if (parent && (parent->gx_widget_type == GX_TYPE_MENU)) {
        list_total_count = ((GX_MENU *)parent)->gx_menu_list_total_count;
    }

    while (definition && status == GX_SUCCESS) {
        if (definition->create_function) {
            if (definition->style & GX_STYLE_DYNAMICALLY_ALLOCATED) {
                status = gx_widget_allocate(&widget, definition->control_block_size);
                if (status != GX_SUCCESS) {
                    return GX_NULL;
                }
            } else {
                if (control == GX_NULL) {
                    return GX_NULL;
                }

                widget = (GX_WIDGET *)(control + definition->control_block_offset);
            }

            status = definition->create_function(definition, widget, parent);

            if (list_count < list_total_count) {
                gx_menu_insert((GX_MENU *)parent, widget);
                ((GX_MENU *)parent)->gx_menu_list_total_count--;
                list_count++;
            }

            if (status == GX_SUCCESS) {
                if (definition->widget_type != GX_TYPE_TEMPLATE) {
#if defined(GUIX_5_4_0_COMPATIBILITY)
                    gx_widget_fill_color_set(widget, definition->normal_fill_color_id, definition->selected_fill_color_id);
#else
                    gx_widget_fill_color_set(widget, definition->normal_fill_color_id, definition->selected_fill_color_id, definition->disabled_fill_color_id);
#endif
                }

                if (!(definition->status & GX_STATUS_ACCEPTS_FOCUS)) {
                    gx_widget_status_remove(widget, GX_STATUS_ACCEPTS_FOCUS);
                }

                if (definition->draw_function) {
                    gx_widget_draw_set(widget, definition->draw_function);
                }

                if (definition->event_function) {
                    gx_widget_event_process_set(widget, definition->event_function);
                }

#if defined(GX_WIDGET_USER_DATA)
                widget->gx_widget_user_data = definition->user_data;
#endif
                if (definition->child_widget) {
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
    if (parent && widget) {
        gx_widget_attach(parent, widget);
    }

    return widget;
}

UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget)
{
    UINT status = GX_FAILURE;
    GX_WIDGET *widget = GX_NULL;
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = guix_simple_widget_table;

    while (entry->widget_information) {
        if (!strcmp(name, entry->widget_information->widget_name)) {
            widget = gx_studio_widget_create((GX_BYTE *) entry->widget, entry->widget_information, parent);
            if (widget) {
                status = GX_SUCCESS;
            }

            break;
        }

        entry++;
    }

    if (new_widget) {
        *new_widget = widget;
    }

    return status;
}

UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root)
{
    GX_RECTANGLE size;
    GX_CONST GX_THEME *theme_ptr;
    GX_STUDIO_DISPLAY_INFO *display_info = &guix_simple_display_table[display];

    // 创建请求的显示
    gx_display_create(display_info->display, display_info->name, driver, (GX_VALUE)display_info->x_resolution, (GX_VALUE)display_info->y_resolution);

    // 安装请求主题
    if (display_info->theme_table) {
        theme_ptr = display_info->theme_table[theme];
        if (theme_ptr) {
            gx_display_color_table_set(display_info->display, theme_ptr->theme_color_table, theme_ptr->theme_color_table_size);

            // 如果需要，安装调色板
            if (display_info->display->gx_display_driver_palette_set && theme_ptr->theme_palette != NULL) {
                display_info->display->gx_display_driver_palette_set(display_info->display, theme_ptr->theme_palette, theme_ptr->theme_palette_size);
            }

            gx_display_font_table_set(display_info->display, theme_ptr->theme_font_table, theme_ptr->theme_font_table_size);
            gx_display_pixelmap_table_set(display_info->display, theme_ptr->theme_pixelmap_table, theme_ptr->theme_pixelmap_table_size);
            gx_system_scroll_appearance_set(theme_ptr->theme_vertical_scroll_style, (GX_SCROLLBAR_APPEARANCE *)&theme_ptr->theme_vertical_scrollbar_appearance);
            gx_system_scroll_appearance_set(theme_ptr->theme_horizontal_scroll_style, (GX_SCROLLBAR_APPEARANCE *)&theme_ptr->theme_horizontal_scrollbar_appearance);
        }
    }

    // 安装语言表
    if (display_info->language_table) {
        gx_display_language_table_set_ext(display_info->display, display_info->language_table, (GX_UBYTE) display_info->language_table_size, display_info->string_table_size);
        gx_display_active_language_set(display_info->display, language);
    }

    // 设置屏幕旋转角度
    display_info->display->gx_display_rotation_angle = display_info->rotation_angle;

    // 为这个显示创建画布
    gx_canvas_create(display_info->canvas, display_info->canvas_name, display_info->display, GX_CANVAS_MANAGED | GX_CANVAS_VISIBLE, display_info->x_resolution, display_info->y_resolution, display_info->canvas_memory, display_info->canvas_memory_size);

    // 为这个画布创建根窗口
    gx_utility_rectangle_define(&size, 0, 0, (GX_VALUE)(display_info->x_resolution - 1), (GX_VALUE)(display_info->y_resolution - 1));

    gx_window_root_create(display_info->root_window, display_info->name, display_info->canvas, GX_STYLE_NONE, 0, &size);
    if (return_root) {
        *return_root = display_info->root_window;
    }

    return GX_SUCCESS;
}
#undef GUIX_STUDIO_GENERATED_FILE
