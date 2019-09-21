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

using Hit = std::pair<std::size_t, std::size_t>;

template <std::size_t Fanout>
class Matcher {
 public:
  Matcher() = default;
  virtual ~Matcher() = default;
  virtual std::vector<Hit> Match(std::string_view text) const = 0;
};

template <typename Mapper, std::size_t Fanout>
class Automaton : public Matcher<Fanout> {
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

  void RetriveMatches(Node* node, std::size_t end,
                      std::vector<Hit>* hits) const;

  std::unique_ptr<Node> root_;
};

template <typename Mapper, std::size_t Fanout>
Automaton<Mapper, Fanout>::Automaton(
    const std::vector<std::string_view>& dict) {
  root_ = std::make_unique<Node>(0, 0);
  for (auto word : dict) {
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
        RetriveMatches(entry.get(), i + 1, &hits);
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
void Automaton<Mapper, Fanout>::RetriveMatches(
    Node* node, std::size_t end_at, std::vector<Hit>* hits) const {
  if (node->in_dict) {
    hits->emplace_back(end_at - node->depth, node->depth);
  }
  // Retrive dict suffix matches.
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
      auto& entry = temp->next[node->index];
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
  std::unique_ptr<Matcher<256>> matcher =
      std::make_unique<Automaton<Mapper, 256>>(
          std::vector<std::string_view>{
              "hello", "world", "hell", "low", "or", "a", "the", "and"});
  
  std::string_view text(
      "Nineteen Eighty-Four is set in Oceania, one of three inter-continental "
      "superstates that divided the world after a global war. Smith's memories "
      "and his reading of the proscribed book, The Theory and Practice of "
      "Oligarchical Collectivism by Emmanuel Goldstein, reveal that after the "
      "Second World War, the United Kingdom became involved in a war during "
      "the early 1950s in which nuclear weapons destroyed hundreds of cities "
      "in Europe, western Russia and North America. Colchester was destroyed "
      "and London also suffered widespread aerial raids, leading Winston's "
      "family to take refuge in a London Underground station. Britain fell "
      "into civil war, with street fighting in London, before the English "
      "Socialist Party, abbreviated as Ingsoc, emerged victorious and formed a "
      "totalitarian government in Britain. The British Commonwealth and Latin "
      "America were absorbed by the United States, resulting in the superstate "
      "of Oceania. Ingsoc became the sole government party in this new nation. "
      "Simultaneously, the Soviet Union conquered continental Europe and "
      "established the second superstate of Eurasia, under a Neo-Bolshevik "
      "regime. The third superstate of Eastasia emerged in the Far East after "
      "another decade of fighting, with a ruling ideology translated from "
      "Chinese as 'Death-Worship' or 'Obliteration of the Self'. The three "
      "superstates wage perpetual war for the remaining unconquered lands of "
      "the world in \"a rough quadrilateral with its corners at Tangier, "
      "Brazzaville, Darwin, and Hong Kong\" through constantly shifting "
      "alliances. Although each of the three states are said to have sufficient "
      "natural resources, the war continues in order to maintain ideological "
      "control over the people. While citizens in each state are trained to "
      "despise the ideologies of the other two as uncivilised and barbarous, "
      "Goldstein's book, The Theory and Practice of Oligarchical Collectivism, "
      "explains that in fact the superstates' ideologies are practically "
      "identical and that the public's ignorance of this fact is imperative so "
      "that they might continue believing otherwise. The only references to the "
      "exterior world for the Oceanian citizenry (the Outer Party and the "
      "Proles) are Ministry of Truth maps and propaganda to ensure their belief "
      "in \"the war\". However, due to the fact that Winston barely remembers "
      "these events and due to the Party's manipulation of historical records, "
      "the continuity and accuracy of these events are unclear. Winston himself "
      "notes that the Party has claimed credit for inventing helicopters, "
      "airplanes and trains, while Julia theorises that the perpetual bombing of "
      "London is merely a false-flag operation designed to convince the populace "
      "that a war is occurring. If the official account was accurate, Smith's "
      "strengthening memories and the story of his family's dissolution suggest "
      "that the atomic bombings occurred first, followed by civil war featuring "
      "\"confused street fighting in London itself\" and the societal postwar "
      "reorganisation, which the Party retrospectively calls \"the Revolution\".");
  std::cout << "Scanning text: " << text << std::endl;

  std::vector<Hit> hits = matcher->Match(text);

  std::cout << "\n";
  std::unordered_map<std::string_view, std::vector<std::size_t>> stats;
  for (const Hit& hit : hits) {
    stats[text.substr(hit.first, hit.second)].emplace_back(hit.first);
  }
  for (const auto& entry : stats) {
    std::cout << "Found occurrance of '" << entry.first << "':";
    for (std::size_t begin_at : entry.second) {
      std::cout << " @" << begin_at;
    }
    std::cout << "\n\n";
  }
  std::cout << std::endl;

  return 0;
}