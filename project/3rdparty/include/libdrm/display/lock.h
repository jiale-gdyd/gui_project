#ifndef LIBDRM_DISPLAY_LOCK_H
#define LIBDRM_DISPLAY_LOCK_H

#include <assert.h>
#include <pthread.h>

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace libdrm {
class LockMutex {
public:
    LockMutex();
    virtual ~LockMutex();

    virtual void lock() = 0;
    virtual void unlock() = 0;

    virtual void wait() {};
    virtual void notify() {};

    void locktimeinc();
    void locktimedec();

protected:
    std::atomic_int lock_times;
};

class NonLockMutex : public LockMutex {
public:
    virtual ~NonLockMutex() = default;

    virtual void lock() {}
    virtual void unlock() {}
};

class ConditionLockMutex : public LockMutex {
public:
    virtual ~ConditionLockMutex() = default;

    virtual void lock() override;
    virtual void unlock() override;

    virtual void wait() override;
    virtual void notify() override;

private:
    std::mutex                  mtx;
    std::condition_variable_any cond;
};

class ReadWriteLockMutex : public LockMutex {
public:
    ReadWriteLockMutex();
    virtual ~ReadWriteLockMutex();

    virtual void lock() override;
    virtual void unlock() override;
    void read_lock();

    bool valid;

private:
    pthread_rwlock_t rwlock;
};

class SpinLockMutex : public LockMutex {
public:
    SpinLockMutex();
    virtual ~SpinLockMutex() = default;

    SpinLockMutex(const SpinLockMutex &) = delete;
    SpinLockMutex &operator=(const SpinLockMutex &) = delete;

    virtual void lock() override;
    virtual void unlock() override;

private:
    std::atomic_flag flag;
};

class AutoLockMutex {
public:
    AutoLockMutex(LockMutex &lm) : m_lm(lm) {
        m_lm.lock();
    }

    ~AutoLockMutex() {
        m_lm.unlock();
    }

private:
    LockMutex &m_lm;
};
}

#endif
