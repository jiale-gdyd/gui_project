#ifndef LIBDRM_XF86DRMRANDOM_H
#define LIBDRM_XF86DRMRANDOM_H

typedef struct RandomState {
    unsigned long magic;
    unsigned long a;
    unsigned long m;
    unsigned long q;
    unsigned long r;
    unsigned long check;
    unsigned long seed;
} RandomState;

#endif
