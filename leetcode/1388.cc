class Solution {
public:
  int maxSizeSlices(const vector<int>& slices) {
    int n = slices.size();
    int m = n / 3;
    return std::max(helper(slices, 0, m, n - 1), helper(slices, 1, m, n - 1));
  }
  
  int helper(const vector<int>& slices, int offset, int m, int n) {
    // dp[i][j]: max # with i takes within first j slices.
    // dp[i][j] = max(dp[i - 1][j - 2] + slices[i], dp[i][j - 1])
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++) {
      for (int j = 1; j <= n; j++) {
        int take = slices[offset + j - 1] + (j < 2 ? 0 : dp[i - 1][j - 2]);
        int leave = dp[i][j - 1];
        dp[i][j] = std::max(take, leave);
      }
    }
    return dp[m][n];
  }
};
