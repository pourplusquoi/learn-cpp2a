#include <iostream>
#include <vector>

using namespace std;

long long num_payment(const vector<int>& currency, int target) {
    vector<long long> dp(target + 1, 0);
    dp[0] = 1;
    for (int val : currency)
        for (int i = val, rest = 0; i <= target; i++, rest++)
            dp[i] += dp[rest];
    return dp[target];
}

int main() {
    cout << num_payment({1, 5, 10, 20, 50, 100}, 10000) << endl;
    return 0;
}