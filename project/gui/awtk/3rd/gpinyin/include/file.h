#ifndef TK_FILE_H
#define TK_FILE_H

#include "tkc/mem.h"

#ifdef ___BUILD_MODEL___
#include <stdio.h>
#else
#include "tkc/utils.h"
#include "tkc/rom_fs.h"
#define fopen rom_fopen
#define fread rom_fread
#define fwrite rom_fwrite
#define fclose rom_fclose
#define ftell rom_ftell
#define ferror rom_ferror
#define fseek rom_fseek
#define feof rom_feof
#define FILE rom_file_t
#endif /*___BUILD_MODEL___*/

#ifndef HAS_STDIO
#define HAS_STDIO 1
#endif

#ifndef LINUX
#define LINUX   1
#endif

#if !defined(WINDOWS) && !defined(LINUX) && !defined(APPLE) && !defined(HAS_STDIO)
#define printf
#define snprintf tk_snprintf
#endif

#endif /*TK_FILE_H*/
