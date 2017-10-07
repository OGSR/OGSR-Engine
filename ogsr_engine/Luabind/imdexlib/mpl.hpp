#pragma once

#include <type_traits>

namespace imdexlib {

class end_t {};

template <typename Predicate, typename... Ts>
struct find_if;

template <typename Predicate, typename T, typename... Ts>
struct find_if<Predicate, T, Ts...>
{
private:
    using result = typename Predicate::template type<T>;
public:
    using type = std::conditional_t<
        result::value,
        T,
        typename find_if<Predicate, Ts...>::type
    >;
};

template <typename Predicate, typename T>
struct find_if<Predicate, T>
{
private:
    using result = typename Predicate::template type<T>;
public:
    using type = std::conditional_t<
        result::value,
        T,
        end_t
    >;
};

template <typename Predicate>
struct find_if<Predicate>
{
    using type = end_t;
};

template <typename Predicate, typename... Ts>
using find_if_t = typename find_if<Predicate, Ts...>::type;

} // imdexlib namespace
