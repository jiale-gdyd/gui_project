#ifndef _DEMO_GUIX_CAR_INFOTAINMENT_SPECIFICATIONS_H_
#define _DEMO_GUIX_CAR_INFOTAINMENT_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define ID_VOLUME_PROGRESS_BAR 1
#define ID_BTN_MUTE 2
#define ID_SCREEN_WEATHER_WIN 3
#define ID_HOME_BTN 4
#define ID_LOGO 5
#define ID_VIDEO_SCREEN 6
#define ID_PLAY_BTN_SMALL 7
#define ID_PLAY_BTN_BIG 8
#define ID_AUDIO_SCREEN 9
#define ID_BTN_KEYPAD 10
#define ID_BTN_END_CALL 11
#define ID_PREVIOUS 12
#define ID_NEXT 13
#define ID_PLAY_BTN 14
#define ID_PHONE_SCREEN 15
#define ID_CLIMATE_SCREEN 16
#define ID_RIGHT_PROGRESS_BAR 17
#define ID_LEFT_PROGRESS_BAR 18
#define ID_LEFT_FAN_BUTTON 19
#define ID_LEFT_SMALL_FAN_BUTTON 20
#define ID_RIGHT_SMALL_FAN_BUTTON 21
#define ID_RIGHT_FAN_BUTTON 22
#define ID_VEHICLE_SCREEN 23
#define ID_MODE_ECO_BTN 24
#define ID_MODE_COMFORT_BTN 25
#define ID_MODE_SPORT_BTN 26
#define ID_LIGHT_SPRITE_LEFT 27
#define ID_LIGHT_SPRITE_RIGHT 28
#define ID_MAIN_SCREEN 29
#define ID_MAIN_WEATHER_WIN 30
#define ID_BTN_WIN 31
#define ID_VEHICLE_BTN 32
#define ID_CLIMATE_BTN 33
#define ID_PHONE_BTN 34
#define ID_AUDIO_BTN 35
#define ID_VIDEO_BTN 36
#define ID_VEHICLE_MODE_BUTTON 37
#define ID_ON_OFF_BTN 38


/* Define animation ids                                                        */

#define ID_ANI_SLIDE_IN_PROGRESS_BAR_WIN 1
#define GX_NEXT_ANIMATION_ID 2


/* Define user event ids                                                       */

enum user_defined_events{
    USER_EVENT_SLIDE_IN_MUSIC_INFO_WIN = GX_FIRST_USER_EVENT,
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
    GX_RESOURCE_ID pixelmap_id;
} GX_ICON_BUTTON_PROPERTIES;

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
   GX_CONST GX_STUDIO_WIDGET *base_info;
   UINT (*base_create_function) (GX_CONST struct GX_STUDIO_WIDGET_STRUCT *, GX_WIDGET *, GX_WIDGET *);
   GX_RECTANGLE size;
} GX_TEMPLATE_PROPERTIES;


/* Declare top-level control blocks                                            */

typedef struct BASE_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_ICON base_logo;
} BASE_CONTROL_BLOCK;

typedef struct MAIN_SCREEN_CONTROL_BLOCK_STRUCT
{
    BASE_CONTROL_BLOCK base;
    GX_ICON main_screen_speedometer;
    GX_NUMERIC_PROMPT main_screen_MPH_val;
    GX_PROMPT main_screen_MPH_label;
    GX_ICON main_screen_gear_position;
    GX_PROMPT main_screen_prompt_11_4;
    GX_PROMPT main_screen_prompt_11_5;
    GX_PROMPT main_screen_prompt_11_6;
    GX_PROMPT main_screen_prompt_11_7;
    GX_WINDOW main_screen_weather_win;
    GX_NUMERIC_PROMPT main_screen_time_hour;
    GX_NUMERIC_PROMPT main_screen_time_minute;
    GX_ICON main_screen_weather_icon;
    GX_NUMERIC_PROMPT main_screen_temperature;
    GX_ICON main_screen_second_dot_up;
    GX_ICON main_screen_second_dot_down;
    GX_WINDOW main_screen_btn_window;
    GX_PIXELMAP_BUTTON main_screen_vehicle_btn;
    GX_ICON main_screen_icon;
    GX_PROMPT main_screen_prompt;
    GX_PIXELMAP_BUTTON main_screen_climate_btn;
    GX_ICON main_screen_icon_1;
    GX_PROMPT main_screen_prompt_1;
    GX_PIXELMAP_BUTTON main_screen_phone_btn;
    GX_ICON main_screen_icon_2;
    GX_PROMPT main_screen_prompt_2;
    GX_PIXELMAP_BUTTON main_screen_audio_btn;
    GX_ICON main_screen_icon_3;
    GX_PROMPT main_screen_prompt_3;
    GX_PIXELMAP_BUTTON main_screen_video_btn;
    GX_ICON main_screen_icon_4;
    GX_PROMPT main_screen_prompt_4;
    GX_NUMERIC_PROMPT main_screen_remain_miles;
    GX_NUMERIC_PROMPT main_screen_remain_battery;
    GX_PROMPT main_screen_prompt_11;
    GX_PROMPT main_screen_prompt_11_1;
    GX_ICON main_screen_horizontal_line;
    GX_ICON main_screen_battery_icon;
    GX_PROGRESS_BAR main_screen_battery_progress_bar;
    GX_PROMPT main_screen_vehicle_mode_text;
    GX_PIXELMAP_BUTTON main_screen_vehicle_mode_button;
    GX_ICON main_screen_vehicle_mode_icon;
    GX_RADIAL_SLIDER main_screen_radial_slider;
    GX_PIXELMAP_BUTTON main_screen_on_off_btn;
    GX_ICON main_screen_icon_5;
    GX_PROMPT main_screen_on_off_text;
} MAIN_SCREEN_CONTROL_BLOCK;

typedef struct SCREEN_BASE_CONTROL_BLOCK_STRUCT
{
    BASE_CONTROL_BLOCK base;
    GX_WINDOW screen_base_weather_win;
    GX_NUMERIC_PROMPT screen_base_hour;
    GX_NUMERIC_PROMPT screen_base_minute;
    GX_ICON screen_base_weather_icon;
    GX_NUMERIC_PROMPT screen_base_temperature;
    GX_ICON screen_base_second_dot_down;
    GX_ICON screen_base_second_dot_up;
    GX_PIXELMAP_BUTTON screen_base_home_btn;
} SCREEN_BASE_CONTROL_BLOCK;

typedef struct VIDEO_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_BASE_CONTROL_BLOCK base;
    GX_WINDOW video_screen_window_1;
    GX_WINDOW video_screen_video_play_progress_bar_bg;
    GX_PROGRESS_BAR video_screen_video_play_progress_bar;
    GX_PIXELMAP_BUTTON video_screen_play_btn_small;
    GX_PIXELMAP_BUTTON video_screen_pixelmap_button_7;
    GX_PIXELMAP_BUTTON video_screen_pixelmap_button_8;
    GX_PIXELMAP_BUTTON video_screen_pixelmap_button_9;
    GX_NUMERIC_PROMPT video_screen_minute;
    GX_NUMERIC_PROMPT video_screen_second;
    GX_PROMPT video_screen_second_colon;
    GX_WINDOW video_screen_play_btn_big_win;
    GX_PIXELMAP_BUTTON video_screen_play_btn_big;
} VIDEO_SCREEN_CONTROL_BLOCK;

typedef struct CLIMATE_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_BASE_CONTROL_BLOCK base;
    GX_WINDOW climate_screen_right_progress_bar_win;
    GX_WINDOW climate_screen_right_progress_bar_bg;
    GX_PROGRESS_BAR climate_screen_right_progress_bar;
    GX_NUMERIC_PROMPT climate_screen_right_progress_val;
    GX_PROMPT climate_screen_prompt;
    GX_WINDOW climate_screen_left_progress_bar_win;
    GX_WINDOW climate_screen_left_progress_bar_bg;
    GX_PROGRESS_BAR climate_screen_left_progress_bar;
    GX_NUMERIC_PROMPT climate_screen_left_progress_val;
    GX_PROMPT climate_screen_prompt_7;
    GX_ICON climate_screen_left_lines_icon;
    GX_PIXELMAP_BUTTON climate_screen_pixelmap_button_3;
    GX_PIXELMAP_BUTTON climate_screen_pixelmap_button;
    GX_ICON climate_screen_right_lines;
    GX_PIXELMAP_BUTTON climate_screen_pixelmap_button_1;
    GX_PIXELMAP_BUTTON climate_screen_pixelmap_button_2;
    GX_ICON climate_screen_center_line;
    GX_ICON climate_screen_right_seat;
    GX_PROMPT climate_screen_left_fan_mode;
    GX_PROMPT climate_screen_right_fan_mode;
    GX_PIXELMAP_BUTTON climate_screen_left_fan_button;
    GX_PIXELMAP_BUTTON climate_screen_left_small_fan_button;
    GX_PIXELMAP_BUTTON climate_screen_right_small_fan_button;
    GX_PIXELMAP_BUTTON climate_screen_right_fan_button;
    GX_ICON climate_screen_left_seat;
    GX_ICON_BUTTON climate_screen_left_warmer_icon_button;
    GX_ICON climate_screen_icon_14_3;
    GX_ICON climate_screen_icon_14_12;
    GX_ICON climate_screen_icon_14_13;
    GX_ICON_BUTTON climate_screen_right_warmer_icon_button;
    GX_ICON climate_screen_icon_14_16_1;
    GX_ICON climate_screen_icon_14_14_1;
    GX_ICON climate_screen_icon_14_15_1;
} CLIMATE_SCREEN_CONTROL_BLOCK;

typedef struct VEHICLE_SCREEN_CONTROL_BLOCK_STRUCT
{
    SCREEN_BASE_CONTROL_BLOCK base;
    GX_PIXELMAP_BUTTON vehicle_screen_mode_eco_btn;
    GX_ICON vehicle_screen_icon_6;
    GX_PROMPT vehicle_screen_prompt_6;
    GX_PIXELMAP_BUTTON vehicle_screen_mode_comfort_btn;
    GX_ICON vehicle_screen_icon;
    GX_PROMPT vehicle_screen_prompt;
    GX_PIXELMAP_BUTTON vehicle_screen_mode_sport_btn;
    GX_ICON vehicle_screen_icon_1;
    GX_PROMPT vehicle_screen_prompt_1;
    GX_PROMPT vehicle_screen_prompt_2;
    GX_PROMPT vehicle_screen_prompt_3;
    GX_ICON vehicle_screen_car_icon;
    GX_SPRITE vehicle_screen_light_sprite_left;
    GX_SPRITE vehicle_screen_light_sprite_right;
    GX_ICON vehicle_screen_icon_8;
    GX_ICON vehicle_screen_icon_9;
    GX_ICON vehicle_screen_icon_2;
    GX_ICON vehicle_screen_icon_3;
    GX_ICON vehicle_screen_icon_4;
    GX_ICON vehicle_screen_icon_5;
    GX_ICON vehicle_screen_icon_10;
    GX_NUMERIC_PROMPT vehicle_screen_battery_usage;
    GX_NUMERIC_PROMPT vehicle_screen_miles;
    GX_NUMERIC_PROMPT vehicle_screen_windshield_washer_level;
    GX_NUMERIC_PROMPT vehicle_screen_lights_bulbs;
    GX_NUMERIC_PROMPT vehicle_screen_air_filter_usage;
    GX_NUMERIC_PROMPT vehicle_screen_front_left_brake_pad;
    GX_NUMERIC_PROMPT vehicle_screen_front_right_brake_pad;
    GX_NUMERIC_PROMPT vehicle_screen_rear_left_brake_pad;
    GX_NUMERIC_PROMPT vehicle_screen_rear_right_brake_pad;
    GX_NUMERIC_PROMPT vehicle_screen_front_left_tire_pressure;
    GX_NUMERIC_PROMPT vehicle_screen_front_right_tire_pressure;
    GX_NUMERIC_PROMPT vehicle_screen_rear_left_tire_pressure;
    GX_NUMERIC_PROMPT vehicle_screen_rear_right_tire_pressure;
    GX_PROMPT vehicle_screen_prompt_13;
    GX_PROMPT vehicle_screen_prompt_13_1;
    GX_PROMPT vehicle_screen_prompt_13_2;
    GX_PROMPT vehicle_screen_prompt_13_3;
    GX_PROMPT vehicle_screen_prompt_13_4;
    GX_PROMPT vehicle_screen_prompt_13_5;
    GX_PROMPT vehicle_screen_prompt_13_6;
    GX_PROMPT vehicle_screen_prompt_13_7;
    GX_PROMPT vehicle_screen_prompt_13_8;
    GX_PROMPT vehicle_screen_prompt_13_9;
    GX_PROMPT vehicle_screen_prompt_13_10;
    GX_PROMPT vehicle_screen_prompt_13_11;
    GX_PROMPT vehicle_screen_prompt_13_11_1;
    GX_PROMPT vehicle_screen_prompt_13_11_2;
    GX_PROMPT vehicle_screen_prompt_13_11_3;
    GX_ICON vehicle_screen_icon_11;
    GX_ICON vehicle_screen_icon_11_1;
} VEHICLE_SCREEN_CONTROL_BLOCK;

typedef struct VOLUME_SCREEN_BASE_CONTROL_BLOCK_STRUCT
{
    SCREEN_BASE_CONTROL_BLOCK base;
    GX_WINDOW volume_screen_base_volume_progress_bar_win;
    GX_WINDOW volume_screen_base_volume_progress_bar_bg;
    GX_PROGRESS_BAR volume_screen_base_volume_progress_bar;
    GX_NUMERIC_PROMPT volume_screen_base_volume_value;
    GX_PROMPT volume_screen_base_volume_label;
    GX_PIXELMAP_BUTTON volume_screen_base_mute_btn;
    GX_PROMPT volume_screen_base_prompt_14;
    GX_ICON volume_screen_base_bute_btn_icon_1;
} VOLUME_SCREEN_BASE_CONTROL_BLOCK;

typedef struct AUDIO_SCREEN_CONTROL_BLOCK_STRUCT
{
    VOLUME_SCREEN_BASE_CONTROL_BLOCK base;
    GX_WINDOW audio_screen_music_info_win;
    GX_PROMPT audio_screen_music_name;
    GX_PROMPT audio_screen_singer_name;
    GX_NUMERIC_PROMPT audio_screen_second;
    GX_PROMPT audio_screen_second_colon;
    GX_NUMERIC_PROMPT audio_screen_minute;
    GX_PIXELMAP_BUTTON audio_screen_keypad_btn;
    GX_PROMPT audio_screen_prompt_6;
    GX_ICON audio_screen_icon;
    GX_PIXELMAP_BUTTON audio_screen_end_call_btn;
    GX_PROMPT audio_screen_prompt_1;
    GX_ICON audio_screen_end_call_btn_icon;
    GX_PIXELMAP_BUTTON audio_screen_pixelmap_button_4;
    GX_PIXELMAP_BUTTON audio_screen_pixelmap_button;
    GX_WINDOW audio_screen_window;
    GX_RADIAL_PROGRESS_BAR audio_screen_radial_progress_bar;
    GX_PIXELMAP_BUTTON audio_screen_play_btn;
    GX_WINDOW audio_screen_play_mode_win;
    GX_PIXELMAP_BUTTON audio_screen_pixelmap_button_1;
    GX_PIXELMAP_BUTTON audio_screen_pixelmap_button_2;
    GX_PIXELMAP_BUTTON audio_screen_pixelmap_button_5;
} AUDIO_SCREEN_CONTROL_BLOCK;

typedef struct PHONE_SCREEN_CONTROL_BLOCK_STRUCT
{
    VOLUME_SCREEN_BASE_CONTROL_BLOCK base;
    GX_PIXELMAP_BUTTON phone_screen_keypad_btn;
    GX_PROMPT phone_screen_prompt_6;
    GX_ICON phone_screen_icon;
    GX_PIXELMAP_BUTTON phone_screen_end_call_btn;
    GX_PROMPT phone_screen_prompt_1;
    GX_ICON phone_screen_end_call_btn_icon;
    GX_WINDOW phone_screen_phone_call_win;
    GX_PROMPT phone_screen_name;
    GX_ICON phone_screen_portait_icon;
    GX_ICON phone_screen_icon_15;
    GX_NUMERIC_PROMPT phone_screen_minute;
    GX_NUMERIC_PROMPT phone_screen_second;
    GX_PROMPT phone_screen_second_colon;
} PHONE_SCREEN_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern VOLUME_SCREEN_BASE_CONTROL_BLOCK volume_screen_base;
extern SCREEN_BASE_CONTROL_BLOCK screen_base;
extern BASE_CONTROL_BLOCK base;
extern VIDEO_SCREEN_CONTROL_BLOCK video_screen;
extern AUDIO_SCREEN_CONTROL_BLOCK audio_screen;
extern PHONE_SCREEN_CONTROL_BLOCK phone_screen;
extern CLIMATE_SCREEN_CONTROL_BLOCK climate_screen;
extern VEHICLE_SCREEN_CONTROL_BLOCK vehicle_screen;
extern MAIN_SCREEN_CONTROL_BLOCK main_screen;
#endif

/* Declare event process functions, draw functions, and callback functions     */

UINT volume_screen_base_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT volume_progress_bar_event_process(GX_PROGRESS_BAR *widget, GX_EVENT *event_ptr);
VOID custom_pixelmap_button_draw(GX_PIXELMAP_BUTTON *widget);
UINT screen_base_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT base_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT video_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID video_screen_pixelmap_button_draw(GX_PIXELMAP_BUTTON *widget);
VOID time_value_format(GX_NUMERIC_PROMPT *, INT);
UINT audio_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT phone_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT climate_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT custom_progress_bar_event_process(GX_PROGRESS_BAR *widget, GX_EVENT *event_ptr);
UINT vehicle_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
UINT main_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID radial_slider_draw(GX_RADIAL_SLIDER *widget);

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

UINT gx_studio_icon_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radial_progress_bar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radial_slider_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_sprite_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_numeric_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_template_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
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
