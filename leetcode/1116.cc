#include <condition_variable>
#include <mutex>
#include <thread>

using namespace std;

// Use yield of this thread.
class ZeroEvenOdd {
private:
    int n;
    int next;
    int state;

public:
    ZeroEvenOdd(int n) {
        this->n = n;
        this->next = 1;
        this->state = 0;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        while (true) {
            while (next <= n && state % 2)
                std::this_thread::yield();
            if (next > n) return;
            printNumber(0);
            state++;
        }
    }

    void even(function<void(int)> printNumber) {
        while (true) {
            while (next <= n && state != 3)
                std::this_thread::yield();
            if (next > n) return;
            printNumber(next++);
            state = 0;
        }
    }

    void odd(function<void(int)> printNumber) {
        while (true) {
            while (next <= n && state != 1)
                std::this_thread::yield();
            if (next > n) return;
            printNumber(next++);
            state = 2;
        }
    }
};

// Use condition variable.
class ZeroEvenOdd {
private:
    int n;
    int next;
    
    int state;
    
    std::mutex mut;
    std::condition_variable cv;

public:
    ZeroEvenOdd(int n) {
        this->n = n;
        this->next = 1;
        this->state = 0;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            cv.wait(lock, [this]() -> bool {
                return this->next > n || this->state % 2 == 0;
            });
            if (next > n) {
                cv.notify_one();
                return;
            } else {
                printNumber(0);
                state++;
                cv.notify_all();
            }
        }
    }

    void even(function<void(int)> printNumber) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            cv.wait(lock, [this]() -> bool {
                return this->next > n || this->state == 3;
            });
            if (next > n) {
                cv.notify_one();
                return;
            } else {
                printNumber(next++);
                state = 0;
                cv.notify_all();
            }
        }
    }

    void odd(function<void(int)> printNumber) {
        while (true) {
            std::unique_lock<std::mutex> lock(mut);
            cv.wait(lock, [this]() -> bool {
                return this->next > n || this->state == 1;
            });
            if (next > n) {
                cv.notify_one();
                return;
            } else {
                printNumber(next++);
                state = 2;
                cv.notify_all();
            }
        }
    }
};