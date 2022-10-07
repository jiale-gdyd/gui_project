#ifndef _GUIX_SMART_WATCH_SPECIFICATIONS_H_
#define _GUIX_SMART_WATCH_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define ID_HOME_BUTTON 1
#define ID_PAGE_3_CONTAINER_SCREEN 2
#define ID_PAGE_2_CONTAINER_SCREEN 3
#define ID_PAGE_1_CONTAINER_SCREEN 4
#define ID_CALORIE_BUTTON 5
#define ID_RUNNING_BUTTON 6
#define ID_EKG_BUTTON 7
#define ID_BELL_BUTTON 8
#define ID_PLAY_BUTTON 9
#define ID_REVERSE_BUTTON 10
#define ID_FORWARD_BUTTON 11
#define ID_SAN_FRANCISCO_SCREEN 12
#define ID_LOS_ANGELES_SCREEN 13
#define ID_SAN_DIEGO_SCREEN 14
#define ID_TEMPLATE 15
#define ID_SCREEN_TEMPLATE 16
#define ID_STAND_SCREEN 17
#define ID_RUN_SCREEN 18
#define ID_CALORIES_SCREEN 19
#define ID_STANDING_BUTTON 20
#define ID_CLOCK_2_SCREEN 21
#define ID_CLOCK_1_SCREEN 22


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

typedef struct
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_UBYTE       whitespace;
    GX_BYTE        line_space;
} GX_ML_TEXT_VIEW_PROPERTIES;

typedef struct
{
    GX_VALUE start_angle;
    GX_VALUE animation_steps;
    GX_VALUE animation_delay;
    GX_VALUE needle_xpos;
    GX_VALUE needle_ypos;
    GX_VALUE needle_xcor;
    GX_VALUE needle_ycor;
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
    GX_RESOURCE_ID needle_pixelmap_id;
} GX_CIRCULAR_GAUGE_PROPERTIES;

typedef struct
{
   GX_CONST GX_STUDIO_WIDGET *base_info;
   UINT (*base_create_function) (GX_CONST struct GX_STUDIO_WIDGET_STRUCT *, GX_WIDGET *, GX_WIDGET *);
   GX_RECTANGLE size;
} GX_TEMPLATE_PROPERTIES;


/* Declare top-level control blocks                                            */

typedef struct SCREEN_SLIDE_PARENT_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
} SCREEN_SLIDE_PARENT_CONTROL_BLOCK;

typedef struct HOME_BUTTON_CONTROL_BLOCK_STRUCT
{
    GX_PIXELMAP_BUTTON_MEMBERS_DECLARE
} HOME_BUTTON_CONTROL_BLOCK;

typedef struct PAGE_3_CONTAINER_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
} PAGE_3_CONTAINER_SCREEN_CONTROL_BLOCK;

typedef struct PAGE_2_CONTAINER_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
} PAGE_2_CONTAINER_SCREEN_CONTROL_BLOCK;

typedef struct PAGE_1_CONTAINER_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
} PAGE_1_CONTAINER_SCREEN_CONTROL_BLOCK;

typedef struct MAIN_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
} MAIN_SCREEN_CONTROL_BLOCK;

typedef struct WEATHER_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_BUTTON weather_screen_calorie_button;
    GX_RADIAL_PROGRESS_BAR weather_screen_calorie_burned_progress;
    GX_ICON weather_screen_calorie_icon;
    GX_NUMERIC_PROMPT weather_screen_calorie_burned_progress_val;
    GX_PROMPT weather_screen_prompt;
    GX_BUTTON weather_screen_running_button;
    GX_RADIAL_PROGRESS_BAR weather_screen_running_progress;
    GX_ICON weather_screen_icon_5;
    GX_NUMERIC_PROMPT weather_screen_running_progress_val;
    GX_PROMPT weather_screen_prompt_9;
    GX_BUTTON weather_screen_ekg_button;
    GX_RADIAL_PROGRESS_BAR weather_screen_ekg_progress;
    GX_ICON weather_screen_icon_6;
    GX_NUMERIC_PROMPT weather_screen_ekg_progress_val;
    GX_PROMPT weather_screen_prompt_11;
    GX_WINDOW weather_screen_window;
    GX_ICON weather_screen_weather_icon;
    GX_NUMERIC_PROMPT weather_screen_temperature_val;
    GX_PROMPT weather_screen_prompt_1;
    GX_PROMPT weather_screen_day_of_week;
    GX_PROMPT weather_screen_date;
    GX_PROMPT weather_screen_city;
    GX_PROMPT weather_screen_second;
    GX_NUMERIC_PROMPT weather_screen_hour;
    GX_NUMERIC_PROMPT weather_screen_minute;
    GX_PROMPT weather_screen_am_pm;
    GX_PIXELMAP_BUTTON weather_screen_bell_button;
    GX_PROMPT weather_screen_singer;
    GX_PROMPT weather_screen_song;
    GX_RADIAL_PROGRESS_BAR weather_screen_music_play_progress;
    GX_PIXELMAP_BUTTON weather_screen_play_button;
} WEATHER_SCREEN_CONTROL_BLOCK;

typedef struct CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_CIRCULAR_GAUGE clock_screen_template_gauge;
} CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK;

typedef struct MESSAGE_TEMPLATE_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_WINDOW message_template_message_bar;
    GX_PROMPT message_template_second;
    GX_NUMERIC_PROMPT message_template_hour;
    GX_NUMERIC_PROMPT message_template_minute;
    GX_PROMPT message_template_am_pm;
    GX_PROMPT message_template_day_of_week;
    GX_PROMPT message_template_comma;
    GX_MULTI_LINE_TEXT_VIEW message_template_text_view;
} MESSAGE_TEMPLATE_CONTROL_BLOCK;

typedef struct WEATHER_INFO_TEMPLATE_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_PROMPT weather_info_template_day_of_week;
    GX_PROMPT weather_info_template_prompt_6;
    GX_ICON weather_info_template_weather_icon;
    GX_NUMERIC_PROMPT weather_info_template_high_temperature;
    GX_NUMERIC_PROMPT weather_info_template_low_temperature;
    GX_PROMPT weather_info_template_prompt_1;
} WEATHER_INFO_TEMPLATE_CONTROL_BLOCK;

typedef struct SCREEN_TEMPLATE_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_WINDOW screen_template_title_bar;
    GX_PROMPT screen_template_title;
    GX_NUMERIC_PROMPT screen_template_hour;
    GX_NUMERIC_PROMPT screen_template_minute;
    GX_PROMPT screen_template_second;
    GX_ICON screen_template_icon;
} SCREEN_TEMPLATE_CONTROL_BLOCK;

typedef struct EKG_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_RADIAL_PROGRESS_BAR ekg_screen_progress;
    GX_NUMERIC_PROMPT ekg_screen_systole_blood_pressure;
    GX_ICON ekg_screen_heart_icon;
    GX_NUMERIC_PROMPT ekg_screen_diastole_blood_pressure;
    GX_PROMPT ekg_screen_prompt_21;
    GX_PROMPT ekg_screen_prompt_21_1;
    GX_ICON ekg_screen_icon_21;
    GX_NUMERIC_PROMPT ekg_screen_heart_rate;
    GX_PROMPT ekg_screen_prompt_20;
} EKG_SCREEN_CONTROL_BLOCK;

typedef struct YOGA_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_RADIAL_PROGRESS_BAR yoga_screen_progress;
    GX_NUMERIC_PROMPT yoga_screen_calories_burned;
    GX_ICON yoga_screen_divide_line;
    GX_ICON yoga_screen_yoga_icon;
    GX_PROMPT yoga_screen_prompt_22;
    GX_NUMERIC_PROMPT yoga_screen_yoga_minute;
    GX_NUMERIC_PROMPT yoga_screen_yoga_hour;
    GX_PROMPT yoga_screen_calorie_label;
    GX_ICON yoga_screen_calorie_icon;
    GX_ICON yoga_screen_icon_18;
    GX_NUMERIC_PROMPT yoga_screen_progress_val;
    GX_PROMPT yoga_screen_prompt_18;
} YOGA_SCREEN_CONTROL_BLOCK;

typedef struct STAND_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_RADIAL_PROGRESS_BAR stand_screen_progress;
    GX_ICON stand_screen_divide_line;
    GX_ICON stand_screen_stand_icon;
    GX_PROMPT stand_screen_min_label;
    GX_ICON stand_screen_clock_icon;
    GX_NUMERIC_PROMPT stand_screen_stand_minute_target;
    GX_PROMPT stand_screen_prompt_22;
    GX_NUMERIC_PROMPT stand_screen_stand_hour_target;
    GX_ICON stand_screen_icon_8;
    GX_NUMERIC_PROMPT stand_screen_stand_hour;
    GX_NUMERIC_PROMPT stand_screen_stand_minute;
    GX_ICON stand_screen_icon;
    GX_ICON stand_screen_icon_15;
    GX_NUMERIC_PROMPT stand_screen_progress_val;
    GX_PROMPT stand_screen_prompt_16;
} STAND_SCREEN_CONTROL_BLOCK;

typedef struct RUN_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_RADIAL_PROGRESS_BAR run_screen_progress;
    GX_NUMERIC_PROMPT run_screen_running_distance;
    GX_NUMERIC_PROMPT run_screen_running_minutes;
    GX_ICON run_screen_divide_line;
    GX_ICON run_screen_running_icon;
    GX_NUMERIC_PROMPT run_screen_running_seconds;
    GX_PROMPT run_screen_second_label;
    GX_ICON run_screen_distance_icon;
    GX_PROMPT run_screen_mile_label;
    GX_NUMERIC_PROMPT run_screen_running_distance_fraction_part;
    GX_ICON run_screen_icon_8;
    GX_ICON run_screen_icon_12;
    GX_NUMERIC_PROMPT run_screen_progress_val;
    GX_PROMPT run_screen_prompt_14;
} RUN_SCREEN_CONTROL_BLOCK;

typedef struct CALORIES_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_RADIAL_PROGRESS_BAR calories_screen_progress;
    GX_NUMERIC_PROMPT calories_screen_calories_burned;
    GX_NUMERIC_PROMPT calories_screen_calories_burned_target;
    GX_ICON calories_screen_divide_line;
    GX_ICON calories_screen_calories_icon;
    GX_ICON calories_screen_icon_7;
    GX_NUMERIC_PROMPT calories_screen_progress_val;
    GX_PROMPT calories_screen_percent_label;
} CALORIES_SCREEN_CONTROL_BLOCK;

typedef struct FITNESS_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_ICON fitness_screen_icon;
    GX_ICON fitness_screen_icon_1;
    GX_ICON fitness_screen_icon_2;
    GX_ICON fitness_screen_icon_3;
    GX_NUMERIC_PROMPT fitness_screen_running_steps;
    GX_NUMERIC_PROMPT fitness_screen_running_distance;
    GX_NUMERIC_PROMPT fitness_screen_prompt_3;
    GX_PROMPT fitness_screen_prompt_4;
    GX_PROMPT fitness_screen_prompt_5;
    GX_PROMPT fitness_screen_prompt_2;
    GX_ICON fitness_screen_ekd_dot_guide_bg;
    GX_ICON fitness_screen_ekg_dot_icon;
    GX_WINDOW fitness_screen_ekg_waveform_win;
    GX_BUTTON fitness_screen_calorie_button;
    GX_RADIAL_PROGRESS_BAR fitness_screen_calorie_burned_progress;
    GX_ICON fitness_screen_calorie_icon;
    GX_NUMERIC_PROMPT fitness_screen_calorie_burned_progress_val;
    GX_PROMPT fitness_screen_prompt_7;
    GX_BUTTON fitness_screen_standing_button;
    GX_RADIAL_PROGRESS_BAR fitness_screen_standing_progress;
    GX_ICON fitness_screen_icon_6;
    GX_NUMERIC_PROMPT fitness_screen_standing_progress_val;
    GX_PROMPT fitness_screen_prompt_11;
    GX_BUTTON fitness_screen_running_button;
    GX_RADIAL_PROGRESS_BAR fitness_screen_running_progress;
    GX_ICON fitness_screen_icon_5;
    GX_NUMERIC_PROMPT fitness_screen_running_progress_val;
    GX_PROMPT fitness_screen_prompt_9;
} FITNESS_SCREEN_CONTROL_BLOCK;

typedef struct CLOCK_2_SCREEN_CONTROL_BLOCK_STRUCT
{
    CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK base;
} CLOCK_2_SCREEN_CONTROL_BLOCK;

typedef struct CLOCK_1_SCREEN_CONTROL_BLOCK_STRUCT
{
    CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK base;
} CLOCK_1_SCREEN_CONTROL_BLOCK;

typedef struct CLOCK_5_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_NUMERIC_PROMPT clock_5_screen_hour;
    GX_NUMERIC_PROMPT clock_5_screen_minute;
} CLOCK_5_SCREEN_CONTROL_BLOCK;

typedef struct CLOCK_3_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_NUMERIC_PROMPT clock_3_screen_hour;
    GX_NUMERIC_PROMPT clock_3_screen_minute;
} CLOCK_3_SCREEN_CONTROL_BLOCK;

typedef struct CLOCK_4_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_NUMERIC_PROMPT clock_4_screen_hour;
    GX_NUMERIC_PROMPT clock_4_screen_minute;
} CLOCK_4_SCREEN_CONTROL_BLOCK;

typedef struct HOME_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_SPRITE home_screen_sprite;
    GX_NUMERIC_PROMPT home_screen_time_hour;
    GX_NUMERIC_PROMPT home_screen_time_minute;
    GX_ICON home_screen_time_second_up;
    GX_ICON home_screen_time_second_down;
    GX_PROMPT home_screen_am_pm;
    GX_PROMPT home_screen_day_of_week;
    GX_PROMPT home_screen_date;
} HOME_SCREEN_CONTROL_BLOCK;

typedef struct MUSIC_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_RADIAL_PROGRESS_BAR music_screen_play_progress;
    GX_PIXELMAP_BUTTON music_screen_play_button;
    GX_PROMPT music_screen_singer;
    GX_PROMPT music_screen_song;
    GX_PIXELMAP_BUTTON music_screen_reverse_button;
    GX_PIXELMAP_BUTTON music_screen_forward_button;
    GX_NUMERIC_PROMPT music_screen_hour;
    GX_NUMERIC_PROMPT music_screen_minute;
    GX_PROMPT music_screen_second;
    GX_WINDOW music_screen_play_mode_win;
    GX_PIXELMAP_BUTTON music_screen_play_list_button;
    GX_PIXELMAP_BUTTON music_screen_play_randon_button;
    GX_PIXELMAP_BUTTON music_screen_play_repeat_button;
} MUSIC_SCREEN_CONTROL_BLOCK;

typedef struct MESSAGE_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_WINDOW message_screen_message_frame;
    GX_SCROLLBAR message_screen_vertical_scroll_1;
} MESSAGE_SCREEN_CONTROL_BLOCK;

typedef struct WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK_STRUCT
{
    SCREEN_TEMPLATE_CONTROL_BLOCK base;
    GX_WINDOW weather_screen_template_weather_frame;
    GX_SCROLLBAR weather_screen_template_vertical_scroll;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_7;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_6;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_5;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_4;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_3;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_2;
    WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_screen_template_day_win_1;
} WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK;

typedef struct SANFRANCISCO_WEATHER_SCREEN_CONTROL_BLOCK_STRUCT
{
    WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK base;
} SANFRANCISCO_WEATHER_SCREEN_CONTROL_BLOCK;

typedef struct LOSANGELES_WEATHER_SCREEN_CONTROL_BLOCK_STRUCT
{
    WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK base;
} LOSANGELES_WEATHER_SCREEN_CONTROL_BLOCK;

typedef struct SANDIEGO_WEATHER_SCREEN_CONTROL_BLOCK_STRUCT
{
    WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK base;
} SANDIEGO_WEATHER_SCREEN_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern SCREEN_SLIDE_PARENT_CONTROL_BLOCK screen_slide_parent;
extern HOME_BUTTON_CONTROL_BLOCK home_button;
extern PAGE_3_CONTAINER_SCREEN_CONTROL_BLOCK page_3_container_screen;
extern PAGE_2_CONTAINER_SCREEN_CONTROL_BLOCK page_2_container_screen;
extern PAGE_1_CONTAINER_SCREEN_CONTROL_BLOCK page_1_container_screen;
extern MAIN_SCREEN_CONTROL_BLOCK main_screen;
extern WEATHER_SCREEN_CONTROL_BLOCK weather_screen;
extern MUSIC_SCREEN_CONTROL_BLOCK music_screen;
extern MESSAGE_SCREEN_CONTROL_BLOCK message_screen;
extern SANFRANCISCO_WEATHER_SCREEN_CONTROL_BLOCK SanFrancisco_weather_screen;
extern LOSANGELES_WEATHER_SCREEN_CONTROL_BLOCK LosAngeles_weather_screen;
extern SANDIEGO_WEATHER_SCREEN_CONTROL_BLOCK SanDiego_weather_screen;
extern CLOCK_SCREEN_TEMPLATE_CONTROL_BLOCK clock_screen_template;
extern MESSAGE_TEMPLATE_CONTROL_BLOCK message_template;
extern WEATHER_INFO_TEMPLATE_CONTROL_BLOCK weather_info_template;
extern WEATHER_SCREEN_TEMPLATE_CONTROL_BLOCK weather_screen_template;
extern SCREEN_TEMPLATE_CONTROL_BLOCK screen_template;
extern EKG_SCREEN_CONTROL_BLOCK ekg_screen;
extern YOGA_SCREEN_CONTROL_BLOCK yoga_screen;
extern STAND_SCREEN_CONTROL_BLOCK stand_screen;
extern RUN_SCREEN_CONTROL_BLOCK run_screen;
extern CALORIES_SCREEN_CONTROL_BLOCK calories_screen;
extern FITNESS_SCREEN_CONTROL_BLOCK fitness_screen;
extern CLOCK_2_SCREEN_CONTROL_BLOCK clock_2_screen;
extern CLOCK_1_SCREEN_CONTROL_BLOCK clock_1_screen;
extern CLOCK_5_SCREEN_CONTROL_BLOCK clock_5_screen;
extern CLOCK_3_SCREEN_CONTROL_BLOCK clock_3_screen;
extern CLOCK_4_SCREEN_CONTROL_BLOCK clock_4_screen;
extern HOME_SCREEN_CONTROL_BLOCK home_screen;
#endif

/* Declare event process functions, draw functions, and callback functions     */

VOID root_home_button_draw(GX_PIXELMAP_BUTTON *widget);
UINT main_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT weather_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID time_format(GX_NUMERIC_PROMPT *, INT);
UINT music_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID custom_pixelmap_button_draw(GX_PIXELMAP_BUTTON *widget);
UINT message_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT clock_screen_template_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT weather_screen_template_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT screen_template_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT ekg_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT yoga_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT stand_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT run_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT calories_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT fitness_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID ekg_waveform_win_draw(GX_WINDOW *widget);
UINT clock_5_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID clock_5_screen_draw(GX_WINDOW *widget);
UINT clock_3_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID clock_3_screen_draw(GX_WINDOW *widget);
UINT clock_4_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT home_screen_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);

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
UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radial_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_sprite_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_numeric_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_multi_line_text_view_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_vertical_scrollbar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_circular_gauge_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
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
