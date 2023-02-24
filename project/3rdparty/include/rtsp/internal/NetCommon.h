#ifndef RTSP_INTERNAL_NETCOMMON_H
#define RTSP_INTERNAL_NETCOMMON_H

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

namespace rtsp {
#define closeSocket             close
#define WSAGetLastError()       errno
#define _strcasecmp             strncasecmp
}

#endif
