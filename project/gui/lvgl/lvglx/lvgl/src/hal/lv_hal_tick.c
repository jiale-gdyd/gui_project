#include "lv_hal_tick.h"
#include "../misc/lv_types.h"

#if LV_USE_ATOMICS == 1
#include <stdatomic.h>
#endif

#include <stddef.h>

#if (LV_TICK_CUSTOM == 1)
#include LV_TICK_CUSTOM_INCLUDE
#endif

#if !LV_TICK_CUSTOM
#if LV_USE_ATOMICS == 1
static _Atomic(uint32_t) sys_time = 0;
static atomic_int tick_irq_flag;
#else
static volatile uint32_t sys_time = 0;
static volatile int tick_irq_flag;
#endif
#endif

#if !LV_TICK_CUSTOM
LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period)
{
    tick_irq_flag = 0;
    sys_time += tick_period;
}
#endif

uint32_t lv_tick_get(void)
{
#if LV_TICK_CUSTOM == 0

    // 如果在读取`sys_time`时从中断中调用`lv_tick_inc`，结果可能会被破坏。这个循环检测在读取`sys_time`时是否调用了`lv_tick_inc`。如果在`lv_tick_inc`中清除了`tick_irq_flag`，请尝试再次读取，直到`tick_irq_flag`保持为 `1`
    uint32_t result;
    do {
        tick_irq_flag = 1;
        result = sys_time;
    } while (!tick_irq_flag);   // 继续直到看到一个非中断的循环

    return result;
#else
    return LV_TICK_CUSTOM_SYS_TIME_EXPR;
#endif
}

uint32_t lv_tick_elaps(uint32_t prev_tick)
{
    uint32_t act_time = lv_tick_get();

    // 如果在sys_time简单减法中没有溢出
    if (act_time >= prev_tick) {
        prev_tick = act_time - prev_tick;
    } else {
        prev_tick = UINT32_MAX - prev_tick + 1;
        prev_tick += act_time;
    }

    return prev_tick;
}
