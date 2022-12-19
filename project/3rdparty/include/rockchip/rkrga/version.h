#ifndef RKRGA_VERSION_H
#define RKRGA_VERSION_H

#define STR_HELPER(x)               #x
#define STR(x)                      STR_HELPER(x)

#define RGA_API_MAJOR_VERSION       1
#define RGA_API_MINOR_VERSION       3
#define RGA_API_REVISION_VERSION    2
#define RGA_API_BUILD_VERSION       0

#define RGA_API_VERSION             STR(RGA_API_MAJOR_VERSION) "." STR(RGA_API_MINOR_VERSION) "." STR(RGA_API_REVISION_VERSION) "_[" STR(RGA_API_BUILD_VERSION) "]"

#define RGA_API_PRODUCT_BASE        "rv1126"
#define RGA_API_FULL_VERSION        "rga_api version " RGA_API_VERSION " (" "base: " RGA_API_PRODUCT_BASE ")"

#endif
