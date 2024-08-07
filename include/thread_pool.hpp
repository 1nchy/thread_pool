#ifndef _ICY_THREAD_POOL_HPP_
#define _ICY_THREAD_POOL_HPP_

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <vector>
#include <any>

namespace icy {
class thread_pool;


class thread_pool {
public:
    explicit thread_pool(
        size_t _thread_n,
        size_t _task_max_n = std::numeric_limits<size_t>::max()
    );
    ~thread_pool();

    template <typename _T, typename... _Args> auto enqueue(_T, _Args&&...) -> std::future<typename std::result_of<_T(_Args...)>::type>;
private:
    void _M_close();
private:
    std::vector<std::thread> _m_worker_threads;
    std::queue<std::function<void()>> _m_task_queue;
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


template <typename _T, typename... _Args> auto
thread_pool::enqueue(_T _t, _Args&&... _args) -> std::future<typename std::result_of<_T(_Args...)>::type> {
    using result_type = typename std::result_of<_T(_Args...)>::type;
    auto _task = std::make_shared<std::packaged_task<result_type()>>(
        std::bind(std::forward<_T>(_t), std::forward<_Args>(_args)...)
    );
    std::future<result_type> _future = _task->get_future();
    {
    std::unique_lock<std::mutex> _lock(_m_mutex);
    if (_m_task_queue.size() >= _m_task_max_size) {
        _m_task_queue_full.wait(_lock, [this] {
            return _m_task_queue.size() < _m_task_max_size;
        });
    }
    _m_task_queue.emplace([_task]() {
        (*_task)();
    });
    }
    _m_task_queue_changed.notify_one();
    return _future;
}

}

#endif // _ICY_THREAD_POOL_HPP_