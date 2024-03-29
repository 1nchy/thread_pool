#include "thread_pool.hpp"

namespace asp {
thread_pool::thread_pool(size_t _thread_n, size_t _task_max_n)
: _m_task_max_size(_task_max_n) {
    _m_worker_threads.reserve(_thread_n);
    for (size_t _i = 0; _i < _thread_n; ++_i) {
        _m_worker_threads.emplace_back(&thread_pool::worker_thread_main, this);
    }
}
thread_pool::~thread_pool() {
    close();
}
std::future<std::thread::id> thread_pool::add_task(thread_task_t _p) {
    std::future<std::thread::id> _future;
    {
    std::unique_lock<std::mutex> _lock(_m_mutex);
    if (_m_task_queue.size() >= _m_task_max_size) {
        _m_task_queue_full.wait(_lock, [this] {
            return _m_task_queue.size() < _m_task_max_size;
        });
    }
    _m_task_queue.emplace(_p);
    _m_promise_queue.emplace();
    _future = _m_promise_queue.back().get_future();
    }
    _m_task_queue_changed.notify_one();
    return _future;
}
void thread_pool::close() {
    {
    std::unique_lock<std::mutex> _lock(_m_mutex);
    if (_m_closed) {
        return;
    }
    _m_closed = true;
    }
    _m_task_queue_changed.notify_all();
    for (auto& _thread : _m_worker_threads) {
        if (_thread.joinable()) {
            _thread.join();
        }
    }
}
void thread_pool::worker_thread_main() {
    while (true) {
        thread_pool::thread_task_t _task;
        {
        std::unique_lock<std::mutex> _lock(_m_mutex);
        _m_task_queue_changed.wait(_lock, [this] {
            return _m_closed || !_m_task_queue.empty();
        });
        if (_m_closed && _m_task_queue.empty()) {
            return;
        }
        _task = std::move(_m_task_queue.front());
        _m_promise_queue.front().set_value(std::this_thread::get_id());
        _m_task_queue.pop();
        _m_promise_queue.pop();
        }
        _m_task_queue_full.notify_one();
        _task();
    }
}
}
