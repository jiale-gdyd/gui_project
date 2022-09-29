#include "lv_draw.h"
#include "sw/lv_draw_sw.h"

void lv_draw_init(void)
{

}

void lv_draw_wait_for_finish(lv_draw_ctx_t *draw_ctx)
{
    if (draw_ctx->wait_for_finish) {
        draw_ctx->wait_for_finish(draw_ctx);
    }
}
