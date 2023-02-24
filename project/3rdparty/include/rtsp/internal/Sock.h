#ifndef __MY_SOCK_H__
#define __MY_SOCK_H__

#include "Mutex.h"
#include "SockCommon.h"

namespace rtsp {
class MySock {
public:
    MySock();
    ~MySock();

    int sock() {
        return fSock;
    }

    unsigned short port() {
        return fPort;
    }

    struct sockaddr_in &clientAddress() {
        return fClientAddr;
    }

    bool isOpened() {
        return fSock != -1;
    }

    int setupStreamSock(short port, int makeNonBlocking);
    int setupDatagramSock(short port, int makeNonBlocking);
    int setupServerSock(short port, int makeNonBlocking);
    int setupClientSock(int serverSock, int makeNonBlocking);

    void closeSock();
    void shutdown();

    int makeTCP_NoDelay() {
        return rtsp::makeTCP_NoDelay(fSock);
    }

    unsigned setSendBufferTo(unsigned requestedSize) {
        return rtsp::setSendBufferTo(fSock, requestedSize);
    }

    unsigned setReceiveBufferTo(unsigned requestedSize) {
        return rtsp::setReceiveBufferTo(fSock, requestedSize);
    }

    unsigned getSendBufferSize() {
        return rtsp::getSendBufferSize(fSock);
    }

    unsigned getReceiveBufferSize() {
        return rtsp::getReceiveBufferSize(fSock);
    }

    int readSocket1(char *buffer, unsigned bufferSize, struct sockaddr_in &fromAddress) {
        return rtsp::readSocket1(fSock, buffer, bufferSize, fromAddress);
    }

    int readSocket(char *buffer, unsigned bufferSize, struct sockaddr_in &fromAddress, struct timeval *timeout = NULL) {
        return rtsp::readSocket(fSock, buffer, bufferSize, fromAddress, timeout);
    }

    int readSocketExact(char *buffer, unsigned bufferSize, struct sockaddr_in &fromAddress, struct timeval *timeout = NULL) {
        return rtsp::readSocketExact(fSock, buffer, bufferSize, fromAddress, timeout);
    }

    int writeSocket(char *buffer, unsigned bufferSize);
    int writeSocket(char *buffer, unsigned bufferSize, struct sockaddr_in &toAddress);
    int sendRTPOverTCP(char *buffer, int len, unsigned char streamChannelId);

    bool joinGroupSSM(unsigned int groupAddress, unsigned int sourceFilterAddr);
    bool leaveGroupSSM(unsigned int groupAddress, unsigned int sourceFilterAddr);
    bool joinGroup(unsigned int groupAddress);
    bool leaveGroup(unsigned int groupAddress);
    bool changePort(short port);
    void changeDestination(struct in_addr const& newDestAddr, short newDestPort);

protected:
    int                fSock;
    unsigned short     fPort;

    struct sockaddr_in fClientAddr;

    bool               fIsSSM;
    unsigned int       fGroupAddress;
    unsigned int       fSourceFilterAddr;

    pthread_mutex_t    fMutex;
};
}

#endif
