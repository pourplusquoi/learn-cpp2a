#include <iostream>
#include <string>
#include <utility>

template <typename T>
using Decay = std::remove_cv_t<std::remove_reference_t<T>>;

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
constexpr List<Decay<T>...> create_list(T&&... vals);

template <typename Head, typename... Rest,
          typename Return = typename CreateList<Decay<Head>,
                                                Decay<Rest>...>::Type>
constexpr Return create_list(Head&& head, Rest&&... rest) {
  return Return {
    std::forward<Head>(head),
    create_list(std::forward<Rest>(rest)...)
  };
}

template <>
constexpr List<> create_list() {
  return List<> {};
}

template <typename... T>
struct ConcatLists;

template <typename Head, typename Tail, typename... Rest>
struct ConcatLists<List<Head, Tail>, Rest...> {
  using Type = List<Head, typename ConcatLists<Tail, Rest...>::Type>;
};

template <typename... Rest>
struct ConcatLists<List<>, Rest...> {
  using Type = typename ConcatLists<Rest...>::Type;
};

template <>
struct ConcatLists<> {
  using Type = List<>;
};

template <typename... T, typename Return = typename ConcatLists<T...>::Type>
constexpr Return concat_lists(T&&... lists);

template <typename Head, typename Tail, typename... Rest,
          typename Return = typename ConcatLists<List<Head, Tail>,
                                                 Decay<Rest>...>::Type>
constexpr Return concat_lists(const List<Head, Tail>& first, Rest&&... rest) {
  return Return {
    first.val,
    concat_lists(first.next, std::forward<Rest>(rest)...)
  };
}

template <typename Head, typename Tail, typename... Rest,
          typename Return = typename ConcatLists<List<Head, Tail>,
                                                 Decay<Rest>...>::Type>
constexpr Return concat_lists(List<Head, Tail>&& first, Rest&&... rest) {
  return Return {
    std::move(first.val),
    concat_lists(std::move(first.next), std::forward<Rest>(rest)...)
  };
}

template <typename... Rest,
          typename Return = typename ConcatLists<Decay<Rest>...>::Type>
constexpr Return concat_lists(const List<>& _, Rest&&... rest) {
  return concat_lists(std::forward<Rest>(rest)...);
}

template <typename... Rest,
          typename Return = typename ConcatLists<Decay<Rest>...>::Type>
constexpr Return concat_lists(List<>&& _, Rest&&... rest) {
  return concat_lists(std::forward<Rest>(rest)...);
}

template <>
constexpr List<> concat_lists() {
  return List<> {};
}

template <typename... T>
struct ReverseList;

template <typename Head, typename Tail>
struct ReverseList<List<Head, Tail>> {
  using Type = typename ConcatLists<typename ReverseList<Tail>::Type,
                                    List<Head, List<>>>::Type;
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
  return concat_lists(reverse_list(list.next), create_list(list.val));
}

template <typename Head, typename Tail,
          typename Return = typename ReverseList<List<Head, Tail>>::Type>
constexpr Return reverse_list(List<Head, Tail>& list) {
  return concat_lists(reverse_list(list.next), create_list(list.val));
}

template <typename Head, typename Tail,
          typename Return = typename ReverseList<List<Head, Tail>>::Type>
constexpr Return reverse_list(List<Head, Tail>&& list) {
  return concat_lists(reverse_list(std::move(list.next)),
                      create_list(std::move(list.val)));
}

template <>
constexpr List<> reverse_list(const List<>& _) {
  return List<> {};
}

template <>
constexpr List<> reverse_list(List<>& _) {
  return List<> {};
}

template <>
constexpr List<> reverse_list(List<>&& _) {
  return List<> {};
}

template <template <typename> class Serializer, typename T>
inline std::string to_string(T&& list);

template <template <typename> class Serializer, typename Head, typename Tail>
inline std::string to_string(const List<Head, Tail>& list) {
  Serializer<decltype((list.val))> serializer;
  return serializer(list.val) + " -> " + to_string<Serializer>(list.next);
}

template <template <typename> class Serializer, typename Head, typename Tail>
inline std::string to_string(List<Head, Tail>& list) {
  Serializer<decltype((list.val))> serializer;
  return serializer(list.val) + " -> " + to_string<Serializer>(list.next);
}

template <template <typename> class Serializer, typename Head, typename Tail>
inline std::string to_string(List<Head, Tail>&& list) {
  Serializer<decltype(std::move(list.val))> serializer;
  return serializer(std::move(list.val)) + " -> " +
      to_string<Serializer>(std::move(list.next));
}

template <template <typename> class Serializer>
inline std::string to_string(const List<>& _) {
  return "null";
}

template <template <typename> class Serializer>
inline std::string to_string(List<>& _) {
  return "null";
}

template <template <typename> class Serializer>
inline std::string to_string(List<>&& _) {
  return "null";
}

template <typename T>
struct DefaultSerializer {
  inline std::string operator()(T&& val) const {
    return std::to_string(std::forward<T>(val));
  }
};

template <>
struct DefaultSerializer<const std::string&> {
  inline std::string operator()(const std::string& val) const {
    return val;
  }
};

template <>
struct DefaultSerializer<std::string&&> {
  inline std::string operator()(std::string&& val) const {
    return std::move(val);
  }
};

int main () {
  constexpr auto list1 =
      create_list(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
  auto list2 = create_list(std::string("hello"),
                           std::string("world"),
                           std::string("lol"));
  auto result = concat_lists(list1, list2,
                             reverse_list(list2), reverse_list(list1));
  std::cout << to_string<DefaultSerializer>(std::move(result)) << std::endl;
}