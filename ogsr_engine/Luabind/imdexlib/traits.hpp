#pragma once

#include <type_traits>

namespace imdexlib {

template <typename T>
using remove_cvr = std::remove_cv<std::remove_reference_t<T>>;

template <typename T>
using remove_cvr_t = typename remove_cvr<T>::type;

namespace detail {

template <typename T, typename U>
struct is_comparable_to final
{
    template <typename A, typename B>
    static auto check(A&& a, B&& b) -> decltype(a == b, std::true_type{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<U>()));
};

} // detail namespace

template <typename T, typename U>
using is_comparable_to = typename detail::is_comparable_to<T, U>::result;

template <typename T, typename U>
constexpr bool is_comparable_to_v = is_comparable_to<T, U>::value;

template <typename T>
using is_comparable = is_comparable_to<T, T>;

template <typename T>
constexpr bool is_comparable_v = is_comparable<T>::value;

namespace detail {

template <typename T, typename U>
struct is_nothrow_comparable_to final
{
    template <typename A, typename B>
    static auto check(A&& a, B&& b) -> decltype(std::bool_constant<noexcept(a == b)>{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<U>()));
};

} // detail namespace

template <typename T, typename U>
using is_nothrow_comparable_to = typename detail::is_nothrow_comparable_to<T, U>::result;

template <typename T, typename U>
constexpr bool is_nothrow_comparable_to_v = is_nothrow_comparable_to<T, U>::value;

template <typename T>
using is_nothrow_comparable = is_nothrow_comparable_to<T, T>;

template <typename T>
constexpr bool is_nothrow_comparable_v = is_nothrow_comparable<T>::value;

} // imdexlib namespace
