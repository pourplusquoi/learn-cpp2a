#include <iostream>
#include <string>
#include <string_view>
#include <utility>

template <typename T>
using Decay = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename... T>
struct List;

template <typename Head, typename Tail>
struct List<Head, Tail> {
  Head val;
  Tail next;
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

template <
    typename Head, typename... Rest,
    typename Return = typename CreateList<Decay<Head>, Decay<Rest>...>::Type>
constexpr Return create_list(Head&& head, Rest&&... rest) {
  return {std::forward<Head>(head), create_list(std::forward<Rest>(rest)...)};
}

template <>
constexpr List<> create_list() {
  return List<>{};
}

template <typename L, typename T>
constexpr List<Decay<T>, Decay<L>> push_front(L&& list, T&& val) {
  return {std::forward<T>(val), std::forward<L>(list)};
}

template <typename L>
constexpr auto pop_front(L&& list) {
  return std::forward<L>(list).next;
}

template <typename L, typename T>
struct PushBack;

template <typename Head, typename Tail, typename T>
struct PushBack<List<Head, Tail>, T> {
  using Type = List<Head, typename PushBack<Tail, T>::Type>;
};

template <typename T>
struct PushBack<List<>, T> {
  using Type = List<T, List<>>;
};

template <typename L, typename T,
          typename Return = typename PushBack<Decay<L>, Decay<T>>::Type>
constexpr Return push_back(L&& list, T&& val);

template <typename Head, typename Tail, typename T,
          typename Return = typename PushBack<List<Head, Tail>, Decay<T>>::Type>
constexpr Return push_back(const List<Head, Tail>& list, T&& val) {
  return {list.val, push_back(list.next, std::forward<T>(val))};
}

template <typename Head, typename Tail, typename T,
          typename Return = typename PushBack<List<Head, Tail>, Decay<T>>::Type>
constexpr Return push_back(List<Head, Tail>& list, T&& val) {
  return {list.val, push_back(list.next, std::forward<T>(val))};
}

template <typename Head, typename Tail, typename T,
          typename Return = typename PushBack<List<Head, Tail>, Decay<T>>::Type>
constexpr Return push_back(const List<Head, Tail>&& list, T&& val) {
  return {std::move(list).val,
          push_back(std::move(list).next, std::forward<T>(val))};
}

template <typename Head, typename Tail, typename T,
          typename Return = typename PushBack<List<Head, Tail>, Decay<T>>::Type>
constexpr Return push_back(List<Head, Tail>&& list, T&& val) {
  return {std::move(list).val,
          push_back(std::move(list).next, std::forward<T>(val))};
}

template <typename T>
constexpr List<Decay<T>, List<>> push_back(const List<>& list, T&& val) {
  return {std::forward<T>(val), list};
}

template <typename T>
constexpr List<Decay<T>, List<>> push_back(List<>& list, T&& val) {
  return {std::forward<T>(val), list};
}

template <typename T>
constexpr List<Decay<T>, List<>> push_back(const List<>&& list, T&& val) {
  return {std::forward<T>(val), std::move(list)};
}

template <typename T>
constexpr List<Decay<T>, List<>> push_back(List<>&& list, T&& val) {
  return {std::forward<T>(val), std::move(list)};
}

template <typename L>
struct PopBack;

template <typename Head, typename Next, typename Tail>
struct PopBack<List<Head, List<Next, Tail>>> {
  using Type = List<Head, typename PopBack<List<Next, Tail>>::Type>;
};

template <typename Head>
struct PopBack<List<Head, List<>>> {
  using Type = List<>;
};

template <typename L, typename Return = typename PopBack<Decay<L>>::Type>
constexpr Return pop_back(L&& list);

template <
    typename Head, typename Next, typename Tail,
    typename Return = typename PopBack<List<Head, List<Next, Tail>>>::Type>
constexpr Return pop_back(const List<Head, List<Next, Tail>>& list) {
  return {list.head, pop_back(list.next)};
}

template <
    typename Head, typename Next, typename Tail,
    typename Return = typename PopBack<List<Head, List<Next, Tail>>>::Type>
constexpr Return pop_back(List<Head, List<Next, Tail>>& list) {
  return {list.head, pop_back(list.next)};
}

template <
    typename Head, typename Next, typename Tail,
    typename Return = typename PopBack<List<Head, List<Next, Tail>>>::Type>
constexpr Return pop_back(const List<Head, List<Next, Tail>>&& list) {
  return {std::move(list).val, pop_back(std::move(list).next)};
}

template <
    typename Head, typename Next, typename Tail,
    typename Return = typename PopBack<List<Head, List<Next, Tail>>>::Type>
constexpr Return pop_back(List<Head, List<Next, Tail>>&& list) {
  return {std::move(list).val, pop_back(std::move(list).next)};
}

template <typename Head>
constexpr List<> pop_back(const List<Head, List<>>& list) {
  return list.next;
}

template <typename Head>
constexpr List<> pop_back(List<Head, List<>>& list) {
  return list.next;
}

template <typename Head>
constexpr List<> pop_back(const List<Head, List<>>&& list) {
  return std::move(list).next;
}

template <typename Head>
constexpr List<> pop_back(List<Head, List<>>&& list) {
  return std::move(list).next;
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
          typename Return =
              typename ConcatLists<List<Head, Tail>, Decay<Rest>...>::Type>
constexpr Return concat_lists(const List<Head, Tail>& first, Rest&&... rest) {
  return {first.val, concat_lists(first.next, std::forward<Rest>(rest)...)};
}

template <typename Head, typename Tail, typename... Rest,
          typename Return =
              typename ConcatLists<List<Head, Tail>, Decay<Rest>...>::Type>
constexpr Return concat_lists(List<Head, Tail>&& first, Rest&&... rest) {
  return {std::move(first).val,
          concat_lists(std::move(first).next, std::forward<Rest>(rest)...)};
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
  return List<>{};
}

template <typename T>
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
constexpr Return reverse_list(const List<Head, Tail>&& list) {
  return concat_lists(reverse_list(std::move(list).next),
                      create_list(std::move(list).val));
}

template <typename Head, typename Tail,
          typename Return = typename ReverseList<List<Head, Tail>>::Type>
constexpr Return reverse_list(List<Head, Tail>&& list) {
  return concat_lists(reverse_list(std::move(list).next),
                      create_list(std::move(list).val));
}

template <>
constexpr List<> reverse_list(const List<>& list) {
  return list;
}

template <>
constexpr List<> reverse_list(List<>& list) {
  return list;
}

template <>
constexpr List<> reverse_list(const List<>&& list) {
  return list;
}

template <>
constexpr List<> reverse_list(List<>&& list) {
  return list;
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
inline std::string to_string(const List<Head, Tail>&& list) {
  Serializer<decltype((std::move(list).val))> serializer;
  return serializer((std::move(list).val)) + " -> " +
         to_string<Serializer>(std::move(list).next);
}

template <template <typename> class Serializer, typename Head, typename Tail>
inline std::string to_string(List<Head, Tail>&& list) {
  Serializer<decltype((std::move(list).val))> serializer;
  return serializer(std::move(list).val) + " -> " +
         to_string<Serializer>(std::move(list).next);
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
inline std::string to_string(const List<>&& _) {
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
struct DefaultSerializer<const std::string_view&> {
  inline std::string operator()(const std::string_view& val) const {
    return std::string(val);
  }
};

template <>
struct DefaultSerializer<std::string_view&> {
  inline std::string operator()(const std::string_view& val) const {
    return std::string(val);
  }
};

template <>
struct DefaultSerializer<const std::string_view&&> {
  inline std::string operator()(const std::string_view&& val) const {
    return std::string(std::move(val));
  }
};

template <>
struct DefaultSerializer<std::string_view&&> {
  inline std::string operator()(const std::string_view&& val) const {
    return std::string(std::move(val));
  }
};

int main() {
  using namespace std::literals;
  constexpr auto list1 =
      create_list(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
  constexpr auto list2 = create_list("hello"sv, "world"sv, "lol"sv);
  constexpr auto list3 = push_front(std::move(list2), "front"sv);
  constexpr auto list4 = push_back(std::move(list3), "back"sv);
  constexpr auto list5 = concat_lists(std::move(list1), std::move(list4),
                                      reverse_list(std::move(list4)),
                                      reverse_list(std::move(list1)));
  std::cout << to_string<DefaultSerializer>(std::move(list5)) << std::endl;
  constexpr auto list6 = pop_front(pop_front(std::move(list5)));
  constexpr auto list7 = pop_back(pop_back(std::move(list6)));
  std::cout << to_string<DefaultSerializer>(std::move(list7)) << std::endl;
}
