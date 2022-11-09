#ifndef LV_FONT_H
#define LV_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../lv_conf_internal.h"
#include "lv_symbol_def.h"
#include "../misc/lv_area.h"

// imgfont识别
#define LV_IMGFONT_BPP      9

struct _lv_font_t;

// 描述字形的属性
typedef struct {
    const struct _lv_font_t *resolved_font;     // 指向在处理回退后实际找到字形的字体的指针
    uint16_t                adv_w;              // 字形需要这个空间。在此宽度之后绘制下一个字形
    uint16_t                box_w;              // 字形边界框的宽度
    uint16_t                box_h;              // 字形边界框的高度
    int16_t                 ofs_x;              // 边界框的x偏移
    int16_t                 ofs_y;              // y边界框的偏移量
    uint8_t                 bpp : 4;            // 每像素位: 1、2、4、8
    uint8_t                 is_placeholder : 1; // 字形丢失。但占位符仍会显示
} lv_font_glyph_dsc_t;

// 位图可能会被放大3以实现亚像素渲染
enum {
    LV_FONT_SUBPX_NONE,
    LV_FONT_SUBPX_HOR,
    LV_FONT_SUBPX_VER,
    LV_FONT_SUBPX_BOTH,
};

typedef uint8_t lv_font_subpx_t;

// 描述字体的属性
typedef struct _lv_font_t {
    // 从字体中获取字形的描述符
    bool (*get_glyph_dsc)(const struct _lv_font_t *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    // 从字体中获取字形的位图
    const uint8_t *(*get_glyph_bitmap)(const struct _lv_font_t *, uint32_t);

    // 指向字体包中字体的指针(必须具有相同的行高)
    lv_coord_t              line_height;            // 任何文本适合的实际行高
    lv_coord_t              base_line;              // 从line_height顶部测量的基线
    uint8_t                 subpx  : 2;             // `lv_font_subpx_t`的一个元素

    int8_t                  underline_position;     // 下划线顶部与基线之间的距离(<0表示低于基线)
    int8_t                  underline_thickness;    // 下划线的粗细

    const void              *dsc;                   // 在此处存储实现特定或运行时数据或缓存
    const struct _lv_font_t *fallback;              // 缺少字形的后备字体。递归解决
#if LV_USE_USER_DATA
    void                    *user_data;             // 字体的自定义用户数据
#endif
} lv_font_t;

/**
 * 函数名称: lv_font_get_glyph_bitmap
 * 功能描述: 返回一个字体的位图
 * 输入参数: font_p --> 指向字体的指针
 *           letter --> 一个UNICODE字符代码
 * 输出参数: 无
 * 返回说明: 指向字母位图的指针
 */
const uint8_t *lv_font_get_glyph_bitmap(const lv_font_t *font_p, uint32_t letter);

/**
 * 函数名称: lv_font_get_glyph_dsc
 * 功能描述: 获取字形的描述符
 * 输入参数: font_p      --> 字体指针
 *           letter      --> 一个UNICODE字母代码
 *           letter_next --> `letter`之后的下一个字母。用于字距调整
 * 输出参数: dsc_out     --> 在此处存储结果描述符
 * 返回说明: true: 描述符已成功加载到`dsc_out`中。false: 未找到该信件，没有数据加载到`dsc_out`
 */
bool lv_font_get_glyph_dsc(const lv_font_t *font_p, lv_font_glyph_dsc_t *dsc_out, uint32_t letter, uint32_t letter_next);

/**
 * 函数名称: lv_font_get_glyph_width
 * 功能描述: 使用字距调整字形的宽度
 * 输入参数: font        --> 指向字体的指针
 *           letter      --> 一个UNICODE字母
 *           letter_next --> `letter`之后的下一个字母。用于字距调整
 * 输出参数: 无
 * 返回说明: 无
 */
uint16_t lv_font_get_glyph_width(const lv_font_t *font, uint32_t letter, uint32_t letter_next);

/**
 * 函数名称: lv_font_get_line_height
 * 功能描述: 获取字体的行高。 所有字符都适合这个高度
 * 输入参数: font_p --> 指向字体的指针
 * 输出参数: 无
 * 返回说明: 字体的高度
 */
static inline lv_coord_t lv_font_get_line_height(const lv_font_t *font_p)
{
    return font_p->line_height;
}

#define LV_FONT_DECLARE(font_name) extern const lv_font_t font_name;

#if LV_FONT_MONTSERRAT_8
LV_FONT_DECLARE(lv_font_montserrat_8)
#endif

#if LV_FONT_MONTSERRAT_10
LV_FONT_DECLARE(lv_font_montserrat_10)
#endif

#if LV_FONT_MONTSERRAT_12
LV_FONT_DECLARE(lv_font_montserrat_12)
#endif

#if LV_FONT_MONTSERRAT_14
LV_FONT_DECLARE(lv_font_montserrat_14)
#endif

#if LV_FONT_MONTSERRAT_16
LV_FONT_DECLARE(lv_font_montserrat_16)
#endif

#if LV_FONT_MONTSERRAT_18
LV_FONT_DECLARE(lv_font_montserrat_18)
#endif

#if LV_FONT_MONTSERRAT_20
LV_FONT_DECLARE(lv_font_montserrat_20)
#endif

#if LV_FONT_MONTSERRAT_22
LV_FONT_DECLARE(lv_font_montserrat_22)
#endif

#if LV_FONT_MONTSERRAT_24
LV_FONT_DECLARE(lv_font_montserrat_24)
#endif

#if LV_FONT_MONTSERRAT_26
LV_FONT_DECLARE(lv_font_montserrat_26)
#endif

#if LV_FONT_MONTSERRAT_28
LV_FONT_DECLARE(lv_font_montserrat_28)
#endif

#if LV_FONT_MONTSERRAT_30
LV_FONT_DECLARE(lv_font_montserrat_30)
#endif

#if LV_FONT_MONTSERRAT_32
LV_FONT_DECLARE(lv_font_montserrat_32)
#endif

#if LV_FONT_MONTSERRAT_34
LV_FONT_DECLARE(lv_font_montserrat_34)
#endif

#if LV_FONT_MONTSERRAT_36
LV_FONT_DECLARE(lv_font_montserrat_36)
#endif

#if LV_FONT_MONTSERRAT_38
LV_FONT_DECLARE(lv_font_montserrat_38)
#endif

#if LV_FONT_MONTSERRAT_40
LV_FONT_DECLARE(lv_font_montserrat_40)
#endif

#if LV_FONT_MONTSERRAT_42
LV_FONT_DECLARE(lv_font_montserrat_42)
#endif

#if LV_FONT_MONTSERRAT_44
LV_FONT_DECLARE(lv_font_montserrat_44)
#endif

#if LV_FONT_MONTSERRAT_46
LV_FONT_DECLARE(lv_font_montserrat_46)
#endif

#if LV_FONT_MONTSERRAT_48
LV_FONT_DECLARE(lv_font_montserrat_48)
#endif

#if LV_FONT_MONTSERRAT_12_SUBPX
LV_FONT_DECLARE(lv_font_montserrat_12_subpx)
#endif

#if LV_FONT_MONTSERRAT_28_COMPRESSED
LV_FONT_DECLARE(lv_font_montserrat_28_compressed)
#endif

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
LV_FONT_DECLARE(lv_font_dejavu_16_persian_hebrew)
#endif

#if LV_FONT_SIMSUN_16_CJK
LV_FONT_DECLARE(lv_font_simsun_16_cjk)
#endif

#if LV_FONT_UNSCII_8
LV_FONT_DECLARE(lv_font_unscii_8)
#endif

#if LV_FONT_UNSCII_16
LV_FONT_DECLARE(lv_font_unscii_16)
#endif

#if LV_FONT_MICROSOFT_YAHEI
LV_FONT_DECLARE(lv_font_microsoft_yahei)
#endif

// 声明自定义(用户定义)字体
#ifdef LV_FONT_CUSTOM_DECLARE
LV_FONT_CUSTOM_DECLARE
#endif

/**
 * 函数名称: lv_font_default
 * 功能描述: 只是LV_FONT_DEFAULT的包装器，因为在某些情况下使用函数可能更方便
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 指向LV_FONT_DEFAULT的指针
 */
static inline const lv_font_t *lv_font_default(void)
{
    return LV_FONT_DEFAULT;
}

#ifdef __cplusplus
}
#endif

#endif
