#include <iostream>
#include <memory>   // include std::unique_ptr
#include <vector>
#include <mutex>
#include <unordered_map>
#include <list>


// ITEM 12: Declare overriding functions override.

/*
 *  Things to remember
 *
 *  # Declare overriding functions override.
 *
 *  # Member function reference qualifies make it possible to treat lvalue
 *    and rvalue objects (*this) differently.
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

namespace item10 {
    // As a general rule, declaring a name inside curly braces limits
    // the visibility of that name to the scope defined by the braces.
    // Not so for the enumerators belong to the scope containing the
    // enum, and that means that nothing else in that scope may have
    // the name
    // enum Color { black, white, red };   // black, white, red are in
                                        // same scope as Color
    //auto white = false; // error! white already declared in this scope

    // The fact that these enumerator names leak into the scope containing
    // their enum defination gives rise to the offical term for this kind
    // of enum: unscoped. Their new C++11 conterparts, scoped enums, don't
    // leak names in this way:
    enum class Color_ {black, white, red };  // black, white, red
                                            // are scoped to Color
    auto white = false; // fine, no other "white" in scope

    // Color c = white; // error! no enumerator named "white"
                     // is in this scope

    Color_ c = Color_::white; // fine

    auto c_ = Color_::white; // also fine

    enum class Status: std::uint32_t ; // forward declaration


    enum class Status: std::uint32_t {
        good = 0,
        failed = 1,
        incomplete = 100,
        corrupt = 200,
        audited = 500,
        indeterminate = 0xFFFFFFFF
    };

    using UserInfo =
        std::tuple<std::string, // name
                   std::string, // email
                   std::size_t>; // reputation

    enum class UserInfoFileds {uiName, uiEmail, uiReputation };

    template<typename E>
    constexpr std::underlying_type_t<E>
    toUtType(E enumerator) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(enumerator);
    }
}

namespace item11 {
    template<class charT, class traits = std::char_traits<charT>>
    class basic_ios : public std::ios_base {
        basic_ios(const basic_ios&) = delete;
        basic_ios& operator=(const basic_ios&) = delete;
    };

    template<typename T>
    void processPointer(T* ptr);

    template<>
    void processPointer<void>(void*) = delete;

    template<>
    void processPointer<char>(char*) = delete;


    class Widget {
    public:
        template<typename T>
        void processPointer(T* ) { }
    private:
        // NOTE(FIXME): I used clang and it compiled this code, w invokes this template specialization although it's private.
        //template<>
        //void processPointer<void>(void*) { std::cout << "sfad"; }; // error ! template specialization must be written at
                                          // namespace scope, not class scope.
    };

    template<>
    void Widget::processPointer<void>(void *) = delete; // still public, but deleted
}

namespace item12 {
    class Base {
    public:
        virtual void doWork() {};      // base class virtual function
    };

    class Derived: public Base {
    public:
        virtual void doWork() override {    // overrides Base::doWork ("virtual"
            std::cout << "doWork() in Derived class" << '\n';  // is optional is here)
        };
    };

    // reference qualifiers
    class Widget {
    public:
        // void doWork() &;  // this version of doWork applies only when *this is an lvalue
        // void doWork() &&; // this version of doWork applies only when *this is an rvalue

        using DataType = std::vector<double>;
        DataType& data() & { return values; } // for lvalue Widgets, return lvalue
        DataType data() && { return std::move(values); } // for rvalue Widgets, return rvalue

    private:
        DataType values;
    };


    Widget makeWidget() {
        return Widget();
    }
}

int main() {
    using namespace item12;

    std::unique_ptr<Base> upb = std::make_unique<Derived>();

    upb->doWork();  // calls doWork through base class ptr; derived class function is invoked.

    Widget w;
    auto vals1 = w.data();  // calls lvalue overload for Widget::data, copy-constructor vals1

    auto vals2 = makeWidget().data(); // calls rvalue overload for Widget::data, move-constructor vals2


    return 0;
}
