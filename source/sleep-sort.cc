#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    std::vector<int> array {5,4,3,7,6,8,5,1,2,3,6,7,4,8,5,1,5,6,12,
        9,3,7,4,8,62,3,1,4,8,1,2,3,6,5,7,8,46,9,3,2,9,45,3,2,9,8,3,
        7,6,22,2,3,6,7,4,8,5,1,2,33,57,5,2,8,17,76,54,35,63,45,0,0};

    std::promise<void> ready_promise;
    std::vector<std::promise<void>> thread_promise(array.size());

    std::shared_future<void> ready_future(ready_promise.get_future());

    std::vector<std::function<void()>> thread_func;
    for (int i = 0; i < array.size(); i++) {
        auto func = [&, i, ready_future]() -> void {
            thread_promise[i].set_value();
            std::string out = std::to_string(array[i]) + " ";
            ready_future.wait();
            std::this_thread::sleep_for(std::chrono::milliseconds(10*array[i]));
            std::cout << out;
        };
        thread_func.push_back(func);
    }

    std::vector<std::future<void>> thread_future;
    for (auto& tp : thread_promise)
        thread_future.push_back(tp.get_future());

    std::vector<std::future<void>> thread_result;
    for (int i = 0; i < array.size(); i++)
        thread_result.push_back(std::async(std::launch::async, thread_func[i]));

    for (auto& tf : thread_future)
        tf.wait();

    ready_promise.set_value();

    return 0;
}
