#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t maximum_threads) {
  if (maximum_threads == 0) {
    maximum_threads = std::thread::hardware_concurrency();
//    printf("use max thread : %zu", maximum_threads);
  }

  threads_.reserve(maximum_threads);
  for (size_t i = 0; i < maximum_threads; ++i) {
      threads_.emplace_back(&ThreadPool::thread_func, this);
  }
}

ThreadPool::~ThreadPool() {
  {
    std::scoped_lock<std::mutex> lk(this->mx_);
    this->stop_ = true;
  }
  this->cv_.notify_all();
  for (std::thread &th : threads_) {
    th.join();
  }
}

void ThreadPool::thread_func() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lk(this->mx_);

      this->cv_.wait(lk, [this] { return this->stop_ || !this->tasks_.empty(); });
      if (this->tasks_.empty()) {
        break;
      }

      task = std::move(this->tasks_.front());
      this->tasks_.pop();
    }
    try {
      task();
    }
    catch (...) {
//      printf("Exception occured in thread\n");
    }
  }
}
