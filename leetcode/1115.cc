#include <condition_variable>
#include <mutex>
#include <thread>

// Use yield of this thread.
class FooBar {
private:
    int n;
    bool flip;

public:
    FooBar(int n) : n(n), flip(true) {}

    void foo(function<void()> printFoo) {
        for (int i = 0; i < n; i++) {
            while (!flip)
                std::this_thread::yield();
            // printFoo() outputs "foo". Do not change or remove this line.
            printFoo();
            flip = !flip;
        }
    }

    void bar(function<void()> printBar) {
        for (int i = 0; i < n; i++) {
            while (flip)
                std::this_thread::yield();
            // printBar() outputs "bar". Do not change or remove this line.
            printBar();
            flip = !flip;
        }
    }
};

// Use condition variable.
class FooBar {
private:
    int n;
    std::mutex mut;
    std::condition_variable cv;
    bool flip;

public:
    FooBar(int n) : n(n), flip(true) {}

    void foo(function<void()> printFoo) {
        for (int i = 0; i < n; i++) {
            std::unique_lock<std::mutex> lock(mut);
            cv.wait(lock, [this]() -> bool {
                return flip;
            });
            // printFoo() outputs "foo". Do not change or remove this line.
            printFoo();
            flip = !flip;
            cv.notify_one();
        }
    }

    void bar(function<void()> printBar) {
        for (int i = 0; i < n; i++) {
            std::unique_lock<std::mutex> lock(mut);
            // printBar() outputs "bar". Do not change or remove this line.
            cv.wait(lock, [this]() -> bool {
                return !flip;
            });
            printBar();
            flip = !flip;
            cv.notify_one();
        }
    }
};
