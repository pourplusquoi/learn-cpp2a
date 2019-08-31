#include <algorithm>
#include <array>
#include <charconv>
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
    
    Transaction(string_view str) {
        array<string_view, 4> parts;
        for (size_t i = 0, start = 0; i < 4; i++) {
            auto terminate = std::min(str.find(',', start), str.size());
            parts[i] = string_view(str.data() + start, terminate - start);
            start = terminate + 1;
        }    
        valid = true;
        name = parts[0];
        city = parts[3];
        std::from_chars(parts[1].data(), parts[1].data() + parts[1].size(), time);
        std::from_chars(parts[2].data(), parts[2].data() + parts[2].size(), amount);
    }
    
    string to_string() {
        return string(name) + "," + to_string(time) + "," +
               to_string(amount) + "," + string(city);
    }
    
    static string to_string(uint16_t num) {
        if (num == 0) return "0";
        string res;
        while (num > 0) {
            res.push_back((num % 10) + '0');
            num /= 10;
        }
        std::reverse(res.begin(), res.end());
        return res;
    }
};

class Solution {
    unordered_map<string_view, vector<Transaction>> hs;
    
    void conditional_push(vector<string>& res, Transaction& ts) {
        if (ts.valid) {
            ts.valid = false;
            res.emplace_back(ts.to_string());
        }
    }

public:
    vector<string> invalidTransactions(const vector<string>& transactions) {
        for (string_view str : transactions) {
            Transaction ts(str);
            hs[ts.name].emplace_back(std::move(ts));
        }

        vector<string> res;
        for (auto& kv : hs) {
            vector<Transaction>& value = kv.second;
            std::sort(value.begin(), value.end(),
                      [](const auto& lhs, const auto& rhs) {
                          return lhs.time < rhs.time;
                      });

            size_t i = 0, j = 1;
            while (true) {
                while (j < value.size() && value[j].city == value[i].city)
                    j++;
                if (j >= value.size()) break;

                while (i < j && value[j].time - value[i].time > 60)
                    i++;
                while (i < j)
                    conditional_push(res, value[i++]);

                size_t k;
                for (k = j; k < value.size() &&
                     value[k].time - value[j - 1].time <= 60 &&
                     value[k].city == value[j].city; k++);
                while (j < k)
                    conditional_push(res, value[j++]);
            }

            for (Transaction& ts : value)
                if (ts.amount > 1000)
                    conditional_push(res, ts);
        }
        return res;
    }
};