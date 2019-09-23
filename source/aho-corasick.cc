#include <array>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using BeginAt = std::size_t;
using Length = std::size_t;

using Hit = std::pair<BeginAt, Length>;

class Matcher {
 public:
  Matcher() = default;
  virtual ~Matcher() = default;
  virtual std::vector<Hit> Match(std::string_view text) const = 0;
};

template <typename Mapper, std::size_t Fanout>
class Automaton : public Matcher {
 public:
  explicit Automaton(const std::vector<std::string_view>& dict);
  ~Automaton() override = default;

  std::vector<Hit> Match(std::string_view text) const override;

 private:
  struct Node {
    bool in_dict;
    const std::size_t depth;

    Node* parent;
    std::size_t index;

    Node* suffix;
    Node* dict_suffix;
    std::array<std::unique_ptr<Node>, Fanout> next;

    Node(std::size_t depth, std::size_t index) : in_dict(false),
                                                 depth(depth),
                                                 index(index) {}
  };

  void Insert(Node* node, std::string_view word);
  void Traverse(Node* node, std::function<void(Node* const)> action);
  void Initialize();

  void CollectMatches(Node* node, std::size_t end_at,
                      std::vector<Hit>* hits) const;

  std::unique_ptr<Node> root_;
};

template <typename Mapper, std::size_t Fanout>
Automaton<Mapper, Fanout>::Automaton(
    const std::vector<std::string_view>& dict) {
  root_ = std::make_unique<Node>(0, 0);
  for (std::string_view word : dict) {
    Insert(root_.get(), word);
  }
  // Build extra lookup links.
  Initialize();
}

template <typename Mapper, std::size_t Fanout>
std::vector<Hit> Automaton<Mapper, Fanout>::Match(
    std::string_view text) const {
  std::vector<Hit> hits;
  Node* cursor = root_.get();
  for (std::size_t i = 0; i < text.size(); i++) {
    std::size_t index = Mapper{}(text[i]);
    Node* temp = cursor;
    while (temp != nullptr) {
      const auto& entry = temp->next[index];
      if (entry != nullptr) {  // Found match.
        CollectMatches(entry.get(), i + 1, &hits);
        cursor = entry.get();
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
void Automaton<Mapper, Fanout>::CollectMatches(
    Node* node, std::size_t end_at, std::vector<Hit>* hits) const {
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

template <typename Mapper, std::size_t Fanout>
void Automaton<Mapper, Fanout>::Insert(Node* node, std::string_view word) {
  if (word.empty()) {
    node->in_dict = true;
    return;
  }
  std::size_t index = Mapper{}(word.front());
  auto& entry = node->next[index];
  if (entry == nullptr) {
    entry = std::make_unique<Node>(node->depth + 1, index);
    entry->parent = node;
  }
  Insert(entry.get(), word.substr(1));
}

template <typename Mapper, std::size_t Fanout>
void Automaton<Mapper, Fanout>::Traverse(
    Node* node, std::function<void(Node* const)> action) {
  for (auto& child : node->next) {
    if (child != nullptr) {
      action(child.get());
      Traverse(child.get(), action);
    }
  }
}

template <typename Mapper, std::size_t Fanout>
void Automaton<Mapper, Fanout>::Initialize() {
  // Build suffix link: from each node to the node that is the longest
  // possible strict suffix of it in the graph.
  Traverse(root_.get(), [&](Node* const node) {
    Node* temp = node->parent;
    if (temp == root_.get()) {
      node->suffix = root_.get();
      return;
    }
    while (true) {
      temp = temp->suffix;
      if (temp == nullptr) {
        node->suffix = root_.get();
        return;  // Reached root.
      }
      const auto& entry = temp->next[node->index];
      if (entry != nullptr) {
        node->suffix = entry.get();
        return;
      }
    }
  });

  // Build dict_suffix link: from each node to the next node in the
  // dictionary that can be reached by following suffix links.
  Traverse(root_.get(), [&](Node* const node) {
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

struct Mapper {
  std::size_t operator()(char c) const {
    return static_cast<std::size_t>(c);
  }
};

int main() {
  std::unique_ptr<Matcher> matcher =
      std::make_unique<Automaton<Mapper, 256>>(
          std::vector<std::string_view>{"a", "ab", "bab", "bc",
                                        "bca", "c", "caa"});
  
  std::string_view text("abccab");
  std::cout << "Scanning text: " << text << std::endl;

  std::vector<Hit> hits = matcher->Match(text);

  std::cout << "\n";
  std::unordered_map<std::string_view, std::vector<BeginAt>> stats;
  for (const Hit& hit : hits) {
    stats[text.substr(hit.first, hit.second)].emplace_back(hit.first);
  }
  for (const auto& entry : stats) {
    std::cout << "Found occurrance(s) of '" << entry.first << "':";
    for (BeginAt begin_at : entry.second) {
      std::cout << " @" << begin_at;
    }
    std::cout << "\n\n";
  }
  std::cout << std::endl;

  return 0;
}