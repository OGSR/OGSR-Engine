#pragma once

namespace imdexlib {

template <typename T>
struct identity
{
    using type = T;
};

template <typename T>
using identity_t = typename identity<T>::type;

} // imdexlib namespace
