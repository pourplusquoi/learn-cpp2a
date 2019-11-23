#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

namespace prime {
namespace {

using Memoization =
    std::unordered_map<
        int64_t, std::unordered_map<int64_t, int64_t>>;

static std::map<int64_t, int64_t> prevp;
static Memoization dp_sum, dp_num;

inline int64_t quick_sqrt(const int64_t n) noexcept {
  return static_cast<int64_t>(std::sqrt(n));
}

constexpr int64_t quick_pow(int64_t base, int64_t power,
                            const int64_t mod) noexcept {
  int64_t res = 1;
  while (power > 0) {
    if (power & 1) {
      res = (res * base) % mod;
    }
    base = (base * base) % mod;
    power >>= 1;
  }
  return res;
}

constexpr bool is_prime(const int64_t n) noexcept {
  int power = 0;
  int64_t odd = n - 1;
  // Refactor s.t. $n = 2^power * odd$ where $odd = 1 mod 2$.
  while (odd % 2 == 0) {
    odd >>= 1;
    power++;
  }
  const float lnn = std::log(static_cast<float>(n));
  const int upper =
      std::min(n - 2, static_cast<int64_t>(std::floor(2 * lnn * lnn)));
  for (int i = 2; i <= upper; i++) {
    int64_t val = quick_pow(i, odd, n);
    if (val == 1 || val == n - 1) {
      continue;
    }
    for (int j = 0; j < power - 1; j++) {
      val = (val * val) % n;
      if (val == n - 1) {
        break;
      }
    }
    if (val == n - 1) {
      continue;
    }
    return false;
  }
  return true;
}

// Get the prime number $p$ s.t. $p <= n$ and there is no prime $p'$
// satisfying $p < p' <= n$.
int64_t prev_prime(int64_t n) noexcept {
  if (n > 2 && n % 2 == 0) {
    n--;
  }

  const auto iter = prevp.lower_bound(n);
  if (iter != prevp.cend() && iter->second <= n) {
    return iter->second;
  }

  int64_t m = n;
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
int64_t prime_sum(const int64_t n, int64_t p) noexcept {
  if (n < p * p) {
    p = quick_sqrt(n);
  }
  if (p > 1) {
    p = prev_prime(p);
  }

  auto& entry = dp_sum[n];
  const auto iter = entry.find(p);
  if (iter != entry.cend()) {
    return iter->second;
  }

  if (p <= 1) {
    const int64_t val = (2 + n) * (n - 1) / 2;
    entry[p] = val;
    return val;
  } else {
    const int64_t val =
        prime_sum(n, p - 1) -
            p * (prime_sum(n / p, p - 1) - prime_sum(p - 1, p - 1));
    entry[p] = val;
    return val;
  }
}

// Calculates the # of primes in the list $[2..n]$ after filtering out
// composite numbers using all the primes ${p' | p' <= p}$.
int64_t prime_num(const int64_t n, int64_t p) noexcept {
  if (n < p * p) {
    p = quick_sqrt(n);
  }
  if (p > 1) {
    p = prev_prime(p);
  }

  auto& entry = dp_num[n];
  const auto iter = entry.find(p);
  if (iter != entry.cend()) {
    return iter->second;
  }

  if (p <= 1) {
    entry[p] = n - 1;
    return n - 1;
  } else {
    const int64_t val =
        prime_num(n, p - 1) -
            (prime_num(n / p, p - 1) - prime_num(p - 1, p - 1));
    entry[p] = val;
    return val;
  }
}

}  // namespace

int64_t prime_sum_upto(const int64_t n) noexcept {
  return prime_sum(n, quick_sqrt(n));
}

int64_t prime_num_upto(const int64_t n) noexcept {
  return prime_num(n, quick_sqrt(n));
}

}  // namespace prime

int main() {
    std::cout << prime::prime_sum_upto(1'000'000'000ll) << std::endl;
    std::cout << prime::prime_num_upto(1'000'000'000ll) << std::endl;
    return 0;
}
