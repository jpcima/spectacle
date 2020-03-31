#pragma once
#include <limits>
#include <utility>

template <class T, class Traits>
struct ordinary_allocator {
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::true_type propagate_on_container_move_assignment;
    template <class U> struct rebind { typedef ordinary_allocator<U, Traits> other; };
    typedef std::true_type is_always_equal;

    ordinary_allocator() noexcept {}
    ordinary_allocator(const ordinary_allocator &) noexcept {}
    template <class U> ordinary_allocator(const ordinary_allocator<U, Traits>&) noexcept {}

    T *address(T &x) const noexcept;
    const T *address(const T &x) const noexcept;

    T *allocate(std::size_t n, const void * = nullptr);
    void deallocate(T *p, std::size_t n) noexcept;
    std::size_t max_size() const noexcept;

    template <class U, class... Args> void construct(U *p, Args &&...args);
    template <class U> void destroy(U *p);
};

template <class T, class Traits>
inline bool operator==(const ordinary_allocator<T, Traits> &, const ordinary_allocator<T, Traits> &) noexcept
{
    return true;
}

template <class T, class Traits>
inline bool operator!=(const ordinary_allocator<T, Traits> &, const ordinary_allocator<T, Traits> &) noexcept
{
    return false;
}

#include "ordinary_allocator.tcc"
