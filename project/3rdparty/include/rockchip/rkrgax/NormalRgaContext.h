#ifndef ROCKCHIP_RKRGAX_NORMALRGA_CONTEXT_H
#define ROCKCHIP_RKRGAX_NORMALRGA_CONTEXT_H

#include "rga.h"

#define __DEBUG         0
#define ALOGE(...)      { printf(__VA_ARGS__); printf("\n"); }

struct rgaContext {
    int                      rgaFd;
    int                      mLogAlways;
    int                      mLogOnce;
    float                    mVersion;
    int                      Is_debug;
    struct rga_hw_versions_t mHwVersions;
    struct rga_version_t     mDriverVersion;
};

#endif
