#pragma once

template <typename C, typename R, typename... A>
constexpr decltype(auto) cdecl_cast(const C& c, R(C::* f)(A...) const)
{
	return static_cast<R(__cdecl*)(A...)>(c);
}
