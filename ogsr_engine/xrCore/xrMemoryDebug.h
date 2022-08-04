#pragma once

template <typename T>
class PointerAllocator
{
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    pointer address(reference ref) const { return &ref; }
    const_pointer address(const_reference ref) const { return &ref; }

    PointerAllocator() = default;
    PointerAllocator(const PointerAllocator<T>&) = default;

    template <class Other>
    PointerAllocator(const PointerAllocator<Other>&)
    {}

    template <class Other>
    PointerAllocator& operator=(const PointerAllocator<Other>&)
    {
        return *this;
    }
#pragma warning(push)
#pragma warning(disable : 4267)
    pointer allocate(const size_type n, const void* p = nullptr) const
    {
        size_t MemoryRequired = sizeof(T) * n;
        return (pointer)malloc(MemoryRequired);
    }
#pragma warning(pop)

    void deallocate(pointer p, const size_type) const { free(p); }

    void deallocate(void* p, const size_type) const { free(p); }

    void construct(pointer p, const T& _Val) { new (p) T(_Val); }

    void destroy(pointer p) { p->~T(); }
};

void RegisterPointer(void* ptr);
void UnregisterPointer(void* ptr);
