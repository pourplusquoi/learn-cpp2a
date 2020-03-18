class Solution {
public:
  int numTimesAllBlue(vector<int>& light) {
    int res = 0, cur_max = 0;
    for (int i = 0; i < light.size(); i++) {
      cur_max = std::max(cur_max, light[i]);
      if (cur_max == i + 1)
        res++;
    }
    return res;
  }
};
