#ifndef INC_NOSHARED_HPP_
#define INC_NOSHARED_HPP_
#pragma once

#include <iostream>
#include <memory>

// define NSTD_USE_LOCKED_PTR to enable lock function for weak pointer
// #define NSTD_USE_LOCKED_PTR

namespace nstd {


template <typename T>
class noshared_ptr { // non-copyable pointer to an object
public:
    using pointer = T*;
    using element_type = T;

    constexpr noshared_ptr() noexcept : sptr_() {}

    constexpr noshared_ptr(nullptr_t) noexcept : sptr_() {}

    noshared_ptr &operator=(nullptr_t) noexcept {
        reset();
        return *this;
    }

    explicit noshared_ptr(pointer ptr) noexcept : sptr_(ptr) {}

    noshared_ptr(noshared_ptr &&right) noexcept
        : sptr_(std::move(right.sptr_)) {}

    template <class T2,
        typename std::enable_if<std::is_convertible<typename noshared_ptr<T2>::pointer, pointer>::value, int>::type = 0>
    noshared_ptr(noshared_ptr<T2> &&right) noexcept
        : sptr_(std::move(right.sptr_)) {}

    template <class T2,
        typename std::enable_if<std::is_convertible<typename noshared_ptr<T2>::pointer, pointer>::value, int>::type = 0>
    noshared_ptr &operator=(noshared_ptr<T2> &&right) noexcept {
        noshared_ptr(std::forward<noshared_ptr>(right)).swap(*this);
        return *this;
    }

    noshared_ptr &operator=(noshared_ptr &&right) noexcept {
        noshared_ptr(std::forward<noshared_ptr>(right)).swap(*this);
        return *this;
    }

    void swap(noshared_ptr &right) noexcept {
        std::swap(this->sptr_, right.sptr_);
    }

    ~noshared_ptr() noexcept {
    }

    typename std::add_lvalue_reference<T>::type operator*() const noexcept /* strengthened */ {
        return *sptr_;
    }

    pointer operator->() const noexcept {
        return sptr_.operator->();
    }

    pointer get() const noexcept {
        return sptr_.get();
    }

    explicit operator bool() const noexcept {
        return sptr_.operator bool();
    }

    void reset(pointer ptr = nullptr) noexcept {
        sptr_.reset(ptr);
    }

    noshared_ptr(const noshared_ptr &) = delete;
    noshared_ptr &operator=(const noshared_ptr &) = delete;
private:
    std::shared_ptr<T> sptr_;
    noshared_ptr(const std::shared_ptr<T> &right) noexcept 
        : sptr_(right) {
    }
    template <class T2>
    friend class noshared_ptr;
    template <class T2>
    friend class noweak_ptr;

    template<typename T1, typename T2>
    friend noshared_ptr<T1> dynamic_pointer_cast(noshared_ptr<T2> &&other);
};

template<typename T1, typename T2>
noshared_ptr<T1> dynamic_pointer_cast(noshared_ptr<T2> &&other) {
    const std::shared_ptr<T1> &sptr = std::dynamic_pointer_cast<T1>(std::move(other.sptr_));
    return noshared_ptr<T1>(sptr);
}


// FUNCTION TEMPLATE make_unique
template <class T, class... Types>
noshared_ptr<T> make_noshared(Types&&... _Args) { // make a noshared_ptr
    return noshared_ptr<T>(new T(std::forward<Types>(_Args)...));
}

template <class T>
void swap(noshared_ptr<T> &left, noshared_ptr<T> &right) noexcept {
    left.swap(right);
}

template <class T1, class T2>
bool operator==(const noshared_ptr<T1> &left, const noshared_ptr<T2> &right) {
    return left.get() == right.get();
}

template <class T1, class T2>
bool operator!=(const noshared_ptr<T1> &left, const noshared_ptr<T2> &right) {
    return !(left == right);
}

template <class T1, class T2>
bool operator<(const noshared_ptr<T1> &left, const noshared_ptr<T2> &right) {
    using Ptr1 = typename noshared_ptr<T1>::pointer;
    using Ptr2 = typename noshared_ptr<T2>::pointer;
    using Common = typename std::common_type<Ptr1, Ptr2>::type;
    return std::less<Common>{}(left.get(), right.get());
}

template <class T1, class T2>
bool operator>=(const noshared_ptr<T1> &left, const noshared_ptr<T2> &right) {
    return !(left < right);
}

template <class T1, class T2>
bool operator>(const noshared_ptr<T1> &left, const noshared_ptr<T2> &right) {
    return right < left;
}

template <class T1, class T2>
bool operator<=(const noshared_ptr<T1> &left, const noshared_ptr<T2> &right) {
    return !(right < left);
}

template <class T>
bool operator==(const noshared_ptr<T> &left, nullptr_t) noexcept {
    return !left;
}

template <class T>
bool operator==(nullptr_t, const noshared_ptr<T> &right) noexcept {
    return !right;
}

template <class T>
bool operator!=(const noshared_ptr<T> &left, nullptr_t right) noexcept {
    return !(left == right);
}

template <class T>
bool operator!=(nullptr_t left, const noshared_ptr<T> &right) noexcept {
    return !(left == right);
}

template <class T>
bool operator<(const noshared_ptr<T> &left, nullptr_t right) {
    using Ptr = typename noshared_ptr<T>::pointer;
    return std::less<Ptr>{}(left.get(), right);
}

template <class T>
bool operator<(nullptr_t left, const noshared_ptr<T> &right) {
    using Ptr = typename noshared_ptr<T>::pointer;
    return std::less<Ptr>{}(left, right.get());
}

template <class T>
bool operator>=(const noshared_ptr<T> &left, nullptr_t right) {
    return !(left < right);
}

template <class T>
bool operator>=(nullptr_t left, const noshared_ptr<T> &right) {
    return !(left < right);
}

template <class T>
bool operator>(const noshared_ptr<T> &left, nullptr_t right) {
    return right < left;
}

template <class T>
bool operator>(nullptr_t left, const noshared_ptr<T> &right) {
    return right < left;
}

template <class T>
bool operator<=(const noshared_ptr<T> &left, nullptr_t right) {
    return !(right < left);
}

template <class T>
bool operator<=(nullptr_t left, const noshared_ptr<T> &right) {
    return !(right < left);
}

template <typename T>
class locked_ptr;

// noweak_ptr

// CLASS TEMPLATE noweak_ptr
template <class T>
class noweak_ptr {
public:
    constexpr noweak_ptr() noexcept 
        : wptr_() {
    }

    noweak_ptr(const noweak_ptr &other) noexcept 
        : wptr_(other.wptr_) {
    }

    template <class T2, typename std::enable_if<std::is_convertible<T2 *, T *>::value, int>::type = 0>
    noweak_ptr(const noshared_ptr<T2> &other) noexcept
        : wptr_(other.sptr_) {
    }
    
    template <class T2, typename std::enable_if<std::is_convertible<T2 *, T *>::value, int>::type = 0>
    noweak_ptr(const locked_ptr<T2> &other) noexcept
        : wptr_(other.sptr_) {
    }

    template <class T2, typename std::enable_if<std::is_convertible<T2 *, T *>::value, int>::type = 0>
    noweak_ptr(const noweak_ptr<T2> &other) noexcept
        : wptr_(other.wptr_) {
    }

    noweak_ptr(noweak_ptr &&other) noexcept
        : wptr_(std::move(other.wptr_)) {
    }

    template <class T2, typename std::enable_if<std::is_convertible<T2 *, T *>::value, int>::type = 0>
    noweak_ptr(noweak_ptr<T2> &&other) noexcept
        : wptr_(std::move(other.wptr_)) {
    }

    ~noweak_ptr() noexcept {
    }

    noweak_ptr &operator=(const noweak_ptr &right) noexcept {
        noweak_ptr(right).swap(*this);
        return *this;
    }

    template <class T2>
    noweak_ptr &operator=(const noweak_ptr<T2> &right) noexcept {
        noweak_ptr(right).swap(*this);
        return *this;
    }

    noweak_ptr &operator=(noweak_ptr &&right) noexcept {
        noweak_ptr(std::move(right)).swap(*this);
        return *this;
    }

    template <class T2>
    noweak_ptr &operator=(noweak_ptr<T2> &&right) noexcept {
        noweak_ptr(std::move(right)).swap(*this);
        return *this;
    }

    template <class T2>
    noweak_ptr &operator=(const noshared_ptr<T2> &right) noexcept {
        noweak_ptr(right).swap(*this);
        return *this;
    }

    void reset() noexcept { // release resource, convert to null noweak_ptr object
        noweak_ptr{}.swap(*this);
    }

    void swap(noweak_ptr &other) noexcept {
        wptr_.swap(other.wptr_);
    }

    bool expired() const noexcept {
        return wptr_.expired();
    }

#ifdef NSTD_ENABLE_WEAK_LOCK
    noshared_ptr<T> lock() const noexcept { // convert to shared_ptr
        return noshared_ptr<T>(wptr_.lock());
    }
#endif

    template <typename T2>
    bool owner_before(const noweak_ptr<T2> &right) const noexcept { // compare addresses of manager objects
        return wptr_.owner_before(right.wptr_);
    }
private:
    std::weak_ptr<T> wptr_;
    friend class locked_ptr<T>;
    template <class T2>
    friend class noweak_ptr;

};

template <typename T>
class locked_ptr { // non-copyable pointer to an object
public:
    using pointer = T*;
    using element_type = T;

    explicit locked_ptr(const noweak_ptr<T> &wptr) noexcept
        : sptr_(wptr.wptr_.lock()) {}

    typename std::add_lvalue_reference<T>::type operator*() const noexcept /* strengthened */ {
        return *sptr_;
    }

    pointer operator->() const noexcept {
        return sptr_.operator->();
    }

    pointer get() const noexcept {
        return sptr_.get();
    }

    explicit operator bool() const noexcept {
        return sptr_.operator bool();
    }

    void reset(pointer ptr = nullptr) noexcept {
        sptr_.reset(ptr);
    }

    locked_ptr() = delete;
    locked_ptr(const locked_ptr &) = delete;
    locked_ptr(locked_ptr &&) = delete;
    locked_ptr &operator=(const locked_ptr &) = delete;

private:
    std::shared_ptr<T> sptr_;
    locked_ptr(const std::shared_ptr<T> &right) noexcept 
        : sptr_(right) {
    }
    template <class T2>
    friend class noweak_ptr;
};


template <class T1, class T2>
bool operator==(const locked_ptr<T1> &left, const locked_ptr<T2> &right) {
    return left.get() == right.get();
}

template <class T1, class T2>
bool operator!=(const locked_ptr<T1> &left, const locked_ptr<T2> &right) {
    return !(left == right);
}

template <class T1, class T2>
bool operator<(const locked_ptr<T1> &left, const locked_ptr<T2> &right) {
    using Ptr1 = typename locked_ptr<T1>::pointer;
    using Ptr2 = typename locked_ptr<T2>::pointer;
    using Common = typename std::common_type<Ptr1, Ptr2>::type;
    return std::less<Common>{}(left.get(), right.get());
}

template <class T1, class T2>
bool operator>=(const locked_ptr<T1> &left, const locked_ptr<T2> &right) {
    return !(left < right);
}

template <class T1, class T2>
bool operator>(const locked_ptr<T1> &left, const locked_ptr<T2> &right) {
    return right < left;
}

template <class T1, class T2>
bool operator<=(const locked_ptr<T1> &left, const locked_ptr<T2> &right) {
    return !(right < left);
}

template <class T>
bool operator==(const locked_ptr<T> &left, nullptr_t) noexcept {
    return !left;
}

template <class T>
bool operator==(nullptr_t, const locked_ptr<T> &right) noexcept {
    return !right;
}

template <class T>
bool operator!=(const locked_ptr<T> &left, nullptr_t right) noexcept {
    return !(left == right);
}

template <class T>
bool operator!=(nullptr_t left, const locked_ptr<T> &right) noexcept {
    return !(left == right);
}

template <class T>
bool operator<(const locked_ptr<T> &left, nullptr_t right) {
    using Ptr = typename locked_ptr<T>::pointer;
    return std::less<Ptr>{}(left.get(), right);
}

template <class T>
bool operator<(nullptr_t left, const locked_ptr<T> &right) {
    using Ptr = typename locked_ptr<T>::pointer;
    return std::less<Ptr>{}(left, right.get());
}

template <class T>
bool operator>=(const locked_ptr<T> &left, nullptr_t right) {
    return !(left < right);
}

template <class T>
bool operator>=(nullptr_t left, const locked_ptr<T> &right) {
    return !(left < right);
}

template <class T>
bool operator>(const locked_ptr<T> &left, nullptr_t right) {
    return right < left;
}

template <class T>
bool operator>(nullptr_t left, const locked_ptr<T> &right) {
    return right < left;
}

template <class T>
bool operator<=(const locked_ptr<T> &left, nullptr_t right) {
    return !(right < left);
}

template <class T>
bool operator<=(nullptr_t left, const locked_ptr<T> &right) {
    return !(right < left);
}

template <class T>
bool operator==(const locked_ptr<T> &left, const noshared_ptr<T> &right) noexcept {
    return left.get() == right.get();
}

template <class T>
bool operator==(const noshared_ptr<T> &left, const locked_ptr<T> &right) noexcept {
    return left.get() == right.get();
}

template <class T>
bool operator!=(const locked_ptr<T> &left, const noshared_ptr<T> &right) noexcept {
    return !(left == right);
}

template <class T>
bool operator!=(const noshared_ptr<T> & left, const locked_ptr<T> &right) noexcept {
    return !(left == right);
}

template <class T>
bool operator<(const locked_ptr<T> &left, const noshared_ptr<T> & right) {
    using Ptr = typename locked_ptr<T>::pointer;
    return std::less<Ptr>{}(left.get(), right.get());
}

template <class T>
bool operator<(const noshared_ptr<T> & left, const locked_ptr<T> &right) {
    using Ptr = typename locked_ptr<T>::pointer;
    return std::less<Ptr>{}(left.get(), right.get());
}

template <class T>
bool operator>=(const locked_ptr<T> &left, const noshared_ptr<T> & right) {
    return !(left < right);
}

template <class T>
bool operator>=(const noshared_ptr<T> & left, const locked_ptr<T> &right) {
    return !(left < right);
}

template <class T>
bool operator>(const locked_ptr<T> &left, const noshared_ptr<T> & right) {
    return right < left;
}

template <class T>
bool operator>(const noshared_ptr<T> & left, const locked_ptr<T> &right) {
    return right < left;
}

template <class T>
bool operator<=(const locked_ptr<T> &left, const noshared_ptr<T> & right) {
    return !(right < left);
}

template <class T>
bool operator<=(const noshared_ptr<T> & left, const locked_ptr<T> &right) {
    return !(right < left);
}


template <class Elem, class Traits, class T>
std::basic_ostream<Elem, Traits> &operator<<(std::basic_ostream<Elem, Traits> &out, const noshared_ptr<T> &ptr) {
    // write contained pointer to stream
    return out << ptr.get();
}

template <class Elem, class Traits, class T>
std::basic_ostream<Elem, Traits> &operator<<(std::basic_ostream<Elem, Traits> &out, const noweak_ptr<T> &ptr) {
    // write contained pointer to stream
    return out << locked_ptr<T>(ptr).get();
}

template <class Elem, class Traits, class T>
std::basic_ostream<Elem, Traits> &operator<<(std::basic_ostream<Elem, Traits> &out, const locked_ptr<T> &ptr) {
    // write contained pointer to stream
    return out << ptr.get();
}

}

#endif
