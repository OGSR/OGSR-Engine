#pragma once

#include <cassert>

#include "utility.hpp"
#include "traits.hpp"

namespace imdexlib {

template <typename T>
class option;

template <typename T>
struct is_option : std::false_type {};

template <typename T>
struct is_option<option<T>> : std::true_type {};

template <typename T>
constexpr bool is_option_v = is_option<T>::value;

struct none_t final {};

template <typename T>
class option final {
    static_assert(!std::is_const_v<T>, "Optional type shouldn't be const. Use 'const option<T>' declaration instead.");
    static_assert(std::is_destructible_v<T>, "Optional type should be destructible");
    static_assert(!std::is_rvalue_reference_v<T>, "Optional type shouldn't be a rvalue reference type");
public:
    using value_type = T;
    using reference = T&;
    using const_reference = std::add_const_t<T>&;

    option() noexcept
        : storage(),
          empty_(true)
    {}

    option(none_t) noexcept
        : option()
    {}

    template <typename U>
    option(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : storage(),
          empty_(false) {
        if constexpr(std::is_constructible_v<T, U&&>) {
            construct(std::forward<U>(value));
        } else {
            static_assert(false, "Optional type needs to be constructible from argument type");
        }
    }

    template <typename U>
    option(const option<U>& that) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : storage(),
          empty_(that.empty_) {
        if constexpr(std::is_constructible_v<T, const U&>) {
            if (that.non_empty()) construct(that.value());
        } else {
            static_assert(false, "Optional type needs to be constructible from optinal argument type");
        }
    }

    template <typename U>
    option(option<U>&& that) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : storage(),
          empty_(that.empty_) {
        if constexpr(std::is_constructible_v<T, U&&>) {
            if (that.non_empty()) construct(std::move(that.value()));
        } else {
            static_assert(false, "Optional type needs to be constructible from optional argument type");
        }
    }

    template <typename... Ts>
    explicit option(const in_place&, Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
        : storage(),
          empty_(false) {
        if constexpr (std::is_constructible_v<T, Ts&&...>) {
            construct(std::forward<Ts>(args)...);
        } else {
            static_assert(false, "Optional type needs to be constructible from passed arguments");
        }
    }

    ~option() noexcept(std::is_nothrow_destructible_v<T>) {
        destroy();
    }

    option& operator= (none_t) noexcept(std::is_nothrow_destructible_v<T>) {
        reset();
        return *this;
    }

    template <typename U>
    option& operator= (const option<U>& that) noexcept(std::is_nothrow_assignable_v<T, const U&>    &&
                                                       std::is_nothrow_constructible_v<T, const U&> &&
                                                       std::is_nothrow_destructible_v<T>)
    {
        if constexpr(std::is_constructible_v<T, const U&> && std::is_assignable_v<T, const U&>) {
            if (non_empty() || that.non_empty()) {
                if (non_empty() && that.non_empty())  assign(that.value());
                else if (empty() && that.non_empty()) initialize(that.value());
                else                                  reset();
            }
        } else {
            static_assert(false, "Optional type needs to be constructible and assignable from the argument optional type");
        }

        return *this;
    }

    template <typename U>
    option& operator= (option<U>&& that) noexcept(std::is_nothrow_assignable_v<T&, U&&>    &&
                                                  std::is_nothrow_constructible_v<T, U&&> &&
                                                  std::is_nothrow_destructible_v<T>)
    {
        if constexpr (std::is_constructible_v<T, U&&> && std::is_assignable_v<T&, U&&>) {
            if (non_empty() || that.non_empty()) {
                if (non_empty() && that.non_empty())  assign(std::move(that.value()));
                else if (empty() && that.non_empty()) initialize(std::move(that.value()));
                else                                  reset();
            }
        } else {
            static_assert(false, "Optional type needs to be constructible and assignable from the argument optional type");
        }

        return *this;
    }

    template <typename U>
    option& operator= (U&& value) noexcept(std::is_nothrow_assignable_v<T&, U&&> && std::is_nothrow_constructible_v<T, U&&>) {
        if constexpr (std::is_constructible_v<T, U&&> && std::is_assignable_v<T&, U&&>) {
            if (empty()) initialize(std::forward<U>(value));
            else         assign(std::forward<U>(value));
        } else {
            static_assert(false, "Optional type needs to be constructible and assignable from the argument type");
        }

        return *this;
    }

    void reset() noexcept(std::is_nothrow_destructible_v<T>) {
        if (non_empty()) {
            destroy();
            empty_ = true;
        }
    }

    template <typename... Ts>
    void emplace(Ts&&... args) noexcept(noexcept(reset()) && std::is_nothrow_constructible_v<T, Ts&&...>) {
        reset();
        initialize(std::forward<Ts>(args)...);
    }

    bool operator== (none_t) const noexcept {
        return empty();
    }

    bool operator!= (none_t) const noexcept {
        return non_empty();
    }

    template <typename U>
    bool operator== (const option<U>& that) const noexcept {
        if constexpr (is_comparable_to_v<const T&, const U&>) {
            return (empty() == that.empty()) && (empty() ? true : (value() == that.value()));
        } else {
            static_assert(false, "Types not comparable");
        }
    }

    template <typename U>
    bool operator!= (const option<U>& that) const noexcept {
        return !(*this == that);
    }

    template <typename U>
    bool operator== (const U& that) const noexcept {
        if constexpr (is_comparable_to_v<const T&, const U&>) {
            return empty() ? false : (value() == that);
        } else {
            static_assert(false, "Types not comparable");
            return false;
        }
    }

    bool operator!= (const T& that) const noexcept {
        return !(*this == that);
    }

    size_t size() const noexcept {
        return empty() ? size_t(0) : size_t(1);
    }

    bool empty() const noexcept { return empty_; }
    bool non_empty() const noexcept { return !empty_; }

    const_reference get() const noexcept {
        assert(non_empty());
        return value();
    }

    reference get() noexcept {
        assert(non_empty());
        return value();
    }

    const_reference get_or_else(const T& $default) const noexcept {
        return empty() ? $default : value();
    }

    reference get_or_else(T& $default) noexcept {
        return empty() ? $default : value();
    }

    template <typename E>
    const_reference get_or_throw() const {
        static_assert(std::is_default_constructible_v<E>, "Exception type needs to be default constructible");
        if (empty()) throw E{};
        return value();
    }

    template <typename E>
    reference get_or_throw() {
        static_assert(std::is_default_constructible_v<E>, "Exception type needs to be default constructible");
        if (empty()) throw E{};
        return value();
    }

    template <typename E>
    const_reference get_or_throw(E&& exception) const {
        if (empty()) throw std::forward<E>(exception);
        return value();
    }

    template <typename E>
    reference get_or_throw(E&& exception) {
        if (empty()) throw std::forward<E>(exception);
        return value();
    }

    reference operator* () noexcept {
        return get();
    }

    const_reference operator* () const noexcept {
        return get();
    }

    T* operator-> () noexcept {
        return std::addressof(get());
    }

    const T* operator-> () const noexcept {
        return std::addressof(get());
    }

    void swap(option& that) noexcept(std::is_nothrow_swappable_v<T&>         &&
                                     std::is_nothrow_move_constructible_v<T> &&
                                     std::is_nothrow_destructible_v<T>) {
        if constexpr (std::is_swappable_v<T&> && std::is_move_constructible_v<T>) {
            using std::swap;

            if (!(empty() && that.empty())) {
                if (non_empty() && that.non_empty()) {
                    swap(value(), that.value());
                } else if (empty()) {
                    initialize(std::move(that.value()));
                    that.reset();
                } else {
                    that.initialize(std::move(value()));
                    reset();
                }
            }
        } else {
            static_assert(false, "Optional type needs to be swapable and move constructible");
        }
    }

    void swap(T& that) noexcept(std::is_nothrow_swappable_v<T&> &&
                                std::is_nothrow_move_constructible_v<T>) {
        if constexpr (std::is_swappable_v<T&> && std::is_move_constructible_v<T>) {
            using std::swap;
            if (empty()) initialize(std::move(that));
            else         swap(value(), that);
        } else {
            static_assert(false, "Optional type needs to be swapable and move constructible");
        }
    }

    template <typename Handler>
    auto map(Handler&& handler) const & {
        if constexpr (is_callable_v<Handler&&, const T&>) {
            using Ret = std::decay_t<std::result_of_t<Handler&&(const T&)>>;
            return empty() ? option<Ret>() : option<Ret>(get());
        } else {
            static_assert(false, "Invalid handler");
            return option();
        }
    }

    template <typename Handler>
    auto map(Handler&& handler) && {
        if constexpr (is_callable_v<Handler&&, T&&>) {
            using Ret = std::decay_t<std::result_of_t<Handler&&(T&&)>>;
            return empty() ? option<Ret>() : option<Ret>(std::move(get()));
        } else {
            static_assert(false, "Invalid handler");
            return option();
        }
    }

    auto flatten() const & {
        if constexpr (is_option_v<T>) {
            using Value = typename T::value_type;

            if (empty()) return T();
            const T& value = get();
            return value.empty() ? T() : option<Value>(value.get());
        } else {
            static_assert(false, "Optional type needs to be an option");
            return option();
        }
    }

    auto flatten() && {
        if constexpr (is_option_v<T>) {
            using Value = typename T::value_type;

            if (empty()) return T();
            T& value = get();
            return value.empty() ? T() : option<Value>(std::move(value.get()));
        } else {
            static_assert(false, "Optional type needs to be an option");
            return option();
        }
    }

private:
    const T* pointer() const noexcept {
        return reinterpret_cast<const T*>(std::addressof(storage));
    }

    T* pointer() noexcept {
        return reinterpret_cast<T*>(std::addressof(storage));
    }

    const T* pointer_or_null() const noexcept {
        return empty() ? nullptr : pointer();
    }

    T* pointer_or_null() noexcept {
        return empty() ? nullptr : pointer();
    }

    const_reference value() const noexcept {
        return *pointer();
    }

    reference value() noexcept {
        return *pointer();
    }

    template <typename... Ts>
    void construct(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>) {
        new (pointer()) T(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void initialize(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>) {
        construct(std::forward<Ts>(args)...);
        empty_ = false;
    }

    template <typename U>
    void assign(U&& object) noexcept(std::is_nothrow_assignable_v<T, U&&>) {
        value() = std::forward<U>(object);
    }

    void destroy() noexcept(std::is_nothrow_destructible_v<T>) {
        if constexpr(!std::is_trivially_destructible_v<T>) {
            if (non_empty()) pointer()->~T();
        }
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    bool empty_;
};

template <typename T>
class option<T&> final {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = std::add_const_t<T>&;

    option() = default;
    option(const option& that) : ref(that.ref) {}
    option(none_t) : option() {}
    option(std::reference_wrapper<T> ref) : ref(&(ref.get())) {}

    option& operator= (const option& that) {
        ref = that.ref;
        return *this;
    }

    option& operator= (std::reference_wrapper<T> ref) {
        this->ref = &(ref.get());
        return *this;
    }

    bool operator== (none_t) const noexcept {
        return empty();
    }

    bool operator!= (none_t) const noexcept {
        return non_empty();
    }

    void reset() {
        ref = nullptr;
    }

    size_t size() const noexcept {
        return ref ? size_t(1) : size_t(0);
    }

    bool empty() const noexcept {
        return ref == nullptr;
    }

    bool non_empty() const noexcept {
        return ref != nullptr;
    }

    const_reference get() const noexcept {
        assert(non_empty());
        return *ref;
    }

    reference get() noexcept {
        assert(non_empty());
        return *ref;
    }

    const_reference get_or_else(const std::reference_wrapper<T> $default) const noexcept {
        return ref ? *ref : $default.get();
    }

    reference get_or_else(const std::reference_wrapper<T> $default) noexcept {
        return ref ? *ref : $default.get();
    }

    template <typename E>
    const_reference get_or_throw() const {
        static_assert(std::is_default_constructible_v<E>, "Exception type needs to be default constructible");
        if (empty()) throw E{};
        return *ref;
    }

    template <typename E>
    reference get_or_throw() {
        static_assert(std::is_default_constructible_v<E>, "Exception type needs to be default constructible");
        if (empty()) throw E{};
        return *ref;
    }

    template <typename E>
    const_reference get_or_throw(E&& exception) const {
        if (empty()) throw std::forward<E>(exception);
        return *ref;
    }

    template <typename E>
    reference get_or_throw(E&& exception) {
        if (empty()) throw std::forward<E>(exception);
        return *ref;
    }

    reference operator* () noexcept {
        return get();
    }

    const_reference operator* () const noexcept {
        return get();
    }

    void swap(option& that) noexcept {
        std::swap(ref, that.ref);
    }

private:
    T* ref = nullptr;
};

template <typename T>
option<std::decay_t<T>> some(T&& value) noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T&&>) {
    return option<std::decay_t<T>>(std::forward<T>(value));
}

template <typename T>
auto some(std::reference_wrapper<T> ref) {
    using type = std::add_lvalue_reference_t<typename std::reference_wrapper<T>::type>;
    return option<type>(ref);
}

constexpr none_t none() noexcept {
    return none_t();
}

template <typename T, typename... Ts>
option<T> make_option(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>) {
    return option<T>(in_place{}, std::forward<Ts>(args)...);
}

template <typename T>
void swap(option<T>& a, option<T>& b) noexcept(noexcept(a.swap(b))) {
    a.swap(b);
}

template <typename T>
void swap(option<T>& option, T& value) noexcept(noexcept(option.swap(value))) {
    option.swap(value);
}

template <typename T>
void swap(T& value, option<T>& option) noexcept(noexcept(option.swap(value))) {
    option.swap(value);
}

template <typename T>
struct is_reference_option : std::false_type {};

template <typename T>
struct is_reference_option<option<T&>> : std::true_type {};

template <typename T>
constexpr bool is_reference_option_v = is_reference_option<T>::value;

} // imdexlib namespace

namespace std {

template <typename T>
struct hash<imdexlib::option<T>> {
    size_t operator()(const imdexlib::option<T>& option) const noexcept {
        if (option.empty()) return 0;
        return hash<T>()(option.get());
    }
};

template <typename T>
struct hash<imdexlib::option<T&>> {
    size_t operator()(const imdexlib::option<T&> option) const noexcept {
        if (option.empty()) return 0;
        return hash<const T*>()(std::addressof(option.get()));
    }
};

} // std namespace
