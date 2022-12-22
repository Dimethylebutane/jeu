#pragma once
//https://rigtorp.se/spinlock/
//copy paste of the interet

#include <atomic>
#include <thread>

#include "SpinFlag.hpp"


//Spinlock with thread ID check -> a thread can lock multiple time the same lock without bug
struct Spinlock_MT {
    std::atomic<bool> lock_{ 0 };
    std::atomic<std::thread::id> idPossessor;

    Spinlock_MT() : idPossessor(std::this_thread::get_id())
    {}

    void lock() noexcept {
        //if lock owned by current thread -> safe
        if (lock_.load(std::memory_order_relaxed) && std::this_thread::get_id() == idPossessor)
            return;
        //else
        for (;;) {
            // Optimistically assume the lock is free on the first try
            //set lock_ to true and return precedet value
            if (!lock_.exchange(true, std::memory_order_acquire)) 
            {
                //the lock is possessed by thread id....
                idPossessor.store(std::this_thread::get_id());
                return;
            }
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

struct SpinLockMTGuard
{
private:
    Spinlock_MT& lock;
public:
    SpinLockMTGuard(Spinlock_MT& loc) : lock(loc) { lock.lock(); }
    ~SpinLockMTGuard() { lock.unlock(); }
};