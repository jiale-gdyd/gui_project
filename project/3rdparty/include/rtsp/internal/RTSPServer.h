#ifndef RTSP_INTERNAL_RTSP_SERVER_H
#define RTSP_INTERNAL_RTSP_SERVER_H

#include "List.h"
#include "Sock.h"
#include "RTSPCommon.h"
#include "TaskScheduler.h"

namespace rtsp {
#define RTSP_BUFFER_SIZE        (20000)

typedef enum {
    OPEN_SERVER_SESSION,
    CLIENT_CONNECTED,
    CLIENT_DISCONNECTED
} ServerCallbackType;

class CallbackParam {
public:
    CallbackParam(ServerCallbackType type);
    virtual ~CallbackParam() {}

public:
    ServerCallbackType fType;
};

class OpenServerSessionParam : public CallbackParam {
public:
    OpenServerSessionParam(char *requestString);
    virtual ~OpenServerSessionParam() {}

public:
    char fRequestString[256];
};

class ClientConnectedParam : public CallbackParam {
public:
    ClientConnectedParam(const char *sessionName, int sock, struct sockaddr_in &sockAddr, int streamType);
    virtual ~ClientConnectedParam() {}

public:
    char               fSessionName[256];
    int                fSock;
    struct sockaddr_in fClientAddr;
    int                fStreamType;
};

class ClientDisconnectedParam : public CallbackParam {
public:
    ClientDisconnectedParam(int sock);
    virtual ~ClientDisconnectedParam() {}

public:
    int fSock;
};

typedef int (*RTSPServerCallback)(void *arg, CallbackParam *param);

class ClientSocket;
class ServerMediaSession;
class ServerMediaSubsession;

class RTSPServer {
public:
    static RTSPServer *instance();
    static void destroy();

    int startServer(unsigned short port = 554, RTSPServerCallback func = NULL, void *arg = NULL);
    void stopServer();

    bool isServerRunning() {
        return fIsServerRunning;
    }

    int serverSessionCount() {
        return fServerMediaSessions.count();
    }

    void addServerMediaSession(ServerMediaSession *serverMediaSession);
    ServerMediaSession *lookupServerMediaSession(char const *streamName);
    void removeServerMediaSession(ServerMediaSession *serverMediaSession);

    void closeAllClientSessionsForServerMediaSession(ServerMediaSession *serverMediaSession);
    void deleteServerMediaSession(ServerMediaSession *serverMediaSession);

    char *rtspURL(ServerMediaSession const *serverMediaSession, int clientSocket = -1);
    char *rtspURLPrefix(int clientSocket = -1);

protected:
    RTSPServer();
    virtual ~RTSPServer();

    static RTSPServer *fInstance;

    virtual char const *allowedCommandNames();

protected:
    static void incomingConnectionHandlerRTSP(void *, int);
    void incomingConnectionHandlerRTSP1();
    void incomingConnectionHandler(int serverSocket);

protected:
    class RTSPClientSession {
    public:
        RTSPClientSession(RTSPServer &ourServer, MySock &clientSock, unsigned int sessionId);
        virtual ~RTSPClientSession();

        friend class RTSPServer;

    protected:
        static void incomingRequestHandler(void *, int);
        void incomingRequestHandler1(unsigned char requestByte);
        void handleRequestBytes(int newBytesRead);

        void resetRequestBuffer();

        void handleCmd_OPTIONS();
        void handleCmd_DESCRIBE(char const *urlPreSuffix, char const *urlSuffix, char const *fullRequestStr);
        void handleCmd_SETUP(char const *urlPreSuffix, char const *urlSuffix, char const *fullRequestStr);
        void handleCmd_PLAY(ServerMediaSubsession *subsession, char const *fullRequestStr);
        void handleCmd_TEARDOWN(ServerMediaSubsession *subsession);
        void handleCmd_PAUSE(ServerMediaSubsession *subsession);
        void handleCmd_GET_PARAMETER(ServerMediaSubsession *subsession, char const *fullRequestStr);
        void handleCmd_SET_PARAMETER(ServerMediaSubsession *subsession, char const *fullRequestStr);
        void handleCmd_withinSession(char const *cmdName, char const *urlPreSuffix, char const *urlSuffix, char const *fullRequestStr);
        void handleCmd_bad();
        void handleCmd_notSupported();
        void handleCmd_notFound();
        void handleCmd_sessionNotFound();
        void handleCmd_unsupportedTransport();

        void reclaimStreamStates();
        void shutdown();

        void setRTSPResponse(char const *responseStr);
        void setRTSPResponse(char const *responseStr, unsigned int sessionId);
        void setRTSPResponse(char const *responseStr, char const *contentStr);
        void setRTSPResponse(char const *responseStr, uint32_t sessionId, char const *contentStr);

        void tcpReadHandler1();
        bool lookupStreamChannelId(unsigned char channel);
        void readRTPOverTCP();

    protected:
        enum {
            AWAITING_DOLLAR,
            AWAITING_STREAM_CHANNEL_ID,
            AWAITING_SIZE1,
            AWAITING_SIZE2,
            AWAITING_PACKET_DATA,
        } fTCPReadingState;

        unsigned char fStreamChannelId, fSizeByte1;
        unsigned      fTCPReadSize;
        char          *fRtpBuffer;
        int           fRtpBufferSize;
        int           fRtpBufferIdx;

    protected:
        RTSPServer         &fOurServer;
        uint32_t           fOurSessionId;
        ServerMediaSession *fOurServerMediaSession;
        bool               fIsActive;
        MySock             *fClientSock;
        unsigned char      fRequestBuffer[RTSP_BUFFER_SIZE];
        unsigned           fRequestBytesAlreadySeen, fRequestBufferBytesLeft;
        unsigned char      *fLastCRLF;
        unsigned char      fResponseBuffer[RTSP_BUFFER_SIZE];
        char const         *fCurrentCSeq;
        unsigned           fBase64RemainderCount;

        bool               fIsMulticast;
        unsigned char      fTCPStreamIdCount;

        unsigned           fNumStreamStates;
        struct streamState {
            ServerMediaSubsession *subsession;
        }  *fStreamStates;

        MyList<ClientSocket> fClientSockList;
    };

    RTSPClientSession *createNewClientSession(MySock &clientSock);
    void addClientSession(RTSPClientSession *clientSession);
    void removeClientSession(RTSPClientSession *clientSession);

protected:
    friend class RTSPClientSession;

protected:
    bool                       fIsServerRunning;
    unsigned short             fServerPort;

    RTSPServerCallback         fServerCallbackFunc;
    void                       *fServerCallbackArg;

    MySock                     fServerSock;
    TaskScheduler              *fTask;

    MyList<ServerMediaSession> fServerMediaSessions;
    MyList<RTSPClientSession>  fClientSessions;
};
}

#endif
