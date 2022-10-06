#ifndef _GUIX_SIMPLE_SPECIFICATIONS_H_
#define _GUIX_SIMPLE_SPECIFICATIONS_H_

#include <gx_api.h>

#ifdef __cplusplus
extern "C" {
#endif

// 定义动画id
#define GX_NEXT_ANIMATION_ID        1

// 定义用户事件id
#define GX_NEXT_USER_EVENT_ID       GX_FIRST_USER_EVENT

// 为每个使用的小部件类型声明属性结构
typedef struct GX_STUDIO_WIDGET_STRUCT {
   GX_CHAR                                 *widget_name;
   USHORT                                  widget_type;
   USHORT                                  widget_id;
#if defined(GX_WIDGET_USER_DATA)
   INT                                     user_data;
#endif
   ULONG                                   style;
   ULONG                                   status;
   ULONG                                   control_block_size;
   GX_RESOURCE_ID                          normal_fill_color_id;
   GX_RESOURCE_ID                          selected_fill_color_id;
   GX_RESOURCE_ID                          disabled_fill_color_id;

   UINT (*create_function)(GX_CONST struct GX_STUDIO_WIDGET_STRUCT *, GX_WIDGET *, GX_WIDGET *);
   void (*draw_function)(GX_WIDGET *);
   UINT (*event_function)(GX_WIDGET *, GX_EVENT *);

   GX_RECTANGLE                            size;
   GX_CONST struct GX_STUDIO_WIDGET_STRUCT *next_widget;
   GX_CONST struct GX_STUDIO_WIDGET_STRUCT *child_widget;
   ULONG                                   control_block_offset;
   GX_CONST void                           *properties;
} GX_STUDIO_WIDGET;

typedef struct {
    GX_CONST GX_STUDIO_WIDGET *widget_information;
    GX_WIDGET                 *widget;
} GX_STUDIO_WIDGET_ENTRY;

typedef struct {
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
    GX_RESOURCE_ID disabled_pixelmap_id;
} GX_PIXELMAP_BUTTON_PROPERTIES;

typedef struct {
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
} GX_ICON_PROPERTIES;

typedef struct {
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
} GX_PROMPT_PROPERTIES;

typedef struct {
    GX_RESOURCE_ID wallpaper_id;
} GX_WINDOW_PROPERTIES;

// 声明顶级控制块
typedef struct SIMPLE_WINDOW_CONTROL_BLOCK_STRUCT {
    GX_WINDOW_MEMBERS_DECLARE
    GX_ICON            simple_window_prompt_border;
    GX_PROMPT          simple_window_hello;
    GX_ICON            simple_window_express_logo;
    GX_PIXELMAP_BUTTON simple_window_simple_button;
    GX_PROMPT          simple_window_button_text;
} SIMPLE_WINDOW_CONTROL_BLOCK;

#ifndef GUIX_STUDIO_GENERATED_FILE
extern SIMPLE_WINDOW_CONTROL_BLOCK simple_window;
#endif

typedef struct GX_STUDIO_DISPLAY_INFO_STRUCT {
    GX_CONST GX_CHAR   *name;
    GX_CONST GX_CHAR   *canvas_name;
    GX_CONST GX_THEME  **theme_table;
    GX_CONST GX_STRING **language_table;
    USHORT             theme_table_size;
    USHORT             language_table_size;
    UINT               string_table_size;
    UINT               x_resolution;
    UINT               y_resolution;
    GX_DISPLAY         *display;
    GX_CANVAS          *canvas;
    GX_WINDOW_ROOT     *root_window;
    GX_COLOR           *canvas_memory;
    ULONG              canvas_memory_size;
    USHORT             rotation_angle;
} GX_STUDIO_DISPLAY_INFO;

UINT gx_studio_icon_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_prompt_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_window_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);
UINT gx_studio_pixelmap_button_create(GX_CONST GX_STUDIO_WIDGET *info, GX_WIDGET *control_block, GX_WIDGET *parent);

UINT gx_studio_named_widget_create(char *name, GX_WIDGET *parent, GX_WIDGET **new_widget);
GX_WIDGET *gx_studio_widget_create(GX_BYTE *storage, GX_CONST GX_STUDIO_WIDGET *definition, GX_WIDGET *parent);

UINT gx_studio_display_configure(USHORT display, UINT (*driver)(GX_DISPLAY *), GX_UBYTE language, USHORT theme, GX_WINDOW_ROOT **return_root);

#ifdef __cplusplus
}
#endif

#endif
