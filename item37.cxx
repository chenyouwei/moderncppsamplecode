#include <iostream>
#include <future>
#include <vector>
#include <memory>


// ITEM 37: Make std::threads unjoinable on all paths.

/*
 *  Things to remember
 *
 *  # Make std::threads unjoinable on all paths.
 *
 *  # join-on-destruction can lead to difficult-to-debug performance anomalies.
 *
 *  # detach-on-destruction can lead to difficult-to-debug undefined behavior.
 *
 *  # Declare std::thread objects last in lists of data members.
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

namespace item37 {
    constexpr auto tenMillion = 10'000'000;

    class ThreadRAII {
    public:
        enum class DtorAction { join, detach };

        ThreadRAII(std::thread&& t, DtorAction a)
            : action(a), t(std::move(t)) {}  // in dtor, take action a on t

        ~ThreadRAII() {
            if (t.joinable()) {

                if (action == DtorAction::join) {
                    t.join();
                } else {
                    t.detach();
                }
            }
        }

        ThreadRAII(ThreadRAII&& ) = default;
        ThreadRAII& operator=(ThreadRAII&& ) = default;

        std::thread& get() { return t; }

    private:
        DtorAction action;
        std::thread t;
    };


    // returns whether computation was performed
    bool doWork(std::function<bool(int)> filter, int maxVal = tenMillion) {
        std::vector<int> goodVals;      // values that satisfy filter

        ThreadRAII t(std::thread([&filter, maxVal, &goodVals] {
                    for (auto i = 0; i <= maxVal; ++i) {
                            if (filter(i)) goodVals.push_back(i);
                        }
                    }), ThreadRAII::DtorAction::join);


        // use t's native handle to set t's priority
        //auto nh = t.get().native_handle(); // use t's native handle to set t's priority

        // if(conditionsAreSatisfied()) {
        if (true) {
            t.get().join();   // let t finish
            // performComputation(goodVals);
            return true;
        }

        return false;
    }


}

int main() {
    return 0;
}
