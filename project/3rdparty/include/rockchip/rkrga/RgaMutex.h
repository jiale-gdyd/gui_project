#ifndef RKRGA_RGAMUTEX_H
#define RKRGA_RGAMUTEX_H

#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>

#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)        __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)
#endif

#define CAPABILITY(x)                           THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY                       THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x)                           THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x)                        THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...)                    THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...)                     THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...)                           THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...)                    THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...)                            THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...)                     THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...)                            THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...)                     THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...)                        THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...)                 THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...)                           THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x)                    THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x)             THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x)                    THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS               THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

class Condition;

class CAPABILITY("mutex") Mutex {
public:
    enum {
        PRIVATE = 0,
        SHARED  = 1
    };

    Mutex();
    explicit Mutex(const char *name);
    explicit Mutex(int type, const char *name = nullptr);
    ~Mutex();

    int32_t lock() ACQUIRE();
    void unlock() RELEASE();

    int32_t tryLock() TRY_ACQUIRE(0);
    int32_t timedLock(int64_t timeoutNs) TRY_ACQUIRE(0);

    class SCOPED_CAPABILITY Autolock {
    public:
        inline explicit Autolock(Mutex& mutex) ACQUIRE(mutex) : mLock(mutex) {
            mLock.lock();
        }

        inline explicit Autolock(Mutex* mutex) ACQUIRE(mutex) : mLock(*mutex) {
            mLock.lock();
        }

        inline ~Autolock() RELEASE() {
            mLock.unlock();
        }

    private:
        Mutex &mLock;

        Autolock(const Autolock &);
        Autolock &operator=(const Autolock &);
    };

private:
    friend class Condition;

    Mutex(const Mutex &);
    Mutex &operator=(const Mutex &);

    pthread_mutex_t mMutex;
};

inline Mutex::Mutex()
{
    pthread_mutex_init(&mMutex, nullptr);
}

inline Mutex::Mutex(__attribute__((unused)) const char *name)
{
    pthread_mutex_init(&mMutex, nullptr);
}

inline Mutex::Mutex(int type, __attribute__((unused)) const char *name)
{
    if (type == SHARED) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&mMutex, &attr);
        pthread_mutexattr_destroy(&attr);
    } else {
        pthread_mutex_init(&mMutex, nullptr);
    }
}

inline Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

inline int32_t Mutex::lock()
{
    return -pthread_mutex_lock(&mMutex);
}

inline void Mutex::unlock()
{
    pthread_mutex_unlock(&mMutex);
}

inline int32_t Mutex::tryLock()
{
    return -pthread_mutex_trylock(&mMutex);
}

inline int32_t Mutex::timedLock(int64_t timeoutNs)
{
    timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    timeoutNs += now.tv_sec * 1000000000 + now.tv_nsec;

    const struct timespec ts = {
        static_cast<time_t>(timeoutNs / 1000000000),
        static_cast<long>(timeoutNs % 1000000000),
    };

    return -pthread_mutex_timedlock(&mMutex, &ts);
}

typedef Mutex::Autolock AutoMutex;

#endif
