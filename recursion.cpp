#include <iostream>
#include <vector>

template <typename Functor>
struct Combinator {
    Functor f;
    Combinator(Functor func) : f(func) {}

    template <typename... Args>
    auto operator()(Args&&... t) const {
        return f(*this, std::forward<Args>(t)...);
    }
};

template <typename Functor>
Combinator<Functor> fix(Functor&& f) {
    return Combinator<Functor>(std::forward<Functor>(f));
}

auto gcd = fix(
    [](auto sub, int a, int b) -> int {
        return b == 0 ? a : sub(b, a % b);
    }
);

auto sqrt = fix(
    [](auto sub, double x, double low, double high) -> double {
        if (high - low < 1e-6)
            return low;
        else {
            double mid = (low + high) / 2.0;
            return mid * mid > x ? sub(x, low, mid) : sub(x, mid, high);
        }
    }
);

template <typename RandomIt>
auto nth_element = fix(
    [](auto sub, RandomIt first, RandomIt nth, RandomIt last) -> void {
        if (first < last && first <= nth && nth < last) {
            RandomIt lo = first, hi = last - 1;
            auto val = *first;
            while (lo < hi) {
                while (lo < hi && !(*hi < val))
                    --hi;
                if (lo < hi)
                    *lo = *hi;
                while (lo < hi && *lo < val)
                    ++lo;
                if (lo < hi)
                    *hi = *lo;
            }
            *lo = val;
            if (nth < lo)
                sub(first, nth, lo);
            else if (lo < nth)
                sub(lo + 1, nth, last);
        }
    }
);

int main() {
    std::vector<int> array{4,2,5,1,6,9,0,7,3,10,8};

    using Iterator = typename std::vector<int>::iterator;
    nth_element<Iterator>(array.begin(), array.begin() + 3, array.end());

    for (auto val : array)
        std::cout << val << " ";
    std::cout << std::endl;
    return 0;
}