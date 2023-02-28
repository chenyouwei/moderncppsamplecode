#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <memory>
#include <mutex>


// ITEM 32: Use init capture to move objects into closures.

/*
 *  Things to remember
 *
 *  # Use C++14's init capture to move objects into closures
 *
 *  # In C++11, emulate init capture via hand-written classes or std::bind
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


int main() {
    using namespace item32;

    auto pw = std::make_unique<Widget>();

    // init capture
    auto func = [pw = std::move(pw)]
                { return pw->isValidated() && pw->isArchived(); };

    // in C++ 11
    auto func_isValAndArch = IsValAndArch(std::make_unique<Widget>());


    return 0;
}
