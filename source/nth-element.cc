#include <iostream>
#include <utility>
#include <vector>

template <typename Type, typename Comparator>
struct Eq {
  bool operator()(Type&& self, Type&& other) const {
    Comparator cmp;
    return !cmp(std::forward<Type>(self), std::forward<Type>(other)) &&
        !cmp(std::forward<Type>(other), std::forward<Type>(self));
  }
};

template <typename Type, typename Comparator>
struct Lt {
  bool operator()(Type&& self, Type&& other) const {
    Comparator cmp;
    return cmp(std::forward<Type>(self), std::forward<Type>(other));
  }
};

template <typename Type, typename Comparator>
struct Gt {
  bool operator()(Type&& self, Type&& other) const {
    Comparator cmp;
    return cmp(std::forward<Type>(other), std::forward<Type>(self));
  }
};

template <typename RandomIt>
struct ElemValue {
  using Type = decltype(*std::declval<RandomIt>());
};

template <typename RandomIt>
using ElemValueType = typename ElemValue<RandomIt>::Type;

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

template <typename RandomIt,
          typename Comparator = std::less<ElemValueType<RandomIt>>>
void NthElement(RandomIt first, RandomIt nth, RandomIt last) {
  if (last - first <= 1 || nth < first || last <= nth) {
    return;
  }

  using Type = ElemValueType<RandomIt>;
  Eq<Type, Comparator> eq;
  Lt<Type, Comparator> lt;
  Gt<Type, Comparator> gt;

  RandomIt lo = first, hi = last - 1, mid = lo + (hi - lo) / 2;
  RandomIt med = Median<RandomIt, Comparator>(lo, mid, hi);

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
    NthElement(first, nth, hd);
  } else if (lo <= nth) {
    NthElement(lo, nth, last);
  }
}

int main() {
  std::vector<int> u{0,1,2,3,4,5,6,7,8,9};
  std::vector<int> v{6,4,7,2,3,8,0,9,5,1};
  std::vector<int> w{6,6,6,6,6,6,6,6,6,6};
  for (int i = 0; i < 10; i++) {
    NthElement(u.begin(), u.begin() + i, u.end());
    NthElement(v.begin(), v.begin() + i, v.end());
    NthElement(w.begin(), w.begin() + i, w.end());
    std::cout << "The " << i + 1
          << "th element for {u, v, w} are: {"
          << *(u.begin() + i) << ", "
          << *(v.begin() + i) << ", "
          << *(w.begin() + i) << "}" << std::endl;
  }
  return 0;
}