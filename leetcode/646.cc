#include <algorithm>
#include <vector>
#include <utility>

class Solution {
    auto search(const vector<pair<int, int>>& time, int tar) -> int {
        int lo = 0, hi = time.size() - 1;
        while (lo < hi) {
            int mid = hi - (hi - lo) / 2;
            if (time[mid].first < tar)
                lo = mid;
            else hi = mid - 1;
        }
        return (lo == hi && time[lo].first < tar) ? lo : -1;
    }
public:
    int findLongestChain(vector<vector<int>>& pairs) {
        std::sort(pairs.begin(), pairs.end(), [](const auto& lhs, const auto& rhs) -> bool {
            return lhs[1] < rhs[1];
        });
        vector<pair<int, int>> time;
        for (const auto& pair : pairs) {
            int idx = search(time, pair[0]);
            int count = idx < 0 ? 1 : time[idx].second + 1;
            if (!time.empty())
                count = std::max(count, time.back().second);
            time.emplace_back(pair[1], count);
        }
        return time.back().second;
    }
};