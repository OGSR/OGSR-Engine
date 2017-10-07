#pragma once

#include <type_traits>

namespace luabind { namespace detail {

template <typename... Ts>
struct policy_cons
{
    template <typename... Us>
    policy_cons<Ts..., Us...> operator+ (const policy_cons<Us...>&) const noexcept
    {
        return policy_cons<Ts..., Us...>();
    }

    template <typename... Us>
    policy_cons<Ts..., Us...> operator| (const policy_cons<Us...>&) const noexcept
    {
        return policy_cons<Ts..., Us...>();
    }

    template <typename... Us>
    policy_cons<Ts..., Us...> operator, (const policy_cons<Us...>&) const noexcept
    {
        return policy_cons<Ts..., Us...>();
    }
};

template<typename T>
struct is_policy_cons : public std::false_type
{
};

template<typename... Ts>
struct is_policy_cons<policy_cons<Ts...>> : public std::true_type
{
};

}}
