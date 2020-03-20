/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

struct Response {
  int overall_max;
  int pending_max;
};

class Solution {
public:
  int longestZigZag(TreeNode* root) {
    auto lhs = longest(root->left, true);
    auto rhs = longest(root->right, false);
    return std::max(std::max(lhs.overall_max, lhs.pending_max + 1),
                    std::max(rhs.overall_max, rhs.pending_max + 1)) - 1;
  }
  
  Response longest(TreeNode* node, bool turned_left) {
    if (node == nullptr)
      return Response { 0, 0 };
    auto lhs = longest(node->left, true);
    auto rhs = longest(node->right, false);
    int overall_max = std::max(std::max(lhs.overall_max, lhs.pending_max + 1),
                               std::max(rhs.overall_max, rhs.pending_max + 1));
    int pending_max = turned_left ? rhs.pending_max + 1 : lhs.pending_max + 1;
    return Response { overall_max, pending_max };
  }
};
