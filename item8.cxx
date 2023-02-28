#include <iostream>
#include <memory>   // include std::unique_ptr
#include <functional>  // include std::function
#include <vector>
#include <mutex>



// ITEM 8: Prefer nullptr to 0 and NULL.

/*
 *  Things to remember
 *
 *  # Prefer nullptr to 0 and null
 *
 *  # Avoid overloading on integral and pointer types
 * */


namespace item7 {
    int x(0);  // initializer in parentheses

    int y = 0; // initializer follows "="

    int z{0};  // initializer is in braces

    // Ignore the equals-sign-plus-braces syntax, because C++ usually
    // treats it the same as braces-only version
    // int z = {0}; // initializer uses "=" and braces

    // C++ 11 introduces uniform initialization, based on braces.
    class Widget{
    public:
        Widget() {}
        Widget(int ) {}

        Widget(int, bool ) { }
        Widget(int, double ) { }

        Widget(std::initializer_list<long double> ) {  }
        //Widget(std::initializer_list<bool> ) { }
        //Widget(std::initializer_list<std::string> ) {}

        operator float() const { return 1.0; }; //  conver to float

        Widget(const Widget& ) {  }

    private:
        int x{0};   // fine, x's default value is 0;
        int y = 0;  // also fine
        //int z(0);   // error!
    };
}

namespace item8 {
    void f(int) {};    // three overloads of f
    void f(bool) {};
    void f(void*) { std::cout << "f(void*) called" << "\n"; };

    // nullptr's advantage is that it doesn't have an integral type.
    // To be honest, it doesn't have a pointer type, either, but you
    // can think of it as a pointer of all types.
    // The type of std::nullptr_t implictly converts to all raw pointer
    // types.

    class Widget {};

    int f1(std::shared_ptr<Widget> spw);        // call these only when
    double f2(std::unique_ptr<Widget> upw);     // the appropriate
    bool f3(Widget* pw);                        // mutex is locked

    std::mutex f1m, f2m, f3m;

    using MuxGuard = std::lock_guard<std::mutex>;

    template<typename FuncType,
             typename MuxType,
             typename PtrType>
    decltype(auto) lockAndCall(FuncType func,
                               MuxType& mutex,
                               PtrType ptr) {
        MuxGuard g(mutex);
        return func(ptr);
    }

}

int main() {
    using namespace item8;

    f(0);   // calls f(int), not f(void*)

    //f(NULL); // might not compile, but typically calls
             // f(int). Never calls f(void*)

    f(nullptr); // calls f(void*) overload;

    //auto result1 = lockAndCall(f1, f1m, 0); // error!

    //auto result2 = lockAndCall(f2, f2m, NULL); // error!

    auto result3 = lockAndCall(f3, f3m, nullptr); // fine

    std::cout << result3 << '\n';



    return 0;
}
