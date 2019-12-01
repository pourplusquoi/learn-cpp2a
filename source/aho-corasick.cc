#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using BeginAt = uint64_t;
using Length = uint64_t;

using Hit = std::pair<BeginAt, Length>;

class Matcher {
 public:
  constexpr Matcher() noexcept = default;
  virtual ~Matcher() noexcept = default;
  [[nodiscard]] virtual std::vector<Hit> Match(
      std::string_view text) const noexcept = 0;
  [[nodiscard]] virtual std::string Serialize() const noexcept = 0;
  [[nodiscard]] virtual std::string ToString() const noexcept = 0;
};

template <typename Mapper, uint64_t Fanout>
class Automaton : public Matcher {
 public:
  template <typename Container>
  constexpr explicit Automaton(Container&& dict) noexcept {
    using ElemT = decltype(*std::declval<Container>().begin());
    static_assert(std::is_convertible_v<ElemT, std::string_view>,
                  "Cannot convert element to std::string_view");

    root_ = std::make_unique<Node>(0, 0);
    for (std::string_view word : dict) {
      Insert(root_.get(), word);
    }
    // Build extra lookup links.
    Init();
  }

  ~Automaton() noexcept override = default;

  // Disallow copy and assign.
  Automaton(const Automaton&) = delete;
  Automaton& operator=(const Automaton&) = delete;

  [[nodiscard]] std::vector<Hit> Match(
      std::string_view text) const noexcept override;

  [[nodiscard]] std::string Serialize() const noexcept override;
  [[nodiscard]] std::string ToString() const noexcept override;

  [[nodiscard]] static std::unique_ptr<Automaton> New(
      std::string_view serialized) noexcept;

 private:
  struct Node {
    bool in_dict;
    const uint64_t depth;
    const uint64_t index;

    Node* const parent;

    Node* suffix;
    Node* dict_suffix;
    std::array<std::unique_ptr<Node>, Fanout> next;

    constexpr Node(uint64_t depth, uint64_t index,
                   Node* parent = nullptr) noexcept
        : in_dict(false), depth(depth), index(index), parent(parent),
          suffix(nullptr), dict_suffix(nullptr) {}
  };
  static_assert(sizeof(Node) == 48 + 8 * Fanout);

  // The private constructor.
  Automaton() = default;

  // Initializes |Automaton|, builds |suffix| and |dict_suffix| links.
  constexpr void Init() noexcept;

  // Inserts |word| into |Automaton|, builds |parent| and |next| links.
  static constexpr void Insert(Node* node, std::string_view word) noexcept;

  // Traverses |Automaton| in pre-order, applys |action| on each node visited
  // at the same time.
  using Fn = std::function<void(const Node* const)>;
  using FnMut = std::function<void(Node* const)>;
  static constexpr void Traverse(
      const Node* node, const Fn& action) noexcept;
  static constexpr void TraverseMut(
      Node* node, const FnMut& action) noexcept;

  // Collects all matches, each of which is a word in dictionay and is the
  // suffix of the initial word we have found.
  static constexpr void CollectMatches(const Node* node, uint64_t end_at,
                                       std::vector<Hit>* hits) noexcept;

  // ...
  static void Serialize(
      const Node* node, const std::unordered_map<const Node*, int64_t>& ids,
      std::string* serialized, char buffer[]) noexcept;

  // ...
  static void ToString(
      const Node* node, const std::unordered_map<const Node*, int64_t>& ids,
      std::string* human_readable) noexcept;

  std::unique_ptr<Node> root_;
};

template <typename Mapper, uint64_t Fanout>
std::vector<Hit> Automaton<Mapper, Fanout>::Match(
    std::string_view text) const noexcept {
  std::vector<Hit> hits;
  const Node* cursor = root_.get();
  for (auto i = 0; i < text.size(); i++) {
    auto index = Mapper{}(text[i]);
    const Node* temp = cursor;
    while (temp != nullptr) {
      const auto* entry = temp->next[index].get();
      if (entry != nullptr) {  // Found match.
        CollectMatches(entry, i + 1, &hits);
        cursor = entry;
        break;
      } else {  // No match found.
        temp = temp->suffix;
      }
    }
    // Get back to root if match failed, as if nothing was seen.
    if (temp == nullptr) {
      cursor = root_.get();
    }
  }
  return hits;
}

template <typename Mapper, uint64_t Fanout>
std::string Automaton<Mapper, Fanout>::Serialize() const noexcept {
  std::string serialized;
  std::unordered_map<const Node*, int64_t> ids;
  ids[nullptr] = -1;

  int64_t next_id = 0;
  Traverse(root_.get(), [&ids, &next_id](const Node* const node) {
    ids[node] = next_id++;
  });

  static_assert(sizeof(char) == 1);
  static_assert(sizeof(int64_t) == 8);
  static_assert(sizeof(uint64_t) == 8);
  constexpr uint32_t kBufferSize = 49 + 16 * Fanout;
  char buffer[kBufferSize];

  Serialize(root_.get(), ids, &serialized, buffer);
  return serialized;
}

template <typename Mapper, uint64_t Fanout>
std::string Automaton<Mapper, Fanout>::ToString() const noexcept {
  std::string human_readable;
  std::unordered_map<const Node*, int64_t> ids;
  ids[nullptr] = -1;

  int64_t next_id = 0;
  Traverse(root_.get(), [&ids, &next_id](const Node* const node) {
    ids[node] = next_id++;
  });

  ToString(root_.get(), ids, &human_readable);
  return human_readable;
}

namespace {
struct Links {
  int64_t suffix_id;
  int64_t dict_suffix_id;
  std::vector<std::pair<uint64_t, int64_t>> next_ids;
};
}  // namespace

template <typename Mapper, uint64_t Fanout>
/*static*/ std::unique_ptr<Automaton<Mapper, Fanout>>
Automaton<Mapper, Fanout>::New(std::string_view serialized) noexcept {
  if (serialized.empty()) {
    return nullptr;
  }

  static_assert(sizeof(char) == 1);
  static_assert(sizeof(int64_t) == 8);
  static_assert(sizeof(uint64_t) == 8);

  std::vector<std::unique_ptr<Node>> owned;
  std::vector<Node*> nodes;
  std::unordered_map<Node*, Links> to_links;

  const char* ptr = serialized.data();
  const char* const end_ptr = serialized.data() + serialized.size();
  while (ptr < end_ptr) {
    const bool in_dict = *reinterpret_cast<const char*>(ptr);
    ptr++;
    const auto depth = *reinterpret_cast<const uint64_t*>(ptr);
    ptr += 8;
    const auto index = *reinterpret_cast<const uint64_t*>(ptr);
    ptr += 8;
    const auto parent_id = *reinterpret_cast<const int64_t*>(ptr);
    ptr += 8;

    auto new_node = std::make_unique<Node>(
        depth, index, (parent_id == -1) ? nullptr : owned[parent_id].get());
    new_node->in_dict = in_dict;
    nodes.emplace_back(new_node.get());
    owned.emplace_back(std::move(new_node));

    const auto suffix_id = *reinterpret_cast<const int64_t*>(ptr);
    ptr += 8;
    const auto dict_suffix_id = *reinterpret_cast<const int64_t*>(ptr);
    ptr += 8;
    const auto count = *reinterpret_cast<const uint64_t*>(ptr);
    ptr += 8;

    std::vector<std::pair<uint64_t, int64_t>> next_ids;
    next_ids.reserve(count);

    for (uint64_t j = 0; j < count; j++) {
      const auto index = *reinterpret_cast<const uint64_t*>(ptr);
      ptr += 8;
      const auto id = *reinterpret_cast<const int64_t*>(ptr);
      ptr += 8;
      next_ids.emplace_back(index, id);
    }
    to_links[nodes.back()] =
        Links{suffix_id, dict_suffix_id, std::move(next_ids)};
  }

  for (Node* node : nodes) {
    assert(to_links.find(node) != to_links.end());
    const Links& links = to_links[node];
    node->suffix =
        (links.suffix_id == -1) ? nullptr : nodes[links.suffix_id];
    node->dict_suffix =
        (links.dict_suffix_id == -1) ? nullptr : nodes[links.dict_suffix_id];
    for (const auto& [index, id] : links.next_ids) {
      node->next[index] = std::move(owned[id]);
    }
  }

  std::unique_ptr<Automaton> automaton(new Automaton);
  automaton->root_ = std::move(owned.front());
  return automaton;
}

template <typename Mapper, uint64_t Fanout>
constexpr void Automaton<Mapper, Fanout>::Init() noexcept {
  // Build suffix link: from each node to the node that is the longest
  // possible strict suffix of it in the graph.
  TraverseMut(root_.get(), [root = root_.get()](Node* const node) {
    if (node == root) {
      return;
    }

    Node* temp = node->parent;
    if (temp == root) {
      node->suffix = root;
      return;
    }
    while (true) {
      temp = temp->suffix;
      if (temp == nullptr) {
        node->suffix = root;
        return;  // Reached root.
      }
      auto* entry = temp->next[node->index].get();
      if (entry != nullptr) {
        node->suffix = entry;
        return;
      }
    }
  });

  // Build dict_suffix link: from each node to the next node in the
  // dictionary that can be reached by following suffix links.
  TraverseMut(root_.get(), [root = root_.get()](Node* const node) {
    if (node == root) {
      return;
    }

    Node* temp = node;
    while (true) {
      Node* recur = temp->suffix;
      if (recur == nullptr) {
        return;  // Reached root.
      }
      if (recur->in_dict) {
        node->dict_suffix = recur;
        return;
      }
      temp = recur;
    }
  });  
}

template <typename Mapper, uint64_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::Insert(
    Node* node, std::string_view word) noexcept {
  if (word.empty()) {
    node->in_dict = true;
    return;
  }
  uint64_t index = Mapper{}(word.front());
  if (node->next[index] == nullptr) {
    node->next[index] = std::make_unique<Node>(node->depth + 1, index, node);
  }
  Insert(node->next[index].get(), word.substr(1));
}

template <typename Mapper, uint64_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::Traverse(
    const Node* node, const Fn& fn) noexcept {
  fn(node);
  for (auto& entry : node->next) {
    if (entry == nullptr) {
      continue;
    }
    Traverse(entry.get(), fn);
  }
}

template <typename Mapper, uint64_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::TraverseMut(
    Node* node, const FnMut& fn_mut) noexcept {
  fn_mut(node);
  for (auto& entry : node->next) {
    if (entry == nullptr) {
      continue;
    }
    TraverseMut(entry.get(), fn_mut);
  }
}

template <typename Mapper, uint64_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::CollectMatches(
    const Node* node, uint64_t end_at, std::vector<Hit>* hits) noexcept {
  if (node->in_dict) {
    hits->emplace_back(end_at - node->depth, node->depth);
  }
  // Collect dict suffix matches.
  while (true) {
    node = node->dict_suffix;
    if (node == nullptr) {
      break;
    }
    hits->emplace_back(end_at - node->depth, node->depth);
  }
}

template <typename Mapper, uint64_t Fanout>
/*static*/ void Automaton<Mapper, Fanout>::Serialize(
    const Node* node, const std::unordered_map<const Node*, int64_t>& ids,
    std::string* serialized, char buffer[]) noexcept {
  uint32_t ptr = 0;
  *reinterpret_cast<char*>(&buffer[ptr]) = node->in_dict;
  ptr++;
  *reinterpret_cast<uint64_t*>(&buffer[ptr]) = node->depth;
  ptr += 8;
  *reinterpret_cast<uint64_t*>(&buffer[ptr]) = node->index;
  ptr += 8;
  *reinterpret_cast<int64_t*>(&buffer[ptr]) = ids.at(node->parent);
  ptr += 8;
  *reinterpret_cast<int64_t*>(&buffer[ptr]) = ids.at(node->suffix);
  ptr += 8;
  *reinterpret_cast<int64_t*>(&buffer[ptr]) = ids.at(node->dict_suffix);
  ptr += 8;

  auto& count = *reinterpret_cast<uint64_t*>(&buffer[ptr]);
  ptr += 8;

  count = 0;
  for (uint64_t i = 0; i < Fanout; i++) {
    const auto& child = node->next[i];
    if (child == nullptr) {
      continue;
    }
    count++;
    *reinterpret_cast<uint64_t*>(&buffer[ptr]) = i;
    ptr += 8;
    *reinterpret_cast<int64_t*>(&buffer[ptr]) = ids.at(child.get());
    ptr += 8;
  }

  *serialized += std::string_view(buffer, ptr);
  for (const auto& child : node->next) {
    if (child == nullptr) {
      continue;
    }
    Serialize(child.get(), ids, serialized, buffer);
  }
}

template <typename Mapper, uint64_t Fanout>
/*static*/ void Automaton<Mapper, Fanout>::ToString(
    const Node* node, const std::unordered_map<const Node*, int64_t>& ids,
    std::string* human_readable) noexcept {
  std::string& str = *human_readable;
  str += "{\n  id: ";
  str += std::to_string(ids.at(node)) + ",\n  in_dict: ";
  str += std::string(node->in_dict ? "YES" : "NO") + ",\n  depth: ";
  str += std::to_string(node->depth) + ",\n  index: ";
  str += std::to_string(node->index) + ",\n  parent: id=";
  str += std::to_string(ids.at(node->parent)) + ",\n  suffix: id=";
  str += std::to_string(ids.at(node->suffix)) + ",\n  dict_suffix: id=";
  str += std::to_string(ids.at(node->dict_suffix)) + ",\n  next: [\n";

  for (uint64_t i = 0; i < Fanout; i++) {
    const auto& child = node->next[i];
    if (child == nullptr) {
      continue;
    }
    str += std::string("    index=") + std::to_string(i) +
        ": id=" + std::to_string(ids.at(child.get())) + ",\n";
  }
  if (str.substr(str.size() - 2) == "[\n") {
    str.pop_back();
  } else {
    str += "  ";
  }
  str += "],\n},\n";

  for (const auto& child : node->next) {
    if (child == nullptr) {
      continue;
    }
    ToString(child.get(), ids, human_readable);
  }
}

struct Mapper {
  constexpr uint64_t operator()(char c) const {
    return static_cast<uint64_t>(c);
  }
};

template <typename Impl, typename Container>
[[nodiscard]] std::unique_ptr<Matcher> NewMatcher(Container&& dict) {
  return std::make_unique<Impl>(std::forward<Container>(dict));
}

int main() {
  constexpr std::array<std::string_view, 7> dict{
      "a", "ab", "bab", "bc", "bca", "c", "caa"};
  constexpr std::string_view text = "abccab";

  std::cout << "Constructing automaton" << std::endl;
  auto matcher = NewMatcher<Automaton<Mapper, 256>>(dict);

  std::cout << "Scanning text: " << text << std::endl;
  auto hits = matcher->Match(text);

  std::cout << "Found " << hits.size() << " hit(s) in total in '"
            << text << "'." << std::endl;
  std::map<std::string_view, std::vector<BeginAt>> stats;
  for (const Hit& hit : hits) {
    stats[text.substr(hit.first, hit.second)].emplace_back(hit.first);
  }
  for (const auto& entry : stats) {
    std::cout << "Found occurrance(s) of '" << entry.first << "': ";
    for (BeginAt begin_at : entry.second) {
      std::cout << "@" << begin_at << " ";
    }
    std::cout << "\n";
  }

  return 0;
}
