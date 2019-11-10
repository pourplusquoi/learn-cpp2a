#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

namespace prime {
namespace {

using int64 = std::int64_t;

static std::map<int64, int64> prevp;
static std::unordered_map<int64,
    std::unordered_map<int64, int64>> dp_sum, dp_num;

inline int64 quick_sqrt(const int64 n) noexcept {
    return static_cast<int64>(std::sqrt(n));
}

constexpr int64 quick_pow(int64 base, int64 power,
                          const int64 mod) noexcept {
    int64 res = 1;
    while (power > 0) {
        if (power & 1)
            res = (res * base) % mod;
        base = (base * base) % mod;
        power >>= 1;
    }
    return res;
}

constexpr bool is_prime(const int64 n) noexcept {
    int power = 0;
    int64 odd = n - 1;
    // Refactor s.t. $n = 2^power * odd$ where $odd = 1 mod 2$.
    while (odd % 2 == 0) {
        odd >>= 1;
        power++;
    }
    const float lnn = std::log(static_cast<float>(n));
    const int upper = std::min(n - 2,
        static_cast<int64>(std::floor(2 * lnn * lnn)));
    for (int i = 2; i <= upper; i++) {
        int64 val = quick_pow(i, odd, n);
        if (val == 1 || val == n - 1)
            continue;
        for (int j = 0; j < power - 1; j++) {
            val = (val * val) % n;
            if (val == n - 1)
                break;
        }
        if (val == n - 1)
            continue;
        return false;
    }
    return true;
}

// Get the prime number $p$ s.t. $p <= n$ and there is no prime $p'$
// satisfying $p < p' <= n$.
int64 prev_prime(int64 n) noexcept {
    if (n > 2 && n % 2 == 0)
        n--;

    const auto iter = prevp.lower_bound(n);
    if (iter != prevp.cend() && iter->second <= n)
        return iter->second;

    int64 m = n;
    while (!is_prime(m)) {
        m -= 2;
        const auto it = prevp.find(m);
        if (it != prevp.cend()) {
            prevp.emplace(n, it->second);
            prevp.erase(it);
        }
    }
    prevp.emplace(n, m);
    return m;
}

// Calculates the sum of the list $[2..n]$ after filtering out composite
// numbers using all the primes ${p' | p' <= p}$.
int64 prime_sum(const int64 n, int64 p) noexcept {
    if (n < p * p)
        p = quick_sqrt(n);
    if (p > 1)
        p = prev_prime(p);

    auto& entry = dp_sum[n];
    const auto iter = entry.find(p);
    if (iter != entry.cend())
        return iter->second;

    if (p <= 1) {
        const int64 val = (2 + n) * (n - 1) / 2;
        entry[p] = val;
        return val;
    } else {
        const int64 val = prime_sum(n, p - 1) -
            p * (prime_sum(n / p, p - 1) - prime_sum(p - 1, p - 1));
        entry[p] = val;
        return val;
    }
}

// Calculates the # of primes in the list $[2..n]$ after filtering out
// composite numbers using all the primes ${p' | p' <= p}$.
int64 prime_num(const int64 n, int64 p) noexcept {
    if (n < p * p)
        p = quick_sqrt(n);
    if (p > 1)
        p = prev_prime(p);

    auto& entry = dp_num[n];
    const auto iter = entry.find(p);
    if (iter != entry.cend())
        return iter->second;

    if (p <= 1) {
        entry[p] = n - 1;
        return n - 1;
    } else {
        const int64 val = prime_num(n, p - 1) -
            (prime_num(n / p, p - 1) - prime_num(p - 1, p - 1));
        entry[p] = val;
        return val;
    }
}

}  // namespace

int64 prime_sum_upto(const int64 n) noexcept {
    return prime_sum(n, quick_sqrt(n));
}

int64 prime_num_upto(const int64 n) noexcept {
    return prime_num(n, quick_sqrt(n));
}

}  // namespace prime

int main() {
    std::cout << prime::prime_sum_upto(1'000'000'000ll) << std::endl;
    std::cout << prime::prime_num_upto(1'000'000'000ll) << std::endl;
    return 0;
}
