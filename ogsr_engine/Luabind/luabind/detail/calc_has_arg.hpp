#pragma once

#include <luabind/detail/policy.hpp>

namespace luabind { namespace detail {

namespace private_detail {

constexpr int toInt(const bool a) noexcept
{
    return a ? 1 : 0;
}

template <int ResInit, int Offset, int Until, int Index = 0, int Result = ResInit>
struct calc_has_args
{
    template <typename... Policies>
    static constexpr int value() noexcept
    {
        using converter_policy = typename find_conversion_policy<Index + Offset, Policies...>::type;
        return calc_has_args<ResInit, Offset, Until, Index + 1, Result + toInt(converter_policy::has_arg)>::template value<Policies...>();
    }
};

template <int ResInit, int Offset, int Index, int Result>
struct calc_has_args<ResInit, Offset, Index, Index, Result>
{
    template <typename...>
    static constexpr int value() noexcept
    {
        return Result;
    }
};

} // private_detail namespace

template <int ResInit, int Offset, int Until, typename... Policies>
constexpr int calcHasArg() noexcept
{
    return private_detail::calc_has_args<ResInit, Offset, Until>::template value<Policies...>();
}

}}
