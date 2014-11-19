#ifndef MTL_THREADING_TASKQUEUE_H
#define MTL_THREADING_TASKQUEUE_H

#include <future>
#include <deque>
#include <functional>

namespace mtl {
namespace threading {

// A concurrent task queue.
// `T` is the type of the task's return type.
template <typename T>
  class TaskQueue
  {
    public:
      typedef std::packaged_task<T()> TTask;

      TaskQueue() : stop_(false), no_more_task_(false) {}

      // Put a task into queue and return a future
      std::future<T> put(std::function<T()> task)
      {
        std::unique_lock<std::mutex> lock(access_);
        tasks_.push_back(TTask(task));
        auto ret = tasks_.back().get_future();
        lock.unlock();
        cond_.notify_one();
        return ret;
      }

      // Add a task into queue, no future returned.
      void add(std::function<T()> task)
      {
        {
          std::unique_lock<std::mutex> lock(access_);
          tasks_.push_back(TTask(task));
        }
        cond_.notify_one();
      }

      // Get a task from queue
      TTask get()
      {
        std::unique_lock<std::mutex> lock(access_);
        while (!stop_ && !no_more_task_ && tasks_.empty()) {
          cond_.wait(lock);
        }
        TTask task;
        if (!stop_ && !tasks_.empty()) {
          task = std::move(tasks_.front());
          tasks_.pop_front();
        }
        return task;
      }

      // Stop the queue regardless whether the queue is empty or not
      void stop()
      {
        {
          std::unique_lock<std::mutex> lock(access_);
          stop_ = true;
        }
        cond_.notify_all();
      }

      // Indicates that no more task will be added to the queue.
      // Invocation of get() will not keep the thread waiting if 
      // the queue is empty.
      void no_more_task() 
      {
        {
          std::unique_lock<std::mutex> lock(access_);
          no_more_task_ = true;
        }
        cond_.notify_all();
      }


    private:
      bool stop_;
      bool no_more_task_;
      mutable std::mutex      access_;
      std::deque<TTask>       tasks_;
      std::condition_variable cond_;
  };

} // namespace threading
} // namespace mtl

#endif
