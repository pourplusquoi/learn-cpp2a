#include <array>
#include <iostream>

template <typename T>
constexpr std::pair<T, T> swap(const T& a, const T& b) {
  return {b, a};
}

template <typename T, std::size_t N, typename Compare = std::less<T>>
constexpr decltype(auto) swim(const std::array<T, N>& nums, std::size_t idx) {
  Compare lt;
  auto ret = nums;
  while (idx > 0) {
    auto par = (idx - 1) / 2;
    if (!lt(ret[idx], ret[par])) {
      break;
    }
    const auto& [a, b] = swap(ret[idx], ret[par]);
    ret[idx] = std::move(a);
    ret[par] = std::move(b);
    idx = par;
  }
  return ret;
}

template <typename T, std::size_t N, typename Compare = std::less<T>>
constexpr decltype(auto) sink(const std::array<T, N>& nums, std::size_t end) {
  Compare lt;
  auto ret = nums;
  auto idx = 0;
  while (idx < end) {
    const auto lhs = idx * 2 + 1;
    if (lhs >= end) {
      break;
    }
    const auto rhs = lhs + 1;
    const bool cmp = (rhs >= end) || lt(ret[lhs], ret[rhs]);
    auto& child = cmp ? ret[lhs] : ret[rhs];
    if (!lt(child, ret[idx])) {
      break;
    }
    const auto& [a, b] = swap(ret[idx], child);
    ret[idx] = std::move(a);
    child = std::move(b);
    idx = cmp ? lhs : rhs;
  }
  return ret;
}

template <typename T, std::size_t N, typename Compare = std::less<T>>
constexpr std::array<T, N> make_heap(const std::array<T, N>& nums) {
  auto ret = nums;
  for (auto i = 0; i < N; i++) {
    ret = swim(ret, i);
  }
  return ret;
}

template <typename T, std::size_t N, typename Compare = std::less<T>>
constexpr std::array<T, N> heap_sort(const std::array<T, N>& nums) {
  auto heap = make_heap(nums);
  auto ret = heap;
  for (std::size_t i = 0, j = N - 1; i < N; i++, j--) {
    ret[i] = std::move(heap[0]);
    heap[0] = std::move(heap[j]);
    heap = sink(heap, j);
  }
  return ret;
}

int main() {
  constexpr auto input =
      std::array{28, 5, 24, 13, 2, 14, 30, 23, 4, 10, 21, 25, 29, 1, 12, 16,
                 7, 6, 26, 3, 15, 31, 18, 7, 20, 0, 22, 19, 11, 17, 27, 9};
  constexpr auto output = heap_sort(input);
  for (const auto& num : output) {
    std::cout << num << std::endl;
  }
}