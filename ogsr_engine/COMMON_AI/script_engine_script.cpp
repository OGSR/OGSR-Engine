////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_script.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator script engine export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ai_space.h"
#include "../../xr_3da/xr_input.h"

struct profile_timer_script
{
    using Clock = std::chrono::high_resolution_clock;
    using Time = Clock::time_point;
    using Duration = Clock::duration;

    Time start_time;
    Duration accumulator;
    u64 count = 0;
    int recurse_mark = 0;

    profile_timer_script() : start_time(), accumulator(), count(0), recurse_mark(0) {}

    bool operator<(const profile_timer_script& profile_timer) const { return accumulator < profile_timer.accumulator; }

    void start()
    {
        if (recurse_mark)
        {
            ++recurse_mark;
            return;
        }

        ++recurse_mark;
        ++count;
        start_time = Clock::now();
    }

    void stop()
    {
        --recurse_mark;

        if (recurse_mark)
            return;

        const auto finish = Clock::now();
        if (finish > start_time)
        {
            accumulator += finish - start_time;
        }
    }

    decltype(auto) time() const
    {
        using namespace std::chrono;
        return duration_cast<microseconds>(accumulator).count();
    }
};

inline profile_timer_script operator+(const profile_timer_script& portion0, const profile_timer_script& portion1)
{
    profile_timer_script result;
    result.accumulator = portion0.accumulator + portion1.accumulator;
    result.count = portion0.count + portion1.count;
    return result;
}

void msg_and_fail(LPCSTR msg)
{
    Msg(msg);
    R_ASSERT(false);
}

void take_screenshot(IRender_interface::ScreenshotMode mode, LPCSTR name) { ::Render->Screenshot(mode, name); }

bool GetShift() { return !!pInput->iGetAsyncKeyState(DIK_LSHIFT) || !!pInput->iGetAsyncKeyState(DIK_RSHIFT); }
bool GetLAlt() { return !!pInput->iGetAsyncKeyState(DIK_LMENU); }
bool GetRAlt() { return !!pInput->iGetAsyncKeyState(DIK_RMENU); }
bool GetAlt() { return !!pInput->iGetAsyncKeyState(DIK_LMENU) || !!pInput->iGetAsyncKeyState(DIK_RMENU); }

using namespace luabind;
#pragma optimize("s", on)

void CScriptEngine::script_register(lua_State* L)
{
    module(
        L)[def("log1", (void (*)(LPCSTR)) & Log), def("fail", &msg_and_fail),

           def("screenshot", &take_screenshot),
           class_<enum_exporter<IRender_interface::ScreenshotMode>>("screenshot_modes")
               .enum_(
                   "modes")[value("normal", int(IRender_interface::ScreenshotMode::SM_NORMAL)), value("cubemap", int(IRender_interface::ScreenshotMode::SM_FOR_CUBEMAP)),
                            value("gamesave", int(IRender_interface::ScreenshotMode::SM_FOR_GAMESAVE)), value("levelmap", int(IRender_interface::ScreenshotMode::SM_FOR_LEVELMAP))],
           class_<profile_timer_script>("profile_timer")
               .def(constructor<>())
               .def(constructor<profile_timer_script&>())
               .def(const_self + profile_timer_script())
               .def(const_self < profile_timer_script())
               .def("start", &profile_timer_script::start)
               .def("stop", &profile_timer_script::stop)
               .def("time", &profile_timer_script::time),
           def("prefetch", [](const char* file_name) { ai().script_engine().process_file(file_name); }), def("editor", [] { return false; }),

           def("bit_and", [](const int i, const int j) { return i & j; }), def("bit_or", [](const int i, const int j) { return i | j; }),
           def("bit_xor", [](const int i, const int j) { return i ^ j; }), def("bit_not", [](const int i) { return ~i; }),

           def("user_name", [] { return Core.UserName; }), def("time_global", [] { return Device.dwTimeGlobal; }),

           def("GetShift", &GetShift), def("GetLAlt", &GetLAlt), def("GetRAlt", &GetRAlt), def("GetAlt", &GetAlt),

           def("device", [] { return &Device; }), def("__debugbreak", [] { __debugbreak(); })];
}
