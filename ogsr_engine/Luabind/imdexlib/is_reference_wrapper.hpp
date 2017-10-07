#pragma once

#include <functional>
#include <type_traits>

namespace imdexlib {

template <typename T>
struct is_reference_wrapper : std::false_type
{
};

template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
{
};

template <typename T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

} // imdexlib namespace
