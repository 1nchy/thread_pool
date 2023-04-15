# thread_pool

## details

When a thread calls `condition_variable::wait()` and get blocked, the mutex is automatically released. When the condition variable is notified and the thread wakes up, it reacquires the mutex.

## example

~~~cpp
#include "thread_pool.hpp"
#include <cstdio>

int main(void) {
    asp::thread_pool thp(5);
    for (int i = 0; i < 40; ++i) {
        thp.add_task([=]() {
            printf("task [ %d ].\n", i);
        });
    }
    thp.close();
    return 0;
}
~~~
~~~shell
$ g++ main.cpp thread_pool.cpp -pthread
$ ./a.out
task [ 1 ].
task [ 2 ].
...
task [ 15 ].
task [ 244928235 ].
task [ 17 ].
...
task [ 31 ].
task [ 125989896 ].
task [ 33 ].
...
~~~