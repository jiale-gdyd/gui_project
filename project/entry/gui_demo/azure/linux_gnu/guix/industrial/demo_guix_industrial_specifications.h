#ifndef _DEMO_GUIX_INDUSTRIAL_SPECIFICATIONS_H_
#define _DEMO_GUIX_INDUSTRIAL_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define ID_COMPLETE_WINDOW 1
#define ID_SEQUENCE_WINDOW 2
#define ID_AUXILIARY 3
#define ID_PALLET_FRICTION 4
#define ID_UNIT_SET 5
#define ID_MAIN_SCREEN 6
#define ID_ON_OFF 7
#define ID_INSPECTING 8
#define ID_PALLETIZING 9
#define ID_WELDING 10
#define ID_ASSEMBLING 11
#define ID_START_WINDOW 12
#define ID_START 13


/* Define animation ids                                                        */

#define ANI_ID_COMPLETE_WIN_FADE_IN 1
#define ANI_ID_COMPLETE_WIN_FADE_OUT 2
#define ANI_ID_MODE_WIN_SLIDE_IN 3
#define ANI_ID_SEQ_WIN_FADE_IN 4
#define ANI_ID_SEQ_WIN_FADE_OUT 5
#define ANI_ID_SPEED_WIN_FADE_IN 6
#define ANI_ID_START_BTN_SLIDE_IN 7
#define ANI_ID_START_BTN_SLIDE_OUT 8
#define ANI_ID_TIMER_WIN_SLIDE_IN 9
#define GX_NEXT_ANIMATION_ID 10


/* Define user event ids                                                       */

enum user_defined_events{
    USER_EVENT_SEQ_WIN_FADE_IN = GX_FIRST_USER_EVENT,
    USER_EVENT_SEQ_WIN_FADE_OUT,
    USER_EVENT_COMPLETE_WIN_FADE_IN,
    USER_EVENT_COMPLETE_WIN_FADE_OUT,
    USER_EVENT_START_WIN_FADE_OUT,
    USER_EVENT_START_WIN_FADE_IN,
    GX_NEXT_USER_EVENT_ID
};

#define GX_ACTION_FLAG_DYNAMIC_TARGET 0x01
#define GX_ACTION_FLAG_DYNAMIC_PARENT 0x02
#define GX_ACTION_FLAG_POP_TARGET     0x04
#define GX_ACTION_FLAG_POP_PARENT     0x08

typedef struct GX_STUDIO_ACTION_STRUCT
{
    GX_UBYTE opcode;
    GX_UBYTE flags;
    GX_CONST VOID *parent;
    GX_CONST VOID *target;
    GX_CONST GX_ANIMATION_INFO  *animation;
} GX_STUDIO_ACTION;

typedef struct GX_STUDIO_EVENT_ENTRY_STRUCT
{
    ULONG event_type;
    USHORT event_sender;
    GX_CONST GX_STUDIO_ACTION *action_list;
} GX_STUDIO_EVENT_ENTRY;

typedef struct GX_STUDIO_EVENT_PROCESS_STRUCT 
{
    GX_CONST GX_STUDIO_EVENT_ENTRY *event_table;
    UINT (*chain_event_handler)(GX_WIDGET *, GX_EVENT *);
} GX_STUDIO_EVENT_PROCESS;

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
    GX_SPRITE_FRAME *frame_list;
    USHORT           frame_count;
} GX_SPRITE_PROPERTIES;

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


/* Declare top-level control blocks                                            */

typedef struct SEQUENCE_NUMBER_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_PROMPT sequence_number_prompt_18_2;
    GX_PROMPT sequence_number_prompt_18_3;
} SEQUENCE_NUMBER_CONTROL_BLOCK;

typedef struct COMPLETE_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_ICON complete_window_icon_12_2;
    GX_PROMPT complete_window_prompt_17_7;
    GX_PROMPT complete_window_prompt_17;
    GX_PROMPT complete_window_prompt_17_2;
    GX_PROMPT complete_window_prompt_17_3;
    GX_PROMPT complete_window_prompt_17_4;
    GX_NUMERIC_PROMPT complete_window_countdown;
    GX_ICON complete_window_icon_13_2;
} COMPLETE_WINDOW_CONTROL_BLOCK;

typedef struct SEQUENCE_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_WINDOW sequence_window_window_progress;
    GX_PROGRESS_BAR sequence_window_bottom_progress;
    GX_PROMPT sequence_window_prompt_7;
    GX_PROMPT sequence_window_progress_1_title;
    GX_PROMPT sequence_window_progress_2_title;
    GX_PROMPT sequence_window_progress_3_title;
    GX_PROMPT sequence_window_progress_4_title;
    GX_PROGRESS_BAR sequence_window_progress_1;
    GX_PROGRESS_BAR sequence_window_progress_2;
    GX_PROGRESS_BAR sequence_window_progress_3;
    GX_PROGRESS_BAR sequence_window_progress_4;
    GX_ICON sequence_window_complete_icon_1;
    GX_ICON sequence_window_complete_icon_2;
    GX_ICON sequence_window_complete_icon_3;
    GX_ICON sequence_window_complete_icon_4;
    GX_WINDOW sequence_window_window_mode;
    GX_PROMPT sequence_window_prompt_15_5;
    GX_PROMPT sequence_window_prompt_15_6;
    GX_PROMPT sequence_window_prompt_15_7;
    GX_PROMPT sequence_window_prompt_15_8;
    GX_PROMPT sequence_window_prompt_15_10_1;
    GX_PROMPT sequence_window_prompt_15_11_5;
    GX_RADIAL_PROGRESS_BAR sequence_window_radial_progress_speed;
    GX_NUMERIC_PROMPT sequence_window_prompt_SX;
    GX_NUMERIC_PROMPT sequence_window_prompt_SY;
    GX_WINDOW sequence_window_window_rotation_angle;
    GX_PROMPT sequence_window_prompt_15_13_2;
    GX_PROMPT sequence_window_prompt_15_14_2;
    GX_PROMPT sequence_window_prompt_15_15_2;
    GX_RADIAL_PROGRESS_BAR sequence_window_radial_progress_rotation;
    GX_NUMERIC_PROMPT sequence_window_prompt_RY;
    GX_NUMERIC_PROMPT sequence_window_prompt_RX;
    GX_PROMPT sequence_window_prompt_15_11_2;
    GX_PROMPT sequence_window_prompt_15_11;
    GX_WINDOW sequence_window_window_force_sensor;
    GX_PROMPT sequence_window_prompt_15_13;
    GX_PROMPT sequence_window_prompt_15_14;
    GX_PROMPT sequence_window_prompt_15_15;
    GX_NUMERIC_PROMPT sequence_window_prompt_DN;
    GX_NUMERIC_PROMPT sequence_window_prompt_UP;
    GX_PROMPT sequence_window_prompt_15_11_1;
    GX_PROMPT sequence_window_prompt_15_11_3;
    GX_RADIAL_PROGRESS_BAR sequence_window_radial_progress_force;
    GX_NUMERIC_PROMPT sequence_window_progress_1_value;
    GX_NUMERIC_PROMPT sequence_window_progress_2_value;
    GX_NUMERIC_PROMPT sequence_window_progress_3_value;
    GX_NUMERIC_PROMPT sequence_window_progress_4_value;
    GX_ICON sequence_window_timer_icon;
    GX_PROMPT sequence_window_overall_label;
    GX_NUMERIC_PROMPT sequence_window_timer_tick;
    GX_NUMERIC_PROMPT sequence_window_timer_minute;
    GX_NUMERIC_PROMPT sequence_window_timer_second;
    GX_PROMPT sequence_window_timer_colon;
    GX_PROMPT sequence_window_timer_dot;
    GX_ICON sequence_window_task_icon;
    GX_PROMPT sequence_window_task_title;
    GX_NUMERIC_PROMPT sequence_window_bottom_progress_value;
    GX_WINDOW sequence_window_sprite_window;
    GX_PROMPT sequence_window_prompt_X_label;
    GX_NUMERIC_PROMPT sequence_window_prompt_X;
    GX_PROMPT sequence_window_prompt_15_16_11;
    GX_PROMPT sequence_window_prompt_Y_label;
    GX_NUMERIC_PROMPT sequence_window_prompt_Y;
    GX_PROMPT sequence_window_prompt_15_16_10_1;
    GX_PROMPT sequence_window_prompt_Z_label;
    GX_NUMERIC_PROMPT sequence_window_prompt_Z;
    GX_PROMPT sequence_window_prompt_15_16_14;
    GX_SPRITE sequence_window_sprite;
    GX_PROMPT sequence_window_progress_1_percent_flag;
    GX_PROMPT sequence_window_progress_2_percent_flag;
    GX_PROMPT sequence_window_progress_3_percent_flag;
    GX_PROMPT sequence_window_progress_4_percent_flag;
} SEQUENCE_WINDOW_CONTROL_BLOCK;

typedef struct MAIN_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_PIXELMAP_BUTTON main_screen_button_home;
    GX_ICON main_screen_expresslogic_label;
    GX_PIXELMAP_BUTTON main_screen_button_on_off;
    GX_PROMPT main_screen_prompt_on_off;
    GX_ICON main_screen_icon_9;
    GX_WINDOW main_screen_button_window;
    GX_PIXELMAP_BUTTON main_screen_button_inspecting;
    GX_PROMPT main_screen_prompt_14_4;
    GX_ICON main_screen_icon;
    GX_PIXELMAP_BUTTON main_screen_button_palletizing;
    GX_PROMPT main_screen_prompt_14;
    GX_ICON main_screen_icon_1;
    GX_ICON main_screen_button_indicator_dot;
    GX_PIXELMAP_BUTTON main_screen_button_welding;
    GX_PROMPT main_screen_prompt_14_3;
    GX_ICON main_screen_icon_2;
    GX_PIXELMAP_BUTTON main_screen_button_assembling;
    GX_PROMPT main_screen_prompt_14_2;
    GX_ICON main_screen_icon_3;
    GX_WINDOW main_screen_start_window;
    GX_PIXELMAP_BUTTON main_screen_button_start;
    GX_ICON main_screen_icon_robot;
    GX_PROMPT main_screen_prompt_initiate;
    GX_PROMPT main_screen_prompt_Start;
    GX_WINDOW main_screen_window_mode;
    GX_PROMPT main_screen_prompt_3;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_2;
    GX_PROMPT main_screen_prompt_4;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_3;
    GX_PROMPT main_screen_prompt_5;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_4;
    GX_PROMPT main_screen_prompt_6;
    GX_WINDOW main_screen_window_speed;
    GX_PROMPT main_screen_prompt;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button;
    GX_PROMPT main_screen_prompt_1;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_1;
    GX_PROMPT main_screen_prompt_2;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_10;
    GX_PROMPT main_screen_prompt_10;
    GX_WINDOW main_screen_window_timer;
    GX_PROMPT main_screen_prompt_11;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_11;
    GX_PROMPT main_screen_prompt_12;
    GX_PIXELMAP_BUTTON main_screen_pixelmap_button_12;
    GX_PROMPT main_screen_prompt_13;
    GX_PROMPT main_screen_prompt_initiate_1;
} MAIN_SCREEN_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern SEQUENCE_NUMBER_CONTROL_BLOCK sequence_number;
extern COMPLETE_WINDOW_CONTROL_BLOCK complete_window;
extern SEQUENCE_WINDOW_CONTROL_BLOCK sequence_window;
extern MAIN_SCREEN_CONTROL_BLOCK main_screen;
#endif

/* Declare event process functions, draw functions, and callback functions     */

UINT complete_window_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT sequence_window_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID mode_value_format(GX_NUMERIC_PROMPT *, INT);
VOID timer_value_format(GX_NUMERIC_PROMPT *, INT);
VOID task_icon_draw(GX_ICON *widget);
VOID task_title_draw(GX_PROMPT *widget);
VOID position_value_format(GX_NUMERIC_PROMPT *, INT);
UINT main_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID main_screen_draw(GX_WINDOW *widget);
VOID mode_button_draw(GX_PIXELMAP_BUTTON *widget);

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

UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radial_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_sprite_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_numeric_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
GX_WIDGET *gx_studio_widget_create(GX_BYTE *storage, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget);
UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root);
UINT gx_studio_auto_event_handler(GX_WIDGET *widget, GX_EVENT *event_ptr, GX_CONST GX_STUDIO_EVENT_PROCESS *record);

/* Determine if a C++ compiler is being used.  If so, complete the standard
  C conditional started above.                                                 */
#ifdef __cplusplus
}
#endif

#endif                                       /* sentry                         */
