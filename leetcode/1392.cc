class Solution_DP {
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

class Solution_RollingHash {
public:
  static constexpr int mod = 1'000'000'007;
  
  string longestPrefix(const string& s) {
    int n = s.size();
    int longest = 0;
    uint64_t pref = 0, suff = 0;
    for (int lo = 0, hi = n - 1; hi > 0; lo++, hi--) {
      pref = ((pref << 8) + s[lo]) % mod;
      suff = (suff + s[hi] * pow(256, lo)) % mod;
      if (pref == suff)
        longest = lo + 1;
    }
    return s.substr(0, longest);
  }
  
  uint64_t pow(uint64_t num, int ord) {
    uint64_t base = num;
    uint64_t res = 1;
    while (ord > 0) {
      if (ord & 1) {
        res = (res * base) % mod;
      }
      ord >>= 1;
      base = (base * base) % mod;
    }
    return res;
  }
};
