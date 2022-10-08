#ifndef _GUIX_CALCULATOR_SPECIFICATIONS_H_
#define _GUIX_CALCULATOR_SPECIFICATIONS_H_

#include <gx_api.h>

/* Determine if C++ compiler is being used, if so use standard C.              */
#ifdef __cplusplus
extern   "C" {
#endif

/* Define widget ids                                                           */

#define ID_CALCULATOR_RESULT 1
#define ID_CALCULATOR_SEVEN 2
#define ID_CALCULATOR_EIGHT 3
#define ID_CALCULATOR_NINE 4
#define ID_CALCULATOR_DIVIDE 5
#define ID_CALCULATOR_MULTI 6
#define ID_CALCULATOR_MINUS 7
#define ID_CALCULATOR_PLUS 8
#define ID_CALCULATOR_PERCENT 9
#define ID_CALCULATOR_DOT 10
#define ID_CALCULATOR_FIVE 11
#define ID_CALCULATOR_TWO 12
#define ID_CALCULATOR_SIX 13
#define ID_CALCULATOR_THREE 14
#define ID_CALCULATOR_ZERO 15
#define ID_CALCULATOR_ONE 16
#define ID_CALCULATOR_FOUR 17
#define ID_CALCULATOR_C 18
#define ID_CALCULATOR_MC 19
#define ID_CALCULATOR_MR 20
#define ID_CALCULATOR_DEL 21


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
    GX_RESOURCE_ID wallpaper_id;
} GX_WINDOW_PROPERTIES;

typedef struct
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID readonly_fill_color_id;
    GX_RESOURCE_ID readonly_text_color_id;
    GX_CHAR *buffer;
    UINT buffer_size;
} GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES;


/* Declare top-level control blocks                                            */

typedef struct FRAME_CONTROL_BLOCK_STRUCT
{
    GX_WINDOW_MEMBERS_DECLARE
    GX_ICON frame_icon_1;
    GX_WINDOW frame_calculator;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_result;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_19;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_10;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_11;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_divide;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_multi;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_minus;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_plus;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_percent;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_dot;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_12;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_13;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_14;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_15;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_16;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_17;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_18;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_c;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_mc;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_mr;
    GX_PIXELMAP_BUTTON frame_pixelmap_button_del;
    GX_WINDOW frame_text_bg_win_2;
    GX_SINGLE_LINE_TEXT_INPUT frame_display_result;
} FRAME_CONTROL_BLOCK;


/* extern statically defined control blocks                                    */

#ifndef GUIX_STUDIO_GENERATED_FILE
extern FRAME_CONTROL_BLOCK frame;
#endif

/* Declare event process functions, draw functions, and callback functions     */

UINT calculator_event_handler(GX_WINDOW *widget, GX_EVENT *event_ptr);
VOID custom_pixelmap_button_draw(GX_PIXELMAP_BUTTON *widget);

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
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_text_input_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
GX_WIDGET *gx_studio_widget_create(GX_BYTE *storage, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);
UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget);
UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root);

/* Determine if a C++ compiler is being used.  If so, complete the standard
  C conditional started above.                                                 */
#ifdef __cplusplus
}
#endif

#endif                                       /* sentry                         */
