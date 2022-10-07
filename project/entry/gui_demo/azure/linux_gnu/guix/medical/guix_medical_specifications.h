#ifndef _GUIX_MEDICAL_SPECIFICATIONS_H_
#define _GUIX_MEDICAL_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define ID_SCHEDULE_SCREEN 1
#define ID_ACTIVE_BALL 2
#define ID_PATIENTS_SCREEN 3
#define ID_PATIENT_LIST 4
#define ID_VITALS_SCREEN 5
#define ID_MEDTYPE1_SLIDER 6
#define ID_MEDTYPE3_SLIDER 7
#define ID_MEDTYPE2_SLIDER 8
#define ID_BTN_VITALS 9
#define ID_BTN_SCHEDULE 10
#define ID_BTN_PATIENTS 11


/* Define animation ids                                                        */

#define ID_EKG_WIN_SLIDE_IN 1
#define ID_PULSE_WIN_SLIDE_IN 2
#define ID_R5_WIN_FADE_IN 3
#define GX_NEXT_ANIMATION_ID 4


/* Define user event ids                                                       */

#define GX_NEXT_USER_EVENT_ID GX_FIRST_USER_EVENT

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
    int min_val;
    int max_val;
    int current_val;
    int increment;
    GX_VALUE min_travel;
    GX_VALUE max_travel;
    GX_VALUE needle_width;
    GX_VALUE needle_height;
    GX_VALUE needle_inset;
    GX_VALUE needle_hotspot;
    GX_RESOURCE_ID lower_pixelmap;
    GX_RESOURCE_ID upper_pixelmap;
    GX_RESOURCE_ID needle_pixelmap;
} GX_PIXELMAP_SLIDER_PROPERTIES;

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
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID left_map_id;
    GX_RESOURCE_ID fill_map_id;
    GX_RESOURCE_ID right_map_id;
    GX_RESOURCE_ID selected_left_map_id;
    GX_RESOURCE_ID selected_fill_map_id;
    GX_RESOURCE_ID selected_right_map_id;
} GX_PIXELMAP_PROMPT_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID wallpaper_id;
} GX_WINDOW_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID wallpaper_id;
    VOID (*callback)(GX_VERTICAL_LIST *, GX_WIDGET *, INT);
    int total_rows;
} GX_VERTICAL_LIST_PROPERTIES;

typedef struct
{
   GX_CONST GX_STUDIO_WIDGET *base_info;
   UINT (*base_create_function) (GX_CONST struct GX_STUDIO_WIDGET_STRUCT *, GX_WIDGET *, GX_WIDGET *);
   GX_RECTANGLE size;
} GX_TEMPLATE_PROPERTIES;


/* Declare top-level control blocks                                            */

typedef struct TEMPLATE_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_ICON template_logo;
    GX_TEXT_BUTTON template_vitals_button;
    GX_TEXT_BUTTON template_schedule_button;
    GX_TEXT_BUTTON template_patients_button;
} TEMPLATE_CONTROL_BLOCK;

typedef struct SCHEDULE_SCREEN_CONTROL_BLOCK_STRUCT
{
    TEMPLATE_CONTROL_BLOCK base;
    GX_PROMPT schedule_screen_month;
    GX_WINDOW schedule_screen_schedule_win;
    GX_PROMPT schedule_screen_c1_week;
    GX_PROMPT schedule_screen_c3_week;
    GX_PROMPT schedule_screen_c2_week;
    GX_PROMPT schedule_screen_c4_week;
    GX_PROMPT schedule_screen_c5_week;
    GX_PROMPT schedule_screen_c6_week;
    GX_PROMPT schedule_screen_c7_week;
    GX_NUMERIC_PROMPT schedule_screen_c1_day;
    GX_NUMERIC_PROMPT schedule_screen_c2_day;
    GX_NUMERIC_PROMPT schedule_screen_c3_day;
    GX_NUMERIC_PROMPT schedule_screen_c4_day;
    GX_NUMERIC_PROMPT schedule_screen_c5_day;
    GX_NUMERIC_PROMPT schedule_screen_c6_day;
    GX_NUMERIC_PROMPT schedule_screen_c7_day;
    GX_PROMPT schedule_screen_r1_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r1_hour;
    GX_NUMERIC_PROMPT schedule_screen_r2_hour;
    GX_PROMPT schedule_screen_r2_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r3_hour;
    GX_PROMPT schedule_screen_r3_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r4_hour;
    GX_PROMPT schedule_screen_r4_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r5_hour;
    GX_PROMPT schedule_screen_r5_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r6_hour;
    GX_PROMPT schedule_screen_r6_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r7_hour;
    GX_PROMPT schedule_screen_r7_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r8_hour;
    GX_PROMPT schedule_screen_r8_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r9_hour;
    GX_PROMPT schedule_screen_r9_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r10_hour;
    GX_PROMPT schedule_screen_r10_am_pm;
    GX_NUMERIC_PROMPT schedule_screen_r11_hour;
    GX_PROMPT schedule_screen_r11_am_pm;
    GX_ICON schedule_screen_current_time;
    GX_NUMERIC_PROMPT schedule_screen_hour;
    GX_NUMERIC_PROMPT schedule_screen_minute;
    GX_PROMPT schedule_screen_colon;
    GX_WINDOW schedule_screen_r1_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_3;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_6;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_7;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_45_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_46_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_47_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_93_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_94_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_95_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_98_1;
    GX_WINDOW schedule_screen_r2_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_12;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_15;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_21_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_22_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_23_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_49_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_50_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_69_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_70_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_71_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_100_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_101_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_102_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_103_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_104_1;
    GX_WINDOW schedule_screen_r3_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_16;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_25_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_26_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_53_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_73_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_74_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_107_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_109_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_110_1;
    GX_WINDOW schedule_screen_r4_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_29_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_56_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_77_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_113_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_116_1;
    GX_WINDOW schedule_screen_r5_win;
    GX_ICON schedule_screen_active_circle;
    GX_PIXELMAP_PROMPT schedule_screen_active_bar;
    GX_PROMPT schedule_screen_patient_info;
    GX_PIXELMAP_BUTTON schedule_screen_active_ball;
    GX_WINDOW schedule_screen_r6_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_18;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_10_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_11_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_59_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_119_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_131_1;
    GX_WINDOW schedule_screen_r7_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_13_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_14_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_33_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_34_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_35_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_60_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_61_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_62_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_81_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_82_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_83_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_120_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_121_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_122_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_132_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_133_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_134_1;
    GX_WINDOW schedule_screen_r8_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_17_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_37_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_38_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_64_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_65_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_85_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_86_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_124_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_125_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_136_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_137_1;
    GX_WINDOW schedule_screen_r9_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_20_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_41_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_67_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_68_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_89_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_127_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_128_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_140_1;
    GX_WINDOW schedule_screen_r10_win;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_44_1;
    GX_PIXELMAP_BUTTON schedule_screen_pixelmap_button_92_1;
    GX_NUMERIC_PROMPT schedule_screen_year;
    GX_ICON schedule_screen_icon_1;
    GX_ICON schedule_screen_icon_2;
} SCHEDULE_SCREEN_CONTROL_BLOCK;

typedef struct PATIENTS_SCREEN_CONTROL_BLOCK_STRUCT
{
    TEMPLATE_CONTROL_BLOCK base;
    GX_PROMPT patients_screen_patient_name_label;
    GX_PROMPT patients_screen_total_admited_label;
    GX_NUMERIC_PROMPT patients_screen_total_admitted_count;
    GX_PROMPT patients_screen_DOB_label;
    GX_PROMPT patients_screen_room_label;
    GX_PROMPT patients_screen_admission_date_label;
    GX_PROMPT patients_screen_pa_num_label;
    GX_VERTICAL_LIST patients_screen_patient_list;
    GX_SCROLLBAR patients_screen_vertical_scroll;
    GX_ICON patients_screen_icon;
} PATIENTS_SCREEN_CONTROL_BLOCK;

typedef struct VITALS_SCREEN_CONTROL_BLOCK_STRUCT
{
    TEMPLATE_CONTROL_BLOCK base;
    GX_WINDOW vitals_screen_insulin_win;
    GX_ICON vitals_screen_insulin;
    GX_PROMPT vitals_screen_insulin_name_1;
    GX_PROMPT vitals_screen_insulin_unit_1;
    GX_NUMERIC_PROMPT vitals_screen_insulin_value;
    GX_PROMPT vitals_screen_prompt_17;
    GX_PROMPT vitals_screen_prompt_18;
    GX_PROMPT vitals_screen_prompt_19;
    GX_PROMPT vitals_screen_prompt_20;
    GX_PROMPT vitals_screen_prompt_21;
    GX_NUMERIC_PROMPT vitals_screen_glucose_value;
    GX_PROMPT vitals_screen_prompt_22;
    GX_PROMPT vitals_screen_prompt_1;
    GX_PROMPT vitals_screen_prompt_2;
    GX_WINDOW vitals_screen_ekg_win;
    GX_ICON vitals_screen_heart_rate_icon;
    GX_PROMPT vitals_screen_ekg_label;
    GX_PROMPT vitals_screen_BPM_label;
    GX_NUMERIC_PROMPT vitals_screen_hr_value;
    GX_WINDOW vitals_screen_ekg_waveform_win;
    GX_WINDOW vitals_screen_pulse_win;
    GX_ICON vitals_screen_spo2_icon;
    GX_PROMPT vitals_screen_pulse_label;
    GX_PROMPT vitals_screen_spo2_label;
    GX_NUMERIC_PROMPT vitals_screen_spo2_value;
    GX_WINDOW vitals_screen_pulse_waveform_win;
    GX_WINDOW vitals_screen_blood_pressure_win;
    GX_PROMPT vitals_screen_prompt_23;
    GX_PROMPT vitals_screen_prompt_24;
    GX_PROMPT vitals_screen_prompt_25;
    GX_NUMERIC_PROMPT vitals_screen_last_bolus_value_9;
    GX_PROMPT vitals_screen_prompt_26;
    GX_NUMERIC_PROMPT vitals_screen_last_bolus_value_10;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_12;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_13;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_14;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_15;
    GX_PROMPT vitals_screen_prompt_12_3;
    GX_PROMPT vitals_screen_prompt_12_4;
    GX_PROMPT vitals_screen_prompt_12_5;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_16;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_17;
    GX_PROMPT vitals_screen_prompt_10_6;
    GX_PROMPT vitals_screen_prompt_10_7;
    GX_PROMPT vitals_screen_prompt_10_8;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_18;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_19;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_20;
    GX_PROMPT vitals_screen_prompt_10_9;
    GX_PROMPT vitals_screen_prompt_10_10;
    GX_PROMPT vitals_screen_prompt_10_11;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_21;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_10_1;
    GX_NUMERIC_PROMPT vitals_screen_prompt_11_11_1;
    GX_PROMPT vitals_screen_prompt_26_1;
    GX_WINDOW vitals_screen_patien_info_win;
    GX_PROMPT vitals_screen_patient_gender;
    GX_PROMPT vitals_screen_patient_name;
    GX_PROMPT vitals_screen_patient_dob;
    GX_NUMERIC_PROMPT vitals_screen_patient_age;
    GX_PROMPT vitals_screen_patient_age_label;
    GX_NUMERIC_PROMPT vitals_screen_patient_number;
    GX_PROMPT vitals_screen_patient_number_label;
    GX_PROMPT vitals_screen_patient_room_label;
    GX_NUMERIC_PROMPT vitals_screen_patient_room_id;
    GX_PROMPT vitals_screen_patient_bed_label;
    GX_PROMPT vitals_screen_patient_bed_id;
    GX_WINDOW vitals_screen_temperature_win;
    GX_PROMPT vitals_screen_prompt_15;
    GX_PROMPT vitals_screen_prompt_13_17;
    GX_PROMPT vitals_screen_prompt_13_18;
    GX_NUMERIC_PROMPT vitals_screen_prompt_14_3;
    GX_PROMPT vitals_screen_prompt_13_13_1;
    GX_PROMPT vitals_screen_prompt;
    GX_PROMPT vitals_screen_prompt_5;
    GX_PROMPT vitals_screen_prompt_13_12_2;
    GX_PROMPT vitals_screen_prompt_13_12_3;
    GX_WINDOW vitals_screen_medtype1_win;
    GX_PROMPT vitals_screen_medtype1_label;
    GX_NUMERIC_PROMPT vitals_screen_medtype1_value;
    GX_PROMPT vitals_screen_medtype1_percentage_flag;
    GX_WINDOW vitals_screen_medtype1_slider_win;
    GX_PIXELMAP_SLIDER vitals_screen_medtype1_slider;
    GX_WINDOW vitals_screen_medtype3_win;
    GX_PROMPT vitals_screen_medtype3_label;
    GX_NUMERIC_PROMPT vitals_screen_medtype3_value;
    GX_PROMPT vitals_screen_medtype3_percentage_flag;
    GX_WINDOW vitals_screen_medtype3_slider_win;
    GX_PIXELMAP_SLIDER vitals_screen_medtype3_slider;
    GX_WINDOW vitals_screen_medtype2_win;
    GX_PROMPT vitals_screen_medtype2_label;
    GX_NUMERIC_PROMPT vitals_screen_medtype2_value;
    GX_PROMPT vitals_screen_medtype2_percentage_flag;
    GX_WINDOW vitals_screen_medtype2_slider_win;
    GX_PIXELMAP_SLIDER vitals_screen_medtype2_slider;
} VITALS_SCREEN_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern SCHEDULE_SCREEN_CONTROL_BLOCK schedule_screen;
extern PATIENTS_SCREEN_CONTROL_BLOCK patients_screen;
extern VITALS_SCREEN_CONTROL_BLOCK vitals_screen;
extern TEMPLATE_CONTROL_BLOCK templatet;
#endif

/* Declare event process functions, draw functions, and callback functions     */

UINT schedule_screen_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID schedule_win_draw(GX_WINDOW *widget);
UINT patients_screen_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID patient_row_create(GX_VERTICAL_LIST *, GX_WIDGET *, INT);
UINT vitals_screen_event_process(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID insulin_value_format(GX_NUMERIC_PROMPT *, INT);
UINT template_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);

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

UINT gx_studio_text_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_slider_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_numeric_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_vertical_list_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_vertical_scrollbar_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_template_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
GX_WIDGET *gx_studio_widget_create(GX_BYTE *storage, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget);
UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root);
UINT gx_studio_auto_event_handler(GX_WIDGET *widget, GX_EVENT *event_ptr, GX_CONST GX_STUDIO_EVENT_PROCESS *record);

VOID patient_list_children_create(PATIENTS_SCREEN_CONTROL_BLOCK* win);
VOID hide_active_bar();

/* Determine if a C++ compiler is being used.  If so, complete the standard
  C conditional started above.                                                 */
#ifdef __cplusplus
}
#endif

#endif                                       /* sentry                         */
