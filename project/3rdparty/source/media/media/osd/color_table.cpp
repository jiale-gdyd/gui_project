#include <stdlib.h>
#include "color_table.h"

int color_tbl_argb_to_avuy(const uint32_t *pu32RgbaTbl, uint32_t *pu32AvuyTbl)
{
    unsigned char r, g, b, a;
    unsigned char y, u, v, alpha;

    for (int i = 0; i < PALETTE_TABLE_LEN; i++) {
        a = (pu32RgbaTbl[i] & 0xFF000000) >> 24;
        r = (pu32RgbaTbl[i] & 0x00FF0000) >> 16;
        g = (pu32RgbaTbl[i] & 0x0000FF00) >> 8;
        b = (pu32RgbaTbl[i] & 0x000000FF);

        y = 16 + 0.257 * r + 0.504 * g + 0.098 * b;
        u = 128 - 0.148 * r - 0.291 * g + 0.439 * b;
        v = 128 + 0.439 * r - 0.368 * g - 0.071 * b;
        alpha = a;

        pu32AvuyTbl[i] = (alpha << 24) | (v << 16) | (u << 8) | y;
    }

    return 0;
}

uint8_t find_argb_color_tbl_by_order(const uint32_t *pal, uint32_t len, uint32_t u32ArgbColor)
{
    uint32_t i = 0;
    uint8_t pixel = 0;
    uint8_t a, r, g, b;
    uint32_t smallest = 0;
    uint32_t distance = 0;
    uint8_t ap, rp, gp, bp;

    a = (u32ArgbColor & 0xFF000000) >> 24;
    r = (u32ArgbColor & 0x00FF0000) >> 16;
    g = (u32ArgbColor & 0x0000FF00) >> 8;
    b = (u32ArgbColor & 0x000000FF);

    smallest = ~0;
    for (i = 0; i < len; ++i) {
        ap = (pal[i] & 0xFF000000) >> 24;
        rp = (pal[i] & 0x00FF0000) >> 16;
        gp = (pal[i] & 0x0000FF00) >> 8;
        bp = (pal[i] & 0x000000FF);

        distance = abs(ap - a) + abs(rp - r) + abs(gp - g) + abs(bp - b);
        if (distance < smallest) {
            pixel = i;

            if (distance == 0) {
                break;
            }

            smallest = distance;
        }
    }

    return pixel;
}

uint8_t find_argb_color_tbl_by_dichotomy(const uint32_t *pal, uint32_t len, uint32_t u32ArgbColor)
{
    int start, mid, end;

    start = 0;
    end = len;
    mid = 0;

    while (start < end) {
        mid = (start + end) / 2;
        if (u32ArgbColor > pal[mid]) {
            start = mid;
        } else {
            end = mid;
        }

        if (end - start <= 1) {
            int delta0 = u32ArgbColor - pal[start];
            int delta1 = pal[end] - u32ArgbColor;
    
            if (abs(delta0) > abs(delta1)) {
                mid = end;
            } else {
                mid = start;
            }
    
            break;
        }
    }

    return mid;
}
