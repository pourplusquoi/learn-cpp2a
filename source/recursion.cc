#include <iostream>
#include <random>
#include <utility>
#include <vector>

template <typename Fn>
struct Combinator {
  Fn fn;
  Combinator(Fn func) : fn(std::move(func)) {}

  template <typename... Args>
  auto operator()(Args&&... params) const {
    return fn(*this, std::forward<Args>(params)...);
  }
};

template <typename Fn>
Combinator<Fn> MakeCombinator(Fn&& fn) {
  return Combinator<Fn>(std::forward<Fn>(fn));
}

auto gcd = MakeCombinator(
    [](auto sub, int a, int b) -> int {
      return b == 0 ? a : sub(b, a % b);
    });

auto sq_root = MakeCombinator(
    [](auto sub, double x, double low, double high) -> double {
      if (high - low < 1e-6)
        return low;
      else {
        double mid = (low + high) / 2.0;
        return mid * mid > x ? sub(x, low, mid) : sub(x, mid, high);
      }
    });

template <typename RandomIt, typename Comparator>
inline RandomIt Median(RandomIt a, RandomIt b, RandomIt c) {
  Comparator cmp;
  if (cmp(*b, *a)) {
    std::swap(a, b);
  }
  if (cmp(*c, *b)) {
    std::swap(b, c);
  }
  if (cmp(*b, *a)) {
    std::swap(a, b);
  }
  return a;
}

template <typename RandomIt, typename Comparator>
auto nth_element = MakeCombinator(
    [](auto sub, RandomIt first, RandomIt nth, RandomIt last) -> void {
      if (first >= last || first > nth || nth >= last) {
        return;
      }

      Comparator cmp;
      RandomIt lo = first, hi = last - 1, mid = lo + (hi - lo) / 2;
      RandomIt med = Median<RandomIt, Comparator>(lo, mid, hi);
      
      auto pivot = *med;
      if (lo != med) {
        std::swap(*lo, *med);
      }

      while (lo < hi) {
        while (lo < hi && !cmp(*hi, pivot)) {
          --hi;
        }
        if (lo < hi) {
            *lo = *hi;
        }
        while (lo < hi && cmp(*lo, pivot)) {
            ++lo;
        }
        if (lo < hi) {
          *hi = *lo;
        }
      }
      *lo = pivot;

      if (nth < lo) {
        sub(first, nth, lo);
      } else if (lo < nth) {
        sub(lo + 1, nth, last);
      }
    });

template <typename RandomIt, typename Comparator>
auto quick_sort = MakeCombinator(
    [](auto sub, RandomIt first, RandomIt last) -> void {
      if (first >= last) {
        return;
      }

      Comparator cmp;
      RandomIt lo = first, hi = last - 1, mid = lo + (hi - lo) / 2;
      RandomIt med = Median<RandomIt, Comparator>(lo, mid, hi);

      auto pivot = *med;
      if (lo != med) {
        std::swap(*lo, *med);
      }

      while (lo < hi) {
        while (lo < hi && !cmp(*hi, pivot)) {
          --hi;
        }
        if (lo < hi) {
            *lo = *hi;
        }
        while (lo < hi && cmp(*lo, pivot)) {
            ++lo;
        }
        if (lo < hi) {
          *hi = *lo;
        }
      }
      *lo = pivot;

      sub(first, lo);
      sub(lo + 1, last);
    });

template <typename RandomIt, typename URBG>
auto random_shuffle = MakeCombinator(
    [](auto sub, RandomIt first, RandomIt last, URBG&& g) {
      if (first >= last) {
        return;
      }

      using Distr =
          std::uniform_int_distribution<
              typename std::iterator_traits<RandomIt>::difference_type>;
      using Param = typename Distr::param_type;

      Distr distr;
      std::swap(*first, *(first + distr(g, Param(0, last - first - 1))));
      sub(first + 1, last, std::forward<URBG>(g));
    });

int main() {
  constexpr int size = 64;

  std::vector<int> array(size);
  for (int i = 0; i < size; i++) {
    array[i] = i + 1;
  }

  std::random_device rd;
  std::mt19937 g(rd());
  using Iterator = typename std::vector<int>::iterator;
  random_shuffle<Iterator, std::mt19937&>(array.begin(), array.end(), g);

  nth_element<Iterator, std::less<int>>(array.begin(), array.begin() + 27, array.end());
  for (auto val : array)
    std::cout << val << " ";
  std::cout << std::endl;

  quick_sort<Iterator, std::less<int>>(array.begin(), array.end());
  for (auto val : array)
    std::cout << val << " ";
  std::cout << std::endl;

  return 0;
}
