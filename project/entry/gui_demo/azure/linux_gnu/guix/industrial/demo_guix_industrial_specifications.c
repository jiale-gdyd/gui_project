#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "demo_guix_industrial_resources.h"
#include "demo_guix_industrial_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
SEQUENCE_NUMBER_CONTROL_BLOCK sequence_number;
COMPLETE_WINDOW_CONTROL_BLOCK complete_window;
SEQUENCE_WINDOW_CONTROL_BLOCK sequence_window;
MAIN_SCREEN_CONTROL_BLOCK main_screen;
GX_DISPLAY main_display_control_block;
GX_WINDOW_ROOT main_display_root_window;
GX_CANVAS  main_display_canvas_control_block;
ULONG      main_display_canvas_memory[307200];

extern GX_CONST GX_THEME *main_display_theme_table[];
extern GX_CONST GX_STRING *main_display_language_table[];

GX_STUDIO_DISPLAY_INFO demo_guix_industrial_display_table[1] =
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

static VOID gx_studio_screen_toggle(GX_WIDGET *target1, GX_WIDGET *target2)
{
    GX_WIDGET *parent = target1->gx_widget_parent;
    if (parent)
    {
        gx_widget_detach(target1);
        gx_widget_attach(parent, target2);
        if (target1->gx_widget_status & GX_STATUS_STUDIO_CREATED)
        {
            gx_widget_delete(target1);
        }
    }
}

static GX_WIDGET *gx_studio_action_target_get(GX_WIDGET *current, GX_CONST GX_STUDIO_ACTION *action)
{
    GX_WIDGET *parent = GX_NULL;
    GX_WIDGET *target = GX_NULL;
    INT        search_depth;
    GX_STUDIO_WIDGET *widget_define;

    if (action->flags & GX_ACTION_FLAG_DYNAMIC_TARGET)
    {
                                             /* dynamically create the target widget */
        widget_define = (GX_STUDIO_WIDGET *) action->target;
        if(action->flags & GX_ACTION_FLAG_DYNAMIC_PARENT)
        {
            gx_window_root_find(current, (GX_WINDOW_ROOT **)&parent);
            search_depth = GX_SEARCH_DEPTH_INFINITE;
        }
        else
        {
            parent = (GX_WIDGET *)action->parent;
            search_depth = 1;
        }
        gx_widget_find(parent, widget_define->widget_id, search_depth, &target);
        if (target == GX_NULL)
        {
            target = gx_studio_widget_create(GX_NULL, widget_define, GX_NULL);
        }
        if (target)
        {
            target->gx_widget_status |= GX_STATUS_STUDIO_CREATED;
        }
    }
    else
    {
        target = (GX_WIDGET *) action->target;
    }
    return target;
}

static GX_WIDGET *gx_studio_action_target_find(GX_WIDGET *current, GX_CONST GX_STUDIO_ACTION *action)
{
    GX_WIDGET *parent = GX_NULL;
    GX_WIDGET *target = GX_NULL;
    GX_STUDIO_WIDGET *widget_define;

    if (action->flags & GX_ACTION_FLAG_DYNAMIC_TARGET)
    {
                                             /* Find the dynamically created target */
        widget_define = (GX_STUDIO_WIDGET *) action->target;
        if(action->flags & GX_ACTION_FLAG_DYNAMIC_PARENT)
        {
            gx_window_root_find(current, (GX_WINDOW_ROOT **)&parent);
        }
        else
        {
            parent = (GX_WIDGET *)action->parent;
        }
        gx_widget_find(parent, widget_define->widget_id, GX_SEARCH_DEPTH_INFINITE, &target);
    }
    else
    {
        target = (GX_WIDGET *) action->target;
    }
    return target;
}

static GX_WIDGET *gx_studio_action_parent_find(GX_WIDGET *current, GX_CONST GX_STUDIO_ACTION *action)
{
GX_WIDGET *parent = GX_NULL;
GX_STUDIO_WIDGET *widget_define;

    if (action->flags & GX_ACTION_FLAG_DYNAMIC_PARENT)
    {
                                             /* Find the dynamically created target */
        widget_define = (GX_STUDIO_WIDGET *)action->parent; 
        gx_window_root_find(current, (GX_WINDOW_ROOT **)&parent); 
        gx_widget_find(parent, widget_define->widget_id, GX_SEARCH_DEPTH_INFINITE, &parent); 
    }
    else
    {
        parent = (GX_WIDGET *)action->parent; 
    }
    return parent; 
}

static VOID gx_studio_animation_execute(GX_WIDGET *current, GX_CONST GX_STUDIO_ACTION *action)
{
    GX_ANIMATION *animation;
    GX_ANIMATION_INFO animation_info;
    GX_WIDGET *parent = GX_NULL;
    GX_WIDGET *target = GX_NULL;
    gx_system_animation_get(&animation);
    if (animation)
    {
        animation_info = *action->animation;

        if((action->flags & GX_ACTION_FLAG_POP_TARGET) ||
           (action->flags & GX_ACTION_FLAG_POP_PARENT))
        {
            gx_system_screen_stack_get((GX_WIDGET **)&parent, &target);
        }

        if(action->flags & GX_ACTION_FLAG_POP_TARGET)
        {
            animation_info.gx_animation_target = target;
        }

        if(action->flags & GX_ACTION_FLAG_POP_PARENT)
        {
            animation_info.gx_animation_parent = (GX_WIDGET *)parent;
        }

        if ((!animation_info.gx_animation_target) &&
            (action->flags & GX_ACTION_FLAG_DYNAMIC_TARGET))
        {
            target = gx_studio_action_target_get(current, action);
            animation_info.gx_animation_target = target;
        }

        if (!animation_info.gx_animation_parent)
        {
            animation_info.gx_animation_parent = gx_studio_action_parent_find(current, action);
        }

        if (animation_info.gx_animation_target &&
            animation_info.gx_animation_parent)
        {
            gx_animation_start(animation, &animation_info);
        }
    }
}

UINT gx_studio_auto_event_handler(GX_WIDGET *widget, GX_EVENT *event_ptr, GX_CONST GX_STUDIO_EVENT_PROCESS *record)
{
    UINT status = GX_SUCCESS;
    GX_CONST GX_STUDIO_ACTION *action;
    GX_CONST GX_WIDGET *parent = GX_NULL;
    GX_WIDGET *target = GX_NULL;
    GX_CONST GX_STUDIO_EVENT_ENTRY *entry = record->event_table;

    while(entry->event_type)
    {
        if (entry->event_type == event_ptr->gx_event_type)
        {
            if((entry->event_type == GX_EVENT_ANIMATION_COMPLETE) &&
               (entry->event_sender != event_ptr->gx_event_sender))
            {
                entry++;
                continue;
            }
            action = entry->action_list;

            while(action->opcode)
            {
                switch(action->opcode)
                {
                case GX_ACTION_TYPE_ATTACH:
                    if((action->flags & GX_ACTION_FLAG_POP_TARGET) ||
                       (action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        gx_system_screen_stack_get((GX_WIDGET **)&parent, &target);
                    }

                    if(!(action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        parent = action->parent;
                    }
                    if(!(action->flags & GX_ACTION_FLAG_POP_TARGET))
                    {
                        target = gx_studio_action_target_get(widget, action);
                    }
                    if (parent && target)
                    {
                        gx_widget_attach(parent, target);
                    }
                    break;

                case GX_ACTION_TYPE_DETACH:
                    target = gx_studio_action_target_find(widget, action);
                    if (target)
                    {
                        gx_widget_detach(target);
                        if (target->gx_widget_status & GX_STATUS_STUDIO_CREATED)
                        {
                            gx_widget_delete(target);
                        }
                    }
                    break;

                case GX_ACTION_TYPE_TOGGLE:
                    if(action->flags & GX_ACTION_FLAG_POP_TARGET)
                    {
                       gx_system_screen_stack_get(GX_NULL, &target);
                    }
                    else
                    {
                        target = gx_studio_action_target_get(widget, action);
                    }
                    gx_studio_screen_toggle(widget, target);
                    break;

                case GX_ACTION_TYPE_SHOW:
                    target = gx_studio_action_target_get(widget, action);
                    if(target)
                    {
                        gx_widget_show(target);
                    }
                    break;

                case GX_ACTION_TYPE_HIDE:
                    target = gx_studio_action_target_find(widget, action);
                    if(target)
                    {
                        gx_widget_hide(target);
                    }
                    break;

                case GX_ACTION_TYPE_ANIMATION:
                    gx_studio_animation_execute(widget, action);
                    break;

                case GX_ACTION_TYPE_WINDOW_EXECUTE:
                    if((action->flags & GX_ACTION_FLAG_POP_TARGET) ||
                       (action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        gx_system_screen_stack_get((GX_WIDGET **)&parent, &target);
                    }

                    if(!(action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        parent = widget->gx_widget_parent;
                    }
                    if(!(action->flags & GX_ACTION_FLAG_POP_TARGET))
                    {
                        target = gx_studio_action_target_get(widget, action);
                    }
                    if (parent && target)
                    {
                        gx_widget_attach(parent, target);
                        gx_window_execute((GX_WINDOW *) target, GX_NULL);
                    }
                    break;

                case GX_ACTION_TYPE_WINDOW_EXECUTE_STOP:
                    return event_ptr->gx_event_sender;

                case GX_ACTION_TYPE_SCREEN_STACK_PUSH:
                    target = gx_studio_action_target_get(widget, action);
                    if(target)
                    {
                        gx_system_screen_stack_push(target);
                    }
                    break;

                case GX_ACTION_TYPE_SCREEN_STACK_POP:
                    gx_system_screen_stack_pop();
                    break;

                case GX_ACTION_TYPE_SCREEN_STACK_RESET:
                    gx_system_screen_stack_reset();
                    break;

                default:
                    break;
                }
                action++;
            }
        }
        entry++;
    }

    if (record->chain_event_handler)
    {
        status = record->chain_event_handler(widget, event_ptr);
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
GX_WINDOW_PROPERTIES sequence_number_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES sequence_number_prompt_18_2_properties =
{
    GX_STRING_ID_STRING_66,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_number_prompt_18_3_properties =
{
    GX_STRING_ID_STRING_67,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET sequence_number_prompt_18_3_define =
{
    "prompt_18_3",
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
    {320, 37, 352, 54},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_NUMBER_CONTROL_BLOCK, sequence_number_prompt_18_3), /* control block */
    (void *) &sequence_number_prompt_18_3_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_number_prompt_18_2_define =
{
    "prompt_18_2",
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
    {264, 37, 313, 54},                      /* widget size                    */
    &sequence_number_prompt_18_3_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_NUMBER_CONTROL_BLOCK, sequence_number_prompt_18_2), /* control block */
    (void *) &sequence_number_prompt_18_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_number_define =
{
    "sequence_number",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SEQUENCE_NUMBER_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {259, 37, 358, 65},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &sequence_number_prompt_18_2_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &sequence_number_properties     /* extended properties            */
};
GX_WINDOW_PROPERTIES complete_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES complete_window_icon_12_2_properties =
{
    GX_PIXELMAP_ID_ICON_THUMB_UP,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES complete_window_prompt_17_7_properties =
{
    GX_STRING_ID_STRING_65,                  /* string id                      */
    GX_FONT_ID_MIDDLE_45,                    /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES complete_window_prompt_17_properties =
{
    GX_STRING_ID_STRING_66,                  /* string id                      */
    GX_FONT_ID_MIDDLE_30,                    /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES complete_window_prompt_17_2_properties =
{
    GX_STRING_ID_STRING_67,                  /* string id                      */
    GX_FONT_ID_MIDDLE_45,                    /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES complete_window_prompt_17_3_properties =
{
    GX_STRING_ID_STRING_69,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES complete_window_prompt_17_4_properties =
{
    GX_STRING_ID_STRING_39,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES complete_window_countdown_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE_30,                    /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    GX_NULL,                                 /* format function                */
    9                                        /* numeric prompt value           */
};
GX_ICON_PROPERTIES complete_window_icon_13_2_properties =
{
    GX_PIXELMAP_ID_VERTICAL_DIVIDER_CONFIRMATION, /* normal pixelmap id        */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET complete_window_countdown_define =
{
    "countdown",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {380, 340, 398, 370},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_countdown), /* control block */
    (void *) &complete_window_countdown_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET complete_window_icon_13_2_define =
{
    "icon_13_2",
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
    {309, 123, 310, 195},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_icon_13_2), /* control block */
    (void *) &complete_window_icon_13_2_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET complete_window_prompt_17_4_define =
{
    "prompt_17_4",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {188, 341, 462, 365},                    /* widget size                    */
    &complete_window_icon_13_2_define,       /* next widget definition         */
    &complete_window_countdown_define,       /* child widget definition        */
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_prompt_17_4), /* control block */
    (void *) &complete_window_prompt_17_4_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET complete_window_prompt_17_3_define =
{
    "prompt_17_3",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {182, 311, 474, 335},                    /* widget size                    */
    &complete_window_prompt_17_4_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_prompt_17_3), /* control block */
    (void *) &complete_window_prompt_17_3_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET complete_window_prompt_17_2_define =
{
    "prompt_17_2",
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
    {346, 153, 432, 199},                    /* widget size                    */
    &complete_window_prompt_17_3_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_prompt_17_2), /* control block */
    (void *) &complete_window_prompt_17_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET complete_window_prompt_17_define =
{
    "prompt_17",
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
    {351, 119, 434, 149},                    /* widget size                    */
    &complete_window_prompt_17_2_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_prompt_17), /* control block */
    (void *) &complete_window_prompt_17_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET complete_window_prompt_17_7_define =
{
    "prompt_17_7",
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
    {85, 233, 571, 279},                     /* widget size                    */
    &complete_window_prompt_17_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_prompt_17_7), /* control block */
    (void *) &complete_window_prompt_17_7_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET complete_window_icon_12_2_define =
{
    "icon_12_2",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {190, 110, 267, 194},                    /* widget size                    */
    &complete_window_prompt_17_7_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(COMPLETE_WINDOW_CONTROL_BLOCK, complete_window_icon_12_2), /* control block */
    (void *) &complete_window_icon_12_2_properties /* extended properties      */
};

GX_ANIMATION_INFO complete_window_animation_1 = {
    (GX_WIDGET *) &complete_window,
    (GX_WIDGET *) &main_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_COMPLETE_WIN_FADE_IN, 0, 1,
    {74, 104}, {74, 104}, 0, 255, 20
};


GX_ANIMATION_INFO complete_window_animation_2 = {
    (GX_WIDGET *) &complete_window,
    (GX_WIDGET *) &main_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, ANI_ID_COMPLETE_WIN_FADE_OUT, 0, 1,
    {74, 104}, {74, 104}, 255, 0, 20
};


GX_STUDIO_ACTION complete_window_on_user_event_complete_win_fade_inactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen, &complete_window, &complete_window_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION complete_window_on_user_event_complete_win_fade_outactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen, &complete_window, &complete_window_animation_2},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_complete_window_event_table[] = {
    { USER_EVENT_COMPLETE_WIN_FADE_IN, 0, complete_window_on_user_event_complete_win_fade_inactions},
    { USER_EVENT_COMPLETE_WIN_FADE_OUT, 0, complete_window_on_user_event_complete_win_fade_outactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS complete_window_event_chain = {gx_studio_complete_window_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))complete_window_event_process};
static UINT gx_studio_complete_window_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &complete_window_event_chain));
}


GX_CONST GX_STUDIO_WIDGET complete_window_define =
{
    "complete_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_COMPLETE_WINDOW,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(COMPLETE_WINDOW_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_complete_window_event_process, /* event function override */
    {74, 104, 580, 400},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &complete_window_icon_12_2_define,       /* child widget                   */
    0,                                       /* control block                  */
    (void *) &complete_window_properties     /* extended properties            */
};
GX_WINDOW_PROPERTIES sequence_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES sequence_window_window_progress_properties =
{
    GX_PIXELMAP_ID_PROGRESS_BAR_BG           /* wallpaper pixelmap id          */
};
GX_PROGRESS_BAR_INFO sequence_window_bottom_progress_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    GX_PIXELMAP_ID_SEQUENCE_BIG_PROGRESS_ACTIVE  /* fill pixelmap              */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_7_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_1_title_properties =
{
    GX_STRING_ID_LEVEL_SURFACE,              /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_2_title_properties =
{
    GX_STRING_ID_ARM_AUXILIARY,              /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_3_title_properties =
{
    GX_STRING_ID_PALLET_FRICTION,            /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_4_title_properties =
{
    GX_STRING_ID_UNITSET,                    /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROGRESS_BAR_INFO sequence_window_progress_1_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0                                        /* fill pixelmap                  */
};
GX_PROGRESS_BAR_INFO sequence_window_progress_2_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0                                        /* fill pixelmap                  */
};
GX_PROGRESS_BAR_INFO sequence_window_progress_3_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0                                        /* fill pixelmap                  */
};
GX_PROGRESS_BAR_INFO sequence_window_progress_4_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0                                        /* fill pixelmap                  */
};
GX_ICON_PROPERTIES sequence_window_complete_icon_1_properties =
{
    GX_PIXELMAP_ID_LEFT_SIDE_CHECK_MARK,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES sequence_window_complete_icon_2_properties =
{
    GX_PIXELMAP_ID_LEFT_SIDE_CHECK_MARK,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES sequence_window_complete_icon_3_properties =
{
    GX_PIXELMAP_ID_LEFT_SIDE_CHECK_MARK,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES sequence_window_complete_icon_4_properties =
{
    GX_PIXELMAP_ID_LEFT_SIDE_CHECK_MARK,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES sequence_window_window_mode_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_5_properties =
{
    GX_STRING_ID_STRING_26,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_6_properties =
{
    GX_STRING_ID_STRING_35,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_7_properties =
{
    GX_STRING_ID_STRING_36,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_8_properties =
{
    GX_STRING_ID_STRING_37,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_10_1_properties =
{
    GX_STRING_ID_STRING_37,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_11_5_properties =
{
    GX_STRING_ID_STRING_12,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO sequence_window_radial_progress_speed_properties =
{
    591,                                     /* xcenter                        */
    146,                                     /* ycenter                        */
    22,                                      /* radius                         */
    -220,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SMALL,                        /* font_id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    4,                                       /* normal brush width             */
    4,                                       /* selected brush width           */
    GX_COLOR_ID_DARK_GRAY,                   /* normal brush color             */
    GX_COLOR_ID_GREEN,                       /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_SX_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_SY_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_WINDOW_PROPERTIES sequence_window_window_rotation_angle_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_13_2_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_14_2_properties =
{
    GX_STRING_ID_STRING_44,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_15_2_properties =
{
    GX_STRING_ID_STRING_45,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO sequence_window_radial_progress_rotation_properties =
{
    592,                                     /* xcenter                        */
    217,                                     /* ycenter                        */
    22,                                      /* radius                         */
    -250,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SMALL,                        /* font_id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    4,                                       /* normal brush width             */
    4,                                       /* selected brush width           */
    GX_COLOR_ID_DARK_GRAY,                   /* normal brush color             */
    GX_COLOR_ID_GREEN,                       /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_RY_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_RX_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_11_2_properties =
{
    GX_STRING_ID_STRING_47,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_11_properties =
{
    GX_STRING_ID_STRING_47,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES sequence_window_window_force_sensor_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_13_properties =
{
    GX_STRING_ID_STRING_28,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_14_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_15_properties =
{
    GX_STRING_ID_STRING_52,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_DN_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_UP_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY,                  /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_11_1_properties =
{
    GX_STRING_ID_STRING_56,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_11_3_properties =
{
    GX_STRING_ID_STRING_56,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_RADIAL_PROGRESS_BAR_INFO sequence_window_radial_progress_force_properties =
{
    592,                                     /* xcenter                        */
    286,                                     /* ycenter                        */
    22,                                      /* radius                         */
    -250,                                    /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SMALL,                        /* font_id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    4,                                       /* normal brush width             */
    4,                                       /* selected brush width           */
    GX_COLOR_ID_DARK_GRAY,                   /* normal brush color             */
    GX_COLOR_ID_GREEN,                       /* selected brush color           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_progress_1_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    100                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_progress_2_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    100                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_progress_3_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    100                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_progress_4_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    100                                      /* numeric prompt value           */
};
GX_ICON_PROPERTIES sequence_window_timer_icon_properties =
{
    GX_PIXELMAP_ID_ICON_TIMER,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES sequence_window_overall_label_properties =
{
    GX_STRING_ID_STRING_30,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_timer_tick_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    timer_value_format,                      /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_timer_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    timer_value_format,                      /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_timer_second_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    timer_value_format,                      /* format function                */
    10                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES sequence_window_timer_colon_properties =
{
    GX_STRING_ID_STRING_63,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_timer_dot_properties =
{
    GX_STRING_ID_STRING_64,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES sequence_window_task_icon_properties =
{
    GX_PIXELMAP_ID_CIRCLE_SEQUENCE,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES sequence_window_task_title_properties =
{
    GX_STRING_ID_PALLETIZING,                /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_bottom_progress_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    100                                      /* numeric prompt value           */
};
GX_WINDOW_PROPERTIES sequence_window_sprite_window_properties =
{
    GX_PIXELMAP_ID_ROBOT_BG                  /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_X_label_properties =
{
    GX_STRING_ID_STRING_57,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_X_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN,                       /* disabled text color            */
    position_value_format,                   /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_16_11_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_Y_label_properties =
{
    GX_STRING_ID_STRING_58,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_Y_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN,                       /* disabled text color            */
    position_value_format,                   /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_16_10_1_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_Z_label_properties =
{
    GX_STRING_ID_STRING_59,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES sequence_window_prompt_Z_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN,                       /* disabled text color            */
    mode_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES sequence_window_prompt_15_16_14_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_SPRITE_FRAME sequence_window_sprite_frame_list[24] =
{
    {
        GX_PIXELMAP_ID_INSPECTING_00,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_01,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_02,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_03,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_04,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_05,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_06,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_07,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_08,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_09,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_10,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_11,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_12,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_13,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_14,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_15,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_16,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_17,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_18,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_19,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_20,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_21,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_22,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_INSPECTING_23,        /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    }
};

GX_SPRITE_PROPERTIES sequence_window_sprite_properties =
{
    sequence_window_sprite_frame_list,       /* address of frame list          */
    24,                                      /* frame count                    */
};
GX_PROMPT_PROPERTIES sequence_window_progress_1_percent_flag_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_2_percent_flag_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_3_percent_flag_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES sequence_window_progress_4_percent_flag_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_7_define =
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
    {577, 357, 595, 377},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_7), /* control block */
    (void *) &sequence_window_prompt_7_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_bottom_progress_define =
{
    "bottom_progress",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_PROGRESS_PERCENT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 351, 600, 381},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &sequence_window_prompt_7_define,        /* child widget definition        */
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_bottom_progress), /* control block */
    (void *) &sequence_window_bottom_progress_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_SY_define =
{
    "prompt_SY",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {504, 155, 534, 172},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_SY), /* control block */
    (void *) &sequence_window_prompt_SY_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_SX_define =
{
    "prompt_SX",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {504, 139, 534, 156},                    /* widget size                    */
    &sequence_window_prompt_SY_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_SX), /* control block */
    (void *) &sequence_window_prompt_SX_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_radial_progress_speed_define =
{
    "radial_progress_speed",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_PROGRESS_PERCENT|GX_STYLE_PROGRESS_TEXT_DRAW|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {567, 122, 615, 170},                    /* widget size                    */
    &sequence_window_prompt_SX_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_radial_progress_speed), /* control block */
    (void *) &sequence_window_radial_progress_speed_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_11_5_define =
{
    "prompt_15_11_5",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {506, 123, 529, 135},                    /* widget size                    */
    &sequence_window_radial_progress_speed_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_11_5), /* control block */
    (void *) &sequence_window_prompt_15_11_5_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_10_1_define =
{
    "prompt_15_10_1",
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
    {535, 158, 556, 170},                    /* widget size                    */
    &sequence_window_prompt_15_11_5_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_10_1), /* control block */
    (void *) &sequence_window_prompt_15_10_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_8_define =
{
    "prompt_15_8",
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
    {535, 142, 556, 154},                    /* widget size                    */
    &sequence_window_prompt_15_10_1_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_8), /* control block */
    (void *) &sequence_window_prompt_15_8_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_7_define =
{
    "prompt_15_7",
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
    {466, 156, 484, 173},                    /* widget size                    */
    &sequence_window_prompt_15_8_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_7), /* control block */
    (void *) &sequence_window_prompt_15_7_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_6_define =
{
    "prompt_15_6",
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
    {466, 140, 484, 157},                    /* widget size                    */
    &sequence_window_prompt_15_7_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_6), /* control block */
    (void *) &sequence_window_prompt_15_6_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_5_define =
{
    "prompt_15_5",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {467, 123, 502, 135},                    /* widget size                    */
    &sequence_window_prompt_15_6_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_5), /* control block */
    (void *) &sequence_window_prompt_15_5_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_11_define =
{
    "prompt_15_11",
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
    {551, 226, 557, 238},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_11), /* control block */
    (void *) &sequence_window_prompt_15_11_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_11_2_define =
{
    "prompt_15_11_2",
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
    {551, 209, 557, 221},                    /* widget size                    */
    &sequence_window_prompt_15_11_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_11_2), /* control block */
    (void *) &sequence_window_prompt_15_11_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_RX_define =
{
    "prompt_RX",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {501, 211, 550, 228},                    /* widget size                    */
    &sequence_window_prompt_15_11_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_RX), /* control block */
    (void *) &sequence_window_prompt_RX_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_RY_define =
{
    "prompt_RY",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {501, 228, 550, 245},                    /* widget size                    */
    &sequence_window_prompt_RX_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_RY), /* control block */
    (void *) &sequence_window_prompt_RY_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_radial_progress_rotation_define =
{
    "radial_progress_rotation",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_PROGRESS_PERCENT|GX_STYLE_PROGRESS_TEXT_DRAW|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_RADIAL_PROGRESS_BAR_FILL,     /* normal color id               */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_RADIAL_PROGRESS_BAR_FILL,     /* disabled color id             */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {568, 193, 616, 241},                    /* widget size                    */
    &sequence_window_prompt_RY_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_radial_progress_rotation), /* control block */
    (void *) &sequence_window_radial_progress_rotation_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_15_2_define =
{
    "prompt_15_15_2",
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
    {466, 227, 484, 244},                    /* widget size                    */
    &sequence_window_radial_progress_rotation_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_15_2), /* control block */
    (void *) &sequence_window_prompt_15_15_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_14_2_define =
{
    "prompt_15_14_2",
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
    {466, 211, 484, 228},                    /* widget size                    */
    &sequence_window_prompt_15_15_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_14_2), /* control block */
    (void *) &sequence_window_prompt_15_14_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_13_2_define =
{
    "prompt_15_13_2",
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
    {467, 194, 554, 206},                    /* widget size                    */
    &sequence_window_prompt_15_14_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_13_2), /* control block */
    (void *) &sequence_window_prompt_15_13_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_radial_progress_force_define =
{
    "radial_progress_force",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_PROGRESS_PERCENT|GX_STYLE_PROGRESS_TEXT_DRAW|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_RADIAL_PROGRESS_BAR_FILL,     /* normal color id               */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_RADIAL_PROGRESS_BAR_FILL,     /* disabled color id             */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {568, 262, 616, 310},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_radial_progress_force), /* control block */
    (void *) &sequence_window_radial_progress_force_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_11_3_define =
{
    "prompt_15_11_3",
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
    {533, 298, 558, 310},                    /* widget size                    */
    &sequence_window_radial_progress_force_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_11_3), /* control block */
    (void *) &sequence_window_prompt_15_11_3_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_11_1_define =
{
    "prompt_15_11_1",
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
    {533, 282, 558, 294},                    /* widget size                    */
    &sequence_window_prompt_15_11_3_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_11_1), /* control block */
    (void *) &sequence_window_prompt_15_11_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_UP_define =
{
    "prompt_UP",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {492, 280, 531, 297},                    /* widget size                    */
    &sequence_window_prompt_15_11_1_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_UP), /* control block */
    (void *) &sequence_window_prompt_UP_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_DN_define =
{
    "prompt_DN",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {489, 296, 531, 313},                    /* widget size                    */
    &sequence_window_prompt_UP_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_DN), /* control block */
    (void *) &sequence_window_prompt_DN_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_15_define =
{
    "prompt_15_15",
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
    {466, 297, 488, 314},                    /* widget size                    */
    &sequence_window_prompt_DN_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_15), /* control block */
    (void *) &sequence_window_prompt_15_15_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_14_define =
{
    "prompt_15_14",
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
    {466, 280, 487, 297},                    /* widget size                    */
    &sequence_window_prompt_15_15_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_14), /* control block */
    (void *) &sequence_window_prompt_15_14_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_13_define =
{
    "prompt_15_13",
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
    {467, 263, 544, 275},                    /* widget size                    */
    &sequence_window_prompt_15_14_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_13), /* control block */
    (void *) &sequence_window_prompt_15_13_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_sprite_define =
{
    "sprite",
    GX_TYPE_SPRITE,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_SPRITE_LOOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SPRITE),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_sprite_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {207, 100, 446, 339},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_sprite), /* control block */
    (void *) &sequence_window_sprite_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_16_14_define =
{
    "prompt_15_16_14",
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
    {416, 112, 426, 124},                    /* widget size                    */
    &sequence_window_sprite_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_16_14), /* control block */
    (void *) &sequence_window_prompt_15_16_14_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_Z_define =
{
    "prompt_Z",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {384, 109, 414, 126},                    /* widget size                    */
    &sequence_window_prompt_15_16_14_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_Z), /* control block */
    (void *) &sequence_window_prompt_Z_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_Z_label_define =
{
    "prompt_Z_label",
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
    {371, 109, 380, 126},                    /* widget size                    */
    &sequence_window_prompt_Z_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_Z_label), /* control block */
    (void *) &sequence_window_prompt_Z_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_16_10_1_define =
{
    "prompt_15_16_10_1",
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
    {347, 112, 357, 124},                    /* widget size                    */
    &sequence_window_prompt_Z_label_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_16_10_1), /* control block */
    (void *) &sequence_window_prompt_15_16_10_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_Y_define =
{
    "prompt_Y",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {315, 109, 345, 126},                    /* widget size                    */
    &sequence_window_prompt_15_16_10_1_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_Y), /* control block */
    (void *) &sequence_window_prompt_Y_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_Y_label_define =
{
    "prompt_Y_label",
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
    {302, 109, 310, 126},                    /* widget size                    */
    &sequence_window_prompt_Y_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_Y_label), /* control block */
    (void *) &sequence_window_prompt_Y_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_15_16_11_define =
{
    "prompt_15_16_11",
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
    {271, 112, 281, 124},                    /* widget size                    */
    &sequence_window_prompt_Y_label_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_15_16_11), /* control block */
    (void *) &sequence_window_prompt_15_16_11_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_X_define =
{
    "prompt_X",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {239, 109, 269, 126},                    /* widget size                    */
    &sequence_window_prompt_15_16_11_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_X), /* control block */
    (void *) &sequence_window_prompt_X_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_prompt_X_label_define =
{
    "prompt_X_label",
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
    {225, 109, 233, 126},                    /* widget size                    */
    &sequence_window_prompt_X_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_prompt_X_label), /* control block */
    (void *) &sequence_window_prompt_X_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_4_percent_flag_define =
{
    "progress_4_percent_flag",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {159, 293, 173, 310},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_4_percent_flag), /* control block */
    (void *) &sequence_window_progress_4_percent_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_3_percent_flag_define =
{
    "progress_3_percent_flag",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {159, 253, 173, 270},                    /* widget size                    */
    &sequence_window_progress_4_percent_flag_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_3_percent_flag), /* control block */
    (void *) &sequence_window_progress_3_percent_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_2_percent_flag_define =
{
    "progress_2_percent_flag",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {159, 207, 173, 224},                    /* widget size                    */
    &sequence_window_progress_3_percent_flag_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_2_percent_flag), /* control block */
    (void *) &sequence_window_progress_2_percent_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_1_percent_flag_define =
{
    "progress_1_percent_flag",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {159, 167, 173, 184},                    /* widget size                    */
    &sequence_window_progress_2_percent_flag_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_1_percent_flag), /* control block */
    (void *) &sequence_window_progress_1_percent_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_sprite_window_define =
{
    "sprite_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {206, 100, 446, 340},                    /* widget size                    */
    &sequence_window_progress_1_percent_flag_define, /* next widget definition */
    &sequence_window_prompt_X_label_define,  /* child widget definition        */
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_sprite_window), /* control block */
    (void *) &sequence_window_sprite_window_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_bottom_progress_value_define =
{
    "bottom_progress_value",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {536, 358, 574, 378},                    /* widget size                    */
    &sequence_window_sprite_window_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_bottom_progress_value), /* control block */
    (void *) &sequence_window_bottom_progress_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_task_title_define =
{
    "task_title",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) task_title_draw, /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {19, 111, 143, 135},                     /* widget size                    */
    &sequence_window_bottom_progress_value_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_task_title), /* control block */
    (void *) &sequence_window_task_title_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_task_icon_define =
{
    "task_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    (VOID (*)(GX_WIDGET *)) task_icon_draw,  /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {148, 108, 177, 137},                    /* widget size                    */
    &sequence_window_task_title_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_task_icon), /* control block */
    (void *) &sequence_window_task_icon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_timer_dot_define =
{
    "timer_dot",
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
    {596, 81, 601, 98},                      /* widget size                    */
    &sequence_window_task_icon_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_timer_dot), /* control block */
    (void *) &sequence_window_timer_dot_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_timer_colon_define =
{
    "timer_colon",
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
    {570, 81, 575, 98},                      /* widget size                    */
    &sequence_window_timer_dot_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_timer_colon), /* control block */
    (void *) &sequence_window_timer_colon_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_timer_second_define =
{
    "timer_second",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {576, 82, 597, 99},                      /* widget size                    */
    &sequence_window_timer_colon_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_timer_second), /* control block */
    (void *) &sequence_window_timer_second_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_timer_minute_define =
{
    "timer_minute",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {550, 82, 571, 99},                      /* widget size                    */
    &sequence_window_timer_second_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_timer_minute), /* control block */
    (void *) &sequence_window_timer_minute_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_timer_tick_define =
{
    "timer_tick",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {602, 82, 619, 99},                      /* widget size                    */
    &sequence_window_timer_minute_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_timer_tick), /* control block */
    (void *) &sequence_window_timer_tick_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_overall_label_define =
{
    "overall_label",
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
    {481, 84, 521, 96},                      /* widget size                    */
    &sequence_window_timer_tick_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_overall_label), /* control block */
    (void *) &sequence_window_overall_label_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_timer_icon_define =
{
    "timer_icon",
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
    {461, 80, 476, 97},                      /* widget size                    */
    &sequence_window_overall_label_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_timer_icon), /* control block */
    (void *) &sequence_window_timer_icon_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_4_value_define =
{
    "progress_4_value",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {131, 292, 157, 309},                    /* widget size                    */
    &sequence_window_timer_icon_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_4_value), /* control block */
    (void *) &sequence_window_progress_4_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_3_value_define =
{
    "progress_3_value",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {131, 252, 157, 269},                    /* widget size                    */
    &sequence_window_progress_4_value_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_3_value), /* control block */
    (void *) &sequence_window_progress_3_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_2_value_define =
{
    "progress_2_value",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {131, 205, 157, 222},                    /* widget size                    */
    &sequence_window_progress_3_value_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_2_value), /* control block */
    (void *) &sequence_window_progress_2_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_1_value_define =
{
    "progress_1_value",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {131, 166, 157, 183},                    /* widget size                    */
    &sequence_window_progress_2_value_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_1_value), /* control block */
    (void *) &sequence_window_progress_1_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_window_force_sensor_define =
{
    "window_force_sensor",
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
    {460, 257, 620, 316},                    /* widget size                    */
    &sequence_window_progress_1_value_define, /* next widget definition        */
    &sequence_window_prompt_15_13_define,    /* child widget definition        */
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_window_force_sensor), /* control block */
    (void *) &sequence_window_window_force_sensor_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_window_rotation_angle_define =
{
    "window_rotation_angle",
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
    {460, 188, 620, 247},                    /* widget size                    */
    &sequence_window_window_force_sensor_define, /* next widget definition     */
    &sequence_window_prompt_15_13_2_define,  /* child widget definition        */
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_window_rotation_angle), /* control block */
    (void *) &sequence_window_window_rotation_angle_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_window_mode_define =
{
    "window_mode",
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
    {460, 117, 620, 176},                    /* widget size                    */
    &sequence_window_window_rotation_angle_define, /* next widget definition   */
    &sequence_window_prompt_15_5_define,     /* child widget definition        */
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_window_mode), /* control block */
    (void *) &sequence_window_window_mode_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_complete_icon_4_define =
{
    "complete_icon_4",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {155, 294, 171, 306},                    /* widget size                    */
    &sequence_window_window_mode_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_complete_icon_4), /* control block */
    (void *) &sequence_window_complete_icon_4_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_complete_icon_3_define =
{
    "complete_icon_3",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {155, 251, 171, 263},                    /* widget size                    */
    &sequence_window_complete_icon_4_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_complete_icon_3), /* control block */
    (void *) &sequence_window_complete_icon_3_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_complete_icon_2_define =
{
    "complete_icon_2",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {155, 210, 171, 222},                    /* widget size                    */
    &sequence_window_complete_icon_3_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_complete_icon_2), /* control block */
    (void *) &sequence_window_complete_icon_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_complete_icon_1_define =
{
    "complete_icon_1",
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
    {155, 167, 171, 179},                    /* widget size                    */
    &sequence_window_complete_icon_2_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_complete_icon_1), /* control block */
    (void *) &sequence_window_complete_icon_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_4_define =
{
    "progress_4",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_UNIT_SET,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_PROGRESS_PERCENT,   /* style flags           */
    0,                                       /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 313, 169, 315},                     /* widget size                    */
    &sequence_window_complete_icon_1_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_4), /* control block */
    (void *) &sequence_window_progress_4_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_3_define =
{
    "progress_3",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_PALLET_FRICTION,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_PROGRESS_PERCENT,   /* style flags           */
    0,                                       /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 271, 169, 273},                     /* widget size                    */
    &sequence_window_progress_4_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_3), /* control block */
    (void *) &sequence_window_progress_3_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_2_define =
{
    "progress_2",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_AUXILIARY,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_PROGRESS_PERCENT,   /* style flags           */
    0,                                       /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {21, 229, 170, 231},                     /* widget size                    */
    &sequence_window_progress_3_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_2), /* control block */
    (void *) &sequence_window_progress_2_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_1_define =
{
    "progress_1",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_PROGRESS_PERCENT,   /* style flags           */
    0,                                       /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 187, 169, 189},                     /* widget size                    */
    &sequence_window_progress_2_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_1), /* control block */
    (void *) &sequence_window_progress_1_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_4_title_define =
{
    "progress_4_title",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {19, 293, 132, 310},                     /* widget size                    */
    &sequence_window_progress_1_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_4_title), /* control block */
    (void *) &sequence_window_progress_4_title_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_3_title_define =
{
    "progress_3_title",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 251, 133, 268},                     /* widget size                    */
    &sequence_window_progress_4_title_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_3_title), /* control block */
    (void *) &sequence_window_progress_3_title_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_2_title_define =
{
    "progress_2_title",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {19, 210, 132, 227},                     /* widget size                    */
    &sequence_window_progress_3_title_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_2_title), /* control block */
    (void *) &sequence_window_progress_2_title_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_progress_1_title_define =
{
    "progress_1_title",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {19, 167, 132, 184},                     /* widget size                    */
    &sequence_window_progress_2_title_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_progress_1_title), /* control block */
    (void *) &sequence_window_progress_1_title_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET sequence_window_window_progress_define =
{
    "window_progress",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 351, 600, 381},                     /* widget size                    */
    &sequence_window_progress_1_title_define, /* next widget definition        */
    &sequence_window_bottom_progress_define, /* child widget definition        */
    offsetof(SEQUENCE_WINDOW_CONTROL_BLOCK, sequence_window_window_progress), /* control block */
    (void *) &sequence_window_window_progress_properties /* extended properties */
};

GX_ANIMATION_INFO sequence_window_animation_1 = {
    (GX_WIDGET *) &sequence_window,
    (GX_WIDGET *) &main_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_SEQ_WIN_FADE_IN, 0, 1,
    {0, 70}, {0, 70}, 0, 255, 20
};


GX_ANIMATION_INFO sequence_window_animation_2 = {
    (GX_WIDGET *) &sequence_window,
    (GX_WIDGET *) &main_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, ANI_ID_SEQ_WIN_FADE_OUT, 0, 1,
    {0, 70}, {0, 70}, 255, 0, 20
};


GX_STUDIO_ACTION sequence_window_on_user_event_seq_win_fade_inactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen, &sequence_window, &sequence_window_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION sequence_window_on_user_event_seq_win_fade_outactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen, &sequence_window, &sequence_window_animation_2},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_sequence_window_event_table[] = {
    { USER_EVENT_SEQ_WIN_FADE_IN, 0, sequence_window_on_user_event_seq_win_fade_inactions},
    { USER_EVENT_SEQ_WIN_FADE_OUT, 0, sequence_window_on_user_event_seq_win_fade_outactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS sequence_window_event_chain = {gx_studio_sequence_window_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))sequence_window_event_process};
static UINT gx_studio_sequence_window_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &sequence_window_event_chain));
}


GX_CONST GX_STUDIO_WIDGET sequence_window_define =
{
    "sequence_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SEQUENCE_WINDOW,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(SEQUENCE_WINDOW_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_sequence_window_event_process, /* event function override */
    {0, 70, 639, 396},                       /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &sequence_window_window_progress_define, /* child widget                   */
    0,                                       /* control block                  */
    (void *) &sequence_window_properties     /* extended properties            */
};
GX_WINDOW_PROPERTIES main_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_home_properties =
{
    GX_PIXELMAP_ID_ICON_SAFE_GUARD,          /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_expresslogic_label_properties =
{
    GX_PIXELMAP_ID_MICROSOFT_AZURE_LOGO,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_on_off_properties =
{
    GX_PIXELMAP_ID_MENU_BUTTON_LONG,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_MENU_BUTTON_LONG_PUSHED,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_on_off_properties =
{
    GX_STRING_ID_TURN_ON,                    /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_icon_9_properties =
{
    GX_PIXELMAP_ID_ICON_POWER,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES main_screen_button_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_inspecting_properties =
{
    GX_PIXELMAP_ID_MENU_BUTTON_C,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_MENU_BUTTON_C_PUSHED,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_14_4_properties =
{
    GX_STRING_ID_INSPECTING,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_icon_properties =
{
    GX_PIXELMAP_ID_ICON_INSPECT,             /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_palletizing_properties =
{
    GX_PIXELMAP_ID_MENU_BUTTON_C,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_MENU_BUTTON_C_PUSHED,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_14_properties =
{
    GX_STRING_ID_PALLETIZING,                /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_ICON_PALLET,              /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_button_indicator_dot_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_INDICATOR, /* normal pixelmap id            */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_welding_properties =
{
    GX_PIXELMAP_ID_MENU_BUTTON_C,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_MENU_BUTTON_C_PUSHED,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_14_3_properties =
{
    GX_STRING_ID_WELDING,                    /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_icon_2_properties =
{
    GX_PIXELMAP_ID_ICON_WELDING,             /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_assembling_properties =
{
    GX_PIXELMAP_ID_MENU_BUTTON_C,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_MENU_BUTTON_C_PUSHED,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_14_2_properties =
{
    GX_STRING_ID_ASSEMBLING,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_icon_3_properties =
{
    GX_PIXELMAP_ID_ICON_ASSEMBLING,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES main_screen_start_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_button_start_properties =
{
    GX_PIXELMAP_ID_BIG_BUTTON,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_BIG_BUTTON_PUSHED,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_robot_properties =
{
    GX_PIXELMAP_ID_ROBOT_UP,                 /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_initiate_properties =
{
    GX_STRING_ID_STRING_2,                   /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_Start_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_WINDOW_PROPERTIES main_screen_window_mode_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES main_screen_prompt_3_properties =
{
    GX_STRING_ID_STRING_10,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_2_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_4_properties =
{
    GX_STRING_ID_STRING_12,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_3_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_5_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_4_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_6_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_WINDOW_PROPERTIES main_screen_window_speed_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES main_screen_prompt_properties =
{
    GX_STRING_ID_STRING_15,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_16,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_1_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_17,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_10_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_10_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_WINDOW_PROPERTIES main_screen_window_timer_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_properties =
{
    GX_STRING_ID_STRING_19,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_11_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_12_properties =
{
    GX_STRING_ID_STRING_20,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_12_properties =
{
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_OUTLINED_BUTTON_HOLDER_PUSHED, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_13_properties =
{
    GX_STRING_ID_STRING_21,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GRAY                   /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_initiate_1_properties =
{
    GX_STRING_ID_STRING_1,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_9_define =
{
    "icon_9",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {533, 405, 547, 421},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_9), /* control block  */
    (void *) &main_screen_icon_9_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_on_off_define =
{
    "prompt_on_off",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {515, 426, 563, 438},                    /* widget size                    */
    &main_screen_icon_9_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_on_off), /* control block */
    (void *) &main_screen_prompt_on_off_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_define =
{
    "icon",
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
    {75, 406, 89, 422},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon), /* control block    */
    (void *) &main_screen_icon_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_14_4_define =
{
    "prompt_14_4",
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
    {51, 426, 114, 438},                     /* widget size                    */
    &main_screen_icon_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_14_4), /* control block */
    (void *) &main_screen_prompt_14_4_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_indicator_dot_define =
{
    "button_indicator_dot",
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
    {450, 401, 454, 406},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_indicator_dot), /* control block */
    (void *) &main_screen_button_indicator_dot_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_1_define =
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
    {403, 406, 419, 422},                    /* widget size                    */
    &main_screen_button_indicator_dot_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_1), /* control block  */
    (void *) &main_screen_icon_1_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_14_define =
{
    "prompt_14",
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
    {382, 426, 441, 438},                    /* widget size                    */
    &main_screen_icon_1_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_14), /* control block */
    (void *) &main_screen_prompt_14_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_2_define =
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
    {297, 406, 305, 422},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_2), /* control block  */
    (void *) &main_screen_icon_2_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_14_3_define =
{
    "prompt_14_3",
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
    {277, 426, 325, 438},                    /* widget size                    */
    &main_screen_icon_2_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_14_3), /* control block */
    (void *) &main_screen_prompt_14_3_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_3_define =
{
    "icon_3",
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
    {185, 406, 200, 422},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_3), /* control block  */
    (void *) &main_screen_icon_3_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_14_2_define =
{
    "prompt_14_2",
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
    {159, 426, 227, 438},                    /* widget size                    */
    &main_screen_icon_3_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_14_2), /* control block */
    (void *) &main_screen_prompt_14_2_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_assembling_define =
{
    "button_assembling",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_ASSEMBLING,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {129, 385, 253, 462},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_prompt_14_2_define,         /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_assembling), /* control block */
    (void *) &main_screen_button_assembling_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_welding_define =
{
    "button_welding",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_WELDING,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {238, 385, 362, 462},                    /* widget size                    */
    &main_screen_button_assembling_define,   /* next widget definition         */
    &main_screen_prompt_14_3_define,         /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_welding), /* control block */
    (void *) &main_screen_button_welding_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_palletizing_define =
{
    "button_palletizing",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PALLETIZING,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {348, 385, 472, 462},                    /* widget size                    */
    &main_screen_button_welding_define,      /* next widget definition         */
    &main_screen_prompt_14_define,           /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_palletizing), /* control block */
    (void *) &main_screen_button_palletizing_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_inspecting_define =
{
    "button_inspecting",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_INSPECTING,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 385, 144, 462},                     /* widget size                    */
    &main_screen_button_palletizing_define,  /* next widget definition         */
    &main_screen_prompt_14_4_define,         /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_inspecting), /* control block */
    (void *) &main_screen_button_inspecting_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_Start_define =
{
    "prompt_Start",
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
    {260, 152, 454, 234},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_Start), /* control block */
    (void *) &main_screen_prompt_Start_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_initiate_define =
{
    "prompt_initiate",
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
    {268, 233, 455, 250},                    /* widget size                    */
    &main_screen_prompt_Start_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_initiate), /* control block */
    (void *) &main_screen_prompt_initiate_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_robot_define =
{
    "icon_robot",
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
    {159, 151, 245, 250},                    /* widget size                    */
    &main_screen_prompt_initiate_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_robot), /* control block */
    (void *) &main_screen_icon_robot_properties /* extended properties         */
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {86, 351, 109, 363},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_4), /* control block */
    (void *) &main_screen_prompt_4_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_5_define =
{
    "prompt_5",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {138, 351, 161, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_5), /* control block */
    (void *) &main_screen_prompt_5_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_6_define =
{
    "prompt_6",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {192, 351, 215, 363},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_6), /* control block */
    (void *) &main_screen_prompt_6_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_4_define =
{
    "pixelmap_button_4",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {178, 333, 227, 382},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_prompt_6_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_4), /* control block */
    (void *) &main_screen_pixelmap_button_4_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_3_define =
{
    "pixelmap_button_3",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {125, 333, 174, 382},                    /* widget size                    */
    &main_screen_pixelmap_button_4_define,   /* next widget definition         */
    &main_screen_prompt_5_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_3), /* control block */
    (void *) &main_screen_pixelmap_button_3_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_2_define =
{
    "pixelmap_button_2",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {72, 333, 121, 382},                     /* widget size                    */
    &main_screen_pixelmap_button_3_define,   /* next widget definition         */
    &main_screen_prompt_4_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_2), /* control block */
    (void *) &main_screen_pixelmap_button_2_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_3_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {28, 336, 59, 348},                      /* widget size                    */
    &main_screen_pixelmap_button_2_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_3), /* control block */
    (void *) &main_screen_prompt_3_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_1_define =
{
    "prompt_1",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {300, 350, 327, 362},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_1), /* control block */
    (void *) &main_screen_prompt_1_properties /* extended properties           */
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {351, 350, 383, 362},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_2), /* control block */
    (void *) &main_screen_prompt_2_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_10_define =
{
    "prompt_10",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {409, 350, 433, 362},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_10), /* control block */
    (void *) &main_screen_prompt_10_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_10_define =
{
    "pixelmap_button_10",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {397, 332, 446, 381},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_prompt_10_define,           /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_10), /* control block */
    (void *) &main_screen_pixelmap_button_10_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_1_define =
{
    "pixelmap_button_1",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {343, 332, 392, 381},                    /* widget size                    */
    &main_screen_pixelmap_button_10_define,  /* next widget definition         */
    &main_screen_prompt_2_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_1), /* control block */
    (void *) &main_screen_pixelmap_button_1_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_define =
{
    "pixelmap_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {290, 332, 339, 381},                    /* widget size                    */
    &main_screen_pixelmap_button_1_define,   /* next widget definition         */
    &main_screen_prompt_1_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button), /* control block */
    (void *) &main_screen_pixelmap_button_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_define =
{
    "prompt",
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
    {243, 335, 280, 347},                    /* widget size                    */
    &main_screen_pixelmap_button_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt), /* control block  */
    (void *) &main_screen_prompt_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_12_define =
{
    "prompt_12",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {518, 350, 545, 362},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_12), /* control block */
    (void *) &main_screen_prompt_12_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_13_define =
{
    "prompt_13",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {568, 350, 600, 362},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_13), /* control block */
    (void *) &main_screen_prompt_13_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_12_define =
{
    "pixelmap_button_12",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {560, 332, 609, 381},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_prompt_13_define,           /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_12), /* control block */
    (void *) &main_screen_pixelmap_button_12_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_11_define =
{
    "pixelmap_button_11",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) mode_button_draw, /* drawing function override     */
    GX_NULL,                                 /* event function override        */
    {507, 332, 556, 381},                    /* widget size                    */
    &main_screen_pixelmap_button_12_define,  /* next widget definition         */
    &main_screen_prompt_12_define,           /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_11), /* control block */
    (void *) &main_screen_pixelmap_button_11_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_define =
{
    "prompt_11",
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
    {461, 335, 498, 347},                    /* widget size                    */
    &main_screen_pixelmap_button_11_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11), /* control block */
    (void *) &main_screen_prompt_11_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_window_timer_define =
{
    "window_timer",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {454, 326, 614, 382},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_prompt_11_define,           /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_window_timer), /* control block */
    (void *) &main_screen_window_timer_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_window_speed_define =
{
    "window_speed",
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
    {237, 327, 449, 383},                    /* widget size                    */
    &main_screen_window_timer_define,        /* next widget definition         */
    &main_screen_prompt_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_window_speed), /* control block */
    (void *) &main_screen_window_speed_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_window_mode_define =
{
    "window_mode",
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
    {19, 327, 231, 383},                     /* widget size                    */
    &main_screen_window_speed_define,        /* next widget definition         */
    &main_screen_prompt_3_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_window_mode), /* control block */
    (void *) &main_screen_window_mode_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_start_define =
{
    "button_start",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_START,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {93, 83, 545, 318},                      /* widget size                    */
    &main_screen_window_mode_define,         /* next widget definition         */
    &main_screen_icon_robot_define,          /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_start), /* control block */
    (void *) &main_screen_button_start_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_initiate_1_define =
{
    "prompt_initiate_1",
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
    {405, 37, 565, 54},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_initiate_1), /* control block */
    (void *) &main_screen_prompt_initiate_1_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_start_window_define =
{
    "start_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_START_WINDOW,                         /* widget id                      */
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
    {0, 82, 639, 384},                       /* widget size                    */
    &main_screen_prompt_initiate_1_define,   /* next widget definition         */
    &main_screen_button_start_define,        /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_start_window), /* control block */
    (void *) &main_screen_start_window_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_window_define =
{
    "button_window",
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
    {14, 384, 477, 470},                     /* widget size                    */
    &main_screen_start_window_define,        /* next widget definition         */
    &main_screen_button_inspecting_define,   /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_window), /* control block */
    (void *) &main_screen_button_window_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_on_off_define =
{
    "button_on_off",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_ON_OFF,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {459, 392, 619, 454},                    /* widget size                    */
    &main_screen_button_window_define,       /* next widget definition         */
    &main_screen_prompt_on_off_define,       /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_on_off), /* control block */
    (void *) &main_screen_button_on_off_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_expresslogic_label_define =
{
    "expresslogic_label",
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
    {29, 26, 131, 65},                       /* widget size                    */
    &main_screen_button_on_off_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_expresslogic_label), /* control block */
    (void *) &main_screen_expresslogic_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_button_home_define =
{
    "button_home",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {570, 24, 607, 60},                      /* widget size                    */
    &main_screen_expresslogic_label_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_button_home), /* control block */
    (void *) &main_screen_button_home_properties /* extended properties        */
};

GX_ANIMATION_INFO main_screen_animation_1 = {
    (GX_WIDGET *) &main_screen.main_screen_window_mode,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {22, 327}, {-213, 327}, 255, 0, 20
};


GX_ANIMATION_INFO main_screen_animation_2 = {
    (GX_WIDGET *) &main_screen.main_screen_button_start,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_START_BTN_SLIDE_OUT, 0, 1,
    {93, 83}, {-453, 83}, 255, 0, 20
};


GX_ANIMATION_INFO main_screen_animation_3 = {
    (GX_WIDGET *) &main_screen.main_screen_window_timer,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {458, 327}, {640, 327}, 255, 0, 20
};


GX_ANIMATION_INFO main_screen_animation_4 = {
    (GX_WIDGET *) &main_screen.main_screen_window_speed,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, 0, 0, 1,
    {240, 327}, {240, 327}, 255, 0, 20
};


GX_ANIMATION_INFO main_screen_animation_5 = {
    (GX_WIDGET *) &main_screen.main_screen_window_mode,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_MODE_WIN_SLIDE_IN, 0, 1,
    {-213, 327}, {22, 327}, 0, 255, 20
};


GX_ANIMATION_INFO main_screen_animation_6 = {
    (GX_WIDGET *) &main_screen.main_screen_window_speed,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_SPEED_WIN_FADE_IN, 0, 1,
    {240, 327}, {240, 327}, 0, 255, 20
};


GX_ANIMATION_INFO main_screen_animation_7 = {
    (GX_WIDGET *) &main_screen.main_screen_button_start,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_START_BTN_SLIDE_IN, 0, 1,
    {-453, 83}, {93, 83}, 0, 255, 20
};


GX_ANIMATION_INFO main_screen_animation_8 = {
    (GX_WIDGET *) &main_screen.main_screen_window_timer,
    (GX_WIDGET *) &main_screen.main_screen_start_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ANI_ID_TIMER_WIN_SLIDE_IN, 0, 1,
    {640, 327}, {458, 327}, 0, 255, 20
};


GX_STUDIO_ACTION main_screen_on_user_event_start_win_fade_outactions[5] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_mode, &main_screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_button_start, &main_screen_animation_2},
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_timer, &main_screen_animation_3},
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_speed, &main_screen_animation_4},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION main_screen_on_user_event_start_win_fade_inactions[5] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_mode, &main_screen_animation_5},
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_speed, &main_screen_animation_6},
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_button_start, &main_screen_animation_7},
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_timer, &main_screen_animation_8},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION main_screen_on_gx_event_animation_complete_id_main_screenon_gx_event_animation_complete_ani_id_start_btn_slide_out_actions[2] = {
    {GX_ACTION_TYPE_DETACH, 0, &main_screen.main_screen_start_window, &main_screen.main_screen_window_speed, GX_NULL},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_main_screen_event_table[] = {
    { USER_EVENT_START_WIN_FADE_OUT, 0, main_screen_on_user_event_start_win_fade_outactions},
    { USER_EVENT_START_WIN_FADE_IN, 0, main_screen_on_user_event_start_win_fade_inactions},
    { GX_EVENT_ANIMATION_COMPLETE, ANI_ID_START_BTN_SLIDE_OUT, main_screen_on_gx_event_animation_complete_id_main_screenon_gx_event_animation_complete_ani_id_start_btn_slide_out_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS main_screen_event_chain = {gx_studio_main_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))main_screen_event_process};
static UINT gx_studio_main_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &main_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET main_screen_define =
{
    "main_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_MAIN_SCREEN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(MAIN_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) main_screen_draw, /* drawing function override     */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_main_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &main_screen_button_home_define,         /* child widget                   */
    0,                                       /* control block                  */
    (void *) &main_screen_properties         /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY demo_guix_industrial_widget_table[] =
{
    { &sequence_number_define, (GX_WIDGET *) &sequence_number },
    { &complete_window_define, (GX_WIDGET *) &complete_window },
    { &sequence_window_define, (GX_WIDGET *) &sequence_window },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = demo_guix_industrial_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &demo_guix_industrial_display_table[display];


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
