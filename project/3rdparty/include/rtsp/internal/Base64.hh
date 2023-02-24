#ifndef RTSP_INTERNAL_BASE64_HH
#define RTSP_INTERNAL_BASE64_HH

namespace rtsp {
unsigned char *base64Decode(char *in, unsigned &resultSize, bool trimTrailingZeros = true);
char *base64Encode(char const *orig, unsigned origLength);
}

#endif
