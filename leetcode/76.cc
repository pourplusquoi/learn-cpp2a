class Solution {
public:
  string minWindow(string s, string t) {
    int poscnt = 0;
    vector<int> count(256, 0);
    for (char c : t) {
      if (count[c]++ == 0)
        poscnt++;
    }
    int start = 0, length = INT_MAX;
    int slow = 0, fast = 0;
    while (fast < s.size()) {
      while (fast < s.size() && poscnt > 0) {
        if (--count[s[fast++]] == 0)
          poscnt--;
      }
      // Not found.
      if (poscnt > 0)
        break;
      while (slow < fast && poscnt == 0) {
        if (count[s[slow++]]++ == 0)
          poscnt++;
      }
      int newlen = fast - slow + 1;
      if (newlen < length) {
        start = slow - 1;
        length = newlen;
      }
    }
    return length < INT_MAX ? s.substr(start, length) : "";
  }
};
