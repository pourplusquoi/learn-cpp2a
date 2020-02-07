class FizzBuzz {
private:
    int n, m;
    std::mutex mut;
    std::condition_variable cv;

public:
    FizzBuzz(int n) : m(1), n(n) {}

    // printFizz() outputs "fizz".
    void fizz(function<void()> printFizz) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            if (m > n) break;
            cv.wait(lock, [this] {
                return m > n || (m % 3 == 0 && m % 5 != 0);
            });
            if (m <= n) {
                printFizz();
                m++;
            }
            cv.notify_all();
        }
    }

    // printBuzz() outputs "buzz".
    void buzz(function<void()> printBuzz) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            if (m > n) break;
            cv.wait(lock, [this] {
                return m > n || (m % 5 == 0 && m % 3 != 0);
            });
            if (m <= n) {
                printBuzz();
                m++;
            }
            cv.notify_all();
        }
    }

    // printFizzBuzz() outputs "fizzbuzz".
    void fizzbuzz(function<void()> printFizzBuzz) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            if (m > n) break;
            cv.wait(lock, [this] {
                return m > n || m % 15 == 0;
            });
            if (m <= n) {
                printFizzBuzz();
                m++;
            }
            cv.notify_all();
        }
    }

    // printNumber(x) outputs "x", where x is an integer.
    void number(function<void(int)> printNumber) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            if (m > n) break;
            cv.wait(lock, [this] {
                return m > n || (m % 3 != 0 && m % 5 != 0);
            });
            if (m <= n) {
                printNumber(m++);
            }
            cv.notify_all();
        }
    }
};
