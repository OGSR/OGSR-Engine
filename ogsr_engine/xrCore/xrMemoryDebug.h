#pragma once

struct PointerInfo
{
public:
    std::string identity;
    unsigned int line{};
    bool is_class{};
    bool is_contaner{};
    size_t size{};
};

template <typename T>
struct PointerAllocator
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

    pointer allocate(const size_type n, const void* = nullptr) const
    {
        size_t size = sizeof(T) * n;
        return (pointer)malloc(size);
    }

    void deallocate(pointer p, const size_type) const { free(p); }

    void construct(pointer p, const T& val) { new (p) T(val); }

    void destroy(pointer p) { p->~T(); }
};

void PointerRegistryAdd(void* ptr, PointerInfo&& info);
void PointerRegistryRelease(const void* ptr, const std::source_location& loc, bool is_class = false);
void PointerRegistryClear();
void PointerRegistryDump(float thresholdInKb = 1.f);
void PointerRegistryInfo();
