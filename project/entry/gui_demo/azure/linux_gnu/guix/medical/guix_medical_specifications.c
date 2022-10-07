#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "guix_medical_resources.h"
#include "guix_medical_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
SCHEDULE_SCREEN_CONTROL_BLOCK schedule_screen;
PATIENTS_SCREEN_CONTROL_BLOCK patients_screen;
VITALS_SCREEN_CONTROL_BLOCK vitals_screen;
GX_DISPLAY main_display_control_block;
GX_WINDOW_ROOT main_display_root_window;
GX_CANVAS  main_display_canvas_control_block;
ULONG      main_display_canvas_memory[307200];

extern GX_CONST GX_THEME *main_display_theme_table[];
extern GX_CONST GX_STRING *main_display_language_table[];

GX_STUDIO_DISPLAY_INFO guix_medical_display_table[1] =
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

UINT gx_studio_vertical_list_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_VERTICAL_LIST *list = (GX_VERTICAL_LIST *) control_block;
    GX_VERTICAL_LIST_PROPERTIES *props = (GX_VERTICAL_LIST_PROPERTIES *) info->properties;
    status = gx_vertical_list_create(list, info->widget_name, parent, props->total_rows,
                                     props->callback, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW *) list, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
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
GX_WINDOW_PROPERTIES template_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES template_logo_properties =
{
    GX_PIXELMAP_ID_MICROSOFT_AZURE_LOGO,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_TEXT_BUTTON_PROPERTIES template_vitals_button_properties =
{
    GX_STRING_ID_STRING_2,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES template_schedule_button_properties =
{
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_TEXT_BUTTON_PROPERTIES template_patients_button_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_LIGHT_GRAY,                  /* normal text color              */
    GX_COLOR_ID_YELLOW,                      /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET template_patients_button_define =
{
    "patients_button",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_PATIENTS,                         /* widget id                      */
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
    {559, 42, 620, 59},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_CONTROL_BLOCK, template_patients_button), /* control block */
    (void *) &template_patients_button_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET template_schedule_button_define =
{
    "schedule_button",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_SCHEDULE,                         /* widget id                      */
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
    {455, 42, 525, 59},                      /* widget size                    */
    &template_patients_button_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_CONTROL_BLOCK, template_schedule_button), /* control block */
    (void *) &template_schedule_button_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET template_vitals_button_define =
{
    "vitals_button",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    ID_BTN_VITALS,                           /* widget id                      */
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
    {381, 42, 421, 59},                      /* widget size                    */
    &template_schedule_button_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_CONTROL_BLOCK, template_vitals_button), /* control block */
    (void *) &template_vitals_button_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET template_logo_define =
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
    {20, 20, 122, 59},                       /* widget size                    */
    &template_vitals_button_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEMPLATE_CONTROL_BLOCK, template_logo), /* control block          */
    (void *) &template_logo_properties       /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET template_define =
{
    "template",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEMPLATE_CONTROL_BLOCK),          /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) template_event_handler, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &template_logo_define,                   /* child widget                   */
    0,                                       /* control block                  */
    (void *) &template_properties            /* extended properties            */
};
GX_TEMPLATE_PROPERTIES schedule_screen_properties =
{
    &template_define,                        /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES schedule_screen_month_properties =
{
    GX_STRING_ID_APRIL,                      /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES schedule_screen_schedule_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES schedule_screen_c1_week_properties =
{
    GX_STRING_ID_WEDNESDAY,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES schedule_screen_c3_week_properties =
{
    GX_STRING_ID_FRIDAY,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES schedule_screen_c2_week_properties =
{
    GX_STRING_ID_THURSDAY,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_YELLOW                       /* disabled text color            */
};
GX_PROMPT_PROPERTIES schedule_screen_c4_week_properties =
{
    GX_STRING_ID_SATURDAY,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES schedule_screen_c5_week_properties =
{
    GX_STRING_ID_SUNDAY,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES schedule_screen_c6_week_properties =
{
    GX_STRING_ID_MONDAY,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES schedule_screen_c7_week_properties =
{
    GX_STRING_ID_TUESDAY,                    /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c1_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    21                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c2_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_YELLOW,                      /* disabled text color            */
    GX_NULL,                                 /* format function                */
    22                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c3_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    23                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c4_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    24                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c5_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    25                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c6_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    26                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_c7_day_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    27                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r1_am_pm_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r1_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    7                                        /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r2_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    8                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r2_am_pm_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r3_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    9                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r3_am_pm_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r4_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r4_am_pm_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r5_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    11                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r5_am_pm_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r6_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    12                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r6_am_pm_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r7_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    1                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r7_am_pm_properties =
{
    GX_STRING_ID_STRING_48,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r8_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    2                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r8_am_pm_properties =
{
    GX_STRING_ID_STRING_48,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r9_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    3                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r9_am_pm_properties =
{
    GX_STRING_ID_STRING_48,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r10_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    4                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r10_am_pm_properties =
{
    GX_STRING_ID_STRING_48,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_r11_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    5                                        /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_r11_am_pm_properties =
{
    GX_STRING_ID_STRING_48,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_ICON_PROPERTIES schedule_screen_current_time_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_TIME_BOX,        /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_hour_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_minute_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    30                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES schedule_screen_colon_properties =
{
    GX_STRING_ID_STRING_26,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES schedule_screen_r1_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_3_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_6_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_7_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_45_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_46_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_47_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED_ON,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_93_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_94_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_95_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_98_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED_ON,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r2_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_12_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_15_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_21_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_22_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_23_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_49_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_50_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_69_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_70_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_71_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_100_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_101_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_102_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_103_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_104_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r3_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_16_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_25_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_26_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_53_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_73_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_74_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_107_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_109_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_110_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r4_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_29_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_56_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_77_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_113_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_116_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r5_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES schedule_screen_active_circle_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_CIRCLE_ACTIVE_LINE_WHITE, /* normal pixelmap id    */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_PROMPT_PROPERTIES schedule_screen_active_bar_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_SCHEDULE_ACTIVE_BAR_LEFT_RED, /* left pixelmap id           */
    GX_PIXELMAP_ID_SCHEDULE_ACTIVE_BAR_CENTER_RED, /* fill pixelmap id         */
    GX_PIXELMAP_ID_SCHEDULE_ACTIVE_BAR_RIGHT_RED, /* right pixelmap id         */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0                                        /* selected right pixelmap id     */
};
GX_PROMPT_PROPERTIES schedule_screen_patient_info_properties =
{
    GX_STRING_ID_STRING_55,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_active_ball_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED_ON,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r6_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_18_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_10_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_11_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_59_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW_ON,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_119_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW_ON,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_131_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW_ON,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r7_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_13_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_14_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_33_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_34_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_35_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_60_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_61_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_62_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_81_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_82_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_83_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_120_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_121_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_122_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_132_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_133_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_134_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r8_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_17_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_37_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_38_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_64_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_65_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_85_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_86_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_124_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_BLUE_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_125_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_136_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_137_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r9_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_20_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY,       /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREY_ON,    /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_41_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED_ON,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_67_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_68_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_89_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED,        /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_RED_ON,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_127_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_128_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_140_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN,      /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_GREEN_ON,   /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_WINDOW_PROPERTIES schedule_screen_r10_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_44_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW_ON,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES schedule_screen_pixelmap_button_92_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW,     /* normal pixelmap id             */
    GX_PIXELMAP_ID_SCHEDULE_BALL_YELLOW_ON,  /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_NUMERIC_PROMPT_PROPERTIES schedule_screen_year_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    2021                                     /* numeric prompt value           */
};
GX_ICON_PROPERTIES schedule_screen_icon_1_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_ARROW_RIGHT,     /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES schedule_screen_icon_2_properties =
{
    GX_PIXELMAP_ID_SCHEDULE_ARROW_LEFT,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_colon_define =
{
    "colon",
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
    {181, 235, 184, 252},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_colon), /* control block */
    (void *) &schedule_screen_colon_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_minute_define =
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
    {185, 236, 202, 253},                    /* widget size                    */
    &schedule_screen_colon_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_minute), /* control block */
    (void *) &schedule_screen_minute_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_hour_define =
{
    "hour",
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
    {163, 236, 180, 253},                    /* widget size                    */
    &schedule_screen_minute_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_hour), /* control block */
    (void *) &schedule_screen_hour_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_98_1_define =
{
    "pixelmap_button_98_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {598, 138, 615, 155},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_98_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_98_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_95_1_define =
{
    "pixelmap_button_95_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {521, 138, 538, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_98_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_95_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_95_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_94_1_define =
{
    "pixelmap_button_94_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {497, 138, 514, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_95_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_94_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_94_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_93_1_define =
{
    "pixelmap_button_93_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {473, 138, 490, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_94_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_93_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_93_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_47_1_define =
{
    "pixelmap_button_47_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {290, 138, 307, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_93_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_47_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_47_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_46_1_define =
{
    "pixelmap_button_46_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {266, 138, 283, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_47_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_46_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_46_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_45_1_define =
{
    "pixelmap_button_45_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {242, 138, 259, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_46_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_45_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_45_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_7_define =
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
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {135, 138, 152, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_45_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_7), /* control block */
    (void *) &schedule_screen_pixelmap_button_7_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_6_define =
{
    "pixelmap_button_6",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {111, 138, 128, 155},                    /* widget size                    */
    &schedule_screen_pixelmap_button_7_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_6), /* control block */
    (void *) &schedule_screen_pixelmap_button_6_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_3_define =
{
    "pixelmap_button_3",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {87, 138, 104, 155},                     /* widget size                    */
    &schedule_screen_pixelmap_button_6_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_3), /* control block */
    (void *) &schedule_screen_pixelmap_button_3_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_104_1_define =
{
    "pixelmap_button_104_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {597, 169, 614, 186},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_104_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_104_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_103_1_define =
{
    "pixelmap_button_103_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {573, 169, 590, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_104_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_103_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_103_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_102_1_define =
{
    "pixelmap_button_102_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {549, 169, 566, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_103_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_102_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_102_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_101_1_define =
{
    "pixelmap_button_101_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {520, 169, 537, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_102_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_101_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_101_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_100_1_define =
{
    "pixelmap_button_100_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {496, 169, 513, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_101_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_100_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_100_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_71_1_define =
{
    "pixelmap_button_71_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {442, 169, 459, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_100_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_71_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_71_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_70_1_define =
{
    "pixelmap_button_70_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 169, 435, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_71_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_70_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_70_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_69_1_define =
{
    "pixelmap_button_69_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {394, 169, 411, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_70_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_69_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_69_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_50_1_define =
{
    "pixelmap_button_50_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 169, 306, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_69_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_50_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_50_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_49_1_define =
{
    "pixelmap_button_49_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {265, 169, 282, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_50_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_49_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_49_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_23_1_define =
{
    "pixelmap_button_23_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {211, 169, 228, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_49_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_23_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_23_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_22_1_define =
{
    "pixelmap_button_22_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {187, 169, 204, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_23_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_22_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_22_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_21_1_define =
{
    "pixelmap_button_21_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {163, 169, 180, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_22_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_21_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_21_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_15_define =
{
    "pixelmap_button_15",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 169, 151, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_21_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_15), /* control block */
    (void *) &schedule_screen_pixelmap_button_15_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_12_define =
{
    "pixelmap_button_12",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {110, 169, 127, 186},                    /* widget size                    */
    &schedule_screen_pixelmap_button_15_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_12), /* control block */
    (void *) &schedule_screen_pixelmap_button_12_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_110_1_define =
{
    "pixelmap_button_110_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {597, 200, 614, 217},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_110_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_110_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_109_1_define =
{
    "pixelmap_button_109_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {573, 200, 590, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_110_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_109_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_109_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_107_1_define =
{
    "pixelmap_button_107_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {520, 200, 537, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_109_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_107_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_107_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_74_1_define =
{
    "pixelmap_button_74_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {442, 200, 459, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_107_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_74_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_74_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_73_1_define =
{
    "pixelmap_button_73_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 200, 435, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_74_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_73_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_73_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_53_1_define =
{
    "pixelmap_button_53_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 200, 306, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_73_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_53_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_53_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_26_1_define =
{
    "pixelmap_button_26_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {211, 200, 228, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_53_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_26_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_26_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_25_1_define =
{
    "pixelmap_button_25_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {187, 200, 204, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_26_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_25_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_25_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_16_define =
{
    "pixelmap_button_16",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 200, 151, 217},                    /* widget size                    */
    &schedule_screen_pixelmap_button_25_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_16), /* control block */
    (void *) &schedule_screen_pixelmap_button_16_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_116_1_define =
{
    "pixelmap_button_116_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {597, 231, 614, 248},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_116_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_116_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_113_1_define =
{
    "pixelmap_button_113_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {520, 231, 537, 248},                    /* widget size                    */
    &schedule_screen_pixelmap_button_116_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_113_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_113_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_77_1_define =
{
    "pixelmap_button_77_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {442, 231, 459, 248},                    /* widget size                    */
    &schedule_screen_pixelmap_button_113_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_77_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_77_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_56_1_define =
{
    "pixelmap_button_56_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 231, 306, 248},                    /* widget size                    */
    &schedule_screen_pixelmap_button_77_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_56_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_56_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_29_1_define =
{
    "pixelmap_button_29_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {211, 231, 228, 248},                    /* widget size                    */
    &schedule_screen_pixelmap_button_56_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_29_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_29_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_active_ball_define =
{
    "active_ball",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    ID_ACTIVE_BALL,                          /* widget id                      */
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
    {211, 263, 228, 280},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_active_ball), /* control block */
    (void *) &schedule_screen_active_ball_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_patient_info_define =
{
    "patient_info",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {233, 263, 518, 280},                    /* widget size                    */
    &schedule_screen_active_ball_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_patient_info), /* control block */
    (void *) &schedule_screen_patient_info_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_active_bar_define =
{
    "active_bar",
    GX_TYPE_PIXELMAP_PROMPT,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PIXELMAP_PROMPT),              /* control block size             */
    GX_COLOR_ID_ORANGE,                      /* normal color id                */
    GX_COLOR_ID_ORANGE,                      /* selected color id              */
    GX_COLOR_ID_ORANGE,                      /* disabled color id              */
    gx_studio_pixelmap_prompt_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {208, 261, 527, 281},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &schedule_screen_patient_info_define,    /* child widget definition        */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_active_bar), /* control block */
    (void *) &schedule_screen_active_bar_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_active_circle_define =
{
    "active_circle",
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
    {204, 256, 233, 285},                    /* widget size                    */
    &schedule_screen_active_bar_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_active_circle), /* control block */
    (void *) &schedule_screen_active_circle_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_131_1_define =
{
    "pixelmap_button_131_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {597, 293, 614, 310},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_131_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_131_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_119_1_define =
{
    "pixelmap_button_119_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {520, 293, 537, 310},                    /* widget size                    */
    &schedule_screen_pixelmap_button_131_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_119_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_119_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_59_1_define =
{
    "pixelmap_button_59_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 293, 306, 310},                    /* widget size                    */
    &schedule_screen_pixelmap_button_119_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_59_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_59_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_11_1_define =
{
    "pixelmap_button_11_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 293, 151, 310},                    /* widget size                    */
    &schedule_screen_pixelmap_button_59_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_11_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_11_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_10_1_define =
{
    "pixelmap_button_10_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {110, 293, 127, 310},                    /* widget size                    */
    &schedule_screen_pixelmap_button_11_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_10_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_10_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_18_define =
{
    "pixelmap_button_18",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {86, 293, 103, 310},                     /* widget size                    */
    &schedule_screen_pixelmap_button_10_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_18), /* control block */
    (void *) &schedule_screen_pixelmap_button_18_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_134_1_define =
{
    "pixelmap_button_134_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {597, 324, 614, 341},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_134_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_134_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_133_1_define =
{
    "pixelmap_button_133_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {573, 324, 590, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_134_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_133_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_133_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_132_1_define =
{
    "pixelmap_button_132_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {549, 324, 566, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_133_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_132_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_132_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_122_1_define =
{
    "pixelmap_button_122_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {520, 324, 537, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_132_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_122_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_122_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_121_1_define =
{
    "pixelmap_button_121_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {496, 324, 513, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_122_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_121_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_121_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_120_1_define =
{
    "pixelmap_button_120_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {472, 324, 489, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_121_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_120_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_120_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_83_1_define =
{
    "pixelmap_button_83_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {445, 324, 462, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_120_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_83_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_83_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_82_1_define =
{
    "pixelmap_button_82_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {421, 324, 438, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_83_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_82_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_82_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_81_1_define =
{
    "pixelmap_button_81_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {397, 324, 414, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_82_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_81_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_81_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_62_1_define =
{
    "pixelmap_button_62_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 324, 306, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_81_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_62_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_62_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_61_1_define =
{
    "pixelmap_button_61_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {265, 324, 282, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_62_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_61_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_61_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_60_1_define =
{
    "pixelmap_button_60_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {241, 324, 258, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_61_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_60_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_60_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_35_1_define =
{
    "pixelmap_button_35_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {212, 324, 229, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_60_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_35_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_35_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_34_1_define =
{
    "pixelmap_button_34_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {187, 324, 204, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_35_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_34_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_34_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_33_1_define =
{
    "pixelmap_button_33_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {163, 324, 180, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_34_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_33_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_33_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_14_1_define =
{
    "pixelmap_button_14_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 324, 151, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_33_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_14_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_14_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_13_1_define =
{
    "pixelmap_button_13_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {110, 324, 127, 341},                    /* widget size                    */
    &schedule_screen_pixelmap_button_14_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_13_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_13_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_137_1_define =
{
    "pixelmap_button_137_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {597, 355, 614, 372},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_137_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_137_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_136_1_define =
{
    "pixelmap_button_136_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {576, 355, 593, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_137_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_136_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_136_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_125_1_define =
{
    "pixelmap_button_125_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {520, 355, 537, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_136_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_125_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_125_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_124_1_define =
{
    "pixelmap_button_124_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {496, 355, 513, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_125_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_124_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_124_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_86_1_define =
{
    "pixelmap_button_86_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {445, 355, 462, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_124_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_86_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_86_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_85_1_define =
{
    "pixelmap_button_85_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {421, 355, 438, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_86_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_85_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_85_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_65_1_define =
{
    "pixelmap_button_65_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {289, 355, 306, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_85_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_65_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_65_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_64_1_define =
{
    "pixelmap_button_64_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {265, 355, 282, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_65_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_64_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_64_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_38_1_define =
{
    "pixelmap_button_38_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {212, 355, 229, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_64_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_38_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_38_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_37_1_define =
{
    "pixelmap_button_37_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {187, 355, 204, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_38_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_37_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_37_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_17_1_define =
{
    "pixelmap_button_17_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 355, 151, 372},                    /* widget size                    */
    &schedule_screen_pixelmap_button_37_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_17_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_17_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_140_1_define =
{
    "pixelmap_button_140_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {598, 386, 615, 403},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_140_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_140_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_128_1_define =
{
    "pixelmap_button_128_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {521, 386, 538, 403},                    /* widget size                    */
    &schedule_screen_pixelmap_button_140_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_128_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_128_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_127_1_define =
{
    "pixelmap_button_127_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {497, 386, 514, 403},                    /* widget size                    */
    &schedule_screen_pixelmap_button_128_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_127_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_127_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_89_1_define =
{
    "pixelmap_button_89_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {445, 386, 462, 403},                    /* widget size                    */
    &schedule_screen_pixelmap_button_127_1_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_89_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_89_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_68_1_define =
{
    "pixelmap_button_68_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {290, 386, 307, 403},                    /* widget size                    */
    &schedule_screen_pixelmap_button_89_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_68_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_68_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_67_1_define =
{
    "pixelmap_button_67_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {266, 386, 283, 403},                    /* widget size                    */
    &schedule_screen_pixelmap_button_68_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_67_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_67_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_41_1_define =
{
    "pixelmap_button_41_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {213, 386, 230, 403},                    /* widget size                    */
    &schedule_screen_pixelmap_button_67_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_41_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_41_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_20_1_define =
{
    "pixelmap_button_20_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {134, 387, 151, 404},                    /* widget size                    */
    &schedule_screen_pixelmap_button_41_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_20_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_20_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_92_1_define =
{
    "pixelmap_button_92_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {444, 417, 461, 434},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_92_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_92_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_pixelmap_button_44_1_define =
{
    "pixelmap_button_44_1",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {212, 417, 229, 434},                    /* widget size                    */
    &schedule_screen_pixelmap_button_92_1_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_pixelmap_button_44_1), /* control block */
    (void *) &schedule_screen_pixelmap_button_44_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r10_win_define =
{
    "r10_win",
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
    {82, 411, 618, 441},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &schedule_screen_pixelmap_button_44_1_define, /* child widget definition   */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r10_win), /* control block */
    (void *) &schedule_screen_r10_win_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r9_win_define =
{
    "r9_win",
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
    {82, 380, 618, 410},                     /* widget size                    */
    &schedule_screen_r10_win_define,         /* next widget definition         */
    &schedule_screen_pixelmap_button_20_1_define, /* child widget definition   */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r9_win), /* control block */
    (void *) &schedule_screen_r9_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r8_win_define =
{
    "r8_win",
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
    {82, 349, 618, 379},                     /* widget size                    */
    &schedule_screen_r9_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_17_1_define, /* child widget definition   */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r8_win), /* control block */
    (void *) &schedule_screen_r8_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r7_win_define =
{
    "r7_win",
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
    {82, 318, 618, 348},                     /* widget size                    */
    &schedule_screen_r8_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_13_1_define, /* child widget definition   */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r7_win), /* control block */
    (void *) &schedule_screen_r7_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r6_win_define =
{
    "r6_win",
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
    {82, 287, 618, 317},                     /* widget size                    */
    &schedule_screen_r7_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_18_define, /* child widget definition     */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r6_win), /* control block */
    (void *) &schedule_screen_r6_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r5_win_define =
{
    "r5_win",
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
    {83, 256, 619, 286},                     /* widget size                    */
    &schedule_screen_r6_win_define,          /* next widget definition         */
    &schedule_screen_active_circle_define,   /* child widget definition        */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r5_win), /* control block */
    (void *) &schedule_screen_r5_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r4_win_define =
{
    "r4_win",
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
    {83, 225, 619, 255},                     /* widget size                    */
    &schedule_screen_r5_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_29_1_define, /* child widget definition   */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r4_win), /* control block */
    (void *) &schedule_screen_r4_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r3_win_define =
{
    "r3_win",
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
    {82, 194, 618, 224},                     /* widget size                    */
    &schedule_screen_r4_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_16_define, /* child widget definition     */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r3_win), /* control block */
    (void *) &schedule_screen_r3_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r2_win_define =
{
    "r2_win",
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
    {83, 163, 619, 193},                     /* widget size                    */
    &schedule_screen_r3_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_12_define, /* child widget definition     */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r2_win), /* control block */
    (void *) &schedule_screen_r2_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r1_win_define =
{
    "r1_win",
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
    {83, 132, 619, 162},                     /* widget size                    */
    &schedule_screen_r2_win_define,          /* next widget definition         */
    &schedule_screen_pixelmap_button_3_define, /* child widget definition      */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r1_win), /* control block */
    (void *) &schedule_screen_r1_win_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_current_time_define =
{
    "current_time",
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
    {160, 232, 205, 254},                    /* widget size                    */
    &schedule_screen_r1_win_define,          /* next widget definition         */
    &schedule_screen_hour_define,            /* child widget definition        */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_current_time), /* control block */
    (void *) &schedule_screen_current_time_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r11_am_pm_define =
{
    "r11_am_pm",
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
    {44, 431, 66, 448},                      /* widget size                    */
    &schedule_screen_current_time_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r11_am_pm), /* control block */
    (void *) &schedule_screen_r11_am_pm_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r11_hour_define =
{
    "r11_hour",
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
    {23, 431, 40, 448},                      /* widget size                    */
    &schedule_screen_r11_am_pm_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r11_hour), /* control block */
    (void *) &schedule_screen_r11_hour_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r10_am_pm_define =
{
    "r10_am_pm",
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
    {44, 400, 66, 417},                      /* widget size                    */
    &schedule_screen_r11_hour_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r10_am_pm), /* control block */
    (void *) &schedule_screen_r10_am_pm_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r10_hour_define =
{
    "r10_hour",
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
    {22, 400, 39, 417},                      /* widget size                    */
    &schedule_screen_r10_am_pm_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r10_hour), /* control block */
    (void *) &schedule_screen_r10_hour_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r9_am_pm_define =
{
    "r9_am_pm",
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
    {44, 369, 66, 386},                      /* widget size                    */
    &schedule_screen_r10_hour_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r9_am_pm), /* control block */
    (void *) &schedule_screen_r9_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r9_hour_define =
{
    "r9_hour",
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
    {21, 369, 38, 386},                      /* widget size                    */
    &schedule_screen_r9_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r9_hour), /* control block */
    (void *) &schedule_screen_r9_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r8_am_pm_define =
{
    "r8_am_pm",
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
    {43, 338, 65, 355},                      /* widget size                    */
    &schedule_screen_r9_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r8_am_pm), /* control block */
    (void *) &schedule_screen_r8_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r8_hour_define =
{
    "r8_hour",
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
    {22, 338, 39, 355},                      /* widget size                    */
    &schedule_screen_r8_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r8_hour), /* control block */
    (void *) &schedule_screen_r8_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r7_am_pm_define =
{
    "r7_am_pm",
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
    {44, 307, 66, 324},                      /* widget size                    */
    &schedule_screen_r8_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r7_am_pm), /* control block */
    (void *) &schedule_screen_r7_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r7_hour_define =
{
    "r7_hour",
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
    {22, 307, 39, 324},                      /* widget size                    */
    &schedule_screen_r7_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r7_hour), /* control block */
    (void *) &schedule_screen_r7_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r6_am_pm_define =
{
    "r6_am_pm",
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
    {44, 276, 66, 293},                      /* widget size                    */
    &schedule_screen_r7_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r6_am_pm), /* control block */
    (void *) &schedule_screen_r6_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r6_hour_define =
{
    "r6_hour",
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
    {22, 276, 39, 293},                      /* widget size                    */
    &schedule_screen_r6_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r6_hour), /* control block */
    (void *) &schedule_screen_r6_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r5_am_pm_define =
{
    "r5_am_pm",
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
    {44, 245, 66, 262},                      /* widget size                    */
    &schedule_screen_r6_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r5_am_pm), /* control block */
    (void *) &schedule_screen_r5_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r5_hour_define =
{
    "r5_hour",
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
    {22, 245, 39, 262},                      /* widget size                    */
    &schedule_screen_r5_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r5_hour), /* control block */
    (void *) &schedule_screen_r5_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r4_am_pm_define =
{
    "r4_am_pm",
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
    {44, 213, 66, 230},                      /* widget size                    */
    &schedule_screen_r5_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r4_am_pm), /* control block */
    (void *) &schedule_screen_r4_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r4_hour_define =
{
    "r4_hour",
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
    {22, 214, 39, 231},                      /* widget size                    */
    &schedule_screen_r4_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r4_hour), /* control block */
    (void *) &schedule_screen_r4_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r3_am_pm_define =
{
    "r3_am_pm",
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
    {44, 183, 66, 200},                      /* widget size                    */
    &schedule_screen_r4_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r3_am_pm), /* control block */
    (void *) &schedule_screen_r3_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r3_hour_define =
{
    "r3_hour",
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
    {22, 183, 39, 200},                      /* widget size                    */
    &schedule_screen_r3_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r3_hour), /* control block */
    (void *) &schedule_screen_r3_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r2_am_pm_define =
{
    "r2_am_pm",
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
    {44, 152, 66, 169},                      /* widget size                    */
    &schedule_screen_r3_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r2_am_pm), /* control block */
    (void *) &schedule_screen_r2_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r2_hour_define =
{
    "r2_hour",
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
    {22, 152, 39, 169},                      /* widget size                    */
    &schedule_screen_r2_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r2_hour), /* control block */
    (void *) &schedule_screen_r2_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r1_hour_define =
{
    "r1_hour",
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
    {22, 121, 39, 138},                      /* widget size                    */
    &schedule_screen_r2_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r1_hour), /* control block */
    (void *) &schedule_screen_r1_hour_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_r1_am_pm_define =
{
    "r1_am_pm",
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
    {44, 121, 66, 138},                      /* widget size                    */
    &schedule_screen_r1_hour_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_r1_am_pm), /* control block */
    (void *) &schedule_screen_r1_am_pm_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c7_day_define =
{
    "c7_day",
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
    {589, 108, 606, 125},                    /* widget size                    */
    &schedule_screen_r1_am_pm_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c7_day), /* control block */
    (void *) &schedule_screen_c7_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c6_day_define =
{
    "c6_day",
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
    {512, 108, 529, 125},                    /* widget size                    */
    &schedule_screen_c7_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c6_day), /* control block */
    (void *) &schedule_screen_c6_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c5_day_define =
{
    "c5_day",
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
    {435, 108, 452, 125},                    /* widget size                    */
    &schedule_screen_c6_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c5_day), /* control block */
    (void *) &schedule_screen_c5_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c4_day_define =
{
    "c4_day",
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
    {358, 108, 375, 125},                    /* widget size                    */
    &schedule_screen_c5_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c4_day), /* control block */
    (void *) &schedule_screen_c4_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c3_day_define =
{
    "c3_day",
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
    {281, 108, 298, 125},                    /* widget size                    */
    &schedule_screen_c4_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c3_day), /* control block */
    (void *) &schedule_screen_c3_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c2_day_define =
{
    "c2_day",
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
    {204, 108, 221, 125},                    /* widget size                    */
    &schedule_screen_c3_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c2_day), /* control block */
    (void *) &schedule_screen_c2_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c1_day_define =
{
    "c1_day",
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
    {127, 108, 144, 125},                    /* widget size                    */
    &schedule_screen_c2_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c1_day), /* control block */
    (void *) &schedule_screen_c1_day_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c7_week_define =
{
    "c7_week",
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
    {553, 108, 585, 125},                    /* widget size                    */
    &schedule_screen_c1_day_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c7_week), /* control block */
    (void *) &schedule_screen_c7_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c6_week_define =
{
    "c6_week",
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
    {476, 108, 508, 125},                    /* widget size                    */
    &schedule_screen_c7_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c6_week), /* control block */
    (void *) &schedule_screen_c6_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c5_week_define =
{
    "c5_week",
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
    {399, 108, 431, 125},                    /* widget size                    */
    &schedule_screen_c6_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c5_week), /* control block */
    (void *) &schedule_screen_c5_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c4_week_define =
{
    "c4_week",
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
    {322, 108, 354, 125},                    /* widget size                    */
    &schedule_screen_c5_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c4_week), /* control block */
    (void *) &schedule_screen_c4_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c2_week_define =
{
    "c2_week",
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
    {168, 108, 200, 125},                    /* widget size                    */
    &schedule_screen_c4_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c2_week), /* control block */
    (void *) &schedule_screen_c2_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c3_week_define =
{
    "c3_week",
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
    {245, 108, 277, 125},                    /* widget size                    */
    &schedule_screen_c2_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c3_week), /* control block */
    (void *) &schedule_screen_c3_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_c1_week_define =
{
    "c1_week",
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
    {90, 108, 122, 125},                     /* widget size                    */
    &schedule_screen_c3_week_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_c1_week), /* control block */
    (void *) &schedule_screen_c1_week_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_icon_2_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {31, 74, 38, 86},                        /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_icon_2), /* control block */
    (void *) &schedule_screen_icon_2_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_icon_1_define =
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {112, 74, 119, 86},                      /* widget size                    */
    &schedule_screen_icon_2_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_icon_1), /* control block */
    (void *) &schedule_screen_icon_1_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_year_define =
{
    "year",
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
    {74, 73, 109, 90},                       /* widget size                    */
    &schedule_screen_icon_1_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_year), /* control block */
    (void *) &schedule_screen_year_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_schedule_win_define =
{
    "schedule_win",
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
    (VOID (*)(GX_WIDGET *)) schedule_win_draw, /* drawing function override    */
    GX_NULL,                                 /* event function override        */
    {20, 99, 619, 459},                      /* widget size                    */
    &schedule_screen_year_define,            /* next widget definition         */
    &schedule_screen_c1_week_define,         /* child widget definition        */
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_schedule_win), /* control block */
    (void *) &schedule_screen_schedule_win_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET schedule_screen_month_define =
{
    "month",
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
    {39, 73, 73, 90},                        /* widget size                    */
    &schedule_screen_schedule_win_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCHEDULE_SCREEN_CONTROL_BLOCK, schedule_screen_month), /* control block */
    (void *) &schedule_screen_month_properties /* extended properties          */
};

GX_ANIMATION_INFO schedule_screen_animation_1 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r10_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {82, 405}, {82, 411}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_2 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r9_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 3, 1,
    {82, 374}, {82, 380}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_3 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r8_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 6, 1,
    {82, 343}, {82, 349}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_4 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r7_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 9, 1,
    {82, 312}, {82, 318}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_5 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r6_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 12, 1,
    {82, 281}, {82, 287}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_6 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r5_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, ID_R5_WIN_FADE_IN, 15, 1,
    {82, 250}, {82, 256}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_7 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r4_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 18, 1,
    {82, 519}, {82, 525}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_8 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r3_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 21, 1,
    {82, 188}, {82, 194}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_9 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r2_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 24, 1,
    {82, 157}, {82, 163}, 0, 255, 10
};


GX_ANIMATION_INFO schedule_screen_animation_10 = {
    (GX_WIDGET *) &schedule_screen.schedule_screen_r1_win,
    (GX_WIDGET *) &schedule_screen.schedule_screen_schedule_win,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 27, 1,
    {82, 126}, {82, 132}, 0, 255, 10
};


GX_STUDIO_ACTION schedule_screen_on_gx_event_showactions[11] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r10_win, &schedule_screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r9_win, &schedule_screen_animation_2},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r8_win, &schedule_screen_animation_3},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r7_win, &schedule_screen_animation_4},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r6_win, &schedule_screen_animation_5},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r5_win, &schedule_screen_animation_6},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r4_win, &schedule_screen_animation_7},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r3_win, &schedule_screen_animation_8},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r2_win, &schedule_screen_animation_9},
    {GX_ACTION_TYPE_ANIMATION, 0, &schedule_screen.schedule_screen_schedule_win, &schedule_screen.schedule_screen_r1_win, &schedule_screen_animation_10},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_schedule_screen_event_table[] = {
    { GX_EVENT_SHOW, 0, schedule_screen_on_gx_event_showactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS schedule_screen_event_chain = {gx_studio_schedule_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))schedule_screen_event_handler};
static UINT gx_studio_schedule_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &schedule_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET schedule_screen_define =
{
    "schedule_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_SCHEDULE_SCREEN,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SCHEDULE_SCREEN_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_schedule_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &schedule_screen_month_define,           /* child widget                   */
    0,                                       /* control block                  */
    (void *) &schedule_screen_properties     /* extended properties            */
};
GX_TEMPLATE_PROPERTIES patients_screen_properties =
{
    &template_define,                        /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES patients_screen_patient_name_label_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES patients_screen_total_admited_label_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DISABLED_TEXT                /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES patients_screen_total_admitted_count_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    168                                      /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES patients_screen_DOB_label_properties =
{
    GX_STRING_ID_STRING_10,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES patients_screen_room_label_properties =
{
    GX_STRING_ID_STRING_32,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES patients_screen_admission_date_label_properties =
{
    GX_STRING_ID_STRING_38,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES patients_screen_pa_num_label_properties =
{
    GX_STRING_ID_STRING_39,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_VERTICAL_LIST_PROPERTIES patients_screen_patient_list_properties =
{
    0,                                       /* wallpaper id                   */
    patient_row_create,                      /* callback function              */
    46                                       /* total rows                     */
};
GX_SCROLLBAR_APPEARANCE  patients_screen_vertical_scroll_properties =
{
    20,                                      /* scroll width                   */
    8,                                       /* thumb width                    */
    5,                                       /* thumb travel min               */
    5,                                       /* thumb travel max               */
    0,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    GX_PIXELMAP_ID_TABLE_SCROLL_THUMB,       /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb color             */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb border color      */
    GX_COLOR_ID_SCROLL_FILL,                 /* scroll button color            */
};
GX_ICON_PROPERTIES patients_screen_icon_properties =
{
    GX_PIXELMAP_ID_PATIENTS_HORZ_TOP_LINE,   /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_vertical_scroll_define =
{
    "vertical_scroll",
    GX_TYPE_VERTICAL_SCROLL,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_SCROLLBAR_VERTICAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_vertical_scrollbar_create,     /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {600, 125, 619, 464},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_vertical_scroll), /* control block */
    (void *) &patients_screen_vertical_scroll_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_icon_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 122, 599, 124},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_icon), /* control block */
    (void *) &patients_screen_icon_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_patient_list_define =
{
    "patient_list",
    GX_TYPE_VERTICAL_LIST,                   /* widget type                    */
    ID_PATIENT_LIST,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_REPEAT_SELECT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_VERTICAL_LIST),                /* control block size             */
    GX_COLOR_ID_BLACK,                       /* normal color id                */
    GX_COLOR_ID_BLACK,                       /* selected color id              */
    GX_COLOR_ID_BLACK,                       /* disabled color id              */
    gx_studio_vertical_list_create,          /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {20, 125, 619, 464},                     /* widget size                    */
    &patients_screen_icon_define,            /* next widget definition         */
    &patients_screen_vertical_scroll_define, /* child widget definition        */
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_patient_list), /* control block */
    (void *) &patients_screen_patient_list_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_pa_num_label_define =
{
    "pa_num_label",
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
    {531, 98, 591, 115},                     /* widget size                    */
    &patients_screen_patient_list_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_pa_num_label), /* control block */
    (void *) &patients_screen_pa_num_label_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_admission_date_label_define =
{
    "admission_date_label",
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
    {382, 98, 499, 115},                     /* widget size                    */
    &patients_screen_pa_num_label_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_admission_date_label), /* control block */
    (void *) &patients_screen_admission_date_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_room_label_define =
{
    "room_label",
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
    {299, 98, 343, 115},                     /* widget size                    */
    &patients_screen_admission_date_label_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_room_label), /* control block */
    (void *) &patients_screen_room_label_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_DOB_label_define =
{
    "DOB_label",
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
    {191, 97, 224, 114},                     /* widget size                    */
    &patients_screen_room_label_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_DOB_label), /* control block */
    (void *) &patients_screen_DOB_label_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_total_admitted_count_define =
{
    "total_admitted_count",
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
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {141, 70, 167, 87},                      /* widget size                    */
    &patients_screen_DOB_label_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_total_admitted_count), /* control block */
    (void *) &patients_screen_total_admitted_count_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_total_admited_label_define =
{
    "total_admited_label",
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
    {20, 70, 133, 87},                       /* widget size                    */
    &patients_screen_total_admitted_count_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_total_admited_label), /* control block */
    (void *) &patients_screen_total_admited_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_patient_name_label_define =
{
    "patient_name_label",
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
    {30, 98, 132, 115},                      /* widget size                    */
    &patients_screen_total_admited_label_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(PATIENTS_SCREEN_CONTROL_BLOCK, patients_screen_patient_name_label), /* control block */
    (void *) &patients_screen_patient_name_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET patients_screen_define =
{
    "patients_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_PATIENTS_SCREEN,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(PATIENTS_SCREEN_CONTROL_BLOCK),   /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) patients_screen_event_handler, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &patients_screen_patient_name_label_define, /* child widget                */
    0,                                       /* control block                  */
    (void *) &patients_screen_properties     /* extended properties            */
};
GX_TEMPLATE_PROPERTIES vitals_screen_properties =
{
    &template_define,                        /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_WINDOW_PROPERTIES vitals_screen_insulin_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES vitals_screen_insulin_properties =
{
    GX_PIXELMAP_ID_BALL_BIG_RED,             /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES vitals_screen_insulin_name_1_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_insulin_unit_1_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_insulin_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    insulin_value_format,                    /* format function                */
    71                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_17_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_18_properties =
{
    GX_STRING_ID_STRING_15,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_19_properties =
{
    GX_STRING_ID_STRING_16,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_20_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_21_properties =
{
    GX_STRING_ID_STRING_17,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_glucose_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    130                                      /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_22_properties =
{
    GX_STRING_ID_STRING_20,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_57,                  /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_2_properties =
{
    GX_STRING_ID_STRING_58,                  /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_ekg_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES vitals_screen_heart_rate_icon_properties =
{
    GX_PIXELMAP_ID_BALL_BIG_GREEN,           /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES vitals_screen_ekg_label_properties =
{
    GX_STRING_ID_STRING_12,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_BPM_label_properties =
{
    GX_STRING_ID_STRING_1,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_hr_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    88                                       /* numeric prompt value           */
};
GX_WINDOW_PROPERTIES vitals_screen_ekg_waveform_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES vitals_screen_pulse_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES vitals_screen_spo2_icon_properties =
{
    GX_PIXELMAP_ID_BALL_BIG_BLUE,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES vitals_screen_pulse_label_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_spo2_label_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_spo2_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    94                                       /* numeric prompt value           */
};
GX_WINDOW_PROPERTIES vitals_screen_pulse_waveform_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES vitals_screen_blood_pressure_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_23_properties =
{
    GX_STRING_ID_STRING_21,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_24_properties =
{
    GX_STRING_ID_STRING_22,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_25_properties =
{
    GX_STRING_ID_STRING_23,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_last_bolus_value_9_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    120                                      /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_26_properties =
{
    GX_STRING_ID_STRING_24,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_last_bolus_value_10_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    78                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_12_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_13_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_14_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_15_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    10                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_12_3_properties =
{
    GX_STRING_ID_STRING_26,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_12_4_properties =
{
    GX_STRING_ID_STRING_26,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_12_5_properties =
{
    GX_STRING_ID_STRING_26,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_16_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    15                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_17_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    30                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_10_6_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_10_7_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_10_8_properties =
{
    GX_STRING_ID_STRING_25,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_18_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    115                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_19_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    115                                      /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_20_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    115                                      /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_10_9_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_10_10_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_10_11_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_21_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    80                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_10_1_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    84                                       /* numeric prompt value           */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_11_11_1_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_GRAY,                        /* normal text color              */
    GX_COLOR_ID_GRAY,                        /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    81                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_26_1_properties =
{
    GX_STRING_ID_STRING_27,                  /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_patien_info_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_gender_properties =
{
    GX_STRING_ID_STRING_29,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_name_properties =
{
    GX_STRING_ID_STRING_28,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_dob_properties =
{
    GX_STRING_ID_STRING_35,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_patient_age_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    67                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_age_label_properties =
{
    GX_STRING_ID_STRING_31,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_patient_number_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled text color            */
    GX_NULL,                                 /* format function                */
    148326                                   /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_number_label_properties =
{
    GX_STRING_ID_STRING_30,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_room_label_properties =
{
    GX_STRING_ID_STRING_32,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_patient_room_id_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    400                                      /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_bed_label_properties =
{
    GX_STRING_ID_STRING_33,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_patient_bed_id_properties =
{
    GX_STRING_ID_STRING_34,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_temperature_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_15_properties =
{
    GX_STRING_ID_STRING_36,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_13_17_properties =
{
    GX_STRING_ID_STRING_37,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_13_18_properties =
{
    GX_STRING_ID_STRING_45,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_prompt_14_3_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_NULL,                                 /* format function                */
    392                                      /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_13_13_1_properties =
{
    GX_STRING_ID_STRING_47,                  /* string id                      */
    GX_FONT_ID_SMALL,                        /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_5_properties =
{
    GX_STRING_ID_STRING_59,                  /* string id                      */
    GX_FONT_ID_LARGE,                        /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_13_12_2_properties =
{
    GX_STRING_ID_STRING_46,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_PROMPT_PROPERTIES vitals_screen_prompt_13_12_3_properties =
{
    GX_STRING_ID_STRING_46,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_medtype1_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES vitals_screen_medtype1_label_properties =
{
    GX_STRING_ID_STRING_40,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_medtype1_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    55                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_medtype1_percentage_flag_properties =
{
    GX_STRING_ID_STRING_43,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_medtype1_slider_win_properties =
{
    GX_PIXELMAP_ID_PROGRESS_BAR_BG           /* wallpaper pixelmap id          */
};
GX_PIXELMAP_SLIDER_PROPERTIES vitals_screen_medtype1_slider_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    25,                                      /* current value                  */
    10,                                      /* increment                      */
    0,                                       /* minimum travel                 */
    0,                                       /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    0,                                       /* needle inset                   */
    0,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_PROGRESS_BAR_LOWER_FILL,  /* lower pixelmap id              */
    GX_PIXELMAP_ID_PROGRESS_UPPER_FILL,      /* upper pixelmap id              */
    0                                        /* needle pixelmap id             */
};
GX_WINDOW_PROPERTIES vitals_screen_medtype3_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES vitals_screen_medtype3_label_properties =
{
    GX_STRING_ID_STRING_42,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_medtype3_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    45                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_medtype3_percentage_flag_properties =
{
    GX_STRING_ID_STRING_43,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_medtype3_slider_win_properties =
{
    GX_PIXELMAP_ID_PROGRESS_BAR_BG           /* wallpaper pixelmap id          */
};
GX_PIXELMAP_SLIDER_PROPERTIES vitals_screen_medtype3_slider_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    45,                                      /* current value                  */
    10,                                      /* increment                      */
    0,                                       /* minimum travel                 */
    0,                                       /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    0,                                       /* needle inset                   */
    0,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_PROGRESS_BAR_LOWER_FILL,  /* lower pixelmap id              */
    GX_PIXELMAP_ID_PROGRESS_UPPER_FILL,      /* upper pixelmap id              */
    0                                        /* needle pixelmap id             */
};
GX_WINDOW_PROPERTIES vitals_screen_medtype2_win_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_PROMPT_PROPERTIES vitals_screen_medtype2_label_properties =
{
    GX_STRING_ID_STRING_41,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_NUMERIC_PROMPT_PROPERTIES vitals_screen_medtype2_value_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_MEDIUM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_GRAY,                        /* disabled text color            */
    GX_NULL,                                 /* format function                */
    25                                       /* numeric prompt value           */
};
GX_PROMPT_PROPERTIES vitals_screen_medtype2_percentage_flag_properties =
{
    GX_STRING_ID_STRING_43,                  /* string id                      */
    GX_FONT_ID_NORMAL,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_DARK_GRAY                    /* disabled text color            */
};
GX_WINDOW_PROPERTIES vitals_screen_medtype2_slider_win_properties =
{
    GX_PIXELMAP_ID_PROGRESS_BAR_BG           /* wallpaper pixelmap id          */
};
GX_PIXELMAP_SLIDER_PROPERTIES vitals_screen_medtype2_slider_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    25,                                      /* current value                  */
    10,                                      /* increment                      */
    0,                                       /* minimum travel                 */
    0,                                       /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    0,                                       /* needle inset                   */
    0,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_PROGRESS_BAR_LOWER_FILL,  /* lower pixelmap id              */
    GX_PIXELMAP_ID_PROGRESS_UPPER_FILL,      /* upper pixelmap id              */
    0                                        /* needle pixelmap id             */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_insulin_value_define =
{
    "insulin_value",
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
    {25, 110, 91, 140},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_insulin_value), /* control block */
    (void *) &vitals_screen_insulin_value_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_insulin_unit_1_define =
{
    "insulin_unit_1",
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
    {49, 141, 73, 150},                      /* widget size                    */
    &vitals_screen_insulin_value_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_insulin_unit_1), /* control block */
    (void *) &vitals_screen_insulin_unit_1_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_insulin_name_1_define =
{
    "insulin_name_1",
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
    {37, 97, 81, 106},                       /* widget size                    */
    &vitals_screen_insulin_unit_1_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_insulin_name_1), /* control block */
    (void *) &vitals_screen_insulin_name_1_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_2_define =
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
    {190, 108, 244, 143},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_2), /* control block */
    (void *) &vitals_screen_prompt_2_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_1_define =
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
    {102, 108, 156, 143},                    /* widget size                    */
    &vitals_screen_prompt_2_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_1), /* control block */
    (void *) &vitals_screen_prompt_1_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_22_define =
{
    "prompt_22",
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
    {251, 143, 298, 160},                    /* widget size                    */
    &vitals_screen_prompt_1_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_22), /* control block */
    (void *) &vitals_screen_prompt_22_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_glucose_value_define =
{
    "glucose_value",
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
    {251, 108, 316, 143},                    /* widget size                    */
    &vitals_screen_prompt_22_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_glucose_value), /* control block */
    (void *) &vitals_screen_glucose_value_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_21_define =
{
    "prompt_21",
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
    {251, 89, 311, 106},                     /* widget size                    */
    &vitals_screen_glucose_value_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_21), /* control block */
    (void *) &vitals_screen_prompt_21_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_20_define =
{
    "prompt_20",
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
    {194, 145, 205, 162},                    /* widget size                    */
    &vitals_screen_prompt_21_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_20), /* control block */
    (void *) &vitals_screen_prompt_20_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_19_define =
{
    "prompt_19",
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
    {194, 89, 221, 106},                     /* widget size                    */
    &vitals_screen_prompt_20_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_19), /* control block */
    (void *) &vitals_screen_prompt_19_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_18_define =
{
    "prompt_18",
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
    {105, 89, 184, 106},                     /* widget size                    */
    &vitals_screen_prompt_19_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_18), /* control block */
    (void *) &vitals_screen_prompt_18_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_17_define =
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
    {105, 145, 116, 162},                    /* widget size                    */
    &vitals_screen_prompt_18_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_17), /* control block */
    (void *) &vitals_screen_prompt_17_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_insulin_define =
{
    "insulin",
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
    {25, 88, 93, 156},                       /* widget size                    */
    &vitals_screen_prompt_17_define,         /* next widget definition         */
    &vitals_screen_insulin_name_1_define,    /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_insulin), /* control block */
    (void *) &vitals_screen_insulin_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_hr_value_define =
{
    "hr_value",
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
    {39, 206, 82, 241},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_hr_value), /* control block */
    (void *) &vitals_screen_hr_value_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_BPM_label_define =
{
    "BPM_label",
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
    {49, 242, 73, 251},                      /* widget size                    */
    &vitals_screen_hr_value_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_BPM_label), /* control block */
    (void *) &vitals_screen_BPM_label_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_ekg_label_define =
{
    "ekg_label",
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
    {49, 194, 71, 203},                      /* widget size                    */
    &vitals_screen_BPM_label_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_ekg_label), /* control block */
    (void *) &vitals_screen_ekg_label_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_ekg_waveform_win_define =
{
    "ekg_waveform_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_CANVAS,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {94, 186, 314, 254},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_ekg_waveform_win), /* control block */
    (void *) &vitals_screen_ekg_waveform_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_heart_rate_icon_define =
{
    "heart_rate_icon",
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
    {25, 187, 93, 255},                      /* widget size                    */
    &vitals_screen_ekg_waveform_win_define,  /* next widget definition         */
    &vitals_screen_ekg_label_define,         /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_heart_rate_icon), /* control block */
    (void *) &vitals_screen_heart_rate_icon_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_spo2_value_define =
{
    "spo2_value",
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
    {39, 304, 82, 339},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_spo2_value), /* control block */
    (void *) &vitals_screen_spo2_value_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_spo2_label_define =
{
    "spo2_label",
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
    {48, 339, 77, 348},                      /* widget size                    */
    &vitals_screen_spo2_value_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_spo2_label), /* control block */
    (void *) &vitals_screen_spo2_label_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_pulse_label_define =
{
    "pulse_label",
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
    {44, 292, 78, 301},                      /* widget size                    */
    &vitals_screen_spo2_label_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_pulse_label), /* control block */
    (void *) &vitals_screen_pulse_label_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_pulse_waveform_win_define =
{
    "pulse_waveform_win",
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
    {94, 285, 314, 353},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_pulse_waveform_win), /* control block */
    (void *) &vitals_screen_pulse_waveform_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_spo2_icon_define =
{
    "spo2_icon",
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
    {25, 285, 93, 353},                      /* widget size                    */
    &vitals_screen_pulse_waveform_win_define, /* next widget definition        */
    &vitals_screen_pulse_label_define,       /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_spo2_icon), /* control block */
    (void *) &vitals_screen_spo2_icon_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_26_1_define =
{
    "prompt_26_1",
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
    {120, 414, 134, 460},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_26_1), /* control block */
    (void *) &vitals_screen_prompt_26_1_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_11_1_define =
{
    "prompt_11_11_1",
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
    {302, 441, 319, 458},                    /* widget size                    */
    &vitals_screen_prompt_26_1_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_11_1), /* control block */
    (void *) &vitals_screen_prompt_11_11_1_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_10_1_define =
{
    "prompt_11_10_1",
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
    {302, 422, 319, 439},                    /* widget size                    */
    &vitals_screen_prompt_11_11_1_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_10_1), /* control block */
    (void *) &vitals_screen_prompt_11_10_1_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_21_define =
{
    "prompt_11_21",
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
    {302, 403, 319, 420},                    /* widget size                    */
    &vitals_screen_prompt_11_10_1_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_21), /* control block */
    (void *) &vitals_screen_prompt_11_21_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_10_11_define =
{
    "prompt_10_11",
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
    {295, 403, 300, 420},                    /* widget size                    */
    &vitals_screen_prompt_11_21_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_10_11), /* control block */
    (void *) &vitals_screen_prompt_10_11_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_10_10_define =
{
    "prompt_10_10",
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
    {295, 422, 300, 439},                    /* widget size                    */
    &vitals_screen_prompt_10_11_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_10_10), /* control block */
    (void *) &vitals_screen_prompt_10_10_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_10_9_define =
{
    "prompt_10_9",
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
    {295, 441, 300, 458},                    /* widget size                    */
    &vitals_screen_prompt_10_10_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_10_9), /* control block */
    (void *) &vitals_screen_prompt_10_9_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_20_define =
{
    "prompt_11_20",
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
    {267, 441, 293, 459},                    /* widget size                    */
    &vitals_screen_prompt_10_9_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_20), /* control block */
    (void *) &vitals_screen_prompt_11_20_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_19_define =
{
    "prompt_11_19",
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
    {267, 422, 293, 439},                    /* widget size                    */
    &vitals_screen_prompt_11_20_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_19), /* control block */
    (void *) &vitals_screen_prompt_11_19_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_18_define =
{
    "prompt_11_18",
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
    {267, 403, 293, 420},                    /* widget size                    */
    &vitals_screen_prompt_11_19_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_18), /* control block */
    (void *) &vitals_screen_prompt_11_18_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_10_8_define =
{
    "prompt_10_8",
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
    {241, 405, 258, 414},                    /* widget size                    */
    &vitals_screen_prompt_11_18_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_10_8), /* control block */
    (void *) &vitals_screen_prompt_10_8_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_10_7_define =
{
    "prompt_10_7",
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
    {241, 425, 258, 434},                    /* widget size                    */
    &vitals_screen_prompt_10_8_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_10_7), /* control block */
    (void *) &vitals_screen_prompt_10_7_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_10_6_define =
{
    "prompt_10_6",
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
    {241, 444, 258, 453},                    /* widget size                    */
    &vitals_screen_prompt_10_7_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_10_6), /* control block */
    (void *) &vitals_screen_prompt_10_6_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_17_define =
{
    "prompt_11_17",
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
    {221, 440, 238, 457},                    /* widget size                    */
    &vitals_screen_prompt_10_6_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_17), /* control block */
    (void *) &vitals_screen_prompt_11_17_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_16_define =
{
    "prompt_11_16",
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
    {221, 421, 238, 438},                    /* widget size                    */
    &vitals_screen_prompt_11_17_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_16), /* control block */
    (void *) &vitals_screen_prompt_11_16_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_12_5_define =
{
    "prompt_12_5",
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
    {218, 439, 221, 456},                    /* widget size                    */
    &vitals_screen_prompt_11_16_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_12_5), /* control block */
    (void *) &vitals_screen_prompt_12_5_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_12_4_define =
{
    "prompt_12_4",
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
    {218, 421, 221, 438},                    /* widget size                    */
    &vitals_screen_prompt_12_5_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_12_4), /* control block */
    (void *) &vitals_screen_prompt_12_4_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_12_3_define =
{
    "prompt_12_3",
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
    {218, 401, 221, 418},                    /* widget size                    */
    &vitals_screen_prompt_12_4_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_12_3), /* control block */
    (void *) &vitals_screen_prompt_12_3_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_15_define =
{
    "prompt_11_15",
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
    {221, 402, 238, 419},                    /* widget size                    */
    &vitals_screen_prompt_12_3_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_15), /* control block */
    (void *) &vitals_screen_prompt_11_15_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_14_define =
{
    "prompt_11_14",
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
    {200, 440, 217, 457},                    /* widget size                    */
    &vitals_screen_prompt_11_15_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_14), /* control block */
    (void *) &vitals_screen_prompt_11_14_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_13_define =
{
    "prompt_11_13",
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
    {200, 421, 217, 438},                    /* widget size                    */
    &vitals_screen_prompt_11_14_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_13), /* control block */
    (void *) &vitals_screen_prompt_11_13_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_11_12_define =
{
    "prompt_11_12",
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
    {200, 402, 217, 419},                    /* widget size                    */
    &vitals_screen_prompt_11_13_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_11_12), /* control block */
    (void *) &vitals_screen_prompt_11_12_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_last_bolus_value_10_define =
{
    "last_bolus_value_10",
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
    {135, 415, 192, 461},                    /* widget size                    */
    &vitals_screen_prompt_11_12_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_last_bolus_value_10), /* control block */
    (void *) &vitals_screen_last_bolus_value_10_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_26_define =
{
    "prompt_26",
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
    {137, 402, 156, 411},                    /* widget size                    */
    &vitals_screen_last_bolus_value_10_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_26), /* control block */
    (void *) &vitals_screen_prompt_26_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_last_bolus_value_9_define =
{
    "last_bolus_value_9",
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
    {34, 415, 120, 461},                     /* widget size                    */
    &vitals_screen_prompt_26_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_last_bolus_value_9), /* control block */
    (void *) &vitals_screen_last_bolus_value_9_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_25_define =
{
    "prompt_25",
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
    {35, 402, 55, 411},                      /* widget size                    */
    &vitals_screen_last_bolus_value_9_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_25), /* control block */
    (void *) &vitals_screen_prompt_25_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_24_define =
{
    "prompt_24",
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
    {156, 381, 208, 398},                    /* widget size                    */
    &vitals_screen_prompt_25_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_24), /* control block */
    (void *) &vitals_screen_prompt_24_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_23_define =
{
    "prompt_23",
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
    {33, 381, 148, 398},                     /* widget size                    */
    &vitals_screen_prompt_24_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_23), /* control block */
    (void *) &vitals_screen_prompt_23_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_bed_id_define =
{
    "patient_bed_id",
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
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {601, 131, 619, 148},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_bed_id), /* control block */
    (void *) &vitals_screen_patient_bed_id_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_bed_label_define =
{
    "patient_bed_label",
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
    {567, 131, 596, 148},                    /* widget size                    */
    &vitals_screen_patient_bed_id_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_bed_label), /* control block */
    (void *) &vitals_screen_patient_bed_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_room_id_define =
{
    "patient_room_id",
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
    {512, 132, 538, 149},                    /* widget size                    */
    &vitals_screen_patient_bed_label_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_room_id), /* control block */
    (void *) &vitals_screen_patient_room_id_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_room_label_define =
{
    "patient_room_label",
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
    {461, 131, 505, 148},                    /* widget size                    */
    &vitals_screen_patient_room_id_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_room_label), /* control block */
    (void *) &vitals_screen_patient_room_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_number_label_define =
{
    "patient_number_label",
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
    {380, 131, 388, 148},                    /* widget size                    */
    &vitals_screen_patient_room_label_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_number_label), /* control block */
    (void *) &vitals_screen_patient_number_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_number_define =
{
    "patient_number",
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
    {389, 131, 442, 148},                    /* widget size                    */
    &vitals_screen_patient_number_label_define, /* next widget definition      */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_number), /* control block */
    (void *) &vitals_screen_patient_number_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_age_label_define =
{
    "patient_age_label",
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
    {457, 110, 484, 127},                    /* widget size                    */
    &vitals_screen_patient_number_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_age_label), /* control block */
    (void *) &vitals_screen_patient_age_label_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_age_define =
{
    "patient_age",
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
    {491, 110, 508, 127},                    /* widget size                    */
    &vitals_screen_patient_age_label_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_age), /* control block */
    (void *) &vitals_screen_patient_age_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_dob_define =
{
    "patient_dob",
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
    {535, 110, 618, 127},                    /* widget size                    */
    &vitals_screen_patient_age_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_dob), /* control block */
    (void *) &vitals_screen_patient_dob_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_name_define =
{
    "patient_name",
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
    {429, 79, 619, 101},                     /* widget size                    */
    &vitals_screen_patient_dob_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_name), /* control block */
    (void *) &vitals_screen_patient_name_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patient_gender_define =
{
    "patient_gender",
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
    {380, 110, 435, 127},                    /* widget size                    */
    &vitals_screen_patient_name_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patient_gender), /* control block */
    (void *) &vitals_screen_patient_gender_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_13_12_3_define =
{
    "prompt_13_12_3",
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
    {609, 185, 620, 202},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_13_12_3), /* control block */
    (void *) &vitals_screen_prompt_13_12_3_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_13_12_2_define =
{
    "prompt_13_12_2",
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
    {481, 185, 492, 202},                    /* widget size                    */
    &vitals_screen_prompt_13_12_3_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_13_12_2), /* control block */
    (void *) &vitals_screen_prompt_13_12_2_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_5_define =
{
    "prompt_5",
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
    {380, 187, 480, 233},                    /* widget size                    */
    &vitals_screen_prompt_13_12_2_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_5), /* control block */
    (void *) &vitals_screen_prompt_5_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_define =
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
    {510, 187, 610, 233},                    /* widget size                    */
    &vitals_screen_prompt_5_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt), /* control block */
    (void *) &vitals_screen_prompt_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_13_13_1_define =
{
    "prompt_13_13_1",
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
    {613, 161, 619, 170},                    /* widget size                    */
    &vitals_screen_prompt_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_13_13_1), /* control block */
    (void *) &vitals_screen_prompt_13_13_1_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_14_3_define =
{
    "prompt_14_3",
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
    {586, 162, 612, 179},                    /* widget size                    */
    &vitals_screen_prompt_13_13_1_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_14_3), /* control block */
    (void *) &vitals_screen_prompt_14_3_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_13_18_define =
{
    "prompt_13_18",
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
    {539, 165, 581, 174},                    /* widget size                    */
    &vitals_screen_prompt_14_3_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_13_18), /* control block */
    (void *) &vitals_screen_prompt_13_18_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_13_17_define =
{
    "prompt_13_17",
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
    {487, 165, 525, 174},                    /* widget size                    */
    &vitals_screen_prompt_13_18_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_13_17), /* control block */
    (void *) &vitals_screen_prompt_13_17_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_prompt_15_define =
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
    {380, 161, 478, 178},                    /* widget size                    */
    &vitals_screen_prompt_13_17_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_prompt_15), /* control block */
    (void *) &vitals_screen_prompt_15_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype1_slider_define =
{
    "medtype1_slider",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_MEDTYPE1_SLIDER,                      /* widget id                      */
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
    {381, 282, 619, 301},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype1_slider), /* control block */
    (void *) &vitals_screen_medtype1_slider_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype1_slider_win_define =
{
    "medtype1_slider_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {381, 282, 619, 301},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &vitals_screen_medtype1_slider_define,   /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype1_slider_win), /* control block */
    (void *) &vitals_screen_medtype1_slider_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype1_percentage_flag_define =
{
    "medtype1_percentage_flag",
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
    {599, 246, 619, 268},                    /* widget size                    */
    &vitals_screen_medtype1_slider_win_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype1_percentage_flag), /* control block */
    (void *) &vitals_screen_medtype1_percentage_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype1_value_define =
{
    "medtype1_value",
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
    {560, 247, 597, 277},                    /* widget size                    */
    &vitals_screen_medtype1_percentage_flag_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype1_value), /* control block */
    (void *) &vitals_screen_medtype1_value_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype1_label_define =
{
    "medtype1_label",
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
    {380, 251, 426, 268},                    /* widget size                    */
    &vitals_screen_medtype1_value_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype1_label), /* control block */
    (void *) &vitals_screen_medtype1_label_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype3_slider_define =
{
    "medtype3_slider",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_MEDTYPE3_SLIDER,                      /* widget id                      */
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
    {381, 432, 619, 451},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype3_slider), /* control block */
    (void *) &vitals_screen_medtype3_slider_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype3_slider_win_define =
{
    "medtype3_slider_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {381, 432, 619, 451},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &vitals_screen_medtype3_slider_define,   /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype3_slider_win), /* control block */
    (void *) &vitals_screen_medtype3_slider_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype3_percentage_flag_define =
{
    "medtype3_percentage_flag",
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
    {599, 396, 619, 418},                    /* widget size                    */
    &vitals_screen_medtype3_slider_win_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype3_percentage_flag), /* control block */
    (void *) &vitals_screen_medtype3_percentage_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype3_value_define =
{
    "medtype3_value",
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
    {560, 397, 597, 427},                    /* widget size                    */
    &vitals_screen_medtype3_percentage_flag_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype3_value), /* control block */
    (void *) &vitals_screen_medtype3_value_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype3_label_define =
{
    "medtype3_label",
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
    {380, 399, 464, 416},                    /* widget size                    */
    &vitals_screen_medtype3_value_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype3_label), /* control block */
    (void *) &vitals_screen_medtype3_label_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype2_slider_define =
{
    "medtype2_slider",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_MEDTYPE2_SLIDER,                      /* widget id                      */
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
    {381, 357, 619, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype2_slider), /* control block */
    (void *) &vitals_screen_medtype2_slider_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype2_slider_win_define =
{
    "medtype2_slider_win",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {381, 357, 619, 376},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &vitals_screen_medtype2_slider_define,   /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype2_slider_win), /* control block */
    (void *) &vitals_screen_medtype2_slider_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype2_percentage_flag_define =
{
    "medtype2_percentage_flag",
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
    {599, 321, 619, 343},                    /* widget size                    */
    &vitals_screen_medtype2_slider_win_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype2_percentage_flag), /* control block */
    (void *) &vitals_screen_medtype2_percentage_flag_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype2_value_define =
{
    "medtype2_value",
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
    {560, 322, 597, 352},                    /* widget size                    */
    &vitals_screen_medtype2_percentage_flag_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype2_value), /* control block */
    (void *) &vitals_screen_medtype2_value_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype2_label_define =
{
    "medtype2_label",
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
    {380, 326, 427, 343},                    /* widget size                    */
    &vitals_screen_medtype2_value_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype2_label), /* control block */
    (void *) &vitals_screen_medtype2_label_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype2_win_define =
{
    "medtype2_win",
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
    {378, 319, 619, 385},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &vitals_screen_medtype2_label_define,    /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype2_win), /* control block */
    (void *) &vitals_screen_medtype2_win_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype3_win_define =
{
    "medtype3_win",
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
    {378, 394, 619, 460},                    /* widget size                    */
    &vitals_screen_medtype2_win_define,      /* next widget definition         */
    &vitals_screen_medtype3_label_define,    /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype3_win), /* control block */
    (void *) &vitals_screen_medtype3_win_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_medtype1_win_define =
{
    "medtype1_win",
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
    {378, 244, 619, 310},                    /* widget size                    */
    &vitals_screen_medtype3_win_define,      /* next widget definition         */
    &vitals_screen_medtype1_label_define,    /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_medtype1_win), /* control block */
    (void *) &vitals_screen_medtype1_win_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_temperature_win_define =
{
    "temperature_win",
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
    {378, 154, 619, 229},                    /* widget size                    */
    &vitals_screen_medtype1_win_define,      /* next widget definition         */
    &vitals_screen_prompt_15_define,         /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_temperature_win), /* control block */
    (void *) &vitals_screen_temperature_win_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_patien_info_win_define =
{
    "patien_info_win",
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
    {378, 77, 619, 151},                     /* widget size                    */
    &vitals_screen_temperature_win_define,   /* next widget definition         */
    &vitals_screen_patient_gender_define,    /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_patien_info_win), /* control block */
    (void *) &vitals_screen_patien_info_win_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_blood_pressure_win_define =
{
    "blood_pressure_win",
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
    {30, 375, 329, 466},                     /* widget size                    */
    &vitals_screen_patien_info_win_define,   /* next widget definition         */
    &vitals_screen_prompt_23_define,         /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_blood_pressure_win), /* control block */
    (void *) &vitals_screen_blood_pressure_win_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_pulse_win_define =
{
    "pulse_win",
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
    {20, 278, 319, 361},                     /* widget size                    */
    &vitals_screen_blood_pressure_win_define, /* next widget definition        */
    &vitals_screen_spo2_icon_define,         /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_pulse_win), /* control block */
    (void *) &vitals_screen_pulse_win_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_ekg_win_define =
{
    "ekg_win",
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
    {20, 180, 319, 263},                     /* widget size                    */
    &vitals_screen_pulse_win_define,         /* next widget definition         */
    &vitals_screen_heart_rate_icon_define,   /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_ekg_win), /* control block */
    (void *) &vitals_screen_ekg_win_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET vitals_screen_insulin_win_define =
{
    "insulin_win",
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
    {20, 81, 319, 164},                      /* widget size                    */
    &vitals_screen_ekg_win_define,           /* next widget definition         */
    &vitals_screen_insulin_define,           /* child widget definition        */
    offsetof(VITALS_SCREEN_CONTROL_BLOCK, vitals_screen_insulin_win), /* control block */
    (void *) &vitals_screen_insulin_win_properties /* extended properties      */
};

GX_ANIMATION_INFO vitals_screen_animation_1 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_insulin_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_SINE_EASE_IN, 0, 0, 1,
    {20, 381}, {20, 81}, 0, 255, 30
};


GX_ANIMATION_INFO vitals_screen_animation_2 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_ekg_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_SINE_EASE_IN, ID_EKG_WIN_SLIDE_IN, 5, 1,
    {20, 480}, {20, 180}, 0, 255, 30
};


GX_ANIMATION_INFO vitals_screen_animation_3 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_pulse_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_SINE_EASE_IN, ID_PULSE_WIN_SLIDE_IN, 10, 1,
    {20, 578}, {20, 278}, 0, 255, 30
};


GX_ANIMATION_INFO vitals_screen_animation_4 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_blood_pressure_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 15, 1,
    {30, 675}, {30, 375}, 0, 255, 30
};


GX_ANIMATION_INFO vitals_screen_animation_5 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_temperature_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 5, 1,
    {378, 154}, {378, 154}, 0, 255, 20
};


GX_ANIMATION_INFO vitals_screen_animation_6 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_medtype1_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 10, 1,
    {378, 244}, {378, 244}, 0, 255, 20
};


GX_ANIMATION_INFO vitals_screen_animation_7 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_medtype2_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 15, 1,
    {378, 319}, {378, 319}, 0, 255, 20
};


GX_ANIMATION_INFO vitals_screen_animation_8 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_medtype3_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 20, 1,
    {378, 394}, {378, 394}, 0, 255, 20
};


GX_ANIMATION_INFO vitals_screen_animation_9 = {
    (GX_WIDGET *) &vitals_screen.vitals_screen_patien_info_win,
    (GX_WIDGET *) &vitals_screen,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {378, 77}, {378, 77}, 0, 255, 20
};


GX_STUDIO_ACTION vitals_screen_on_gx_event_showactions[10] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_insulin_win, &vitals_screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_ekg_win, &vitals_screen_animation_2},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_pulse_win, &vitals_screen_animation_3},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_blood_pressure_win, &vitals_screen_animation_4},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_temperature_win, &vitals_screen_animation_5},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_medtype1_win, &vitals_screen_animation_6},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_medtype2_win, &vitals_screen_animation_7},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_medtype3_win, &vitals_screen_animation_8},
    {GX_ACTION_TYPE_ANIMATION, 0, &vitals_screen, &vitals_screen.vitals_screen_patien_info_win, &vitals_screen_animation_9},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_vitals_screen_event_table[] = {
    { GX_EVENT_SHOW, 0, vitals_screen_on_gx_event_showactions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS vitals_screen_event_chain = {gx_studio_vitals_screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))vitals_screen_event_process};
static UINT gx_studio_vitals_screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &vitals_screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET vitals_screen_define =
{
    "vitals_screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_VITALS_SCREEN,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(VITALS_SCREEN_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_CANVAS,                      /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_DISABLED_FILL,               /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_vitals_screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &vitals_screen_insulin_win_define,       /* child widget                   */
    0,                                       /* control block                  */
    (void *) &vitals_screen_properties       /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY guix_medical_widget_table[] =
{
    { &schedule_screen_define, (GX_WIDGET *) &schedule_screen },
    { &patients_screen_define, (GX_WIDGET *) &patients_screen },
    { &vitals_screen_define, (GX_WIDGET *) &vitals_screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = guix_medical_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &guix_medical_display_table[display];


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
