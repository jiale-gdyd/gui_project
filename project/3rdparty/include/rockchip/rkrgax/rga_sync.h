#ifndef ROCKCHIP_RKRGAX_RGA_SYNC_H
#define ROCKCHIP_RKRGAX_RGA_SYNC_H

#include <stdint.h>

int rga_sync_wait(int fd, int timeout);
int32_t rga_sync_merge(const char* name, int32_t fd1, int32_t fd2);

#endif
