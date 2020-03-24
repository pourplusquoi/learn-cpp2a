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
