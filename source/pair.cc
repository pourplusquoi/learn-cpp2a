#include <iostream>
#include <type_traits>
#include "../header/unwrap_refwrapper.h"

template <typename T1, typename T2>
struct pair {
    T1 first;
    T2 second;
    
    pair() {}

    template <
        typename U1,
        typename U2,
        typename = std::enable_if_t<
            std::is_convertible<U1, T1>::value &&
            std::is_convertible<U2, T2>::value>>
    pair(U1&& f, U2&& s) :
        first(std::forward<U1>(f)),
        second(std::forward<U2>(s)) {}

    template <
        typename U1,
        typename U2,
        typename = std::enable_if_t<
            std::is_convertible<U1, T1>::value &&
            std::is_convertible<U2, T2>::value>>
    pair(const ::pair<U1, U2>& rhs) :
        first(rhs.first), second(rhs.second) {}
};

template <
    typename U1,
    typename U2,
    typename T1 = ::special_decay_t<U1>,
    typename T2 = ::special_decay_t<U2>>
constexpr auto make_pair(U1&& f, U2&& s) {
    return ::pair<T1, T2>(std::forward<U1>(f), std::forward<U2>(s));
}

template <
    std::size_t I,
    typename T1,
    typename T2,
    typename = std::enable_if_t<I == 0>>
constexpr T1& get(::pair<T1, T2>& p) noexcept {
    return p.first;
}

template <
    std::size_t I,
    typename T1,
    typename T2,
    typename = std::enable_if_t<I == 1>>
constexpr T2& get(::pair<T1, T2>& p) noexcept {
    return p.second;
}

template <
    typename T,
    typename T1,
    typename T2,
    typename = std::enable_if_t<
        std::is_same<T, T1>::value &&
        !std::is_same<T, T2>::value>>
constexpr T1& get(::pair<T1, T2>& p) noexcept {
    return p.first;
}

template <
    typename T,
    typename T1,
    typename T2,
    typename = std::enable_if_t<
        std::is_same<T, T2>::value && 
        !std::is_same<T, T1>::value>>
constexpr T2& get(::pair<T1, T2>& p) noexcept {
    return p.second;
}

int main() {
    const char* hello = "hello";
    ::pair<int, std::string> p = ::make_pair(1, hello);
    
    std::cout << ::get<0>(p) << ", " << ::get<1>(p) << std::endl;
    std::cout << ::get<int>(p) << ", " << ::get<std::string>(p) << std::endl;
    
    return 0;
}