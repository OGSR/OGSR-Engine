#include "stdafx.h"
#include "Event.hpp"

Event::Event() noexcept { handle = (void*)CreateEvent(nullptr, FALSE, FALSE, nullptr); }
Event::~Event() noexcept { CloseHandle(handle); }
void Event::Reset() noexcept { ResetEvent(handle); }
void Event::Set() noexcept { SetEvent(handle); }
void Event::Wait() noexcept { WaitForSingleObject(handle, INFINITE); }
bool Event::Wait(u32 millisecondsTimeout) noexcept { return WaitForSingleObject(handle, millisecondsTimeout) != WAIT_TIMEOUT; }
void Event::WaitEx(u32 millisecondsTimeout) noexcept
{
    DWORD WaitResult = WAIT_TIMEOUT;
    do
    {
        WaitResult = WaitForSingleObject(handle, millisecondsTimeout);
        if (WaitResult == WAIT_TIMEOUT)
        {
            MSG msg;
            if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    } while (WaitResult == WAIT_TIMEOUT);
}
