#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <rtsp/internal/util.h>
#include <rtsp/internal/NetCommon.h>
#include <rtsp/internal/RTSPCommon.h>
#include <rtsp/internal/RTSPCommonEnv.h>

namespace rtsp {
#define initializeWinsockIfNecessary()      1

bool seqNumLT(uint16_t s1, uint16_t s2)
{
    int diff = s2 - s1;
    if (diff > 0) {
        return (diff < 0x8000);
    } else if (diff < 0) {
        return (diff < -0x8000);
    } else { 
        return false;
    }
}

static void decodeURL(char *url)
{
    char *cursor = url;

    while (*cursor) {
        if ((cursor[0] == '%')
            && cursor[1] && isxdigit(cursor[1])
            && cursor[2] && isxdigit(cursor[2]))
        {
            char hex[3];
            hex[0] = cursor[1];
            hex[1] = cursor[2];
            hex[2] = '\0';
            *url++ = (char)strtol(hex, NULL, 16);
            cursor += 3;
        } else {
            *url++ = *cursor++;
        }
    }

    *url = '\0';
}

bool parseRTSPRequestString(char const *reqStr,
    unsigned reqStrSize,
    char *resultCmdName,
    unsigned resultCmdNameMaxSize,
    char *resultURLPreSuffix,
    unsigned resultURLPreSuffixMaxSize,
    char *resultURLSuffix,
    unsigned resultURLSuffixMaxSize,
    char *resultCSeq,
    unsigned resultCSeqMaxSize,
    char *resultSessionIdStr,
    unsigned resultSessionIdStrMaxSize,
    unsigned &contentLength)
{
    unsigned i;

    for (i = 0; i < reqStrSize; ++i) {
        char c = reqStr[i];
        if (!((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') || (c == '\0'))) {
            break;
        }
    }

    if (i == reqStrSize) {
        return false;
    }

    unsigned i1 = 0;
    bool parseSucceeded = false;

    for (; (i1 < (resultCmdNameMaxSize - 1)) && (i < reqStrSize); ++i, ++i1) {
        char c = reqStr[i];
        if ((c == ' ') || (c == '\t')) {
            parseSucceeded = true;
            break;
        }

        resultCmdName[i1] = c;
    }

    resultCmdName[i1] = '\0';
    if (!parseSucceeded) {
        return false;
    }

    unsigned j = i + 1;
    while ((j < reqStrSize) && ((reqStr[j] == ' ') || (reqStr[j] == '\t'))) {
        ++j;
    }

    for (; (int)j < (int)(reqStrSize - 8); ++j) {
        if ((reqStr[j] == 'r' || reqStr[j] == 'R')
            && (reqStr[j+1] == 't' || reqStr[j+1] == 'T')
            && (reqStr[j+2] == 's' || reqStr[j+2] == 'S')
            && (reqStr[j+3] == 'p' || reqStr[j+3] == 'P')
            && reqStr[j+4] == ':' && reqStr[j+5] == '/') 
        {
            j += 6;
            if (reqStr[j] == '/') {
                ++j;
                while ((j < reqStrSize) && (reqStr[j] != '/') && (reqStr[j] != ' ')) {
                    ++j;
                }
            } else {
                --j;
            }

            i = j;
            break;
        }
    }

    parseSucceeded = false;
    for (unsigned k = i + 1; (int)k < (int)(reqStrSize - 5); ++k) {
        if (reqStr[k] == 'R'
        && reqStr[k+1] == 'T'
            && reqStr[k+2] == 'S'
            && reqStr[k+3] == 'P'
            && reqStr[k+4] == '/') 
        {
            while ((--k >= i) && (reqStr[k] == ' ')) {}
            unsigned k1 = k;
            while ((k1 > i) && (reqStr[k1] != '/')) {
                --k1;
            }

            unsigned n = 0, k2 = k1+1;
            if (k2 <= k) {
                if ((k - k1 + 1) > resultURLSuffixMaxSize) {
                    return false;
                }

                while (k2 <= k) {
                    resultURLSuffix[n++] = reqStr[k2++];
                }
            }
            resultURLSuffix[n] = '\0';

            n = 0; k2 = i + 1;
            if ((k2 + 1) <= k1) {
                if ((k1 - i) > resultURLPreSuffixMaxSize) {
                    return false;
                }

                while (k2 <= (k1 - 1)) {
                    resultURLPreSuffix[n++] = reqStr[k2++];
                }
            }

            resultURLPreSuffix[n] = '\0';
            decodeURL(resultURLPreSuffix);

            i = k + 7;
            parseSucceeded = true;
            break;
        }
    }

    if (!parseSucceeded) {
        return false;
    }

    parseSucceeded = false;
    for (j = i; (int)j < (int)(reqStrSize - 5); ++j) {
        if (_strcasecmp("CSeq:", &reqStr[j], 5) == 0) {
            j += 5;
            while ((j < reqStrSize) && ((reqStr[j] ==  ' ') || (reqStr[j] == '\t'))) {
                ++j;
            }

            unsigned n;
            for (n = 0; (n < (resultCSeqMaxSize - 1)) && (j < reqStrSize); ++n, ++j) {
                char c = reqStr[j];
                if ((c == '\r') || (c == '\n')) {
                    parseSucceeded = true;
                    break;
                }

                resultCSeq[n] = c;
            }

            resultCSeq[n] = '\0';
            break;
        }
    }

    if (!parseSucceeded) {
        return false;
    }

    resultSessionIdStr[0] = '\0';
    for (j = i; (int)j < (int)(reqStrSize - 8); ++j) {
        if (_strcasecmp("Session:", &reqStr[j], 8) == 0) {
            j += 8;
            while ((j < reqStrSize) && ((reqStr[j] ==  ' ') || (reqStr[j] == '\t'))) {
                ++j;
            }

            unsigned n;
            for (n = 0; (n < (resultSessionIdStrMaxSize - 1)) && (j < reqStrSize); ++n,++j) {
                char c = reqStr[j];
                if ((c == '\r') || (c == '\n')) {
                    break;
                }

                resultSessionIdStr[n] = c;
            }

            resultSessionIdStr[n] = '\0';
            break;
        }
    }

    contentLength = 0;
    for (j = i; (int)j < (int)(reqStrSize-15); ++j) {
        if (_strcasecmp("Content-Length:", &(reqStr[j]), 15) == 0) {
            j += 15;
            while ((j < reqStrSize) && ((reqStr[j] ==  ' ') || (reqStr[j] == '\t'))) {
                ++j;
            }

            unsigned num;
            if (sscanf(&reqStr[j], "%u", &num) == 1) {
                contentLength = num;
            }
        }
    }

    return true;
}

bool parseRangeParam(char const *paramStr, double &rangeStart, double &rangeEnd, char *&absStartTime, char *&absEndTime, bool &startTimeIsNow) 
{
    delete[] absStartTime;
    delete[] absEndTime;

    absStartTime = absEndTime = NULL;
    startTimeIsNow = false;

    double start, end;
    int numCharsMatched1 = 0, numCharsMatched2 = 0, numCharsMatched3 = 0, numCharsMatched4 = 0;

    if (sscanf(paramStr, "npt = %lf - %lf", &start, &end) == 2) {
        rangeStart = start;
        rangeEnd = end;
    } else if (sscanf(paramStr, "npt = %n%lf -", &numCharsMatched1, &start) == 1) {
        if (paramStr[numCharsMatched1] == '-') {
            rangeStart = 0.0; startTimeIsNow = true;
            rangeEnd = -start;
        } else {
            rangeStart = start;
            rangeEnd = 0.0;
        }
    } else if (sscanf(paramStr, "npt = now - %lf", &end) == 1) {
        rangeStart = 0.0; startTimeIsNow = true;
        rangeEnd = end;
    } else if (sscanf(paramStr, "npt = now -%n", &numCharsMatched2) == 0 && numCharsMatched2 > 0) {
        rangeStart = 0.0; startTimeIsNow = true;
        rangeEnd = 0.0;
    } else if (sscanf(paramStr, "clock = %n", &numCharsMatched3) == 0 && numCharsMatched3 > 0) {
        rangeStart = rangeEnd = 0.0;

        char const *utcTimes = &paramStr[numCharsMatched3];
        size_t len = strlen(utcTimes) + 1;
        char *as = new char[len];
        char *ae = new char[len];
        int sscanfResult = sscanf(utcTimes, "%[^-]-%s", as, ae);

        if (sscanfResult == 2) {
            absStartTime = as;
            absEndTime = ae;
        } else if (sscanfResult == 1) {
            absStartTime = as;
            delete[] ae;
        } else {
            delete[] as; delete[] ae;
            return false;
        }
    } else if (sscanf(paramStr, "smtpe = %n", &numCharsMatched4) == 0 && numCharsMatched4 > 0) {

    } else {
        return false;
    }

    return true;
}

bool parseRangeHeader(char const *buf, double &rangeStart, double &rangeEnd, char *&absStartTime, char *&absEndTime, bool &startTimeIsNow) 
{
    while (1) {
        if (*buf == '\0') {
            return false;
        }

        if (_strcasecmp(buf, "Range: ", 7) == 0) {
            break;
        }

        ++buf;
    }

    char const *fields = buf + 7;
    while (*fields == ' ') {
        ++fields;
    }

    return parseRangeParam(fields, rangeStart, rangeEnd, absStartTime, absEndTime, startTimeIsNow);
}

bool parseScaleHeader(char const *buf, float &scale)
{
    scale = 1.0;

    while (1) {
        if (*buf == '\0') {
            return false;
        }

        if (_strcasecmp(buf, "Scale:", 6) == 0) {
            break;
        }

        ++buf;
    }

    char const* fields = buf + 6;
    while (*fields == ' ') {
        ++fields;
    }

    float sc;
    if (sscanf(fields, "%f", &sc) == 1) {
        scale = sc;
    } else {
        return false;
    }

    return true;
}

char const *dateHeader() 
{
    static char buf[200];
    time_t tt = time(NULL);
    strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
    return buf;
}

bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

bool parseRTSPURL(const char *url, unsigned &address, unsigned short &port, char const **urlSuffix)
{
    do {
        char const *prefix = "rtsp://";
        unsigned const prefixLength = 7;
        if (_strcasecmp(url, prefix, prefixLength) != 0) {
            DPRINTF("URL is not of the form %s ", prefix);
            break;
        }

        unsigned const parseBufferSize = 100;
        char parseBuffer[parseBufferSize];
        char const *from = &url[prefixLength];

        char const *from1 = from;
        while ((*from1 != '\0') && (*from1 != '/')) {
            if (*from1 == '@') {
                from = ++from1;
                break;
            }
            ++from1;
        }

        unsigned i;
        char *to = &parseBuffer[0];

        for (i = 0; i < parseBufferSize; ++i) {
            if ((*from == '\0') || (*from == ':') || (*from == '/')) {
                *to = '\0';
                break;
            }

            *to++ = *from++;
        }

        if (i == parseBufferSize) {
            DPRINTF0("URL is too long");
            break;
        }

        printf("parseBuffer:[%s]\n", parseBuffer);
        if (isValidIpAddress(parseBuffer)) {
            address = inet_addr(parseBuffer);
        } else {
            if (!initializeWinsockIfNecessary()) {
                return false;
            }

            struct hostent *remoteHost = gethostbyname(parseBuffer);

            if (remoteHost && (remoteHost->h_addrtype == AF_INET)) {
                if ((remoteHost->h_length != 4) || (remoteHost->h_addr_list == NULL)) {
                    return false;
                }

                struct in_addr addr;
                addr.s_addr = *(unsigned long *)remoteHost->h_addr_list[0];
                address = addr.s_addr;
            } else {
                return false;
            }
        }

        port = 554;
        char nextChar = *from;

        if (nextChar == ':') {
            int portNumInt;
            if (sscanf(++from, "%d", &portNumInt) != 1) {
                DPRINTF0("No port number follows  : ");
                break;
            }

            if ((portNumInt < 1) || (portNumInt > 65535)) {
                DPRINTF0("Bad port number ");
                break;
            }

            port = portNumInt;
            while ((*from >= '0') && (*from <= '9')) {
                ++from;
            }
        }

        if (urlSuffix != NULL) {
            *urlSuffix = from;
        }

        return true;
    } while (0);

    return false;
}

bool parseRTSPURLUsernamePassword(char const *url, char *&username, char *&password) 
{
    username = password = NULL;

    do {
        char const *prefix = "rtsp://";
        unsigned const prefixLength = 7;
        if (_strcasecmp(url, prefix, prefixLength) != 0) {
            break;
        }

        unsigned colonIndex = 0, atIndex = 0;
        unsigned usernameIndex = prefixLength;

        for (unsigned i = usernameIndex; (url[i] != '\0') && (url[i] != '/'); ++i) {
            if ((url[i] == ':') && (colonIndex == 0)) {
                colonIndex = i;
            } else if (url[i] == '@') {
                atIndex = i;
                break;
            }
        }

        if (atIndex == 0) {
            break;
        }

        char *urlCopy = strDup(url);
        urlCopy[atIndex] = '\0';
        if (colonIndex > 0) {
            urlCopy[colonIndex] = '\0';
            password = strDup(&urlCopy[colonIndex+1]);
        } else {
            password = strDup("");
        }

        username = strDup(&urlCopy[usernameIndex]);
        delete[] urlCopy;

        return true;
    } while (0);

    return false;
}

int trimStartCode(uint8_t *buf, int len)
{
    uint8_t *ptr = buf;
    if (len < 4) {
        return 0;
    }

    if ((ptr[0] == 0x00) && (ptr[1] == 0x00)) {
        while ((*ptr == 0x00) && (ptr < (buf + len - 1))) {
            ptr++;
        }

        if (*ptr != 0x01) {
            DPRINTF("invalid stream, 0x%02x\n", *ptr);
            ptr = buf;
        } else {
            ptr++;
        }
    }

    return (ptr - buf);
}

char *getLine(char *startOfLine) 
{
    for (char *ptr = startOfLine; *ptr != '\0'; ++ptr) {
        if ((*ptr == '\r') || (*ptr == '\n')) {
            if (*ptr == '\r') {
                *ptr++ = '\0';
                if (*ptr == '\n') ++ptr;
            } else {
                *ptr++ = '\0';
            }

            return ptr;
        }
    }

    return NULL;
}

int checkEndian()
{
    int i = 0x00000001;
    if (((char *) & i)[0]) {
        return 0;
    }

    return 1;
}

char* createSDPString(char *mediaType, unsigned char payloadType, char *codec, unsigned frequency, char *trackId)
{
    char const *const sdpFmt =
        "m=%s 0 RTP/AVP %d\r\n"
        "a=rtpmap:%d %s/%d\r\n"
        "a=control:%s\r\n";

    unsigned sdpFmtSize = strlen(sdpFmt)
        + strlen(mediaType) + 5 + 3
        + strlen("a=rtpmap:") + 3 + strlen(codec) + 4
        + strlen("a=control:") + strlen(trackId)
        + 10;

    char *sdpLines = new char[sdpFmtSize];

    sprintf(sdpLines, sdpFmt, mediaType, payloadType, payloadType, codec, frequency, trackId);
    return sdpLines;
}
}
