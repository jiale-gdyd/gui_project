#ifndef RKRGA_IM2D_SYNC_H
#define RKRGA_IM2D_SYNC_H

#include <stdint.h>

int rga_sync_wait(int fd, int timeout);
int32_t rga_sync_merge(const char *name, int32_t fd1, int32_t fd2);

#endif
