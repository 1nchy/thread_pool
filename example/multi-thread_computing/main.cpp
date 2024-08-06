#include "thread_pool.hpp"

#include "segment_computing.hpp"

#include <vector>

#include <cstdio>

int main(void) {
    constexpr size_t _n = 100;
    constexpr size_t _gap = 10;
    std::vector<std::future<size_t>> _vf;
    icy::thread_pool _pool(4);
    for (size_t _i = 1; _i < _n; _i += _gap) {
        _vf.push_back(_pool.enqueue(sum, _i, _i + _gap));
    }
    size_t _sum = 0;
    for (auto& _f : _vf) {
        _sum += _f.get();
    }
    printf("sum from 1 to %ld equals %ld\n", _n, _sum);
    return 0;
}