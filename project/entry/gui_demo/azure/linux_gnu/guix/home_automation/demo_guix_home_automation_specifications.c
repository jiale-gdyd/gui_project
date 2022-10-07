#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "demo_guix_home_automation_resources.h"
#include "demo_guix_home_automation_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
THERMOSTAT_PAGE_4_CONTROL_BLOCK thermostat_page_4;
THERMOSTAT_PAGE_3_CONTROL_BLOCK thermostat_page_3;
THERMOSTAT_PAGE_2_CONTROL_BLOCK thermostat_page_2;
THERMOSTAT_PAGE_1_CONTROL_BLOCK thermostat_page_1;
THERMOSTAT_SCREEN_CONTROL_BLOCK thermostat_screen;
LIGHTS_PAGE_3_CONTROL_BLOCK lights_page_3;
LIGHTS_PAGE_2_CONTROL_BLOCK lights_page_2;
LIGHTS_PAGE_4_CONTROL_BLOCK lights_page_4;
LIGHTS_PAGE_1_CONTROL_BLOCK lights_page_1;
LIGHTS_SCREEN_CONTROL_BLOCK lights_screen;
LOCKS_PAGE_3_CONTROL_BLOCK locks_page_3;
LOCKS_PAGE_2_CONTROL_BLOCK locks_page_2;
LOCKS_PAGE_1_CONTROL_BLOCK locks_page_1;
LOCKS_SCREEN_CONTROL_BLOCK locks_screen;
WIN_SAN_DIEGO_CONTROL_BLOCK win_San_Diego;
WIN_NEW_YORK_CONTROL_BLOCK win_New_York;
WIN_LAS_VEGAS_CONTROL_BLOCK win_Las_Vegas;
WEATHER_SCREEN_CONTROL_BLOCK weather_screen;
PASSCODE_SCREEN_CONTROL_BLOCK passcode_screen;
MAIN_SCREEN_CONTROL_BLOCK main_screen;
GX_DISPLAY display_1_control_block;
GX_WINDOW_ROOT display_1_root_window;
GX_CANVAS  display_1_canvas_control_block;
ULONG      display_1_canvas_memory[307200];

extern GX_CONST GX_THEME *display_1_theme_table[];
extern GX_CONST GX_STRING *display_1_language_table[];

GX_STUDIO_DISPLAY_INFO demo_guix_home_automation_display_table[1] =
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


UINT gx_studio_multi_line_text_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_MULTI_LINE_TEXT_BUTTON *button = (GX_MULTI_LINE_TEXT_BUTTON *) control_block;
    GX_ML_TEXT_BUTTON_PROPERTIES *props = (GX_ML_TEXT_BUTTON_PROPERTIES *) info->properties;
    status = gx_multi_line_text_button_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set((GX_TEXT_BUTTON *) button, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_text_button_text_color_set((GX_TEXT_BUTTON *) button, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_text_button_text_color_set((GX_TEXT_BUTTON *) button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
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
GX_WINDOW_PROPERTIES weather_base_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES weather_base_window_bg_properties =
{
    GX_PIXELMAP_ID_BOX_WIDE_50               /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_city_name_properties =
{
    GX_STRING_ID_SANDIEGO,                   /* string id                      */
    GX_FONT_ID_CITY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_temperature_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_TIME,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_degree_lable_properties =
{
    GX_STRING_ID_STRING_16,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_weather_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES weather_base_weather_icon_properties =
{
    GX_PIXELMAP_ID_WEATHER_ICON_CLOUDY,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES weather_base_weather_text_properties =
{
    GX_STRING_ID_PARTLY_CLOUDY,              /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_wind_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES weather_base_wind_icon_properties =
{
    GX_PIXELMAP_ID_WEATHER_ICON_WIND,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_1_properties =
{
    GX_STRING_ID_STRING_23,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_3_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_wind_speed_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    19                                       /* numeric prompt value           */
};
GX_WINDOW_PROPERTIES weather_base_humdity_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES weather_base_humidity_icon_properties =
{
    GX_PIXELMAP_ID_WEATHER_ICON_HUMIDITY,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_2_properties =
{
    GX_STRING_ID_STRING_26,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_humidity_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    58                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_4_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_thu_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_thu_properties =
{
    GX_STRING_ID_STRING_28,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_thu_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_SUN,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_thu_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_thu_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_1_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_fri_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_5_properties =
{
    GX_STRING_ID_STRING_29,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_fri_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_SUN,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_fri_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_fri_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_2_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_3_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_sat_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_6_properties =
{
    GX_STRING_ID_STRING_30,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_sat_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_CLOUDY, /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_sat_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_sat_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_4_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_5_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_sun_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_7_properties =
{
    GX_STRING_ID_STRING_31,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_sun_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_CLOUDY, /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_sun_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_sun_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_6_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_7_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_mon_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_8_properties =
{
    GX_STRING_ID_STRING_32,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_mon_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_CLOUDY, /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_mon_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_mon_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_8_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_9_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_tue_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_9_properties =
{
    GX_STRING_ID_STRING_34,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_tue_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_SUN,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_tue_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_tue_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_10_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_11_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_wed_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_prompt_14_10_properties =
{
    GX_STRING_ID_STRING_33,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES weather_base_wed_wicon_properties =
{
    GX_PIXELMAP_ID_WEATHER_7DAY_ICON_SUN,    /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_wed_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_wed_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_12_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_prompt_17_13_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_bottom_info_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES weather_base_hi_label_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    70                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_low_label_properties =
{
    GX_STRING_ID_STRING_10,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_rain_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    0                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES weather_base_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    82                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES weather_base_rain_label_properties =
{
    GX_STRING_ID_RAIN,                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES weather_base_percent_lable_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES weather_base_v_line_left_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES weather_base_v_line_right_properties =
{
    0                                        /* wallpaper pixelmap id          */
};

GX_CONST GX_STUDIO_WIDGET weather_base_weather_text_define =
{
    "weather_text",
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
    {68, 172, 146, 184},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_weather_text), /* control block */
    (void *) &weather_base_weather_text_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_weather_icon_define =
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
    {77, 108, 140, 169},                     /* widget size                    */
    &weather_base_weather_text_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_weather_icon), /* control block */
    (void *) &weather_base_weather_icon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wind_speed_define =
{
    "wind_speed",
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
    {116, 254, 133, 271},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wind_speed), /* control block */
    (void *) &weather_base_wind_speed_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_3_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {137, 258, 164, 270},                    /* widget size                    */
    &weather_base_wind_speed_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_3), /* control block */
    (void *) &weather_base_prompt_14_3_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_1_define =
{
    "prompt_14_1",
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
    GX_COLOR_ID_GRAY,                        /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {54, 258, 115, 270},                     /* widget size                    */
    &weather_base_prompt_14_3_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_1), /* control block */
    (void *) &weather_base_prompt_14_1_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wind_icon_define =
{
    "wind_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {76, 203, 139, 249},                     /* widget size                    */
    &weather_base_prompt_14_1_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wind_icon), /* control block */
    (void *) &weather_base_wind_icon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_4_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {144, 340, 154, 352},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_4), /* control block */
    (void *) &weather_base_prompt_14_4_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_humidity_value_define =
{
    "humidity_value",
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
    {126, 339, 143, 356},                    /* widget size                    */
    &weather_base_prompt_14_4_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_humidity_value), /* control block */
    (void *) &weather_base_humidity_value_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_2_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {62, 343, 120, 355},                     /* widget size                    */
    &weather_base_humidity_value_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_2), /* control block */
    (void *) &weather_base_prompt_14_2_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_humidity_icon_define =
{
    "humidity_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {79, 290, 142, 336},                     /* widget size                    */
    &weather_base_prompt_14_2_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_humidity_icon), /* control block */
    (void *) &weather_base_humidity_icon_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_1_define =
{
    "prompt_17_1",
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
    {593, 111, 598, 120},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_1), /* control block */
    (void *) &weather_base_prompt_17_1_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {562, 111, 567, 120},                    /* widget size                    */
    &weather_base_prompt_17_1_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17), /* control block */
    (void *) &weather_base_prompt_17_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_thu_low_value_define =
{
    "thu_low_value",
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
    {578, 115, 593, 127},                    /* widget size                    */
    &weather_base_prompt_17_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_thu_low_value), /* control block */
    (void *) &weather_base_thu_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_thu_hi_value_define =
{
    "thu_hi_value",
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
    {547, 115, 562, 127},                    /* widget size                    */
    &weather_base_thu_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_thu_hi_value), /* control block */
    (void *) &weather_base_thu_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_thu_wicon_define =
{
    "thu_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {503, 107, 536, 136},                    /* widget size                    */
    &weather_base_thu_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_thu_wicon), /* control block */
    (void *) &weather_base_thu_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_thu_define =
{
    "thu",
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
    {464, 115, 486, 127},                    /* widget size                    */
    &weather_base_thu_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_thu), /* control block   */
    (void *) &weather_base_thu_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_3_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {593, 149, 598, 158},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_3), /* control block */
    (void *) &weather_base_prompt_17_3_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_2_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {562, 149, 567, 158},                    /* widget size                    */
    &weather_base_prompt_17_3_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_2), /* control block */
    (void *) &weather_base_prompt_17_2_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_fri_low_value_define =
{
    "fri_low_value",
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
    {578, 153, 593, 165},                    /* widget size                    */
    &weather_base_prompt_17_2_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_fri_low_value), /* control block */
    (void *) &weather_base_fri_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_fri_hi_value_define =
{
    "fri_hi_value",
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
    {547, 153, 562, 165},                    /* widget size                    */
    &weather_base_fri_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_fri_hi_value), /* control block */
    (void *) &weather_base_fri_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_fri_wicon_define =
{
    "fri_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {502, 145, 535, 174},                    /* widget size                    */
    &weather_base_fri_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_fri_wicon), /* control block */
    (void *) &weather_base_fri_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_5_define =
{
    "prompt_14_5",
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
    {464, 153, 478, 165},                    /* widget size                    */
    &weather_base_fri_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_5), /* control block */
    (void *) &weather_base_prompt_14_5_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_5_define =
{
    "prompt_17_5",
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
    {593, 187, 598, 196},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_5), /* control block */
    (void *) &weather_base_prompt_17_5_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_4_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {562, 187, 567, 196},                    /* widget size                    */
    &weather_base_prompt_17_5_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_4), /* control block */
    (void *) &weather_base_prompt_17_4_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sat_low_value_define =
{
    "sat_low_value",
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
    {578, 191, 593, 203},                    /* widget size                    */
    &weather_base_prompt_17_4_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sat_low_value), /* control block */
    (void *) &weather_base_sat_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sat_hi_value_define =
{
    "sat_hi_value",
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
    {547, 191, 562, 203},                    /* widget size                    */
    &weather_base_sat_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sat_hi_value), /* control block */
    (void *) &weather_base_sat_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sat_wicon_define =
{
    "sat_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {501, 183, 534, 212},                    /* widget size                    */
    &weather_base_sat_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sat_wicon), /* control block */
    (void *) &weather_base_sat_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_6_define =
{
    "prompt_14_6",
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
    {465, 191, 484, 203},                    /* widget size                    */
    &weather_base_sat_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_6), /* control block */
    (void *) &weather_base_prompt_14_6_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_7_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {593, 226, 598, 235},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_7), /* control block */
    (void *) &weather_base_prompt_17_7_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_6_define =
{
    "prompt_17_6",
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
    {562, 226, 567, 235},                    /* widget size                    */
    &weather_base_prompt_17_7_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_6), /* control block */
    (void *) &weather_base_prompt_17_6_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sun_low_value_define =
{
    "sun_low_value",
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
    {578, 230, 593, 242},                    /* widget size                    */
    &weather_base_prompt_17_6_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sun_low_value), /* control block */
    (void *) &weather_base_sun_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sun_hi_value_define =
{
    "sun_hi_value",
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
    {547, 230, 562, 242},                    /* widget size                    */
    &weather_base_sun_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sun_hi_value), /* control block */
    (void *) &weather_base_sun_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sun_wicon_define =
{
    "sun_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {502, 222, 535, 251},                    /* widget size                    */
    &weather_base_sun_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sun_wicon), /* control block */
    (void *) &weather_base_sun_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_7_define =
{
    "prompt_14_7",
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
    {464, 229, 487, 241},                    /* widget size                    */
    &weather_base_sun_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_7), /* control block */
    (void *) &weather_base_prompt_14_7_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_9_define =
{
    "prompt_17_9",
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
    {593, 264, 598, 273},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_9), /* control block */
    (void *) &weather_base_prompt_17_9_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_8_define =
{
    "prompt_17_8",
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
    {562, 264, 567, 273},                    /* widget size                    */
    &weather_base_prompt_17_9_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_8), /* control block */
    (void *) &weather_base_prompt_17_8_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_mon_low_value_define =
{
    "mon_low_value",
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
    {578, 268, 593, 280},                    /* widget size                    */
    &weather_base_prompt_17_8_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_mon_low_value), /* control block */
    (void *) &weather_base_mon_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_mon_hi_value_define =
{
    "mon_hi_value",
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
    {547, 268, 562, 280},                    /* widget size                    */
    &weather_base_mon_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_mon_hi_value), /* control block */
    (void *) &weather_base_mon_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_mon_wicon_define =
{
    "mon_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {502, 260, 535, 289},                    /* widget size                    */
    &weather_base_mon_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_mon_wicon), /* control block */
    (void *) &weather_base_mon_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_8_define =
{
    "prompt_14_8",
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
    {466, 268, 490, 280},                    /* widget size                    */
    &weather_base_mon_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_8), /* control block */
    (void *) &weather_base_prompt_14_8_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_11_define =
{
    "prompt_17_11",
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
    {593, 302, 598, 311},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_11), /* control block */
    (void *) &weather_base_prompt_17_11_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_10_define =
{
    "prompt_17_10",
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
    {562, 302, 567, 311},                    /* widget size                    */
    &weather_base_prompt_17_11_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_10), /* control block */
    (void *) &weather_base_prompt_17_10_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_tue_low_value_define =
{
    "tue_low_value",
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
    {578, 306, 593, 318},                    /* widget size                    */
    &weather_base_prompt_17_10_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_tue_low_value), /* control block */
    (void *) &weather_base_tue_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_tue_hi_value_define =
{
    "tue_hi_value",
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
    {547, 306, 562, 318},                    /* widget size                    */
    &weather_base_tue_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_tue_hi_value), /* control block */
    (void *) &weather_base_tue_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_tue_wicon_define =
{
    "tue_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {502, 298, 535, 327},                    /* widget size                    */
    &weather_base_tue_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_tue_wicon), /* control block */
    (void *) &weather_base_tue_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_9_define =
{
    "prompt_14_9",
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
    {466, 304, 487, 316},                    /* widget size                    */
    &weather_base_tue_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_9), /* control block */
    (void *) &weather_base_prompt_14_9_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_13_define =
{
    "prompt_17_13",
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
    {593, 339, 598, 348},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_13), /* control block */
    (void *) &weather_base_prompt_17_13_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_17_12_define =
{
    "prompt_17_12",
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
    {562, 339, 567, 348},                    /* widget size                    */
    &weather_base_prompt_17_13_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_17_12), /* control block */
    (void *) &weather_base_prompt_17_12_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wed_low_value_define =
{
    "wed_low_value",
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
    {578, 343, 593, 355},                    /* widget size                    */
    &weather_base_prompt_17_12_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wed_low_value), /* control block */
    (void *) &weather_base_wed_low_value_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wed_hi_value_define =
{
    "wed_hi_value",
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
    {547, 343, 562, 355},                    /* widget size                    */
    &weather_base_wed_low_value_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wed_hi_value), /* control block */
    (void *) &weather_base_wed_hi_value_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wed_wicon_define =
{
    "wed_wicon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {502, 334, 535, 363},                    /* widget size                    */
    &weather_base_wed_hi_value_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wed_wicon), /* control block */
    (void *) &weather_base_wed_wicon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_prompt_14_10_define =
{
    "prompt_14_10",
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
    {465, 343, 491, 355},                    /* widget size                    */
    &weather_base_wed_wicon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_prompt_14_10), /* control block */
    (void *) &weather_base_prompt_14_10_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_percent_lable_define =
{
    "percent_lable",
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
    {397, 341, 407, 353},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_percent_lable), /* control block */
    (void *) &weather_base_percent_lable_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET weather_base_rain_label_define =
{
    "rain_label",
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
    {349, 343, 374, 355},                    /* widget size                    */
    &weather_base_percent_lable_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_rain_label), /* control block */
    (void *) &weather_base_rain_label_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET weather_base_hi_value_define =
{
    "hi_value",
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
    {260, 340, 277, 357},                    /* widget size                    */
    &weather_base_rain_label_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_hi_value), /* control block */
    (void *) &weather_base_hi_value_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_base_rain_value_define =
{
    "rain_value",
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
    {378, 340, 398, 357},                    /* widget size                    */
    &weather_base_hi_value_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_rain_value), /* control block */
    (void *) &weather_base_rain_value_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET weather_base_low_label_define =
{
    "low_label",
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
    {286, 344, 309, 356},                    /* widget size                    */
    &weather_base_rain_value_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_low_label), /* control block */
    (void *) &weather_base_low_label_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_low_value_define =
{
    "low_value",
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
    {313, 340, 330, 357},                    /* widget size                    */
    &weather_base_low_label_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_low_value), /* control block */
    (void *) &weather_base_low_value_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_hi_label_define =
{
    "hi_label",
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
    {244, 344, 255, 356},                    /* widget size                    */
    &weather_base_low_value_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_hi_label), /* control block */
    (void *) &weather_base_hi_label_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_base_v_line_right_define =
{
    "v_line_right",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_V_LINE,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_GRAY,                        /* selected color id              */
    GX_COLOR_ID_GRAY,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {441, 116, 441, 358},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_v_line_right), /* control block */
    (void *) &weather_base_v_line_right_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_v_line_left_define =
{
    "v_line_left",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_V_LINE,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_GRAY,                        /* normal color id                */
    GX_COLOR_ID_GRAY,                        /* selected color id              */
    GX_COLOR_ID_GRAY,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {196, 118, 196, 360},                    /* widget size                    */
    &weather_base_v_line_right_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_v_line_left), /* control block */
    (void *) &weather_base_v_line_left_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_bottom_info_win_define =
{
    "bottom_info_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_BOTTOM_INFO_WIN,                      /* widget id                      */
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
    {234, 328, 414, 366},                    /* widget size                    */
    &weather_base_v_line_left_define,        /* next widget definition         */
    &weather_base_hi_label_define,           /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_bottom_info_win), /* control block */
    (void *) &weather_base_bottom_info_win_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wed_win_define =
{
    "wed_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_WED_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 333, 611, 366},                    /* widget size                    */
    &weather_base_bottom_info_win_define,    /* next widget definition         */
    &weather_base_prompt_14_10_define,       /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wed_win), /* control block */
    (void *) &weather_base_wed_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_tue_win_define =
{
    "tue_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_TUE_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 296, 611, 329},                    /* widget size                    */
    &weather_base_wed_win_define,            /* next widget definition         */
    &weather_base_prompt_14_9_define,        /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_tue_win), /* control block */
    (void *) &weather_base_tue_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_mon_win_define =
{
    "mon_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_MON_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 258, 611, 291},                    /* widget size                    */
    &weather_base_tue_win_define,            /* next widget definition         */
    &weather_base_prompt_14_8_define,        /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_mon_win), /* control block */
    (void *) &weather_base_mon_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sun_win_define =
{
    "sun_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SUN_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 220, 611, 253},                    /* widget size                    */
    &weather_base_mon_win_define,            /* next widget definition         */
    &weather_base_prompt_14_7_define,        /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sun_win), /* control block */
    (void *) &weather_base_sun_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_sat_win_define =
{
    "sat_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_SAT_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 181, 611, 214},                    /* widget size                    */
    &weather_base_sun_win_define,            /* next widget definition         */
    &weather_base_prompt_14_6_define,        /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_sat_win), /* control block */
    (void *) &weather_base_sat_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_fri_win_define =
{
    "fri_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_FRI_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 143, 611, 176},                    /* widget size                    */
    &weather_base_sat_win_define,            /* next widget definition         */
    &weather_base_prompt_14_5_define,        /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_fri_win), /* control block */
    (void *) &weather_base_fri_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_thu_win_define =
{
    "thu_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_THU_WIN,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {455, 105, 611, 138},                    /* widget size                    */
    &weather_base_fri_win_define,            /* next widget definition         */
    &weather_base_thu_define,                /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_thu_win), /* control block */
    (void *) &weather_base_thu_win_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET weather_base_humdity_win_define =
{
    "humdity_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_HUMIDITY_WIN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {48, 287, 174, 367},                     /* widget size                    */
    &weather_base_thu_win_define,            /* next widget definition         */
    &weather_base_humidity_icon_define,      /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_humdity_win), /* control block */
    (void *) &weather_base_humdity_win_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_wind_win_define =
{
    "wind_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_WIND_WIN,                             /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {48, 200, 174, 280},                     /* widget size                    */
    &weather_base_humdity_win_define,        /* next widget definition         */
    &weather_base_wind_icon_define,          /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_wind_win), /* control block */
    (void *) &weather_base_wind_win_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET weather_base_weather_win_define =
{
    "weather_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_WEATHER_WIN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {48, 113, 174, 193},                     /* widget size                    */
    &weather_base_wind_win_define,           /* next widget definition         */
    &weather_base_weather_icon_define,       /* child widget definition        */
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_weather_win), /* control block */
    (void *) &weather_base_weather_win_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_degree_lable_define =
{
    "degree_lable",
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
    {398, 180, 414, 202},                    /* widget size                    */
    &weather_base_weather_win_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_degree_lable), /* control block */
    (void *) &weather_base_degree_lable_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET weather_base_temperature_define =
{
    "temperature",
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
    {244, 185, 403, 279},                    /* widget size                    */
    &weather_base_degree_lable_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_temperature), /* control block */
    (void *) &weather_base_temperature_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET weather_base_city_name_define =
{
    "city_name",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_TITLE,                                /* widget id                      */
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
    {221, 114, 424, 155},                    /* widget size                    */
    &weather_base_temperature_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_city_name), /* control block */
    (void *) &weather_base_city_name_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_window_bg_define =
{
    "window_bg",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_WEATHER_BG,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {21, 90, 620, 384},                      /* widget size                    */
    &weather_base_city_name_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_BASE_CONTROL_BLOCK, weather_base_window_bg), /* control block */
    (void *) &weather_base_window_bg_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET weather_base_define =
{
    "weather_base",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_BASE_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_TEXT,                        /* selected color id              */
    GX_COLOR_ID_TEXT,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {21, 90, 631, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &weather_base_window_bg_define,          /* child widget                   */
    0,                                       /* control block                  */
    (void *) &weather_base_properties        /* extended properties            */
};
GX_WINDOW_PROPERTIES passcode_screen_properties =
{
    GX_PIXELMAP_ID_OVERLAY_PASSCODE_BG       /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES passcode_screen_passcode_1_properties =
{
    GX_PIXELMAP_ID_PASSCODE_ASTERIC,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES passcode_screen_passcode_4_properties =
{
    GX_PIXELMAP_ID_PASSCODE_ASTERIC,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES passcode_screen_passcode_3_properties =
{
    GX_PIXELMAP_ID_PASSCODE_ASTERIC,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES passcode_screen_passcode_2_properties =
{
    GX_PIXELMAP_ID_PASSCODE_ASTERIC,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES passcode_screen_num_pad_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_0_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_10_properties =
{
    GX_STRING_ID_STRING_80,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_1_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_properties =
{
    GX_STRING_ID_STRING_70,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_2_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_1_properties =
{
    GX_STRING_ID_STRING_71,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_3_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_2_properties =
{
    GX_STRING_ID_STRING_72,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_4_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_3_properties =
{
    GX_STRING_ID_STRING_73,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_5_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_4_properties =
{
    GX_STRING_ID_STRING_74,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_6_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_5_properties =
{
    GX_STRING_ID_STRING_75,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_7_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_6_properties =
{
    GX_STRING_ID_STRING_76,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_8_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_7_properties =
{
    GX_STRING_ID_STRING_77,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_9_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_8_properties =
{
    GX_STRING_ID_STRING_78,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_num_clear_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_9_properties =
{
    GX_STRING_ID_STRING_81,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_close_properties =
{
    GX_PIXELMAP_ID_ICON_CLOSE,               /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_CLOSE_ACTIVE,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES passcode_screen_ok_properties =
{
    GX_PIXELMAP_ID_PASSCODE_BTN,             /* normal pixelmap id             */
    GX_PIXELMAP_ID_PASSCODE_BTN_ACTIVE,      /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES passcode_screen_prompt_11_11_properties =
{
    GX_STRING_ID_STRING_79,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES passcode_screen_title_properties =
{
    GX_STRING_ID_ENTER_PASSCODE,             /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_10_define =
{
    "prompt_11_10",
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
    {314, 419, 327, 441},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_10), /* control block */
    (void *) &passcode_screen_prompt_11_10_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {113, 194, 126, 216},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11), /* control block */
    (void *) &passcode_screen_prompt_11_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_1_define =
{
    "prompt_11_1",
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
    {314, 194, 327, 216},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_1), /* control block */
    (void *) &passcode_screen_prompt_11_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_2_define =
{
    "prompt_11_2",
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
    {515, 194, 528, 216},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_2), /* control block */
    (void *) &passcode_screen_prompt_11_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_3_define =
{
    "prompt_11_3",
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
    {113, 269, 126, 291},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_3), /* control block */
    (void *) &passcode_screen_prompt_11_3_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_4_define =
{
    "prompt_11_4",
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
    {314, 270, 327, 292},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_4), /* control block */
    (void *) &passcode_screen_prompt_11_4_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_5_define =
{
    "prompt_11_5",
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
    {515, 270, 528, 292},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_5), /* control block */
    (void *) &passcode_screen_prompt_11_5_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_6_define =
{
    "prompt_11_6",
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
    {113, 344, 126, 366},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_6), /* control block */
    (void *) &passcode_screen_prompt_11_6_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_7_define =
{
    "prompt_11_7",
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
    {314, 344, 327, 366},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_7), /* control block */
    (void *) &passcode_screen_prompt_11_7_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_8_define =
{
    "prompt_11_8",
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
    {515, 344, 528, 366},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_8), /* control block */
    (void *) &passcode_screen_prompt_11_8_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_9_define =
{
    "prompt_11_9",
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
    {93, 420, 149, 442},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_9), /* control block */
    (void *) &passcode_screen_prompt_11_9_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_prompt_11_11_define =
{
    "prompt_11_11",
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
    {504, 419, 534, 441},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_prompt_11_11), /* control block */
    (void *) &passcode_screen_prompt_11_11_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_title_define =
{
    "title",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {233, 30, 412, 52},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_title), /* control block */
    (void *) &passcode_screen_title_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_ok_define =
{
    "ok",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_OK,                                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 388, 625, 469},                    /* widget size                    */
    &passcode_screen_title_define,           /* next widget definition         */
    &passcode_screen_prompt_11_11_define,    /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_ok), /* control block */
    (void *) &passcode_screen_ok_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_close_define =
{
    "close",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CLOSE,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {596, 31, 617, 51},                      /* widget size                    */
    &passcode_screen_ok_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_close), /* control block */
    (void *) &passcode_screen_close_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_clear_define =
{
    "num_clear",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CLEAR,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {16, 388, 223, 469},                     /* widget size                    */
    &passcode_screen_close_define,           /* next widget definition         */
    &passcode_screen_prompt_11_9_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_clear), /* control block */
    (void *) &passcode_screen_num_clear_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_9_define =
{
    "num_9",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_9,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 313, 625, 394},                    /* widget size                    */
    &passcode_screen_num_clear_define,       /* next widget definition         */
    &passcode_screen_prompt_11_8_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_9), /* control block */
    (void *) &passcode_screen_num_9_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_8_define =
{
    "num_8",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_8,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {217, 313, 424, 394},                    /* widget size                    */
    &passcode_screen_num_9_define,           /* next widget definition         */
    &passcode_screen_prompt_11_7_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_8), /* control block */
    (void *) &passcode_screen_num_8_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_7_define =
{
    "num_7",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_7,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {16, 313, 223, 394},                     /* widget size                    */
    &passcode_screen_num_8_define,           /* next widget definition         */
    &passcode_screen_prompt_11_6_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_7), /* control block */
    (void *) &passcode_screen_num_7_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_6_define =
{
    "num_6",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_6,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 239, 625, 320},                    /* widget size                    */
    &passcode_screen_num_7_define,           /* next widget definition         */
    &passcode_screen_prompt_11_5_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_6), /* control block */
    (void *) &passcode_screen_num_6_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_5_define =
{
    "num_5",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_5,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {217, 239, 424, 320},                    /* widget size                    */
    &passcode_screen_num_6_define,           /* next widget definition         */
    &passcode_screen_prompt_11_4_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_5), /* control block */
    (void *) &passcode_screen_num_5_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_4_define =
{
    "num_4",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_4,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {16, 238, 223, 319},                     /* widget size                    */
    &passcode_screen_num_5_define,           /* next widget definition         */
    &passcode_screen_prompt_11_3_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_4), /* control block */
    (void *) &passcode_screen_num_4_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_3_define =
{
    "num_3",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_3,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 163, 625, 244},                    /* widget size                    */
    &passcode_screen_num_4_define,           /* next widget definition         */
    &passcode_screen_prompt_11_2_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_3), /* control block */
    (void *) &passcode_screen_num_3_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_2_define =
{
    "num_2",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_2,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {217, 163, 424, 244},                    /* widget size                    */
    &passcode_screen_num_3_define,           /* next widget definition         */
    &passcode_screen_prompt_11_1_define,     /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_2), /* control block */
    (void *) &passcode_screen_num_2_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_1_define =
{
    "num_1",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_1,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {16, 163, 223, 244},                     /* widget size                    */
    &passcode_screen_num_2_define,           /* next widget definition         */
    &passcode_screen_prompt_11_define,       /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_1), /* control block */
    (void *) &passcode_screen_num_1_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_0_define =
{
    "num_0",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_NUM_0,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {217, 388, 424, 469},                    /* widget size                    */
    &passcode_screen_num_1_define,           /* next widget definition         */
    &passcode_screen_prompt_11_10_define,    /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_0), /* control block */
    (void *) &passcode_screen_num_0_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_num_pad_define =
{
    "num_pad",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {9, 11, 633, 472},                       /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &passcode_screen_num_0_define,           /* child widget definition        */
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_num_pad), /* control block */
    (void *) &passcode_screen_num_pad_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_passcode_2_define =
{
    "passcode_2",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {258, 78, 317, 137},                     /* widget size                    */
    &passcode_screen_num_pad_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_passcode_2), /* control block */
    (void *) &passcode_screen_passcode_2_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_passcode_3_define =
{
    "passcode_3",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {322, 78, 381, 137},                     /* widget size                    */
    &passcode_screen_passcode_2_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_passcode_3), /* control block */
    (void *) &passcode_screen_passcode_3_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_passcode_4_define =
{
    "passcode_4",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {386, 78, 445, 137},                     /* widget size                    */
    &passcode_screen_passcode_3_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_passcode_4), /* control block */
    (void *) &passcode_screen_passcode_4_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET passcode_screen_passcode_1_define =
{
    "passcode_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {195, 78, 254, 137},                     /* widget size                    */
    &passcode_screen_passcode_4_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PASSCODE_SCREEN_CONTROL_BLOCK, passcode_screen_passcode_1), /* control block */
    (void *) &passcode_screen_passcode_1_properties /* extended properties     */
};

GX_ANIMATION_INFO passcode_screen_animation_1 = {
    (GX_WIDGET *) &passcode_screen.passcode_screen_num_pad,
    (GX_WIDGET *) &passcode_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_CIRC_EASE_OUT, 0, 0, 1,
    {9, -400}, {9, 11}, 255, 255, 20
};


GX_ANIMATION_INFO passcode_screen_animation_2 = {
    (GX_WIDGET *) &passcode_screen.passcode_screen_passcode_1,
    (GX_WIDGET *) &passcode_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_CIRC_EASE_OUT, 0, 20, 1,
    {195, -60}, {195, 78}, 255, 255, 20
};


GX_ANIMATION_INFO passcode_screen_animation_3 = {
    (GX_WIDGET *) &passcode_screen.passcode_screen_passcode_2,
    (GX_WIDGET *) &passcode_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_CIRC_EASE_OUT, 0, 25, 1,
    {258, -60}, {258, 78}, 255, 255, 20
};


GX_ANIMATION_INFO passcode_screen_animation_4 = {
    (GX_WIDGET *) &passcode_screen.passcode_screen_passcode_3,
    (GX_WIDGET *) &passcode_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_CIRC_EASE_OUT, 0, 30, 1,
    {322, -60}, {322, 78}, 255, 255, 20
};


GX_ANIMATION_INFO passcode_screen_animation_5 = {
    (GX_WIDGET *) &passcode_screen.passcode_screen_passcode_4,
    (GX_WIDGET *) &passcode_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_CIRC_EASE_OUT, 0, 35, 1,
    {386, -60}, {386, 78}, 255, 255, 20
};


GX_STUDIO_ACTION passcode_screen_on_gx_event_showactions[6] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &passcode_screen, &passcode_screen.passcode_screen_num_pad, &passcode_screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &passcode_screen, &passcode_screen.passcode_screen_passcode_1, &passcode_screen_animation_2},
    {GX_ACTION_TYPE_ANIMATION, 0, &passcode_screen, &passcode_screen.passcode_screen_passcode_2, &passcode_screen_animation_3},
    {GX_ACTION_TYPE_ANIMATION, 0, &passcode_screen, &passcode_screen.passcode_screen_passcode_3, &passcode_screen_animation_4},
    {GX_ACTION_TYPE_ANIMATION, 0, &passcode_screen, &passcode_screen.passcode_screen_passcode_4, &passcode_screen_animation_5},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_passcode_screen_event_table[] = {
    { GX_EVENT_SHOW, 0, passcode_screen_on_gx_event_showactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS passcode_screen_event_chain = {gx_studio_passcode_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))passcode_screen_event_process};
static UINT gx_studio_passcode_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &passcode_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET passcode_screen_define =
{
    "passcode_screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(PASSCODE_SCREEN_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_passcode_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &passcode_screen_passcode_1_define,      /* child widget                   */
    0,                                       /* control block                  */
    (void *) &passcode_screen_properties     /* extended properties            */
};
GX_WINDOW_PROPERTIES controller_base_properties =
{
    GX_PIXELMAP_ID_BOX_60                    /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES controller_base_title_properties =
{
    GX_STRING_ID_LOWER_DECK_DOOR,            /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET controller_base_title_define =
{
    "title",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_TITLE,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {345, 103, 491, 120},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(CONTROLLER_BASE_CONTROL_BLOCK, controller_base_title), /* control block */
    (void *) &controller_base_title_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET controller_base_define =
{
    "controller_base",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(CONTROLLER_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {323, 89, 619, 383},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &controller_base_title_define,           /* child widget                   */
    0,                                       /* control block                  */
    (void *) &controller_base_properties     /* extended properties            */
};
GX_WINDOW_PROPERTIES screen_base_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_button_light_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_BTN,          /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_BTN_ACTIVE,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_light_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_LIGHT,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_LIGHT_HOVER, /* selected pixelmap id       */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES screen_base_light_text_properties =
{
    GX_STRING_ID_STRING_2,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_button_thermostat_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_BTN,          /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_BTN_ACTIVE,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_thermostat_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_THERMOSTAT, /* normal pixelmap id          */
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_THERMOSTAT_ACTIVE, /* selected pixelmap id */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES screen_base_thermostat_text_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_button_locks_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_BTN,          /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_BTN_ACTIVE,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_locks_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_LOCK,    /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_LOCK_ACTIVE, /* selected pixelmap id       */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES screen_base_locks_text_properties =
{
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_button_weather_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_BTN,          /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_BTN_ACTIVE,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES screen_base_weather_text_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_weather_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_WEATHER, /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_WEATHER_ACTIVE, /* selected pixelmap id    */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_home_properties =
{
    GX_PIXELMAP_ID_FOOTER_ICON_HOME,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_ICON_HOME_ACTIVE,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_setting_properties =
{
    GX_PIXELMAP_ID_FOOTER_ICON_SETTINGS,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_ICON_SETTINGS_ACTIVE, /* selected pixelmap id        */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES screen_base_slide_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES screen_base_add_properties =
{
    GX_PIXELMAP_ID_FOOTER_ADD_CIRCLE_ICON,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_FOOTER_ADD_CIRCLE_ICON_ACTIVE, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES screen_base_add_text_properties =
{
    GX_STRING_ID_ADD_LIGHT,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_DISABLED                     /* disabled text color            */
};
GX_ICON_PROPERTIES screen_base_icon_6_properties =
{
    GX_PIXELMAP_ID_FOOTER_ADD_CIRCLE_BG,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET screen_base_light_text_define =
{
    "light_text",
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
    GX_COLOR_ID_DISABLED,                    /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {40, 441, 69, 450},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_light_text), /* control block */
    (void *) &screen_base_light_text_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET screen_base_light_define =
{
    "light",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {36, 402, 75, 447},                      /* widget size                    */
    &screen_base_light_text_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_light), /* control block   */
    (void *) &screen_base_light_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_thermostat_text_define =
{
    "thermostat_text",
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
    {105, 442, 161, 451},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_thermostat_text), /* control block */
    (void *) &screen_base_thermostat_text_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET screen_base_thermostat_define =
{
    "thermostat",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {120, 405, 146, 449},                    /* widget size                    */
    &screen_base_thermostat_text_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_thermostat), /* control block */
    (void *) &screen_base_thermostat_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET screen_base_locks_text_define =
{
    "locks_text",
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
    {192, 442, 220, 451},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_locks_text), /* control block */
    (void *) &screen_base_locks_text_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET screen_base_locks_define =
{
    "locks",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {192, 404, 228, 450},                    /* widget size                    */
    &screen_base_locks_text_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_locks), /* control block   */
    (void *) &screen_base_locks_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_weather_define =
{
    "weather",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {257, 405, 309, 443},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_weather), /* control block */
    (void *) &screen_base_weather_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_weather_text_define =
{
    "weather_text",
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
    {263, 442, 304, 451},                    /* widget size                    */
    &screen_base_weather_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_weather_text), /* control block */
    (void *) &screen_base_weather_text_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET screen_base_icon_6_define =
{
    "icon_6",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {601, 393, 621, 413},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_icon_6), /* control block  */
    (void *) &screen_base_icon_6_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_add_text_define =
{
    "add_text",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_GRAY,                        /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {527, 395, 596, 407},                    /* widget size                    */
    &screen_base_icon_6_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_add_text), /* control block */
    (void *) &screen_base_add_text_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET screen_base_add_define =
{
    "add",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_ADD,                                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_HALIGN_RIGHT|GX_STYLE_VALIGN_TOP,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {525, 393, 621, 414},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &screen_base_add_text_define,            /* child widget definition        */
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_add), /* control block     */
    (void *) &screen_base_add_properties     /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_slide_win_define =
{
    "slide_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_BTN_TEXT,                    /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) slide_win_event_process, /* event function override */
    {20, 90, 630, 384},                      /* widget size                    */
    &screen_base_add_define,                 /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_slide_win), /* control block */
    (void *) &screen_base_slide_win_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET screen_base_setting_define =
{
    "setting",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_SETTING,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {548, 436, 572, 460},                    /* widget size                    */
    &screen_base_slide_win_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_setting), /* control block */
    (void *) &screen_base_setting_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_home_define =
{
    "home",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_HOME,                                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {593, 435, 623, 459},                    /* widget size                    */
    &screen_base_setting_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_home), /* control block    */
    (void *) &screen_base_home_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET screen_base_button_weather_define =
{
    "button_weather",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_FOOTER_WEATHER,                       /* widget id                      */
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
    {248, 393, 318, 463},                    /* widget size                    */
    &screen_base_home_define,                /* next widget definition         */
    &screen_base_weather_text_define,        /* child widget definition        */
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_button_weather), /* control block */
    (void *) &screen_base_button_weather_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET screen_base_button_locks_define =
{
    "button_locks",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_FOOTER_LOCKS,                         /* widget id                      */
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
    {171, 393, 241, 463},                    /* widget size                    */
    &screen_base_button_weather_define,      /* next widget definition         */
    &screen_base_locks_define,               /* child widget definition        */
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_button_locks), /* control block */
    (void *) &screen_base_button_locks_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET screen_base_button_thermostat_define =
{
    "button_thermostat",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_FOOTER_THERMOSTAT,                    /* widget id                      */
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
    {96, 393, 166, 463},                     /* widget size                    */
    &screen_base_button_locks_define,        /* next widget definition         */
    &screen_base_thermostat_define,          /* child widget definition        */
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_button_thermostat), /* control block */
    (void *) &screen_base_button_thermostat_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET screen_base_button_light_define =
{
    "button_light",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_FOOTER_LIGHTS,                        /* widget id                      */
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
    {20, 393, 90, 463},                      /* widget size                    */
    &screen_base_button_thermostat_define,   /* next widget definition         */
    &screen_base_light_define,               /* child widget definition        */
    offsetof(SCREEN_BASE_CONTROL_BLOCK, screen_base_button_light), /* control block */
    (void *) &screen_base_button_light_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET screen_base_define =
{
    "screen_base",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SCREEN_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) screen_base_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &screen_base_button_light_define,        /* child widget                   */
    0,                                       /* control block                  */
    (void *) &screen_base_properties         /* extended properties            */
};
GX_WINDOW_PROPERTIES main_screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES main_screen_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES main_screen_weather_text_properties =
{
    GX_STRING_ID_PARTLY_CLOUDY,              /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_weather_icon_properties =
{
    GX_PIXELMAP_ID_FOOTER_MENU_ICON_WEATHER_ACTIVE, /* normal pixelmap id      */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_t_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    75                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_low_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    62                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES main_screen_low_lable_properties =
{
    GX_STRING_ID_STRING_10,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_hi_lable_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_hi_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    82                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES main_screen_dot_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES main_screen_icon_properties =
{
    GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_WINDOW_PROPERTIES main_screen_home_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_lights_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_BTN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_BTN_ACTIVE,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_properties =
{
    GX_STRING_ID_STRING_2,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_4_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_ICON_LIGHT,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_ICON_LIGHT_ACTIVE, /* selected pixelmap id        */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_thermostat_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_BTN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_BTN_ACTIVE,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_5_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_ICON_THERMOSTAT, /* normal pixelmap id            */
    GX_PIXELMAP_ID_HOME_MENU_ICON_THERMOSTAT_ACTIVE, /* selected pixelmap id   */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_weather_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_BTN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_BTN_ACTIVE,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_3_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_7_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_ICON_WEATHER,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_ICON_WEATHER_ACTIVE, /* selected pixelmap id      */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_locks_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_BTN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_BTN_ACTIVE,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES main_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES main_screen_pixelmap_button_6_properties =
{
    GX_PIXELMAP_ID_HOME_MENU_ICON_LOCK,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_HOME_MENU_ICON_LOCK_ACTIVE, /* selected pixelmap id         */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES main_screen_time_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES main_screen_am_pm_properties =
{
    GX_STRING_ID_PM,                         /* string id                      */
    GX_FONT_ID_MIDDLE,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES main_screen_date_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_TIME,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    time_format,                             /* format function                */
    12                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES main_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_TIME,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    time_format,                             /* format function                */
    38                                       /* numeric prompt value           */
};
GX_ICON_PROPERTIES main_screen_upper_dot_properties =
{
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES main_screen_lower_dot_properties =
{
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_dot_define =
{
    "dot",
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
    {610, 13, 616, 25},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_dot), /* control block     */
    (void *) &main_screen_dot_properties     /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_hi_value_define =
{
    "hi_value",
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
    {557, 63, 574, 80},                      /* widget size                    */
    &main_screen_dot_define,                 /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_hi_value), /* control block */
    (void *) &main_screen_hi_value_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_hi_lable_define =
{
    "hi_lable",
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
    {544, 65, 555, 77},                      /* widget size                    */
    &main_screen_hi_value_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_hi_lable), /* control block */
    (void *) &main_screen_hi_lable_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_low_lable_define =
{
    "low_lable",
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
    {581, 65, 604, 77},                      /* widget size                    */
    &main_screen_hi_lable_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_low_lable), /* control block */
    (void *) &main_screen_low_lable_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_low_value_define =
{
    "low_value",
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
    {606, 62, 623, 79},                      /* widget size                    */
    &main_screen_low_lable_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_low_value), /* control block */
    (void *) &main_screen_low_value_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_t_value_define =
{
    "t_value",
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
    {549, 14, 612, 65},                      /* widget size                    */
    &main_screen_low_value_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_t_value), /* control block */
    (void *) &main_screen_t_value_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_weather_icon_define =
{
    "weather_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {468, 19, 520, 57},                      /* widget size                    */
    &main_screen_t_value_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_weather_icon), /* control block */
    (void *) &main_screen_weather_icon_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_weather_text_define =
{
    "weather_text",
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
    {458, 66, 536, 78},                      /* widget size                    */
    &main_screen_weather_icon_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_weather_text), /* control block */
    (void *) &main_screen_weather_text_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_4_define =
{
    "pixelmap_button_4",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {59, 254, 185, 410},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_4), /* control block */
    (void *) &main_screen_pixelmap_button_4_properties /* extended properties  */
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {102, 376, 140, 389},                    /* widget size                    */
    &main_screen_pixelmap_button_4_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt), /* control block  */
    (void *) &main_screen_prompt_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_5_define =
{
    "pixelmap_button_5",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {188, 258, 330, 414},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_5), /* control block */
    (void *) &main_screen_pixelmap_button_5_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_1_define =
{
    "prompt_1",
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
    {217, 376, 291, 389},                    /* widget size                    */
    &main_screen_pixelmap_button_5_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_1), /* control block */
    (void *) &main_screen_prompt_1_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_7_define =
{
    "pixelmap_button_7",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {447, 258, 589, 414},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_7), /* control block */
    (void *) &main_screen_pixelmap_button_7_properties /* extended properties  */
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {492, 377, 546, 390},                    /* widget size                    */
    &main_screen_pixelmap_button_7_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_3), /* control block */
    (void *) &main_screen_prompt_3_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_pixelmap_button_6_define =
{
    "pixelmap_button_6",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {323, 259, 465, 415},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_pixelmap_button_6), /* control block */
    (void *) &main_screen_pixelmap_button_6_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET main_screen_prompt_2_define =
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
    {369, 377, 405, 390},                    /* widget size                    */
    &main_screen_pixelmap_button_6_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_prompt_2), /* control block */
    (void *) &main_screen_prompt_2_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET main_screen_lower_dot_define =
{
    "lower_dot",
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
    {281, 176, 287, 182},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_lower_dot), /* control block */
    (void *) &main_screen_lower_dot_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_upper_dot_define =
{
    "upper_dot",
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
    {281, 135, 287, 141},                    /* widget size                    */
    &main_screen_lower_dot_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_upper_dot), /* control block */
    (void *) &main_screen_upper_dot_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET main_screen_minute_define =
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
    {290, 109, 449, 203},                    /* widget size                    */
    &main_screen_upper_dot_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_minute), /* control block  */
    (void *) &main_screen_minute_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_hour_define =
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
    {117, 109, 276, 203},                    /* widget size                    */
    &main_screen_minute_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_hour), /* control block    */
    (void *) &main_screen_hour_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_date_define =
{
    "date",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {213, 224, 429, 246},                    /* widget size                    */
    &main_screen_hour_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_date), /* control block    */
    (void *) &main_screen_date_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_am_pm_define =
{
    "am_pm",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {447, 114, 490, 144},                    /* widget size                    */
    &main_screen_date_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_am_pm), /* control block   */
    (void *) &main_screen_am_pm_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_time_window_define =
{
    "time_window",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {89, 98, 506, 257},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_am_pm_define,               /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_time_window), /* control block */
    (void *) &main_screen_time_window_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_locks_define =
{
    "locks",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_LOCKS,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 290, 450, 410},                    /* widget size                    */
    &main_screen_time_window_define,         /* next widget definition         */
    &main_screen_prompt_2_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_locks), /* control block   */
    (void *) &main_screen_locks_properties   /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_weather_define =
{
    "weather",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_WEATHER,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {456, 290, 582, 410},                    /* widget size                    */
    &main_screen_locks_define,               /* next widget definition         */
    &main_screen_prompt_3_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_weather), /* control block */
    (void *) &main_screen_weather_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_thermostat_define =
{
    "thermostat",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_THERMOSTAT,                           /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {191, 290, 317, 410},                    /* widget size                    */
    &main_screen_weather_define,             /* next widget definition         */
    &main_screen_prompt_1_define,            /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_thermostat), /* control block */
    (void *) &main_screen_thermostat_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET main_screen_lights_define =
{
    "lights",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_LIGHTS,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_TEXT,                        /* normal color id                */
    GX_COLOR_ID_TEXT,                        /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {60, 290, 186, 410},                     /* widget size                    */
    &main_screen_thermostat_define,          /* next widget definition         */
    &main_screen_prompt_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_lights), /* control block  */
    (void *) &main_screen_lights_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_home_window_define =
{
    "home_window",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {44, 90, 593, 418},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &main_screen_lights_define,              /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_home_window), /* control block */
    (void *) &main_screen_home_window_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET main_screen_icon_define =
{
    "icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {18, 18, 131, 76},                       /* widget size                    */
    &main_screen_home_window_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_icon), /* control block    */
    (void *) &main_screen_icon_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET main_screen_window_define =
{
    "window",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {448, 9, 632, 88},                       /* widget size                    */
    &main_screen_icon_define,                /* next widget definition         */
    &main_screen_weather_text_define,        /* child widget definition        */
    offsetof(MAIN_SCREEN_CONTROL_BLOCK, main_screen_window), /* control block  */
    (void *) &main_screen_window_properties  /* extended properties            */
};

GX_ANIMATION_INFO main_screen_animation_1 = {
    (GX_WIDGET *) &main_screen.main_screen_home_window,
    (GX_WIDGET *) &main_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, 0, 0, 1,
    {44, 90}, {44, 90}, 255, 0, 3
};


GX_ANIMATION_INFO main_screen_animation_2 = {
    (GX_WIDGET *) &main_screen.main_screen_home_window,
    (GX_WIDGET *) &main_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {44, 90}, {44, 90}, 0, 255, 3
};


GX_STUDIO_ACTION main_screen_on_user_event_fade_out_home_windowactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen, &main_screen.main_screen_home_window, &main_screen_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};


GX_STUDIO_ACTION main_screen_on_user_event_fade_in_home_windowactions[2] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &main_screen, &main_screen.main_screen_home_window, &main_screen_animation_2},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_main_screen_event_table[] = {
    { USER_EVENT_FADE_OUT_HOME_WINDOW, 0, main_screen_on_user_event_fade_out_home_windowactions},
    { USER_EVENT_FADE_IN_HOME_WINDOW, 0, main_screen_on_user_event_fade_in_home_windowactions},
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
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MAIN_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) main_screen_draw, /* drawing function override     */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_main_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &main_screen_window_define,              /* child widget                   */
    0,                                       /* control block                  */
    (void *) &main_screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES thermostat_base_properties =
{
    &controller_base_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {19, 92, 315, 386}                       /* widget size                    */
};
GX_ICON_PROPERTIES thermostat_base_ruller_thermostat_properties =
{
    GX_PIXELMAP_ID_RULLER_THERMOSTAT,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_properties =
{
    GX_STRING_ID_STRING_55,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_1_properties =
{
    GX_STRING_ID_STRING_57,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_2_properties =
{
    GX_STRING_ID_STRING_56,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_3_properties =
{
    GX_STRING_ID_STRING_58,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_4_properties =
{
    GX_STRING_ID_STRING_59,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_5_properties =
{
    GX_STRING_ID_STRING_61,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_6_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_7_properties =
{
    GX_STRING_ID_STRING_62,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_16_8_properties =
{
    GX_STRING_ID_STRING_63,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_RADIAL_SLIDER_PROPERTIES thermostat_base_radial_slider_properties =
{
    95,                                      /* xcenter                        */
    92,                                      /* ycenter                        */
    78,                                      /* radius                         */
    22,                                      /* track width                    */
    230,                                     /* current angle                  */
    -50,                                     /* min angle                      */
    230,                                     /* max angle                      */
    GX_PIXELMAP_ID_RULLER_BACKGROUND,        /* background pixelmap            */
    GX_PIXELMAP_ID_CIRCULAR_DIAL,            /* needle pixelmap                */
    15,                                      /* animation total steps          */
    2,                                       /* animation delay                */
    GX_ANIMATION_CIRC_EASE_IN_OUT,           /* animation style                */
    thermostat_radial_slider_value_update,   /* animation update callback func */
};
GX_PROMPT_PROPERTIES thermostat_base_cold_heat_text_properties =
{
    GX_STRING_ID_STRING_52,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_current_label_properties =
{
    GX_STRING_ID_STRING_53,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES thermostat_base_center_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    40                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES thermostat_base_dot_lable_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_base_prompt_15_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES thermostat_base_cold_heat_icon_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_HEAT,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES thermostat_base_bottom_value_properties =
{
    GX_STRING_ID_STRING_54,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES thermostat_base_button_close_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_POWER,           /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_POWER_ACTIVE,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES thermostat_base_button_open_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_POWER_LG,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_POWER_LG,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_8_define =
{
    "prompt_16_8",
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
    {223, 322, 240, 331},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_8), /* control block */
    (void *) &thermostat_base_prompt_16_8_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_7_define =
{
    "prompt_16_7",
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
    {252, 274, 269, 283},                    /* widget size                    */
    &thermostat_base_prompt_16_8_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_7), /* control block */
    (void *) &thermostat_base_prompt_16_7_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_6_define =
{
    "prompt_16_6",
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
    {218, 163, 229, 172},                    /* widget size                    */
    &thermostat_base_prompt_16_7_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_6), /* control block */
    (void *) &thermostat_base_prompt_16_6_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_5_define =
{
    "prompt_16_5",
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
    {250, 213, 267, 222},                    /* widget size                    */
    &thermostat_base_prompt_16_6_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_5), /* control block */
    (void *) &thermostat_base_prompt_16_5_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_4_define =
{
    "prompt_16_4",
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
    {160, 144, 171, 153},                    /* widget size                    */
    &thermostat_base_prompt_16_5_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_4), /* control block */
    (void *) &thermostat_base_prompt_16_4_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_3_define =
{
    "prompt_16_3",
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
    {102, 163, 113, 172},                    /* widget size                    */
    &thermostat_base_prompt_16_4_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_3), /* control block */
    (void *) &thermostat_base_prompt_16_3_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_2_define =
{
    "prompt_16_2",
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
    {66, 211, 77, 220},                      /* widget size                    */
    &thermostat_base_prompt_16_3_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_2), /* control block */
    (void *) &thermostat_base_prompt_16_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_1_define =
{
    "prompt_16_1",
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
    {64, 273, 75, 282},                      /* widget size                    */
    &thermostat_base_prompt_16_2_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16_1), /* control block */
    (void *) &thermostat_base_prompt_16_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_16_define =
{
    "prompt_16",
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
    {97, 321, 108, 330},                     /* widget size                    */
    &thermostat_base_prompt_16_1_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_16), /* control block */
    (void *) &thermostat_base_prompt_16_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_bottom_value_define =
{
    "bottom_value",
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
    {166, 339, 211, 351},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_bottom_value), /* control block */
    (void *) &thermostat_base_bottom_value_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_cold_heat_icon_define =
{
    "cold_heat_icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {157, 290, 177, 308},                    /* widget size                    */
    &thermostat_base_bottom_value_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_cold_heat_icon), /* control block */
    (void *) &thermostat_base_cold_heat_icon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_prompt_15_define =
{
    "prompt_15",
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
    {204, 334, 209, 343},                    /* widget size                    */
    &thermostat_base_cold_heat_icon_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_prompt_15), /* control block */
    (void *) &thermostat_base_prompt_15_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_dot_lable_define =
{
    "dot_lable",
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
    {200, 220, 212, 242},                    /* widget size                    */
    &thermostat_base_prompt_15_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_dot_lable), /* control block */
    (void *) &thermostat_base_dot_lable_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_center_value_define =
{
    "center_value",
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
    {120, 224, 215, 275},                    /* widget size                    */
    &thermostat_base_dot_lable_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_center_value), /* control block */
    (void *) &thermostat_base_center_value_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_current_label_define =
{
    "current_label",
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
    {124, 338, 169, 350},                    /* widget size                    */
    &thermostat_base_center_value_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_current_label), /* control block */
    (void *) &thermostat_base_current_label_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_cold_heat_text_define =
{
    "cold_heat_text",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {125, 318, 206, 330},                    /* widget size                    */
    &thermostat_base_current_label_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_cold_heat_text), /* control block */
    (void *) &thermostat_base_cold_heat_text_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_button_open_define =
{
    "button_open",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_OPEN,                                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_PLUS_NORMAL,                 /* normal color id                */
    GX_COLOR_ID_PLUS_SELECTED,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {116, 178, 221, 301},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_button_open), /* control block */
    (void *) &thermostat_base_button_open_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_button_close_define =
{
    "button_close",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CLOSE,                                /* widget id                      */
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
    {279, 347, 301, 374},                    /* widget size                    */
    &thermostat_base_button_open_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_button_close), /* control block */
    (void *) &thermostat_base_button_close_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_radial_slider_define =
{
    "radial_slider",
    GX_TYPE_RADIAL_SLIDER,                   /* widget type                    */
    ID_RADIAL_SLIDER,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_SLIDER),                /* control block size             */
    GX_COLOR_ID_RULLER_BG,                   /* normal color id                */
    GX_COLOR_ID_RULLER_BG,                   /* selected color id              */
    GX_COLOR_ID_RULLER_BG,                   /* disabled color id              */
    gx_studio_radial_slider_create,          /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_radial_slider_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {72, 156, 260, 356},                     /* widget size                    */
    &thermostat_base_button_close_define,    /* next widget definition         */
    &thermostat_base_cold_heat_text_define,  /* child widget definition        */
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_radial_slider), /* control block */
    (void *) &thermostat_base_radial_slider_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_ruller_thermostat_define =
{
    "ruller_thermostat",
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
    {36, 118, 297, 352},                     /* widget size                    */
    &thermostat_base_radial_slider_define,   /* next widget definition         */
    &thermostat_base_prompt_16_define,       /* child widget definition        */
    offsetof(THERMOSTAT_BASE_CONTROL_BLOCK, thermostat_base_ruller_thermostat), /* control block */
    (void *) &thermostat_base_ruller_thermostat_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_base_define =
{
    "thermostat_base",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) thermostat_base_event_process, /* event function override */
    {19, 92, 315, 386},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &thermostat_base_ruller_thermostat_define, /* child widget                 */
    0,                                       /* control block                  */
    (void *) &thermostat_base_properties     /* extended properties            */
};
GX_TEMPLATE_PROPERTIES thermostat_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_RADIO_BUTTON_PROPERTIES thermostat_screen_pagination_1_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES thermostat_screen_pagination_2_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES thermostat_screen_pagination_3_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES thermostat_screen_pagination_4_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_WINDOW_PROPERTIES thermostat_screen_info_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES thermostat_screen_on_lable_properties =
{
    GX_STRING_ID_STRING_65,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES thermostat_screen_icon_properties =
{
    GX_PIXELMAP_ID_FOOTER_NUMBER_CIRCLE_ACTIVE, /* normal pixelmap id          */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES thermostat_screen_num_on_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    7                                        /* numeric prompt value           */
};
GX_ICON_PROPERTIES thermostat_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_FOOTER_NUMBER_CIRCLE,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES thermostat_screen_num_off_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED,                    /* disabled text color            */
    GX_NULL,                                 /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES thermostat_screen_off_lable_properties =
{
    GX_STRING_ID_STRING_66,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED                     /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_screen_kwh_lable_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_screen_kwh_lable_2_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES thermostat_screen_overall_today_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    power_value_format,                      /* format function                */
    8746594                                  /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES thermostat_screen_energy_in_use_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    power_value_format,                      /* format function                */
    68361                                    /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES thermostat_screen_energy_in_use_lable_properties =
{
    GX_STRING_ID_STRING_67,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES thermostat_screen_overall_today_lable_properties =
{
    GX_STRING_ID_STRING_68,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_num_on_define =
{
    "num_on",
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
    {427, 397, 434, 409},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_num_on), /* control block */
    (void *) &thermostat_screen_num_on_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_num_off_define =
{
    "num_off",
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
    {482, 397, 489, 409},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_num_off), /* control block */
    (void *) &thermostat_screen_num_off_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_overall_today_lable_define =
{
    "overall_today_lable",
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
    {326, 451, 424, 463},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_overall_today_lable), /* control block */
    (void *) &thermostat_screen_overall_today_lable_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_energy_in_use_lable_define =
{
    "energy_in_use_lable",
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
    {346, 432, 440, 444},                    /* widget size                    */
    &thermostat_screen_overall_today_lable_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_energy_in_use_lable), /* control block */
    (void *) &thermostat_screen_energy_in_use_lable_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_energy_in_use_define =
{
    "energy_in_use",
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
    {450, 430, 500, 447},                    /* widget size                    */
    &thermostat_screen_energy_in_use_lable_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_energy_in_use), /* control block */
    (void *) &thermostat_screen_energy_in_use_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_overall_today_define =
{
    "overall_today",
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
    {432, 449, 500, 466},                    /* widget size                    */
    &thermostat_screen_energy_in_use_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_overall_today), /* control block */
    (void *) &thermostat_screen_overall_today_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_kwh_lable_2_define =
{
    "kwh_lable_2",
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
    {504, 451, 528, 463},                    /* widget size                    */
    &thermostat_screen_overall_today_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_kwh_lable_2), /* control block */
    (void *) &thermostat_screen_kwh_lable_2_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_kwh_lable_define =
{
    "kwh_lable",
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
    {503, 432, 527, 444},                    /* widget size                    */
    &thermostat_screen_kwh_lable_2_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_kwh_lable), /* control block */
    (void *) &thermostat_screen_kwh_lable_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_off_lable_define =
{
    "off_lable",
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
    {500, 398, 522, 410},                    /* widget size                    */
    &thermostat_screen_kwh_lable_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_off_lable), /* control block */
    (void *) &thermostat_screen_off_lable_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_icon_1_define =
{
    "icon_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {474, 393, 495, 413},                    /* widget size                    */
    &thermostat_screen_off_lable_define,     /* next widget definition         */
    &thermostat_screen_num_off_define,       /* child widget definition        */
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_icon_1), /* control block */
    (void *) &thermostat_screen_icon_1_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_icon_define =
{
    "icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {420, 393, 441, 413},                    /* widget size                    */
    &thermostat_screen_icon_1_define,        /* next widget definition         */
    &thermostat_screen_num_on_define,        /* child widget definition        */
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_icon), /* control block */
    (void *) &thermostat_screen_icon_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_on_lable_define =
{
    "on_lable",
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
    {446, 398, 463, 410},                    /* widget size                    */
    &thermostat_screen_icon_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_on_lable), /* control block */
    (void *) &thermostat_screen_on_lable_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_info_win_define =
{
    "info_win",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {323, 387, 528, 469},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &thermostat_screen_on_lable_define,      /* child widget definition        */
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_info_win), /* control block */
    (void *) &thermostat_screen_info_win_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_pagination_4_define =
{
    "pagination_4",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_4,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {353, 68, 368, 83},                      /* widget size                    */
    &thermostat_screen_info_win_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_pagination_4), /* control block */
    (void *) &thermostat_screen_pagination_4_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_pagination_3_define =
{
    "pagination_3",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_3,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {328, 68, 343, 83},                      /* widget size                    */
    &thermostat_screen_pagination_4_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_pagination_3), /* control block */
    (void *) &thermostat_screen_pagination_3_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_pagination_2_define =
{
    "pagination_2",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_2,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {302, 68, 317, 83},                      /* widget size                    */
    &thermostat_screen_pagination_3_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_pagination_2), /* control block */
    (void *) &thermostat_screen_pagination_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_pagination_1_define =
{
    "pagination_1",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_1,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {276, 68, 291, 83},                      /* widget size                    */
    &thermostat_screen_pagination_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_SCREEN_CONTROL_BLOCK, thermostat_screen_pagination_1), /* control block */
    (void *) &thermostat_screen_pagination_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_screen_define =
{
    "thermostat_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_THERMOSTAT_SCREEN,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_SCREEN_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &thermostat_screen_pagination_1_define,  /* child widget                   */
    0,                                       /* control block                  */
    (void *) &thermostat_screen_properties   /* extended properties            */
};
GX_TEMPLATE_PROPERTIES lights_base_properties =
{
    &controller_base_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_PIXELMAP_BUTTON_PROPERTIES lights_base_button_open_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_POWER_LG,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_POWER_LG,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES lights_base_icon_ruller_properties =
{
    GX_PIXELMAP_ID_RULLER_LIGHTS,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_properties =
{
    GX_STRING_ID_STRING_1,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_2_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_3_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_4_properties =
{
    GX_STRING_ID_STRING_55,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_5_properties =
{
    GX_STRING_ID_STRING_57,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_6_properties =
{
    GX_STRING_ID_STRING_56,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_7_properties =
{
    GX_STRING_ID_STRING_58,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_8_properties =
{
    GX_STRING_ID_STRING_59,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_9_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_10_properties =
{
    GX_STRING_ID_STRING_61,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_base_prompt_16_1_properties =
{
    GX_STRING_ID_STRING_80,                  /* string id                      */
    GX_FONT_ID_TINY,                         /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES lights_base_button_close_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_POWER,           /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_POWER_ACTIVE,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_RADIAL_SLIDER_PROPERTIES lights_base_radial_slider_properties =
{
    95,                                      /* xcenter                        */
    92,                                      /* ycenter                        */
    78,                                      /* radius                         */
    22,                                      /* track width                    */
    230,                                     /* current angle                  */
    -50,                                     /* min angle                      */
    230,                                     /* max angle                      */
    GX_PIXELMAP_ID_RULLER_BACKGROUND,        /* background pixelmap            */
    GX_PIXELMAP_ID_CIRCULAR_DIAL,            /* needle pixelmap                */
    15,                                      /* animation total steps          */
    2,                                       /* animation delay                */
    GX_ANIMATION_CIRC_EASE_IN_OUT,           /* animation style                */
    radial_slider_value_update,              /* animation update callback func */
};
GX_PROMPT_PROPERTIES lights_base_color_text_properties =
{
    GX_STRING_ID_WARM_WHITE,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES lights_base_power_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    power_value_format,                      /* format function                */
    104596                                   /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES lights_base_power_lable_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES lights_base_power_percent_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BIG,                          /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    8                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES lights_base_precent_lable_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_1_define =
{
    "prompt_16_1",
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
    {100, 320, 105, 329},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_1), /* control block */
    (void *) &lights_base_prompt_16_1_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_10_define =
{
    "prompt_16_10",
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
    {223, 318, 240, 327},                    /* widget size                    */
    &lights_base_prompt_16_1_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_10), /* control block */
    (void *) &lights_base_prompt_16_10_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_9_define =
{
    "prompt_16_9",
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
    {253, 282, 264, 291},                    /* widget size                    */
    &lights_base_prompt_16_10_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_9), /* control block */
    (void *) &lights_base_prompt_16_9_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_8_define =
{
    "prompt_16_8",
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
    {259, 233, 270, 242},                    /* widget size                    */
    &lights_base_prompt_16_9_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_8), /* control block */
    (void *) &lights_base_prompt_16_8_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_7_define =
{
    "prompt_16_7",
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
    {243, 185, 254, 194},                    /* widget size                    */
    &lights_base_prompt_16_8_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_7), /* control block */
    (void *) &lights_base_prompt_16_7_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_6_define =
{
    "prompt_16_6",
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
    {208, 155, 219, 164},                    /* widget size                    */
    &lights_base_prompt_16_7_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_6), /* control block */
    (void *) &lights_base_prompt_16_6_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_5_define =
{
    "prompt_16_5",
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
    {163, 145, 174, 154},                    /* widget size                    */
    &lights_base_prompt_16_6_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_5), /* control block */
    (void *) &lights_base_prompt_16_5_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_4_define =
{
    "prompt_16_4",
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
    {117, 154, 128, 166},                    /* widget size                    */
    &lights_base_prompt_16_5_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_4), /* control block */
    (void *) &lights_base_prompt_16_4_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_3_define =
{
    "prompt_16_3",
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
    {79, 186, 90, 195},                      /* widget size                    */
    &lights_base_prompt_16_4_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_3), /* control block */
    (void *) &lights_base_prompt_16_3_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_2_define =
{
    "prompt_16_2",
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
    {62, 232, 73, 241},                      /* widget size                    */
    &lights_base_prompt_16_3_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16_2), /* control block */
    (void *) &lights_base_prompt_16_2_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_prompt_16_define =
{
    "prompt_16",
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
    {71, 279, 82, 288},                      /* widget size                    */
    &lights_base_prompt_16_2_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_prompt_16), /* control block */
    (void *) &lights_base_prompt_16_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET lights_base_precent_lable_define =
{
    "precent_lable",
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
    {185, 224, 205, 246},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_precent_lable), /* control block */
    (void *) &lights_base_precent_lable_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET lights_base_power_percent_value_define =
{
    "power_percent_value",
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
    {119, 219, 214, 270},                    /* widget size                    */
    &lights_base_precent_lable_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_power_percent_value), /* control block */
    (void *) &lights_base_power_percent_value_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET lights_base_power_lable_define =
{
    "power_lable",
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
    {185, 337, 209, 349},                    /* widget size                    */
    &lights_base_power_percent_value_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_power_lable), /* control block */
    (void *) &lights_base_power_lable_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_power_value_define =
{
    "power_value",
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
    {128, 333, 181, 350},                    /* widget size                    */
    &lights_base_power_lable_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_power_value), /* control block */
    (void *) &lights_base_power_value_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_color_text_define =
{
    "color_text",
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
    {127, 316, 208, 328},                    /* widget size                    */
    &lights_base_power_value_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_color_text), /* control block */
    (void *) &lights_base_color_text_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET lights_base_radial_slider_define =
{
    "radial_slider",
    GX_TYPE_RADIAL_SLIDER,                   /* widget type                    */
    ID_RADIAL_SLIDER,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIAL_SLIDER),                /* control block size             */
    GX_COLOR_ID_RULLER_BG,                   /* normal color id                */
    GX_COLOR_ID_RULLER_BG,                   /* selected color id              */
    GX_COLOR_ID_RULLER_BG,                   /* disabled color id              */
    gx_studio_radial_slider_create,          /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_radial_slider_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {74, 154, 262, 354},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &lights_base_color_text_define,          /* child widget definition        */
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_radial_slider), /* control block */
    (void *) &lights_base_radial_slider_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET lights_base_button_close_define =
{
    "button_close",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CLOSE,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_PLUS_NORMAL,                 /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {278, 344, 300, 371},                    /* widget size                    */
    &lights_base_radial_slider_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_button_close), /* control block */
    (void *) &lights_base_button_close_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET lights_base_icon_ruller_define =
{
    "icon_ruller",
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
    {37, 115, 298, 349},                     /* widget size                    */
    &lights_base_button_close_define,        /* next widget definition         */
    &lights_base_prompt_16_define,           /* child widget definition        */
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_icon_ruller), /* control block */
    (void *) &lights_base_icon_ruller_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_button_open_define =
{
    "button_open",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_OPEN,                                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_PLUS_NORMAL,                 /* normal color id                */
    GX_COLOR_ID_PLUS_SELECTED,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {116, 176, 221, 299},                    /* widget size                    */
    &lights_base_icon_ruller_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_BASE_CONTROL_BLOCK, lights_base_button_open), /* control block */
    (void *) &lights_base_button_open_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_base_define =
{
    "lights_base",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) lights_base_event_process, /* event function override */
    {20, 90, 316, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &lights_base_button_open_define,         /* child widget                   */
    0,                                       /* control block                  */
    (void *) &lights_base_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES lights_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_RADIO_BUTTON_PROPERTIES lights_screen_pagination_1_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES lights_screen_pagination_2_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES lights_screen_pagination_3_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES lights_screen_pagination_4_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_WINDOW_PROPERTIES lights_screen_info_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES lights_screen_on_lable_properties =
{
    GX_STRING_ID_STRING_65,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ICON_PROPERTIES lights_screen_icon_properties =
{
    GX_PIXELMAP_ID_FOOTER_NUMBER_CIRCLE_ACTIVE, /* normal pixelmap id          */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES lights_screen_num_on_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_NULL,                                 /* format function                */
    7                                        /* numeric prompt value           */
};
GX_ICON_PROPERTIES lights_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_FOOTER_NUMBER_CIRCLE,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES lights_screen_num_off_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED,                    /* disabled text color            */
    GX_NULL,                                 /* format function                */
    0                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES lights_screen_off_lable_properties =
{
    GX_STRING_ID_STRING_66,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED                     /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_screen_kwh_lable_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_screen_kwh_lable_2_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES lights_screen_overall_today_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    power_value_format,                      /* format function                */
    8746594                                  /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES lights_screen_energy_in_use_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    power_value_format,                      /* format function                */
    68361                                    /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES lights_screen_energy_in_use_lable_properties =
{
    GX_STRING_ID_STRING_67,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PROMPT_PROPERTIES lights_screen_overall_today_lable_properties =
{
    GX_STRING_ID_STRING_68,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_num_on_define =
{
    "num_on",
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
    {427, 398, 434, 410},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_num_on), /* control block */
    (void *) &lights_screen_num_on_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_num_off_define =
{
    "num_off",
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
    {481, 398, 488, 410},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_num_off), /* control block */
    (void *) &lights_screen_num_off_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_overall_today_lable_define =
{
    "overall_today_lable",
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
    {326, 451, 424, 463},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_overall_today_lable), /* control block */
    (void *) &lights_screen_overall_today_lable_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_energy_in_use_lable_define =
{
    "energy_in_use_lable",
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
    {346, 432, 440, 444},                    /* widget size                    */
    &lights_screen_overall_today_lable_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_energy_in_use_lable), /* control block */
    (void *) &lights_screen_energy_in_use_lable_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_energy_in_use_define =
{
    "energy_in_use",
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
    {450, 430, 500, 447},                    /* widget size                    */
    &lights_screen_energy_in_use_lable_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_energy_in_use), /* control block */
    (void *) &lights_screen_energy_in_use_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_overall_today_define =
{
    "overall_today",
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
    {432, 449, 500, 466},                    /* widget size                    */
    &lights_screen_energy_in_use_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_overall_today), /* control block */
    (void *) &lights_screen_overall_today_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_kwh_lable_2_define =
{
    "kwh_lable_2",
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
    {504, 451, 528, 463},                    /* widget size                    */
    &lights_screen_overall_today_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_kwh_lable_2), /* control block */
    (void *) &lights_screen_kwh_lable_2_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_kwh_lable_define =
{
    "kwh_lable",
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
    {503, 432, 527, 444},                    /* widget size                    */
    &lights_screen_kwh_lable_2_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_kwh_lable), /* control block */
    (void *) &lights_screen_kwh_lable_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_off_lable_define =
{
    "off_lable",
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
    {500, 398, 522, 410},                    /* widget size                    */
    &lights_screen_kwh_lable_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_off_lable), /* control block */
    (void *) &lights_screen_off_lable_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_icon_1_define =
{
    "icon_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {474, 393, 495, 413},                    /* widget size                    */
    &lights_screen_off_lable_define,         /* next widget definition         */
    &lights_screen_num_off_define,           /* child widget definition        */
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_icon_1), /* control block */
    (void *) &lights_screen_icon_1_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_icon_define =
{
    "icon",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {420, 393, 441, 413},                    /* widget size                    */
    &lights_screen_icon_1_define,            /* next widget definition         */
    &lights_screen_num_on_define,            /* child widget definition        */
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_icon), /* control block */
    (void *) &lights_screen_icon_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_on_lable_define =
{
    "on_lable",
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
    {446, 398, 463, 410},                    /* widget size                    */
    &lights_screen_icon_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_on_lable), /* control block */
    (void *) &lights_screen_on_lable_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_info_win_define =
{
    "info_win",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {323, 387, 528, 469},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &lights_screen_on_lable_define,          /* child widget definition        */
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_info_win), /* control block */
    (void *) &lights_screen_info_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_pagination_4_define =
{
    "pagination_4",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_4,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {353, 68, 368, 83},                      /* widget size                    */
    &lights_screen_info_win_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_pagination_4), /* control block */
    (void *) &lights_screen_pagination_4_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_pagination_3_define =
{
    "pagination_3",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_3,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {328, 68, 343, 83},                      /* widget size                    */
    &lights_screen_pagination_4_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_pagination_3), /* control block */
    (void *) &lights_screen_pagination_3_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_pagination_2_define =
{
    "pagination_2",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_2,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {302, 68, 317, 83},                      /* widget size                    */
    &lights_screen_pagination_3_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_pagination_2), /* control block */
    (void *) &lights_screen_pagination_2_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_pagination_1_define =
{
    "pagination_1",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_1,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {276, 68, 291, 83},                      /* widget size                    */
    &lights_screen_pagination_2_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_SCREEN_CONTROL_BLOCK, lights_screen_pagination_1), /* control block */
    (void *) &lights_screen_pagination_1_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET lights_screen_define =
{
    "lights_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_LIGHTS_SCREEN,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_SCREEN_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &lights_screen_pagination_1_define,      /* child widget                   */
    0,                                       /* control block                  */
    (void *) &lights_screen_properties       /* extended properties            */
};
GX_TEMPLATE_PROPERTIES locks_door_base_properties =
{
    &controller_base_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {323, 89, 621, 385}                      /* widget size                    */
};
GX_ICON_PROPERTIES locks_door_base_status_icon_properties =
{
    GX_PIXELMAP_ID_LED_GREEN,                /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES locks_door_base_status_text_properties =
{
    GX_STRING_ID_STRING_47,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES locks_door_base_button_lock_properties =
{
    GX_PIXELMAP_ID_LOCKS_ROUND_BTN_BIG_ACTIVE, /* normal pixelmap id           */
    GX_PIXELMAP_ID_LOCKS_ROUND_BTN_BIG_ACTIVE_HOVER, /* selected pixelmap id   */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES locks_door_base_lock_icon_properties =
{
    GX_PIXELMAP_ID_LOCKS_ICON_BTN_CLOSED_LOCK, /* normal pixelmap id           */
    GX_PIXELMAP_ID_LOCKS_ICON_BTN_CLOSED_LOCK_HOVER  /* selected pixelmap id   */
};
GX_PROMPT_PROPERTIES locks_door_base_lock_text_properties =
{
    GX_STRING_ID_LOCKED,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_BLUE,                        /* normal text color              */
    GX_COLOR_ID_BLUE,                        /* selected text color            */
    GX_COLOR_ID_BLUE                         /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES locks_door_base_button_close_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_POWER,           /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_POWER_ACTIVE,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES locks_door_base_button_open_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_POWER_LG,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_POWER_LG,        /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_lock_text_define =
{
    "lock_text",
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
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {430, 262, 514, 279},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_lock_text), /* control block */
    (void *) &locks_door_base_lock_text_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_lock_icon_define =
{
    "lock_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {438, 185, 506, 251},                    /* widget size                    */
    &locks_door_base_lock_text_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_lock_icon), /* control block */
    (void *) &locks_door_base_lock_icon_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_button_open_define =
{
    "button_open",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_OPEN,                                 /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_PLUS_NORMAL,                 /* normal color id                */
    GX_COLOR_ID_PLUS_SELECTED,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {419, 175, 524, 298},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_button_open), /* control block */
    (void *) &locks_door_base_button_open_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_button_close_define =
{
    "button_close",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_CLOSE,                                /* widget id                      */
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
    {582, 345, 604, 372},                    /* widget size                    */
    &locks_door_base_button_open_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_button_close), /* control block */
    (void *) &locks_door_base_button_close_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_button_lock_define =
{
    "button_lock",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_LOCK_BUTTON,                          /* widget id                      */
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
    {376, 151, 561, 336},                    /* widget size                    */
    &locks_door_base_button_close_define,    /* next widget definition         */
    &locks_door_base_lock_icon_define,       /* child widget definition        */
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_button_lock), /* control block */
    (void *) &locks_door_base_button_lock_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_status_text_define =
{
    "status_text",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_STATUS,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_COPY|GX_STYLE_TEXT_LEFT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {362, 134, 421, 146},                    /* widget size                    */
    &locks_door_base_button_lock_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_status_text), /* control block */
    (void *) &locks_door_base_status_text_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_status_icon_define =
{
    "status_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    ID_STATUS,                               /* widget id                      */
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
    {340, 129, 359, 148},                    /* widget size                    */
    &locks_door_base_status_text_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_DOOR_BASE_CONTROL_BLOCK, locks_door_base_status_icon), /* control block */
    (void *) &locks_door_base_status_icon_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET locks_door_base_define =
{
    "locks_door_base",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(LOCKS_DOOR_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) locks_door_base_event_process, /* event function override */
    {323, 89, 621, 385},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &locks_door_base_status_icon_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &locks_door_base_properties     /* extended properties            */
};
GX_TEMPLATE_PROPERTIES locks_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_RADIO_BUTTON_PROPERTIES locks_screen_pagination_1_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES locks_screen_pagination_2_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES locks_screen_pagination_3_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};

GX_CONST GX_STUDIO_WIDGET locks_screen_pagination_3_define =
{
    "pagination_3",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_3,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {341, 68, 356, 83},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_SCREEN_CONTROL_BLOCK, locks_screen_pagination_3), /* control block */
    (void *) &locks_screen_pagination_3_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET locks_screen_pagination_2_define =
{
    "pagination_2",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_2,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {315, 68, 330, 83},                      /* widget size                    */
    &locks_screen_pagination_3_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_SCREEN_CONTROL_BLOCK, locks_screen_pagination_2), /* control block */
    (void *) &locks_screen_pagination_2_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET locks_screen_pagination_1_define =
{
    "pagination_1",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_1,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 68, 304, 83},                      /* widget size                    */
    &locks_screen_pagination_2_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_SCREEN_CONTROL_BLOCK, locks_screen_pagination_1), /* control block */
    (void *) &locks_screen_pagination_1_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET locks_screen_define =
{
    "locks_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_LOCKS_SCREEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &locks_screen_pagination_1_define,       /* child widget                   */
    0,                                       /* control block                  */
    (void *) &locks_screen_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES win_San_Diego_properties =
{
    &weather_base_define,                    /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {20, 90, 630, 384}                       /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET win_San_Diego_define =
{
    "win_San_Diego",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WIN_SAN_DIEGO_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &win_San_Diego_properties       /* extended properties            */
};
GX_TEMPLATE_PROPERTIES win_New_York_properties =
{
    &weather_base_define,                    /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {20, 90, 630, 384}                       /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET win_New_York_define =
{
    "win_New_York",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WIN_NEW_YORK_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &win_New_York_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES win_Las_Vegas_properties =
{
    &weather_base_define,                    /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {20, 90, 630, 384}                       /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET win_Las_Vegas_define =
{
    "win_Las_Vegas",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WIN_LAS_VEGAS_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    GX_NULL,                                 /* child widget                   */
    0,                                       /* control block                  */
    (void *) &win_Las_Vegas_properties       /* extended properties            */
};
GX_TEMPLATE_PROPERTIES weather_screen_properties =
{
    &screen_base_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_RADIO_BUTTON_PROPERTIES weather_screen_pagination_1_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES weather_screen_pagination_2_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES weather_screen_pagination_3_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT,               /* disabled text color            */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off pixelmap id                */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE,    /* on pixelmap id                 */
    GX_PIXELMAP_ID_PAGINATION_DOT,           /* off disabled pixelmap id       */
    GX_PIXELMAP_ID_PAGINATION_DOT_ACTIVE     /* on disabled pixelmap id        */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_pagination_3_define =
{
    "pagination_3",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_3,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {341, 68, 356, 83},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_pagination_3), /* control block */
    (void *) &weather_screen_pagination_3_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_pagination_2_define =
{
    "pagination_2",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_2,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {315, 68, 330, 83},                      /* widget size                    */
    &weather_screen_pagination_3_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_pagination_2), /* control block */
    (void *) &weather_screen_pagination_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_pagination_1_define =
{
    "pagination_1",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    ID_PAGINATION_1,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_BUTTON_PUSHED|GX_STYLE_BUTTON_RADIO|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RADIO_BUTTON),                 /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_radio_button_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 68, 304, 83},                      /* widget size                    */
    &weather_screen_pagination_2_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WEATHER_SCREEN_CONTROL_BLOCK, weather_screen_pagination_1), /* control block */
    (void *) &weather_screen_pagination_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET weather_screen_define =
{
    "weather_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_WEATHER_SCREEN,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WEATHER_SCREEN_CONTROL_BLOCK),    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &weather_screen_pagination_1_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &weather_screen_properties      /* extended properties            */
};
GX_TEMPLATE_PROPERTIES add_base_properties =
{
    &controller_base_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};
GX_PIXELMAP_BUTTON_PROPERTIES add_base_add_properties =
{
    GX_PIXELMAP_ID_BOX_ICON_PLUS_LG,         /* normal pixelmap id             */
    GX_PIXELMAP_ID_BOX_ICON_PLUS_LG,         /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET add_base_add_define =
{
    "add",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_ADD,                                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_PLUS_NORMAL,                 /* normal color id                */
    GX_COLOR_ID_PLUS_SELECTED,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {405, 177, 535, 306},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(ADD_BASE_CONTROL_BLOCK, add_base_add), /* control block           */
    (void *) &add_base_add_properties        /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET add_base_define =
{
    "add_base",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(ADD_BASE_CONTROL_BLOCK),          /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &add_base_add_define,                    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &add_base_properties            /* extended properties            */
};
GX_WINDOW_PROPERTIES thermostat_page_4_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES thermostat_page_4_office_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES thermostat_page_4_add_room_properties =
{
    &add_base_define,                        /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_4_add_room_define =
{
    "add_room",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(ADD_BASE_CONTROL_BLOCK),          /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_4_CONTROL_BLOCK, thermostat_page_4_add_room), /* control block */
    (void *) &thermostat_page_4_add_room_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_4_office_define =
{
    "office",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &thermostat_page_4_add_room_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_4_CONTROL_BLOCK, thermostat_page_4_office), /* control block */
    (void *) &thermostat_page_4_office_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_4_define =
{
    "thermostat_page_4",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(THERMOSTAT_PAGE_4_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &thermostat_page_4_office_define,        /* child widget                   */
    0,                                       /* control block                  */
    (void *) &thermostat_page_4_properties   /* extended properties            */
};
GX_WINDOW_PROPERTIES thermostat_page_3_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES thermostat_page_3_dinning_room_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES thermostat_page_3_outdoor_patio_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_3_outdoor_patio_define =
{
    "outdoor_patio",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_3_CONTROL_BLOCK, thermostat_page_3_outdoor_patio), /* control block */
    (void *) &thermostat_page_3_outdoor_patio_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_3_dinning_room_define =
{
    "dinning_room",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &thermostat_page_3_outdoor_patio_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_3_CONTROL_BLOCK, thermostat_page_3_dinning_room), /* control block */
    (void *) &thermostat_page_3_dinning_room_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_3_define =
{
    "thermostat_page_3",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(THERMOSTAT_PAGE_3_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &thermostat_page_3_dinning_room_define,  /* child widget                   */
    0,                                       /* control block                  */
    (void *) &thermostat_page_3_properties   /* extended properties            */
};
GX_WINDOW_PROPERTIES thermostat_page_2_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES thermostat_page_2_kids_bedroom_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES thermostat_page_2_living_room_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_2_living_room_define =
{
    "living_room",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_2_CONTROL_BLOCK, thermostat_page_2_living_room), /* control block */
    (void *) &thermostat_page_2_living_room_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_2_kids_bedroom_define =
{
    "kids_bedroom",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &thermostat_page_2_living_room_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_2_CONTROL_BLOCK, thermostat_page_2_kids_bedroom), /* control block */
    (void *) &thermostat_page_2_kids_bedroom_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_2_define =
{
    "thermostat_page_2",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(THERMOSTAT_PAGE_2_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &thermostat_page_2_kids_bedroom_define,  /* child widget                   */
    0,                                       /* control block                  */
    (void *) &thermostat_page_2_properties   /* extended properties            */
};
GX_WINDOW_PROPERTIES thermostat_page_1_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES thermostat_page_1_kitchen_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES thermostat_page_1_master_bedroom_properties =
{
    &thermostat_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_1_master_bedroom_define =
{
    "master_bedroom",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_1_CONTROL_BLOCK, thermostat_page_1_master_bedroom), /* control block */
    (void *) &thermostat_page_1_master_bedroom_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_1_kitchen_define =
{
    "kitchen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(THERMOSTAT_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &thermostat_page_1_master_bedroom_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(THERMOSTAT_PAGE_1_CONTROL_BLOCK, thermostat_page_1_kitchen), /* control block */
    (void *) &thermostat_page_1_kitchen_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET thermostat_page_1_define =
{
    "thermostat_page_1",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(THERMOSTAT_PAGE_1_CONTROL_BLOCK), /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &thermostat_page_1_kitchen_define,       /* child widget                   */
    0,                                       /* control block                  */
    (void *) &thermostat_page_1_properties   /* extended properties            */
};
GX_WINDOW_PROPERTIES lights_page_3_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES lights_page_3_dinning_room_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES lights_page_3_outdoor_patio_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET lights_page_3_outdoor_patio_define =
{
    "outdoor_patio",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_3_CONTROL_BLOCK, lights_page_3_outdoor_patio), /* control block */
    (void *) &lights_page_3_outdoor_patio_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET lights_page_3_dinning_room_define =
{
    "dinning_room",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &lights_page_3_outdoor_patio_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_3_CONTROL_BLOCK, lights_page_3_dinning_room), /* control block */
    (void *) &lights_page_3_dinning_room_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET lights_page_3_define =
{
    "lights_page_3",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_PAGE_3_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &lights_page_3_dinning_room_define,      /* child widget                   */
    0,                                       /* control block                  */
    (void *) &lights_page_3_properties       /* extended properties            */
};
GX_WINDOW_PROPERTIES lights_page_2_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES lights_page_2_kids_bedroom_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES lights_page_2_living_room_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET lights_page_2_living_room_define =
{
    "living_room",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_2_CONTROL_BLOCK, lights_page_2_living_room), /* control block */
    (void *) &lights_page_2_living_room_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET lights_page_2_kids_bedroom_define =
{
    "kids_bedroom",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &lights_page_2_living_room_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_2_CONTROL_BLOCK, lights_page_2_kids_bedroom), /* control block */
    (void *) &lights_page_2_kids_bedroom_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET lights_page_2_define =
{
    "lights_page_2",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_PAGE_2_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &lights_page_2_kids_bedroom_define,      /* child widget                   */
    0,                                       /* control block                  */
    (void *) &lights_page_2_properties       /* extended properties            */
};
GX_WINDOW_PROPERTIES lights_page_4_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES lights_page_4_office_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES lights_page_4_add_light_properties =
{
    &add_base_define,                        /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET lights_page_4_add_light_define =
{
    "add_light",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(ADD_BASE_CONTROL_BLOCK),          /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_4_CONTROL_BLOCK, lights_page_4_add_light), /* control block */
    (void *) &lights_page_4_add_light_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET lights_page_4_office_define =
{
    "office",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &lights_page_4_add_light_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_4_CONTROL_BLOCK, lights_page_4_office), /* control block */
    (void *) &lights_page_4_office_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET lights_page_4_define =
{
    "lights_page_4",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_PAGE_4_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &lights_page_4_office_define,            /* child widget                   */
    0,                                       /* control block                  */
    (void *) &lights_page_4_properties       /* extended properties            */
};
GX_WINDOW_PROPERTIES lights_page_1_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES lights_page_1_kitchen_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES lights_page_1_master_bedroom_properties =
{
    &lights_base_define,                     /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET lights_page_1_master_bedroom_define =
{
    "master_bedroom",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_1_CONTROL_BLOCK, lights_page_1_master_bedroom), /* control block */
    (void *) &lights_page_1_master_bedroom_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET lights_page_1_kitchen_define =
{
    "kitchen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_BASE_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &lights_page_1_master_bedroom_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LIGHTS_PAGE_1_CONTROL_BLOCK, lights_page_1_kitchen), /* control block */
    (void *) &lights_page_1_kitchen_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET lights_page_1_define =
{
    "lights_page_1",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LIGHTS_PAGE_1_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &lights_page_1_kitchen_define,           /* child widget                   */
    0,                                       /* control block                  */
    (void *) &lights_page_1_properties       /* extended properties            */
};
GX_WINDOW_PROPERTIES locks_page_3_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES locks_page_3_lower_deck_door_properties =
{
    &locks_door_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_TEMPLATE_PROPERTIES locks_page_3_add_lock_properties =
{
    &add_base_define,                        /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET locks_page_3_add_lock_define =
{
    "add_lock",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(ADD_BASE_CONTROL_BLOCK),          /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_3_CONTROL_BLOCK, locks_page_3_add_lock), /* control block */
    (void *) &locks_page_3_add_lock_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET locks_page_3_lower_deck_door_define =
{
    "lower_deck_door",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_DOOR_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &locks_page_3_add_lock_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_3_CONTROL_BLOCK, locks_page_3_lower_deck_door), /* control block */
    (void *) &locks_page_3_lower_deck_door_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET locks_page_3_define =
{
    "locks_page_3",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_PAGE_3_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &locks_page_3_lower_deck_door_define,    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &locks_page_3_properties        /* extended properties            */
};
GX_WINDOW_PROPERTIES locks_page_2_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES locks_page_2_upper_deck_door_properties =
{
    &locks_door_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};
GX_TEMPLATE_PROPERTIES locks_page_2_patio_door_properties =
{
    &locks_door_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET locks_page_2_patio_door_define =
{
    "patio_door",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_DOOR_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_2_CONTROL_BLOCK, locks_page_2_patio_door), /* control block */
    (void *) &locks_page_2_patio_door_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET locks_page_2_upper_deck_door_define =
{
    "upper_deck_door",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_DOOR_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    &locks_page_2_patio_door_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_2_CONTROL_BLOCK, locks_page_2_upper_deck_door), /* control block */
    (void *) &locks_page_2_upper_deck_door_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET locks_page_2_define =
{
    "locks_page_2",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_PAGE_2_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &locks_page_2_upper_deck_door_define,    /* child widget                   */
    0,                                       /* control block                  */
    (void *) &locks_page_2_properties        /* extended properties            */
};
GX_WINDOW_PROPERTIES locks_page_1_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEMPLATE_PROPERTIES locks_page_1_security_alarm_properties =
{
    &controller_base_define,                 /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {20, 90, 316, 384}                       /* widget size                    */
};
GX_ICON_PROPERTIES locks_page_1_icon_3_properties =
{
    GX_PIXELMAP_ID_LED_GREEN,                /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES locks_page_1_prompt_15_1_properties =
{
    GX_STRING_ID_STRING_37,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES locks_page_1_button_enable_diable_properties =
{
    GX_PIXELMAP_ID_LOCKS_ROUND_BTN_BIG,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_LOCKS_ROUND_BTN_BIG_HOVER, /* selected pixelmap id          */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES locks_page_1_icon_5_properties =
{
    GX_PIXELMAP_ID_LOCKS_ICON_BTN_BECON,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_LOCKS_ICON_BTN_BECON_HOVER  /* selected pixelmap id         */
};
GX_PROMPT_PROPERTIES locks_page_1_enable_disable_text_properties =
{
    GX_STRING_ID_DISABLED,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GREEN,                       /* normal text color              */
    GX_COLOR_ID_GREEN,                       /* selected text color            */
    GX_COLOR_ID_GREEN                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES locks_page_1_button_lock_properties =
{
    GX_PIXELMAP_ID_LOCKS_ROUND_BTN_SMALL_ACTIVE, /* normal pixelmap id         */
    GX_PIXELMAP_ID_LOCKS_ROUND_BTN_SMALL,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_ICON_PROPERTIES locks_page_1_lock_unlock_icon_properties =
{
    GX_PIXELMAP_ID_LOCKS_ICON_BTN_SMALL_CLOSED_LOCK, /* normal pixelmap id     */
    GX_PIXELMAP_ID_LOCKS_ICON_BTN_SMALL_CLOSED_LOCK_HOVER  /* selected pixelmap id */
};
GX_PROMPT_PROPERTIES locks_page_1_lock_unlock_text_properties =
{
    GX_STRING_ID_UNLOCK_ALL,                 /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_BLUE,                        /* normal text color              */
    GX_COLOR_ID_BLUE,                        /* selected text color            */
    GX_COLOR_ID_BLUE                         /* disabled text color            */
};
GX_ICON_PROPERTIES locks_page_1_icon_1_properties =
{
    GX_PIXELMAP_ID_LED_YELLOW,               /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES locks_page_1_icon_2_properties =
{
    GX_PIXELMAP_ID_LED_BLACK,                /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES locks_page_1_prompt_15_5_properties =
{
    GX_STRING_ID_STRING_43,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_ML_TEXT_BUTTON_PROPERTIES locks_page_1_multi_line_button_properties =
{
    GX_STRING_ID_STRING_45,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_TEMPLATE_PROPERTIES locks_page_1_front_door_properties =
{
    &locks_door_base_define,                 /* base info                      */
    gx_studio_template_create,               /* base create function           */
    {324, 90, 620, 384}                      /* widget size                    */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_enable_disable_text_define =
{
    "enable_disable_text",
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
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {98, 263, 173, 280},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_enable_disable_text), /* control block */
    (void *) &locks_page_1_enable_disable_text_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_icon_5_define =
{
    "icon_5",
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
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {101, 187, 169, 253},                    /* widget size                    */
    &locks_page_1_enable_disable_text_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_icon_5), /* control block */
    (void *) &locks_page_1_icon_5_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_lock_unlock_text_define =
{
    "lock_unlock_text",
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
    GX_COLOR_ID_GREEN,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {203, 318, 278, 330},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_lock_unlock_text), /* control block */
    (void *) &locks_page_1_lock_unlock_text_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_lock_unlock_icon_define =
{
    "lock_unlock_icon",
    GX_TYPE_ICON,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_BLUE,                        /* normal color id                */
    GX_COLOR_ID_BLUE,                        /* selected color id              */
    GX_COLOR_ID_BLUE,                        /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {221, 276, 259, 314},                    /* widget size                    */
    &locks_page_1_lock_unlock_text_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_lock_unlock_icon), /* control block */
    (void *) &locks_page_1_lock_unlock_icon_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_multi_line_button_define =
{
    "multi_line_button",
    GX_TYPE_MULTI_LINE_TEXT_BUTTON,          /* widget type                    */
    ID_STATUS,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_RIGHT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_MULTI_LINE_TEXT_BUTTON),       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_multi_line_text_button_create,     /* create function            */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {235, 155, 298, 180},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_multi_line_button), /* control block */
    (void *) &locks_page_1_multi_line_button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_prompt_15_5_define =
{
    "prompt_15_5",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_STATUS,                               /* widget id                      */
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
    {61, 352, 155, 364},                     /* widget size                    */
    &locks_page_1_multi_line_button_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_prompt_15_5), /* control block */
    (void *) &locks_page_1_prompt_15_5_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_icon_2_define =
{
    "icon_2",
    GX_TYPE_ICON,                            /* widget type                    */
    ID_STATUS,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {35, 346, 54, 365},                      /* widget size                    */
    &locks_page_1_prompt_15_5_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_icon_2), /* control block */
    (void *) &locks_page_1_icon_2_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_icon_1_define =
{
    "icon_1",
    GX_TYPE_ICON,                            /* widget type                    */
    ID_STATUS,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {274, 130, 293, 149},                    /* widget size                    */
    &locks_page_1_icon_2_define,             /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_icon_1), /* control block */
    (void *) &locks_page_1_icon_1_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_button_lock_define =
{
    "button_lock",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BUTTON_LOCK_UNLOCK_ALL,               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {185, 254, 291, 360},                    /* widget size                    */
    &locks_page_1_icon_1_define,             /* next widget definition         */
    &locks_page_1_lock_unlock_icon_define,   /* child widget definition        */
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_button_lock), /* control block */
    (void *) &locks_page_1_button_lock_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_button_enable_diable_define =
{
    "button_enable_diable",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_BUTTON_ENABLE_DISABLE,                /* widget id                      */
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
    {42, 149, 227, 334},                     /* widget size                    */
    &locks_page_1_button_lock_define,        /* next widget definition         */
    &locks_page_1_icon_5_define,             /* child widget definition        */
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_button_enable_diable), /* control block */
    (void *) &locks_page_1_button_enable_diable_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_prompt_15_1_define =
{
    "prompt_15_1",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_STATUS,                               /* widget id                      */
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
    {59, 134, 118, 146},                     /* widget size                    */
    &locks_page_1_button_enable_diable_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_prompt_15_1), /* control block */
    (void *) &locks_page_1_prompt_15_1_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_icon_3_define =
{
    "icon_3",
    GX_TYPE_ICON,                            /* widget type                    */
    ID_STATUS,                               /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_HALIGN_LEFT|GX_STYLE_VALIGN_TOP,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_ICON),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {35, 130, 54, 149},                      /* widget size                    */
    &locks_page_1_prompt_15_1_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_icon_3), /* control block */
    (void *) &locks_page_1_icon_3_properties /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_front_door_define =
{
    "front_door",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_DOOR_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {324, 90, 620, 384},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_front_door), /* control block */
    (void *) &locks_page_1_front_door_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_security_alarm_define =
{
    "security_alarm",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(CONTROLLER_BASE_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 90, 316, 384},                      /* widget size                    */
    &locks_page_1_front_door_define,         /* next widget definition         */
    &locks_page_1_icon_3_define,             /* child widget definition        */
    offsetof(LOCKS_PAGE_1_CONTROL_BLOCK, locks_page_1_security_alarm), /* control block */
    (void *) &locks_page_1_security_alarm_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET locks_page_1_define =
{
    "locks_page_1",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(LOCKS_PAGE_1_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) locks_page_1_event_process, /* event function override */
    {20, 90, 630, 384},                      /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &locks_page_1_security_alarm_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &locks_page_1_properties        /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY demo_guix_home_automation_widget_table[] =
{
    { &thermostat_page_4_define, (GX_WIDGET *) &thermostat_page_4 },
    { &thermostat_page_3_define, (GX_WIDGET *) &thermostat_page_3 },
    { &thermostat_page_2_define, (GX_WIDGET *) &thermostat_page_2 },
    { &thermostat_page_1_define, (GX_WIDGET *) &thermostat_page_1 },
    { &thermostat_screen_define, (GX_WIDGET *) &thermostat_screen },
    { &lights_page_3_define, (GX_WIDGET *) &lights_page_3 },
    { &lights_page_2_define, (GX_WIDGET *) &lights_page_2 },
    { &lights_page_4_define, (GX_WIDGET *) &lights_page_4 },
    { &lights_page_1_define, (GX_WIDGET *) &lights_page_1 },
    { &lights_screen_define, (GX_WIDGET *) &lights_screen },
    { &locks_page_3_define, (GX_WIDGET *) &locks_page_3 },
    { &locks_page_2_define, (GX_WIDGET *) &locks_page_2 },
    { &locks_page_1_define, (GX_WIDGET *) &locks_page_1 },
    { &locks_screen_define, (GX_WIDGET *) &locks_screen },
    { &win_San_Diego_define, (GX_WIDGET *) &win_San_Diego },
    { &win_New_York_define, (GX_WIDGET *) &win_New_York },
    { &win_Las_Vegas_define, (GX_WIDGET *) &win_Las_Vegas },
    { &weather_screen_define, (GX_WIDGET *) &weather_screen },
    { &passcode_screen_define, (GX_WIDGET *) &passcode_screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = demo_guix_home_automation_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &demo_guix_home_automation_display_table[display];


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
