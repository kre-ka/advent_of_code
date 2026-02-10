#include <memory>
#include <vector>

template <typename T>
struct TreeNode : public std::enable_shared_from_this<TreeNode<T>> {
  TreeNode(T data) : data(data) {};

  void addChild(std::shared_ptr<TreeNode<T>> child) {
    child->parent = this->weak_from_this();
    // connect child with sibling, if there is any
    if (this->children.size() > 0) {
      child->older_sibling = this->children[children.size() - 1];
      this->children[children.size() - 1]->younger_sibling = child;
    }
    this->children.push_back(child);
  }

  std::shared_ptr<TreeNode<T>> getOldestLeaf() {
    auto current_node = this->shared_from_this();
    while (!current_node->children.empty()) {
      current_node = current_node->children[0];
    }
    return current_node;
  }
  std::shared_ptr<TreeNode<T>> getNextLeaf() {
    auto current_node = this->shared_from_this();
    while (current_node) {
      auto temp_node = current_node->younger_sibling.lock();
      if (temp_node) {
        return temp_node->getOldestLeaf();
      }
      current_node = current_node->parent.lock();
    }
    return current_node;
  }

  std::vector<std::vector<T>> flatten() {
    std::vector<std::vector<T>> out;
    auto current_node = getOldestLeaf();

    // traverse all (younger) leaves
    while (current_node) {
      auto temp_node = current_node;
      // make vector of all ancestor data
      std::vector<T> current_vec;
      do {
        current_vec.push_back(temp_node->data);
        temp_node = temp_node->parent.lock();
      } while (temp_node);
      out.push_back(current_vec);

      current_node = current_node->getNextLeaf();
    }
    return out;
  }

  T data;
  std::vector<std::shared_ptr<TreeNode<T>>> children;
  std::weak_ptr<TreeNode<T>> parent, older_sibling, younger_sibling;
};
