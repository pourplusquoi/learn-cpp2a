#include <iostream>

template <typename T1, typename T2>
struct pair {
    T1 first;
    T2 second;
    
    pair() {}
    pair(const T1& f, const T2& s) : first(f), second(s) {}
    pair(const pair& rhs) : first(rhs.first), second(rhs.second) {}
    
    pair& operator=(const pair& rhs) {
        this->first = rhs.first;
        this->second = rhs.second;
        return *this;
    }
};

template <typename T1, typename T2>
constexpr pair<T1, T2> make_pair(T1&& f, T2&& s) {
    return pair<T1, T2>(std::forward<T1>(f), std::forward<T2>(s));
}

template <
    std::size_t I,
    typename T1,
    typename T2,
    typename = std::enable_if_t<I == 0>>
constexpr T1& get(pair<T1, T2>& p) noexcept {
    return p.first;
}

template <
    std::size_t I,
    typename T1,
    typename T2,
    typename = std::enable_if_t<I == 1>>
constexpr T2& get(pair<T1, T2>& p) noexcept {
    return p.second;
}

template <
    typename T,
    typename T1,
    typename T2,
    typename = std::enable_if_t<
        std::is_same<T, T1>::value &&
        !std::is_same<T, T2>::value>>
constexpr T1& get(pair<T1, T2>& p) noexcept {
    return p.first;
}

template <
    typename T,
    typename T1,
    typename T2,
    typename = std::enable_if_t<
        std::is_same<T, T2>::value && 
        !std::is_same<T, T1>::value>>
constexpr T2& get(pair<T1, T2>& p) noexcept {
    return p.second;
}

int main() {
    ::pair<int, std::string> p = ::make_pair(1, std::string("hello"));
    std::cout << p.first << ", " << p.second << std::endl;
    std::cout << ::get<0>(p) << ", " << ::get<1>(p) << std::endl;
    std::cout << ::get<int>(p) << ", " << ::get<std::string>(p) << std::endl;
    return 0;
}