/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

struct Result {
  bool is_bst;
  int total;
  int max_sum;
  int min_val;
  int max_val;
};

class Solution {
public:
  int maxSumBST(TreeNode* root) {
    return traverse(root).max_sum;
  }
  
  Result traverse(TreeNode* node) {
    if (node == nullptr)
      return Result {
          .is_bst = true,
          .total = 0,
          .max_sum = 0,
          .min_val = INT_MAX,
          .max_val = INT_MIN,
      };
    auto lhs = traverse(node->left);
    auto rhs = traverse(node->right);
    if (is_valid(node, lhs, rhs)) {
      int total = node->val + lhs.total + rhs.total;
      return Result {
          .is_bst = true,
          .total = total,
          .max_sum = std::max(total, std::max(lhs.max_sum, rhs.max_sum)),
          .min_val = std::min(node->val, lhs.min_val),
          .max_val = std::max(node->val, rhs.max_val),
      };
    } else {
      return Result {
          .is_bst = false,
          .total = node->val,
          .max_sum = std::max(lhs.max_sum, rhs.max_sum),
          .min_val = node->val,
          .max_val = node->val,
      };
    }
  }
  
  inline constexpr bool is_valid(TreeNode* node,
                              const Result& lhs,
                              const Result& rhs) {
    return lhs.is_bst && rhs.is_bst
        && (node->left == nullptr || lhs.max_val < node->val)
        && (node->right == nullptr || rhs.min_val > node->val);
  }
};
