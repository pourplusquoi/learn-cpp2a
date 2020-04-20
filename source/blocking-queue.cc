#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

template <typename T>
class Queue {
public:
  Queue(uint64_t size)
    : size_(size), begin_(0), end_(0), data_(std::vector<T>(size + 1)) {}

  void Push(const T& val) {
    std::unique_lock<std::mutex> lock(mut_);

    cv_.wait(lock, [this] {
      return !IsFull();
    });

    data_[end_] = val;
    end_ = (end_ + 1) % data_.size();

    lock.unlock();
    cv_.notify_one();
  }

  T Pop() {
    std::unique_lock<std::mutex> lock(mut_);

    cv_.wait(lock, [this] {
      return !IsEmpty();
    });

    T ret = data_[begin_];
    begin_ = (begin_ + 1) % data_.size();

    lock.unlock();
    cv_.notify_one();

    return ret;
  }

private:
  bool IsEmpty() const {
    return begin_ == end_;
  }

  bool IsFull() const {
    return (begin_ == 0 && end_ == size_) || (begin_ > end_ && begin_ - end_ == 1);
  }

  uint64_t size_;
  uint64_t begin_;
  uint64_t end_;
  std::vector<T> data_;

  std::mutex mut_;
  std::condition_variable cv_;
};

template <typename T>
class Producer {
public:
  Producer(Queue<T>* queue) : queue_(queue) {
    assert(queue != nullptr);
  }

  void Produce(const T& val) {
    queue_->Push(val);
  }

private:
  Queue<T>* const queue_;
};

template <typename T>
class Consumer {
public:
  Consumer(Queue<T>* queue) : queue_(queue) {
    assert(queue != nullptr);
  }

  T Consume() {
    return queue_->Pop();
  }

private:
  Queue<T>* const queue_;
};

int main () {
  auto queue = std::make_unique<Queue<int>>(4);

  std::srand(std::time(0));
  std::mutex mut;

  std::vector<std::future<void>> futures;

  for (int i = 0; i < 10; i++) {
    auto fut = std::async([i, &mut, q = queue.get()]() {
      Producer<int> producer {q};
      while (true) {
        int value = std::rand();
        producer.Produce(value);
        std::lock_guard<std::mutex> lock(mut);
        std::cout << "[x] Producer #" << i << " produced value: " << value << std::endl;
      }
    });
    futures.push_back(std::move(fut));
  }

  for (int i = 0; i < 10; i++) {
    auto fut = std::async([i, &mut, q = queue.get()]() {
      Consumer<int> consumer {q};
      // while (true) {
        int value = consumer.Consume();
        std::lock_guard<std::mutex> lock(mut);
        std::cout << "[o] Consumer #" << i << " consumed value: " << value << std::endl;
      // }
    });
    futures.push_back(std::move(fut));
  }

  for (const auto& fut : futures) {
    fut.wait();
  }
}
