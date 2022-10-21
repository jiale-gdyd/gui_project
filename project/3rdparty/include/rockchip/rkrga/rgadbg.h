#ifndef RKRGA_RGADBG_H
#define RKRGA_RGADBG_H

#ifndef rga_print
#define rga_print(msg, ...)             fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef rga_error
#define rga_error(msg, ...)             rga_print("\033[1;31m[RGA][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef rga_warn
#define rga_warn(msg, ...)              rga_print("\033[1;33m[RGA][W]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef rga_info
#define rga_info(msg, ...)              rga_print("\033[1;32m[RGA][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef rga_debug
#define rga_debug(msg, ...)             rga_print("\033[1;34m[RGA][D]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#endif
