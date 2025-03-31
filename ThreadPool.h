#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

// https://github.com/slmcbane/ThreadPool/blob/master/ThreadPool.hpp

#include <condition_variable>
#include <future>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool final {
 public:
  explicit ThreadPool(size_t maximum_threads = 0);
  ~ThreadPool();

 public:
  template<class F, class... Args>
  auto enqueue(F&& f, Args&& ... args) {
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
      std::scoped_lock<std::mutex> lk(this->mx_);

      if (this->stop_) {
        throw std::runtime_error("enqueue on stopped Thread-Pool");
      }

      tasks_.emplace([task]() { (*task)(); });
    }
    this->cv_.notify_one();
    return res;
  }

 private:
  void thread_func();
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;

  std::mutex mx_;
  std::condition_variable cv_;

  bool stop_{};
};

#endif // _THREADPOOL_H_
