#include "primer/trie.h"
#include <any>
#include <string_view>
#include "common/exception.h"
#include "fmt/chrono.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  if (!root_) {
    return nullptr;
  }
  if (key.empty()) {
    if (!root_->is_value_node_) {
      return nullptr;
    }
    auto val_ptr = dynamic_cast<const TrieNodeWithValue<T> *>(root_.get());
    if (val_ptr == nullptr) {
      return nullptr;
    }
    return val_ptr->value_.get();
  }
  auto p = root_;
  for (const auto &ch : key) {
    if (!p->Contains(ch)) {
      return nullptr;
    }
    p = p->children_.at(ch);
  }
  auto val_ptr = dynamic_cast<const TrieNodeWithValue<T> *>(p.get());
  if (val_ptr == nullptr) {
    return nullptr;
  }
  if (!val_ptr->is_value_node_) {
    return nullptr;
  }

  return val_ptr->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  auto root = root_;
  if (!root) {
    root = std::make_shared<const TrieNode>();
  }
  auto dfs = [&](auto &&self, std::string_view kv,
                 const std::shared_ptr<const TrieNode> &node) -> std::shared_ptr<const TrieNode> {
    if (kv.empty()) {
      return std::make_shared<TrieNodeWithValue<T>>(node->children_, std::make_shared<T>(std::move(value)));
    }
    char ch = kv.front();
    std::map<char, std::shared_ptr<const TrieNode>> children;
    if (node) {
      children = node->children_;
    }
    std::shared_ptr<const TrieNode> next_node;
    if (node and node->Contains(ch)) {
      next_node = node->children_.at(ch);
    } else {
      next_node = std::make_shared<const TrieNode>();
    }
    children[ch] = self(self, kv.substr(1), next_node);
    if (node) {
      if (node->is_value_node_) {
        auto clone_node = node->Clone();
        clone_node->children_ = std::move(children);
        return clone_node;
      }
    }
    return std::make_shared<const TrieNode>(children);
  };
  return Trie{dfs(dfs, key, root)};
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  auto dfs = [&](auto &&self, const std::shared_ptr<const TrieNode> &node, int ind) -> std::shared_ptr<const TrieNode> {
    if (ind == static_cast<int>(key.size())) {
      if (!node->is_value_node_) {
        return node;
      }
      if (node->children_.empty()) {
        return nullptr;
      }
      return std::make_shared<const TrieNode>(node->children_);
    }
    const char &ch = key[ind];
    if (!node->Contains(ch)) {
      return node;
    }
    auto next_node = self(self, node->children_.at(ch), ind + 1);
    auto clone_node = node->Clone();
    if (next_node == nullptr) {
      clone_node->children_.erase(clone_node->children_.find(ch));
    } else {
      clone_node->children_[ch] = next_node;
    }
    if (clone_node->children_.empty() and !node->is_value_node_) {
      return nullptr;
    }
    return clone_node;
  };
  return Trie{dfs(dfs, root_, 0)};
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
