#include <array>
#include <iostream>

template <typename T>
constexpr void swap(T& a, T& b) {
  T c = std::move(a);
  a = std::move(b);
  b = std::move(c);
}

template <
    typename T,
    typename E = decltype(std::declval<T>()[0]),
    typename Compare = std::less<E>>
constexpr void swim(T&& nums, std::size_t idx) {
  Compare lt;
  while (idx > 0) {
    auto par = (idx - 1) / 2;
    if (!lt(nums[idx], nums[par])) {
      break;
    }
    swap(nums[idx], nums[par]);
    idx = par;
  }
}

template <
    typename T,
    typename E = decltype(std::declval<T>()[0]),
    typename Compare = std::less<E>>
constexpr void sink(T&& nums, std::size_t end) {
  Compare lt;
  auto idx = 0;
  while (idx < end) {
    const auto lhs = idx * 2 + 1;
    if (lhs >= end) {
      break;
    }
    const auto rhs = lhs + 1;
    const bool cmp = (rhs >= end) || lt(nums[lhs], nums[rhs]);
    auto& child = cmp ? nums[lhs] : nums[rhs];
    if (!lt(child, nums[idx])) {
      break;
    }
    swap(nums[idx], child);
    idx = cmp ? lhs : rhs;
  }
}

template <
    typename T,
    typename E = decltype(std::declval<T>()[0]),
    typename Compare = std::less<E>>
constexpr void make_heap(T&& nums) {
  for (auto i = 0; i < nums.size(); i++) {
    swim(std::forward<T>(nums), i);
  }
}

template <
    typename T,
    typename E = decltype(std::declval<T>()[0]),
    typename Compare = std::less<E>>
constexpr decltype(auto) heap_sort(T&& nums) {
  auto heap = nums;
  make_heap(heap);
  auto sorted = heap;
  for (std::size_t i = 0, j = heap.size() - 1; i < heap.size(); i++, j--) {
    sorted[i] = std::move(heap[0]);
    heap[0] = std::move(heap[j]);
    sink(heap, j);
  }
  return sorted;
}

int main() {
  constexpr auto input =
      std::array{28, 5, 24, 13, 2, 14, 30, 23, 4, 10, 21, 25, 29, 1, 12, 16,
                 7, 6, 26, 3, 15, 31, 18, 8, 20, 0, 22, 19, 11, 17, 27, 9};
  constexpr auto output = heap_sort(std::move(input));
  for (const auto& num : output) {
    std::cout << num << std::endl;
  }
}
