class Solution {
public:
  int palindromePartition(string s, int k) {
    int n = s.size();
    vector<vector<int>> require(n);
    for (int i = 0; i < n; i++) {
      require[i] = vector<int>(n - i + 1, 0);
      for (int j = i + 2; j <= n; j++) {
        int len = j - i;
        require[i][len] = calculate(string_view(&s[i], len));
      }
    }
    vector<int> dp = require[0];
    for (int i = 1; i < k; i++) {
      vector<int> cp(n + 1, INT_MAX);
      for (int j = i + 1; j <= n; j++) {
        for (int p = i; p < j; p++)
          cp[j] = std::min(cp[j], dp[p] + require[p][j - p]);
      }
      dp = std::move(cp);
    }
    return dp[n];
  }
  
  int calculate(string_view sv) {
    int n = sv.size();
    int count = 0;
    for (int i = 0, j = n - 1; i < j; i++, j--) {
      if (sv[i] != sv[j])
        count++;
    }
    return count;
  }
};
