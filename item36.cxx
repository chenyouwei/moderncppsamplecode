#include <iostream>
#include <future>


// ITEM 36: Specify std::launch::async if asynchronicity is essential.

/*
 *  Things to remember
 *
 *  # The default launch policy for std::async permits both asynchronous and
 *    synchronous task execution.
 *
 *  # This flexibility leads to uncertainty when accessing thread_locals, implies
 *    that the task may never execture, and affects program logic for timeout-based
 *    wait calls.
 *
 *  # Specify std::launch::async if asynchronous task execution is essential.
 *
 * */

namespace item35 {
    int doAsyncwork(int value) { return value; };
}

namespace item36 {
   /*
    *  # The std::launch::async launch policy means that f must run asynchronously, i.e.,
    *    on a different thead.
    *
    *  # The std::launch::deferred launch policy means that f may run only when get or wait
    *    is called on the future returned by std::async. That is, f's execution is deferred
    *    until such a call is made. When get or wait is invoked, f will execute synchronously,
    *    i.e., the caller will block until f finishes running. If neither get nor wait is called,
    *    f will never run.
    *
    * */

    using namespace std::literals;
    void f() { std::this_thread::sleep_for(1s); };

    /*
     *  # It's not possible to predict whether f will run concurrently with t, because f might
     *    because f might be scheduled to run deferred.
     *
     *  # It's not possible to predict whether f runs on a thread different from the thead invoking
     *    get or wait on fut. If that thread is t, the implication is that it's not possible to predict
     *    whether f runs on a thread different from t.
     *
     *  # It may not be possible to predict whether f runs at all, because it may not be possible to
     *    guarantee that get or wait will be called on fut along every path through the program.
     *
     *  # thead-local storage(TLS), it's not possible to predict which thread's variables will be
     *    accessed
     * */

    // C++11
    //template<typename F, typename... Ts>
    //inline std::future<typename std::result_of<F(Ts...)>::type>reallyAsync(F&& f, Ts&&... params) {
    //    return std::async(std::launch::async,
    //                      std::forward<F>(f),
    //                      std::forward<Ts>(params)...);
    //}


    // C++14
    template<typename F, typename... Ts>
    inline auto reallyAsync(F&& f, Ts&&... params) {
        return std::async(std::launch::async,
                          std::forward<F>(f),
                          std::forward<Ts>(params)...);
    }

}

int main() {
    using namespace item36;
    // the following two calls have exactly the same meaning
    //auto fut1 = std::async(f); // runn f using default launch policy

    //auto fut2 = std::async(std::launch::async | std::launch::deferred, f); // run f either async or deferred

    // Given a thead t executing this statement
    auto fut = std::async(f); // run f using default launch policy


    // loop until f has finished running... which may never happen!
    // while (fut.wait_for(100ms) != std::future_status::ready) {}

    if (fut.wait_for(0s) == std::future_status::deferred) { // if task is deferred
        // use wait or get on fut to call f synchronously
    } else { // task isn't deferred
        while (fut.wait_for(100ms) != std::future_status::ready) {
            // task is neither deferred nor ready, so do
            // concurrent work until it's ready
        }
        // fut is ready
    }


    return 0;
}
