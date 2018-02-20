////////////////////////////////////////////////////////////////////////////
//	Module 		: luabind_memory_allocator_inline.h
//	Created 	: 18.03.2007
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : luabind memory allocator template class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T>
luabind::memory_allocator<T>::memory_allocator										()
{
}

template <typename T>
luabind::memory_allocator<T>::memory_allocator										(self_type const&)
{
}

template <typename T>
template<class other>
luabind::memory_allocator<T>::memory_allocator										(memory_allocator<other> const&)
{
}

template <typename T>
template<class other>
luabind::memory_allocator<T> &luabind::memory_allocator<T>::operator=							(memory_allocator<other> const&)
{
	return			(*this);
}

template <typename T>
typename luabind::memory_allocator<T>::pointer luabind::memory_allocator<T>::address			(reference value) const
{
	return			(&value);
}

template <typename T>
typename luabind::memory_allocator<T>::const_pointer luabind::memory_allocator<T>::address		(const_reference value) const
{
	return			(&value);
}

template <typename T>
typename luabind::memory_allocator<T>::pointer luabind::memory_allocator<T>::allocate			(size_type const n, void const* const p) const
{
	pointer			result = (pointer)call_allocator(p,n*sizeof(T));
	if (!n)
		result		= (pointer)call_allocator(p,1*sizeof(T));

	return			(result);
}

template <typename T>
char *luabind::memory_allocator<T>::__charalloc										(size_type const n)
{
	return 			((char _FARQ *)allocate(n));
}

template <typename T>
void luabind::memory_allocator<T>::deallocate										(pointer const p, size_type const n) const
{
	call_allocator	(p,0);
}

template <typename T>
void luabind::memory_allocator<T>::deallocate										(void *p, size_type n) const
{
	call_allocator	(p,0);
}

template <typename T>
void luabind::memory_allocator<T>::construct										(pointer const p, T const& value)
{
	new(p) T		(value);
}

template <typename T>
void luabind::memory_allocator<T>::destroy											(pointer const p)
{
	p->~T			();
}

template <typename T>
typename luabind::memory_allocator<T>::size_type luabind::memory_allocator<T>::max_size			() const
{
	size_type		count = ((size_type)(-1))/sizeof(T);
	if (count)
		return		(count);

	return			(1);
}

namespace luabind {

template<class _0, class _1>
inline bool operator==	(memory_allocator<_0> const&, memory_allocator<_1> const&)
{
	return 			(true);
}

template<class _0, class _1>
inline bool operator!=	(memory_allocator<_0> const&, memory_allocator<_1> const&)
{
	return 			(false);
}

} // namespace luabind
