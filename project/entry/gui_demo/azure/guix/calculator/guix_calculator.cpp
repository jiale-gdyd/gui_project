#include <stdio.h>
#include <gx_api.h>

#include "evaluate.h"
#include "guix_calculator.h"
#include "../display_driver.h"
#include "guix_calculator_resources.h"
#include "guix_calculator_specifications.h"

#define TYPE_OPERAND                1
#define TYPE_OPERATION              2
#define TYPE_DOT                    3
#define TYPE_FUNCTION               4

#define MAX_TEXT_LENGTH             3
#define MAX_EXPRESSION_LENGTH       99

GX_WINDOW_ROOT *root;
GX_BOOL is_decimal_point = GX_FALSE;
GX_BOOL clear_input_buffer = GX_FALSE;

typedef struct {
    GX_CHAR *text;
    INT     x_offset;
    INT     y_offset;
    USHORT  widget_id;
    USHORT  event_type;
    LONG    type;
} KEY_INFO_ENTRY;

KEY_INFO_ENTRY key_info[] = {
    { "0", 0, 0, ID_CALCULATOR_ZERO, GX_SIGNAL(ID_CALCULATOR_ZERO, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "1", 0, 0, ID_CALCULATOR_ONE, GX_SIGNAL(ID_CALCULATOR_ONE, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "2", 0, 0, ID_CALCULATOR_TWO, GX_SIGNAL(ID_CALCULATOR_TWO, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "3", 0, 0, ID_CALCULATOR_THREE, GX_SIGNAL(ID_CALCULATOR_THREE, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "4", 0, 0, ID_CALCULATOR_FOUR, GX_SIGNAL(ID_CALCULATOR_FOUR, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "5", 0, 0, ID_CALCULATOR_FIVE, GX_SIGNAL(ID_CALCULATOR_FIVE, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "6", 0, 0, ID_CALCULATOR_SIX, GX_SIGNAL(ID_CALCULATOR_SIX, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "7", 0, 0, ID_CALCULATOR_SEVEN, GX_SIGNAL(ID_CALCULATOR_SEVEN, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "8", 0, 0, ID_CALCULATOR_EIGHT, GX_SIGNAL(ID_CALCULATOR_EIGHT, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "9", 0, 0, ID_CALCULATOR_NINE, GX_SIGNAL(ID_CALCULATOR_NINE, GX_EVENT_CLICKED), TYPE_OPERAND },
    { "+", 0, -2, ID_CALCULATOR_PLUS, GX_SIGNAL(ID_CALCULATOR_PLUS, GX_EVENT_CLICKED), TYPE_OPERATION },
    { "-", 0, -2, ID_CALCULATOR_MINUS, GX_SIGNAL(ID_CALCULATOR_MINUS, GX_EVENT_CLICKED), TYPE_OPERATION },
    { "*", 0, 2, ID_CALCULATOR_MULTI, GX_SIGNAL(ID_CALCULATOR_MULTI, GX_EVENT_CLICKED), TYPE_OPERATION },
    { "/", 0, -2, ID_CALCULATOR_DIVIDE, GX_SIGNAL(ID_CALCULATOR_DIVIDE, GX_EVENT_CLICKED), TYPE_OPERATION },
    { ".", 0, 0, ID_CALCULATOR_DOT, GX_SIGNAL(ID_CALCULATOR_DOT, GX_EVENT_CLICKED), TYPE_DOT },
    { "C", 0, 0, ID_CALCULATOR_C, 0, TYPE_FUNCTION },
    { "MC", 0, 0, ID_CALCULATOR_MC, 0, TYPE_FUNCTION },
    { "MR", 0, 0, ID_CALCULATOR_MR, 0, TYPE_FUNCTION },
    { "Del", 0, 0, ID_CALCULATOR_DEL, 0, TYPE_FUNCTION },
    { "=", 0, 0, ID_CALCULATOR_RESULT, 0, TYPE_FUNCTION },
    { "%", 0, 0, ID_CALCULATOR_PERCENT, 0, TYPE_FUNCTION },
    { GX_NULL, 0, 0, 0, 0, 0 }
};

int azure_guix_calculator_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_DRM_DISP_DRIVER)
    gx_drm_graphics_driver_setup(0, 0, 0, 0, 1);
#endif

    tx_kernel_enter();
    return 0;
}

int azure_guix_calculator_demo_exit(void)
{
    return gx_drm_graphics_driver_exit();
}

// 自定义像素地图按钮绘制
VOID custom_pixelmap_button_draw(GX_PIXELMAP_BUTTON *key)
{
    INT x_offset;
    INT y_offset;
    GX_STRING text;
    GX_RESOURCE_ID font_id = GX_FONT_ID_MIDSIZE;
    KEY_INFO_ENTRY *pKeyInfo = key_info;

    gx_pixelmap_button_draw(key);

    while (pKeyInfo->text) {
        if (pKeyInfo->widget_id == key->gx_widget_id) {
            x_offset = pKeyInfo->x_offset;
            y_offset = pKeyInfo->y_offset;

            if (key->gx_widget_style & GX_STYLE_BUTTON_PUSHED) {
                x_offset++;
                y_offset++;
            }

            text.gx_string_ptr = pKeyInfo->text;
            text.gx_string_length = string_length_get(pKeyInfo->text, MAX_TEXT_LENGTH);

            gx_widget_text_draw_ext(key, GX_COLOR_ID_WHITE, font_id, &text, x_offset, y_offset);
            break;
        }

        pKeyInfo++;
    }
}

VOID tx_application_define(void *first_unused_memory)
{
    // 初始化GUIX
    gx_system_initialize();

#if defined(CONFIG_DRM_DISP_DRIVER)
    gx_studio_display_configure(MAIN_SCREEN, gx_drm_graphics_driver_setup_565rgb, LANGUAGE_ENGLISH, MAIN_SCREEN_DEFAULT_THEME, &root);
#elif defined(CONFIG_X11_DISP_DRIVER)
    gx_studio_display_configure(MAIN_SCREEN, gx_x11_graphics_driver_setup_565rgb, LANGUAGE_ENGLISH, MAIN_SCREEN_DEFAULT_THEME, &root);
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
    gx_studio_display_configure(MAIN_SCREEN, gx_fbdev_graphics_driver_setup_565rgb, LANGUAGE_ENGLISH, MAIN_SCREEN_DEFAULT_THEME, &root);
#else
    return;
#endif

    // 创建计算机屏幕
    gx_studio_named_widget_create("frame", (GX_WIDGET *)root, GX_NULL);

    // 显示根窗口，使它和患者屏幕可见
    gx_widget_show(root);

    // 开始GUIX线程
    gx_system_start();
}

UINT calculator_event_handler(GX_WINDOW *window, GX_EVENT *event_ptr)
{
    INT string_size;
    GX_STRING string;
    GX_CHAR pre_char = '\0';
    KEY_INFO_ENTRY *pKeyInfo = key_info;
    GX_SINGLE_LINE_TEXT_INPUT *text_input;

    text_input = &frame.frame_display_result;

    if ((event_ptr->gx_event_type & GX_SIGNAL_EVENT_MASK) == GX_EVENT_CLICKED) {
        while (pKeyInfo->text) {
            if (pKeyInfo->event_type == event_ptr->gx_event_type) {
                if (clear_input_buffer) {
                    if ((pKeyInfo->text[0] >= '0') && (pKeyInfo->text[0] <= '9')) {
                        // 清除输入缓冲区
                        gx_single_line_text_input_buffer_clear(text_input);
                    } else if (!strcmp(text_input->gx_single_line_text_input_buffer, "Error Format")) {
                        return 0;
                    }

                    clear_input_buffer = GX_FALSE;
                }

                string_size = text_input->gx_single_line_text_input_string_size;
                if (string_size) {
                    // 获取前一个字符
                    pre_char = text_input->gx_single_line_text_input_buffer[string_size - 1];
                }

                if (pKeyInfo->type == TYPE_OPERAND) {
                    if ((pre_char == '0') && (string_size == 1)) {
                        gx_single_line_text_input_backspace(text_input);
                    } else if ((pre_char == '/') && (pKeyInfo->text[0] == '0')) {
                        return 0;
                    }

                    gx_single_line_text_input_character_insert(text_input, (GX_UBYTE *)&pKeyInfo->text[0], 1);
                } else if (string_size) {
                    if ((pre_char >= '0') && (pre_char <= '9')) {
                        if (pKeyInfo->type == TYPE_OPERATION) {
                            gx_single_line_text_input_character_insert(text_input, (GX_UBYTE *)&pKeyInfo->text[0], 1);
                            is_decimal_point = GX_FALSE;
                        } else if ((pKeyInfo->type == TYPE_DOT) && !is_decimal_point) {
                            gx_single_line_text_input_character_insert(text_input, (GX_UBYTE *)&pKeyInfo->text[0], 1);
                            is_decimal_point = GX_TRUE;
                        }
                    }
                }

                return 0;
            }

            pKeyInfo++;
        }
    }

    switch (event_ptr->gx_event_type)  {
        case GX_SIGNAL(ID_CALCULATOR_DEL, GX_EVENT_CLICKED):
            gx_single_line_text_input_backspace(text_input);
            break;

        case GX_SIGNAL(ID_CALCULATOR_C, GX_EVENT_CLICKED):
            // 清除输入缓存
            gx_single_line_text_input_buffer_clear(text_input);
            break;

        case GX_SIGNAL(ID_CALCULATOR_RESULT, GX_EVENT_CLICKED):
            if (string_length_get(text_input->gx_single_line_text_input_buffer, MAX_EXPRESSION_LENGTH) > 0) {
                // 评估表达式
                string.gx_string_ptr = text_input->gx_single_line_text_input_buffer;
                string.gx_string_length = string_length_get(string.gx_string_ptr, text_input->gx_single_line_text_input_buffer_size - 1);
                evaluate(&string, GX_FALSE);

                // 清除输入缓存
                gx_single_line_text_input_buffer_clear(text_input);

                // 显示结果
                gx_single_line_text_input_character_insert(text_input, (GX_UBYTE *)result, string_length_get(result, MAX_EXPRESSION_LENGTH));
                clear_input_buffer = GX_TRUE;
            }
            break;

        case GX_SIGNAL(ID_CALCULATOR_PERCENT, GX_EVENT_CLICKED):
            if (string_length_get(text_input->gx_single_line_text_input_buffer, MAX_EXPRESSION_LENGTH) > 0) {
                string.gx_string_ptr = text_input->gx_single_line_text_input_buffer;
                string.gx_string_length = string_length_get(string.gx_string_ptr, text_input->gx_single_line_text_input_buffer_size - 1);
                evaluate(&string, GX_TRUE);

                // 清除输入缓存
                gx_single_line_text_input_buffer_clear(text_input);

                // 显示结果
                gx_single_line_text_input_character_insert(text_input, (GX_UBYTE *)result, string_length_get(result, MAX_EXPRESSION_LENGTH));

                clear_input_buffer = GX_TRUE;
            }
            break;

        default:
            return gx_window_event_process(window, event_ptr);
    }

    return 0;
}
