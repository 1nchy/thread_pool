#include "thread_pool.hpp"

namespace icy {
thread_pool::thread_pool(size_t _thread_n, size_t _task_max_n)
: _m_task_max_size(_task_max_n) {
    for (size_t _i = 0; _i < _thread_n; ++_i) {
        _M_create_worker_thread();
    }
}
thread_pool::~thread_pool() {
    _M_close();
}
void thread_pool::_M_create_worker_thread() {
    _m_worker_threads.emplace_front(this);
    _m_worker_threads.front()._iter = _m_worker_threads.cbegin();
}
void thread_pool::_M_close() {
    {
    std::unique_lock<std::mutex> _lock(_m_mutex);
    if (_m_closed) {
        return;
    }
    _m_closed = true;
    }
    _m_task_queue_changed.notify_all();
    for (auto& _worker : _m_worker_threads) {
        if (_worker._thread.joinable()) {
            _worker._thread.join();
        }
    }
}
void thread_pool::worker_thread_main() {
    while (true) {
        std::function<void()> _task;
        {
        std::unique_lock<std::mutex> _lock(_m_mutex);
        _m_task_queue_changed.wait(_lock, [this] {
            return _m_closed || !_m_task_queue.empty();
        });
        if (_m_closed && _m_task_queue.empty()) {
            return;
        }
        _task = std::move(_m_task_queue.front());
        _m_task_queue.pop();
        }
        _m_task_queue_full.notify_one();
        _task();
    }
}
}
