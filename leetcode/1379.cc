class Solution {
public:
  TreeNode* getTargetCopy(TreeNode* original, TreeNode* cloned, TreeNode* target) {
    if (original == nullptr && cloned == nullptr)
      return nullptr;
    if (original == target)
      return cloned;
    auto lhs = getTargetCopy(original->left, cloned->left, target);
    if (lhs != nullptr)
      return lhs;
    auto rhs = getTargetCopy(original->right, cloned->right, target);
    if (rhs != nullptr)
      return rhs;
    return nullptr;
  }
};
