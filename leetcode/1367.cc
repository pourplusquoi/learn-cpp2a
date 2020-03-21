/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

class Solution {
public:
  bool isSubPath(ListNode* head, TreeNode* root) {
    if (head == nullptr)
      return true;
    if (root == nullptr)
      return false;
    return dfs(head, root)
        || isSubPath(head, root->left) || isSubPath(head, root->right);
  }
  
  bool dfs(ListNode* head, TreeNode* root) {
    if (head == nullptr)
      return true;
    if (root == nullptr)
      return false;
    return head->val == root->val
        && (dfs(head->next, root->left) || dfs(head->next, root->right));
  }
};

class SolutionTLE {
public:
  bool isSubPath(ListNode* head, TreeNode* root) {
    vector<int> nums;
    while (head != nullptr) {
      nums.push_back(head->val);
      head = head->next;
    }
    return recurse(nums, 0, root, root);
  }
  
  bool recurse(const vector<int>& nums, int idx,
               TreeNode* node, TreeNode* fallback) {
    if (idx == nums.size())
      return true;
    if (node == nullptr || node->val != nums[idx]) {
      return (fallback->left && recurse(nums, 0, fallback->left, fallback->left))
          || (fallback->right && recurse(nums, 0, fallback->right, fallback->right));
    }
    return recurse(nums, idx + 1, node->left, fallback)
        || recurse(nums, idx + 1, node->right, fallback);
  }
};
