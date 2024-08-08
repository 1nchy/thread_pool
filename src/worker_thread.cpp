#include "thread_pool.hpp"

namespace icy {

thread_pool::worker_thread::worker_thread(thread_pool* const _pool)
: _pool(_pool), _thread(&thread_pool::worker_thread_main, _pool) {

}

}