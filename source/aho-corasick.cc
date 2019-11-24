#include <array>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using BeginAt = std::size_t;
using Length = std::size_t;

using Hit = std::pair<BeginAt, Length>;

class Matcher {
 public:
  constexpr Matcher() = default;
  virtual ~Matcher() = default;
  virtual std::vector<Hit> Match(std::string_view text) const = 0;
};

template <typename Mapper, std::size_t Fanout>
class Automaton : public Matcher {
 public:
  constexpr explicit Automaton(const std::vector<std::string>& dict);
  ~Automaton() override = default;

  std::vector<Hit> Match(std::string_view text) const override;

 private:
  struct Node {
    bool in_dict;
    const std::size_t depth;

    Node* const parent;
    const std::size_t index;

    Node* suffix;
    Node* dict_suffix;
    std::array<std::unique_ptr<Node>, Fanout> next;

    constexpr Node(std::size_t depth, std::size_t index,
                   Node* parent = nullptr)
        : in_dict(false), depth(depth), index(index), parent(parent) {}
  };

  constexpr void Init();

  constexpr static void Insert(Node* node, std::string_view word);
  constexpr static void Traverse(Node* node,
                                 std::function<void(Node* const)> action);
  constexpr static void CollectMatches(const Node* node, std::size_t end_at,
                                       std::vector<Hit>* hits);

  std::unique_ptr<Node> root_;
};

template <typename Mapper, std::size_t Fanout>
constexpr Automaton<Mapper, Fanout>::Automaton(
    const std::vector<std::string>& dict) {
  root_ = std::make_unique<Node>(0, 0);
  for (std::string_view word : dict) {
    Insert(root_.get(), word);
  }
  // Build extra lookup links.
  Init();
}

template <typename Mapper, std::size_t Fanout>
std::vector<Hit> Automaton<Mapper, Fanout>::Match(
    std::string_view text) const {
  std::vector<Hit> hits;
  const Node* cursor = root_.get();
  for (std::size_t i = 0; i < text.size(); i++) {
    std::size_t index = Mapper{}(text[i]);
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

template <typename Mapper, std::size_t Fanout>
constexpr void Automaton<Mapper, Fanout>::Init() {
  // Build suffix link: from each node to the node that is the longest
  // possible strict suffix of it in the graph.
  Traverse(root_.get(), [root = root_.get()](Node* const node) {
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
  Traverse(root_.get(), [](Node* const node) {
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

template <typename Mapper, std::size_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::Insert(
    Node* node, std::string_view word) {
  if (word.empty()) {
    node->in_dict = true;
    return;
  }
  std::size_t index = Mapper{}(word.front());
  if (node->next[index] == nullptr) {
    node->next[index] = std::make_unique<Node>(node->depth + 1, index, node);
  }
  Insert(node->next[index].get(), word.substr(1));
}

template <typename Mapper, std::size_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::Traverse(
    Node* node, std::function<void(Node* const)> action) {
  for (auto it = node->next.begin(); it != node->next.end(); ++it) {
    if (*it != nullptr) {
      action(it->get());
      Traverse(it->get(), action);
    }
  }
}

template <typename Mapper, std::size_t Fanout>
/*static*/ constexpr void Automaton<Mapper, Fanout>::CollectMatches(
    const Node* node, std::size_t end_at, std::vector<Hit>* hits) {
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

struct Mapper {
  constexpr std::size_t operator()(char c) const {
    return static_cast<std::size_t>(c);
  }
};

int main() {
  for (int i=0; i < 100000; i++) {
    std::cout << i << std::endl;
  std::vector<std::string> dict{"a", "ab", "bab", "bc",
                                "bca", "c", "caa"};
  constexpr std::string_view text = "abccab";

  std::cout << "Constructing automaton" << std::endl;
  std::unique_ptr<Matcher> matcher =
      std::make_unique<Automaton<Mapper, 256>>(dict);

  std::cout << "Scanning text: " << text << std::endl;
  std::vector<Hit> hits = matcher->Match(text);

  std::cout << "Found " << hits.size() << " hit(s) in total in '" << text << "'." << std::endl;
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
  }}

  return 0;
}
