#include <iostream>
#include <vector>
#include <iterator>

template <typename RandomIt>
void nth_element(RandomIt first, RandomIt nth, RandomIt last) {
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
            ::nth_element(first, nth, lo);
        else if (lo < nth)
            ::nth_element(lo + 1, nth, last);
    }
}

int main() {
    std::vector<int> u{0,1,2,3,4,5,6,7,8,9};
    std::vector<int> v{6,4,7,2,3,8,0,9,5,1};
    std::vector<int> w{6,6,6,6,6,6,6,6,6,6};
    for (int i = 0; i < 10; i++) {
        ::nth_element(u.begin(), u.begin() + i, u.end());
        ::nth_element(v.begin(), v.begin() + i, v.end());
        ::nth_element(w.begin(), w.begin() + i, w.end());
        std::cout << "The " << i + 1
                  << "th element for {u, v, w} are: {"
                  << *(u.begin() + i) << ", "
                  << *(v.begin() + i) << ", "
                  << *(w.begin() + i) << "}" << std::endl;
    }
    return 0;
}