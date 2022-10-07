#ifndef _DEMO_GUIX_WASHING_MACHINE_SPECIFICATIONS_H_
#define _DEMO_GUIX_WASHING_MACHINE_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define ID_BTN_QUICK 1
#define ID_BTN_DEEP 2
#define ID_BTN_QUICK_SMALL 3
#define ID_BTN_DEEP_SMALL 4
#define ID_BTN_ECO 5
#define ID_BTN_ECO_SMALL 6
#define ID_WATER_LEVEL_SLIDER 7
#define ID_BTN_MAX 8
#define ID_BTN_HIGH 9
#define ID_BTN_MEDIUM 10
#define ID_BTN_LOW 11
#define ID_BTN_MIN 12
#define ID_TEMPERATURE_SLIDER 13
#define ID_WASH_CYCLE_SLIDER 14
#define ID_BTN_WASH_CYCLE 15
#define ID_BTN_TEMPERATURE 16
#define ID_BTN_WATER_LEVEL 17
#define ID_BTN_PLAY 18


/* Define animation ids                                                        */

#define GX_NEXT_ANIMATION_ID 1


/* Define user event ids                                                       */

#define GX_NEXT_USER_EVENT_ID GX_FIRST_USER_EVENT


/* Declare properties structures for each utilized widget type                 */

typedef struct GX_STUDIO_WIDGET_STRUCT
{
   GX_CHAR *widget_name;
   USHORT  widget_type;
   USHORT  widget_id;
   #if defined(GX_WIDGET_USER_DATA)
   INT   user_data;
   #endif
   ULONG style;
   ULONG status;
   ULONG control_block_size;
   GX_RESOURCE_ID normal_fill_color_id;
   GX_RESOURCE_ID selected_fill_color_id;
   GX_RESOURCE_ID disabled_fill_color_id;
   UINT (*create_function) (GX_CONST struct GX_STUDIO_WIDGET_STRUCT *, GX_WIDGET *, GX_WIDGET *);
   void (*draw_function) (GX_WIDGET *);
   UINT (*event_function) (GX_WIDGET *, GX_EVENT *);
   GX_RECTANGLE size;
   GX_CONST struct GX_STUDIO_WIDGET_STRUCT *next_widget;
   GX_CONST struct GX_STUDIO_WIDGET_STRUCT *child_widget;
   ULONG control_block_offset;
   GX_CONST void *properties;
} GX_STUDIO_WIDGET;

typedef struct
{
    GX_CONST GX_STUDIO_WIDGET *widget_information;
    GX_WIDGET        *widget;
} GX_STUDIO_WIDGET_ENTRY;

typedef struct
{
    GX_RESOURCE_ID string_id; 
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
} GX_TEXT_BUTTON_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
    GX_RESOURCE_ID disabled_pixelmap_id;
} GX_PIXELMAP_BUTTON_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
} GX_ICON_PROPERTIES;

typedef struct
{
    GX_VALUE       xcenter;
    GX_VALUE       ycenter;
    USHORT         radius;
    USHORT         track_width;
    GX_VALUE       needle_offset;
    GX_VALUE       current_angle;
    GX_VALUE       min_angle;
    GX_VALUE       max_angle;
    GX_RESOURCE_ID background_pixelmap;
    GX_RESOURCE_ID needle_pixelmap;
    USHORT         animation_total_steps;
    USHORT         animation_delay;
    USHORT         animation_style;
    VOID         (*animation_update_callback)(struct GX_RADIAL_SLIDER_STRUCT *slider);
} GX_RADIAL_SLIDER_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
} GX_PROMPT_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    VOID (*format_func)(GX_NUMERIC_PROMPT *, INT);
    INT            numeric_prompt_value;
} GX_NUMERIC_PROMPT_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID wallpaper_id;
} GX_WINDOW_PROPERTIES;

typedef struct
{
   GX_CONST GX_STUDIO_WIDGET *base_info;
   UINT (*base_create_function) (GX_CONST struct GX_STUDIO_WIDGET_STRUCT *, GX_WIDGET *, GX_WIDGET *);
   GX_RECTANGLE size;
} GX_TEMPLATE_PROPERTIES;


/* Declare top-level control blocks                                            */

typedef struct TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK_STRUCT
{
    GX_PIXELMAP_BUTTON_MEMBERS_DECLARE
    GX_PROMPT template_mode_btn_small_mode_label;
    GX_PROMPT template_mode_btn_small_mode_label_4;
    GX_NUMERIC_PROMPT template_mode_btn_small_total_time;
    GX_PROMPT template_mode_btn_small_mode_label_5;
} TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK;

typedef struct TEMPLATE_MODE_BTN_CONTROL_BLOCK_STRUCT
{
    GX_PIXELMAP_BUTTON_MEMBERS_DECLARE
    GX_PROMPT template_mode_btn_mode_label;
    GX_PROMPT template_mode_btn_wash_label;
    GX_PROMPT template_mode_btn_min_label;
    GX_NUMERIC_PROMPT template_mode_btn_total_time;
    GX_RADIAL_PROGRESS_BAR template_mode_btn_mode_progress_bar;
    GX_NUMERIC_PROMPT template_mode_btn_minute;
    GX_NUMERIC_PROMPT template_mode_btn_second;
    GX_PROMPT template_mode_btn_prompt_3;
    GX_PROMPT template_mode_btn_prompt_2;
} TEMPLATE_MODE_BTN_CONTROL_BLOCK;

typedef struct MODE_SELECT_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    TEMPLATE_MODE_BTN_CONTROL_BLOCK mode_select_window_btn_quick;
    TEMPLATE_MODE_BTN_CONTROL_BLOCK mode_select_window_btn_deep;
    TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK mode_select_window_btn_quick_small;
    TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK mode_select_window_btn_deep_small;
    TEMPLATE_MODE_BTN_CONTROL_BLOCK mode_select_window_btn_eco;
    TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK mode_select_window_btn_eco_small;
} MODE_SELECT_WINDOW_CONTROL_BLOCK;

typedef struct WATER_LEVEL_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_RADIAL_SLIDER water_level_window_water_level_slider;
    GX_NUMERIC_PROMPT water_level_window_water_level_value;
    GX_PROMPT water_level_window_percentage_flag;
    GX_TEXT_BUTTON water_level_window_btn_max;
    GX_TEXT_BUTTON water_level_window_btn_high;
    GX_TEXT_BUTTON water_level_window_btn_medium;
    GX_TEXT_BUTTON water_level_window_btn_low;
    GX_TEXT_BUTTON water_level_window_btn_min;
} WATER_LEVEL_WINDOW_CONTROL_BLOCK;

typedef struct TEMPERATURE_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_RADIAL_SLIDER temperature_window_temperature_slider;
    GX_NUMERIC_PROMPT temperature_window_temperature_value;
    GX_PROMPT temperature_window_temperature_flag;
} TEMPERATURE_WINDOW_CONTROL_BLOCK;

typedef struct WASH_CYCLE_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_RADIAL_SLIDER wash_cycle_window_wash_cycle_slider;
    GX_NUMERIC_PROMPT wash_cycle_window_minute;
    GX_NUMERIC_PROMPT wash_cycle_window_second;
    GX_ICON wash_cycle_window_icon_1;
    GX_ICON wash_cycle_window_icon_2;
} WASH_CYCLE_WINDOW_CONTROL_BLOCK;

typedef struct MAIN_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_ICON main_screen_logo;
    GX_BUTTON main_screen_btn_wash_cycle;
    GX_PROMPT main_screen_wash_cycle_mode;
    GX_PROMPT main_screen_wash_cycle_lable;
    GX_BUTTON main_screen_btn_temperature;
    GX_PROMPT main_screen_prompt_2;
    GX_PROMPT main_screen_temperature_lable;
    GX_NUMERIC_PROMPT main_screen_temperature_value;
    GX_BUTTON main_screen_btn_water_level;
    GX_PROMPT main_screen_prompt_4;
    GX_PROMPT main_screen_water_level_label;
    GX_NUMERIC_PROMPT main_screen_water_level_value;
    GX_PIXELMAP_BUTTON main_screen_btn_play;
    GX_PROGRESS_BAR main_screen_wash_cycle_progress_bar;
    GX_PROMPT main_screen_label_1_soak;
    GX_PROMPT main_screen_label_2_spin;
    GX_PROMPT main_screen_label_3_wash;
    GX_PROMPT main_screen_label_4_spin;
    GX_PROMPT main_screen_label_5_rinse;
    GX_PROMPT main_screen_label_6_spin;
} MAIN_SCREEN_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern TEMPLATE_MODE_BTN_SMALL_CONTROL_BLOCK template_mode_btn_small;
extern TEMPLATE_MODE_BTN_CONTROL_BLOCK template_mode_btn;
extern MODE_SELECT_WINDOW_CONTROL_BLOCK mode_select_window;
extern WATER_LEVEL_WINDOW_CONTROL_BLOCK water_level_window;
extern TEMPERATURE_WINDOW_CONTROL_BLOCK temperature_window;
extern WASH_CYCLE_WINDOW_CONTROL_BLOCK wash_cycle_window;
extern MAIN_SCREEN_CONTROL_BLOCK main_screen;
#endif

/* Declare event process functions, draw functions, and callback functions     */

VOID time_format(GX_NUMERIC_PROMPT *, INT);
UINT mode_select_window_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT water_level_window_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT temperature_window_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID temperature_window_draw(GX_WINDOW *widget);
VOID temperature_slider_animation_callback(GX_RADIAL_SLIDER *slider);
UINT wash_cycle_window_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID wash_cycle_window_draw(GX_WINDOW *widget);
VOID wash_cycle_slider_draw(GX_RADIAL_SLIDER *widget);
UINT main_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID btn_text_draw(GX_PROMPT *widget);
VOID btn_numeric_text_draw(GX_NUMERIC_PROMPT *widget);
VOID wash_cycle_progress_bar_draw(GX_PROGRESS_BAR *widget);

/* Declare the GX_STUDIO_DISPLAY_INFO structure                                */


typedef struct GX_STUDIO_DISPLAY_INFO_STRUCT 
{
    GX_CONST GX_CHAR *name;
    GX_CONST GX_CHAR *canvas_name;
    GX_CONST GX_THEME **theme_table;
    GX_CONST GX_STRING **language_table;
    USHORT   theme_table_size;
    USHORT   language_table_size;
    UINT     string_table_size;
    UINT     x_resolution;
    UINT     y_resolution;
    GX_DISPLAY *display;
    GX_CANVAS  *canvas;
    GX_WINDOW_ROOT *root_window;
    GX_COLOR   *canvas_memory;
    ULONG      canvas_memory_size;
    USHORT     rotation_angle;
} GX_STUDIO_DISPLAY_INFO;


/* Declare Studio-generated functions for creating top-level widgets           */

UINT gx_studio_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_text_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radial_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radial_slider_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_numeric_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_template_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
GX_WIDGET *gx_studio_widget_create(GX_BYTE *storage, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget);
UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root);

/* Determine if a C++ compiler is being used.  If so, complete the standard
  C conditional started above.                                                 */
#ifdef __cplusplus
}
#endif

#endif                                       /* sentry                         */
