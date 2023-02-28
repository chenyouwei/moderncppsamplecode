#include <iostream>
#include <algorithm>
#include <future>
#include <mutex>


// ITEM 40: Use std::atomic for concurrency, volatile for special memory.

/*
 *  Things to remember
 *
 *  # std::atomic is for data accessed from multiple threads without using
 *    mutexes. It's a tool for writing concurrent software.
 *
 *  # volatile is for memory where reads and writes should not be optmized
 *    away. It's a tool for working with special memory.
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
        // auto nh = t.get().native_handle(); // use t's native handle to set t's priority

        // if(conditionsAreSatisfied()) {
        if (true) {
            t.get().join();   // let t finish
            // performComputation(goodVals);
            return true;
        }

        return false;
    }
}

namespace item38 {
    // this container might block in its dtor, because one or more contained
    // futures could refer to a shared state for a non-deferred task launched
    // via std::async
    std::vector<std::future<void>> futs;

    class Widget {
    private:
        std::shared_future<double> fut;
    };
}

namespace item39 {
    std::condition_variable cv;  // condvar for event
    std::mutex m; // mutex for use with cv

    int i = 0;


    // 1. If the detecting task notfies the condvar before the reacting waits,
    //    the reacting task will hang.
    // 2. The wait statement fails to account for spurious wakeups.

    void detectTask() {
        // the code in the detecting is as simple as simple can be:
        // ...      detect event
        std::unique_lock<std::mutex> lk(m);
        i = 1;
        cv.notify_one(); // tell reacting task
        std::cout << "detectTask finished." << "\n";
    }

    void reactTask() {
        // The code for the reacting task is a bit more complicated,
        // because before calling wait on the condvar, it must lock a
        // mutex through a std::unique_lock object. (Locking a mutex
        // before waiting on a condition is typical for threading libraries.
        // The need to lock the mutex through a std::unique_lock object is
        // simply part of the C++11 API.

        // ... Prepare to react
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{ std::cout << "...finished waiting" << "\n";  return i == 1;});
        std::cout << "reactTask finished." << "\n";
    }


    //std::atomic<bool> flag(false);  // detect event
    bool flag(false);

    void detectFlag() {
        {
            std::lock_guard<std::mutex> g(m);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            flag = true;
        }
        cv.notify_one();
    }

    void reactFlag() {
        //while(!flag) {
        //    std::this_thread::sleep_for(std::chrono::seconds(1));
        //}
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return flag; });
        std::cout << "flag has set to true" << "\n";
    }


    std::promise<void> p;   // promise for communications channel


    void detectVoid() {
        // ... // detect event
        p.set_value();  // tell reacting task
    }

    void reactVoid() {
        p.get_future().wait();
    }


    void reactSuspend() {};

    void detectSuspend() {
        //std::thread t([]    // create thread
        //        {
        //            p.get_future().wait();  // suspend t until future is set
        //            reactSuspend();
        //        });

        using item37::ThreadRAII;

        ThreadRAII tr(
                std::thread([]{
                        p.get_future().wait();
                        reactSuspend();
                    }),
                ThreadRAII::DtorAction::join);


        // ... // here t is suspened prior to call to react
        p.set_value();  // unsuspend t (and thus call react)

        // ... // do addtoinal work
        //t.join();   // make t unjoinable
    }


    void detectMuitiple() {
        auto sf = p.get_future().share(); // sf's type is std::shared_future<void>

        std::vector<std::thread> vt;    // container for reacting threads

        for (int i = 0; i < 3; ++i) {
            vt.emplace_back([sf] { sf.wait();
                                   reactVoid(); });
        }

        // ... // detect hangs if this "..." code throws!
        p.set_value();  // unsuspend all threads

        for (auto& t: vt) { // make all threads unjoinable
            t.join();
        }
    }
}


namespace item40 {

    // std::atomic offers operations that are guaranteed to be seen as atomic by other threads.
    // Once a std::atomic object has been construted, operations on it behave as if they were
    // inside a mutex-protected critical section, but the operations are generally implemented
    // using special machine instructions that are more efficient than would be the case if a
    // mutex were employed.


}


int main() {
    using namespace item40;

    // Consider this code using std::atomic
    std::atomic<int> ai(0); // initialize ai to 0

    ai = 10;    // atomically set ai to 10

    std::cout << ai;    // atomically read ai's value


    // read-modify-write(RWM) operations
    // all member functions on it, including RMW operations,
    // are guaranteed to be seen by other threads as atomic.

    ++ai;       // atomically increment ai to 11

    --ai;       // atmically decrement ai to 10

    // In contrast, volatile gurantees virtually nothing in a
    // multithreaded context:

    volatile int vi(0);     // initialize vi to 0

    vi = 10;    // set vi to 10

    std::cout << vi;    // read vi's value

    ++vi;   // increment vi to 11

    --vi;   // decrement vi to 10

    // During execution of this code, if other threads are reading the value
    // of vi, they may see anything, e.g., -12, 68, 4090727.


    // Note: the use of std::atmoic imposes restrictions on how code can be
    // reodered. Volatile doesn't impose the same restrictions.


    // "Normal" memory has the characteristic that if you write a value to memory
    // location, the value remains there until something overwrites it. So if I
    // have a normal int

    int x = 1;

    // and a compiler sees the following sequence of operations on it,
    auto y = x; // read x
    y = x;       // read x again

    // the compiler can optimize the generated code by eliminating the assignment to y,
    // because it's redundant with y's initialization.


    // Volatile is the way we tell compilers that we're dealing with special memory. Its
    // meaning to compilers is "Don't perform any optimizations on operations on this memory."
    // So if x corresponds to special memory, it'd be declared volatile.


    // useful if vai corresponding to a memory-mapped I/O location that was concurrently
    // accessed by multiple threads.
    // volatile std::atomic<int> vai;



    return 0;
}
