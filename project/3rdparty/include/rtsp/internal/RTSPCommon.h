#ifndef RTSP_INTERNAL_RTSP_COMMON_H
#define RTSP_INTERNAL_RTSP_COMMON_H

#include <stdint.h>
#include <inttypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

namespace rtsp {
#pragma pack(push, 1)
typedef struct {
    uint16_t cc : 4;
    uint16_t ext : 1;
    uint16_t pad : 1;
    uint16_t ver : 2;
    uint16_t pt : 7;
    uint16_t mk : 1;
    uint16_t seq;
    uint32_t ts;
    uint32_t ssrc;
} RTP_HEADER;

typedef struct {
    uint16_t rc : 5;
    uint16_t pad : 1;
    uint16_t ver : 2;
    uint16_t pt : 8;
    uint16_t length;
} RTCP_HEADER;

typedef struct {
    uint16_t profile;
    uint16_t length;
    int64_t  timestamp;
} EXT_HEADER;
#pragma pack(pop)

enum STREAM_TYPE {
    STREAM_TYPE_UDP = 0,
    STREAM_TYPE_TCP = 1,
    STREAM_TYPE_MULTICAST = 2
};

bool seqNumLT(uint16_t s1, uint16_t s2);

#define RTSP_PARAM_STRING_MAX       200

bool parseRTSPRequestString(char const *reqStr, unsigned reqStrSize,
    char *resultCmdName,
    unsigned resultCmdNameMaxSize,
    char *resultURLPreSuffix,
    unsigned resultURLPreSuffixMaxSize,
    char *resultURLSuffix,
    unsigned resultURLSuffixMaxSize,
    char *resultCSeq,
    unsigned resultCSeqMaxSize,
    char *resultSessionId,
    unsigned resultSessionIdMaxSize,
    unsigned &contentLength);

bool parseRangeParam(char const *paramStr, double &rangeStart, double &rangeEnd, char* &absStartTime, char *&absEndTime, bool &startTimeIsNow);
bool parseRangeHeader(char const *buf, double &rangeStart, double &rangeEnd, char* &absStartTime, char *&absEndTime, bool &startTimeIsNow);
bool parseScaleHeader(char const *buf, float &scale);

bool parseRTSPURL(char const *url, unsigned &address, unsigned short &port, char const **urlSuffix);
bool parseRTSPURLUsernamePassword(char const *url, char *&username, char *&password);

char const *dateHeader();
int trimStartCode(uint8_t *buf, int len);

char *getLine(char *startOfLine);
int checkEndian();

#define htonll(x) \
    ((((x)  &0xff00000000000000LL) >> 56) | \
    (((x)  &0x00ff000000000000LL) >> 40) | \
    (((x)  &0x0000ff0000000000LL) >> 24) | \
    (((x)  &0x000000ff00000000LL) >> 8) | \
    (((x)  &0x00000000ff000000LL) << 8) | \
    (((x)  &0x0000000000ff0000LL) << 24) | \
    (((x)  &0x000000000000ff00LL) << 40) | \
    (((x)  &0x00000000000000ffLL) << 56))

#define ntohll(x) \
    ((((x)  &0x00000000000000FF) << 56) | \
    (((x)  &0x000000000000FF00) << 40) | \
    (((x)  &0x0000000000FF0000) << 24) | \
    (((x)  &0x00000000FF000000) << 8)  | \
    (((x)  &0x000000FF00000000) >> 8)  | \
    (((x)  &0x0000FF0000000000) >> 24) | \
    (((x)  &0x00FF000000000000) >> 40) | \
    (((x)  &0xFF00000000000000) >> 56))

char *createSDPString(char *mediaType, unsigned char payloadType, char *codec, unsigned frequency, char *trackId);
}

#if defined(__cplusplus)
}
#endif

#endif
