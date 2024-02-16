#ifndef _ASP_THREAD_POOL_HPP_
#define _ASP_THREAD_POOL_HPP_

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <vector>

namespace asp {
class thread_pool;

class thread_pool {
public:
    // typedef typename thread_task_t::handler_t handler_t;
    typedef std::function<void()> thread_task_t;
    explicit thread_pool(
        size_t _thread_n,
        size_t _task_max_n = std::numeric_limits<size_t>::max()
    );
    ~thread_pool();

    std::future<std::thread::id> add_task(thread_task_t _p);
    void close();
private:
    std::vector<std::thread> _m_worker_threads;
    std::queue<thread_task_t> _m_task_queue;
    std::queue<std::promise<std::thread::id>> _m_promise_queue;
    size_t _m_task_max_size;
    std::mutex _m_mutex;
    // close flag, avoid redundant closes.
    bool _m_closed = false;
    // used to limit the size of %_m_task_queue.
    std::condition_variable _m_task_queue_full;
    // used to nofity %_m_worker_threads.
    std::condition_variable _m_task_queue_changed;

    void worker_thread_main();
};
}

#endif // _ASP_THREAD_POOL_HPP_