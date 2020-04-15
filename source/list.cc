#include <iostream>
#include <string>
#include <utility>

template <typename... T>
struct List;

template <typename Head, typename... Rest>
struct List<Head, List<Rest...>> {
  Head val;
  List<Rest...> next;
};

template <>
struct List<> {};

template <typename... T>
struct CreateList;

template <typename Head, typename... Rest>
struct CreateList<Head, Rest...> {
  using Type = List<Head, typename CreateList<Rest...>::Type>;
};

template <>
struct CreateList<> {
  using Type = List<>;
};

template <typename... T>
constexpr List<std::remove_reference_t<T>...> create_list(T&&... vals);

template <typename Head, typename... Rest,
          typename Return = typename CreateList<std::remove_reference_t<Head>,
                                                std::remove_reference_t<Rest>...>::Type>
constexpr Return create_list(Head&& head, Rest&&... rest) {
  return Return { std::forward<Head>(head), create_list(std::forward<Rest>(rest)...) };
}

template <>
constexpr List<> create_list() {
  return List<> {};
}

template <typename... T>
struct CombineLists;

template <typename Head, typename Tail, typename... Rest>
struct CombineLists<List<Head, Tail>, Rest...> {
  using Type = List<Head, typename CombineLists<Tail, Rest...>::Type>;
};

template <typename... Rest>
struct CombineLists<List<>, Rest...> {
  using Type = typename CombineLists<Rest...>::Type;
};

template <>
struct CombineLists<> {
  using Type = List<>;
};

template <typename T>
using Decay = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename... T, typename Return = typename CombineLists<T...>::Type>
constexpr Return combine_lists(T&&... lists);

template <typename Head, typename Tail, typename... Rest,
          typename Return = typename CombineLists<List<Head, Tail>,
                                                  Decay<Rest>...>::Type>
constexpr Return combine_lists(const List<Head, Tail>& first, Rest&&... rest) {
  return Return { first.val, combine_lists(first.next, std::forward<Rest>(rest)...) };
}

template <typename Head, typename Tail, typename... Rest,
          typename Return = typename CombineLists<List<Head, Tail>,
                                                  Decay<Rest>...>::Type>
constexpr Return combine_lists(List<Head, Tail>&& first, Rest&&... rest) {
  return Return { first.val, combine_lists(first.next, std::forward<Rest>(rest)...) };
}

template <typename... Rest,
          typename Return = typename CombineLists<Decay<Rest>...>::Type>
constexpr Return combine_lists(const List<>& _, Rest&&... rest) {
  return combine_lists(std::forward<Rest>(rest)...);
}

template <typename... Rest,
          typename Return = typename CombineLists<Decay<Rest>...>::Type>
constexpr Return combine_lists(List<>&& _, Rest&&... rest) {
  return combine_lists(std::forward<Rest>(rest)...);
}

template <>
constexpr List<> combine_lists() {
  return List<> {};
}

template <typename... T>
struct ReverseList;

template <typename Head, typename Tail>
struct ReverseList<List<Head, Tail>> {
  using Type = typename CombineLists<typename ReverseList<Tail>::Type, List<Head, List<>>>::Type;
};

template <>
struct ReverseList<List<>> {
  using Type = List<>;
};

template <typename T, typename Return = typename ReverseList<Decay<T>>::Type>
constexpr Return reverse_list(T&& list);

template <typename Head, typename Tail,
          typename Return = typename ReverseList<List<Head, Tail>>::Type>
constexpr Return reverse_list(const List<Head, Tail>& list) {
  return combine_lists(reverse_list(list.next),
                       create_list(const_cast<Head&>(list.val)));
}

template <typename Head, typename Tail,
          typename Return = typename ReverseList<List<Head, Tail>>::Type>
constexpr Return reverse_list(List<Head, Tail>&& list) {
  return combine_lists(reverse_list(list.next), create_list(list.val));
}

template <>
constexpr List<> reverse_list(const List<>& list) {
  return List<> {};
}

template <>
constexpr List<> reverse_list(List<>&& list) {
  return List<> {};
}

template <typename... T>
std::string to_string(const List<T...>& list);

template <typename... T>
std::string to_string(List<T...>&& list);

template <typename Head, typename... Rest>
std::string to_string(const List<Head, List<Rest...>>& list) {
  return std::to_string(list.val) + " -> " + to_string(list.next);
}

template <typename Head, typename... Rest>
std::string to_string(List<Head, List<Rest...>>&& list) {
  return std::to_string(list.val) + " -> " + to_string(list.next);
}

template <>
std::string to_string(const List<>& list) {
  return "null";
}

template <>
std::string to_string(List<>&& list) {
  return "null";
}

int main () {
  constexpr auto list1 = create_list(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
  constexpr auto list2 = create_list(17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32);
  constexpr auto result = combine_lists(list1, list2, reverse_list(list2), reverse_list(list1));
  std::cout << to_string(result) << std::endl;
}