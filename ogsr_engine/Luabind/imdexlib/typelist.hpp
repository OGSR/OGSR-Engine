#pragma once

namespace imdexlib {

template <typename... Ts>
struct typelist
{
    static constexpr size_t size() noexcept
    {
        return sizeof...(Ts);
    }
};

} // imdexlib
