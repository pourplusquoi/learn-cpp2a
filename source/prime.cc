#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>

class Prime {
 public:
  static int64_t SumUpto(const int64_t n) noexcept;
  static int64_t NumUpto(const int64_t n) noexcept;

 private:
  // Calculates the sum of primes in the list $[2..n]$ after filtering out 
  // composite numbers using all the primes ${p' | p' <= p}$.
  static int64_t PrimeSum(const int64_t n, int64_t p) noexcept;

  // Calculates the # of primes in the list $[2..n]$ after filtering out
  // composite numbers using all the primes ${p' | p' <= p}$.
  static int64_t PrimeNum(const int64_t n, int64_t p) noexcept;

  // Gets the prime number $p$ s.t. $p <= n$ and there is no prime $p'$
  // satisfying $p < p' <= n$.
  static int64_t PreviousPrime(int64_t n) noexcept;

  using Memoization =
      std::unordered_map<
          int64_t, std::unordered_map<int64_t, int64_t>>;

  static inline std::map<int64_t, int64_t> previous_prime_;
  static inline Memoization dp_sum_;
  static inline Memoization dp_num_;
};

namespace {
inline int64_t Sqrt(const int64_t n) noexcept {
  return static_cast<int64_t>(std::sqrt(n));
}

constexpr int64_t QuickPower(int64_t base, int64_t power,
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

constexpr bool IsPrime(const int64_t n) noexcept {
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
    int64_t val = QuickPower(i, odd, n);
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
}  // namespace

int64_t Prime::SumUpto(const int64_t n) noexcept {
  return PrimeSum(n, Sqrt(n));
}

int64_t Prime::NumUpto(const int64_t n) noexcept {
  return PrimeNum(n, Sqrt(n));
}

int64_t Prime::PrimeSum(const int64_t n, int64_t p) noexcept {
  if (n < p * p) {
    p = Sqrt(n);
  }
  if (p > 1) {
    p = PreviousPrime(p);
  }

  auto& entry = dp_sum_[n];
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
        PrimeSum(n, p - 1) -
            p * (PrimeSum(n / p, p - 1) - PrimeSum(p - 1, p - 1));
    entry[p] = val;
    return val;
  }
}

int64_t Prime::PrimeNum(const int64_t n, int64_t p) noexcept {
  if (n < p * p) {
    p = Sqrt(n);
  }
  if (p > 1) {
    p = PreviousPrime(p);
  }

  auto& entry = dp_num_[n];
  const auto iter = entry.find(p);
  if (iter != entry.cend()) {
    return iter->second;
  }

  if (p <= 1) {
    entry[p] = n - 1;
    return n - 1;
  } else {
    const int64_t val =
        PrimeNum(n, p - 1) -
            (PrimeNum(n / p, p - 1) - PrimeNum(p - 1, p - 1));
    entry[p] = val;
    return val;
  }
}

int64_t Prime::PreviousPrime(int64_t n) noexcept {
  if (n > 2 && n % 2 == 0) {
    n--;
  }

  const auto iter = previous_prime_.lower_bound(n);
  if (iter != previous_prime_.cend() && iter->second <= n) {
    return iter->second;
  }

  int64_t m = n;
  while (!IsPrime(m)) {
    m -= 2;
    const auto it = previous_prime_.find(m);
    if (it != previous_prime_.cend()) {
      previous_prime_.emplace(n, it->second);
      previous_prime_.erase(it);
    }
  }
  previous_prime_.emplace(n, m);
  return m;
}

int main() {
  std::cout << Prime::SumUpto(1'000'000'000ll) << std::endl;
  std::cout << Prime::NumUpto(1'000'000'000ll) << std::endl;
  return 0;
}
