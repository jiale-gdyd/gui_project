#ifndef RTSP_INTERNAL_OUR_MD5_H
#define RTSP_INTERNAL_OUR_MD5_H

namespace rtsp {
typedef unsigned UNSIGNED32;

#undef _ANSI_ARGS_
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#define _ANSI_ARGS_(x)      x
#else
#define _ANSI_ARGS_(x)      ()
#endif

#ifdef __cplusplus
#define EXTERN              extern "C"
#else
#define EXTERN              extern
#endif

typedef struct MD5Context {
    UNSIGNED32    state[4];
    UNSIGNED32    count[2];
    unsigned char buffer[64];
} MD5_CTX;

EXTERN void our_MD5Init(MD5_CTX *);
EXTERN void ourMD5Update(MD5_CTX *, const unsigned char *, unsigned int);
EXTERN void our_MD5Pad(MD5_CTX *);
EXTERN void our_MD5Final(unsigned char [16], MD5_CTX *);
EXTERN char *our_MD5End(MD5_CTX *, char *);
EXTERN char *our_MD5File(const char *, char *);
EXTERN char *our_MD5Data(const unsigned char *, unsigned int, char *);
}

#endif
