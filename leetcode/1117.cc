#include <condition_variable>
#include <mutex>

using namespace std;

class H2O {
    int hy, ox;
    std::mutex mut;
    std::condition_variable cv;
    
public:
    H2O() {
        hy = 0, ox = 0;
    }

    void hydrogen(function<void()> releaseHydrogen) {
        std::unique_lock<std::mutex> lock(mut);
        cv.wait(lock, [this]() -> bool {
            return hy < 2 || ox == 1;
        });
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        releaseHydrogen();
        hy++;
        if (hy >= 2 && ox == 1)
            hy -= 2, ox--;
        cv.notify_all();
    }

    void oxygen(function<void()> releaseOxygen) {
        std::unique_lock<std::mutex> lock(mut);
        cv.wait(lock, [this]() -> bool {
            return hy == 2 || ox < 1;
        });
        // releaseOxygen() outputs "O". Do not change or remove this line.
        releaseOxygen();
        ox++;
        if (hy == 2 && ox >= 1)
            hy -= 2, ox--;
        cv.notify_all();
    }
};