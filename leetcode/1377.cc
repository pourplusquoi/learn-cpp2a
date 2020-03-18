class Solution {
public:
  double frogPosition(int n, vector<vector<int>>& edges, int t, int target) {
    vector<unordered_set<int>> graph(n + 1);
    for (const auto& e : edges) {
      graph[e[0]].emplace(e[1]);
      graph[e[1]].emplace(e[0]);
    }
    double res = 0.0;
    unordered_set<int> visited;
    visited.insert(1);
    dfs(graph, visited, 1, target, t, 1.0, res);
    return res;
  }
  
  void dfs(const vector<unordered_set<int>>& graph,
           unordered_set<int>& visited,
           int cur, int tar, int t, double p, double& res) {
    if (t == 0) {
      res += (cur == tar) ? p : 0.0;
      return;
    }
    
    int count = 0;
    for (int nxt : graph[cur])
      if (visited.find(nxt) == visited.end())
        count++;
    
    if (count == 0) {
      res += (cur == tar) ? p : 0.0;
      return;
    }
    
    for (int nxt : graph[cur]) {
      if (visited.find(nxt) != visited.end())
        continue;
      visited.insert(nxt);
      dfs(graph, visited, nxt, tar, t - 1, p * 1.0 / count, res);
      visited.erase(nxt);
    }
  }
};
