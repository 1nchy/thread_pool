#include "segment_computing.hpp"

size_t sum(size_t _l, size_t _r) {
    size_t _n = 0;
    for (; _l != _r; ++_l) {
        _n += _l;
    }
    return _n;
}