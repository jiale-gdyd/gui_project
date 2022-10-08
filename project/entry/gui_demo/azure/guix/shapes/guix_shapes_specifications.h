#ifndef _GUIX_SHAPES_SPECIFICATIONS_H_
#define _GUIX_SHAPES_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define TEXT_WIN_8BPP 1
#define TEXT_WIN_4BPP 2
#define TEXT_WIN_1BPP 3
#define DYNAMIC_TEXT_WINDOW 4
#define ID_ROTATION_ANGLE_VAL 5
#define ID_ROTATION_ANGLE_SLIDER 6
#define ID_AUTO_ROTATE 7
#define ID_AUTO_SHIFT 8
#define ID_RED_TEXT 9
#define ID_BLUE_TEXT 10
#define ID_GREEN_TEXT 11
#define ID_SHAPES_SCREEN 12
#define ID_CIRCLE 13
#define ID_ARC 14
#define ID_PIE 15
#define ID_POLYGON 16
#define ID_ELLIPSE 17
#define ID_RECTANGLE 18
#define ID_TEXT 19
#define ID_SHAPES_WINDOW 20
#define ID_GRAPHICS_WINDOW 21
#define ID_RADIUS_PROMPT_VAL 22
#define ID_RADIUS_SLIDER 23
#define ID_BRUSH_WIDTH_VAL 24
#define ID_BRUSH_WIDTH 25
#define ID_ANTI_ALIASED 26
#define ID_ROUND_END 27
#define ID_SOLID_FILL 28
#define ID_PIXELMAP_FILL 29


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
    GX_RESOURCE_ID unchecked_pixelmap_id;
    GX_RESOURCE_ID checked_pixelmap_id;
    GX_RESOURCE_ID unchecked_disabled_pixelmap_id;
    GX_RESOURCE_ID checked_disabled_pixelmap_id;
} GX_CHECKBOX_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID string_id; 
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID off_pixelmap_id;
    GX_RESOURCE_ID on_pixelmap_id;
    GX_RESOURCE_ID off_disabled_pixelmap_id;
    GX_RESOURCE_ID on_disabled_pixelmap_id;
} GX_RADIO_BUTTON_PROPERTIES;

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


/* Declare top-level control blocks                                            */

typedef struct TEXT_ROTAION_WINDOW_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_WINDOW Text_Rotaion_Window_Text_Window;
    GX_WINDOW Text_Rotaion_Window_text_window_8bpp;
    GX_WINDOW Text_Rotaion_Window_text_window_4bpp;
    GX_WINDOW Text_Rotaion_Window_text_window_1bpp;
    GX_WINDOW Text_Rotaion_Window_dynamic_text_window;
    GX_PROMPT Text_Rotaion_Window_prompt_rotate_angle;
    GX_PIXELMAP_PROMPT Text_Rotaion_Window_rotation_angle_value;
    GX_PIXELMAP_SLIDER Text_Rotaion_Window_Rotation_Angle_slider;
    GX_CHECKBOX Text_Rotaion_Window_checkbox_auto_rotate;
    GX_CHECKBOX Text_Rotaion_Window_checkbox_auto_shift;
    GX_WINDOW Text_Rotaion_Window_color_group_window;
    GX_PROMPT Text_Rotaion_Window_prompt_text_color;
    GX_RADIO_BUTTON Text_Rotaion_Window_radio_button_red;
    GX_RADIO_BUTTON Text_Rotaion_Window_radio_button_blue;
    GX_RADIO_BUTTON Text_Rotaion_Window_radio_button_greeb;
} TEXT_ROTAION_WINDOW_CONTROL_BLOCK;

typedef struct SHAPES_SCREEN_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_WINDOW Shapes_Screen_shape_type_container;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_circle;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_arc;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_pie;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_polygon;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_ellipse;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_rectangle;
    GX_RADIO_BUTTON Shapes_Screen_radio_button_text;
    GX_WINDOW Shapes_Screen_Shapes_Window;
    GX_WINDOW Shapes_Screen_graphics_window;
    GX_PROMPT Shapes_Screen_prompt_radius;
    GX_PIXELMAP_PROMPT Shapes_Screen_radius_val;
    GX_PIXELMAP_SLIDER Shapes_Screen_radius_slider;
    GX_PROMPT Shapes_Screen_prompt_brush_width;
    GX_PIXELMAP_PROMPT Shapes_Screen_brush_width_val;
    GX_PIXELMAP_SLIDER Shapes_Screen_brush_width_slider;
    GX_PROMPT Shapes_Screen_prompt_brush_style;
    GX_CHECKBOX Shapes_Screen_checkbox_anti_alised;
    GX_CHECKBOX Shapes_Screen_checkbox_round_end;
    GX_CHECKBOX Shapes_Screen_checkbox_fill;
    GX_CHECKBOX Shapes_Screen_checkbox_pixelmap_fill;
    GX_ICON Shapes_Screen_express_icon;
} SHAPES_SCREEN_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern TEXT_ROTAION_WINDOW_CONTROL_BLOCK Text_Rotaion_Window;
extern SHAPES_SCREEN_CONTROL_BLOCK Shapes_Screen;
#endif

/* Declare event process functions, draw functions, and callback functions     */

UINT Text_Rotation_Window_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID text_window_draw(GX_WINDOW *widget);
UINT ShapesScreen_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID graphics_draw(GX_WINDOW *widget);

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

UINT gx_studio_checkbox_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_radio_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_slider_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
GX_WIDGET *gx_studio_widget_create(GX_BYTE *storage, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget);
UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root);

VOID memory_free(VOID *mem);
UINT string_length_get(GX_CONST GX_CHAR* input_string, UINT max_string_length);

/* Determine if a C++ compiler is being used.  If so, complete the standard
  C conditional started above.                                                 */
#ifdef __cplusplus
}
#endif

#endif                                       /* sentry                         */
