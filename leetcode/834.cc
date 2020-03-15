class Solution1 {
    using Message = pair<int, int>;
    
    void collect(vector<unordered_map<int, Message>>& graph, vector<Message>& sum, int cur, int par) {
        for (auto &elem : graph[cur]) {
            int nxt = elem.first;
            if (nxt == par) continue;
            collect(graph, sum, nxt, cur);
            int dE = sum[nxt].first + 1;
            int dW = sum[nxt].second + dE;
            sum[cur].first += dE;
            sum[cur].second += dW;
            elem.second = {dE, dW};
        }
    }
    
    void distribute(vector<unordered_map<int, Message>>& graph, vector<Message>& sum, int cur, int par) {
        for (auto &elem : graph[cur]) {
            int nxt = elem.first;
            if (nxt == par) continue;
            auto& msg = elem.second;
            int dE = sum[cur].first - msg.first + 1;
            int dW = sum[cur].second - msg.second + dE;
            sum[nxt].first += dE;
            sum[nxt].second += dW;
            distribute(graph, sum, nxt, cur);
        }
    }
    
public:
    vector<int> sumOfDistancesInTree(int n, const vector<vector<int>>& edges) {
        vector<Message> sum(n, {0, 0});
        vector<unordered_map<int, Message>> graph(n);
        for (const auto& e : edges) {
            graph[e[0]][e[1]] = {0, 0};
            graph[e[1]][e[0]] = {0, 0};
        }
        
        collect(graph, sum, 0, -1);
        distribute(graph, sum, 0, -1);
        
        vector<int> res;
        res.reserve(n);
        for (int i = 0; i < n; i++)
            res.push_back(sum[i].second);
        return res;
    }
};

class Solution2 {
public:
    using Message = pair<int, int>;
    
    vector<int> sumOfDistancesInTree(int n, const vector<vector<int>>& edges) {
        vector<Message> sum(n);
        vector<unordered_map<int, Message>> graph(n);
        for (const auto& e : edges) {
            graph[e[0]][e[1]] = {0, 0};
            graph[e[1]][e[0]] = {0, 0};
        }
        
        [&](int cur, int par) {
            const auto collect = [&](int cur, int par, const auto& ref) -> void {
                for (auto &elem : graph[cur]) {
                    int nxt = elem.first;
                    if (nxt == par) continue;
                    ref(nxt, cur, ref);
                    int dE = sum[nxt].first + 1;
                    int dW = sum[nxt].second + dE;
                    sum[cur].first += dE;
                    sum[cur].second += dW;
                    elem.second = {dE, dW};
                }
            };
            collect(cur, par, collect);
        }(0, -1);
        
        [&](int cur, int par) {
            const auto distribute = [&](int cur, int par, const auto& ref) -> void {
                for (auto &elem : graph[cur]) {
                    int nxt = elem.first;
                    if (nxt == par) continue;
                    auto& msg = elem.second;
                    int dE = sum[cur].first - msg.first + 1;
                    int dW = sum[cur].second - msg.second + dE;
                    sum[nxt].first += dE;
                    sum[nxt].second += dW;
                    ref(nxt, cur, ref);
                }
            };
            distribute(cur, par, distribute);
        }(0, -1);
        
        vector<int> res;
        res.reserve(n);
        for (int i = 0; i < n; i++)
            res.push_back(sum[i].second);
        return res;
    }
};