#pragma once
//https://rigtorp.se/spinlock/
//copy paste of the interet

#include <atomic>
#include <thread>

//Spinlock
struct Spinlock {
    std::atomic<bool> lock_{ 0 };

    Spinlock()
    {}

    void lock() noexcept {
        //if lock owned by current thread -> safe
        if (lock_.load(std::memory_order_relaxed))
            return;
        //else
        for (;;) {
            // Optimistically assume the lock is free on the first try
            //set lock_ to true and return precedet value
            if (!lock_.exchange(true, std::memory_order_acquire))
                return;

            // Wait for lock to be released without generating cache misses
            while (lock_.load(std::memory_order_relaxed)) {
                //Issue X86 PAUSE or ARM YIELD instruction to reduce contention between hyper-threads
#ifndef __unix__ //win
                _mm_pause();
#else   //unix
                __builtin_ia32_pause();
#endif //unix / windows
            }
        }
    }

    //Tries to lock the mutex. Returns immediately. 
    //On successful lock acquisition returns true, otherwise returns false
    bool try_lock() noexcept {
        // First do a relaxed load to check if lock is free in order to prevent
        // unnecessary cache misses if someone does while(!try_lock())
        return !lock_.load(std::memory_order_relaxed) &&
            !lock_.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept {
        lock_.store(false, std::memory_order_release);
    }
};

struct SpinlockGuard
{
private:
    Spinlock& lock;
public:
    SpinlockGuard(Spinlock& loc) : lock(loc) { lock.lock(); }
    ~SpinlockGuard() { lock.unlock(); }
};