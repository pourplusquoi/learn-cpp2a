class Solution {
public:
    TreeNode* balanceBST(TreeNode* root) {
        vector<TreeNode*> preorder;
        traverse(root, preorder);
        return rebuild(preorder, 0, preorder.size() - 1);
    }
    
    TreeNode* rebuild(const vector<TreeNode*>& preorder, int lo, int hi) {
        if (lo > hi)
            return nullptr;
        int mid = lo + (hi - lo) / 2;
        TreeNode* node = preorder[mid];
        node->left = rebuild(preorder, lo, mid - 1);
        node->right = rebuild(preorder, mid + 1, hi);
        return node;
    }
    
    void traverse(TreeNode* node, vector<TreeNode*>& preorder) {
        if (node == nullptr)
            return;
        traverse(node->left, preorder);
        preorder.push_back(node);
        traverse(node->right, preorder);
    }
};
