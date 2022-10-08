#define GUIX_STUDIO_GENERATED_FILE
#include <stddef.h>
#include "guix_widget_types_resources.h"
#include "guix_widget_types_specifications.h"

static GX_WIDGET *gx_studio_nested_widget_create(GX_BYTE *control, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
MENU_SCREEN_CONTROL_BLOCK Menu_Screen;
SCROLL_WHEEL_SCREEN_CONTROL_BLOCK Scroll_Wheel_Screen;
GAUGE_SCREEN_CONTROL_BLOCK Gauge_Screen;
WINDOW_SCREEN_CONTROL_BLOCK Window_Screen;
SLIDER_SCREEN_CONTROL_BLOCK Slider_Screen;
TEXT_SCREEN_CONTROL_BLOCK Text_Screen;
BUTTON_SCREEN_CONTROL_BLOCK Button_Screen;
GX_DISPLAY Primary_control_block;
GX_WINDOW_ROOT Primary_root_window;
GX_CANVAS  Primary_canvas_control_block;
ULONG      Primary_canvas_memory[307200];

extern GX_CONST GX_THEME *Primary_theme_table[];
extern GX_CONST GX_STRING *Primary_language_table[];

GX_STUDIO_DISPLAY_INFO guix_widget_types_display_table[1] =
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

UINT gx_studio_numeric_pixelmap_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_NUMERIC_PIXELMAP_PROMPT *pix_prompt = (GX_NUMERIC_PIXELMAP_PROMPT *) control_block;
    GX_PROMPT *prompt = (GX_PROMPT *) pix_prompt;
    GX_NUMERIC_PIXELMAP_PROMPT_PROPERTIES *props = (GX_NUMERIC_PIXELMAP_PROMPT_PROPERTIES *) info->properties;
    status = gx_numeric_pixelmap_prompt_create(pix_prompt, info->widget_name, parent,
               props->string_id,
               props->fill_map_id,
               info->style, info->widget_id, &info->size);

    if (status == GX_SUCCESS)
    {
        if(!props->string_id)
        {
            gx_numeric_pixelmap_prompt_value_set(pix_prompt, props->numeric_prompt_value);
        }
        if(props->format_func)
        {
            gx_numeric_pixelmap_prompt_format_function_set(pix_prompt, props->format_func);
        }
        gx_pixelmap_prompt_pixelmap_set((GX_PIXELMAP_PROMPT *)pix_prompt,
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

UINT gx_studio_horizontal_list_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_HORIZONTAL_LIST *list = (GX_HORIZONTAL_LIST *) control_block;
    GX_HORIZONTAL_LIST_PROPERTIES *props = (GX_HORIZONTAL_LIST_PROPERTIES *) info->properties;
    status = gx_horizontal_list_create(list, info->widget_name, parent, props->total_rows,
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

UINT gx_studio_string_scroll_wheel_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_STRING_SCROLL_WHEEL *wheel = (GX_STRING_SCROLL_WHEEL *) control_block;
    GX_STRING_SCROLL_WHEEL_PROPERTIES *props = (GX_STRING_SCROLL_WHEEL_PROPERTIES *) info->properties;
    status = gx_string_scroll_wheel_create_ext(wheel, info->widget_name, parent, props->total_rows, GX_NULL, 
                                               info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if(props->string_id_list)
        {
            gx_string_scroll_wheel_string_id_list_set(wheel, props->string_id_list, props->total_rows);
        }
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW *) wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if(props->selected_background)
        {
            gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL *)wheel, props->selected_background);
        }
        gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL *)wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL *)wheel, props->start_alpha, props->end_alpha);
        gx_scroll_wheel_row_height_set((GX_SCROLL_WHEEL *)wheel, props->row_height);
        gx_text_scroll_wheel_font_set((GX_TEXT_SCROLL_WHEEL *)wheel, props->normal_font, props->selected_font);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL *)wheel, props->normal_text_color, props->selected_text_color);
#else
        gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL *)wheel, props->normal_text_color, props->selected_text_color, props->disabled_text_color);
#endif
        if(props->callback)
        {
            gx_text_scroll_wheel_callback_set_ext((GX_TEXT_SCROLL_WHEEL *)wheel, (UINT (*)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING *))props->callback);
        }
    }
    return status;
}

UINT gx_studio_numeric_scroll_wheel_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_NUMERIC_SCROLL_WHEEL *wheel = (GX_NUMERIC_SCROLL_WHEEL *) control_block;
    GX_NUMERIC_SCROLL_WHEEL_PROPERTIES *props = (GX_NUMERIC_SCROLL_WHEEL_PROPERTIES *) info->properties;
    status = gx_numeric_scroll_wheel_create(wheel, info->widget_name, parent, props->start_val, props->end_val, 
                                    info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW *) wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if(props->selected_background)
        {
            gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL *)wheel, props->selected_background);
        }

        if (props->total_rows)
        {
            gx_scroll_wheel_total_rows_set((GX_SCROLL_WHEEL *)wheel, props->total_rows);
        }

        gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL *)wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL *)wheel, props->start_alpha, props->end_alpha);
        gx_scroll_wheel_row_height_set((GX_SCROLL_WHEEL *)wheel, props->row_height);
        gx_text_scroll_wheel_font_set((GX_TEXT_SCROLL_WHEEL *)wheel, props->normal_font, props->selected_font);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL *)wheel, props->normal_text_color, props->selected_text_color);
#else
        gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL *)wheel, props->normal_text_color, props->selected_text_color, props->disabled_text_color);
#endif
        if(props->callback)
        {
            gx_text_scroll_wheel_callback_set_ext((GX_TEXT_SCROLL_WHEEL *)wheel, (UINT (*)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING *))props->callback);
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
             gx_display_string_get_ext(guix_widget_types_display_table[0].display, props->string_id, &text);

             if (text.gx_string_ptr)
             {
                 gx_single_line_text_input_text_set_ext(input, &text);
             }
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

UINT gx_studio_multi_line_text_input_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_STRING text;
    GX_MULTI_LINE_TEXT_INPUT *input = (GX_MULTI_LINE_TEXT_INPUT *) control_block;
    GX_MULTI_LINE_TEXT_INPUT_PROPERTIES *props = (GX_MULTI_LINE_TEXT_INPUT_PROPERTIES *) info->properties;
    status = gx_multi_line_text_input_create(input, info->widget_name, parent, props->buffer, props->buffer_size, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_multi_line_text_view_font_set((GX_MULTI_LINE_TEXT_VIEW *) input, props->font_id);
        gx_multi_line_text_input_fill_color_set(input, info->normal_fill_color_id, info->selected_fill_color_id,
                                                info->disabled_fill_color_id, props->readonly_fill_color_id);
        gx_multi_line_text_input_text_color_set(input, props->normal_text_color_id, props->selected_text_color_id,
                                                props->disabled_text_color_id, props->readonly_text_color_id);
        gx_multi_line_text_view_whitespace_set((GX_MULTI_LINE_TEXT_VIEW *) input, props->whitespace);
        gx_multi_line_text_view_line_space_set((GX_MULTI_LINE_TEXT_VIEW *) input, props->line_space);
        if(props->buffer && (props->buffer_size > 0) && props->string_id)
        {
            gx_display_string_get_ext(guix_widget_types_display_table[0].display, props->string_id, &text);
            if(text.gx_string_ptr)
            {
                gx_multi_line_text_input_text_set_ext(input, &text);
            }
        }
    }
    return status;
}

UINT gx_studio_horizontal_scrollbar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_SCROLLBAR *scroll = (GX_SCROLLBAR *) control_block;
    GX_SCROLLBAR_APPEARANCE *appearance = (GX_SCROLLBAR_APPEARANCE *) info->properties;
    status = gx_horizontal_scrollbar_create(scroll, info->widget_name, parent, appearance, info->style);
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

UINT gx_studio_menu_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_MENU *menu = (GX_MENU *) control_block;
    GX_MENU_PROPERTIES *props = (GX_MENU_PROPERTIES *) info->properties;
    status = gx_menu_create(menu, info->widget_name, parent,
               props->string_id,
               props->fill_map_id,
               info->style, info->widget_id, &info->size);

    if (status == GX_SUCCESS)
    {
        menu->gx_menu_list_total_count = props->list_total_count;
        gx_menu_text_offset_set(menu, props->text_x_offset, props->text_y_offset);
        gx_pixelmap_prompt_pixelmap_set((GX_PIXELMAP_PROMPT *)menu,
                                        props->left_map_id,
                                        props->fill_map_id,
                                        props->right_map_id,
                                        props->selected_left_map_id,
                                        props->selected_fill_map_id,
                                        props->selected_right_map_id);
        gx_prompt_font_set((GX_PROMPT *)menu, props->font_id);
#if defined(GUIX_5_4_0_COMPATIBILITY)
        gx_prompt_text_color_set((GX_PROMPT *)menu, props->normal_text_color_id, props->selected_text_color_id);
#else
        gx_prompt_text_color_set((GX_PROMPT *)menu, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
#endif
    }
    return status;
}

UINT gx_studio_accordion_menu_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_ACCORDION_MENU *accordion = (GX_ACCORDION_MENU *) control_block;
    status = gx_accordion_menu_create(accordion, info->widget_name,
                                      parent, info->style, info->widget_id, &info->size);
    return status;
}

UINT gx_studio_tree_view_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_TREE_VIEW *tree = (GX_TREE_VIEW *) control_block;
    GX_TREE_VIEW_PROPERTIES *props = (GX_TREE_VIEW_PROPERTIES *) info->properties;
    status = gx_tree_view_create(tree, info->widget_name, parent,
                                 info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->expand_map_id || props->collapse_map_id)
        {
            gx_tree_view_root_pixelmap_set(tree, props->expand_map_id, props->collapse_map_id);
        }
        gx_tree_view_indentation_set(tree, props->indentation);
        gx_tree_view_root_line_color_set(tree, props->root_line_color_id);
    }
    return status;
}

UINT gx_studio_rich_text_view_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent)
{
    UINT status;
    GX_RICH_TEXT_VIEW *view = (GX_RICH_TEXT_VIEW *) control_block;
    GX_RICH_TEXT_VIEW_PROPERTIES *props = (GX_RICH_TEXT_VIEW_PROPERTIES *) info->properties;
    status = gx_rich_text_view_create(view, info->widget_name, parent, props->string_id, props->fonts, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_multi_line_text_view_text_color_set((GX_MULTI_LINE_TEXT_VIEW *)view, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
        gx_multi_line_text_view_whitespace_set((GX_MULTI_LINE_TEXT_VIEW *)view, props->whitespace);
        gx_multi_line_text_view_line_space_set((GX_MULTI_LINE_TEXT_VIEW *)view, props->line_space);
    }
    return status;
}
GX_WINDOW_PROPERTIES Base_Screen_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_PROPERTIES Base_Screen_Icon_EL_Logo_properties =
{
    GX_PIXELMAP_ID_MS_AZURE_LOGO_SMALL,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_BUTTON_PROPERTIES Base_Screen_Next_Button_properties =
{
    GX_PIXELMAP_ID_NEXT_BUTTON_2,            /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET Base_Screen_Next_Button_define =
{
    "Next_Button",
    GX_TYPE_PIXELMAP_BUTTON,                 /* widget type                    */
    IDB_NEXT,                                /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_BUTTON),              /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_pixelmap_button_create,        /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_next_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {534, 448, 610, 465},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BASE_SCREEN_CONTROL_BLOCK, Base_Screen_Next_Button), /* control block */
    (void *) &Base_Screen_Next_Button_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Base_Screen_Icon_EL_Logo_define =
{
    "Icon_EL_Logo",
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
    {20, 20, 208, 49},                       /* widget size                    */
    &Base_Screen_Next_Button_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BASE_SCREEN_CONTROL_BLOCK, Base_Screen_Icon_EL_Logo), /* control block */
    (void *) &Base_Screen_Icon_EL_Logo_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET Base_Screen_define =
{
    "Base_Screen",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(BASE_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Base_Screen_Icon_EL_Logo_define,        /* child widget                   */
    0,                                       /* control block                  */
    (void *) &Base_Screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Gauge_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES Gauge_Screen_Prompt_Circular_Gauges_properties =
{
    GX_STRING_ID_STRING_55,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_CIRCULAR_GAUGE_PROPERTIES Gauge_Screen_Gauge_Animated_properties =
{
    0,                                       /* start angle                    */
    20,                                      /* animation steps                */
    1,                                       /* animation delay                */
    120,                                     /* needle xpos                    */
    123,                                     /* needle yos                     */
    11,                                      /* needle xcor                    */
    93,                                      /* needle ycor                    */
    GX_PIXELMAP_ID_CIRCULAR_GAUGE_BG,        /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    GX_PIXELMAP_ID_CIRCULAR_GAUGE_NEEDLE     /* needle pixelmap id             */
};
GX_PROMPT_PROPERTIES Gauge_Screen_prompt_animated_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_LARGE_FONT,                   /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_SLIDER_PROPERTIES Gauge_Screen_Pixelmap_Slider_Animated_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    0,                                       /* current value                  */
    10,                                      /* increment                      */
    20,                                      /* minimum travel                 */
    20,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    20,                                      /* needle height                  */
    5,                                       /* needle inset                   */
    10,                                      /* needle hotspot                 */
    GX_PIXELMAP_ID_SLIDER_THIN_ACTIVE_HORIZONTAL, /* lower pixelmap id         */
    GX_PIXELMAP_ID_SLIDER_THIN_HORIZONTAL,   /* upper pixelmap id              */
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id           */
};
GX_PIXELMAP_SLIDER_PROPERTIES Gauge_Screen_Pixelmap_Slider_Not_Animated_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    0,                                       /* current value                  */
    10,                                      /* increment                      */
    20,                                      /* minimum travel                 */
    20,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    20,                                      /* needle height                  */
    5,                                       /* needle inset                   */
    10,                                      /* needle hotspot                 */
    GX_PIXELMAP_ID_SLIDER_THIN_ACTIVE_HORIZONTAL, /* lower pixelmap id         */
    GX_PIXELMAP_ID_SLIDER_THIN_HORIZONTAL,   /* upper pixelmap id              */
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id           */
};
GX_PROMPT_PROPERTIES Gauge_Screen_Prompt_Animated_properties =
{
    GX_STRING_ID_STRING_57,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Gauge_Screen_Prompt_Not_Animated_properties =
{
    GX_STRING_ID_STRING_58,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_CIRCULAR_GAUGE_PROPERTIES Gauge_Screen_Gauge_Not_Animated_properties =
{
    0,                                       /* start angle                    */
    0,                                       /* animation steps                */
    1,                                       /* animation delay                */
    120,                                     /* needle xpos                    */
    123,                                     /* needle yos                     */
    11,                                      /* needle xcor                    */
    93,                                      /* needle ycor                    */
    GX_PIXELMAP_ID_CIRCULAR_GAUGE_BG,        /* normal pixelmap id             */
    0,                                       /* selected pixelmap id           */
    GX_PIXELMAP_ID_CIRCULAR_GAUGE_NEEDLE     /* needle pixelmap id             */
};
GX_PROMPT_PROPERTIES Gauge_Screen_prompt_not_animated_properties =
{
    GX_STRING_ID_STRING_7,                   /* string id                      */
    GX_FONT_ID_LARGE_FONT,                   /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_prompt_animated_define =
{
    "prompt_animated",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_PROMPT_ANIMATED,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_DARK_TEAL,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_TEAL,                   /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {178, 294, 229, 331},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_prompt_animated), /* control block */
    (void *) &Gauge_Screen_prompt_animated_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_prompt_not_animated_define =
{
    "prompt_not_animated",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_PROMPT_NOT_ANIMATED,                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_DARK_TEAL,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_TEAL,                   /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {452, 294, 503, 331},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_prompt_not_animated), /* control block */
    (void *) &Gauge_Screen_prompt_not_animated_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Gauge_Not_Animated_define =
{
    "Gauge_Not_Animated",
    GX_TYPE_CIRCULAR_GAUGE,                  /* widget type                    */
    ID_GAUGE_NOT_ANIMATED,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CIRCULAR_GAUGE),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_circular_gauge_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {359, 120, 595, 357},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Gauge_Screen_prompt_not_animated_define, /* child widget definition       */
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Gauge_Not_Animated), /* control block */
    (void *) &Gauge_Screen_Gauge_Not_Animated_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Prompt_Not_Animated_define =
{
    "Prompt_Not_Animated",
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
    {359, 378, 496, 398},                    /* widget size                    */
    &Gauge_Screen_Gauge_Not_Animated_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Prompt_Not_Animated), /* control block */
    (void *) &Gauge_Screen_Prompt_Not_Animated_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Prompt_Animated_define =
{
    "Prompt_Animated",
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
    {80, 378, 180, 401},                     /* widget size                    */
    &Gauge_Screen_Prompt_Not_Animated_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Prompt_Animated), /* control block */
    (void *) &Gauge_Screen_Prompt_Animated_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Pixelmap_Slider_Not_Animated_define =
{
    "Pixelmap_Slider_Not_Animated",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_SLIDER_NOT_ANIMATED,                  /* widget id                      */
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
    {359, 402, 595, 437},                    /* widget size                    */
    &Gauge_Screen_Prompt_Animated_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Pixelmap_Slider_Not_Animated), /* control block */
    (void *) &Gauge_Screen_Pixelmap_Slider_Not_Animated_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Pixelmap_Slider_Animated_define =
{
    "Pixelmap_Slider_Animated",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_SLIDER_ANIMATED,                      /* widget id                      */
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
    {80, 402, 316, 437},                     /* widget size                    */
    &Gauge_Screen_Pixelmap_Slider_Not_Animated_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Pixelmap_Slider_Animated), /* control block */
    (void *) &Gauge_Screen_Pixelmap_Slider_Animated_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Gauge_Animated_define =
{
    "Gauge_Animated",
    GX_TYPE_CIRCULAR_GAUGE,                  /* widget type                    */
    ID_GAUGE_ANIMATED,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_CIRCULAR_GAUGE),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_circular_gauge_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {85, 120, 321, 357},                     /* widget size                    */
    &Gauge_Screen_Pixelmap_Slider_Animated_define, /* next widget definition   */
    &Gauge_Screen_prompt_animated_define,    /* child widget definition        */
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Gauge_Animated), /* control block */
    (void *) &Gauge_Screen_Gauge_Animated_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Gauge_Screen_Prompt_Circular_Gauges_define =
{
    "Prompt_Circular_Gauges",
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
    {256, 64, 415, 84},                      /* widget size                    */
    &Gauge_Screen_Gauge_Animated_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(GAUGE_SCREEN_CONTROL_BLOCK, Gauge_Screen_Prompt_Circular_Gauges), /* control block */
    (void *) &Gauge_Screen_Prompt_Circular_Gauges_properties /* extended properties */
};

GX_ANIMATION_INFO Gauge_Screen_animation_1 = {
    (GX_WIDGET *) &Scroll_Wheel_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {0, 0}, {0, 0}, 0, 255, 10
};


GX_STUDIO_ACTION Gauge_Screen__idb_next_gx_event_clicked_actions[3] = {
    {GX_ACTION_TYPE_TOGGLE, 0, &Primary_root_window, &Scroll_Wheel_Screen, GX_NULL},
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Scroll_Wheel_Screen, &Gauge_Screen_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Gauge_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Gauge_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Gauge_Screen_event_chain = {gx_studio_Gauge_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))gauge_window_event_handler};
static UINT gx_studio_Gauge_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Gauge_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Gauge_Screen_define =
{
    "Gauge_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_GAUGE_SCREEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GAUGE_SCREEN_CONTROL_BLOCK),      /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Gauge_Screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Gauge_Screen_Prompt_Circular_Gauges_define, /* child widget               */
    0,                                       /* control block                  */
    (void *) &Gauge_Screen_properties        /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Window_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES Window_Screen_Prompt_Nested_Windows_properties =
{
    GX_STRING_ID_STRING_45,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Window_Screen_Prompt_Scollable_Frame_properties =
{
    GX_STRING_ID_STRING_46,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_WINDOW_PROPERTIES Window_Screen_Nested_Window_BG_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Window_Screen_Nested_Window_Blue_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Window_Screen_nest_window_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Window_Screen_Scrollable_Frame_BG_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_WINDOW_PROPERTIES Window_Screen_Scrollable_Frame_Sky_properties =
{
    GX_PIXELMAP_ID_SCROLLABLE_WINDOW_BG      /* wallpaper pixelmap id          */
};
GX_SCROLLBAR_APPEARANCE  Window_Screen_Hscroll_properties =
{
    30,                                      /* scroll width                   */
    8,                                       /* thumb width                    */
    20,                                      /* thumb travel min               */
    20,                                      /* thumb travel max               */
    4,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    0,                                       /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_WHITE,                       /* scroll thumb color             */
    GX_COLOR_ID_WHITE,                       /* scroll thumb border color      */
    GX_COLOR_ID_WHITE,                       /* scroll button color            */
};
GX_SCROLLBAR_APPEARANCE  Window_Screen_Vertical_Scroll_properties =
{
    20,                                      /* scroll width                   */
    8,                                       /* thumb width                    */
    20,                                      /* thumb travel min               */
    20,                                      /* thumb travel max               */
    4,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    0,                                       /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_WHITE,                       /* scroll thumb color             */
    GX_COLOR_ID_WHITE,                       /* scroll thumb border color      */
    GX_COLOR_ID_WHITE,                       /* scroll button color            */
};
GX_PROMPT_PROPERTIES Window_Screen_Prompt_Vertical_List_properties =
{
    GX_STRING_ID_STRING_47,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Window_Screen_Prompt_Horizontal_List_properties =
{
    GX_STRING_ID_STRING_49,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_VERTICAL_LIST_PROPERTIES Window_Screen_Vertical_List_properties =
{
    0,                                       /* wallpaper id                   */
    GX_NULL,                                 /* callback function              */
    3                                        /* total rows                     */
};
GX_WINDOW_PROPERTIES Window_Screen_window_6_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEXT_BUTTON_PROPERTIES Window_Screen_button_1_properties =
{
    GX_STRING_ID_STRING_51,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_WINDOW_PROPERTIES Window_Screen_window_8_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEXT_BUTTON_PROPERTIES Window_Screen_button_3_properties =
{
    GX_STRING_ID_STRING_53,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_WINDOW_PROPERTIES Window_Screen_window_7_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_TEXT_BUTTON_PROPERTIES Window_Screen_button_2_properties =
{
    GX_STRING_ID_STRING_54,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_LIGHT_GRAY,                  /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_HORIZONTAL_LIST_PROPERTIES Window_Screen_Horizontal_List_properties =
{
    0,                                       /* wallpaper id                   */
    GX_NULL,                                 /* callback function              */
    5                                        /* total columns                  */
};
GX_WINDOW_PROPERTIES Window_Screen_window_5_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_BUTTON_PROPERTIES Window_Screen_icon_button_1_properties =
{
    0                                        /* pixelmap id                    */
};
GX_WINDOW_PROPERTIES Window_Screen_window_9_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_BUTTON_PROPERTIES Window_Screen_icon_button_2_properties =
{
    0                                        /* pixelmap id                    */
};
GX_WINDOW_PROPERTIES Window_Screen_window_10_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_BUTTON_PROPERTIES Window_Screen_icon_button_3_properties =
{
    0                                        /* pixelmap id                    */
};
GX_WINDOW_PROPERTIES Window_Screen_window_11_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_BUTTON_PROPERTIES Window_Screen_icon_button_4_properties =
{
    0                                        /* pixelmap id                    */
};
GX_WINDOW_PROPERTIES Window_Screen_window_12_properties =
{
    0                                        /* wallpaper pixelmap id          */
};
GX_ICON_BUTTON_PROPERTIES Window_Screen_icon_button_5_properties =
{
    0                                        /* pixelmap id                    */
};
GX_DROP_LIST_PROPERTIES Window_Screen_Drop_List_properties =
{
    0,                                       /* widget pixelmap id             */
    0,                                       /* popup list wallpaper pixelmap id */
    drop_list_row_create,                    /* callback function              */
    100,                                     /* total rows                     */
    132                                      /* open height                    */
};
GX_PIXELMAP_BUTTON_PROPERTIES Window_Screen_Drop_List_Button_properties =
{
    GX_PIXELMAP_ID_ICON_DROPDOWN,            /* normal pixelmap id             */
    GX_PIXELMAP_ID_ICON_DROPDOWN_ACTIVE,     /* selected pixelmap id           */
    0                                        /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES Window_Screen_Prompt_Drop_List_properties =
{
    GX_STRING_ID_STRING_52,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_nest_window_define =
{
    "nest_window",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_ORANGE,                      /* normal color id                */
    GX_COLOR_ID_ORANGE,                      /* selected color id              */
    GX_COLOR_ID_ORANGE,                      /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) nested_parent_window_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {180, 140, 289, 219},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_nest_window), /* control block */
    (void *) &Window_Screen_nest_window_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Nested_Window_Blue_define =
{
    "Nested_Window_Blue",
    GX_TYPE_WINDOW,                          /* widget type                    */
    ID_NESTED_WINDOW_BLUE,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_NEXT_BUTTON_LOWER,           /* normal color id                */
    GX_COLOR_ID_NEXT_BUTTON_LOWER,           /* selected color id              */
    GX_COLOR_ID_NEXT_BUTTON_LOWER,           /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {105, 115, 215, 200},                    /* widget size                    */
    &Window_Screen_nest_window_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Nested_Window_Blue), /* control block */
    (void *) &Window_Screen_Nested_Window_Blue_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Vertical_Scroll_define =
{
    "Vertical_Scroll",
    GX_TYPE_VERTICAL_SCROLL,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_SCROLLBAR_RELATIVE_THUMB|GX_SCROLLBAR_VERTICAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_vertical_scrollbar_create,     /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {556, 95, 575, 219},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Vertical_Scroll), /* control block */
    (void *) &Window_Screen_Vertical_Scroll_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Hscroll_define =
{
    "Hscroll",
    GX_TYPE_HORIZONTAL_SCROLL,               /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_SCROLLBAR_RELATIVE_THUMB|GX_SCROLLBAR_HORIZONTAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_TEAL,                        /* normal color id                */
    GX_COLOR_ID_TEAL,                        /* selected color id              */
    GX_COLOR_ID_TEAL,                        /* disabled color id              */
    gx_studio_horizontal_scrollbar_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {355, 210, 575, 239},                    /* widget size                    */
    &Window_Screen_Vertical_Scroll_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Hscroll), /* control block */
    (void *) &Window_Screen_Hscroll_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Scrollable_Frame_Sky_define =
{
    "Scrollable_Frame_Sky",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {355, 95, 985, 540},                     /* widget size                    */
    &Window_Screen_Hscroll_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Scrollable_Frame_Sky), /* control block */
    (void *) &Window_Screen_Scrollable_Frame_Sky_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_button_1_define =
{
    "button_1",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_text_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {90, 281, 238, 324},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_button_1), /* control block */
    (void *) &Window_Screen_button_1_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_button_3_define =
{
    "button_3",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_text_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {90, 337, 238, 380},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_button_3), /* control block */
    (void *) &Window_Screen_button_3_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_button_2_define =
{
    "button_2",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_text_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {89, 392, 237, 435},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_button_2), /* control block */
    (void *) &Window_Screen_button_2_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_7_define =
{
    "window_7",
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
    {90, 391, 240, 445},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Window_Screen_button_2_define,          /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_7), /* control block */
    (void *) &Window_Screen_window_7_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_8_define =
{
    "window_8",
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
    {90, 336, 240, 390},                     /* widget size                    */
    &Window_Screen_window_7_define,          /* next widget definition         */
    &Window_Screen_button_3_define,          /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_8), /* control block */
    (void *) &Window_Screen_window_8_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_6_define =
{
    "window_6",
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
    {90, 280, 240, 335},                     /* widget size                    */
    &Window_Screen_window_8_define,          /* next widget definition         */
    &Window_Screen_button_1_define,          /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_6), /* control block */
    (void *) &Window_Screen_window_6_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_icon_button_1_define =
{
    "icon_button_1",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    ICON_BUTTON_DISC,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_icon_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {274, 388, 322, 431},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_icon_button_1), /* control block */
    (void *) &Window_Screen_icon_button_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_icon_button_2_define =
{
    "icon_button_2",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    ICON_BUTTON_USER,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_icon_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {337, 389, 385, 432},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_icon_button_2), /* control block */
    (void *) &Window_Screen_icon_button_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_icon_button_3_define =
{
    "icon_button_3",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    ICON_BUTTON_PEN,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_icon_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {398, 389, 446, 432},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_icon_button_3), /* control block */
    (void *) &Window_Screen_icon_button_3_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_icon_button_4_define =
{
    "icon_button_4",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    ICON_BUTTON_TRASH,                       /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_icon_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {458, 389, 506, 432},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_icon_button_4), /* control block */
    (void *) &Window_Screen_icon_button_4_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_icon_button_5_define =
{
    "icon_button_5",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    ICON_BUTTON_COG,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_icon_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {518, 389, 566, 432},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_icon_button_5), /* control block */
    (void *) &Window_Screen_icon_button_5_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_12_define =
{
    "window_12",
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
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {513, 385, 572, 439},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Window_Screen_icon_button_5_define,     /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_12), /* control block */
    (void *) &Window_Screen_window_12_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_11_define =
{
    "window_11",
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
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {453, 385, 512, 439},                    /* widget size                    */
    &Window_Screen_window_12_define,         /* next widget definition         */
    &Window_Screen_icon_button_4_define,     /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_11), /* control block */
    (void *) &Window_Screen_window_11_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_10_define =
{
    "window_10",
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
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {393, 385, 452, 439},                    /* widget size                    */
    &Window_Screen_window_11_define,         /* next widget definition         */
    &Window_Screen_icon_button_3_define,     /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_10), /* control block */
    (void *) &Window_Screen_window_10_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_9_define =
{
    "window_9",
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
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {333, 385, 392, 439},                    /* widget size                    */
    &Window_Screen_window_10_define,         /* next widget definition         */
    &Window_Screen_icon_button_2_define,     /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_9), /* control block */
    (void *) &Window_Screen_window_9_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_window_5_define =
{
    "window_5",
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
    {273, 385, 332, 439},                    /* widget size                    */
    &Window_Screen_window_9_define,          /* next widget definition         */
    &Window_Screen_icon_button_1_define,     /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_window_5), /* control block */
    (void *) &Window_Screen_window_5_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Drop_List_Button_define =
{
    "Drop_List_Button",
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
    {521, 284, 568, 326},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Drop_List_Button), /* control block */
    (void *) &Window_Screen_Drop_List_Button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Prompt_Drop_List_define =
{
    "Prompt_Drop_List",
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
    {273, 254, 368, 277},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Prompt_Drop_List), /* control block */
    (void *) &Window_Screen_Prompt_Drop_List_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Drop_List_define =
{
    "Drop_List",
    GX_TYPE_DROP_LIST,                       /* widget type                    */
    ID_DROP_LIST,                            /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK,                   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_DROP_LIST),                    /* control block size             */
    GX_COLOR_ID_TEAL,                        /* normal color id                */
    GX_COLOR_ID_TEAL,                        /* selected color id              */
    GX_COLOR_ID_TEAL,                        /* disabled color id              */
    gx_studio_drop_list_create,              /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {273, 283, 568, 326},                    /* widget size                    */
    &Window_Screen_Prompt_Drop_List_define,  /* next widget definition         */
    &Window_Screen_Drop_List_Button_define,  /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Drop_List), /* control block */
    (void *) &Window_Screen_Drop_List_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Horizontal_List_define =
{
    "Horizontal_List",
    GX_TYPE_HORIZONTAL_LIST,                 /* widget type                    */
    ID_HORIZONTAL_LIST,                      /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_HORIZONTAL_LIST),              /* control block size             */
    GX_COLOR_ID_TEAL,                        /* normal color id                */
    GX_COLOR_ID_TEAL,                        /* selected color id              */
    GX_COLOR_ID_TEAL,                        /* disabled color id              */
    gx_studio_horizontal_list_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {273, 385, 574, 439},                    /* widget size                    */
    &Window_Screen_Drop_List_define,         /* next widget definition         */
    &Window_Screen_window_5_define,          /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Horizontal_List), /* control block */
    (void *) &Window_Screen_Horizontal_List_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Vertical_List_define =
{
    "Vertical_List",
    GX_TYPE_VERTICAL_LIST,                   /* widget type                    */
    ID_VERTICAL_LIST,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_VERTICAL_LIST),                /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_vertical_list_create,          /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {90, 280, 240, 445},                     /* widget size                    */
    &Window_Screen_Horizontal_List_define,   /* next widget definition         */
    &Window_Screen_window_6_define,          /* child widget definition        */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Vertical_List), /* control block */
    (void *) &Window_Screen_Vertical_List_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Prompt_Horizontal_List_define =
{
    "Prompt_Horizontal_List",
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
    {273, 356, 422, 379},                    /* widget size                    */
    &Window_Screen_Vertical_List_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Prompt_Horizontal_List), /* control block */
    (void *) &Window_Screen_Prompt_Horizontal_List_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Prompt_Vertical_List_define =
{
    "Prompt_Vertical_List",
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
    {89, 253, 215, 276},                     /* widget size                    */
    &Window_Screen_Prompt_Horizontal_List_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Prompt_Vertical_List), /* control block */
    (void *) &Window_Screen_Prompt_Vertical_List_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Scrollable_Frame_BG_define =
{
    "Scrollable_Frame_BG",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_TEAL,                        /* normal color id                */
    GX_COLOR_ID_TEAL,                        /* selected color id              */
    GX_COLOR_ID_TEAL,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) scroll_frame_draw, /* drawing function override    */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) scroll_frame_event_handler, /* event function override */
    {355, 95, 575, 239},                     /* widget size                    */
    &Window_Screen_Prompt_Vertical_List_define, /* next widget definition      */
    &Window_Screen_Scrollable_Frame_Sky_define, /* child widget definition     */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Scrollable_Frame_BG), /* control block */
    (void *) &Window_Screen_Scrollable_Frame_BG_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Nested_Window_BG_define =
{
    "Nested_Window_BG",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_TEAL,                        /* normal color id                */
    GX_COLOR_ID_TEAL,                        /* selected color id              */
    GX_COLOR_ID_TEAL,                        /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {90, 95, 310, 238},                      /* widget size                    */
    &Window_Screen_Scrollable_Frame_BG_define, /* next widget definition       */
    &Window_Screen_Nested_Window_Blue_define, /* child widget definition       */
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Nested_Window_BG), /* control block */
    (void *) &Window_Screen_Nested_Window_BG_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Prompt_Scollable_Frame_define =
{
    "Prompt_Scollable_Frame",
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
    {354, 70, 530, 93},                      /* widget size                    */
    &Window_Screen_Nested_Window_BG_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Prompt_Scollable_Frame), /* control block */
    (void *) &Window_Screen_Prompt_Scollable_Frame_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Window_Screen_Prompt_Nested_Windows_define =
{
    "Prompt_Nested_Windows",
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
    {90, 70, 261, 93},                       /* widget size                    */
    &Window_Screen_Prompt_Scollable_Frame_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(WINDOW_SCREEN_CONTROL_BLOCK, Window_Screen_Prompt_Nested_Windows), /* control block */
    (void *) &Window_Screen_Prompt_Nested_Windows_properties /* extended properties */
};

GX_ANIMATION_INFO Window_Screen_animation_1 = {
    (GX_WIDGET *) &Gauge_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {0, 0}, {0, 0}, 0, 255, 10
};


GX_STUDIO_ACTION Window_Screen__idb_next_gx_event_clicked_actions[3] = {
    {GX_ACTION_TYPE_HIDE, 0, &Primary_root_window, &Window_Screen, GX_NULL},
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Gauge_Screen, &Window_Screen_animation_1},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Window_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Window_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Window_Screen_event_chain = {gx_studio_Window_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))window_screen_event_handler};
static UINT gx_studio_Window_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Window_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Window_Screen_define =
{
    "Window_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_WINDOW_SCREEN,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TILE_WALLPAPER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(WINDOW_SCREEN_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Window_Screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Window_Screen_Prompt_Nested_Windows_define, /* child widget               */
    0,                                       /* control block                  */
    (void *) &Window_Screen_properties       /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Slider_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES Slider_Screen_Prompt_Various_Sliders_properties =
{
    GX_STRING_ID_STRING_44,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixelmap_Slider_H_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    20,                                      /* current value                  */
    10,                                      /* increment                      */
    11,                                      /* minimum travel                 */
    19,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    8,                                       /* needle inset                   */
    2,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_SLIDER_BG_HORIZONTAL,     /* lower pixelmap id              */
    0,                                       /* upper pixelmap id              */
    GX_PIXELMAP_ID_SLIDER_NEEDLES_HORIZONTAL  /* needle pixelmap id            */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixelmap_Slider_V_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    77,                                      /* current value                  */
    10,                                      /* increment                      */
    20,                                      /* minimum travel                 */
    9,                                       /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    11,                                      /* needle inset                   */
    2,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_SLIDER_BG_VERTICAL,       /* lower pixelmap id              */
    0,                                       /* upper pixelmap id              */
    GX_PIXELMAP_ID_SLIDER_NEEDLES_VERTICAL   /* needle pixelmap id             */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixelmap_Slider_Thin_H_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    75,                                      /* current value                  */
    10,                                      /* increment                      */
    20,                                      /* minimum travel                 */
    20,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    20,                                      /* needle height                  */
    5,                                       /* needle inset                   */
    10,                                      /* needle hotspot                 */
    GX_PIXELMAP_ID_SLIDER_THIN_ACTIVE_HORIZONTAL, /* lower pixelmap id         */
    GX_PIXELMAP_ID_SLIDER_THIN_HORIZONTAL,   /* upper pixelmap id              */
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id           */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixelmap_Slider_Thin_V_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    80,                                      /* current value                  */
    10,                                      /* increment                      */
    30,                                      /* minimum travel                 */
    20,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    3,                                       /* needle inset                   */
    2,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_SLIDER_THIN_ACTIVE_VERTICAL, /* lower pixelmap id           */
    GX_PIXELMAP_ID_SLIDER_THIN_VERTICAL,     /* upper pixelmap id              */
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL  /* needle pixelmap id           */
};
GX_WINDOW_PROPERTIES Slider_Screen_Progress_Bar_BG_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_PROGRESS_BG   /* wallpaper pixelmap id          */
};
GX_PROGRESS_BAR_INFO Slider_Screen_Progress_Bar_properties =
{
    0,                                       /* mimimun value                  */
    100,                                     /* maximum value                  */
    75,                                      /* current value                  */
    GX_FONT_ID_SYSTEM,                       /* font_id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_SLIDER_THIN_PROGRESS_ZEBRA  /* fill pixelmap                */
};
GX_PROMPT_PROPERTIES Slider_Screen_Progress_Bar_Prompt_properties =
{
    GX_STRING_ID_STRING_60,                  /* string id                      */
    GX_FONT_ID_SYSTEM,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixelmap_Slider_Red_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    10,                                      /* increment                      */
    12,                                      /* minimum travel                 */
    8,                                       /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    3,                                       /* needle inset                   */
    7,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_I_REDFILL_MIDDLE,         /* lower pixelmap id              */
    GX_PIXELMAP_ID_I_EMPTYFILL2_MIDDLE,      /* upper pixelmap id              */
    GX_PIXELMAP_ID_I_INDICATOR               /* needle pixelmap id             */
};
GX_ICON_PROPERTIES Slider_Screen_Icon_Red_Top_properties =
{
    GX_PIXELMAP_ID_I_EMPTYFILL_TOP,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES Slider_Screen_Icon_Red_Bottom_properties =
{
    GX_PIXELMAP_ID_I_REDFILL_BOTTOM,         /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixelmap_Slider_Green_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    10,                                      /* increment                      */
    10,                                      /* minimum travel                 */
    10,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    0,                                       /* needle inset                   */
    8,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_I_GREENFILL_MIDDLE,       /* lower pixelmap id              */
    GX_PIXELMAP_ID_I_EMPTYFILL_MIDDLE,       /* upper pixelmap id              */
    GX_PIXELMAP_ID_I_INDICATOR               /* needle pixelmap id             */
};
GX_ICON_PROPERTIES Slider_Screen_Icon_Green_Top_properties =
{
    GX_PIXELMAP_ID_I_EMPTYFILL_TOP,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES Slider_Screen_Icon_Green_Bottom_properties =
{
    GX_PIXELMAP_ID_I_GREENFILL_BOTTOM,       /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PIXELMAP_SLIDER_PROPERTIES Slider_Screen_Pixlemap_Slider_Orange_properties =
{
    0,                                       /* minimum value                  */
    100,                                     /* maximum value                  */
    50,                                      /* current value                  */
    10,                                      /* increment                      */
    10,                                      /* minimum travel                 */
    10,                                      /* maximum travel                 */
    5,                                       /* needle width                   */
    10,                                      /* needle height                  */
    0,                                       /* needle inset                   */
    8,                                       /* needle hotspot                 */
    GX_PIXELMAP_ID_I_ORANGEFILL_MIDDLE,      /* lower pixelmap id              */
    GX_PIXELMAP_ID_I_EMPTYFILL_MIDDLE,       /* upper pixelmap id              */
    GX_PIXELMAP_ID_I_INDICATOR               /* needle pixelmap id             */
};
GX_ICON_PROPERTIES Slider_Screen_Icon_Orange_Bottom_properties =
{
    GX_PIXELMAP_ID_I_ORANGEFILL_BOTTOM,      /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES Slider_Screen_Icon_Orange_Top_properties =
{
    GX_PIXELMAP_ID_I_EMPTYFILL_TOP,          /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Progress_Bar_Prompt_define =
{
    "Progress_Bar_Prompt",
    GX_TYPE_PROMPT,                          /* widget type                    */
    ID_PROGRESS_BAR_PROMPT,                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_RIGHT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {517, 206, 561, 236},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Progress_Bar_Prompt), /* control block */
    (void *) &Slider_Screen_Progress_Bar_Prompt_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Progress_Bar_define =
{
    "Progress_Bar",
    GX_TYPE_PROGRESS_BAR,                    /* widget type                    */
    ID_PROGRESS_BAR,                         /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_PROGRESS_PERCENT,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROGRESS_BAR),                 /* control block size             */
    GX_COLOR_ID_DARK_TEAL,                   /* normal color id                */
    GX_COLOR_ID_CANVAS,                      /* selected color id              */
    GX_COLOR_ID_DARK_TEAL,                   /* disabled color id              */
    gx_studio_progress_bar_create,           /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {275, 206, 562, 236},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Slider_Screen_Progress_Bar_Prompt_define, /* child widget definition      */
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Progress_Bar), /* control block */
    (void *) &Slider_Screen_Progress_Bar_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Icon_Orange_Top_define =
{
    "Icon_Orange_Top",
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
    {530, 273, 551, 282},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Icon_Orange_Top), /* control block */
    (void *) &Slider_Screen_Icon_Orange_Top_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Icon_Orange_Bottom_define =
{
    "Icon_Orange_Bottom",
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
    {530, 404, 551, 413},                    /* widget size                    */
    &Slider_Screen_Icon_Orange_Top_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Icon_Orange_Bottom), /* control block */
    (void *) &Slider_Screen_Icon_Orange_Bottom_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixlemap_Slider_Orange_define =
{
    "Pixlemap_Slider_Orange",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_RAISED|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_SLIDER_VERTICAL|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {522, 280, 560, 403},                    /* widget size                    */
    &Slider_Screen_Icon_Orange_Bottom_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixlemap_Slider_Orange), /* control block */
    (void *) &Slider_Screen_Pixlemap_Slider_Orange_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Icon_Green_Bottom_define =
{
    "Icon_Green_Bottom",
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
    {403, 404, 424, 413},                    /* widget size                    */
    &Slider_Screen_Pixlemap_Slider_Orange_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Icon_Green_Bottom), /* control block */
    (void *) &Slider_Screen_Icon_Green_Bottom_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Icon_Green_Top_define =
{
    "Icon_Green_Top",
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
    {403, 273, 424, 282},                    /* widget size                    */
    &Slider_Screen_Icon_Green_Bottom_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Icon_Green_Top), /* control block */
    (void *) &Slider_Screen_Icon_Green_Top_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixelmap_Slider_Green_define =
{
    "Pixelmap_Slider_Green",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_RAISED|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_SLIDER_VERTICAL|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {395, 280, 433, 403},                    /* widget size                    */
    &Slider_Screen_Icon_Green_Top_define,    /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixelmap_Slider_Green), /* control block */
    (void *) &Slider_Screen_Pixelmap_Slider_Green_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Icon_Red_Bottom_define =
{
    "Icon_Red_Bottom",
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
    {269, 404, 311, 422},                    /* widget size                    */
    &Slider_Screen_Pixelmap_Slider_Green_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Icon_Red_Bottom), /* control block */
    (void *) &Slider_Screen_Icon_Red_Bottom_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Icon_Red_Top_define =
{
    "Icon_Red_Top",
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
    {280, 271, 301, 280},                    /* widget size                    */
    &Slider_Screen_Icon_Red_Bottom_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Icon_Red_Top), /* control block */
    (void *) &Slider_Screen_Icon_Red_Top_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixelmap_Slider_Red_define =
{
    "Pixelmap_Slider_Red",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_RAISED|GX_STYLE_ENABLED|GX_STYLE_SLIDER_VERTICAL|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {270, 280, 309, 403},                    /* widget size                    */
    &Slider_Screen_Icon_Red_Top_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixelmap_Slider_Red), /* control block */
    (void *) &Slider_Screen_Pixelmap_Slider_Red_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Progress_Bar_BG_define =
{
    "Progress_Bar_BG",
    GX_TYPE_WINDOW,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT,   /* style flags                */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_WINDOW),                       /* control block size             */
    GX_COLOR_ID_DARK_TEAL,                   /* normal color id                */
    GX_COLOR_ID_DARK_TEAL,                   /* selected color id              */
    GX_COLOR_ID_DARK_TEAL,                   /* disabled color id              */
    gx_studio_window_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {270, 201, 572, 241},                    /* widget size                    */
    &Slider_Screen_Pixelmap_Slider_Red_define, /* next widget definition       */
    &Slider_Screen_Progress_Bar_define,      /* child widget definition        */
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Progress_Bar_BG), /* control block */
    (void *) &Slider_Screen_Progress_Bar_BG_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixelmap_Slider_Thin_V_define =
{
    "Pixelmap_Slider_Thin_V",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_SLIDER_VERTICAL|GX_STYLE_TILE_BACKGROUND,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {162, 83, 187, 344},                     /* widget size                    */
    &Slider_Screen_Progress_Bar_BG_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixelmap_Slider_Thin_V), /* control block */
    (void *) &Slider_Screen_Pixelmap_Slider_Thin_V_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixelmap_Slider_Thin_H_define =
{
    "Pixelmap_Slider_Thin_H",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_PIXELMAP_SLIDER_THIN_H,               /* widget id                      */
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
    {272, 147, 572, 182},                    /* widget size                    */
    &Slider_Screen_Pixelmap_Slider_Thin_V_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixelmap_Slider_Thin_H), /* control block */
    (void *) &Slider_Screen_Pixelmap_Slider_Thin_H_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixelmap_Slider_V_define =
{
    "Pixelmap_Slider_V",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_PIXELMAP_SLIDER_V,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_SLIDER_VERTICAL,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {79, 83, 123, 344},                      /* widget size                    */
    &Slider_Screen_Pixelmap_Slider_Thin_H_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixelmap_Slider_V), /* control block */
    (void *) &Slider_Screen_Pixelmap_Slider_V_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Pixelmap_Slider_H_define =
{
    "Pixelmap_Slider_H",
    GX_TYPE_PIXELMAP_SLIDER,                 /* widget type                    */
    ID_PIXELMAP_SLIDER_H,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_SLIDER),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_slider_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {272, 83, 572, 128},                     /* widget size                    */
    &Slider_Screen_Pixelmap_Slider_V_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Pixelmap_Slider_H), /* control block */
    (void *) &Slider_Screen_Pixelmap_Slider_H_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Slider_Screen_Prompt_Various_Sliders_define =
{
    "Prompt_Various_Sliders",
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
    {417, 37, 572, 60},                      /* widget size                    */
    &Slider_Screen_Pixelmap_Slider_H_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SLIDER_SCREEN_CONTROL_BLOCK, Slider_Screen_Prompt_Various_Sliders), /* control block */
    (void *) &Slider_Screen_Prompt_Various_Sliders_properties /* extended properties */
};

GX_STUDIO_ACTION Slider_Screen__idb_next_gx_event_clicked_actions[2] = {
    {GX_ACTION_TYPE_TOGGLE, 0, &Primary_root_window, &Window_Screen, GX_NULL},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Slider_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Slider_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Slider_Screen_event_chain = {gx_studio_Slider_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))slider_screen_event_process};
static UINT gx_studio_Slider_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Slider_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Slider_Screen_define =
{
    "Slider_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_SLIDER_SCREEN,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SLIDER_SCREEN_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Slider_Screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Slider_Screen_Prompt_Various_Sliders_define, /* child widget              */
    0,                                       /* control block                  */
    (void *) &Slider_Screen_properties       /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Text_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES Text_Screen_Prompt_Texts_properties =
{
    GX_STRING_ID_STRING_9,                   /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Text_Screen_Prompt_Transparent_Thin_Border_properties =
{
    GX_STRING_ID_STRING_13,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES Text_Screen_Prompt_Transparent_No_Border_properties =
{
    GX_STRING_ID_STRING_14,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PROMPT_PROPERTIES Text_Screen_Prompt_Transparent_Thick_Border_properties =
{
    GX_STRING_ID_STRING_16,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE                        /* disabled text color            */
};
GX_PIXELMAP_PROMPT_PROPERTIES Text_Screen_Pixelmap_Prompt_properties =
{
    GX_STRING_ID_STRING_17,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_PROMPT_PIXELMAP_BG,       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0                                        /* selected right pixelmap id     */
};
GX_ML_TEXT_VIEW_PROPERTIES Text_Screen_ML_Text_View_properties =
{
    GX_STRING_ID_STRING_18,                  /* string id                      */
    GX_FONT_ID_TEXT_INPUT,                   /* font id                        */
    GX_COLOR_ID_BLACK,                       /* normal text color              */
    GX_COLOR_ID_BLACK,                       /* selected text color            */
    GX_COLOR_ID_BLACK,                       /* disabled text color            */
    5,                                       /* whitespace                     */
    0                                        /* line_space                     */
};
GX_CHAR Text_Screen_ML_Text_Input_buffer[100];
GX_MULTI_LINE_TEXT_INPUT_PROPERTIES Text_Screen_ML_Text_Input_properties =
{
    GX_STRING_ID_STRING_20,                  /* string id                      */
    GX_FONT_ID_TEXT_INPUT,                   /* font id                        */
    GX_COLOR_ID_BLACK,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_BLACK,                       /* disabled text color            */
    GX_COLOR_ID_READONLY_FILL,               /* readonly fill color            */
    GX_COLOR_ID_READONLY_TEXT,               /* readonly text color            */
    5,                                       /* whitespace                     */
    0,                                       /* line_space                     */
    Text_Screen_ML_Text_Input_buffer,        /* buffer                         */
    100                                      /* buffer size                    */
};
GX_CHAR Text_Screen_text_input_buffer[100];
GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES Text_Screen_text_input_properties =
{
    GX_STRING_ID_STRING_76,                  /* string id                      */
    GX_FONT_ID_TEXT_INPUT,                   /* font id                        */
    GX_COLOR_ID_BLACK,                       /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_BLACK,                       /* disabled text color            */
    GX_COLOR_ID_READONLY_FILL,               /* readonly fill color            */
    GX_COLOR_ID_READONLY_TEXT,               /* readonly text color            */
    Text_Screen_text_input_buffer,           /* buffer                         */
    100,                                     /* buffer size                    */
};
GX_NUMERIC_PROMPT_PROPERTIES Text_Screen_numeric_prompt_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    numeric_prompt_format_func,              /* format function                */
    1024                                     /* numeric prompt value           */
};
GX_NUMERIC_PIXELMAP_PROMPT_PROPERTIES Text_Screen_numeric_pixelmap_prompt_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_WHITE,                       /* normal text color              */
    GX_COLOR_ID_WHITE,                       /* selected text color            */
    GX_COLOR_ID_WHITE,                       /* disabled text color            */
    GX_PIXELMAP_ID_BUTTON,                   /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    numeric_pixelmap_prompt_format_func,     /* format function                */
    2048                                     /* numeric prompt value           */
};
GX_RICH_TEXT_FONTS Text_Screen_ML_Text_Rich_View_fonts =
{
    GX_FONT_ID_SYSTEM,                       /* normal font.                   */
    GX_FONT_ID_LARGE_FONT,                   /* bold font.                     */
    GX_FONT_ID_SYSTEM,                       /* italic font.                   */
    GX_FONT_ID_SYSTEM                        /* bold italic font.              */
};

GX_RICH_TEXT_VIEW_PROPERTIES Text_Screen_ML_Text_Rich_View_properties =
{
    GX_STRING_ID_STRING_84,                  /* string id                      */
    GX_COLOR_ID_BLACK,                       /* normal text color              */
    GX_COLOR_ID_BLACK,                       /* selected text color            */
    GX_COLOR_ID_BLACK,                       /* disabled text color            */
    5,                                       /* whitespace                     */
    0,                                       /* line_space                     */
    &Text_Screen_ML_Text_Rich_View_fonts,    /* fonts                          */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_ML_Text_Rich_View_define =
{
    "ML_Text_Rich_View",
    GX_TYPE_RICH_TEXT_VIEW,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_RICH_TEXT_VIEW),               /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_rich_text_view_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {449, 312, 609, 432},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_ML_Text_Rich_View), /* control block */
    (void *) &Text_Screen_ML_Text_Rich_View_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_numeric_pixelmap_prompt_define =
{
    "numeric_pixelmap_prompt",
    GX_TYPE_NUMERIC_PIXELMAP_PROMPT,         /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PIXELMAP_PROMPT),      /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_pixelmap_prompt_create,     /* create function           */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 212, 558, 257},                    /* widget size                    */
    &Text_Screen_ML_Text_Rich_View_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_numeric_pixelmap_prompt), /* control block */
    (void *) &Text_Screen_numeric_pixelmap_prompt_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_numeric_prompt_define =
{
    "numeric_prompt",
    GX_TYPE_NUMERIC_PROMPT,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_NUMERIC_PROMPT),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_numeric_prompt_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {418, 155, 557, 200},                    /* widget size                    */
    &Text_Screen_numeric_pixelmap_prompt_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_numeric_prompt), /* control block */
    (void *) &Text_Screen_numeric_prompt_properties /* extended properties     */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_text_input_define =
{
    "text_input",
    GX_TYPE_SINGLE_LINE_TEXT_INPUT,          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_RECESSED|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_SINGLE_LINE_TEXT_INPUT),       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_text_input_create,             /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {89, 266, 399, 299},                     /* widget size                    */
    &Text_Screen_numeric_prompt_define,      /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_text_input), /* control block */
    (void *) &Text_Screen_text_input_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_ML_Text_Input_define =
{
    "ML_Text_Input",
    GX_TYPE_MULTI_LINE_TEXT_INPUT,           /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MULTI_LINE_TEXT_INPUT),        /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_multi_line_text_input_create,     /* create function             */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {91, 313, 251, 433},                     /* widget size                    */
    &Text_Screen_text_input_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_ML_Text_Input), /* control block */
    (void *) &Text_Screen_ML_Text_Input_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_ML_Text_View_define =
{
    "ML_Text_View",
    GX_TYPE_MULTI_LINE_TEXT_VIEW,            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MULTI_LINE_TEXT_VIEW),         /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_multi_line_text_view_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {270, 313, 430, 433},                    /* widget size                    */
    &Text_Screen_ML_Text_Input_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_ML_Text_View), /* control block */
    (void *) &Text_Screen_ML_Text_View_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_Pixelmap_Prompt_define =
{
    "Pixelmap_Prompt",
    GX_TYPE_PIXELMAP_PROMPT,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PIXELMAP_PROMPT),              /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_pixelmap_prompt_create,        /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {89, 211, 398, 254},                     /* widget size                    */
    &Text_Screen_ML_Text_View_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_Pixelmap_Prompt), /* control block */
    (void *) &Text_Screen_Pixelmap_Prompt_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_Prompt_Transparent_Thick_Border_define =
{
    "Prompt_Transparent_Thick_Border",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_transparent_prompt_thick_border_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {89, 156, 398, 199},                     /* widget size                    */
    &Text_Screen_Pixelmap_Prompt_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_Prompt_Transparent_Thick_Border), /* control block */
    (void *) &Text_Screen_Prompt_Transparent_Thick_Border_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_Prompt_Transparent_No_Border_define =
{
    "Prompt_Transparent_No_Border",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_WIDGET_FILL,                 /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {89, 115, 398, 144},                     /* widget size                    */
    &Text_Screen_Prompt_Transparent_Thick_Border_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_Prompt_Transparent_No_Border), /* control block */
    (void *) &Text_Screen_Prompt_Transparent_No_Border_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_Prompt_Transparent_Thin_Border_define =
{
    "Prompt_Transparent_Thin_Border",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_TEXT_CENTER,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_WHITE,                       /* normal color id                */
    GX_COLOR_ID_WHITE,                       /* selected color id              */
    GX_COLOR_ID_WHITE,                       /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {89, 74, 398, 103},                      /* widget size                    */
    &Text_Screen_Prompt_Transparent_No_Border_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_Prompt_Transparent_Thin_Border), /* control block */
    (void *) &Text_Screen_Prompt_Transparent_Thin_Border_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Text_Screen_Prompt_Texts_define =
{
    "Prompt_Texts",
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
    {506, 77, 566, 100},                     /* widget size                    */
    &Text_Screen_Prompt_Transparent_Thin_Border_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(TEXT_SCREEN_CONTROL_BLOCK, Text_Screen_Prompt_Texts), /* control block */
    (void *) &Text_Screen_Prompt_Texts_properties /* extended properties       */
};

GX_STUDIO_ACTION Text_Screen__idb_next_gx_event_clicked_actions[2] = {
    {GX_ACTION_TYPE_TOGGLE, 0, &Primary_root_window, &Slider_Screen, GX_NULL},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Text_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Text_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Text_Screen_event_chain = {gx_studio_Text_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))text_screen_event_handler};
static UINT gx_studio_Text_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Text_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Text_Screen_define =
{
    "Text_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_TEXT_SCREEN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TILE_WALLPAPER,   /* style flags             */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(TEXT_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Text_Screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Text_Screen_Prompt_Texts_define,        /* child widget                   */
    0,                                       /* control block                  */
    (void *) &Text_Screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Button_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_Button_properties =
{
    GX_STRING_ID_STRING_1,                   /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_BUTTON_properties =
{
    GX_STRING_ID_STRING_10,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_TEXT_BUTTON_properties =
{
    GX_STRING_ID_STRING_2,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_MULTI_LINE_TEXT_BUTTON_properties =
{
    GX_STRING_ID_STRING_40,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PIXELMAP_BUTTON_PROPERTIES Button_Screen_Pixelmap_Button_properties =
{
    GX_PIXELMAP_ID_PIXELMAP_BUTTON_NORMAL,   /* normal pixelmap id             */
    GX_PIXELMAP_ID_PIXELMAP_BUTTON_SELECTED, /* selected pixelmap id           */
    GX_PIXELMAP_ID_PIXELMAP_BUTTON_DISABLED  /* disabled pixelmap id           */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_PIXELMAP_BUTTON_properties =
{
    GX_STRING_ID_STRING_11,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_ICON_PROPERTIES Button_Screen_Icon_properties =
{
    GX_PIXELMAP_ID_ICON_DOWNLOAD,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_ICON_BUTTON_properties =
{
    GX_STRING_ID_STRING_4,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_CHECKBOX_properties =
{
    GX_STRING_ID_STRING_5,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Button_Screen_Prompt_GX_RADIO_BUTTON_properties =
{
    GX_STRING_ID_STRING_6,                   /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_CHECKBOX_PROPERTIES Button_Screen_Checkbox_properties =
{
    GX_STRING_ID_STRING_8,                   /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_TEAL,                        /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* unchecked pixelmap id          */
    0,                                       /* checked pixelmap id            */
    0,                                       /* unchecked disabled pixelmap id */
    0                                        /* checked disabled pixelmap id   */
};
GX_RADIO_BUTTON_PROPERTIES Button_Screen_Radio_Button_Off_properties =
{
    0,                                       /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_BTN_TEXT,                    /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_RADIO_BUTTON_PROPERTIES Button_Screen_Radio_Button_On_properties =
{
    GX_STRING_ID_STRING_24,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_TEAL,                        /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* off pixelmap id                */
    0,                                       /* on pixelmap id                 */
    0,                                       /* off disabled pixelmap id       */
    0                                        /* on disabled pixelmap id        */
};
GX_ICON_BUTTON_PROPERTIES Button_Screen_Icon_Button_properties =
{
    0                                        /* pixelmap id                    */
};
GX_TEXT_BUTTON_PROPERTIES Button_Screen_Text_Button_properties =
{
    GX_STRING_ID_STRING_48,                  /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_BTN_TEXT                     /* disabled text color            */
};
GX_ML_TEXT_BUTTON_PROPERTIES Button_Screen_Multi_Line_Button_properties =
{
    GX_STRING_ID_STRING_3,                   /* string id                      */
    GX_FONT_ID_BUTTON,                       /* font id                        */
    GX_COLOR_ID_BTN_TEXT,                    /* normal text color              */
    GX_COLOR_ID_BTN_TEXT,                    /* selected text color            */
    GX_COLOR_ID_BTN_TEXT                     /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Multi_Line_Button_define =
{
    "Multi_Line_Button",
    GX_TYPE_MULTI_LINE_TEXT_BUTTON,          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_BUTTON_TOGGLE|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MULTI_LINE_TEXT_BUTTON),       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_multi_line_text_button_create,     /* create function            */
    (VOID (*)(GX_WIDGET *)) custom_multi_line_text_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {90, 200, 229, 264},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Multi_Line_Button), /* control block */
    (void *) &Button_Screen_Multi_Line_Button_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Button_define =
{
    "Button",
    GX_TYPE_BUTTON,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_BUTTON),                       /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_button_create,                 /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_button_draw, /* drawing function override   */
    GX_NULL,                                 /* event function override        */
    {91, 96, 230, 139},                      /* widget size                    */
    &Button_Screen_Multi_Line_Button_define, /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Button), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Text_Button_define =
{
    "Text_Button",
    GX_TYPE_TEXT_BUTTON,                     /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TEXT_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_text_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_text_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {90, 149, 229, 192},                     /* widget size                    */
    &Button_Screen_Button_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Text_Button), /* control block */
    (void *) &Button_Screen_Text_Button_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Icon_Button_define =
{
    "Icon_Button",
    GX_TYPE_ICON_BUTTON,                     /* widget type                    */
    ICON_BUTTON_DISC,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THICK|GX_STYLE_ENABLED|GX_STYLE_HALIGN_CENTER|GX_STYLE_VALIGN_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ICON_BUTTON),                  /* control block size             */
    GX_COLOR_ID_BTN_LOWER,                   /* normal color id                */
    GX_COLOR_ID_BTN_UPPER,                   /* selected color id              */
    GX_COLOR_ID_BTN_LOWER,                   /* disabled color id              */
    gx_studio_icon_button_create,            /* create function                */
    (VOID (*)(GX_WIDGET *)) custom_icon_button_draw, /* drawing function override */
    GX_NULL,                                 /* event function override        */
    {90, 325, 138, 368},                     /* widget size                    */
    &Button_Screen_Text_Button_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Icon_Button), /* control block */
    (void *) &Button_Screen_Icon_Button_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Radio_Button_On_define =
{
    "Radio_Button_On",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    {130, 430, 265, 454},                    /* widget size                    */
    &Button_Screen_Icon_Button_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Radio_Button_On), /* control block */
    (void *) &Button_Screen_Radio_Button_On_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Radio_Button_Off_define =
{
    "Radio_Button_Off",
    GX_TYPE_RADIO_BUTTON,                    /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
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
    {90, 430, 110, 454},                     /* widget size                    */
    &Button_Screen_Radio_Button_On_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Radio_Button_Off), /* control block */
    (void *) &Button_Screen_Radio_Button_Off_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Checkbox_define =
{
    "Checkbox",
    GX_TYPE_CHECKBOX,                        /* widget type                    */
    IDB_CHECKBOX,                            /* widget id                      */
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
    {90, 390, 195, 413},                     /* widget size                    */
    &Button_Screen_Radio_Button_Off_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Checkbox), /* control block */
    (void *) &Button_Screen_Checkbox_properties /* extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_RADIO_BUTTON_define =
{
    "Prompt_GX_RADIO_BUTTON",
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
    {290, 430, 446, 453},                    /* widget size                    */
    &Button_Screen_Checkbox_define,          /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_RADIO_BUTTON), /* control block */
    (void *) &Button_Screen_Prompt_GX_RADIO_BUTTON_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_CHECKBOX_define =
{
    "Prompt_GX_CHECKBOX",
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
    {290, 390, 401, 413},                    /* widget size                    */
    &Button_Screen_Prompt_GX_RADIO_BUTTON_define, /* next widget definition    */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_CHECKBOX), /* control block */
    (void *) &Button_Screen_Prompt_GX_CHECKBOX_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_ICON_BUTTON_define =
{
    "Prompt_GX_ICON_BUTTON",
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
    {290, 341, 522, 364},                    /* widget size                    */
    &Button_Screen_Prompt_GX_CHECKBOX_define, /* next widget definition        */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_ICON_BUTTON), /* control block */
    (void *) &Button_Screen_Prompt_GX_ICON_BUTTON_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Icon_define =
{
    "Icon",
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
    {190, 340, 222, 363},                    /* widget size                    */
    &Button_Screen_Prompt_GX_ICON_BUTTON_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Icon), /* control block */
    (void *) &Button_Screen_Icon_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_PIXELMAP_BUTTON_define =
{
    "Prompt_GX_PIXELMAP_BUTTON",
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
    {290, 283, 470, 306},                    /* widget size                    */
    &Button_Screen_Icon_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_PIXELMAP_BUTTON), /* control block */
    (void *) &Button_Screen_Prompt_GX_PIXELMAP_BUTTON_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Pixelmap_Button_define =
{
    "Pixelmap_Button",
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
    {89, 272, 228, 315},                     /* widget size                    */
    &Button_Screen_Prompt_GX_PIXELMAP_BUTTON_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Pixelmap_Button), /* control block */
    (void *) &Button_Screen_Pixelmap_Button_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_MULTI_LINE_TEXT_BUTTON_define =
{
    "Prompt_GX_MULTI_LINE_TEXT_BUTTON",
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
    {289, 219, 533, 236},                    /* widget size                    */
    &Button_Screen_Pixelmap_Button_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_MULTI_LINE_TEXT_BUTTON), /* control block */
    (void *) &Button_Screen_Prompt_GX_MULTI_LINE_TEXT_BUTTON_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_TEXT_BUTTON_define =
{
    "Prompt_GX_TEXT_BUTTON",
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
    {289, 161, 431, 178},                    /* widget size                    */
    &Button_Screen_Prompt_GX_MULTI_LINE_TEXT_BUTTON_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_TEXT_BUTTON), /* control block */
    (void *) &Button_Screen_Prompt_GX_TEXT_BUTTON_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_GX_BUTTON_define =
{
    "Prompt_GX_BUTTON",
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
    {289, 105, 384, 122},                    /* widget size                    */
    &Button_Screen_Prompt_GX_TEXT_BUTTON_define, /* next widget definition     */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_GX_BUTTON), /* control block */
    (void *) &Button_Screen_Prompt_GX_BUTTON_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Button_Screen_Prompt_Button_define =
{
    "Prompt_Button",
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
    {290, 49, 369, 72},                      /* widget size                    */
    &Button_Screen_Prompt_GX_BUTTON_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(BUTTON_SCREEN_CONTROL_BLOCK, Button_Screen_Prompt_Button), /* control block */
    (void *) &Button_Screen_Prompt_Button_properties /* extended properties    */
};

GX_ANIMATION_INFO Button_Screen_animation_1 = {
    (GX_WIDGET *) &Button_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, 0, 0, 1,
    {0, 0}, {-640, 0}, 255, 0, 20
};


GX_ANIMATION_INFO Button_Screen_animation_2 = {
    (GX_WIDGET *) &Text_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {640, 0}, {0, 0}, 0, 255, 20
};


GX_STUDIO_ACTION Button_Screen__idb_next_gx_event_clicked_actions[3] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Button_Screen, &Button_Screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Text_Screen, &Button_Screen_animation_2},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Button_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Button_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Button_Screen_event_chain = {gx_studio_Button_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))gx_window_event_process};
static UINT gx_studio_Button_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Button_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Button_Screen_define =
{
    "Button_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_BUTTON_SCREEN,                        /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(BUTTON_SCREEN_CONTROL_BLOCK),     /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Button_Screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Button_Screen_Prompt_Button_define,     /* child widget                   */
    0,                                       /* control block                  */
    (void *) &Button_Screen_properties       /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Menu_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 638, 479}                         /* widget size                    */
};
GX_PROMPT_PROPERTIES Menu_Screen_title_properties =
{
    GX_STRING_ID_STRING_34,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_1_properties =
{
    GX_STRING_ID_MENU_1,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    GX_PIXELMAP_ID_SCROLL_WHEEL_SELECTED_BG, /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    1,                                       /* list_total_count               */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_1_1_properties =
{
    GX_STRING_ID_STRING_74,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    1,                                       /* list_total_count               */
};
GX_ML_TEXT_VIEW_PROPERTIES Menu_Screen_accordion_menu_1_1_1_properties =
{
    GX_STRING_ID_STRING_43,                  /* string id                      */
    GX_FONT_ID_TEXT_INPUT,                   /* font id                        */
    GX_COLOR_ID_TEXT_INPUT_TEXT,             /* normal text color              */
    GX_COLOR_ID_TEXT_INPUT_TEXT,             /* selected text color            */
    GX_COLOR_ID_TEXT_INPUT_TEXT,             /* disabled text color            */
    0,                                       /* whitespace                     */
    2                                        /* line_space                     */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_1_2_properties =
{
    GX_STRING_ID_STRING_75,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    1,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_accordion_menu_1_2_1_properties =
{
    GX_STRING_ID_STRING_71,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_2_properties =
{
    GX_STRING_ID_STRING_37,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    GX_PIXELMAP_ID_SCROLL_WHEEL_SELECTED_BG, /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    2,                                       /* list_total_count               */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_2_1_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    0,                                       /* list_total_count               */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_2_2_properties =
{
    GX_STRING_ID_STRING_56,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    0,                                       /* list_total_count               */
};
GX_MENU_PROPERTIES Menu_Screen_accordion_menu_3_properties =
{
    GX_STRING_ID_MENU_3,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEXT,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    GX_PIXELMAP_ID_SCROLL_WHEEL_SELECTED_BG, /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    10,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    2,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_accordion_menu_3_1_properties =
{
    GX_STRING_ID_ITEM_1,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Menu_Screen_accordion_menu_3_2_properties =
{
    GX_STRING_ID_ITEM_2,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEXT,                        /* normal text color              */
    GX_COLOR_ID_TEXT,                        /* selected text color            */
    GX_COLOR_ID_TEXT                         /* disabled text color            */
};
GX_ICON_PROPERTIES Menu_Screen_accordion_menu_3_icon_properties =
{
    GX_PIXELMAP_ID_ICON_DOWNLOAD,            /* normal pixelmap id             */
    0                                        /* selected pixelmap id           */
};
GX_TREE_VIEW_PROPERTIES Menu_Screen_tree_view_properties =
{
    GX_PIXELMAP_ID_EXPAND,                   /* expand map id                  */
    GX_PIXELMAP_ID_COLLAPSE,                 /* collapse map id                */
    GX_COLOR_ID_SHADOW,                      /* root line color id             */
    30                                       /* indentation                    */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_properties =
{
    GX_STRING_ID_MENU_1,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    2,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_1_properties =
{
    GX_STRING_ID_ITEM_1,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_1_1_properties =
{
    GX_STRING_ID_ITEM_2,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_ICON_PROPERTIES Menu_Screen_icon_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_2_properties =
{
    GX_STRING_ID_STRING_37,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    2,                                       /* list_total_count               */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_2_1_properties =
{
    GX_STRING_ID_STRING_50,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    2,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_2_1_1_properties =
{
    GX_STRING_ID_ITEM_1,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_2_1_2_properties =
{
    GX_STRING_ID_ITEM_2,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_ICON_PROPERTIES Menu_Screen_icon_1_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_2_2_properties =
{
    GX_STRING_ID_STRING_56,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    1,                                       /* list_total_count               */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_2_2_1_properties =
{
    GX_STRING_ID_STRING_63,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    2,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_2_2_1_1_properties =
{
    GX_STRING_ID_STRING_67,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_2_2_1_2_properties =
{
    GX_STRING_ID_STRING_68,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_ICON_PROPERTIES Menu_Screen_icon_2_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES Menu_Screen_icon_3_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_ICON_PROPERTIES Menu_Screen_icon_4_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_3_properties =
{
    GX_STRING_ID_MENU_3,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    1,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_tree_menu_3_1_properties =
{
    GX_STRING_ID_STRING_65,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_ICON_PROPERTIES Menu_Screen_icon_5_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_SCROLLBAR_APPEARANCE  Menu_Screen_tree_hscroll_properties =
{
    20,                                      /* scroll width                   */
    18,                                      /* thumb width                    */
    20,                                      /* thumb travel min               */
    20,                                      /* thumb travel max               */
    4,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    0,                                       /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb color             */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb border color      */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll button color            */
};
GX_SCROLLBAR_APPEARANCE  Menu_Screen_tree_vscroll_properties =
{
    20,                                      /* scroll width                   */
    18,                                      /* thumb width                    */
    20,                                      /* thumb travel min               */
    20,                                      /* thumb travel max               */
    4,                                       /* thumb border style             */
    0,                                       /* scroll fill pixelmap           */
    0,                                       /* scroll thumb pixelmap          */
    0,                                       /* scroll up pixelmap             */
    0,                                       /* scroll down pixelmap           */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb color             */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll thumb border color      */
    GX_COLOR_ID_SCROLL_BUTTON,               /* scroll button color            */
};
GX_MENU_PROPERTIES Menu_Screen_tree_menu_4_properties =
{
    GX_STRING_ID_STRING_41,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL,                        /* disabled text color            */
    0,                                       /* left pixelmap id               */
    0,                                       /* fill pixelmap id               */
    0,                                       /* right pixelmap id              */
    0,                                       /* selected left pixelmap id      */
    0,                                       /* selected fill pixelmap id      */
    0,                                       /* selected right pixelmap id     */
    25,                                      /* text display x offset          */
    0,                                       /* text display y offset          */
    1,                                       /* list_total_count               */
};
GX_PROMPT_PROPERTIES Menu_Screen_prompt_10_properties =
{
    GX_STRING_ID_ITEM_1,                     /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_ICON_PROPERTIES Menu_Screen_icon_6_properties =
{
    GX_PIXELMAP_ID_SLIDER_THIN_NUB_HORIZONTAL, /* normal pixelmap id           */
    0                                        /* selected pixelmap id           */
};
GX_PROMPT_PROPERTIES Menu_Screen_prompt_properties =
{
    GX_STRING_ID_STRING_72,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};
GX_PROMPT_PROPERTIES Menu_Screen_prompt_1_properties =
{
    GX_STRING_ID_STRING_73,                  /* string id                      */
    GX_FONT_ID_PROMPT,                       /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_1_1_1_define =
{
    "accordion_menu_1_1_1",
    GX_TYPE_MULTI_LINE_TEXT_VIEW,            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MULTI_LINE_TEXT_VIEW),         /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_multi_line_text_view_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 237, 294, 297},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_1_1_1), /* control block */
    (void *) &Menu_Screen_accordion_menu_1_1_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_1_2_1_define =
{
    "accordion_menu_1_2_1",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 331, 294, 391},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_1_2_1), /* control block */
    (void *) &Menu_Screen_accordion_menu_1_2_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_1_2_define =
{
    "accordion_menu_1_2",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_DARK_TEAL,                   /* normal color id                */
    GX_COLOR_ID_NEXT_BUTTON_UPPER,           /* selected color id              */
    GX_COLOR_ID_DARK_TEAL,                   /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 298, 294, 330},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Menu_Screen_accordion_menu_1_2_1_define, /* child widget definition       */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_1_2), /* control block */
    (void *) &Menu_Screen_accordion_menu_1_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_1_1_define =
{
    "accordion_menu_1_1",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_DARK_TEAL,                   /* normal color id                */
    GX_COLOR_ID_NEXT_BUTTON_UPPER,           /* selected color id              */
    GX_COLOR_ID_DARK_TEAL,                   /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 204, 294, 236},                     /* widget size                    */
    &Menu_Screen_accordion_menu_1_2_define,  /* next widget definition         */
    &Menu_Screen_accordion_menu_1_1_1_define, /* child widget definition       */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_1_1), /* control block */
    (void *) &Menu_Screen_accordion_menu_1_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_embedded_accordion_define =
{
    "embedded_accordion",
    GX_TYPE_ACCORDION_MENU,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE,                    /* style flags                    */
    0,                                       /* status flags                   */
    sizeof(GX_ACCORDION_MENU),               /* control block size             */
    GX_COLOR_ID_NEXT_BUTTON_UPPER,           /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_NEXT_BUTTON_UPPER,           /* disabled color id              */
    gx_studio_accordion_menu_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 204, 294, 330},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Menu_Screen_accordion_menu_1_1_define,  /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_embedded_accordion), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_2_2_define =
{
    "accordion_menu_2_2",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {38, 278, 291, 310},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_2_2), /* control block */
    (void *) &Menu_Screen_accordion_menu_2_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_2_1_define =
{
    "accordion_menu_2_1",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {38, 245, 290, 277},                     /* widget size                    */
    &Menu_Screen_accordion_menu_2_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_2_1), /* control block */
    (void *) &Menu_Screen_accordion_menu_2_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_3_icon_define =
{
    "accordion_menu_3_icon",
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {253, 320, 285, 343},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_3_icon), /* control block */
    (void *) &Menu_Screen_accordion_menu_3_icon_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_3_2_define =
{
    "accordion_menu_3_2",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 393, 295, 425},                     /* widget size                    */
    &Menu_Screen_accordion_menu_3_icon_define, /* next widget definition       */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_3_2), /* control block */
    (void *) &Menu_Screen_accordion_menu_3_2_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_3_1_define =
{
    "accordion_menu_3_1",
    GX_TYPE_PROMPT,                          /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_PROMPT),                       /* control block size             */
    GX_COLOR_ID_DARK_GRAY,                   /* normal color id                */
    GX_COLOR_ID_DARK_GRAY,                   /* selected color id              */
    GX_COLOR_ID_DARK_GRAY,                   /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {42, 360, 294, 392},                     /* widget size                    */
    &Menu_Screen_accordion_menu_3_2_define,  /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_3_1), /* control block */
    (void *) &Menu_Screen_accordion_menu_3_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_3_define =
{
    "accordion_menu_3",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {38, 311, 290, 355},                     /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Menu_Screen_accordion_menu_3_1_define,  /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_3), /* control block */
    (void *) &Menu_Screen_accordion_menu_3_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_2_define =
{
    "accordion_menu_2",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {38, 200, 290, 244},                     /* widget size                    */
    &Menu_Screen_accordion_menu_3_define,    /* next widget definition         */
    &Menu_Screen_accordion_menu_2_1_define,  /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_2), /* control block */
    (void *) &Menu_Screen_accordion_menu_2_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_menu_1_define =
{
    "accordion_menu_1",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {38, 155, 290, 199},                     /* widget size                    */
    &Menu_Screen_accordion_menu_2_define,    /* next widget definition         */
    &Menu_Screen_embedded_accordion_define,  /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion_menu_1), /* control block */
    (void *) &Menu_Screen_accordion_menu_1_properties /* extended properties   */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {385, 157, 406, 180},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon), /* control block    */
    (void *) &Menu_Screen_icon_properties    /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_1_1_define =
{
    "tree_menu_1_1",
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
    {413, 214, 492, 243},                    /* widget size                    */
    &Menu_Screen_icon_define,                /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_1_1), /* control block */
    (void *) &Menu_Screen_tree_menu_1_1_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_1_define =
{
    "tree_menu_1",
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
    {413, 184, 492, 213},                    /* widget size                    */
    &Menu_Screen_tree_menu_1_1_define,       /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_1), /* control block */
    (void *) &Menu_Screen_tree_menu_1_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_1_define =
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
    {415, 277, 436, 300},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon_1), /* control block  */
    (void *) &Menu_Screen_icon_1_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_1_2_define =
{
    "tree_menu_2_1_2",
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
    {443, 334, 522, 363},                    /* widget size                    */
    &Menu_Screen_icon_1_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_1_2), /* control block */
    (void *) &Menu_Screen_tree_menu_2_1_2_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_1_1_define =
{
    "tree_menu_2_1_1",
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
    {443, 304, 522, 333},                    /* widget size                    */
    &Menu_Screen_tree_menu_2_1_2_define,     /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_1_1), /* control block */
    (void *) &Menu_Screen_tree_menu_2_1_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_2_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {446, 397, 467, 420},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon_2), /* control block  */
    (void *) &Menu_Screen_icon_2_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_2_1_2_define =
{
    "tree_menu_2_2_1_2",
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
    {473, 454, 725, 483},                    /* widget size                    */
    &Menu_Screen_icon_2_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_2_1_2), /* control block */
    (void *) &Menu_Screen_tree_menu_2_2_1_2_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_2_1_1_define =
{
    "tree_menu_2_2_1_1",
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
    {473, 424, 706, 453},                    /* widget size                    */
    &Menu_Screen_tree_menu_2_2_1_2_define,   /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_2_1_1), /* control block */
    (void *) &Menu_Screen_tree_menu_2_2_1_1_properties /* extended properties  */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_3_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {416, 367, 437, 390},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon_3), /* control block  */
    (void *) &Menu_Screen_icon_3_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_2_1_define =
{
    "tree_menu_2_2_1",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {443, 394, 562, 423},                    /* widget size                    */
    &Menu_Screen_icon_3_define,              /* next widget definition         */
    &Menu_Screen_tree_menu_2_2_1_1_define,   /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_2_1), /* control block */
    (void *) &Menu_Screen_tree_menu_2_2_1_properties /* extended properties    */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_4_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {385, 247, 406, 270},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon_4), /* control block  */
    (void *) &Menu_Screen_icon_4_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_2_define =
{
    "tree_menu_2_2",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {413, 364, 512, 393},                    /* widget size                    */
    &Menu_Screen_icon_4_define,              /* next widget definition         */
    &Menu_Screen_tree_menu_2_2_1_define,     /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_2), /* control block */
    (void *) &Menu_Screen_tree_menu_2_2_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_1_define =
{
    "tree_menu_2_1",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {413, 274, 512, 303},                    /* widget size                    */
    &Menu_Screen_tree_menu_2_2_define,       /* next widget definition         */
    &Menu_Screen_tree_menu_2_1_1_define,     /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2_1), /* control block */
    (void *) &Menu_Screen_tree_menu_2_1_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_5_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_icon_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {385, 487, 406, 510},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon_5), /* control block  */
    (void *) &Menu_Screen_icon_5_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_3_1_define =
{
    "tree_menu_3_1",
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
    {413, 514, 645, 543},                    /* widget size                    */
    &Menu_Screen_icon_5_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_3_1), /* control block */
    (void *) &Menu_Screen_tree_menu_3_1_properties /* extended properties      */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_icon_6_define =
{
    "icon_6",
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
    {386, 547, 407, 570},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_icon_6), /* control block  */
    (void *) &Menu_Screen_icon_6_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_prompt_10_define =
{
    "prompt_10",
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
    {413, 574, 492, 603},                    /* widget size                    */
    &Menu_Screen_icon_6_define,              /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_prompt_10), /* control block */
    (void *) &Menu_Screen_prompt_10_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_4_define =
{
    "tree_menu_4",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {383, 544, 482, 573},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    &Menu_Screen_prompt_10_define,           /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_4), /* control block */
    (void *) &Menu_Screen_tree_menu_4_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_vscroll_define =
{
    "tree_vscroll",
    GX_TYPE_VERTICAL_SCROLL,                 /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_SCROLLBAR_RELATIVE_THUMB|GX_SCROLLBAR_END_BUTTONS|GX_SCROLLBAR_VERTICAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_SCROLL_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_SCROLL_FILL,                 /* disabled color id              */
    gx_studio_vertical_scrollbar_create,     /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {557, 154, 576, 383},                    /* widget size                    */
    &Menu_Screen_tree_menu_4_define,         /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_vscroll), /* control block */
    (void *) &Menu_Screen_tree_vscroll_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_hscroll_define =
{
    "tree_hscroll",
    GX_TYPE_HORIZONTAL_SCROLL,               /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_ENABLED|GX_SCROLLBAR_RELATIVE_THUMB|GX_SCROLLBAR_END_BUTTONS|GX_SCROLLBAR_HORIZONTAL,   /* style flags */
    0,                                       /* status flags                   */
    sizeof(GX_SCROLLBAR),                    /* control block size             */
    GX_COLOR_ID_SCROLL_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_SCROLL_FILL,                 /* disabled color id              */
    gx_studio_horizontal_scrollbar_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {353, 384, 556, 403},                    /* widget size                    */
    &Menu_Screen_tree_vscroll_define,        /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_hscroll), /* control block */
    (void *) &Menu_Screen_tree_hscroll_properties /* extended properties       */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_3_define =
{
    "tree_menu_3",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {383, 484, 482, 513},                    /* widget size                    */
    &Menu_Screen_tree_hscroll_define,        /* next widget definition         */
    &Menu_Screen_tree_menu_3_1_define,       /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_3), /* control block */
    (void *) &Menu_Screen_tree_menu_3_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_2_define =
{
    "tree_menu_2",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {383, 244, 482, 273},                    /* widget size                    */
    &Menu_Screen_tree_menu_3_define,         /* next widget definition         */
    &Menu_Screen_tree_menu_2_1_define,       /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu_2), /* control block */
    (void *) &Menu_Screen_tree_menu_2_properties /* extended properties        */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_menu_define =
{
    "tree_menu",
    GX_TYPE_MENU,                            /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_LEFT,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_MENU),                         /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_menu_create,                   /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {383, 154, 488, 183},                    /* widget size                    */
    &Menu_Screen_tree_menu_2_define,         /* next widget definition         */
    &Menu_Screen_tree_menu_1_define,         /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_menu), /* control block */
    (void *) &Menu_Screen_tree_menu_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_prompt_1_define =
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
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_prompt_create,                 /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {420, 114, 499, 131},                    /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_prompt_1), /* control block */
    (void *) &Menu_Screen_prompt_1_properties /* extended properties           */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_prompt_define =
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
    {95, 114, 216, 131},                     /* widget size                    */
    &Menu_Screen_prompt_1_define,            /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_prompt), /* control block  */
    (void *) &Menu_Screen_prompt_properties  /* extended properties            */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_tree_view_define =
{
    "tree_view",
    GX_TYPE_TREE_VIEW,                       /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED|GX_STYLE_TREE_VIEW_SHOW_ROOT_LINES,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_TREE_VIEW),                    /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_SHADOW,                      /* disabled color id              */
    gx_studio_tree_view_create,              /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {352, 153, 577, 404},                    /* widget size                    */
    &Menu_Screen_prompt_define,              /* next widget definition         */
    &Menu_Screen_tree_menu_define,           /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_tree_view), /* control block */
    (void *) &Menu_Screen_tree_view_properties /* extended properties          */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_accordion_define =
{
    "accordion",
    GX_TYPE_ACCORDION_MENU,                  /* widget type                    */
    GX_ID_NONE,                              /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN|GX_STYLE_ENABLED,   /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_ACCORDION_MENU),               /* control block size             */
    GX_COLOR_ID_WIDGET_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WIDGET_FILL,                 /* disabled color id              */
    gx_studio_accordion_menu_create,         /* create function                */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {37, 154, 291, 356},                     /* widget size                    */
    &Menu_Screen_tree_view_define,           /* next widget definition         */
    &Menu_Screen_accordion_menu_1_define,    /* child widget definition        */
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_accordion), /* control block */
    (void *) GX_NULL                         /* no extended properties         */
};

GX_CONST GX_STUDIO_WIDGET Menu_Screen_title_define =
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
    {278, 55, 342, 75},                      /* widget size                    */
    &Menu_Screen_accordion_define,           /* next widget definition         */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(MENU_SCREEN_CONTROL_BLOCK, Menu_Screen_title), /* control block   */
    (void *) &Menu_Screen_title_properties   /* extended properties            */
};

GX_ANIMATION_INFO Menu_Screen_animation_1 = {
    (GX_WIDGET *) &Menu_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, 0, 0, 1,
    {0, 0}, {0, -480}, 255, 255, 20
};


GX_ANIMATION_INFO Menu_Screen_animation_2 = {
    (GX_WIDGET *) &Button_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {0, 480}, {0, 0}, 255, 255, 20
};


GX_STUDIO_ACTION Menu_Screen__idb_next_gx_event_clicked_actions[3] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Menu_Screen, &Menu_Screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Button_Screen, &Menu_Screen_animation_2},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Menu_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Menu_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Menu_Screen_event_chain = {gx_studio_Menu_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))gx_window_event_process};
static UINT gx_studio_Menu_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Menu_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Menu_Screen_define =
{
    "Menu_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_MENU_SCREEN,                          /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(MENU_SCREEN_CONTROL_BLOCK),       /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Menu_Screen_event_process, /* event function override */
    {0, 0, 638, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Menu_Screen_title_define,               /* child widget                   */
    0,                                       /* control block                  */
    (void *) &Menu_Screen_properties         /* extended properties            */
};
GX_TEMPLATE_PROPERTIES Scroll_Wheel_Screen_properties =
{
    &Base_Screen_define,                     /* base info                      */
    gx_studio_window_create,                 /* base create function           */
    {0, 0, 639, 479}                         /* widget size                    */
};
GX_NUMERIC_SCROLL_WHEEL_PROPERTIES Scroll_Wheel_Screen_scroll_wheel_day_properties =
{
    30,                                      /* total rows                     */
    0,                                       /* selected row                   */
    45,                                      /* row height                     */
    200,                                     /* start alpha                    */
    0,                                       /* end alpha                      */
    GX_FONT_ID_SCROLL_WHEEL_NORMAL,          /* normal font id                 */
    GX_FONT_ID_SCROLL_WHEEL_SELECTED,        /* selected font id               */
    GX_COLOR_ID_SHADOW,                      /* normal text color id           */
    GX_COLOR_ID_WHITE,                       /* selected text color id         */
    GX_COLOR_ID_SHADOW,                      /* disabled text color id         */
    0,                                       /* wallpaper id                   */
    GX_PIXELMAP_ID_SCROLL_WHEEL_SELECTED_BG, /* selected background            */
    day_wheel_value_format,                  /* format callback                */
    1,                                       /* start val                      */
    30,                                      /* end val                        */
};
GX_NUMERIC_SCROLL_WHEEL_PROPERTIES Scroll_Wheel_Screen_scroll_wheel_year_properties =
{
    0,                                       /* total rows                     */
    57,                                      /* selected row                   */
    45,                                      /* row height                     */
    200,                                     /* start alpha                    */
    0,                                       /* end alpha                      */
    GX_FONT_ID_SCROLL_WHEEL_NORMAL,          /* normal font id                 */
    GX_FONT_ID_SCROLL_WHEEL_SELECTED,        /* selected font id               */
    GX_COLOR_ID_SHADOW,                      /* normal text color id           */
    GX_COLOR_ID_WHITE,                       /* selected text color id         */
    GX_COLOR_ID_SHADOW,                      /* disabled text color id         */
    0,                                       /* wallpaper id                   */
    GX_PIXELMAP_ID_SCROLL_WHEEL_SELECTED_BG, /* selected background            */
    GX_NULL,                                 /* format callback                */
    1960,                                    /* start val                      */
    2020,                                    /* end val                        */
};
GX_CONST GX_RESOURCE_ID Scroll_Wheel_Screen_scroll_wheel_month_string_id_list[]={
    GX_STRING_ID_STRING_19,
    GX_STRING_ID_STRING_21,
    GX_STRING_ID_STRING_22,
    GX_STRING_ID_STRING_25,
    GX_STRING_ID_STRING_26,
    GX_STRING_ID_STRING_27,
    GX_STRING_ID_STRING_28,
    GX_STRING_ID_STRING_29,
    GX_STRING_ID_STRING_30,
    GX_STRING_ID_STRING_31,
    GX_STRING_ID_STRING_32,
    GX_STRING_ID_STRING_33
};
GX_STRING_SCROLL_WHEEL_PROPERTIES Scroll_Wheel_Screen_scroll_wheel_month_properties =
{
    12,                                      /* total rows                     */
    5,                                       /* selected row                   */
    45,                                      /* row height                     */
    200,                                     /* start alpha                    */
    0,                                       /* end alpha                      */
    GX_FONT_ID_SCROLL_WHEEL_NORMAL,          /* normal font id                 */
    GX_FONT_ID_SCROLL_WHEEL_SELECTED,        /* selected font id               */
    GX_COLOR_ID_SHADOW,                      /* normal text color id           */
    GX_COLOR_ID_WHITE,                       /* selected text color id         */
    GX_COLOR_ID_SHADOW,                      /* disabled text color id         */
    0,                                       /* wallpaper id                   */
    GX_PIXELMAP_ID_SCROLL_WHEEL_SELECTED_BG, /* selected background            */
    Scroll_Wheel_Screen_scroll_wheel_month_string_id_list, /* string list      */
    GX_NULL                                  /* callback                       */

};
GX_PROMPT_PROPERTIES Scroll_Wheel_Screen_Prompt_Circular_Gauges_1_properties =
{
    GX_STRING_ID_STRING_15,                  /* string id                      */
    GX_FONT_ID_TITLE,                        /* font id                        */
    GX_COLOR_ID_TEAL,                        /* normal text color              */
    GX_COLOR_ID_SELECTED_TEXT,               /* selected text color            */
    GX_COLOR_ID_TEAL                         /* disabled text color            */
};

GX_CONST GX_STUDIO_WIDGET Scroll_Wheel_Screen_Prompt_Circular_Gauges_1_define =
{
    "Prompt_Circular_Gauges_1",
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
    {247, 57, 381, 77},                      /* widget size                    */
    GX_NULL,                                 /* no next widget                 */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCROLL_WHEEL_SCREEN_CONTROL_BLOCK, Scroll_Wheel_Screen_Prompt_Circular_Gauges_1), /* control block */
    (void *) &Scroll_Wheel_Screen_Prompt_Circular_Gauges_1_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Scroll_Wheel_Screen_scroll_wheel_month_define =
{
    "scroll_wheel_month",
    GX_TYPE_STRING_SCROLL_WHEEL,             /* widget type                    */
    ID_SCROLL_WHEEL_MONTH,                   /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_STRING_SCROLL_WHEEL),          /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_string_scroll_wheel_create,     /* create function               */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {210, 117, 378, 352},                    /* widget size                    */
    &Scroll_Wheel_Screen_Prompt_Circular_Gauges_1_define, /* next widget definition */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCROLL_WHEEL_SCREEN_CONTROL_BLOCK, Scroll_Wheel_Screen_scroll_wheel_month), /* control block */
    (void *) &Scroll_Wheel_Screen_scroll_wheel_month_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Scroll_Wheel_Screen_scroll_wheel_year_define =
{
    "scroll_wheel_year",
    GX_TYPE_NUMERIC_SCROLL_WHEEL,            /* widget type                    */
    ID_SCROLL_WHEEL_YEAR,                    /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_ENABLED|GX_STYLE_WRAP|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_SCROLL_WHEEL),         /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_numeric_scroll_wheel_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {379, 117, 538, 352},                    /* widget size                    */
    &Scroll_Wheel_Screen_scroll_wheel_month_define, /* next widget definition  */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCROLL_WHEEL_SCREEN_CONTROL_BLOCK, Scroll_Wheel_Screen_scroll_wheel_year), /* control block */
    (void *) &Scroll_Wheel_Screen_scroll_wheel_year_properties /* extended properties */
};

GX_CONST GX_STUDIO_WIDGET Scroll_Wheel_Screen_scroll_wheel_day_define =
{
    "scroll_wheel_day",
    GX_TYPE_NUMERIC_SCROLL_WHEEL,            /* widget type                    */
    ID_SCROLL_WHEEL_DAY,                     /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_NONE|GX_STYLE_TRANSPARENT|GX_STYLE_DRAW_SELECTED|GX_STYLE_ENABLED|GX_STYLE_WRAP|GX_STYLE_TEXT_CENTER,   /* style flags */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(GX_NUMERIC_SCROLL_WHEEL),         /* control block size             */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_WINDOW_FILL,                 /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_numeric_scroll_wheel_create,     /* create function              */
    GX_NULL,                                 /* drawing function override      */
    GX_NULL,                                 /* event function override        */
    {100, 117, 209, 352},                    /* widget size                    */
    &Scroll_Wheel_Screen_scroll_wheel_year_define, /* next widget definition   */
    GX_NULL,                                 /* no child widgets               */ 
    offsetof(SCROLL_WHEEL_SCREEN_CONTROL_BLOCK, Scroll_Wheel_Screen_scroll_wheel_day), /* control block */
    (void *) &Scroll_Wheel_Screen_scroll_wheel_day_properties /* extended properties */
};

GX_ANIMATION_INFO Scroll_Wheel_Screen_animation_1 = {
    (GX_WIDGET *) &Scroll_Wheel_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE|GX_ANIMATION_DETACH, 0, 0, 1,
    {0, 0}, {-640, 0}, 255, 255, 20
};


GX_ANIMATION_INFO Scroll_Wheel_Screen_animation_2 = {
    (GX_WIDGET *) &Menu_Screen,
    (GX_WIDGET *) &Primary_root_window,
    GX_NULL,
    GX_ANIMATION_TRANSLATE, 0, 0, 1,
    {640, 0}, {0, 0}, 255, 255, 20
};


GX_STUDIO_ACTION Scroll_Wheel_Screen__idb_next_gx_event_clicked_actions[3] = {
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Scroll_Wheel_Screen, &Scroll_Wheel_Screen_animation_1},
    {GX_ACTION_TYPE_ANIMATION, 0, &Primary_root_window, &Menu_Screen, &Scroll_Wheel_Screen_animation_2},
    {0, 0, GX_NULL, GX_NULL, GX_NULL}
};

static GX_STUDIO_EVENT_ENTRY gx_studio_Scroll_Wheel_Screen_event_table[] = {
    {GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED), 0, Scroll_Wheel_Screen__idb_next_gx_event_clicked_actions},
    {0, 0, GX_NULL}
};

GX_STUDIO_EVENT_PROCESS Scroll_Wheel_Screen_event_chain = {gx_studio_Scroll_Wheel_Screen_event_table, (UINT (*)(GX_WIDGET *, GX_EVENT *))scroll_wheel_screen_event_process};
static UINT gx_studio_Scroll_Wheel_Screen_event_process(GX_WIDGET *target, GX_EVENT *event_ptr)
{
    return (gx_studio_auto_event_handler(target, event_ptr, &Scroll_Wheel_Screen_event_chain));
}


GX_CONST GX_STUDIO_WIDGET Scroll_Wheel_Screen_define =
{
    "Scroll_Wheel_Screen",
    GX_TYPE_TEMPLATE,                        /* widget type                    */
    ID_SCROLL_WHEEL_SCREEN,                  /* widget id                      */
    #if defined(GX_WIDGET_USER_DATA)
    0,                                       /* user data                      */
    #endif
    GX_STYLE_BORDER_THIN,                    /* style flags                    */
    GX_STATUS_ACCEPTS_FOCUS,                 /* status flags                   */
    sizeof(SCROLL_WHEEL_SCREEN_CONTROL_BLOCK), /* control block size           */
    GX_COLOR_ID_WINDOW_FILL,                 /* normal color id                */
    GX_COLOR_ID_SELECTED_FILL,               /* selected color id              */
    GX_COLOR_ID_WINDOW_FILL,                 /* disabled color id              */
    gx_studio_template_create,               /* create function                */
    GX_NULL,                                 /* drawing function override      */
    (UINT (*)(GX_WIDGET *, GX_EVENT *)) gx_studio_Scroll_Wheel_Screen_event_process, /* event function override */
    {0, 0, 639, 479},                        /* widget size                    */
    GX_NULL,                                 /* next widget                    */
    &Scroll_Wheel_Screen_scroll_wheel_day_define, /* child widget              */
    0,                                       /* control block                  */
    (void *) &Scroll_Wheel_Screen_properties /* extended properties            */
};
GX_CONST GX_STUDIO_WIDGET_ENTRY guix_widget_types_widget_table[] =
{
    { &Menu_Screen_define, (GX_WIDGET *) &Menu_Screen },
    { &Scroll_Wheel_Screen_define, (GX_WIDGET *) &Scroll_Wheel_Screen },
    { &Gauge_Screen_define, (GX_WIDGET *) &Gauge_Screen },
    { &Window_Screen_define, (GX_WIDGET *) &Window_Screen },
    { &Slider_Screen_define, (GX_WIDGET *) &Slider_Screen },
    { &Text_Screen_define, (GX_WIDGET *) &Text_Screen },
    { &Button_Screen_define, (GX_WIDGET *) &Button_Screen },
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
    GX_CONST GX_STUDIO_WIDGET_ENTRY *entry = guix_widget_types_widget_table;
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

    GX_STUDIO_DISPLAY_INFO *display_info = &guix_widget_types_display_table[display];


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