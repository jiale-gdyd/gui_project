#ifndef RTSP_SIMPLE_RTSP_SERVER_COMMON_H
#define RTSP_SIMPLE_RTSP_SERVER_COMMON_H

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef simple_rtsp_server_print
#define simple_rtsp_server_print(msg, ...)                 fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef simple_rtsp_server_error
#define simple_rtsp_server_error(msg, ...)                 simple_rtsp_server_print("\033[1;31m[RTSP_SERVER][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef simple_rtsp_server_warn
#define simple_rtsp_server_warn(msg, ...)                  simple_rtsp_server_print("\033[1;33m[RTSP_SERVER][W]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef simple_rtsp_server_info
#define simple_rtsp_server_info(msg, ...)                  simple_rtsp_server_print("\033[1;32m[RTSP_SERVER][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef simple_rtsp_server_debug
#define simple_rtsp_server_debug(msg, ...)                 simple_rtsp_server_print("\033[1;34m[RTSP_SERVER][D]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#endif

