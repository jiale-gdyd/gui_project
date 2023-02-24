#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtsp/internal/RTSPCommonEnv.h>

int _vscprintf(const char *format, va_list pargs)
{ 
    int retval; 
    va_list argcopy;

    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy); 
    va_end(argcopy); 

    return retval;
}

namespace rtsp {
int RTSPCommonEnv::nDebugPrint = 0;
int RTSPCommonEnv::nDebugFlag = DEBUG_FLAG_RTSP;

unsigned short RTSPCommonEnv::nClientPortRangeMin = 10000;
unsigned short RTSPCommonEnv::nClientPortRangeMax = 65535;

unsigned short RTSPCommonEnv::nServerPortRangeMin = 20000;
unsigned short RTSPCommonEnv::nServerPortRangeMax = 65535;

void RTSPCommonEnv::DebugPrint(char *lpszFormat, ...)
{
    int len;
    char *buffer;
    va_list args;

    va_start(args, lpszFormat);

    len = _vscprintf(lpszFormat, args) + 32;
    buffer = (char *)malloc(len * sizeof(char));

    const char *prefix = "[RTSP] ";

    int prefix_len = strlen(prefix);
    memcpy(buffer, prefix, prefix_len);

    vsprintf(&buffer[prefix_len], lpszFormat, args);
    fprintf(stdout, buffer);

    free(buffer);
}

void RTSPCommonEnv::SetDebugFlag(int flag)
{
    nDebugFlag |= flag;
}

void RTSPCommonEnv::UnsetDebugFlag(int flag)
{
    int x = ~flag;
    nDebugFlag &= x;
}
}
