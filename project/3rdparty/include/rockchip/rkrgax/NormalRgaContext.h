#ifndef RKRGA_NORMALRGACONTEXT_H
#define RKRGA_NORMALRGACONTEXT_H

#include "rga_ioctl.h"

typedef enum {
    RGA_DRIVER_IOC_UNKONW  = 0,
    RGA_DRIVER_IOC_RGA2,
    RGA_DRIVER_IOC_MULTI_RGA,
    RGA_DRIVER_IOC_DEFAULT = RGA_DRIVER_IOC_MULTI_RGA,
} RGA_DRIVER_IOC_TYPE;

struct rgaContext {
    int                      rgaFd;
    int                      mLogAlways;
    int                      mLogOnce;
    float                    mVersion;
    int                      Is_debug;
    struct rga_hw_versions_t mHwVersions;
    struct rga_version_t     mDriverVersion;
    RGA_DRIVER_IOC_TYPE      driver = RGA_DRIVER_IOC_DEFAULT;
};

#endif
