#include <iostream>
#include <memory>   // include std::unique_ptr
#include <vector>
#include <mutex>
#include <unordered_map>
#include <list>



// ITEM 9: Prefer alias declaration to typedefs.

/*
 *  Things to remember
 *
 *  # typedefs don't support templatization, but alias declarations do.
 *
 *  # Alias templates avoid the "::type" suffix and, in templates, the "typename" prefix
 *    required to refer to typedefs.
 *
 *  # C++14 offers alias templates for all the C++ 11 type traits transformations
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


namespace item9 {
    using UPtrMapSS = std::unique_ptr<std::unordered_map<std::string, std::string>>;

    // function pointers
    // FP is a synonym for a pointer to a function taking an int and a const
    // std::string& and returning nothing
    typedef void(*FP)(int, const std::string&); // typedef
    using FB = void (*)(int, const std::string&); // alias declaration


    // alias templates
    template<typename T>
    using MyAlloc = std::allocator<T>;  // Note: use std::allocator for simplification

    template<typename T>
    using MyAllocList = std::list<T, MyAlloc<T>>;

    // With a typedef
    template<typename T>
    struct MyAllocList_ {
        typedef std::list<T, MyAlloc<T>> type;
    };

    template<typename T>
    class Widget{
    private:
        typename MyAllocList_<T>::type list; // Widget<T> contains a MyAllocList<T> as
                                             // a data member

        MyAllocList<T> list_;       // no "typename", no "::type"
    };

    class Wine {};

    template<>
    class MyAllocList_<Wine> {
    private:
        enum class WineType { White, Red, Rose };

        WineType type; // in this class, type is a data member!
    };

    // mimic remove_const_t in C+11 using alias template
    template<class T>
    using remove_const_t = typename std::remove_const<T>::type;
}


int main() {
    using namespace item9;

    MyAllocList<Widget<int>> lw; // client code
    MyAllocList_<Widget<int>>::type lw_; // client code



    return 0;
}
