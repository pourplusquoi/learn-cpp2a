#include <iostream>
#include <string>
#include <vector>

template <template<typename> class Container, typename Elem,
          typename ElemDecay = std::remove_reference_t<Elem>>
void push(Container<ElemDecay>& container, Elem&& elem);

template <typename Elem,
          typename ElemDecay = std::remove_reference_t<Elem>>
void push(std::vector<ElemDecay>& container, Elem&& elem) {
    container.push_back(elem);
}

template <typename F>
struct Filter {
    F f;
    Filter(F&& f) : f(std::move(f)) {}

    template <template<typename> class Container, typename Elem>
    Container<Elem> operator()(const Container<Elem>& container) {
        Container<Elem> ret;
        for (auto&& elem : container) {
            if (f(elem)) {
                push(ret, elem);
            }
        }
        return ret;
    }
};

template <typename F>
struct Transform {
    F f;
    Transform(F&& f) : f(std::move(f)) {}

    template <template<typename> class Container, typename ElemIn,
              typename ElemOut = decltype(std::declval<F>()(std::declval<ElemIn>()))>
    Container<ElemOut> operator()(const Container<ElemIn>& container) {
        Container<ElemOut> ret;
        for (auto&& elem : container) {
            push(ret, f(elem));
        }
        return ret;
    }
};

template <template<typename> class Container, typename ElemIn, typename Operation>
auto operator|(const Container<ElemIn>& container, Operation&& operation) {
    return operation(container);
}

int main() {
    std::vector<int> num { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    auto res = num
        | Filter([](int v) { return v % 2 == 0; })
        | Transform([](int v) { return "%" + std::to_string(v * v); });

    for (auto&& v : res) {
        std::cout << v << std::endl;
    }
}
