#ifndef RLOTTIE_CONFIG_H
#define RLOTTIE_CONFIG_H

#define LOTTIE_CACHE
#define LOTTIE_THREAD

#ifdef LOTTIE_THREAD
#define LOTTIE_THREAD_SUPPORT
#endif

#ifdef LOTTIE_CACHE
#define LOTTIE_CACHE_SUPPORT
#endif

#endif
