#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <memory>


// ITEM 31: Avoid default capture modes.

/*
 *  Things to remember
 *
 *  # Default by-reference capture can lead to dangling references.
 *
 *  # Default by-value capture is susceptible to dangling pointers (especially this),
 *    and it misleadingly suggests that lambdas are self contained.
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


int main() {
    using namespace item31;


    filters.emplace_back(
            [](int value) { return value % 5 == 0; }
    );


    addDivisorFilter();

    std::cout << std::boolalpha << filters[1](31321) << "\n";

    auto vec = std::vector<int>({120, 160});

    workWithContainer(vec);


    return 0;
}
