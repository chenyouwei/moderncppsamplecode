#include <iostream>
#include <future>
#include <thread>


// ITEM 35: Prefer task-based programming to thread-based.

/*
 *  Things to remember
 *
 *  # The std::thread API offers no direct way to get return values from asychronosly run functions.
 *    , and if those functions throw, the program is terminated.
 *
 *  # Thread-based programming calls for manual management of thread exhaution, oversubscription,
 *    load balancing, and adaption to new platforms.
 *
 *  # Task-based programming via std::async with the default lanuch policy handles most of these
 *    issues for you.
 * */

namespace item35 {
    int doAsyncwork(int value) { return value; };
}

int main() {
    using namespace item35;

    std::thread t(doAsyncwork);

    // task-based
    auto fut = std::async(doAsyncwork, 1);


    return 0;
}
