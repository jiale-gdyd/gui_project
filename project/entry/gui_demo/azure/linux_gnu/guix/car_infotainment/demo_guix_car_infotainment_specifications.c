#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "demo_guix_car_infotainment_resources.h"
#include "demo_guix_car_infotainment_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
VIDEO_SCREEN_CONTROL_BLOCK video_screen;
AUDIO_SCREEN_CONTROL_BLOCK audio_screen;
PHONE_SCREEN_CONTROL_BLOCK phone_screen;
CLIMATE_SCREEN_CONTROL_BLOCK climate_screen;
VEHICLE_SCREEN_CONTROL_BLOCK vehicle_screen;
MAIN_SCREEN_CONTROL_BLOCK main_screen;
GX_DISPLAY main_display_control_block;
GX_WINDOW_ROOT main_display_root_window;
GX_CANVAS  main_display_canvas_control_block;
ULONG      main_display_canvas_memory[307200];

extern GX_CONST GX_THEME *main_display_theme_table[];
extern GX_CONST GX_STRING *main_display_language_table[];

GX_STUDIO_DISPLAY_INFO demo_guix_car_infotainment_display_table[1] =
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
GX_WINDOW_PROPERTIES base_properties =
{
    GX_PIXELMAP_ID_BG_IMAGE                  /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES base_logo_properties =
{
    GX_PIXELMAP_ID_MICROSOFT_AZURE_LOGO,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET base_logo_define =
{
    "logo",
    GX_TYPE_ICON,                            /* widget type                    */
    ID_LOGO,                                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {43, 36, 145, 75},                       /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BASE_CONTROL_BLOCK, base_logo), /* control block                  */
    (void *) &base_logo_properties           /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET base_define =
{
    "base",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(BASE_CONTROL_BLOCK),              /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) base_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &base_logo_define,                       /* child widget                   */
    0,                                       /* control block                  */
    (void *) &base_properties                /* extended properties            */
};
GX_TEMPLATE_PROPERTIES main_screen_properties =
{
    &base_define,                            /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_ICON_PROPERTIES main_screen_speedometer_properties =
{
    GX_PIXELMAP_ID_INSTRUMENT_CLUSTER_FRAME, /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_MPH_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE_NUMBER_60,              /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_CANVAS,                      /* disabled text color            */
    GX_NULL,                                 /* format function                */
    64                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES main_screen_MPH_label_properties =
{
    GX_STRING_ID_STRING_73,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_gear_position_properties =
{
    GX_PIXELMAP_ID_INSTRUMENT_CLUSTER_SPEED_LETTER_DRIVE, /* normal pixelmap id */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_4_properties =
{
    GX_STRING_ID_STRING_83,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_5_properties =
{
    GX_STRING_ID_STRING_84,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_6_properties =
{
    GX_STRING_ID_STRING_85,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_7_properties =
{
    GX_STRING_ID_STRING_96,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_WINDOW_PROPERTIES main_screen_weather_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_time_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE_NUMBER_60,              /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    time_value_format,                       /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_time_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE_NUMBER_60,              /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    time_value_format,                       /* format function                */
    32                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES main_screen_weather_icon_properties =
{
    GX_PIXELMAP_ID_WEATHER_ICON_PARTLY_CLOUDY, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_temperature_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES main_screen_second_dot_up_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_second_dot_down_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES main_screen_btn_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_vehicle_btn_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ACTIVE,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_properties =
{
    GX_PIXELMAP_ID_BTN_ICON_VEHICLE,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ICON_VEHICLE_ACTIVE   /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_climate_btn_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ACTIVE,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_BTN_ICON_CLIMATE,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_65,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_phone_btn_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ACTIVE,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_2_properties =
{
    GX_PIXELMAP_ID_BTN_ICON_PHONE,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_audio_btn_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ACTIVE,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_3_properties =
{
    GX_PIXELMAP_ID_BTN_ICON_AUDIO,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_3_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_video_btn_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ACTIVE,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_4_properties =
{
    GX_PIXELMAP_ID_BTN_ICON_VIDEO,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_4_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_remain_miles_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_CANVAS,                      /* disabled text color            */
    GX_NULL,                                 /* format function                */
    382                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_remain_battery_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_CANVAS,                      /* disabled text color            */
    GX_NULL,                                 /* format function                */
    86                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_prompt_11_1_properties =
{
    GX_STRING_ID_STRING_69,                  /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_horizontal_line_properties =
{
    GX_PIXELMAP_ID_HOME_LEFT_LINE,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_battery_icon_properties =
{
    GX_PIXELMAP_ID_HOME_LEFT_BATTERY,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROGRESS_BAR_INFO main_screen_battery_progress_bar_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    86,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_HOME_LEFT_BATTERY_LEVEL   /* fill pixelmap                  */
};
GX_PROMPT_PROPERTIES main_screen_vehicle_mode_text_properties =
{
    GX_STRING_ID_ECO,                        /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_vehicle_mode_button_properties =
{
    GX_PIXELMAP_ID_HOME_MODE_BTN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MODE_BTN_OVER,       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_vehicle_mode_icon_properties =
{
    GX_PIXELMAP_ID_HOME_MODE_BTN_ICON_ECO,   /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_RADIAL_SLIDER_PROPERTIES main_screen_radial_slider_properties =
{
    141,                                     /* xcenter                        */
    141,                                     /* ycenter                        */
    123,                                     /* radius                         */
    36,                                      /* track width                    */
    120,                                     /* current angle                  */
    -52,                                     /* min angle                      */
    232,                                     /* max angle                      */
    GX_PIXELMAP_ID_INSTRUMENT_CLUSTER_SPEED_INDICATOR_BLUE, /* background pixelmap */
    0,                                       /* needle pixelmap                */
    15,                                      /* animation total steps          */
    2,                                       /* animation delay                */
    GX_ANIMATION_CIRC_EASE_IN_OUT,           /* animation style                */
    GX_NULL,                                 /* animation update callback func */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_on_off_btn_properties =
{
    GX_PIXELMAP_ID_BTN,                      /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_ACTIVE,               /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_icon_5_properties =
{
    GX_PIXELMAP_ID_BTN_ICON_POWER,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_on_off_text_properties =
{
    GX_STRING_ID_OFF,                        /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_7_define =
{
    "prompt_11_7",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {339, 284, 360, 312},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11_7), /* control block */
    (void *) &main_screen_prompt_11_7_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_6_define =
{
    "prompt_11_6",
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
    {318, 284, 338, 312},                    /* widget size                    */
    &main_screen_prompt_11_7_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11_6), /* control block */
    (void *) &main_screen_prompt_11_6_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_5_define =
{
    "prompt_11_5",
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
    {299, 284, 317, 312},                    /* widget size                    */
    &main_screen_prompt_11_6_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11_5), /* control block */
    (void *) &main_screen_prompt_11_5_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_4_define =
{
    "prompt_11_4",
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
    {282, 284, 298, 312},                    /* widget size                    */
    &main_screen_prompt_11_5_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11_4), /* control block */
    (void *) &main_screen_prompt_11_4_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_gear_position_define =
{
    "gear_position",
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
    {270, 278, 369, 312},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_prompt_11_4_define,         /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_gear_position), /* control block */
    (void *) &main_screen_gear_position_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_MPH_label_define =
{
    "MPH_label",
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
    {302, 250, 336, 267},                    /* widget size                    */
    &main_screen_gear_position_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_MPH_label), /* control block */
    (void *) &main_screen_MPH_label_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_MPH_val_define =
{
    "MPH_val",
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
    {282, 185, 357, 230},                    /* widget size                    */
    &main_screen_MPH_label_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_MPH_val), /* control block */
    (void *) &main_screen_MPH_val_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_second_dot_down_define =
{
    "second_dot_down",
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
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {474, 73, 478, 77},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_second_dot_down), /* control block */
    (void *) &main_screen_second_dot_down_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET main_screen_second_dot_up_define =
{
    "second_dot_up",
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
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {474, 44, 478, 48},                      /* widget size                    */
    &main_screen_second_dot_down_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_second_dot_up), /* control block */
    (void *) &main_screen_second_dot_up_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_temperature_define =
{
    "temperature",
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
    {565, 63, 600, 91},                      /* widget size                    */
    &main_screen_second_dot_up_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_temperature), /* control block */
    (void *) &main_screen_temperature_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_weather_icon_define =
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
    {562, 27, 602, 55},                      /* widget size                    */
    &main_screen_temperature_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_weather_icon), /* control block */
    (void *) &main_screen_weather_icon_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_time_minute_define =
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
    {482, 36, 557, 81},                      /* widget size                    */
    &main_screen_weather_icon_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_time_minute), /* control block */
    (void *) &main_screen_time_minute_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_time_hour_define =
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
    {396, 36, 471, 81},                      /* widget size                    */
    &main_screen_time_minute_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_time_hour), /* control block */
    (void *) &main_screen_time_hour_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_define =
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
    {51, 416, 104, 433},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt), /* control block  */
    (void *) &main_screen_prompt_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_define =
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
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {61, 390, 91, 408},                      /* widget size                    */
    &main_screen_prompt_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon), /* control block    */
    (void *) &main_screen_icon_properties    /* extended properties            */
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
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {146, 416, 203, 433},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_1), /* control block */
    (void *) &main_screen_prompt_1_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_1_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {158, 390, 193, 407},                    /* widget size                    */
    &main_screen_prompt_1_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_1), /* control block  */
    (void *) &main_screen_icon_1_properties  /* extended properties            */
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
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {247, 416, 295, 433},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_2), /* control block */
    (void *) &main_screen_prompt_2_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_2_define =
{
    "icon_2",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {261, 390, 281, 410},                    /* widget size                    */
    &main_screen_prompt_2_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_2), /* control block  */
    (void *) &main_screen_icon_2_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_3_define =
{
    "prompt_3",
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
    {347, 416, 390, 433},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_3), /* control block */
    (void *) &main_screen_prompt_3_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_3_define =
{
    "icon_3",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {359, 390, 377, 407},                    /* widget size                    */
    &main_screen_prompt_3_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_3), /* control block  */
    (void *) &main_screen_icon_3_properties  /* extended properties            */
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
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {445, 416, 486, 433},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_4), /* control block */
    (void *) &main_screen_prompt_4_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_4_define =
{
    "icon_4",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 391, 476, 409},                    /* widget size                    */
    &main_screen_prompt_4_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_4), /* control block  */
    (void *) &main_screen_icon_4_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_video_btn_define =
{
    "video_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_VIDEO_BTN,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {418, 376, 512, 465},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_icon_4_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_video_btn), /* control block */
    (void *) &main_screen_video_btn_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_audio_btn_define =
{
    "audio_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_AUDIO_BTN,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {321, 376, 415, 465},                    /* widget size                    */
    &main_screen_video_btn_define,           /* next widget definition         */
    &main_screen_icon_3_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_audio_btn), /* control block */
    (void *) &main_screen_audio_btn_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_phone_btn_define =
{
    "phone_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PHONE_BTN,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {224, 376, 318, 465},                    /* widget size                    */
    &main_screen_audio_btn_define,           /* next widget definition         */
    &main_screen_icon_2_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_phone_btn), /* control block */
    (void *) &main_screen_phone_btn_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_climate_btn_define =
{
    "climate_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CLIMATE_BTN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {127, 376, 221, 465},                    /* widget size                    */
    &main_screen_phone_btn_define,           /* next widget definition         */
    &main_screen_icon_1_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_climate_btn), /* control block */
    (void *) &main_screen_climate_btn_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_vehicle_btn_define =
{
    "vehicle_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_VEHICLE_BTN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {30, 376, 124, 465},                     /* widget size                    */
    &main_screen_climate_btn_define,         /* next widget definition         */
    &main_screen_icon_define,                /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_vehicle_btn), /* control block */
    (void *) &main_screen_vehicle_btn_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_battery_progress_bar_define =
{
    "battery_progress_bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {41, 316, 154, 349},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_battery_progress_bar), /* control block */
    (void *) &main_screen_battery_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_vehicle_mode_icon_define =
{
    "vehicle_mode_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {516, 288, 559, 331},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_vehicle_mode_icon), /* control block */
    (void *) &main_screen_vehicle_mode_icon_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_on_off_text_define =
{
    "on_off_text",
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
    {552, 416, 573, 433},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_on_off_text), /* control block */
    (void *) &main_screen_on_off_text_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_5_define =
{
    "icon_5",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {554, 391, 570, 409},                    /* widget size                    */
    &main_screen_on_off_text_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon_5), /* control block  */
    (void *) &main_screen_icon_5_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_on_off_btn_define =
{
    "on_off_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_ON_OFF_BTN,                           /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {515, 376, 609, 465},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_icon_5_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_on_off_btn), /* control block */
    (void *) &main_screen_on_off_btn_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET main_screen_radial_slider_define =
{
    "radial_slider",
    GX_TYPE_RADIAL_SLIDER,                   /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_SLIDER),                /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_radial_slider_create,          /* create function                */
    (VOID (*)(GX_WIDGET *)) radial_slider_draw, /* drawing function override   */
    GX_NULL,                                 /* event function override        */
    {179, 99, 460, 351},                     /* widget size                    */
    &main_screen_on_off_btn_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_radial_slider), /* control block */
    (void *) &main_screen_radial_slider_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET main_screen_vehicle_mode_button_define =
{
    "vehicle_mode_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_VEHICLE_MODE_BUTTON,                  /* widget id                      */
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
    {478, 250, 597, 369},                    /* widget size                    */
    &main_screen_radial_slider_define,       /* next widget definition         */
    &main_screen_vehicle_mode_icon_define,   /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_vehicle_mode_button), /* control block */
    (void *) &main_screen_vehicle_mode_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET main_screen_vehicle_mode_text_define =
{
    "vehicle_mode_text",
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
    {483, 233, 596, 261},                    /* widget size                    */
    &main_screen_vehicle_mode_button_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_vehicle_mode_text), /* control block */
    (void *) &main_screen_vehicle_mode_text_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_battery_icon_define =
{
    "battery_icon",
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
    {41, 316, 159, 349},                     /* widget size                    */
    &main_screen_vehicle_mode_text_define,   /* next widget definition         */
    &main_screen_battery_progress_bar_define, /* child widget definition       */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_battery_icon), /* control block */
    (void *) &main_screen_battery_icon_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_horizontal_line_define =
{
    "horizontal_line",
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
    {45, 269, 131, 270},                     /* widget size                    */
    &main_screen_battery_icon_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_horizontal_line), /* control block */
    (void *) &main_screen_horizontal_line_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_1_define =
{
    "prompt_11_1",
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
    {100, 233, 131, 261},                    /* widget size                    */
    &main_screen_horizontal_line_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11_1), /* control block */
    (void *) &main_screen_prompt_11_1_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_11_define =
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
    {81, 283, 107, 311},                     /* widget size                    */
    &main_screen_prompt_11_1_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_11), /* control block */
    (void *) &main_screen_prompt_11_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_remain_battery_define =
{
    "remain_battery",
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
    {47, 283, 82, 311},                      /* widget size                    */
    &main_screen_prompt_11_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_remain_battery), /* control block */
    (void *) &main_screen_remain_battery_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET main_screen_remain_miles_define =
{
    "remain_miles",
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
    {47, 233, 100, 261},                     /* widget size                    */
    &main_screen_remain_battery_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_remain_miles), /* control block */
    (void *) &main_screen_remain_miles_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_btn_window_define =
{
    "btn_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_BTN_WIN,                              /* widget id                      */
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
    {15, 375, 514, 466},                     /* widget size                    */
    &main_screen_remain_miles_define,        /* next widget definition         */
    &main_screen_vehicle_btn_define,         /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_btn_window), /* control block */
    (void *) &main_screen_btn_window_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET main_screen_weather_win_define =
{
    "weather_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_MAIN_WEATHER_WIN,                     /* widget id                      */
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
    {396, 22, 610, 91},                      /* widget size                    */
    &main_screen_btn_window_define,          /* next widget definition         */
    &main_screen_time_hour_define,           /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_weather_win), /* control block */
    (void *) &main_screen_weather_win_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_speedometer_define =
{
    "speedometer",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {197, 109, 442, 366},                    /* widget size                    */
    &main_screen_weather_win_define,         /* next widget definition         */
    &main_screen_MPH_val_define,             /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_speedometer), /* control block */
    (void *) &main_screen_speedometer_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_define =
{
    "main_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_MAIN_SCREEN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MAIN_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) main_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &main_screen_speedometer_define,         /* child widget                   */
    0,                                       /* control block                  */
    (void *) &main_screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES screen_base_properties =
{
    &base_define,                            /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_WINDOW_PROPERTIES screen_base_weather_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_NUMERIC_PROMPT_PROPERTIES screen_base_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES screen_base_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    32                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES screen_base_weather_icon_properties =
{
    GX_PIXELMAP_ID_WEATHER_ICON_PARTLY_CLOUDY_SMALL, /* normal pixelmap id     */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES screen_base_temperature_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    64                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES screen_base_second_dot_down_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES screen_base_second_dot_up_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_home_btn_properties =
{
    GX_PIXELMAP_ID_BTN_HOME,                 /* normal pixelmap id             */
    GX_PIXELMAP_ID_BTN_HOME_ACTIVE,          /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET screen_base_second_dot_up_define =
{
    "second_dot_up",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {451, 47, 455, 51},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_second_dot_up), /* control block */
    (void *) &screen_base_second_dot_up_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET screen_base_second_dot_down_define =
{
    "second_dot_down",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {451, 65, 455, 69},                      /* widget size                    */
    &screen_base_second_dot_up_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_second_dot_down), /* control block */
    (void *) &screen_base_second_dot_down_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET screen_base_temperature_define =
{
    "temperature",
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
    {512, 59, 529, 76},                      /* widget size                    */
    &screen_base_second_dot_down_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_temperature), /* control block */
    (void *) &screen_base_temperature_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET screen_base_weather_icon_define =
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
    {507, 38, 533, 56},                      /* widget size                    */
    &screen_base_temperature_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_weather_icon), /* control block */
    (void *) &screen_base_weather_icon_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET screen_base_minute_define =
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
    {454, 40, 503, 81},                      /* widget size                    */
    &screen_base_weather_icon_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_minute), /* control block  */
    (void *) &screen_base_minute_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_hour_define =
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
    {400, 40, 449, 81},                      /* widget size                    */
    &screen_base_minute_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_hour), /* control block    */
    (void *) &screen_base_hour_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_home_btn_define =
{
    "home_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_HOME_BTN,                             /* widget id                      */
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
    {544, 23, 608, 87},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_home_btn), /* control block */
    (void *) &screen_base_home_btn_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET screen_base_weather_win_define =
{
    "weather_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SCREEN_WEATHER_WIN,                   /* widget id                      */
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
    {400, 23, 544, 87},                      /* widget size                    */
    &screen_base_home_btn_define,            /* next widget definition         */
    &screen_base_hour_define,                /* child widget definition        */
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_weather_win), /* control block */
    (void *) &screen_base_weather_win_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET screen_base_define =
{
    "screen_base",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SCREEN_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) screen_base_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &screen_base_weather_win_define,         /* child widget                   */
    0,                                       /* control block                  */
    (void *) &screen_base_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES video_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_WINDOW_PROPERTIES video_screen_window_1_properties =
{
    GX_PIXELMAP_ID_VIDEO_BG_BOTTOM           /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES video_screen_video_play_progress_bar_bg_properties =
{
    GX_PIXELMAP_ID_VIDEO_PROGRESS_BG         /* wallpaper pixelmap id          */
};
GX_PROGRESS_BAR_INFO video_screen_video_play_progress_bar_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_VIDEO_PROGRESS_ACTIVE     /* fill pixelmap                  */
};
GX_PIXELMAP_BUTTON_PROPERTIES video_screen_play_btn_small_properties =
{
    GX_PIXELMAP_ID_VIDEO_PALY_SMALL,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_VIDEO_PAUSE_SMALL,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES video_screen_pixelmap_button_7_properties =
{
    GX_PIXELMAP_ID_VIDEO_ICON_RETURN,        /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES video_screen_pixelmap_button_8_properties =
{
    GX_PIXELMAP_ID_VIDEO_ICON_FILL_SCREEN,   /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES video_screen_pixelmap_button_9_properties =
{
    GX_PIXELMAP_ID_VIDEO_ICON_SOUND_OFF,     /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES video_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES video_screen_second_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_value_format,                       /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES video_screen_second_colon_properties =
{
    GX_STRING_ID_STRING_22,                  /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_WINDOW_PROPERTIES video_screen_play_btn_big_win_properties =
{
    GX_PIXELMAP_ID_VIDEO_BIG_ROUND_BUBBLE    /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES video_screen_play_btn_big_properties =
{
    GX_PIXELMAP_ID_VIDEO_PALY_BIG,           /* normal pixelmap id             */
    GX_PIXELMAP_ID_VIDEO_PALY_BIG_HOVER,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET video_screen_second_colon_define =
{
    "second_colon",
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
    {549, 417, 561, 458},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_second_colon), /* control block */
    (void *) &video_screen_second_colon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET video_screen_second_define =
{
    "second",
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
    {558, 420, 611, 465},                    /* widget size                    */
    &video_screen_second_colon_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_second), /* control block */
    (void *) &video_screen_second_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET video_screen_minute_define =
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
    {499, 420, 552, 465},                    /* widget size                    */
    &video_screen_second_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_minute), /* control block */
    (void *) &video_screen_minute_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET video_screen_pixelmap_button_9_define =
{
    "pixelmap_button_9",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) video_screen_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {167, 427, 195, 454},                    /* widget size                    */
    &video_screen_minute_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_pixelmap_button_9), /* control block */
    (void *) &video_screen_pixelmap_button_9_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET video_screen_pixelmap_button_8_define =
{
    "pixelmap_button_8",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) video_screen_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {101, 429, 129, 454},                    /* widget size                    */
    &video_screen_pixelmap_button_9_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_pixelmap_button_8), /* control block */
    (void *) &video_screen_pixelmap_button_8_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET video_screen_pixelmap_button_7_define =
{
    "pixelmap_button_7",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) video_screen_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {40, 429, 64, 455},                      /* widget size                    */
    &video_screen_pixelmap_button_8_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_pixelmap_button_7), /* control block */
    (void *) &video_screen_pixelmap_button_7_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET video_screen_play_btn_small_define =
{
    "play_btn_small",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PLAY_BTN_SMALL,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {300, 425, 339, 464},                    /* widget size                    */
    &video_screen_pixelmap_button_7_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_play_btn_small), /* control block */
    (void *) &video_screen_play_btn_small_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET video_screen_video_play_progress_bar_define =
{
    "video_play_progress_bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_GREEN,                       /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {39, 404, 602, 407},                     /* widget size                    */
    &video_screen_play_btn_small_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_video_play_progress_bar), /* control block */
    (void *) &video_screen_video_play_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET video_screen_video_play_progress_bar_bg_define =
{
    "video_play_progress_bar_bg",
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
    {39, 404, 604, 409},                     /* widget size                    */
    &video_screen_video_play_progress_bar_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_video_play_progress_bar_bg), /* control block */
    (void *) &video_screen_video_play_progress_bar_bg_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET video_screen_play_btn_big_define =
{
    "play_btn_big",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PLAY_BTN_BIG,                         /* widget id                      */
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
    {263, 155, 394, 297},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_play_btn_big), /* control block */
    (void *) &video_screen_play_btn_big_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET video_screen_play_btn_big_win_define =
{
    "play_btn_big_win",
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
    {232, 139, 406, 313},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &video_screen_play_btn_big_define,       /* child widget definition        */
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_play_btn_big_win), /* control block */
    (void *) &video_screen_play_btn_big_win_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET video_screen_window_1_define =
{
    "window_1",
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
    {0, 396, 639, 479},                      /* widget size                    */
    &video_screen_play_btn_big_win_define,   /* next widget definition         */
    &video_screen_video_play_progress_bar_bg_define, /* child widget definition */
    offsetof(VIDEO_SCREEN_CONTROL_BLOCK, video_screen_window_1), /* control block */
    (void *) &video_screen_window_1_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET video_screen_define =
{
    "video_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_VIDEO_SCREEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(VIDEO_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) video_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &video_screen_window_1_define,           /* child widget                   */
    0,                                       /* control block                  */
    (void *) &video_screen_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES climate_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_WINDOW_PROPERTIES climate_screen_right_progress_bar_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES climate_screen_right_progress_bar_bg_properties =
{
    GX_PIXELMAP_ID_RANGE_GREY_SINGLE_LINE    /* wallpaper pixelmap id          */
};
GX_PROGRESS_BAR_INFO climate_screen_right_progress_bar_properties =
{
    0,                                       /* mimimun value                  */
    33,                                      /* maximum value                  */
    29,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_RANGE_GREEN_SINGLE_LINE   /* fill pixelmap                  */
};
GX_NUMERIC_PROMPT_PROPERTIES climate_screen_right_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    89                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES climate_screen_prompt_properties =
{
    GX_STRING_ID_STRING_21,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_WINDOW_PROPERTIES climate_screen_left_progress_bar_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES climate_screen_left_progress_bar_bg_properties =
{
    GX_PIXELMAP_ID_RANGE_GREY_SINGLE_LINE    /* wallpaper pixelmap id          */
};
GX_PROGRESS_BAR_INFO climate_screen_left_progress_bar_properties =
{
    0,                                       /* mimimun value                  */
    33,                                      /* maximum value                  */
    23,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_RANGE_GREEN_SINGLE_LINE   /* fill pixelmap                  */
};
GX_NUMERIC_PROMPT_PROPERTIES climate_screen_left_progress_val_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    72                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES climate_screen_prompt_7_properties =
{
    GX_STRING_ID_STRING_21,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES climate_screen_left_lines_icon_properties =
{
    GX_PIXELMAP_ID_CLIMATE_LINES_LEFT,       /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_pixelmap_button_3_properties =
{
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_LEFT_BOTTOM, /* normal pixelmap id         */
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_LEFT_BOTTOM_ACTIVE, /* selected pixelmap id */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_pixelmap_button_properties =
{
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_LEFT,    /* normal pixelmap id             */
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_LEFT_ACTIVE, /* selected pixelmap id       */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_right_lines_properties =
{
    GX_PIXELMAP_ID_CLIMATE_LINES_RIGHT,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_pixelmap_button_1_properties =
{
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_RIGHT,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_RIGHT_ACTIVE, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_pixelmap_button_2_properties =
{
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_RIGHT_BOTTOM, /* normal pixelmap id        */
    GX_PIXELMAP_ID_CLIMATE_GRADIENT_RIGHT_BOTTOM_ACTIVE, /* selected pixelmap id */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_center_line_properties =
{
    GX_PIXELMAP_ID_CLIMATE_LINE_CENTER_DEVIDER, /* normal pixelmap id          */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_right_seat_properties =
{
    GX_PIXELMAP_ID_SEAT_RIGHT,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES climate_screen_left_fan_mode_properties =
{
    GX_STRING_ID_AUTO,                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES climate_screen_right_fan_mode_properties =
{
    GX_STRING_ID_LOW,                        /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_left_fan_button_properties =
{
    GX_PIXELMAP_ID_C_ICON_FAN,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_C_ICON_FAN_ACTIVE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_left_small_fan_button_properties =
{
    GX_PIXELMAP_ID_C_ICON_FAN_SMALL,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_C_ICON_FAN_SMALL_ACTIVE,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_right_small_fan_button_properties =
{
    GX_PIXELMAP_ID_C_ICON_FAN_SMALL,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_C_ICON_FAN_SMALL_ACTIVE,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES climate_screen_right_fan_button_properties =
{
    GX_PIXELMAP_ID_C_ICON_FAN,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_C_ICON_FAN_ACTIVE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_left_seat_properties =
{
    GX_PIXELMAP_ID_SEAT_LEFT,                /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_BUTTON_PROPERTIES climate_screen_left_warmer_icon_button_properties =
{
    0                                        /* pixelmap id                    */
};
GX_ICON_PROPERTIES climate_screen_icon_14_3_properties =
{
    GX_PIXELMAP_ID_SEAT_WARMER_LEFT_SINGLE,  /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_icon_14_12_properties =
{
    GX_PIXELMAP_ID_SEAT_WARMER_LEFT_SINGLE,  /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_icon_14_13_properties =
{
    GX_PIXELMAP_ID_SEAT_WARMER_LEFT_SINGLE,  /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_BUTTON_PROPERTIES climate_screen_right_warmer_icon_button_properties =
{
    0                                        /* pixelmap id                    */
};
GX_ICON_PROPERTIES climate_screen_icon_14_16_1_properties =
{
    GX_PIXELMAP_ID_SEAT_WARMER_RIGHT_SINGLE_ACTIVE, /* normal pixelmap id      */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_icon_14_14_1_properties =
{
    GX_PIXELMAP_ID_SEAT_WARMER_RIGHT_SINGLE_ACTIVE, /* normal pixelmap id      */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES climate_screen_icon_14_15_1_properties =
{
    GX_PIXELMAP_ID_SEAT_WARMER_RIGHT_SINGLE_ACTIVE, /* normal pixelmap id      */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_progress_bar_define =
{
    "right_progress_bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_RIGHT_PROGRESS_BAR,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_PROGRESS_VERTICAL,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) custom_progress_bar_event_process, /* event function override */
    {557, 171, 594, 302},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_progress_bar), /* control block */
    (void *) &climate_screen_right_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_prompt_define =
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
    {592, 131, 598, 143},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_prompt), /* control block */
    (void *) &climate_screen_prompt_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_progress_val_define =
{
    "right_progress_val",
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
    {557, 137, 592, 165},                    /* widget size                    */
    &climate_screen_prompt_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_progress_val), /* control block */
    (void *) &climate_screen_right_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_progress_bar_bg_define =
{
    "right_progress_bar_bg",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {557, 171, 594, 302},                    /* widget size                    */
    &climate_screen_right_progress_val_define, /* next widget definition       */
    &climate_screen_right_progress_bar_define, /* child widget definition      */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_progress_bar_bg), /* control block */
    (void *) &climate_screen_right_progress_bar_bg_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_progress_bar_define =
{
    "left_progress_bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_LEFT_PROGRESS_BAR,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_PROGRESS_VERTICAL,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) custom_progress_bar_event_process, /* event function override */
    {42, 171, 79, 302},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_progress_bar), /* control block */
    (void *) &climate_screen_left_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_prompt_7_define =
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
    {78, 131, 84, 143},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_prompt_7), /* control block */
    (void *) &climate_screen_prompt_7_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_progress_val_define =
{
    "left_progress_val",
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
    {43, 137, 78, 165},                      /* widget size                    */
    &climate_screen_prompt_7_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_progress_val), /* control block */
    (void *) &climate_screen_left_progress_val_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_progress_bar_bg_define =
{
    "left_progress_bar_bg",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 171, 79, 302},                      /* widget size                    */
    &climate_screen_left_progress_val_define, /* next widget definition        */
    &climate_screen_left_progress_bar_define, /* child widget definition       */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_progress_bar_bg), /* control block */
    (void *) &climate_screen_left_progress_bar_bg_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_pixelmap_button_define =
{
    "pixelmap_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    {188, 130, 243, 221},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_pixelmap_button), /* control block */
    (void *) &climate_screen_pixelmap_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_pixelmap_button_3_define =
{
    "pixelmap_button_3",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    {188, 223, 243, 314},                    /* widget size                    */
    &climate_screen_pixelmap_button_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_pixelmap_button_3), /* control block */
    (void *) &climate_screen_pixelmap_button_3_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_pixelmap_button_2_define =
{
    "pixelmap_button_2",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    {393, 223, 448, 314},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_pixelmap_button_2), /* control block */
    (void *) &climate_screen_pixelmap_button_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_pixelmap_button_1_define =
{
    "pixelmap_button_1",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    {393, 130, 448, 221},                    /* widget size                    */
    &climate_screen_pixelmap_button_2_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_pixelmap_button_1), /* control block */
    (void *) &climate_screen_pixelmap_button_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_icon_14_13_define =
{
    "icon_14_13",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {132, 261, 158, 313},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_icon_14_13), /* control block */
    (void *) &climate_screen_icon_14_13_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_icon_14_12_define =
{
    "icon_14_12",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {145, 261, 171, 313},                    /* widget size                    */
    &climate_screen_icon_14_13_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_icon_14_12), /* control block */
    (void *) &climate_screen_icon_14_12_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_icon_14_3_define =
{
    "icon_14_3",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {158, 261, 184, 313},                    /* widget size                    */
    &climate_screen_icon_14_12_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_icon_14_3), /* control block */
    (void *) &climate_screen_icon_14_3_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_icon_14_15_1_define =
{
    "icon_14_15_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {481, 261, 507, 313},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_icon_14_15_1), /* control block */
    (void *) &climate_screen_icon_14_15_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_icon_14_14_1_define =
{
    "icon_14_14_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {468, 261, 494, 313},                    /* widget size                    */
    &climate_screen_icon_14_15_1_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_icon_14_14_1), /* control block */
    (void *) &climate_screen_icon_14_14_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_icon_14_16_1_define =
{
    "icon_14_16_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 261, 481, 313},                    /* widget size                    */
    &climate_screen_icon_14_14_1_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_icon_14_16_1), /* control block */
    (void *) &climate_screen_icon_14_16_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_warmer_icon_button_define =
{
    "right_warmer_icon_button",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {459, 264, 503, 309},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &climate_screen_icon_14_16_1_define,     /* child widget definition        */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_warmer_icon_button), /* control block */
    (void *) &climate_screen_right_warmer_icon_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_warmer_icon_button_define =
{
    "left_warmer_icon_button",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 265, 184, 309},                    /* widget size                    */
    &climate_screen_right_warmer_icon_button_define, /* next widget definition */
    &climate_screen_icon_14_3_define,        /* child widget definition        */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_warmer_icon_button), /* control block */
    (void *) &climate_screen_left_warmer_icon_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_seat_define =
{
    "left_seat",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {94, 137, 213, 298},                     /* widget size                    */
    &climate_screen_left_warmer_icon_button_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_seat), /* control block */
    (void *) &climate_screen_left_seat_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_fan_button_define =
{
    "right_fan_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_RIGHT_FAN_BUTTON,                     /* widget id                      */
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
    {339, 166, 370, 199},                    /* widget size                    */
    &climate_screen_left_seat_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_fan_button), /* control block */
    (void *) &climate_screen_right_fan_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_small_fan_button_define =
{
    "right_small_fan_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_RIGHT_SMALL_FAN_BUTTON,               /* widget id                      */
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
    {345, 245, 364, 265},                    /* widget size                    */
    &climate_screen_right_fan_button_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_small_fan_button), /* control block */
    (void *) &climate_screen_right_small_fan_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_small_fan_button_define =
{
    "left_small_fan_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_LEFT_SMALL_FAN_BUTTON,                /* widget id                      */
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
    {273, 245, 292, 265},                    /* widget size                    */
    &climate_screen_right_small_fan_button_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_small_fan_button), /* control block */
    (void *) &climate_screen_left_small_fan_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_fan_button_define =
{
    "left_fan_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_LEFT_FAN_BUTTON,                      /* widget id                      */
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
    {267, 166, 298, 199},                    /* widget size                    */
    &climate_screen_left_small_fan_button_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_fan_button), /* control block */
    (void *) &climate_screen_left_fan_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_fan_mode_define =
{
    "right_fan_mode",
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
    {330, 212, 386, 232},                    /* widget size                    */
    &climate_screen_left_fan_button_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_fan_mode), /* control block */
    (void *) &climate_screen_right_fan_mode_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_fan_mode_define =
{
    "left_fan_mode",
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
    {254, 212, 310, 232},                    /* widget size                    */
    &climate_screen_right_fan_mode_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_fan_mode), /* control block */
    (void *) &climate_screen_left_fan_mode_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_seat_define =
{
    "right_seat",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {430, 137, 549, 298},                    /* widget size                    */
    &climate_screen_left_fan_mode_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_seat), /* control block */
    (void *) &climate_screen_right_seat_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_center_line_define =
{
    "center_line",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {318, 156, 319, 289},                    /* widget size                    */
    &climate_screen_right_seat_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_center_line), /* control block */
    (void *) &climate_screen_center_line_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_lines_define =
{
    "right_lines",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {393, 127, 449, 317},                    /* widget size                    */
    &climate_screen_center_line_define,      /* next widget definition         */
    &climate_screen_pixelmap_button_1_define, /* child widget definition       */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_lines), /* control block */
    (void *) &climate_screen_right_lines_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_lines_icon_define =
{
    "left_lines_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {188, 127, 244, 317},                    /* widget size                    */
    &climate_screen_right_lines_define,      /* next widget definition         */
    &climate_screen_pixelmap_button_3_define, /* child widget definition       */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_lines_icon), /* control block */
    (void *) &climate_screen_left_lines_icon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_left_progress_bar_win_define =
{
    "left_progress_bar_win",
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
    {35, 126, 88, 309},                      /* widget size                    */
    &climate_screen_left_lines_icon_define,  /* next widget definition         */
    &climate_screen_left_progress_bar_bg_define, /* child widget definition    */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_left_progress_bar_win), /* control block */
    (void *) &climate_screen_left_progress_bar_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET climate_screen_right_progress_bar_win_define =
{
    "right_progress_bar_win",
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
    {553, 126, 601, 309},                    /* widget size                    */
    &climate_screen_left_progress_bar_win_define, /* next widget definition    */
    &climate_screen_right_progress_bar_bg_define, /* child widget definition   */
    offsetof(CLIMATE_SCREEN_CONTROL_BLOCK, climate_screen_right_progress_bar_win), /* control block */
    (void *) &climate_screen_right_progress_bar_win_properties /* extended properties */
};

GX_ANIMATION_INFO climate_screen_animation_1 = {
    (GX_WIDGET *) &climate_screen.climate_screen_left_progress_bar_win,
    (GX_WIDGET *) &climate_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ID_ANI_SLIDE_IN_PROGRESS_BAR_WIN, 0, 1,
    {0, 126}, {35, 126}, 255, 255, 20
};


GX_ANIMATION_INFO climate_screen_animation_2 = {
    (GX_WIDGET *) &climate_screen.climate_screen_right_progress_bar_win,
    (GX_WIDGET *) &climate_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {586, 126}, {553, 126}, 255, 255, 20
};


GX_ANIMATION_INFO climate_screen_animation_3 = {
    (GX_WIDGET *) &climate_screen.climate_screen_left_warmer_icon_button,
    (GX_WIDGET *) &climate_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {134, 310}, {134, 265}, 255, 255, 10
};


GX_ANIMATION_INFO climate_screen_animation_4 = {
    (GX_WIDGET *) &climate_screen.climate_screen_right_warmer_icon_button,
    (GX_WIDGET *) &climate_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {459, 310}, {459, 264}, 255, 255, 10
};


GX_STUDIO_ACTION climate_screen_on_gx_event_showactions[5] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &climate_screen, &climate_screen.climate_screen_left_progress_bar_win, &climate_screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &climate_screen, &climate_screen.climate_screen_right_progress_bar_win, &climate_screen_animation_2},
    {GX_ACTION_TYPE_DETACH, 0, &climate_screen, &climate_screen.climate_screen_left_warmer_icon_button, GX_NULL},
    {GX_ACTION_TYPE_DETACH, 0, &climate_screen, &climate_screen.climate_screen_right_warmer_icon_button, GX_NULL},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION climate_screen_on_gx_event_animation_complete_id_ani_slide_in_progress_bar_win_actions[3] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &climate_screen, &climate_screen.climate_screen_left_warmer_icon_button, &climate_screen_animation_3},
    {GX_ACTION_TYPE_ANIMATION, 0, &climate_screen, &climate_screen.climate_screen_right_warmer_icon_button, &climate_screen_animation_4},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_climate_screen_event_table[] = {
    { GX_EVENT_SHOW, 0, climate_screen_on_gx_event_showactions},
    { GX_EVENT_ANIMATION_COMPLETE, ID_ANI_SLIDE_IN_PROGRESS_BAR_WIN, climate_screen_on_gx_event_animation_complete_id_ani_slide_in_progress_bar_win_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS climate_screen_event_chain = {gx_studio_climate_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))climate_screen_event_process};
static UINT gx_studio_climate_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &climate_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET climate_screen_define =
{
    "climate_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_CLIMATE_SCREEN,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CLIMATE_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_climate_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &climate_screen_right_progress_bar_win_define, /* child widget             */
    0,                                       /* control block                  */
    (void *) &climate_screen_properties      /* extended properties            */
};
GX_TEMPLATE_PROPERTIES vehicle_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PIXELMAP_BUTTON_PROPERTIES vehicle_screen_mode_eco_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_V_MODE_BTN_ACTIVE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_6_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN_ICON_ECO,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_6_properties =
{
    GX_STRING_ID_ECO,                        /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES vehicle_screen_mode_comfort_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_V_MODE_BTN_ACTIVE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN_ICON_COMFORT,  /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_properties =
{
    GX_STRING_ID_COMFORT,                    /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES vehicle_screen_mode_sport_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_V_MODE_BTN_ACTIVE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN_ICON_SPORT,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_1_properties =
{
    GX_STRING_ID_SPORT,                      /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_100,                 /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_3_properties =
{
    GX_STRING_ID_STRING_101,                 /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES vehicle_screen_car_icon_properties =
{
    GX_PIXELMAP_ID_V_CAR_WITH_CHECK_MARK,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_SPRITE_FRAME vehicle_screen_light_sprite_left_frame_list[11] =
{
    {
        GX_PIXELMAP_ID_LLIGHT01,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        0,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT02,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT03,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT04,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT05,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT06,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT07,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT08,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT09,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT10,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        0,                                   /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    }
};

GX_SPRITE_PROPERTIES vehicle_screen_light_sprite_left_properties =
{
    vehicle_screen_light_sprite_left_frame_list,     /* address of frame list  */
    11,                                      /* frame count                    */
};
GX_SPRITE_FRAME vehicle_screen_light_sprite_right_frame_list[11] =
{
    {
        GX_PIXELMAP_ID_LLIGHT01,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        0,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT02,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT03,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT04,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT05,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT06,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT07,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT08,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT09,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        GX_PIXELMAP_ID_LLIGHT10,             /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    },
    {
        0,                                   /* pixelmap id                    */
        0,                                   /* x offset                       */
        0,                                   /* y offset                       */
        5,                                   /* frame delay                    */
        GX_SPRITE_BACKGROUND_NO_ACTION,      /* background operation           */
        255                                  /* alpha value                    */
    }
};

GX_SPRITE_PROPERTIES vehicle_screen_light_sprite_right_properties =
{
    vehicle_screen_light_sprite_right_frame_list,     /* address of frame list */
    11,                                      /* frame count                    */
};
GX_ICON_PROPERTIES vehicle_screen_icon_8_properties =
{
    GX_PIXELMAP_ID_V_VERTICAL_LINE,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_9_properties =
{
    GX_PIXELMAP_ID_V_ICON_BATTERY,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_2_properties =
{
    GX_PIXELMAP_ID_V_ICON_WINDSHILED,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_3_properties =
{
    GX_PIXELMAP_ID_V_ICON_LIGHTS,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_4_properties =
{
    GX_PIXELMAP_ID_V_ICON_AIR_FILTER,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_5_properties =
{
    GX_PIXELMAP_ID_V_ICON_BRAKE,             /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_10_properties =
{
    GX_PIXELMAP_ID_V_ICON_TIRE,              /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_battery_usage_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    86                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_miles_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    382                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_windshield_washer_level_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    54                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_lights_bulbs_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    77                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_air_filter_usage_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    73                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_front_left_brake_pad_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    61                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_front_right_brake_pad_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    65                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_rear_left_brake_pad_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    71                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_rear_right_brake_pad_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_front_left_tire_pressure_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    35                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_front_right_tire_pressure_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    35                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_rear_left_tire_pressure_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    38                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vehicle_screen_rear_right_tire_pressure_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN,                 /* disabled text color            */
    GX_NULL,                                 /* format function                */
    39                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_properties =
{
    GX_STRING_ID_STRING_30,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_1_properties =
{
    GX_STRING_ID_STRING_103,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_2_properties =
{
    GX_STRING_ID_STRING_104,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_3_properties =
{
    GX_STRING_ID_STRING_105,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_4_properties =
{
    GX_STRING_ID_STRING_106,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_5_properties =
{
    GX_STRING_ID_STRING_107,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_6_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_7_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_8_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_9_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_10_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_11_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_11_1_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_11_2_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vehicle_screen_prompt_13_11_3_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GREEN,                 /* normal text color              */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected text color            */
    GX_COLOR_ID_LIGHT_GREEN                  /* disabled text color            */
};
GX_ICON_PROPERTIES vehicle_screen_icon_11_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES vehicle_screen_icon_11_1_properties =
{
    GX_PIXELMAP_ID_DOT,                      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_6_define =
{
    "prompt_6",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {113, 163, 139, 180},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_6), /* control block */
    (void *) &vehicle_screen_prompt_6_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_6_define =
{
    "icon_6",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {113, 120, 139, 146},                    /* widget size                    */
    &vehicle_screen_prompt_6_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_6), /* control block */
    (void *) &vehicle_screen_icon_6_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_define =
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
    {161, 163, 222, 180},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt), /* control block */
    (void *) &vehicle_screen_prompt_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {178, 120, 204, 146},                    /* widget size                    */
    &vehicle_screen_prompt_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon), /* control block */
    (void *) &vehicle_screen_icon_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_1_define =
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
    {235, 163, 276, 180},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_1), /* control block */
    (void *) &vehicle_screen_prompt_1_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_1_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {242, 120, 268, 146},                    /* widget size                    */
    &vehicle_screen_prompt_1_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_1), /* control block */
    (void *) &vehicle_screen_icon_1_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_light_sprite_right_define =
{
    "light_sprite_right",
    GX_TYPE_SPRITE,                          /* widget type                    */
    ID_LIGHT_SPRITE_RIGHT,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_SPRITE_AUTO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_SPRITE),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_sprite_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {209, 275, 246, 312},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_light_sprite_right), /* control block */
    (void *) &vehicle_screen_light_sprite_right_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_light_sprite_left_define =
{
    "light_sprite_left",
    GX_TYPE_SPRITE,                          /* widget type                    */
    ID_LIGHT_SPRITE_LEFT,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_SPRITE_AUTO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_SPRITE),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_sprite_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {81, 275, 118, 312},                     /* widget size                    */
    &vehicle_screen_light_sprite_right_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_light_sprite_left), /* control block */
    (void *) &vehicle_screen_light_sprite_left_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_11_1_define =
{
    "icon_11_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_GRAY,                        /* selected color id              */
    GX_COLOR_ID_GRAY,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {431, 319, 435, 323},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_11_1), /* control block */
    (void *) &vehicle_screen_icon_11_1_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_11_define =
{
    "icon_11",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_GRAY,                        /* selected color id              */
    GX_COLOR_ID_GRAY,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {431, 282, 435, 286},                    /* widget size                    */
    &vehicle_screen_icon_11_1_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_11), /* control block */
    (void *) &vehicle_screen_icon_11_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_11_3_define =
{
    "prompt_13_11_3",
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
    {460, 314, 465, 331},                    /* widget size                    */
    &vehicle_screen_icon_11_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_11_3), /* control block */
    (void *) &vehicle_screen_prompt_13_11_3_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_11_2_define =
{
    "prompt_13_11_2",
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
    {398, 314, 403, 331},                    /* widget size                    */
    &vehicle_screen_prompt_13_11_3_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_11_2), /* control block */
    (void *) &vehicle_screen_prompt_13_11_2_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_11_1_define =
{
    "prompt_13_11_1",
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
    {460, 276, 465, 293},                    /* widget size                    */
    &vehicle_screen_prompt_13_11_2_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_11_1), /* control block */
    (void *) &vehicle_screen_prompt_13_11_1_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_11_define =
{
    "prompt_13_11",
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
    {398, 276, 403, 293},                    /* widget size                    */
    &vehicle_screen_prompt_13_11_1_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_11), /* control block */
    (void *) &vehicle_screen_prompt_13_11_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_10_define =
{
    "prompt_13_10",
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
    {486, 276, 500, 293},                    /* widget size                    */
    &vehicle_screen_prompt_13_11_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_10), /* control block */
    (void *) &vehicle_screen_prompt_13_10_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_9_define =
{
    "prompt_13_9",
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
    {399, 238, 413, 255},                    /* widget size                    */
    &vehicle_screen_prompt_13_10_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_9), /* control block */
    (void *) &vehicle_screen_prompt_13_9_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_8_define =
{
    "prompt_13_8",
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
    {399, 200, 413, 217},                    /* widget size                    */
    &vehicle_screen_prompt_13_9_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_8), /* control block */
    (void *) &vehicle_screen_prompt_13_8_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_7_define =
{
    "prompt_13_7",
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
    {399, 163, 413, 180},                    /* widget size                    */
    &vehicle_screen_prompt_13_8_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_7), /* control block */
    (void *) &vehicle_screen_prompt_13_7_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_6_define =
{
    "prompt_13_6",
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
    {399, 125, 413, 142},                    /* widget size                    */
    &vehicle_screen_prompt_13_7_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_6), /* control block */
    (void *) &vehicle_screen_prompt_13_6_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_5_define =
{
    "prompt_13_5",
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
    {472, 127, 500, 139},                    /* widget size                    */
    &vehicle_screen_prompt_13_6_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_5), /* control block */
    (void *) &vehicle_screen_prompt_13_5_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_4_define =
{
    "prompt_13_4",
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
    {429, 165, 578, 177},                    /* widget size                    */
    &vehicle_screen_prompt_13_5_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_4), /* control block */
    (void *) &vehicle_screen_prompt_13_4_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_3_define =
{
    "prompt_13_3",
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
    {429, 202, 502, 214},                    /* widget size                    */
    &vehicle_screen_prompt_13_4_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_3), /* control block */
    (void *) &vehicle_screen_prompt_13_3_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_2_define =
{
    "prompt_13_2",
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
    {429, 240, 519, 252},                    /* widget size                    */
    &vehicle_screen_prompt_13_3_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_2), /* control block */
    (void *) &vehicle_screen_prompt_13_2_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_1_define =
{
    "prompt_13_1",
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
    {509, 278, 574, 290},                    /* widget size                    */
    &vehicle_screen_prompt_13_2_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13_1), /* control block */
    (void *) &vehicle_screen_prompt_13_1_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_13_define =
{
    "prompt_13",
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
    {492, 317, 574, 329},                    /* widget size                    */
    &vehicle_screen_prompt_13_1_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_13), /* control block */
    (void *) &vehicle_screen_prompt_13_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_rear_right_tire_pressure_define =
{
    "rear_right_tire_pressure",
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
    {467, 314, 484, 331},                    /* widget size                    */
    &vehicle_screen_prompt_13_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_rear_right_tire_pressure), /* control block */
    (void *) &vehicle_screen_rear_right_tire_pressure_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_rear_left_tire_pressure_define =
{
    "rear_left_tire_pressure",
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
    {441, 314, 458, 331},                    /* widget size                    */
    &vehicle_screen_rear_right_tire_pressure_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_rear_left_tire_pressure), /* control block */
    (void *) &vehicle_screen_rear_left_tire_pressure_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_front_right_tire_pressure_define =
{
    "front_right_tire_pressure",
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
    {405, 314, 422, 331},                    /* widget size                    */
    &vehicle_screen_rear_left_tire_pressure_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_front_right_tire_pressure), /* control block */
    (void *) &vehicle_screen_front_right_tire_pressure_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_front_left_tire_pressure_define =
{
    "front_left_tire_pressure",
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
    {379, 314, 396, 331},                    /* widget size                    */
    &vehicle_screen_front_right_tire_pressure_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_front_left_tire_pressure), /* control block */
    (void *) &vehicle_screen_front_left_tire_pressure_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_rear_right_brake_pad_define =
{
    "rear_right_brake_pad",
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
    {467, 276, 484, 293},                    /* widget size                    */
    &vehicle_screen_front_left_tire_pressure_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_rear_right_brake_pad), /* control block */
    (void *) &vehicle_screen_rear_right_brake_pad_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_rear_left_brake_pad_define =
{
    "rear_left_brake_pad",
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
    {441, 276, 458, 293},                    /* widget size                    */
    &vehicle_screen_rear_right_brake_pad_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_rear_left_brake_pad), /* control block */
    (void *) &vehicle_screen_rear_left_brake_pad_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_front_right_brake_pad_define =
{
    "front_right_brake_pad",
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
    {405, 276, 422, 293},                    /* widget size                    */
    &vehicle_screen_rear_left_brake_pad_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_front_right_brake_pad), /* control block */
    (void *) &vehicle_screen_front_right_brake_pad_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_front_left_brake_pad_define =
{
    "front_left_brake_pad",
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
    {379, 276, 396, 293},                    /* widget size                    */
    &vehicle_screen_front_right_brake_pad_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_front_left_brake_pad), /* control block */
    (void *) &vehicle_screen_front_left_brake_pad_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_air_filter_usage_define =
{
    "air_filter_usage",
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
    {379, 238, 396, 255},                    /* widget size                    */
    &vehicle_screen_front_left_brake_pad_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_air_filter_usage), /* control block */
    (void *) &vehicle_screen_air_filter_usage_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_lights_bulbs_define =
{
    "lights_bulbs",
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
    {379, 200, 396, 217},                    /* widget size                    */
    &vehicle_screen_air_filter_usage_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_lights_bulbs), /* control block */
    (void *) &vehicle_screen_lights_bulbs_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_windshield_washer_level_define =
{
    "windshield_washer_level",
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
    {379, 163, 396, 180},                    /* widget size                    */
    &vehicle_screen_lights_bulbs_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_windshield_washer_level), /* control block */
    (void *) &vehicle_screen_windshield_washer_level_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_miles_define =
{
    "miles",
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
    {438, 125, 464, 142},                    /* widget size                    */
    &vehicle_screen_windshield_washer_level_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_miles), /* control block */
    (void *) &vehicle_screen_miles_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_battery_usage_define =
{
    "battery_usage",
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
    {379, 125, 396, 142},                    /* widget size                    */
    &vehicle_screen_miles_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_battery_usage), /* control block */
    (void *) &vehicle_screen_battery_usage_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_10_define =
{
    "icon_10",
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
    {343, 306, 368, 331},                    /* widget size                    */
    &vehicle_screen_battery_usage_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_10), /* control block */
    (void *) &vehicle_screen_icon_10_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_5_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {343, 268, 368, 293},                    /* widget size                    */
    &vehicle_screen_icon_10_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_5), /* control block */
    (void *) &vehicle_screen_icon_5_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_4_define =
{
    "icon_4",
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
    {343, 235, 368, 251},                    /* widget size                    */
    &vehicle_screen_icon_5_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_4), /* control block */
    (void *) &vehicle_screen_icon_4_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_3_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {343, 197, 368, 220},                    /* widget size                    */
    &vehicle_screen_icon_4_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_3), /* control block */
    (void *) &vehicle_screen_icon_3_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_2_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {343, 158, 368, 180},                    /* widget size                    */
    &vehicle_screen_icon_3_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_2), /* control block */
    (void *) &vehicle_screen_icon_2_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_9_define =
{
    "icon_9",
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
    {343, 123, 368, 142},                    /* widget size                    */
    &vehicle_screen_icon_2_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_9), /* control block */
    (void *) &vehicle_screen_icon_9_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_icon_8_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {319, 123, 320, 331},                    /* widget size                    */
    &vehicle_screen_icon_9_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_icon_8), /* control block */
    (void *) &vehicle_screen_icon_8_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_car_icon_define =
{
    "car_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {39, 216, 280, 331},                     /* widget size                    */
    &vehicle_screen_icon_8_define,           /* next widget definition         */
    &vehicle_screen_light_sprite_left_define, /* child widget definition       */
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_car_icon), /* control block */
    (void *) &vehicle_screen_car_icon_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_3_define =
{
    "prompt_3",
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
    {39, 133, 80, 150},                      /* widget size                    */
    &vehicle_screen_car_icon_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_3), /* control block */
    (void *) &vehicle_screen_prompt_3_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_prompt_2_define =
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
    {39, 116, 90, 133},                      /* widget size                    */
    &vehicle_screen_prompt_3_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_prompt_2), /* control block */
    (void *) &vehicle_screen_prompt_2_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_mode_sport_btn_define =
{
    "mode_sport_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_MODE_SPORT_BTN,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {219, 97, 292, 180},                     /* widget size                    */
    &vehicle_screen_prompt_2_define,         /* next widget definition         */
    &vehicle_screen_icon_1_define,           /* child widget definition        */
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_mode_sport_btn), /* control block */
    (void *) &vehicle_screen_mode_sport_btn_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_mode_comfort_btn_define =
{
    "mode_comfort_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_MODE_COMFORT_BTN,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {155, 97, 228, 180},                     /* widget size                    */
    &vehicle_screen_mode_sport_btn_define,   /* next widget definition         */
    &vehicle_screen_icon_define,             /* child widget definition        */
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_mode_comfort_btn), /* control block */
    (void *) &vehicle_screen_mode_comfort_btn_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vehicle_screen_mode_eco_btn_define =
{
    "mode_eco_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_MODE_ECO_BTN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {90, 97, 163, 180},                      /* widget size                    */
    &vehicle_screen_mode_comfort_btn_define, /* next widget definition         */
    &vehicle_screen_icon_6_define,           /* child widget definition        */
    offsetof(VEHICLE_SCREEN_CONTROL_BLOCK, vehicle_screen_mode_eco_btn), /* control block */
    (void *) &vehicle_screen_mode_eco_btn_properties /* extended properties    */
};

GX_ANIMATION_INFO vehicle_screen_animation_1 = {
    (GX_WIDGET *) &vehicle_screen.vehicle_screen_car_icon,
    (GX_WIDGET *) &vehicle_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {300, 216}, {39, 216}, 0, 255, 20
};


GX_STUDIO_ACTION vehicle_screen_id_mode_eco_btnid_btn_ecoon_gx_event_showactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &vehicle_screen, &vehicle_screen.vehicle_screen_car_icon, &vehicle_screen_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_vehicle_screen_event_table[] = {
    { GX_EVENT_SHOW, 0, vehicle_screen_id_mode_eco_btnid_btn_ecoon_gx_event_showactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS vehicle_screen_event_chain = {gx_studio_vehicle_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))vehicle_screen_event_process};
static UINT gx_studio_vehicle_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &vehicle_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET vehicle_screen_define =
{
    "vehicle_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_VEHICLE_SCREEN,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(VEHICLE_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_vehicle_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &vehicle_screen_mode_eco_btn_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &vehicle_screen_properties      /* extended properties            */
};
GX_TEMPLATE_PROPERTIES volume_screen_base_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_WINDOW_PROPERTIES volume_screen_base_volume_progress_bar_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES volume_screen_base_volume_progress_bar_bg_properties =
{
    GX_PIXELMAP_ID_RANGE_GREY_SINGLE_LINE    /* wallpaper pixelmap id          */
};
GX_PROGRESS_BAR_INFO volume_screen_base_volume_progress_bar_properties =
{
    0,                                       /* mimimun value                  */
    47,                                      /* maximum value                  */
    23,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_RANGE_GREEN_SINGLE_LINE   /* fill pixelmap                  */
};
GX_NUMERIC_PROMPT_PROPERTIES volume_screen_base_volume_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    48                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES volume_screen_base_volume_label_properties =
{
    GX_STRING_ID_STRING_113,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES volume_screen_base_mute_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES volume_screen_base_prompt_14_properties =
{
    GX_STRING_ID_STRING_110,                 /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES volume_screen_base_bute_btn_icon_1_properties =
{
    GX_PIXELMAP_ID_ICON_SOUND_ON,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_SOUND_OFF            /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_volume_progress_bar_define =
{
    "volume_progress_bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_VOLUME_PROGRESS_BAR,                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_PROGRESS_VERTICAL,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) volume_progress_bar_event_process, /* event function override */
    {558, 143, 595, 330},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_volume_progress_bar), /* control block */
    (void *) &volume_screen_base_volume_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_volume_label_define =
{
    "volume_label",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {554, 339, 598, 351},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_volume_label), /* control block */
    (void *) &volume_screen_base_volume_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_volume_value_define =
{
    "volume_value",
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
    {550, 112, 603, 140},                    /* widget size                    */
    &volume_screen_base_volume_label_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_volume_value), /* control block */
    (void *) &volume_screen_base_volume_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_volume_progress_bar_bg_define =
{
    "volume_progress_bar_bg",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {558, 143, 595, 330},                    /* widget size                    */
    &volume_screen_base_volume_value_define, /* next widget definition         */
    &volume_screen_base_volume_progress_bar_define, /* child widget definition */
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_volume_progress_bar_bg), /* control block */
    (void *) &volume_screen_base_volume_progress_bar_bg_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_bute_btn_icon_1_define =
{
    "bute_btn_icon_1",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_RED,                         /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {65, 211, 86, 232},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_bute_btn_icon_1), /* control block */
    (void *) &volume_screen_base_bute_btn_icon_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_prompt_14_define =
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
    {57, 250, 94, 267},                      /* widget size                    */
    &volume_screen_base_bute_btn_icon_1_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_prompt_14), /* control block */
    (void *) &volume_screen_base_prompt_14_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_mute_btn_define =
{
    "mute_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BTN_MUTE,                             /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {39, 184, 112, 272},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &volume_screen_base_prompt_14_define,    /* child widget definition        */
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_mute_btn), /* control block */
    (void *) &volume_screen_base_mute_btn_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_volume_progress_bar_win_define =
{
    "volume_progress_bar_win",
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
    {550, 108, 603, 356},                    /* widget size                    */
    &volume_screen_base_mute_btn_define,     /* next widget definition         */
    &volume_screen_base_volume_progress_bar_bg_define, /* child widget definition */
    offsetof(VOLUME_SCREEN_BASE_CONTROL_BLOCK, volume_screen_base_volume_progress_bar_win), /* control block */
    (void *) &volume_screen_base_volume_progress_bar_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET volume_screen_base_define =
{
    "volume_screen_base",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(VOLUME_SCREEN_BASE_CONTROL_BLOCK), /* control block size            */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) volume_screen_base_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &volume_screen_base_volume_progress_bar_win_define, /* child widget        */
    0,                                       /* control block                  */
    (void *) &volume_screen_base_properties  /* extended properties            */
};
GX_TEMPLATE_PROPERTIES audio_screen_properties =
{
    &volume_screen_base_define,              /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_WINDOW_PROPERTIES audio_screen_music_info_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES audio_screen_music_name_properties =
{
    GX_STRING_ID_STRING_115,                 /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES audio_screen_singer_name_properties =
{
    GX_STRING_ID_STRING_114,                 /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES audio_screen_second_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    time_value_format,                       /* format function                */
    30                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES audio_screen_second_colon_properties =
{
    GX_STRING_ID_STRING_22,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES audio_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_keypad_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES audio_screen_prompt_6_properties =
{
    GX_STRING_ID_STRING_116,                 /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES audio_screen_icon_properties =
{
    GX_PIXELMAP_ID_ICON_ARTIST,              /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_end_call_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES audio_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES audio_screen_end_call_btn_icon_properties =
{
    GX_PIXELMAP_ID_ICON_RADIO,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_pixelmap_button_4_properties =
{
    GX_PIXELMAP_ID_PLAYER_BACK,              /* normal pixelmap id             */
    GX_PIXELMAP_ID_PLAYER_BACK_HOVER,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_pixelmap_button_properties =
{
    GX_PIXELMAP_ID_PLAYER_FORWARD,           /* normal pixelmap id             */
    GX_PIXELMAP_ID_PLAYER_FORWARD_HOVER,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES audio_screen_window_properties =
{
    GX_PIXELMAP_ID_PLAYER_CIRCLE             /* wallpaper pixelmap id          */
};
GX_RADIAL_PROGRESS_BAR_INFO audio_screen_radial_progress_bar_properties =
{
    319,                                     /* xcenter                        */
    231,                                     /* ycenter                        */
    55,                                      /* radius                         */
    -90,                                     /* current val                    */
    90,                                      /* anchor val                     */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    12,                                      /* normal brush width             */
    12,                                      /* selected brush width           */
    GX_COLOR_ID_SLIDER_NEEDLE_FILL,          /* normal brush color             */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected brush color           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_play_btn_properties =
{
    GX_PIXELMAP_ID_PLAY,                     /* normal pixelmap id             */
    GX_PIXELMAP_ID_PAUSE,                    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES audio_screen_play_mode_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_pixelmap_button_1_properties =
{
    GX_PIXELMAP_ID_ICON_RANDOM,              /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_RANDOM_ACTIVE,       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_pixelmap_button_2_properties =
{
    GX_PIXELMAP_ID_ICON_LOOP,                /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_LOOP_ACTIVE,         /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES audio_screen_pixelmap_button_5_properties =
{
    GX_PIXELMAP_ID_ICON_LIST,                /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_LIST_ACTIVE,         /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_minute_define =
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
    {416, 167, 428, 187},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_minute), /* control block */
    (void *) &audio_screen_minute_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_second_colon_define =
{
    "second_colon",
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
    {430, 165, 436, 185},                    /* widget size                    */
    &audio_screen_minute_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_second_colon), /* control block */
    (void *) &audio_screen_second_colon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_second_define =
{
    "second",
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
    {437, 167, 462, 187},                    /* widget size                    */
    &audio_screen_second_colon_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_second), /* control block */
    (void *) &audio_screen_second_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_singer_name_define =
{
    "singer_name",
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
    {271, 98, 367, 126},                     /* widget size                    */
    &audio_screen_second_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_singer_name), /* control block */
    (void *) &audio_screen_singer_name_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_music_name_define =
{
    "music_name",
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
    {199, 133, 439, 153},                    /* widget size                    */
    &audio_screen_singer_name_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_music_name), /* control block */
    (void *) &audio_screen_music_name_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_icon_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {65, 123, 87, 144},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_icon), /* control block  */
    (void *) &audio_screen_icon_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_prompt_6_define =
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
    {56, 163, 95, 180},                      /* widget size                    */
    &audio_screen_icon_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_prompt_6), /* control block */
    (void *) &audio_screen_prompt_6_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_end_call_btn_icon_define =
{
    "end_call_btn_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {65, 298, 86, 317},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_end_call_btn_icon), /* control block */
    (void *) &audio_screen_end_call_btn_icon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_prompt_1_define =
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
    {54, 339, 96, 356},                      /* widget size                    */
    &audio_screen_end_call_btn_icon_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_prompt_1), /* control block */
    (void *) &audio_screen_prompt_1_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_play_btn_define =
{
    "play_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PLAY_BTN,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {285, 193, 355, 265},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_play_btn), /* control block */
    (void *) &audio_screen_play_btn_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_radial_progress_bar_define =
{
    "radial_progress_bar",
    GX_TYPE_RADIAL_PROGRESS_BAR,             /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_RADIAL_PROGRESS_NO_BACKTRACK|GX_STYLE_RADIAL_PROGRESS_ALIAS|GX_STYLE_RADIAL_PROGRESS_ROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_PROGRESS_BAR),          /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_GREEN,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radial_progress_bar_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {258, 170, 380, 292},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &audio_screen_play_btn_define,           /* child widget definition        */
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_radial_progress_bar), /* control block */
    (void *) &audio_screen_radial_progress_bar_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_pixelmap_button_5_define =
{
    "pixelmap_button_5",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {223, 316, 266, 345},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_pixelmap_button_5), /* control block */
    (void *) &audio_screen_pixelmap_button_5_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_pixelmap_button_2_define =
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
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {371, 316, 414, 345},                    /* widget size                    */
    &audio_screen_pixelmap_button_5_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_pixelmap_button_2), /* control block */
    (void *) &audio_screen_pixelmap_button_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_pixelmap_button_1_define =
{
    "pixelmap_button_1",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_LIGHT_GREEN,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {298, 316, 341, 345},                    /* widget size                    */
    &audio_screen_pixelmap_button_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_pixelmap_button_1), /* control block */
    (void *) &audio_screen_pixelmap_button_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_play_mode_win_define =
{
    "play_mode_win",
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
    {179, 303, 461, 360},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &audio_screen_pixelmap_button_1_define,  /* child widget definition        */
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_play_mode_win), /* control block */
    (void *) &audio_screen_play_mode_win_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_window_define =
{
    "window",
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
    {256, 168, 383, 295},                    /* widget size                    */
    &audio_screen_play_mode_win_define,      /* next widget definition         */
    &audio_screen_radial_progress_bar_define, /* child widget definition       */
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_window), /* control block */
    (void *) &audio_screen_window_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_pixelmap_button_define =
{
    "pixelmap_button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NEXT,                                 /* widget id                      */
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
    {404, 205, 461, 252},                    /* widget size                    */
    &audio_screen_window_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_pixelmap_button), /* control block */
    (void *) &audio_screen_pixelmap_button_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_pixelmap_button_4_define =
{
    "pixelmap_button_4",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_PREVIOUS,                             /* widget id                      */
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
    {179, 205, 236, 252},                    /* widget size                    */
    &audio_screen_pixelmap_button_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_pixelmap_button_4), /* control block */
    (void *) &audio_screen_pixelmap_button_4_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_end_call_btn_define =
{
    "end_call_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BTN_END_CALL,                         /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {39, 268, 112, 356},                     /* widget size                    */
    &audio_screen_pixelmap_button_4_define,  /* next widget definition         */
    &audio_screen_prompt_1_define,           /* child widget definition        */
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_end_call_btn), /* control block */
    (void *) &audio_screen_end_call_btn_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_keypad_btn_define =
{
    "keypad_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BTN_KEYPAD,                           /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {39, 96, 112, 184},                      /* widget size                    */
    &audio_screen_end_call_btn_define,       /* next widget definition         */
    &audio_screen_prompt_6_define,           /* child widget definition        */
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_keypad_btn), /* control block */
    (void *) &audio_screen_keypad_btn_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET audio_screen_music_info_win_define =
{
    "music_info_win",
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
    {164, 87, 483, 194},                     /* widget size                    */
    &audio_screen_keypad_btn_define,         /* next widget definition         */
    &audio_screen_music_name_define,         /* child widget definition        */
    offsetof(AUDIO_SCREEN_CONTROL_BLOCK, audio_screen_music_info_win), /* control block */
    (void *) &audio_screen_music_info_win_properties /* extended properties    */
};

GX_ANIMATION_INFO audio_screen_animation_1 = {
    (GX_WIDGET *) &audio_screen.audio_screen_music_info_win,
    (GX_WIDGET *) &audio_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_SINE_EASE_IN, 0, 0, 1,
    {164, 0}, {164, 87}, 255, 255, 15
};


GX_STUDIO_ACTION audio_screen_on_user_event_slide_in_music_info_winactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &audio_screen, &audio_screen.audio_screen_music_info_win, &audio_screen_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_audio_screen_event_table[] = {
    { USER_EVENT_SLIDE_IN_MUSIC_INFO_WIN, 0, audio_screen_on_user_event_slide_in_music_info_winactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS audio_screen_event_chain = {gx_studio_audio_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))audio_screen_event_process};
static UINT gx_studio_audio_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &audio_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET audio_screen_define =
{
    "audio_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_AUDIO_SCREEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(AUDIO_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_audio_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &audio_screen_music_info_win_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &audio_screen_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES phone_screen_properties =
{
    &volume_screen_base_define,              /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PIXELMAP_BUTTON_PROPERTIES phone_screen_keypad_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES phone_screen_prompt_6_properties =
{
    GX_STRING_ID_STRING_109,                 /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES phone_screen_icon_properties =
{
    GX_PIXELMAP_ID_ICON_PHONE_DIALS,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES phone_screen_end_call_btn_properties =
{
    GX_PIXELMAP_ID_V_MODE_BTN,               /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES phone_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_111,                 /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES phone_screen_end_call_btn_icon_properties =
{
    GX_PIXELMAP_ID_ICON_PHONE,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_PHONE_OFF            /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES phone_screen_phone_call_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES phone_screen_name_properties =
{
    GX_STRING_ID_STRING_112,                 /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_ICON_PROPERTIES phone_screen_portait_icon_properties =
{
    GX_PIXELMAP_ID_PHONE_PHOTO_FRAME,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES phone_screen_icon_15_properties =
{
    GX_PIXELMAP_ID_PHONE_PHOTO,              /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES phone_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    time_value_format,                       /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES phone_screen_second_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    time_value_format,                       /* format function                */
    10                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES phone_screen_second_colon_properties =
{
    GX_STRING_ID_STRING_22,                  /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_icon_define =
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
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {65, 123, 86, 144},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_icon), /* control block  */
    (void *) &phone_screen_icon_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_prompt_6_define =
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
    {48, 163, 103, 180},                     /* widget size                    */
    &phone_screen_icon_define,               /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_prompt_6), /* control block */
    (void *) &phone_screen_prompt_6_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_end_call_btn_icon_define =
{
    "end_call_btn_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_RED,                         /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {65, 295, 86, 316},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_end_call_btn_icon), /* control block */
    (void *) &phone_screen_end_call_btn_icon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_prompt_1_define =
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
    {48, 335, 103, 352},                     /* widget size                    */
    &phone_screen_end_call_btn_icon_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_prompt_1), /* control block */
    (void *) &phone_screen_prompt_1_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_icon_15_define =
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
    {257, 103, 382, 228},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_icon_15), /* control block */
    (void *) &phone_screen_icon_15_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_second_colon_define =
{
    "second_colon",
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
    {303, 284, 315, 325},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_second_colon), /* control block */
    (void *) &phone_screen_second_colon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_second_define =
{
    "second",
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
    {315, 289, 364, 330},                    /* widget size                    */
    &phone_screen_second_colon_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_second), /* control block */
    (void *) &phone_screen_second_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_minute_define =
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
    {257, 289, 306, 330},                    /* widget size                    */
    &phone_screen_second_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_minute), /* control block */
    (void *) &phone_screen_minute_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_portait_icon_define =
{
    "portait_icon",
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
    {257, 103, 382, 228},                    /* widget size                    */
    &phone_screen_minute_define,             /* next widget definition         */
    &phone_screen_icon_15_define,            /* child widget definition        */
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_portait_icon), /* control block */
    (void *) &phone_screen_portait_icon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_name_define =
{
    "name",
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
    {197, 239, 433, 267},                    /* widget size                    */
    &phone_screen_portait_icon_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_name), /* control block  */
    (void *) &phone_screen_name_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_phone_call_win_define =
{
    "phone_call_win",
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
    {156, 96, 475, 335},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &phone_screen_name_define,               /* child widget definition        */
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_phone_call_win), /* control block */
    (void *) &phone_screen_phone_call_win_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_end_call_btn_define =
{
    "end_call_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BTN_END_CALL,                         /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {39, 268, 112, 356},                     /* widget size                    */
    &phone_screen_phone_call_win_define,     /* next widget definition         */
    &phone_screen_prompt_1_define,           /* child widget definition        */
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_end_call_btn), /* control block */
    (void *) &phone_screen_end_call_btn_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET phone_screen_keypad_btn_define =
{
    "keypad_btn",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BTN_KEYPAD,                           /* widget id                      */
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
    (VOID (*)(GX_WIDGET *)) custom_pixelmap_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {39, 96, 112, 184},                      /* widget size                    */
    &phone_screen_end_call_btn_define,       /* next widget definition         */
    &phone_screen_prompt_6_define,           /* child widget definition        */
    offsetof(PHONE_SCREEN_CONTROL_BLOCK, phone_screen_keypad_btn), /* control block */
    (void *) &phone_screen_keypad_btn_properties /* extended properties        */
};

GX_ANIMATION_INFO phone_screen_animation_1 = {
    (GX_WIDGET *) &phone_screen.phone_screen_phone_call_win,
    (GX_WIDGET *) &phone_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, 0, 0, 1,
    {156, 96}, {156, 96}, 255, 0, 20
};


GX_STUDIO_ACTION phone_screen__id_btn_end_call_gx_event_toggle_on_actions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &phone_screen, &phone_screen.phone_screen_phone_call_win, &phone_screen_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION phone_screen_on_gx_event_showactions[2] = {
    {GX_ACTION_TYPE_ATTACH, 0, &phone_screen, &phone_screen.phone_screen_phone_call_win, GX_NULL},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_phone_screen_event_table[] = {
    {GX_SIGNAL(ID_BTN_END_CALL, GX_EVENT_TOGGLE_ON), 0, phone_screen__id_btn_end_call_gx_event_toggle_on_actions},
    { GX_EVENT_SHOW, 0, phone_screen_on_gx_event_showactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS phone_screen_event_chain = {gx_studio_phone_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))phone_screen_event_process};
static UINT gx_studio_phone_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &phone_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET phone_screen_define =
{
    "phone_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_PHONE_SCREEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(PHONE_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_phone_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &phone_screen_keypad_btn_define,         /* child widget                   */
    0,                                       /* control block                  */
    (void *) &phone_screen_properties        /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY demo_guix_car_infotainment_widget_table[] =
{
    { &video_screen_define, (GX_WIDGET *) &video_screen },
    { &audio_screen_define, (GX_WIDGET *) &audio_screen },
    { &phone_screen_define, (GX_WIDGET *) &phone_screen },
    { &climate_screen_define, (GX_WIDGET *) &climate_screen },
    { &vehicle_screen_define, (GX_WIDGET *) &vehicle_screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = demo_guix_car_infotainment_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &demo_guix_car_infotainment_display_table[display];


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
