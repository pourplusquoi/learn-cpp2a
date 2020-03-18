struct Engineer {
  int spd;
  int eff;
};

struct Greater {
  bool operator()(const Engineer& lhs, const Engineer& rhs) {
    return lhs.spd > rhs.spd;
  }
};

class Solution {
public:
  static constexpr int64_t modulo = 1'000'000'007;
    
  int maxPerformance(int n, vector<int>& spd, vector<int>& eff, int k) {
    vector<Engineer> engs;
    engs.reserve(n);
    for (int i = 0; i < n; i++) {
      engs.emplace_back(Engineer{spd[i], eff[i]});
    }
    
    std::sort(engs.begin(), engs.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.eff > rhs.eff;
    });
    
    // Use minimum heap.
    priority_queue<Engineer, vector<Engineer>, Greater> pq;
    
    int64_t max_perf = 0, sum_spd = 0;
    for (int i = 0; i < n; i++) {
      pq.push(engs[i]);
      sum_spd += engs[i].spd;
      if (pq.size() > k) {
        sum_spd -= pq.top().spd;
        pq.pop();
      }
      auto cur_perf = static_cast<int64_t>(engs[i].eff) * static_cast<int64_t>(sum_spd);
      max_perf = max(max_perf, cur_perf);
    }
    
    return max_perf % modulo;
  }
};
