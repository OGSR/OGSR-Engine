#ifdef DEBUG_MEMORY_NAME

template <typename T, typename... Args>
T* xr_new(Args&&... args) {
	T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T), typeid(T).name()));
	return new (ptr) T(std::forward<Args>(args)...);
}

#else // DEBUG_MEMORY_NAME

template <typename T, typename... Args>
T* xr_new(Args&&... args) {
	T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
	return new (ptr) T(std::forward<Args>(args)...);
}

#endif // DEBUG_MEMORY_NAME


template <bool _is_pm, typename T>
struct xr_special_free
{
	IC void operator()(T* &ptr)
	{
		void*	_real_ptr	= dynamic_cast<void*>(ptr);
		ptr->~T			();
		Memory.mem_free	(_real_ptr);
	}
};

template <typename T>
struct xr_special_free<false,T>
{
	IC void operator()(T* &ptr)
	{
		ptr->~T			();
		Memory.mem_free	(ptr);
	}
};

template <class T>
IC	void	xr_delete	(T* &ptr)
{
	if (ptr) 
	{
		xr_special_free<std::is_polymorphic_v<T>, T>()(ptr);
		ptr = NULL;
	}
}

#ifdef DEBUG_MEMORY_MANAGER
	void XRCORE_API mem_alloc_gather_stats				(const bool &value);
	void XRCORE_API mem_alloc_gather_stats_frequency	(const float &value);
	void XRCORE_API mem_alloc_show_stats				();
	void XRCORE_API mem_alloc_clear_stats				();
#endif // DEBUG_MEMORY_MANAGER