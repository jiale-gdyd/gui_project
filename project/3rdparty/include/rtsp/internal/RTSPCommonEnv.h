#ifndef RTSP_INTERNAL_RTSP_COMMON_ENV_H
#define RTSP_INTERNAL_RTSP_COMMON_ENV_H

#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

int _vscprintf(const char *format, va_list pargs);

#if defined(__cplusplus)
}
#endif

namespace rtsp {
#define DPRINTF                     RTSPCommonEnv::DebugPrint
#define DPRINTF0                    RTSPCommonEnv::DebugPrint

#define DEBUG_FLAG_RTSP             (0x01)
#define DEBUG_FLAG_RTP              (0x02)
#define DEBUG_FLAG_RTP_PAYLOAD      (0x04)
#define DEBUG_FLAG_ALL              (0xFF)

#define DELETE_OBJECT(obj)          if (obj) { delete obj; obj = NULL; }
#define DELETE_ARRAY(arr)           if (arr) { delete[] arr; arr = NULL; }

class RTSPCommonEnv {
public:
    static int            nDebugFlag;
    static int            nDebugPrint;

    static unsigned short nClientPortRangeMin;
    static unsigned short nClientPortRangeMax;

    static unsigned short nServerPortRangeMin;
    static unsigned short nServerPortRangeMax;

    static void DebugPrint(char *lpszFormat, ...);
    static void SetDebugFlag(int flag);
    static void UnsetDebugFlag(int flag);
};
}

#endif
