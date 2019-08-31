#include <algorithm>
#include <array>
#include <charconv>
#include <future>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

struct Transaction {
    string_view name;
    uint16_t time;
    uint16_t amount;
    string_view city;
    bool valid;
    
    Transaction(string_view str) : valid(true) {
        array<string_view, 4> arr;
        for (size_t i = 0, start = 0; i < 4; i++) {
            auto terminate = std::min(str.find(',', start), str.size());
            arr[i] = string_view(str.data() + start, terminate - start);
            start = terminate + 1;
        }
        name = arr[0];
        city = arr[3];
        std::from_chars(arr[1].data(), arr[1].data() + arr[1].size(), time);
        std::from_chars(arr[2].data(), arr[2].data() + arr[2].size(), amount);
    }
};

class Solution {
    unordered_map<string_view, vector<Transaction>> hs;
    
    // Parallel execution members.
    bool parallel;
    std::mutex mut;
    static constexpr size_t kParallelThreshold = 600;

    void conditional_push(vector<string>& res, Transaction& ts) {
        if (ts.valid) {
            if (parallel) {
                std::unique_lock<std::mutex> lock(mut);
                if (ts.valid) {
                    ts.valid = false;
                    res.emplace_back(ts.name.data());  // Note: This is very hacky.
                }
            } else {
                ts.valid = false;
                res.emplace_back(ts.name.data());
            }
        }
    }

public:
    vector<string> invalidTransactions(const vector<string>& transactions) {
        for (string_view str : transactions) {
            Transaction ts(str);
            hs[ts.name].emplace_back(std::move(ts));
        }
        parallel = hs.size() > kParallelThreshold;

        vector<string> res;
        vector<future<void>> futs;
        for (auto& kv : hs) {
            vector<Transaction>& value = kv.second;
            const auto exec = [&]() -> void {
                std::sort(value.begin(), value.end(),
                          [](const auto& lhs, const auto& rhs) -> bool {
                              return lhs.time < rhs.time;
                          });

                size_t n = value.size(), i = 0, j = 1;
                while (true) {
                    while (j < n && value[j].city == value[i].city)
                        j++;
                    if (j >= n) break;

                    while (i < j && value[j].time - value[i].time > 60)
                        i++;
                    while (i < j)
                        conditional_push(res, value[i++]);

                    size_t k;
                    for (k = j; k < n &&
                         value[k].time - value[j - 1].time <= 60 &&
                         value[k].city == value[j].city; k++);
                    while (j < k)
                        conditional_push(res, value[j++]);
                }

                for (Transaction& ts : value)
                    if (ts.amount > 1000)
                        conditional_push(res, ts);
            };
            if (!parallel) exec();
            else futs.push_back(std::async(std::launch::async, exec));
        }

        for (const auto& fut : futs)
            fut.wait();

        return res;
    }
};