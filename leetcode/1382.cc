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

class SolutionEx {
public:
    TreeNode* balanceBST(TreeNode* root) {
        vector<TreeNode*> preorder;
        [&](TreeNode* node) {
            const auto traverse = [&](TreeNode* node, const auto& ref) {
                if (node == nullptr)
                    return;
                ref(node->left, ref);
                preorder.push_back(node);
                ref(node->right, ref);
            };
            traverse(node, traverse);
        }(root);
        return [&](int lo, int hi) {
            const auto rebuild = [&](int lo, int hi, const auto& ref) -> TreeNode* {
                if (lo > hi)
                    return nullptr;
                int mid = lo + (hi - lo) / 2;
                TreeNode* node = preorder[mid];
                node->left = ref(lo, mid - 1, ref);
                node->right = ref(mid + 1, hi, ref);
                return node;
            };
            return rebuild(lo, hi, rebuild);
        }(0, preorder.size() - 1);
    }
};
