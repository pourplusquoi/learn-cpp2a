#include <iostream>
#include <vector>

template <typename Type, typename Comparator>
struct Eq {
  bool operator()(Type&& self, Type&& other) const {
    return !Comparator{}(
        std::forward<Type>(self), std::forward<Type>(other)) &&
            !Comparator{}(
                std::forward<Type>(other), std::forward<Type>(self));
  }
};

template <typename Type, typename Comparator>
struct Lt {
  bool operator()(Type&& self, Type&& other) const {
    return Comparator{}(std::forward<Type>(self),
                        std::forward<Type>(other));
  }
};

template <typename Type, typename Comparator>
struct Gt {
  bool operator()(Type&& self, Type&& other) const {
    return Comparator{}(std::forward<Type>(other),
                        std::forward<Type>(self));
  }
};

template <typename RandomIt>
struct Value {
  using Type = decltype(*std::declval<RandomIt>());
};

template <typename RandomIt>
using ValueType = typename Value<RandomIt>::Type;

template <typename RandomIt, typename Comparator>
inline RandomIt median(RandomIt a, RandomIt b, RandomIt c) {
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

template <typename RandomIt, typename Comparator = std::less<ValueType<RandomIt>>>
void nth_elem(RandomIt first, RandomIt nth, RandomIt last) {
  RandomIt lo = first, hi = last - 1;
  if (lo >= hi || lo > nth || nth > hi) {
    return;
  }

  using Type = ValueType<RandomIt>;
  Eq<Type, Comparator> eq;
  Lt<Type, Comparator> lt;
  Gt<Type, Comparator> gt;

  RandomIt mid = lo + (hi - lo) / 2;
  RandomIt med = median<RandomIt, Comparator>(lo, mid, hi);

  auto pivot = *med;
  if (lo != med) {
    std::swap(*lo, *med);
  }

  RandomIt hd = lo;
  while (lo < hi) {
    while (lo < hi && eq(*lo, pivot)) {
      ++lo;
    }
    if (lo < hi && lt(*lo, pivot)) {
      std::swap(*lo, *hd);
      ++hd;
      continue;
    }
    while (lo < hi && gt(*hi, pivot)) {
      --hi;
    }
    if (lo < hi) {
      std::swap(*lo, *hi);
    }
  }

  if (nth < hd) {
    nth_elem(first, nth, hd);
  } else if (lo <= nth) {
    nth_elem(lo, nth, last);
  }
}

int main() {
  std::vector<int> u{0,1,2,3,4,5,6,7,8,9};
  std::vector<int> v{6,4,7,2,3,8,0,9,5,1};
  std::vector<int> w{6,6,6,6,6,6,6,6,6,6};
  for (int i = 0; i < 10; i++) {
    nth_elem(u.begin(), u.begin() + i, u.end());
    nth_elem(v.begin(), v.begin() + i, v.end());
    nth_elem(w.begin(), w.begin() + i, w.end());
    std::cout << "The " << i + 1
          << "th element for {u, v, w} are: {"
          << *(u.begin() + i) << ", "
          << *(v.begin() + i) << ", "
          << *(w.begin() + i) << "}" << std::endl;
  }
  return 0;
}