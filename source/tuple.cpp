#include <iostream>
#include "../header/unwrap_refwrapper.h"

template <typename... Types>
struct tuple;

template <typename Head, class... Tail>
struct tuple<Head, Tail...> {
    Head elem;
    ::tuple<Tail...> next;
    
    tuple() {}

    template <
        typename H,
        typename... T,
        typename = std::enable_if_t<
            std::is_convertible<H, Head>::value>>
    tuple(H&& head, T&&... tail) :
        elem(std::forward<H>(head)),
        next(std::forward<T>(tail)...) {}

    template <
        typename H,
        typename... T,
        typename = std::enable_if_t<
            std::is_convertible<H, Head>::value>>
    tuple(const tuple<H, T...>& t) : 
        elem(t.elem), next(t.next) {};
};

template <typename Type>
struct tuple<Type> {
    Type elem;

    tuple() {}

    template <
        typename T,
        typename = std::enable_if_t<
            std::is_convertible<T, Type>::value>>
    tuple(T&& last) : elem(std::forward<T>(last)) {}

    template <
        typename T,
        typename = std::enable_if_t<
            std::is_convertible<T, Type>::value>>
    tuple(const tuple<T>& t) : elem(t.elem) {}
};

template <class... Types>
constexpr auto make_tuple(Types&&... args) {
    return ::tuple<::special_decay_t<Types>...>(std::forward<Types>(args)...);
}

template <std::size_t I, class T>
struct tuple_element;

// recursive case
template <std::size_t I, class Head, class... Tail>
struct tuple_element<I, ::tuple<Head, Tail...>>
    : public ::tuple_element<I-1, ::tuple<Tail...>> {};

// base case
template <class Head, class... Tail>
struct tuple_element<0, ::tuple<Head, Tail...>> {
   using type = Head;
};

template <std::size_t I, typename... Types>
constexpr typename tuple_element<I,
    ::tuple<Types...>>::type& get(::tuple<Types...>& t) noexcept {
    return get<I - 1>(t.next);
}

template <
    std::size_t I,
    typename Head,
    typename... Tail,
    typename = std::enable_if_t<I == 0>>
constexpr Head& get(::tuple<Head, Tail...>& t) noexcept {
    return t.elem;
}

template <typename Target, typename... Types>
struct contains;

template <
    typename Target,
    typename Head,
    typename... Tail>
struct contains<Target, Head, Tail...> {
    static constexpr bool value = std::is_same<Target, Head>::value
        || ::contains<Target, Tail...>::value;
};

template <typename Target, typename Last>
struct contains<Target, Last> {
    static constexpr bool value = std::is_same<Target, Last>::value;
};

template <
    typename T,
    typename Head,
    typename... Tail,
    typename = std::enable_if_t<
        std::is_same<T, Head>::value &&
        !::contains<T, Tail...>::value>>
constexpr Head& get(::tuple<Head, Tail...>& t) noexcept {
    return t.elem;
}

template <
    typename T,
    typename Head,
    typename... Tail,
    typename = std::enable_if_t<
        !std::is_same<T, Head>::value &&
        ::contains<T, Tail...>::value>>
constexpr T& get(::tuple<Head, Tail...>& t) noexcept {
    return get(t.next);
}

int main() {
    const char* hello = "hello";
    tuple<int, double, std::string> elem = ::make_tuple(3, 2.0, hello);
    tuple<int, tuple<int, double, std::string>> t(9, elem);

    std::cout << ::get<0>(t) << ", " << ::get<int>(t) << std::endl;
    std::cout << ::get<0>(::get<1>(t)) << ", ";
    std::cout << ::get<1>(::get<1>(t)) << ", ";
    std::cout << ::get<2>(::get<1>(t)) << std::endl;
    
    return 0;
}