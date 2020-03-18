class Solution {
public:
  int numOfMinutes(int n, int head,
                   const vector<int>& manager,
                   const vector<int>& time) {
    vector<vector<int>> graph(n);
    for (int i = 0; i < n; i++) {
      if (manager[i] == -1)
        continue;
      graph[manager[i]].push_back(i);
    }
    return dfs(graph, time, head);
  }
  
  int dfs(const vector<vector<int>>& graph, const vector<int>& time, int cur) {
    int beneath_time = 0;
    for (int nxt : graph[cur])
      beneath_time = max(beneath_time, dfs(graph, time, nxt));
    return time[cur] + beneath_time;
  }
};
