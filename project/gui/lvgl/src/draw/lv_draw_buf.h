/**
 * @file lv_draw_buf.h
 *
 */

#ifndef LV_DRAW_BUF_H
#define LV_DRAW_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area.h"
#include "../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void * (*lv_draw_buf_malloc_cb)(size_t size, lv_color_format_t color_format);

typedef void (*lv_draw_buf_free_cb)(void * draw_buf);

typedef void * (*lv_draw_buf_align_cb)(void * buf, lv_color_format_t color_format);

typedef void (*lv_draw_buf_invalidate_cache_cb)(void * buf, uint32_t stride, lv_color_format_t color_format,
                                                const lv_area_t * area);

typedef uint32_t (*lv_draw_buf_width_to_stride_cb)(uint32_t w, lv_color_format_t color_format);

typedef void * (*lv_draw_buf_go_to_xy_cb)(const void * buf, uint32_t stride, lv_color_format_t color_format,
                                          lv_coord_t x, lv_coord_t y);

typedef void (*lv_draw_buf_clear_cb)(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * a);

typedef void (*lv_draw_buf_copy_cb)(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                                    void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format);

typedef struct {
    lv_draw_buf_malloc_cb buf_malloc_cb;
    lv_draw_buf_free_cb buf_free_cb;
    lv_draw_buf_align_cb align_pointer_cb;
    lv_draw_buf_invalidate_cache_cb invalidate_cache_cb;
    lv_draw_buf_width_to_stride_cb width_to_stride_cb;
    lv_draw_buf_go_to_xy_cb go_to_xy_cb;
    lv_draw_buf_clear_cb buf_clear_cb;
    lv_draw_buf_copy_cb buf_copy_cb;
} lv_draw_buf_handlers_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called internally to initialize the draw_buf_handlers in lv_global
 */
void _lv_draw_buf_init_handlers(void);

/**
 * Get the struct which holds the callbacks for draw buf management.
 * Custom callback can be set on the returned value
 * @return                  pointer to the struct of handlers
 */
lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void);

/**
 * Allocate a buffer with the given size. It might allocate slightly larger buffer to fulfill the alignment requirements.
 * @param size          the size to allocate in bytes
 * @param color_format  the color format of the buffer to allcoate
 * @return              the allocated buffer.
 * @note The returned value can be saved in draw_buf->buf
 * @note lv_draw_buf_align can be sued the align the returned pointer
 */
void * lv_draw_buf_malloc(size_t size_bytes, lv_color_format_t color_format);

/**
 * Free a buffer allocated by lv_draw_buf_malloc
 * @param buf      pointer to a buffer
 */
void lv_draw_buf_free(void  * buf);

/**
 * Align the address of a buffer. The buffer needs to be large enough for the real data after alignement
 * @param buf           the data to align
 * @param color_format  the color format of the buffer
 * @return              the aligned buffer
 */
void * lv_draw_buf_align(void * buf, lv_color_format_t color_format);

/**
 * Invalidate the cache of the buffer
 * @param buf          a memory address to invalidate
 * @param stride       stride of the buffer
 * @param color_format color format of the buffer
 * @param area         the area to invalidate in the buffer
 */
void lv_draw_buf_invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area);

/**
 * Calculate the stride in bytes based on a width and color format
 * @param w                 the width in pixels
 * @param color_format      the color format
 * @return                  the stride in bytes
 */
uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format);

/**
 * Got to a pixel at X and Y coordinate in a buffer
 * @param buf               pointer to a buffer
 * @param stride            stride of the buffer
 * @param color_format      color format of the buffer
 * @param x                 the target X coordinate
 * @param y                 the target X coordinate
 * @return                  `buf` offset to point to the given X and Y coordinate
 */
void * lv_draw_buf_go_to_xy(const void * buf, uint32_t stride, lv_color_format_t color_format, lv_coord_t x,
                            lv_coord_t y);

/**
 * Clear an area on the buffer
 * @param draw_buf          pointer to draw buffer
 * @param stride            stride of the buffer
 * @param color_format      color format of the buffer
 * @param a                 the area to clear, or NULL to clear the whole buffer
 */
void lv_draw_buf_clear(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * a);

/**
 * Copy an area from a buffer to an other
 * @param dest_buf          pointer to the destination buffer (not draw_buf)
 * @param dest_stride       the stride of the destination buffer in bytes
 * @param dest_area         pointer to the destination area
 * @param src_buf           pointer to the source buffer (not draw_buf)
 * @param src_stride        the stride of the source buffer in bytes
 * @param src_area          pointer to the source area
 * @param color_format      the color format (should be the same for the source and destination)
 */
void lv_draw_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                      void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_H*/
