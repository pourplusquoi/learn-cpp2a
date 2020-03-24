class Solution {
public:
  string longestPrefix(const string& s) {
    // KMP...
    int n = s.size();
    vector<int> next(n + 1, 0);
    int j = 0;
    for (int i = 1; i < n; i++) {
      while (j > 0 && s[i] != s[j])
        j = next[j];
      if (s[i] == s[j])
        j++;
      next[i + 1] = j;
    }
    return s.substr(0, next[n]);
  }
};
