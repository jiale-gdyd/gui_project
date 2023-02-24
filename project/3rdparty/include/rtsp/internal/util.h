#ifndef RTSP_INTERNAL_UTIL_H
#define RTSP_INTERNAL_UTIL_H

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#if defined(__cplusplus)
extern "C" {
#endif

namespace rtsp {
char *strDup(char const *str);
char *strDupSize(char const *str);
int CheckUdpPort(unsigned short port);
}

#if defined(__cplusplus)
}
#endif

#endif
