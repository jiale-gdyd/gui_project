#include <stdio.h>
#include <stdlib.h>

#ifndef _NET_COMMON_H
#include <live555/groupsock/NetCommon.h>
#endif

#define initializeWinsockIfNecessary()  1

#ifndef NULL
#define NULL                            0
#endif

long our_random()
{
    return random();
}

void our_srandom(unsigned x)
{
    srandom(x);
}

u_int32_t our_random32()
{
    long random_1 = our_random();
    u_int32_t random16_1 = (u_int32_t)(random_1&0x00FFFF00);

    long random_2 = our_random();
    u_int32_t random16_2 = (u_int32_t)(random_2&0x00FFFF00);

    return (random16_1<<8) | (random16_2>>8);
}

#ifdef USE_OUR_BZERO
#ifndef __bzero
void __bzero (char *to, int count)

{
    while (count-- > 0) {
        *to++ = 0;
    }
}
#endif
#endif
