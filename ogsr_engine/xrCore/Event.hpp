#pragma once

class XRCORE_API Event
{
    void* handle;

public:
    Event() noexcept;
    ~Event() noexcept;

    // Reset the event to the unsignalled state.
    void Reset() noexcept;
    // Set the event to the signalled state.
    void Set() noexcept;
    // Wait indefinitely for the object to become signalled.
    void Wait() noexcept;
    // Wait, with a time limit, for the object to become signalled.
    bool Wait(u32 millisecondsTimeout) noexcept;
    // Expected thread can issue a exception. But it require to process one message from HWND message queue, otherwise, thread can't show error message
    void WaitEx(u32 millisecondsTimeout) noexcept;

    void* GetHandle() noexcept { return handle; }
};
