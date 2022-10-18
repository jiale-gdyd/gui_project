#ifndef RKRGA_NORMALRGACONTEXT_H
#define RKRGA_NORMALRGACONTEXT_H

struct rgaContext {
    int   rgaFd;
    int   mLogAlways;
    int   mLogOnce;
    float mVersion;
    int   Is_debug;
    char  mVersion_str[16];
    char  reserved[128];
};

#endif
