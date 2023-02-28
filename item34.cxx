#include <iostream>
#include <algorithm>
#include <atomic>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>


// ITEM 34: Prefer lambda to std::bind.

/*
 *  Things to remember
 *
 *  # Lambdas are more readable, more expressive, and many be more efficient than using std::bind.
 *
 *
 *  # In C++11 only, std::bind may be useful for implementing move capture or for binding
 *    objects with templatized function call operators.
 *
 * */

namespace item31 {
    using FilterContainer = std::vector<std::function<bool(int)>>;
    FilterContainer filters;

    double computeSomeValue() {
       std::random_device rd;
       auto mt = std::mt19937(rd());
       return std::uniform_real_distribution<double>(1.0, 200.0)(mt);
    }

    std::size_t computeDivisor(double x, double y) {
        return x / y;
    }


    void addDivisorFilter() {
        auto calc1 = computeSomeValue();
        auto calc2 = computeSomeValue();

        auto divisor = computeDivisor(calc1, calc2);

        filters.emplace_back(
                [&](int value){ return value % divisor == 0; }
        ); // danger! ref to divisor will dangle!

        filters.emplace_back(
                [=](int value) { return value % divisor == 0; }
        ); // now divisor can't dangle (copy by value)
    }

    template<typename C>
    void workWithContainer(const C& container) {
        auto calc1 = computeSomeValue();
        auto calc2 = computeSomeValue();

        auto divisor = computeDivisor(calc1, calc2);

        divisor = (divisor == 0) ? 1 : divisor;
        using ContElemT = typename C::value_type;  // type of elements in container

        using std::begin;
        using std::end;

        if (std::all_of(
              begin(container), end(container),
              [&](const ContElemT& value) { return value % divisor == 0; })) {
            std::cout << "all statisfied" << "\n";
        }
    }

    class Widget {
    public:
        void addFilter() const;  // add an entry to filters
    private:
        int divisor;    // used in Widget's filter
    };

    // Captures apply only to non-static local variables (including parameters) visible
    // the scope where the lambda is created
    // what's being captured is the Widget's this pointer, not divisor.
    void Widget::addFilter() const {
//        filters.emplace_back(
//                [=](int value) { return value % divisor == 0; }
//        );

       // In C++14, a better way to capture a data member is to use generalized lambda capture
       filters.emplace_back(
               [divisor = divisor](int value) { return value % divisor == 0; }
       );

    }

    void doSomeWork() {
        auto pw = std::make_unique<Widget>();

        pw->addFilter(); // add filter that uses Widget::divisor
    }   // destory Widget; filters now holds dangling pointers!

}


namespace item32 {
    // Using an init capture makes it possible for you to specify
    //  1. the name of data member in the closure class generated from the lambda and
    //  2. an expression initlializing that data member.


    // use init capture to move a std::unique_ptr into a closure
    class Widget {
    public:
        bool isValidated() const;
        bool isProcessed() const;
        bool isArchived() const;
    };

    bool Widget::isValidated() const { return true; }
    bool Widget::isProcessed() const { return true; }
    bool Widget::isArchived() const { return true; }


    class IsValAndArch {
    public:
        using DataType = std::unique_ptr<Widget>;

        explicit IsValAndArch(DataType&& ptr) : pw(std::move(ptr)) {}

        bool operator()() const
        { return pw->isValidated() && pw->isArchived(); }

    private:
        DataType pw;
    };
}

namespace item33 {
    template<typename T>
    T&& forward(std::remove_reference_t<T>& param) {
        return static_cast<T&&>(param);
    }

    class Widget{};
}

namespace item34 {

    using Time = std::chrono::steady_clock::time_point;

    enum class Sound { Beep, Siren, Whistle };

    using Duration = std::chrono::steady_clock::duration;

    // at time t, make sound s for duration d
    void setAlarm(Time, Sound, Duration ) {};

    auto setSoundL = [](Sound s) {
        using namespace std::chrono;

        setAlarm(steady_clock::now() + hours(1),  // alarm to go off
                s,                                // in an hour for
                seconds(30));                     // 30 seconds
    };


    auto setSoundL_14 = [](Sound s) {
        using namespace std::chrono;
        using namespace std::literals;

        setAlarm(steady_clock::now() + 1h,  // C++14, but
                 s,                         // same meaning
                 30s);                      // as above
    };

    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::placeholders;

    //auto setSoundB =
    //    std::bind(setAlarm,
    //              steady_clock::now() + 1h,
    //              _1,
    //              30s);

    //auto setSoundB =
    //    std::bind(setAlarm,
    //              std::bind(std::plus<>(), steady_clock::now(), 1h),
    //              _1,
    //              30s);

    // setAlarm is overloaded
    enum class Volume { Normal, Loud, LoudPlusPLus };

    void setAlarm(Time t, Sound s, Duration d, Volume v);

    using SetAlarm3ParamType = void(*)(Time t, Sound s, Duration d);

    auto setSoundB = std::bind(static_cast<SetAlarm3ParamType>(setAlarm),
                               std::bind(std::plus<>(),
                                         steady_clock::now(),
                                         1h),
                               _1,
                               30s);


    // function to create compressed copies of Widgets.
    enum class CompLevel { Low, Normal, High }; // compression level

    class Widget {};

    // make compressed copy of w
    Widget compress(const Widget&, CompLevel) {
        return Widget();
    };

    // Polymorphic function objects
    class PolyWidget {
    public:
        template<typename T>
        void operator()(const T&) {};
    };

}


int main() {
    using namespace item34;

//    auto f = [](auto&&... params) {
//                return func(normaize(std::forward<decltype<params>(params)...));
//    }

    auto lowVal = 1;
    auto highVal = 20;


    // return whether its argument is between a lowVal and highVal
    auto betweenL =
        [lowVal, highVal]
        (const auto& val)
        { return lowVal <= val && val <= highVal; };

    using namespace std::placeholders;

    auto betweenB =
        std::bind(std::logical_and<>(), // C++14
            std::bind(std::less_equal<>(), lowVal, _1),
            std::bind(std::less_equal<>(), _1, highVal));

    std::cout << std::boolalpha;

    std::cout << betweenL(10) << "\n";

    highVal = 2;

    std::cout << betweenB(10) << "\n";

    //Widget w;

    //using namespace std::placeholders;

    //// w is stored by value
    //auto compressRateB = std::bind(compress, w, _1);


    PolyWidget pw;

    auto boundPW = std::bind(pw, _1);

    // pass int
    boundPW(1930);

    // pass nullptr
    boundPW(nullptr);

    // pass string literal
    boundPW("Rosebud");

    // there is no way to do this with C++11 lambda.
    // In C++ 14, however, it's easily achieved via
    // a lambda with an auto parameter

    // capture by value not compiled. Why?
    auto boundPW_14 = [&pw](const auto& param) {
        pw(param);
    };

    boundPW_14(1930);

    boundPW_14(nullptr);


    return 0;
}
