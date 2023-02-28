#include <stdio.h>
#include <stdlib.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmRandom.h>
#include <libdrm/libdrm_macros.h>

#define RANDOM_MAGIC                0xfeedbeef

drm_public void *drmRandomCreate(unsigned long seed)
{
    RandomState *state;

    state = (RandomState *)drmMalloc(sizeof(*state));
    if (!state) {
        return NULL;
    }

    state->magic = RANDOM_MAGIC;
    state->a = 48271;
    state->m = 2147483647;
    state->check = 399268537;
    state->q = state->m / state->a;
    state->r = state->m % state->a;
    state->seed = seed;

    if (state->seed <= 0) {
        state->seed = 1;
    }

    if (state->seed >= state->m) {
        state->seed = state->m - 1;
    }

    return state;
}

drm_public int drmRandomDestroy(void *state)
{
    drmFree(state);
    return 0;
}

drm_public unsigned long drmRandom(void *state)
{
    unsigned long hi;
    unsigned long lo;
    RandomState *s = (RandomState *)state;

    hi = s->seed / s->q;
    lo = s->seed % s->q;
    s->seed = s->a * lo - s->r * hi;
    if ((s->a * lo) <= (s->r * hi)) {
        s->seed += s->m;
    }

    return s->seed;
}

drm_public double drmRandomDouble(void *state)
{
    RandomState *s = (RandomState *)state;
    
    return (double)drmRandom(state) / (double)s->m;
}
