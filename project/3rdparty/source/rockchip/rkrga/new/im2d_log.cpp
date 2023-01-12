#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>

#ifndef __cplusplus
#include <stdatomic.h>
#else
#include <atomic>
#define _Atomic(X)      std::atomic<X>
using namespace std;
#endif

#include <rockchip/rkrgax/im2d_log.h>

static int rga_log_property_get(void);
static int rga_log_level_property_get(void);

__thread char g_rga_err_str[IM_ERR_MSG_LEN] = "The current error message is empty!";

static size_t g_start_time = rga_get_current_time_ms();
static atomic_int g_log_en = ATOMIC_VAR_INIT(rga_log_property_get());
static atomic_int g_log_level = ATOMIC_VAR_INIT(rga_log_level_property_get());

int rga_error_msg_set(const char* format, ...)
{
    va_list ap;
    int ret = 0;

    va_start(ap, format);
    ret = vsnprintf(g_rga_err_str, IM_ERR_MSG_LEN, format, ap);
    va_end(ap);

    return ret;
}

static int inline rga_log_property_get(void)
{
    char *level = getenv("ROCKCHIP_RGA_LOG");
    if (level == nullptr) {
        level = (char *)"0";
    }

    return atoi(level);
}

static int inline rga_log_level_property_get(void)
{
    char *level = getenv("ROCKCHIP_RGA_LOG_LEVEL");
    if (level == nullptr) {
        level = (char *)"0";
    }

    return atoi(level);
}

int rga_log_level_init(void)
{
    return rga_log_level_get();
}

void rga_log_level_update(void)
{
    g_log_level = rga_log_level_get();
}

int rga_log_level_get(void)
{
    return g_log_level;
}

int rga_log_enable_get(void)
{
    return g_log_en;
}

size_t rga_get_current_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

size_t rga_get_start_time_ms(void)
{
    return g_start_time;
}
