class Solution {
public:
  int findKthNumber(int n, int k) {
    k--;
    int cur = 1;
    while (k > 0) {
      int step = count(n, cur);
      if (step <= k) {
        cur += 1;
        k -= step;
      } else {
        cur *= 10;
        k -= 1;
      }
    }
    return cur;
  }
  
  int count(long n, long cur) {
    long nxt = cur + 1;
    int step = 0;
    while (cur <= n) {
      step += std::min(n + 1, nxt) - cur;
      cur *= 10;
      nxt *= 10;
    }
    return step;
  }
};
