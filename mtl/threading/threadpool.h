#ifndef MTL_THREADING_THREADPOOL_H
#define MTL_THREADING_THREADPOOL_H

#include <future>
#include <thread>
#include <functional>
#include <vector>

#include "taskqueue.h"

namespace mtl {
namespace threading {

template <typename T>
  class ThreadPool {

    public:
      typedef TaskQueue<T> TQueue;

      ~ThreadPool() 
      {
        stop();
      }

      // Start threads
      void start(unsigned num) 
      {
        assert (num > 0);
        while(num-- >0) {
          addWorker();
        }
      }

      // Run a task and return a future.
      // `task` must be convertable to std::function<T()>.
      std::future<T> runTask(std::function<T()> task) 
      {
        return queue_.put(task);
      }

      // Run a task, no future returned.
      void addTask(std::function<T()> task) 
      {
        queue_.add(task);
      }

      // Notify threads to exit and wait for them.
      void stop() 
      {
        if (pool_.empty()) return;

        queue_.stop();
        for(std::thread &t : pool_) {
          t.join();
        }
        pool_.clear();
      }

      // Wait for all tasks to finish and then stop the threads.
      void wait()
      {
        queue_.no_more_task();
        for(std::thread &t : pool_) {
          t.join();
        }
        pool_.clear();
      }

    private:
      void addWorker() {
        std::thread t([this]() {
            while (true) {
            typename TQueue::TTask task(queue_.get());
            if (!task.valid()) break;
            task();
            }
            });
        pool_.push_back(std::move(t));
      }

    private:
      TQueue queue_;
      std::vector<std::thread> pool_;
  };

} // namespace threading
} // namespace mtl

#endif

