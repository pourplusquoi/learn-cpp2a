class SolutionBF {
public:
    unordered_map<TreeNode*, vector<int>> dp;
    
    int minCameraCover(TreeNode* root) {
        return min(dfs(root, false, false), dfs(root, true, false));
    }
    
    int dfs(TreeNode* node, bool cur, bool par) {
        if (node == nullptr)
            return 0;
        
        const auto& [iter, _] = dp.emplace(node, vector<int>{-1, -1, -1});
        int& res = iter->second[index(cur, par)];
        if (res >= 0)
            return res;
        
        int lt = dfs(node->left, true, cur);
        int rt = dfs(node->right, true, cur);
        int lm = min(dfs(node->left, false, cur), lt);
        int rm = min(dfs(node->right, false, cur), rt);
        
        if (!cur) {
            if (!node->left && !node->right) {
                res = par ? 0 : INT_MAX;
            } else if (!node->left) {
                res = par ? rm : rt;
            } else if (!node->right) {
                res = par ? lm : lt;
            } else {
                res = par ? lm + rm : min(lt + rm, rt + lm);
            }
        } else {
            res = 1 + lm + rm;
        }
        
        return res;
    }
    
    inline constexpr int index(int cur, int par) {
        return cur ? 2 : par;
    }
};

class SolutionDP {
public:
    int minCameraCover(TreeNode* root) {
        array<int, 3> res = solve(root);
        return min(res[1], res[2]);
    }
    
    array<int, 3> solve(TreeNode* node) {
        if (node == nullptr)
            return {0, 0, 9999};
        
        auto lhs = solve(node->left);
        auto rhs = solve(node->right);
        
        int lm = min(lhs[1], lhs[2]);
        int rm = min(rhs[1], rhs[2]);
        return {
            lhs[1] + rhs[1],
            min(lhs[2] + rm, lm + rhs[2]),
            1 + min(lhs[0], lm) + min(rhs[0], rm),
        };
    }
};

class SolutionGreedy {
public:
    int minCameraCover(TreeNode* root) {
        int res = 0;
        unordered_set<TreeNode*> covered;
        covered.insert(nullptr);
        dfs(root, nullptr, covered, res);
        return res;
    }
    
    void dfs(auto cur, auto par, auto& covered, int& res) {
        if (cur == nullptr)
            return;
        
        dfs(cur->left, cur, covered, res);
        dfs(cur->right, cur, covered, res);
        
        if ((par == nullptr && covered.find(cur) == covered.end())
           || covered.find(cur->left) == covered.end()
           || covered.find(cur->right) == covered.end()) {
            res++;
            covered.insert(par);
            covered.insert(cur);
            covered.insert(cur->left);
            covered.insert(cur->right);
        }
    }
};