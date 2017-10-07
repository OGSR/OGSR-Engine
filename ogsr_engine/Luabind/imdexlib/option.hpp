#pragma once

#include <cassert>

#include "utility.hpp"
#include "traits.hpp"

namespace imdexlib {

struct none_t final {};

template <typename T>
class option final
{
    static_assert(!std::is_const_v<T>, "Optional type shouldn't be const. Use 'const option<T>' declaration instead.");
    static_assert(!std::is_reference_v<T>, "Optional type shouldn't be reference.");
    static_assert(!std::is_pointer_v<T>, "Optional type shouldn't be pointer.");
    static_assert(std::is_destructible_v<T>, "Optional type should be destructible");
public:

    option() noexcept
        : storage(),
          empty(true)
    {
    }

    explicit option(const none_t&) noexcept
        : option()
    {
    }

    explicit option(const T& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : storage(),
          empty(false)
    {
        static_assert(std::is_copy_constructible_v<T>, "Optional type should be copy constructible");
        construct(that);
    }

    explicit option(T&& that) noexcept(std::is_nothrow_move_constructible_v<T>)
        : storage(),
          empty(false)
    {
        static_assert(std::is_move_constructible_v<T>, "Optional type should be move constructible");
        construct(std::move(that));
    }

    option(const option& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : storage(),
          empty(that.empty)
    {
        static_assert(std::is_copy_constructible_v<T>, "Optional type should be copy constructible");
        if (that.non_empty()) construct(that.value());
    }

    option(option&& that) noexcept(std::is_nothrow_move_constructible_v<T>)
        : storage(),
          empty(that.empty)
    {
        static_assert(std::is_move_constructible_v<T>, "Optional type should be move constructible");
        if (that.non_empty()) construct(std::move(that.value()));
    }

    template <typename... Ts>
    explicit option(const in_place&, Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
        : storage(),
          empty(false)
    {
        static_assert(std::is_constructible_v<T, Ts&&...>, "Optional type should be constructible from 'Ts&&...'");
        construct(std::forward<Ts>(args)...);
    }

    ~option() noexcept(std::is_nothrow_destructible_v<T>)
    {
        destroy();
    }

    option& operator= (const none_t&) noexcept(std::is_nothrow_destructible_v<T>)
    {
        reset();
        return *this;
    }

    option& operator= (const option& that) noexcept(std::is_nothrow_copy_assignable_v<T>    &&
                                                    std::is_nothrow_copy_constructible_v<T> &&
                                                    std::is_nothrow_destructible_v<T>)
    {
        static_assert(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>, "Optional type should be copy constructible and copy assignable");

        if (!(is_empty() && that.empty))
        {
            if (non_empty() && that.non_empty())      assign(that.value());
            else if (is_empty() && that.non_empty())  initialize(that.value());
            else                                      reset();
        }

        return *this;
    }

    option& operator= (option&& that) noexcept(std::is_nothrow_move_assignable_v<T>    &&
                                               std::is_nothrow_move_constructible_v<T> &&
                                               std::is_nothrow_destructible_v<T>)
    {
        static_assert(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>, "Optional type should be move constructible and move assignable");

        if (!(is_empty() && that.empty))
        {
            if (non_empty() && that.non_empty())      assign(std::move(that).value());
            else if (is_empty() && that.non_empty())  initialize(std::move(that).value());
            else                                      reset();
        }

        return *this;
    }

    option& operator= (const T& that) noexcept(std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_copy_constructible_v<T>)
    {
        static_assert(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>, "Optional type should be copy constructible and copy assignable");

        if (is_empty()) initialize(that);
        else            assign(that);

        return *this;
    }

    option& operator= (T&& that) noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>)
    {
        static_assert(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>, "Optional type should be move constructible and move assignable");

        if (is_empty()) initialize(std::move(that));
        else            assign(std::move(that));

        return *this;
    }

    void reset() noexcept(std::is_nothrow_destructible_v<T>)
    {
        if (non_empty())
        {
            destroy();
            empty = true;
        }
    }

    bool operator== (const none_t&) const noexcept
    {
        return is_empty();
    }

    bool operator!= (const none_t&) const noexcept
    {
        return non_empty();
    }

    bool operator== (const option& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        static_assert(is_comparable_v<const T>, "Optional type should be comparable");
        return (is_empty() == that.empty) && (is_empty() ? true : (value() == that.value()));
    }

    bool operator!= (const option& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(*this == that);
    }

    bool operator== (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        static_assert(is_comparable_v<const T>, "Optional type should be comparable");
        return empty ? false : (value() == that);
    }

    bool operator!= (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(*this == that);
    }

    size_t size() const noexcept
    {
        return empty ? size_t(0) : size_t(1);
    }

    bool is_empty() const noexcept { return empty; }
    bool non_empty() const noexcept { return !empty; }

    const T& get() const & noexcept
    {
        assert(non_empty());
        return value();
    }

    T& get() & noexcept
    {
        assert(non_empty());
        return value();
    }

    T get() && noexcept
    {
        assert(non_empty());
        return value();
    }

    const T& get_or_else(const T& $default) const noexcept
    {
        return empty ? $default : value();
    }

    T& get_or_else(T& $default) noexcept
    {
        return empty ? $default : value();
    }

    T get_or_else(T&& $default) && noexcept
    {
        return empty ? $default : std::move(value());
    }

    template <typename E>
    const T& get_or_throw() const &
    {
        static_assert(std::is_default_constructible_v<E>, "Exception type should be default constructible");
        if (is_empty()) throw E{};
        return value();
    }

    template <typename E>
    T& get_or_throw() &
    {
        static_assert(std::is_default_constructible_v<E>, "Exception type should be default constructible");
        if (is_empty()) throw E{};
        return value();
    }

    template <typename E>
    T get_or_throw() &&
    {
        static_assert(std::is_default_constructible_v<E>, "Exception type should be default constructible");
        if (is_empty()) throw E{};
        return std::move(value());
    }

    template <typename E>
    const T& get_or_throw(E&& exception) const &
    {
        if (empty) throw std::forward<E>(exception);
        return value();
    }

    template <typename E>
    T& get_or_throw(E&& exception) &
    {
        if (empty) throw std::forward<E>(exception);
        return value();
    }

    template <typename E>
    T get_or_throw(E&& exception) const &&
    {
        if (empty) throw std::forward<E>(exception);
        return std::move(value());
    }

    void swap(option& that) noexcept(std::is_nothrow_swappable_v<T&>         &&
                                     std::is_nothrow_move_constructible_v<T> &&
                                     std::is_nothrow_destructible_v<T>)
    {
        static_assert(std::is_swappable_v<T&> && std::is_move_constructible_v<T>, "Optional type should be swapable and move constructible");

        using std::swap;

        if (!(is_empty() && that.empty))
        {
            if (non_empty() && that.non_empty()) swap(value(), that.value());
            else if (is_empty())
            {
                initialize(std::move(that.value()));
                that.reset();
            }
            else
            {
                that.initialize(std::move(value()));
                reset();
            }
        }
    }

    void swap(T& that) noexcept(std::is_nothrow_swappable_v<T&> && std::is_nothrow_move_constructible_v<T>)
    {
        static_assert(std::is_swappable_v<T&> && std::is_move_constructible_v<T>, "Optional type should be swapable and move constructible");

        using std::swap;

        if (is_empty()) initialize(std::move(that));
        else            swap(value(), that);
    }

private:

    template <typename U>
    friend struct std::hash;

    const T* pointer() const noexcept
    {
        return reinterpret_cast<const T*>(std::addressof(storage));
    }

    T* pointer() noexcept
    {
        return reinterpret_cast<T*>(std::addressof(storage));
    }

    const T* pointer_or_null() const noexcept
    {
        return empty ? nullptr : pointer();
    }

    T* pointer_or_null() noexcept
    {
        return empty ? nullptr : pointer();
    }

    const T& value() const & noexcept
    {
        return *pointer();
    }

    T& value() & noexcept
    {
        return *pointer();
    }

    T value() && noexcept
    {
        return std::move(*pointer());
    }

    template <typename... Ts>
    void construct(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
    {
        new (pointer()) T(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void initialize(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
    {
        construct(std::forward<Ts>(args)...);
        empty = false;
    }

    template <typename U>
    void assign(U&& object) noexcept(std::is_nothrow_assignable_v<T, U&&>)
    {
        value() = std::forward<U>(object);
    }

    template <typename U>
    static auto destroy(U*) noexcept -> std::enable_if_t<std::is_trivially_destructible_v<U>>
    {
    }

    template <typename U>
    static auto destroy(U* object) noexcept(std::is_nothrow_destructible_v<U>) -> std::enable_if_t<!std::is_trivially_destructible_v<U>>
    {
        object->~U();
    }

    void destroy() noexcept(noexcept(destroy(pointer())))
    {
        if (non_empty()) destroy(pointer());
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    bool empty;
};

template <typename T>
option<remove_cvr_t<T>> some(T&& value) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<T>, T&&>)
{
    return option<remove_cvr_t<T>>(std::forward<T>(value));
}

constexpr none_t none() noexcept
{
    return none_t();
}

template <typename T, typename... Ts>
option<T> make_option(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
{
    return option<T>(in_place{}, std::forward<Ts>(args)...);
}

} // imdexlib namespace

namespace std {

template <typename T>
void swap(imdexlib::option<T>& a, imdexlib::option<T>& b) noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

template <typename T>
void swap(imdexlib::option<T>& option, T& value) noexcept(noexcept(option.swap(value)))
{
    option.swap(value);
}

template <typename T>
void swap(T& value, imdexlib::option<T>& option) noexcept(noexcept(option.swap(value)))
{
    option.swap(value);
}

template <typename T>
struct hash<imdexlib::option<T>>
{
    size_t operator()(const imdexlib::option<T>& option) const noexcept
    {
        if (option.is_empty()) return 0;

        hash<T> valueHash;
        return valueHash(option.value());
    }
};

} // std namespace
