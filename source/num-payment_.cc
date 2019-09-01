#include <iostream>
#include <type_traits>
#include <chrono>

using namespace std;

template <int ...Currency>
struct NumPayment;

template <int First, int ...Rest>
struct NumPayment<First, Rest...> {
    
    template <int Index, typename = void>
    struct Result : integral_constant<
        size_t,
        NumPayment<Rest...>::template Result<Index>::value +
            Result<std::max(Index - First, -1)>::value> {};

    template <typename T>
    struct Result<-1, T> : integral_constant<size_t, 0> {};
};

template <>
struct NumPayment<> {
    template <int Index, typename = void>
    struct Result : integral_constant<long long, 0> {};

    template <typename T>
    struct Result<0, T> : integral_constant<long long, 1> {};
};

int main() {
    auto start = std::chrono::system_clock::now();
    
    constexpr long long res = NumPayment<1, 5, 10, 20, 50, 100>::Result<10000>::value;

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    cout << res << endl << elapsed_seconds.count() << endl;
    return 0;
}