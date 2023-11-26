#pragma once

// KRodin: Теперь это просто обёртка над std::recursive_mutex, т.к. мне лень кучу кода переделывать.

//#define PROFILE_CRITICAL_SECTIONS // TODO: Порезать этот трэш везде.

class xrCriticalSection final : std::recursive_mutex
{
public:
    inline void Enter() { __super::lock(); }
    inline void Leave() { __super::unlock(); }
    inline bool TryEnter() { return __super::try_lock(); }
};

class SpinLock
{
    std::atomic_flag locked = ATOMIC_FLAG_INIT;

public:
    void lock()
    {
        while (locked.test_and_set(std::memory_order_acquire))
        {
            ;
        }
    }
    void unlock() { locked.clear(std::memory_order_release); }
};
