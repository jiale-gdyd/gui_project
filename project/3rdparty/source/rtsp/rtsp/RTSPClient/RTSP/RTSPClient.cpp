#include <time.h>
#include <rtsp/internal/Base64.hh>
#include <rtsp/internal/NetCommon.h>
#include <rtsp/internal/RTPSource.h>
#include <rtsp/internal/RTSPClient.h>
#include <rtsp/internal/RTSPCommonEnv.h>

namespace rtsp {
static char const *NoSessionErr = "No RTSP session is currently in progress\n";

unsigned char *parseH264ConfigStr(char const *configStr, unsigned int &configSize, unsigned int &spsSize);
unsigned char *parseGeneralConfigStr(char const *configStr, unsigned &configSize);

RTSPClient::RTSPClient()
{
    fCSeq = 0;

    fTCPReadingState = AWAITING_DOLLAR;
    fNextTCPSource = NULL;
    fNextTCPSourceType = 0;

    fResponseBuffer = new char[RECV_BUF_SIZE];
    fResponseBufferSize = RECV_BUF_SIZE;
    resetResponseBuffer();

    fRtpBuffer = new char[RECV_BUF_SIZE];
    fRtpBufferSize = RECV_BUF_SIZE;

    m_nTimeoutSecond = 2;

    fUserAgentHeaderStr = "User-Agent: LJLMediaPlayer\r\n";
    fUserAgentHeaderStrSize = strlen(fUserAgentHeaderStr);

    fBaseURL = NULL;
    fMediaSession = NULL;

    fLastSessionId = fLastSessionIdStr = NULL;
    fTCPStreamIdCount = 0;
    fSessionTimeoutParameter = 0;

    fLastResponseCode = 0;

    fCloseFunc = NULL; fCloseFuncData = NULL;

    fVideoCodec = fAudioCodec = NULL;
    fVideoWidth = fVideoHeight = fVideoFps = 0;
    fChannel = fAudioSampleRate = 0;

    fVideoExtraData = fAudioExtraData = NULL;
    fVideoExtraDataSize = fAudioExtraDataSize = 0;

    fPlayStartTime = fPlayEndTime = 0.0f;

    fIsSendGetParam = false;
    fLastSendGetParam = 0;

    fRTPReceiveFunc = fRTCPReceiveFunc = NULL;
    fRTPReceiveFuncData = fRTCPReceiveFuncData = NULL;

    fTask = new TaskScheduler();
}

RTSPClient::~RTSPClient()
{
    reset();
    
    DELETE_ARRAY(fResponseBuffer);
    DELETE_ARRAY(fRtpBuffer);
    DELETE_OBJECT(fTask);
}

void RTSPClient::reset()
{
    fTask->stopEventLoop();

    if (fRtspSock.isOpened()) {
        fTask->turnOffBackgroundReadHandling(fRtspSock.sock());
        fRtspSock.closeSock();
    }

    fTCPStreamIdCount = 0;

    DELETE_OBJECT(fMediaSession);
    DELETE_ARRAY(fLastSessionId);
    DELETE_ARRAY(fLastSessionIdStr);
    DELETE_ARRAY(fBaseURL);

    fCurrentAuthenticator.reset();

    fCloseFunc = NULL; fCloseFuncData = NULL;

    fVideoCodec = fAudioCodec = NULL;
    fVideoWidth = fVideoHeight = fVideoFps = 0;
    fChannel = fAudioSampleRate = 0;

    DELETE_ARRAY(fVideoExtraData);
    DELETE_ARRAY(fAudioExtraData);
    fVideoExtraDataSize = fAudioExtraDataSize = 0;

    fCSeq = 0;

    fTCPReadingState = AWAITING_DOLLAR;
    fNextTCPSource = NULL;
    fNextTCPSourceType = 0;

    fIsSendGetParam = false;
    fLastSendGetParam = 0;
}

void RTSPClient::resetResponseBuffer()
{
    fResponseBufferIdx = 0;
    memset(fResponseBuffer, 0, RECV_BUF_SIZE);
}

int RTSPClient::connectToServer(const char *ip_addr, unsigned short port, int timeout)
{
    int ret, err;

    int sock = fRtspSock.setupStreamSock(0, true);
    if (sock <= 0) {
        return -1;
    }

    fRtspSock.setReceiveBufferTo(1024*1024);

    struct sockaddr_in svr_addr;
    memset(&svr_addr, 0, sizeof(svr_addr));
    svr_addr.sin_addr.s_addr = inet_addr(ip_addr);
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(port);

    fd_set set;
    FD_ZERO(&set);
    timeval tvout = {timeout, 0};

    FD_SET(sock, &set);

    if ((ret = connect(sock, (struct sockaddr *)&svr_addr, sizeof(svr_addr))) != 0) {
        err = WSAGetLastError();
        if ((err != EINPROGRESS) && (err != EWOULDBLOCK)) {
            DPRINTF0("connect() failed\n");
            goto connect_fail;
        }
        
        if (select(sock + 1, NULL, &set, NULL, &tvout) <= 0) {
            DPRINTF0("select/connect() failed\n");
            goto connect_fail;
        }
        
        err = 0;
        socklen_t len = sizeof(err);
        if ((getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len) < 0) || (err != 0)) {
            DPRINTF("getsockopt() error: %d\n", err);
            goto connect_fail;
        }
    }

    DPRINTF("connected to server %s:%d\n", ip_addr, port);
    return 0;

connect_fail:
    err = WSAGetLastError();
    DPRINTF("cannot connect to server, err:%d\n", err);
    fRtspSock.closeSock();

    return -2;
}

int RTSPClient::sendRequest(char *str, char *tag)
{
    if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTSP) {
        DPRINTF("Sending Request:\n%s\n", str);
    }

    int ret = fRtspSock.writeSocket(str, strlen(str));
    if (ret <= 0) {
        if (tag == NULL) tag = "";
        DPRINTF("send() failed: %s, err: %d\n", tag, WSAGetLastError());
    }

    return ret;
}

bool RTSPClient::parseResponseCode(char const *line, unsigned &responseCode) 
{
    if (sscanf(line, "%*s%u", &responseCode) != 1) {
        DPRINTF("no response code in line: \"""%s""\"", line);
        return false;
    }

    return true;
}

bool RTSPClient::getResponse(char const *tag, unsigned &bytesRead, unsigned &responseCode, char *&firstLine, char *&nextLineStart, bool checkFor200Response) 
{
    do {
        char *readBuf = fResponseBuffer;
        bytesRead = getResponse1(readBuf, fResponseBufferSize);
        if (bytesRead == 0) {
            DPRINTF0("Failed to read response: \n");
            break;
        }

        if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTSP) {
            DPRINTF("Received %s response:\n%s\n", tag, readBuf);
        }

        firstLine = readBuf;
        nextLineStart = getLine(firstLine);
        if (!parseResponseCode(firstLine, responseCode)) {
            break;
        }

        fLastResponseCode = responseCode;

        if ((responseCode != 200) && checkFor200Response) {
            DPRINTF("%s : cannot handle response: %s\n", tag, firstLine);
            break;
        }

        return true;
    } while (0);

    fLastResponseCode = 0;
    return false;
}

unsigned RTSPClient::getResponse1(char *&responseBuffer, unsigned responseBufferSize) 
{
    int sock = fRtspSock.sock();
    struct sockaddr_in fromAddress;

    if (responseBufferSize == 0) {
        return 0;
    }

    responseBuffer[0] = '\0';

    bool success = false;
    while (1) {
        unsigned char firstByte;
        struct timeval timeout;
        timeout.tv_sec = m_nTimeoutSecond; timeout.tv_usec = 0;
        
        if (fRtspSock.readSocket((char *)&firstByte, 1, fromAddress, &timeout) != 1) {
            break;
        }

        if (firstByte != '$') {
            responseBuffer[0] = firstByte;
            success = true;
            break;
        } else {
            unsigned char streamChannelId;
            if (fRtspSock.readSocket((char *)&streamChannelId, 1, fromAddress) != 1) {
                break;
            }

            unsigned short size;
            if (fRtspSock.readSocketExact((char *)&size, 2, fromAddress) != 2) {
                break;
            }
            size = ntohs(size);

            DPRINTF("Discarding interleaved RTP or RTCP packet (%u bytes, channel id %u)\n", size, streamChannelId);

            unsigned char *tmpBuffer = new unsigned char[size];
            if (tmpBuffer == NULL) {
                break;
            }

            int curBytesRead;
            unsigned bytesRead = 0;
            unsigned bytesToRead = size;

            while ((curBytesRead = fRtspSock.readSocket((char *)&tmpBuffer[bytesRead], bytesToRead, fromAddress)) > 0) {
                bytesRead += curBytesRead;
                if (bytesRead >= size) {
                    break;
                }

                bytesToRead -= curBytesRead;
            }

            delete[] tmpBuffer;
            if (bytesRead != size) {
                break;
            }

            success = true;
        }
    }

    if (!success) {
        return 0;
    }

    int bytesRead = 1;
    char *p = responseBuffer;
    bool haveSeenNonCRLF = false;

    while (bytesRead < (int)responseBufferSize) {
        int bytesReadNow = fRtspSock.readSocket((char *)(responseBuffer + bytesRead), 1, fromAddress);
        if (bytesReadNow <= 0) {
            DPRINTF0("RTSP response was truncated\n");
            break;
        }
        bytesRead += bytesReadNow;

        char *lastToCheck = responseBuffer + bytesRead - 4;
        if (lastToCheck < responseBuffer) {
            continue;
        }

        for (; p <= lastToCheck; ++p) {
            if (haveSeenNonCRLF) {
                if ((*p == '\r'
                    && *(p+1) == '\n'
                    && *(p+2) == '\r'
                    && *(p+3) == '\n')
                    || (*(p+2) == '\r'
                    && *(p+3) == '\r')
                    || (*(p+2) == '\n'
                    && *(p+3) == '\n'))
                {
                    responseBuffer[bytesRead] = '\0';

                    while ((*responseBuffer == '\r') || (*responseBuffer == '\n')) {
                        ++responseBuffer;
                        --bytesRead;
                    }

                    return bytesRead;
                }
            } else {
                if ((*p != '\r') && (*p != '\n')) {
                    haveSeenNonCRLF = true;
                }
            }
        }
    }

    DPRINTF0("We received a response not ending with <CR><LF><CR><LF>\n");
    return 0;
}

void RTSPClient::tcpReadError(int result)
{
    int err = WSAGetLastError();
    DPRINTF("failed to read RTSP, err: %d, result: %d\n", err, result);

    fTask->turnOffBackgroundReadHandling(fRtspSock.sock());
    if (fCloseFunc) {
        fCloseFunc(fCloseFuncData, err, result);
    }
}

void RTSPClient::tcpReadHandler(void *instance, int)
{
    RTSPClient *rtsp = (RTSPClient *)instance;
    rtsp->tcpReadHandler1();
}

void RTSPClient::tcpReadHandler1()
{
    int result = 0;
    unsigned char c;
    struct sockaddr_in fromAddress;

    if ((fTCPReadingState != AWAITING_PACKET_DATA) && (fTCPReadingState != AWAITING_RTSP_MESSAGE)) {
        result = fRtspSock.readSocket1((char *)&c, 1, fromAddress);
        if (result != 1) {
            tcpReadError(result);
            return;
        }
    }

    switch (fTCPReadingState) {
        case AWAITING_DOLLAR: {
            if (c == '$') {
                fTCPReadingState = AWAITING_STREAM_CHANNEL_ID;
            } else {
                if (fResponseBufferIdx < fResponseBufferSize) {
                    fResponseBuffer[fResponseBufferIdx++] = c;

                    if (fResponseBufferIdx >= 4) {
                        if (fResponseBuffer[fResponseBufferIdx-4] == '\r'
                            && fResponseBuffer[fResponseBufferIdx-3] == '\n'
                            && fResponseBuffer[fResponseBufferIdx-2] == '\r'
                            && fResponseBuffer[fResponseBufferIdx-1] == '\n')
                        {
                            parseRTSPMessage();
                        }
                    }
                } else {
                    resetResponseBuffer();
                }

                return;
            }
        } break;

        case AWAITING_STREAM_CHANNEL_ID: {
            if (lookupStreamChannelId(c)) {
                fStreamChannelId = c;
                fTCPReadingState = AWAITING_SIZE1;

                if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTP) {
                    DPRINTF("channel id: %d\n", fStreamChannelId);
                }
            } else {
                fTCPReadingState = AWAITING_DOLLAR;
            }
        } break;

        case AWAITING_SIZE1: {
            fSizeByte1 = c;
            fTCPReadingState = AWAITING_SIZE2;
        } break;

        case AWAITING_SIZE2: {
            fTCPReadSize = (fSizeByte1 << 8) | c;
            fTCPReadingState = AWAITING_PACKET_DATA;
            fRtpBufferIdx = 0;

            if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTP) {
                DPRINTF("size: %d\n", fTCPReadSize);
            }
        } break;

        case AWAITING_PACKET_DATA: {
            readRTPOverTCP();
        } break;

        case AWAITING_RTSP_MESSAGE: {
            readRTSPMessage();
        } break;
    }
}

bool RTSPClient::readRTSPMessage()
{

    struct sockaddr_in fromAddress;
    int len = fResponseBufferSize - fResponseBufferIdx - 1;

    if (len <= 0) {
        DPRINTF0("response buffer is full\n");
        fTCPReadingState = AWAITING_DOLLAR;
        return false;
    }

    int result = fRtspSock.readSocket1(&fResponseBuffer[fResponseBufferIdx], len, fromAddress);
    if (result <= 0) {
        tcpReadError(result);
        return false;
    }
    
    fResponseBufferIdx += result;
    fResponseBuffer[fResponseBufferIdx] = 0;

    return parseRTSPMessage();
}

bool RTSPClient::parseRTSPMessage()
{
    bool isCompleted = false;
    char *p = fResponseBuffer;
    bool haveSeenNonCRLF = false;
    char *responseBuffer = fResponseBuffer;

    for (; p <= fResponseBuffer+fResponseBufferIdx-4; ++p) {
        isCompleted = false;
        if ((*p != '\r') && (*p != '\n')) {
            haveSeenNonCRLF = true;
        } else {
            if ((*p == '\r'
                && *(p+1) == '\n'
                && *(p+2) == '\r'
                && *(p+3) == '\n')
                || (*(p+2) == '\r'
                && *(p+3) == '\r')
                || (*(p+2) == '\n'
                && *(p+3) == '\n')) 
            {
                while ((*responseBuffer == '\r') || (*responseBuffer == '\n')) {
                    ++responseBuffer;
                }

                if (responseBuffer < p) {
                    int sz = p+3-responseBuffer;
                    char *msg = new char[sz+1];
                    memcpy(msg, responseBuffer, sz);
                    *(msg+sz) = '\0';

                    delete[] msg;
                }

                responseBuffer = p + 4;
                isCompleted = true;
                haveSeenNonCRLF = false;
            }
        }
    }

    if (!isCompleted) {
        DPRINTF0("RTSP message was fragmented\n");
        int sz = fResponseBuffer + fResponseBufferIdx - responseBuffer;
        memmove(fResponseBuffer, responseBuffer, sz);
        fResponseBufferIdx = sz;
        fTCPReadingState = AWAITING_RTSP_MESSAGE;
    } else {
        if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTSP) {
            DPRINTF("Received %d bytes response:\n%s\n", fResponseBufferIdx, fResponseBuffer);
        }

        resetResponseBuffer();
    }

    return isCompleted;
}

void RTSPClient::handleCmd_notSupported(char const* cseq) 
{
    char tmpBuf[512];
    snprintf((char *)tmpBuf, sizeof(tmpBuf), "RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n\r\n", cseq);
    fRtspSock.writeSocket(tmpBuf, strlen(tmpBuf));
}

bool RTSPClient::lookupStreamChannelId(unsigned char channel)
{
    MediaSubsession *subsession = NULL;
    MediaSubsessionIterator *iter = new MediaSubsessionIterator(*fMediaSession);

    while ((subsession=iter->next()) != NULL) {
        if (subsession->fRTPSource) { 
            if (channel == subsession->rtpChannelId) {
                fNextTCPSource = subsession->fRTPSource;
                fNextTCPSourceType = 0;
                break;
            } else if (channel == subsession->rtcpChannelId) {
                fNextTCPSource = subsession->fRTPSource;
                fNextTCPSourceType = 1;
                break;
            }
        }
    }

    delete iter;

    if (subsession == NULL) {
        DPRINTF("channel id: %d not found handler\n", channel);
        return false;
    }

    return true;
}

void RTSPClient::readRTPOverTCP()
{
    struct sockaddr_in fromAddress;
    int bytesRead = fTCPReadSize - fRtpBufferIdx;

    int result = fRtspSock.readSocket1(&fRtpBuffer[fRtpBufferIdx], bytesRead, fromAddress);
    if (result <= 0) {
        tcpReadError(result);
        return;
    }

    fRtpBufferIdx += result;
    if (fRtpBufferIdx != fTCPReadSize) {
        return;
    }

    if (fNextTCPSource) {
        if (fNextTCPSourceType == 0)  {
            fNextTCPSource->rtpReadHandler(fRtpBuffer, fRtpBufferIdx, fromAddress);
        } else {
            fNextTCPSource->rtcpReadHandler(fRtpBuffer, fRtpBufferIdx, fromAddress);
        }
    }

    fTCPReadingState = AWAITING_DOLLAR;	
    fRtpBufferIdx = 0;
}

char *RTSPClient::sendOptionsCmd(const char *url, char *username, char *password, Authenticator *authenticator)
{
    char *cmd = NULL;
    char *result = NULL;
    bool haveAllocatedAuthenticator = false;

    do {
        if (authenticator == NULL) {
            if ((username == NULL) && (password == NULL) && parseRTSPURLUsernamePassword(url, username, password)) {
                Authenticator newAuthenticator;
                newAuthenticator.setUsernameAndPassword(username, password);
                result = sendOptionsCmd(url, username, password, &newAuthenticator);
                delete[] username; delete[] password;
                break;
            } else if ((username != NULL) && (password != NULL)) {
                authenticator = new Authenticator;
                haveAllocatedAuthenticator = true;
                authenticator->setUsernameAndPassword(username, password);

                result = sendOptionsCmd(url, username, password, authenticator);
                if (result != NULL) {
                    break;
                }

                if (authenticator->realm() == NULL) {
                    break;
                }
            }
        }

        char *authenticatorStr = createAuthenticatorString(authenticator, "OPTIONS", url);

        char *const cmdFmt =
            "OPTIONS %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "%s"
            "%s"
            "\r\n";

        unsigned cmdSize = strlen(cmdFmt)
            + strlen(url)
            + 20
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize;

        cmd = new char[cmdSize];
        sprintf(cmd, cmdFmt,
            url,
            ++fCSeq,
            authenticatorStr,
            fUserAgentHeaderStr);
        delete[] authenticatorStr;

        if (sendRequest(cmd, "OPTIONS") <= 0) {
            break;
        }

        char *firstLine;
        char *nextLineStart;
        unsigned bytesRead; unsigned responseCode;

        if (!getResponse("OPTIONS", bytesRead, responseCode, firstLine, nextLineStart, false)) {
            break;
        }

        if (responseCode != 200) {
            checkForAuthenticationFailure(responseCode, nextLineStart, authenticator);
            DPRINTF("cannot handle OPTIONS response: %s\n", firstLine);
            break;
        }

        char *lineStart;
        while (1) {
            lineStart = nextLineStart;
            if (lineStart == NULL) {
                break;
            }

            nextLineStart = getLine(lineStart);
            if (_strcasecmp(lineStart, "Public: ", 8) == 0) {
                delete[] result; result = strDup(&lineStart[8]);
            } else if (_strcasecmp(lineStart, "Session: ", 9) == 0) {
                delete[] fLastSessionId; fLastSessionId = strDup(&lineStart[9]);
            }
        }
    } while (0);

    delete[] cmd;
    if (haveAllocatedAuthenticator) {
        delete authenticator;
    }

    return result;
}

char* RTSPClient::describeURL(const char *url, Authenticator *authenticator, bool allowKasennaProtocol)
{
    char *cmd = NULL;

    do {
        char *username;
        char *password;

        if ((authenticator == NULL) && parseRTSPURLUsernamePassword(url, username, password)) {
                char *result = describeWithPassword(url, username, password, allowKasennaProtocol);
                delete[] username;
                delete[] password;
                return result;
        }

        fCurrentAuthenticator.reset();
        char *authenticatorStr = createAuthenticatorString(authenticator, "DESCRIBE", url);

        char const *acceptStr = allowKasennaProtocol ? "Accept: application/x-rtsp-mh, application/sdp\r\n" : "Accept: application/sdp\r\n";

        char *const cmdFmt =
            "DESCRIBE %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "%s"
            "%s"
            "%s"
            "\r\n";

        delete[] fLastSessionIdStr;

        int Strlen;
        if (fLastSessionId) {
            Strlen = strlen(fLastSessionId);
        } else {
            Strlen = 0;
        }

        fLastSessionIdStr = new char[20 + Strlen];
        if (fLastSessionId) {
            sprintf(fLastSessionIdStr, "Session: %s\r\n", fLastSessionId);
        }

        unsigned cmdSize = strlen(cmdFmt)
            + strlen(url)
            + 20
            + strlen(acceptStr)
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize;

        cmd = new char[cmdSize];
        sprintf(cmd, cmdFmt,
            url,
            ++fCSeq,
            fLastSessionIdStr,
            acceptStr,
            authenticatorStr,
            fUserAgentHeaderStr);
        delete[] authenticatorStr;

        if (sendRequest(cmd, "DESCRIBE") <= 0) {
            break;
        }

        char *firstLine;
        char *nextLineStart;
        unsigned bytesRead, responseCode;

        if (!getResponse("DESCRIBE", bytesRead, responseCode, firstLine, nextLineStart,  false)) {
            DPRINTF0("Cannot read DESCRIBE response\n");
            break;
        }

        char *redirectionURL = NULL;
        bool wantRedirection = false;

        if ((responseCode == 301) || (responseCode == 302)) {
            wantRedirection = true;
            redirectionURL = new char[fResponseBufferSize];
        } else if (responseCode != 200) {
            checkForAuthenticationFailure(responseCode, nextLineStart, authenticator);
            DPRINTF("cannot handle DESCRIBE response: %s\n", firstLine);
            break;
        }

        char *lineStart;
        int contentLength = -1;
        char *serverType = new char[fResponseBufferSize];

        while (1) {
            lineStart = nextLineStart;
            if (lineStart == NULL) {
                break;
            }

            nextLineStart = getLine(lineStart);
            if (lineStart[0] == '\0') {
                break;
            }

            if (sscanf(lineStart, "Content-Length: %d", &contentLength) == 1
                || sscanf(lineStart, "Content-length: %d", &contentLength) == 1)
            {
                if (contentLength < 0) {
                    DPRINTF("Bad \"Content-length:\" header: \"%s\"\n", lineStart);
                    break;
                }
            } else if (strncmp(lineStart, "Content-Base:", 13) == 0
                || strncmp(lineStart, "Content-base:", 13) == 0)
            {
                int cbIndex = 13;

                while (lineStart[cbIndex] == ' ' || lineStart[cbIndex] == '\t') ++cbIndex;
                if (lineStart[cbIndex] != '\0') {
                    delete[] fBaseURL; fBaseURL = strDup(&lineStart[cbIndex]);
                }
            } else if (sscanf(lineStart, "Session: %[^;]", serverType) == 1) {
                delete[] fLastSessionId; fLastSessionId = strDup(serverType);
            } else if (wantRedirection) {
                if (sscanf(lineStart, "Location: %s", redirectionURL) == 1) {
                    DPRINTF("Redirecting to the new URL \"%s\"\n", redirectionURL);
                }

                reset();

                char *result = describeURL(redirectionURL, authenticator, allowKasennaProtocol);

                delete[] redirectionURL;
                delete[] serverType;
                delete[] cmd;
                return result;
            }
        }

        delete[] serverType;

        if (wantRedirection) {
            DPRINTF0("Saw redirection response code, but not a \"Location:\" header");
            delete[] redirectionURL;
            break;
        }

        if (lineStart == NULL) {
            DPRINTF("no content following header lines: %s\n", fResponseBuffer);
            break;
        }

        char *bodyStart = nextLineStart;
        if (contentLength >= 0) {
            unsigned numBodyBytes = &firstLine[bytesRead] - bodyStart;
            if (contentLength > (int)numBodyBytes) {
                unsigned numExtraBytesNeeded = contentLength - numBodyBytes;
                unsigned remainingBufferSize = fResponseBufferSize - (bytesRead + (firstLine - fResponseBuffer));
                if (numExtraBytesNeeded > remainingBufferSize) {
                    char tmpBuf[200];
                    sprintf(tmpBuf, "Read buffer size (%d) is too small for \"Content-length:\" %d (need a buffer size of >= %d bytes\n",
                        fResponseBufferSize, contentLength, fResponseBufferSize + numExtraBytesNeeded - remainingBufferSize);
                    DPRINTF0(tmpBuf);
                    break;
                }

                if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTSP) {
                    DPRINTF("Need to read %d extra bytes\n", numExtraBytesNeeded);
                }

                while (numExtraBytesNeeded > 0) {
                    struct sockaddr_in fromAddress;
                    char *ptr = &firstLine[bytesRead];
                    int bytesRead2 = fRtspSock.readSocket(ptr, numExtraBytesNeeded, fromAddress);
                    if (bytesRead2 < 0) {
                        break;
                    }

                    ptr[bytesRead2] = '\0';

                    if (RTSPCommonEnv::nDebugFlag & DEBUG_FLAG_RTSP) {
                        DPRINTF("Read %d extra bytes:\n%s\n", bytesRead2, ptr);
                    }

                    bytesRead += bytesRead2;
                    numExtraBytesNeeded -= bytesRead2;
                }

                if (numExtraBytesNeeded > 0) {
                    break;
                }
            }

            int from, to = 0;
            for (from = 0; from < contentLength; ++from) {
                if (bodyStart[from] != '\0') {
                    if (to != from) {
                        bodyStart[to] = bodyStart[from];
                    }

                    ++to;
                }
            }

            if (from != to) {
                DPRINTF("Warning: %s invalid 'NULL' bytes were found in (and removed from) the SDP description.\n", from-to);
            }

            bodyStart[to] = '\0';
        }

        delete[] cmd;
        return strDup(bodyStart);
    } while (0);

    delete[] cmd;
    return NULL;
}

char *RTSPClient::describeWithPassword(char const *url, char const *username, char const *password, bool allowKasennaProtocol)
{
    Authenticator authenticator;
    authenticator.setUsernameAndPassword(username, password);
    char *describeResult = describeURL(url, &authenticator, allowKasennaProtocol);
    if (describeResult != NULL) {
        return describeResult;
    }

    if (authenticator.realm() == NULL) {
        return NULL;
    }

    describeResult = describeURL(url, &authenticator, allowKasennaProtocol);
    if (describeResult != NULL) {
        fCurrentAuthenticator = authenticator;
    }

    return describeResult;
}

static bool isAbsoluteURL(char const* url) 
{
    while ((*url != '\0') && (*url != '/')) {
        if (*url == ':') {
            return true;
        }

        ++url;
    }

    return false;
}

char const *RTSPClient::sessionURL(MediaSession const &session) const 
{
    char const *url = session.controlPath();
    if ((url == NULL) || (strcmp(url, "*") == 0)) {
        url = fBaseURL;
    }

    return url;
}

void RTSPClient::constructSubsessionURL(MediaSubsession const &subsession, char const *&prefix, char const *&separator, char const *&suffix) 
{
    prefix = sessionURL(subsession.parentSession());
    if (prefix == NULL) {
        prefix = "";
    }

    suffix = subsession.controlPath();
    if (suffix == NULL) {
        suffix = "";
    }

    if (isAbsoluteURL(suffix)) {
        prefix = separator = "";
    } else {
        unsigned prefixLen = strlen(prefix);
        separator = (prefix[prefixLen-1] == '/' || suffix[0] == '/') ? "" : "/";
    }
}

bool RTSPClient::parseTransportResponse(char const *line, char *&serverAddressStr, unsigned short &serverPortNum, unsigned char &rtpChannelId, unsigned char &rtcpChannelId) 
{
    serverAddressStr = NULL;
    serverPortNum = 0;
    rtpChannelId = rtcpChannelId = 0xFF;

    char *foundServerAddressStr = NULL;
    bool foundServerPortNum = false;
    bool foundChannelIds = false;
    unsigned rtpCid, rtcpCid;
    bool isMulticast = true;
    char *foundDestinationStr = NULL;
    unsigned short multicastPortNumRTP, multicastPortNumRTCP;
    bool foundMulticastPortNum = false;

    if (_strcasecmp(line, "Transport: ", 11) != 0) {
        return false;
    }
    line += 11;

    char const *fields = line;
    char *field = strDupSize(fields);

    while (sscanf(fields, "%[^;]", field) == 1) {
        if (sscanf(field, "server_port=%hu", &serverPortNum) == 1) {
            foundServerPortNum = true;
        } else if (_strcasecmp(field, "source=", 7) == 0) {
            delete[] foundServerAddressStr;
            foundServerAddressStr = strDup(field+7);
        } else if (sscanf(field, "interleaved=%u-%u", &rtpCid, &rtcpCid) == 2) {
            rtpChannelId = (unsigned char)rtpCid;
            rtcpChannelId = (unsigned char)rtcpCid;
            foundChannelIds = true;
        } else if (strcmp(field, "unicast") == 0) {
            isMulticast = false;
        } else if (_strcasecmp(field, "destination=", 12) == 0) {
            delete[] foundDestinationStr;
            foundDestinationStr = strDup(field+12);
        } else if (sscanf(field, "port=%hu-%hu", &multicastPortNumRTP, &multicastPortNumRTCP) == 2) {
            foundMulticastPortNum = true;
        }

        fields += strlen(field);
        while (fields[0] == ';') {
            ++fields;
        }

        if (fields[0] == '\0') {
            break;
        }
    }
    delete[] field;

    if (isMulticast && (foundDestinationStr != NULL) && foundMulticastPortNum) {
        delete[] foundServerAddressStr;
        serverAddressStr = foundDestinationStr;
        serverPortNum = multicastPortNumRTP;
        return true;
    }
    delete[] foundDestinationStr;

    if (foundServerPortNum || foundChannelIds) {
        serverAddressStr = foundServerAddressStr;
        return true;
    }

    delete[] foundServerAddressStr;
    return false;
}

bool RTSPClient::setupMediaSubsession(MediaSubsession &subsession, bool streamOutgoing, bool streamUsingTCP, bool forceMulticastOnUnspecified)
{
    char *cmd = NULL;
    char *setupStr = NULL;

    do {
        char *sessionStr;
        char *authenticatorStr = createAuthenticatorString(&fCurrentAuthenticator, "SETUP", fBaseURL);

        if (fLastSessionId != NULL) {
            sessionStr = new char[20 + strlen(fLastSessionId)];
            sprintf(sessionStr, "Session: %s\r\n", fLastSessionId);
        } else {
            sessionStr = strDup("");
        }

        char *transportFmt;
        char *transportStr = NULL;

        char const *prefix, *separator, *suffix;
        constructSubsessionURL(subsession, prefix, separator, suffix);

        if (strcmp(subsession.protocolName(), "UDP") == 0) {
            char const *setupFmt = "SETUP %s%s RTSP/1.0\r\n";
            unsigned setupSize = strlen(setupFmt) + strlen(prefix) + strlen (separator);
            setupStr = new char[setupSize];
            sprintf(setupStr, setupFmt, prefix, separator);

            transportFmt = "Transport: RAW/RAW/UDP%s%s%s=%d-%d\r\n";
        } else {
            char const *setupFmt = "SETUP %s%s%s RTSP/1.0\r\n";
            unsigned setupSize = strlen(setupFmt) + strlen(prefix) + strlen (separator) + strlen(suffix);
            setupStr = new char[setupSize];
            sprintf(setupStr, setupFmt, prefix, separator, suffix);

            transportFmt = "Transport: RTP/AVP%s%s%s=%d-%d\r\n";
        }

        if (transportStr == NULL) {
            char const *transportTypeStr;
            char const *modeStr = streamOutgoing ? ";mode=receive" : "";

            char const *portTypeStr;
            unsigned short rtpNumber, rtcpNumber;
            if (streamUsingTCP) {
                transportTypeStr = "/TCP;unicast";
                portTypeStr = ";interleaved";
                rtpNumber = fTCPStreamIdCount++;
                rtcpNumber = fTCPStreamIdCount++;
            } else {
                unsigned connectionAddress = subsession.connectionEndpointAddress();
                bool requestMulticastStreaming = isMulticastAddress(connectionAddress) || (connectionAddress == 0 && forceMulticastOnUnspecified);
                transportTypeStr = requestMulticastStreaming ? ";multicast" : ";unicast";
                portTypeStr = ";client_port";
                rtpNumber = subsession.clientPortNum();
                if (rtpNumber == 0) {
                    DPRINTF0("Client port number unknown\n");
                    delete[] authenticatorStr;
                    delete[] sessionStr;
                    delete[] setupStr;
                    break;
                }

                rtcpNumber = rtpNumber + 1;
            }

            unsigned transportSize = strlen(transportFmt) + strlen(transportTypeStr) + strlen(modeStr) + strlen(portTypeStr) + 2*5;
            transportStr = new char[transportSize];
            sprintf(transportStr, transportFmt, transportTypeStr, modeStr, portTypeStr, rtpNumber, rtcpNumber);
        }

        char *const cmdFmt =
            "%s"
            "CSeq: %d\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "\r\n";

        unsigned cmdSize = strlen(cmdFmt)
            + strlen(setupStr)
            + 20
            + strlen(transportStr)
            + strlen(sessionStr)
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize;

        cmd = new char[cmdSize];

        sprintf(cmd, cmdFmt,
            setupStr,
            ++fCSeq,
            transportStr,
            sessionStr,
            authenticatorStr,
            fUserAgentHeaderStr);

        delete[] authenticatorStr;
        delete[] sessionStr;
        delete[] setupStr;
        delete[] transportStr;

        if (!sendRequest(cmd, "SETUP")) {
            break;
        }

        char *firstLine; char *nextLineStart;
        unsigned bytesRead; unsigned responseCode;

        if (!getResponse("SETUP", bytesRead, responseCode, firstLine, nextLineStart)) {
            break;
        }

        char *lineStart;
        unsigned cLength = 0;
        char *sessionId = new char[fResponseBufferSize];

        while (1) {
            lineStart = nextLineStart;
            if (lineStart == NULL) {
                break;
            }

            nextLineStart = getLine(lineStart);

            if (sscanf(lineStart, "Session: %[^;]", sessionId) == 1) {
                subsession.sessionId = strDup(sessionId);
                delete[] fLastSessionId;
                fLastSessionId = strDup(sessionId);

                char *afterSessionId = lineStart + strlen(sessionId) + strlen("Session: ");;
                int timeoutVal;
                if (sscanf(afterSessionId, "; timeout = %d", &timeoutVal) == 1) {
                    fSessionTimeoutParameter = timeoutVal;
                }

                continue;
            }

            char *serverAddressStr;
            unsigned short serverPortNum;
            unsigned char rtpChannelId, rtcpChannelId;
            
            if (parseTransportResponse(lineStart, serverAddressStr, serverPortNum, rtpChannelId, rtcpChannelId))  {
                delete[] subsession.connectionEndpointName();
                subsession.connectionEndpointName() = serverAddressStr;
                subsession.serverPortNum = serverPortNum;
                subsession.rtpChannelId = rtpChannelId;
                subsession.rtcpChannelId = rtcpChannelId;
                continue;
            }

            if (sscanf(lineStart, "Content-Length: %d", &cLength) == 1) {
                continue;
            }
        }
        delete[] sessionId;

        if (subsession.sessionId == NULL) {
            DPRINTF0("\"Session:\" header is missing in the response");
            break;
        }

        if (cLength > 0) {
            char *dummyBuf = new char[cLength];
            getResponse1(dummyBuf, cLength);
            delete[] dummyBuf;
        }

        if (streamUsingTCP) {
            if (subsession.fRTPSource) {
                subsession.fRTPSource->setRtspSock(&fRtspSock);
                subsession.fRTPSource->setRtcpChannelId(subsession.rtcpChannelId);
            }
        } else {
            if (subsession.fRTPSource) {
                subsession.fRTPSource->setServerPort(subsession.serverPortNum);
            }

            unsigned int destAddress = subsession.connectionEndpointAddress();
            if (destAddress != 0) {
                subsession.setDestinations(destAddress);
            }
        }

        delete[] cmd;
        return true;
    } while (0);

    delete[] cmd;
    return false;
}

static char *createScaleString(float scale, float currentScale)
{
    char buf[100];
    if ((scale == 1.0f) && (currentScale == 1.0f)) {
        buf[0] = '\0';
    } else {
        sprintf(buf, "Scale: %f\r\n", scale);
    }

    return strDup(buf);
}

static char *createRangeString(double start, double end)
{
    char buf[100];

    if (start < 0) {
        buf[0] = '\0';
    } else if (end < 0) {
        sprintf(buf, "Range: npt=%.3f-\r\n", start);
    } else {
        sprintf(buf, "Range: npt=%.3f-%.3f\r\n", start, end);
    }

    return strDup(buf);
}

bool RTSPClient::parseRangeHeader(char const *buf, double &rangeStart, double &rangeEnd) 
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

    double start, end;
    if (sscanf(fields, "npt = %lf - %lf", &start, &end) == 2) {
        rangeStart = start;
        rangeEnd = end;
    } else if (sscanf(fields, "npt = %lf -", &start) == 1) {
        rangeStart = start;
        rangeEnd = 0.0;
    } else {
        return false;
    }

    return true;
}

bool RTSPClient::parseRTPInfoHeader(char *&line, unsigned short &seqNum, unsigned int &timestamp) 
{
    if (_strcasecmp(line, "RTP-Info: ", 10) == 0) {
        line += 10;
    } else if (line[0] == ',') {
        ++line;
    } else {
        return false;
    }

    char *field = strDupSize(line);

    while (sscanf(line, "%[^;,]", field) == 1) {
        if ((sscanf(field, "seq=%hu", &seqNum) == 1) || (sscanf(field, "rtptime=%u", &timestamp) == 1)) {

        }

        line += strlen(field);
        if ((line[0] == '\0') || (line[0] == ',')) {
            break;
        }

        ++line;
    }

    delete[] field;
    return true;
}

bool RTSPClient::playMediaSession(MediaSession &session, bool response, double start, double end, float scale) 
{
    char *cmd = NULL;

    do {
        if (fLastSessionId == NULL) {
            DPRINTF0((char *)NoSessionErr);
            break;
        }

        char *authenticatorStr = createAuthenticatorString(&fCurrentAuthenticator, "PLAY", fBaseURL);
        char *scaleStr = createScaleString(scale, session.scale());
        char *rangeStr = createRangeString(start, end);

        char *const cmdFmt =
            "PLAY %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "Session: %s\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "\r\n";

        char const *sessURL = sessionURL(session);
        unsigned cmdSize = strlen(cmdFmt)
            + strlen(sessURL)
            + 20
            + strlen(fLastSessionId)
            + strlen(scaleStr)
            + strlen(rangeStr)
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize;

        cmd = new char[cmdSize];
        sprintf(cmd, cmdFmt,
            sessURL,
            ++fCSeq,
            fLastSessionId,
            scaleStr,
            rangeStr,
            authenticatorStr,
            fUserAgentHeaderStr);
        delete[] scaleStr;
        delete[] rangeStr;
        delete[] authenticatorStr;

        if (!sendRequest(cmd, "PLAY")) {
            break;
        }

        if (!response) {
            goto skip_response;
        }

        char *firstLine; char *nextLineStart;
        unsigned bytesRead; unsigned responseCode;

        if (!getResponse("PLAY", bytesRead, responseCode, firstLine, nextLineStart)) {
            break;
        }

        char *lineStart;

        while (1) {
            lineStart = nextLineStart;
            if (lineStart == NULL) {
                break;
            }

            nextLineStart = getLine(lineStart);

            if (parseScaleHeader(lineStart, session.scale())) {
                continue;
            }

            if (parseRangeHeader(lineStart, session.playStartTime(), session.playEndTime())) {
                continue;
            }

            unsigned short seqNum; unsigned int timestamp;
            if (parseRTPInfoHeader(lineStart, seqNum, timestamp)) {
                MediaSubsession *subsession;
                MediaSubsessionIterator iter(session);

                while ((subsession = iter.next()) != NULL) {
                    subsession->rtpInfo.seqNum = seqNum;
                    subsession->rtpInfo.timestamp = timestamp;
                    subsession->rtpInfo.infoIsNew = true;

                    if (!parseRTPInfoHeader(lineStart, seqNum, timestamp)) {
                        break;
                    }
                }

                continue;
            }
        }

skip_response:
        delete[] cmd;
        return true;
    } while (0);

    delete[] cmd;
    return false;
}

bool RTSPClient::pauseMediaSession(MediaSession &session)
{
    char *cmd = NULL;

    do {
        if (fLastSessionId == NULL) {
            DPRINTF0((char *)NoSessionErr);
            break;
        }

        char *authenticatorStr = createAuthenticatorString(&fCurrentAuthenticator, "PAUSE", fBaseURL);

        char const *sessURL = sessionURL(session);
        char *const cmdFmt =
            "PAUSE %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "Session: %s\r\n"
            "%s"
            "%s"
            "\r\n";

        unsigned cmdSize = strlen(cmdFmt)
            + strlen(sessURL)
            + 20
            + strlen(fLastSessionId)
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize;

        cmd = new char[cmdSize];
        sprintf(cmd, cmdFmt,
            sessURL,
            ++fCSeq,
            fLastSessionId,
            authenticatorStr,
            fUserAgentHeaderStr);
        delete[] authenticatorStr;

        if (!sendRequest(cmd, "PAUSE")) {
            break;
        }

        delete[] cmd;
        return true;
    } while (0);

    delete[] cmd;
    return false;
}

int RTSPClient::openURL(const char *url, int streamType, int timeout, bool rtpOnly)
{
    bool isOpened = false;
    char *username = NULL;
    char *password = NULL;
    const char *parsedURL = NULL;

    m_nTimeoutSecond = timeout;

    do {
        unsigned address = 0;
        char const *urlSuffix;
        unsigned short port = 0;

        if (!parseRTSPURL(url, address, port, &urlSuffix)) {
            printf("parseRTSPURL failed\n");
            break;
        }

        struct in_addr addr;
        addr.s_addr = address;
        const char *ip_address = inet_ntoa(addr);

        parsedURL = new char[strlen(ip_address)+strlen(urlSuffix)+20];
        sprintf((char *)parsedURL, "rtsp://%s:%hu%s", ip_address, port, urlSuffix);

        parseRTSPURLUsernamePassword(url, username, password);

        if (connectToServer(ip_address, port, timeout) < 0) {
            break;
        }

        char *result = sendOptionsCmd(url, username, password, NULL);
        if (!result) {
            break;
        }

        if ((fLastResponseCode == 200) && strstr(result, "GET_PARAMETER")) {
            fIsSendGetParam = true;
        }

        delete[] result;

        char *sdpDescription = NULL;
        if ((username != NULL) && (password != NULL)) {
            sdpDescription = describeWithPassword(url, username, password);
        } else {
            sdpDescription = describeURL(url);
        }

        if (!sdpDescription) {
            break;
        }

        fMediaSession = MediaSession::createNew(sdpDescription);
        if (!fMediaSession) {
            DPRINTF0("create MediaSession failed\n");
            delete[] sdpDescription;
            break;
        }

        MediaSubsession *subsession = NULL;
        MediaSubsessionIterator *iter = new MediaSubsessionIterator(*fMediaSession);

        while ((subsession=iter->next()) != NULL) {
            if (!subsession->initiate(streamType, *fTask, rtpOnly)) {
                DPRINTF("RTSP subsession '%s/%s' failed\n", subsession->mediumName(), subsession->codecName());
                continue;
            }

            bool res;
            if (streamType == 0) {
                res = setupMediaSubsession(*subsession, false, false);
            } else if (streamType == 1) {
                res = setupMediaSubsession(*subsession, false, true);
            } else if (streamType == 2) {
                res = setupMediaSubsession(*subsession, false, false, true);
            } else {
                continue;
            }

            if (!res) {
                DPRINTF0("setup media subsession failed\n");
                continue;
            }

            if (strcmp(subsession->mediumName(), "video") == 0) {
                fVideoCodec = subsession->codecName();
                fVideoWidth = subsession->videoWidth();
                fVideoHeight = subsession->videoHeight();
                fVideoFps = subsession->videoFPS();

                if (!strcmp(fVideoCodec, "H264")) {
                    char *spropParameterSets = (char *)subsession->fmtp_spropparametersets();
                    if (spropParameterSets) {
                        unsigned int sps_size;
                        fVideoExtraData = parseH264ConfigStr(spropParameterSets, fVideoExtraDataSize, sps_size);
                    }
                } else if (!strcmp(fVideoCodec, "MP4V-ES")) {
                    if (subsession->fmtp_config()) {
                        fVideoExtraData = parseGeneralConfigStr((char const*)subsession->fmtp_config(), fVideoExtraDataSize);
                    }
                }
            } else if (strcmp(subsession->mediumName(), "audio") == 0) {
                fAudioCodec = subsession->codecName();
                fChannel = subsession->numChannels();
                fAudioSampleRate = subsession->rtpTimestampFrequency();

                if (!strcmp(fAudioCodec, "MPEG4-GENERIC")) {
                    if (subsession->fmtp_config()) {
                        fAudioExtraData = parseGeneralConfigStr((char const*)subsession->fmtp_config(), fAudioExtraDataSize);
                    }
                }
            }

            isOpened = true;
        }

        fPlayStartTime = fMediaSession->playStartTime();
        fPlayEndTime = fMediaSession->playEndTime();

        delete iter;
        delete[] sdpDescription;
    } while (0);

    if (parsedURL) {
        delete[] parsedURL;
    }

    if (username) {
        delete[] username;
    }

    if (password) {
        delete[] password;
    }

    if (!isOpened) {
        return -1;
    }

    return 0;
}

int RTSPClient::playURL(FrameHandlerFunc func, void *funcData,  OnCloseFunc onCloseFunc, void *onCloseFuncData,  OnPacketReceiveFunc onRTPReceiveFunc, void *onRTPReceiveFuncData, OnPacketReceiveFunc onRTCPReceiveFunc, void *onRTCPReceiveFuncData)
{
    if (!fMediaSession) {
        return -1;
    }

    if (!playMediaSession(*fMediaSession, true)) {
        return -2;
    }

    fCloseFunc = onCloseFunc;
    fCloseFuncData = onCloseFuncData;

    fRTPReceiveFunc = onRTPReceiveFunc;
    fRTPReceiveFuncData = onRTPReceiveFuncData;

    fRTCPReceiveFunc = onRTCPReceiveFunc;
    fRTCPReceiveFuncData = onRTCPReceiveFuncData;

    MediaSubsession *subsession = NULL;
    MediaSubsessionIterator *iter = new MediaSubsessionIterator(*fMediaSession);

    while ((subsession = iter->next()) != NULL) {
        if (subsession->fRTPSource) {
            subsession->fRTPSource->startNetworkReading(func, funcData, rtpHandlerCallback, this, rtcpHandlerCallback, this);
        }
    }

    if (fTCPStreamIdCount > 0) {
        fTCPReadingState = AWAITING_DOLLAR;
    } else {
        fTCPReadingState = AWAITING_RTSP_MESSAGE;
    }

    resetResponseBuffer();

    fTask->turnOnBackgroundReadHandling(fRtspSock.sock(), tcpReadHandler, this);
    fTask->startEventLoop();

    delete iter;
    return 0;
}

int RTSPClient::sendPause()
{
    if (!fMediaSession) {
        return -1;
    }

    if (!pauseMediaSession(*fMediaSession)) {
        return -1;
    }

    return 0;
}

int RTSPClient::sendPlay(double start, double end, float scale)
{
    if (!fMediaSession) {
        return -1;
    }

    if (!playMediaSession(*fMediaSession, false, start, end, scale)) {
        return -1;
    }

    return 0;
}

int RTSPClient::sendSetParam(char *name, char *value)
{
    if (!fMediaSession) {
        return -1;
    }

    if (!setMediaSessionParameter(*fMediaSession, name, value)) {
        return -1;
    }

    return 0;
}

char* RTSPClient::createAuthenticatorString(Authenticator const *authenticator, char const *cmd, char const *url) 
{
    if ((authenticator != NULL) && (authenticator->realm() != NULL) && (authenticator->username() != NULL) && (authenticator->password() != NULL)) {
            char *authenticatorStr;
            if (authenticator->nonce() != NULL) {
                char *const authFmt =
                    "Authorization: Digest username=\"%s\", realm=\"%s\", "
                    "nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n";
                char const *response = authenticator->computeDigestResponse(cmd, url);
                unsigned authBufSize = strlen(authFmt)
                    + strlen(authenticator->username()) + strlen(authenticator->realm())
                    + strlen(authenticator->nonce()) + strlen(url) + strlen(response);
                authenticatorStr = new char[authBufSize];
                sprintf(authenticatorStr, authFmt, authenticator->username(), authenticator->realm(), authenticator->nonce(), url, response);
                authenticator->reclaimDigestResponse(response);
            } else {
                char *const authFmt = "Authorization: Basic %s\r\n";

                unsigned usernamePasswordLength = strlen(authenticator->username()) + 1 + strlen(authenticator->password());
                char *usernamePassword = new char[usernamePasswordLength + 1];
                sprintf(usernamePassword, "%s:%s", authenticator->username(), authenticator->password());

                char *response = base64Encode(usernamePassword, usernamePasswordLength);
                unsigned const authBufSize = strlen(authFmt) + strlen(response) + 1;
                authenticatorStr = new char[authBufSize];
                sprintf(authenticatorStr, authFmt, response);

                delete[] response;
                delete[] usernamePassword;
            }

            return authenticatorStr;
    }

    return strDup("");
}

void RTSPClient::checkForAuthenticationFailure(unsigned responseCode, char *&nextLineStart, Authenticator *authenticator)
{
    if ((responseCode == 401) && (authenticator != NULL)) {
        char *lineStart;

        while (1) {
            lineStart = nextLineStart;
            if (lineStart == NULL) {
                break;
            }

            nextLineStart = getLine(lineStart);
            if (lineStart[0] == '\0') {
                break;
            }

            char *realm = strDupSize(lineStart);
            char *nonce = strDupSize(lineStart);
            bool foundAuthenticateHeader = false;

            if (sscanf(lineStart, "WWW-Authenticate: Digest realm=\"%[^\"]\", nonce=\"%[^\"]\"", realm, nonce) == 2) {
                authenticator->setRealmAndNonce(realm, nonce);
                foundAuthenticateHeader = true;
            } else if (sscanf(lineStart, "WWW-Authenticate: Basic realm=\"%[^\"]\"", realm) == 1) {
                authenticator->setRealmAndNonce(realm, NULL);
                foundAuthenticateHeader = true;
            }

            delete[] realm;
            delete[] nonce;
            if (foundAuthenticateHeader) {
                break;
            }
        }
    }
}

void RTSPClient::rtpHandlerCallback(void *arg, char *trackId, char *buf, int len)
{
    RTSPClient *client = (RTSPClient *)arg;

    if (client->fIsSendGetParam) {
        time_t curr_time;
        time(&curr_time);

        if (curr_time - client->fLastSendGetParam >= SEND_GET_PARAM_DURATION) {
            client->sendGetParam();
            client->fLastSendGetParam = curr_time;
        }
    }

    if (client->fRTPReceiveFunc) {
        client->fRTPReceiveFunc(client->fRTPReceiveFuncData, trackId, buf, len);
    }
}

void RTSPClient::rtcpHandlerCallback(void *arg, char *trackId, char *buf, int len)
{
    RTSPClient *client = (RTSPClient *)arg;

    if (client->fRTCPReceiveFunc) {
        client->fRTCPReceiveFunc(client->fRTCPReceiveFuncData, trackId, buf, len);
    }
}

void RTSPClient::sendGetParam()
{
    char *str = NULL;
    getMediaSessionParameter(*fMediaSession, NULL, str);
}

bool RTSPClient::setMediaSessionParameter(MediaSession &session, char const *parameterName, char const *parameterValue) 
{
    char *cmd = NULL;

    do {
        if (fLastSessionId == NULL) {
            DPRINTF0((char *)NoSessionErr);
            break;
        }

        char *authenticatorStr = createAuthenticatorString(&fCurrentAuthenticator, "SET_PARAMETER", fBaseURL);

        char *const cmdFmt =
            "SET_PARAMETER %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "Session: %s\r\n"
            "%s"
            "%s"
            "Content-length: %d\r\n\r\n"
            "%s: %s\r\n\r\n";

        unsigned parameterNameLen = strlen(parameterName);
        unsigned parameterValueLen = strlen(parameterValue);
        unsigned cmdSize = strlen(cmdFmt)
            + strlen(fBaseURL)
            + 20
            + strlen(fLastSessionId)
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize
            + parameterNameLen + parameterValueLen;

        cmd = new char[cmdSize];
        sprintf(cmd, cmdFmt,
            fBaseURL,
            ++fCSeq,
            fLastSessionId,
            authenticatorStr,
            fUserAgentHeaderStr,
            parameterNameLen + parameterValueLen + 4,
            parameterName, parameterValue);
        delete[] authenticatorStr;

        if (!sendRequest(cmd, "SET_PARAMETER")) {
            break;
        }

        if (fTCPStreamIdCount == 0) {
            char *firstLine; char *nextLineStart;
            unsigned bytesRead; unsigned responseCode;

            if (!getResponse("SET_PARAMETER", bytesRead, responseCode, firstLine, nextLineStart)) {
                break;
            }
        }

        delete[] cmd;
        return true;
    } while (0);

    delete[] cmd;
    return false;
}

bool RTSPClient::getMediaSessionParameter(MediaSession &/*session*/, char const *parameterName, char *&parameterValue) 
{
    parameterValue = NULL;

    char *cmd = NULL;
    bool const haveParameterName = parameterName != NULL && parameterName[0] != '\0';

    do {
        if (fLastSessionId == NULL) {
            DPRINTF0((char *)NoSessionErr);
            break;
        }

        char *authenticatorStr = createAuthenticatorString(&fCurrentAuthenticator, "GET_PARAMETER", fBaseURL);

        if (haveParameterName) {
            char *const cmdFmt =
                "GET_PARAMETER %s RTSP/1.0\r\n"
                "CSeq: %d\r\n"
                "Session: %s\r\n"
                "%s"
                "%s"
                "Content-type: text/parameters\r\n"
                "Content-length: %d\r\n\r\n"
                "%s\r\n";

            unsigned parameterNameLen = strlen(parameterName);
            unsigned cmdSize = strlen(cmdFmt)
                + strlen(fBaseURL)
                + 20
                + strlen(fLastSessionId)
                + strlen(authenticatorStr)
                + fUserAgentHeaderStrSize
                + parameterNameLen;

            cmd = new char[cmdSize];
            sprintf(cmd, cmdFmt,
                fBaseURL,
                ++fCSeq,
                fLastSessionId,
                authenticatorStr,
                fUserAgentHeaderStr,
                parameterNameLen + 2,
                parameterName);
        } else {
            char *const cmdFmt =
                "GET_PARAMETER %s RTSP/1.0\r\n"
                "CSeq: %d\r\n"
                "Session: %s\r\n"
                "%s"
                "%s"
                "\r\n";

            unsigned cmdSize = strlen(cmdFmt)
                + strlen(fBaseURL)
                + 20
                + strlen(fLastSessionId)
                + strlen(authenticatorStr)
                + fUserAgentHeaderStrSize;

            cmd = new char[cmdSize];
            sprintf(cmd, cmdFmt,
                fBaseURL,
                ++fCSeq,
                fLastSessionId,
                authenticatorStr,
                fUserAgentHeaderStr);
        }

        delete[] authenticatorStr;

        if (!sendRequest(cmd, "GET_PARAMETER")) {
            break;
        }

        delete[] cmd;
        return true;
    } while (0);

    delete[] cmd;
    return false;
}

bool RTSPClient::teardownMediaSession(MediaSession &session)
{
    char *cmd = NULL;

    do {
        if (fLastSessionId == NULL) {
            DPRINTF0((char *)NoSessionErr);
            break;
        }

        char *authenticatorStr = createAuthenticatorString(&fCurrentAuthenticator, "TEARDOWN", fBaseURL);

        char const *sessURL = sessionURL(session);
        char *const cmdFmt =
            "TEARDOWN %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "Session: %s\r\n"
            "%s"
            "%s"
            "\r\n";

        unsigned cmdSize = strlen(cmdFmt)
            + strlen(sessURL)
            + 20
            + strlen(fLastSessionId)
            + strlen(authenticatorStr)
            + fUserAgentHeaderStrSize;

        cmd = new char[cmdSize];
        sprintf(cmd, cmdFmt,
            sessURL,
            ++fCSeq,
            fLastSessionId,
            authenticatorStr,
            fUserAgentHeaderStr);
        delete[] authenticatorStr;

        if (!sendRequest(cmd, "TEARDOWN")) {
            break;
        }

        delete[] cmd;
        return true;
    } while (0);

    delete[] cmd;
    return false;
}

void RTSPClient::closeURL()
{
    if (!fMediaSession) {
        return;
    }

    teardownMediaSession(*fMediaSession);	
    reset();
}
}
