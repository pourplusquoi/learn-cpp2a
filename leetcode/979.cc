class Solution1 {
public:
    int distributeCoins(TreeNode* root) {
        int res = 0;
        dfs(root, res);
        return res;
    }    
    int dfs(TreeNode* node, int& res) {
        if (node == nullptr)
            return 0;
        int lhs = dfs(node->left, res);
        int rhs = dfs(node->right, res);
        res += abs(lhs) + abs(rhs);
        return lhs + rhs + node->val - 1;
    }
};

// Using lambda.
class Solution2 {
public:
    int distributeCoins(TreeNode* root) {
        int res = 0;
        dfs(root, [&res](auto node, int lhs, int rhs) {
            res += abs(lhs) + abs(rhs);
            return lhs + rhs + node->val - 1;
        });
        return res;
    }

    int dfs(auto node, const auto& fn) {
        return node ? fn(node, dfs(node->left, fn), dfs(node->right, fn)) : 0;
    }
};


// Wrapping lambda.
class Solution3 {
public:
    int distributeCoins(TreeNode* root) {
        int res = 0;
        [](auto node, const auto& fn) {
            auto dfs = [](auto node, const auto& fn, const auto& ref) -> int {
                return node
                    ? fn(node,
                         ref(node->left, fn, ref),
                         ref(node->right, fn, ref))
                    : 0;
            };
            return dfs(node, fn, dfs);
        }(root, [&](auto node, int lhs, int rhs) {
            res += abs(lhs) + abs(rhs);
            return lhs + rhs + node->val - 1;
        });
        return res;
    }
};