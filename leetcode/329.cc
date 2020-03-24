class Solution_DFS_Memoization {
public:
  int longestIncreasingPath(const vector<vector<int>>& matrix) {
    int m = matrix.size(), n = matrix.empty() ? 0 : matrix[0].size();
    vector<vector<int>> dp(m, vector<int>(n, -1));
    int longest = 0;
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        longest = std::max(longest, 1 + dfs(matrix, i, j, dp));
      }
    }
    return longest;
  }
  
  int dfs(const vector<vector<int>>& matrix, int r, int c,
          vector<vector<int>>& dp) {
    if (dp[r][c] >= 0) {
      return dp[r][c];
    }
    int m = matrix.size(), n = matrix[0].size();
    int longest = 0;
    vector<int> dirs = { 0, 0, -1, 1, -1, 1, 0, 0 };
    for (int i = 0; i < 4; i++) {
      int nr = r + dirs[i], nc = c + dirs[i + 4];
      if (nr < 0 || nc < 0 || nr >= m || nc >= n)
        continue;
      if (matrix[nr][nc] > matrix[r][c]) {
        longest = std::max(longest, 1 + dfs(matrix, nr, nc, dp));
      }
    }
    dp[r][c] = longest;
    return longest;
  }
};

class Solution_Topological_Sort {
public:
  int longestIncreasingPath(vector<vector<int>>& matrix) {
    int m = matrix.size(), n = matrix.empty() ? 0 : matrix[0].size();
    vector<vector<int>> degree(m, vector<int>(n, 0));
    vector<int> dirs = { 0, 0, -1, 1, -1, 1, 0, 0 };
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        for (int k = 0; k < 4; k++) {
          int ni = i + dirs[k], nj = j + dirs[k + 4];
          if (ni < 0 || nj < 0 || ni >= m || nj >= n)
            continue;
          if (matrix[i][j] > matrix[ni][nj])
            degree[i][j]++;
        }
      }
    }
    queue<pair<int, int>> head;
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (degree[i][j] == 0)
          head.emplace(i, j);
      }
    }
    int longest = 0;
    while (!head.empty()) {
      longest++;
      for (int _ = head.size(); _ > 0; _--) {
        auto [i, j] = head.front();
        head.pop();
        for (int k = 0; k < 4; k++) {
          int ni = i + dirs[k], nj = j + dirs[k + 4];
          if (ni < 0 || nj < 0 || ni >= m || nj >= n)
            continue;
          if (matrix[i][j] < matrix[ni][nj]) {
            if (--degree[ni][nj] == 0)
              head.emplace(ni, nj);
          }
        }
      }
    }
    return longest;
  }
};
