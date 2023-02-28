#include <iostream>


// ITEM 23: Understand std::move and std::forward

/*
 *  Things to remember
 *
 *  # std::move performs an unconditional cast to an rvalue. In and of itself, it doesn't move anything.
 *
 *  # std::forward casts its argument to an rvalue only if that argument is bound to rvalue.
 *
 *  # Neither std::move nor std::forward do anything at runtime.
 * */

namespace item23 {
    // sample implementation of std::move in C++ 11.
    template<typename T>
    typename std::remove_reference<T>::type&&
    move(T&& param) {
        using ReturnType = typename std::remove_reference<T>::type&&;
        return static_cast<ReturnType>(param);
    }

    // C++14 implementation
    template<typename T>
    decltype(auto) move(T&& param) {
        using ReturnType = std::remove_reference<T>&&;
        return static_cast<ReturnType>(param);
    }


    class Annotation {
    public:
        // "move" text into value; this code doesn't do what it seems to!
        // the result of std::move(text) is an rvalue of type const std::string. That rvalue
        // can't be passed to std::string's move constructor, because the move constructor
        // takes an rvalue refernce to a non-const std::string. The rvalue, however, be passed
        // to the copy constructor, because an lvalue-reference-to-const is permitted to bind to
        // a const rvalue.
        //
        // There are two lessons to be drawn from this example. First, don't declare objects const
        // if you want to be able to move from them. Move requests on const objects are silently
        // transformed into copy operations. Second, std::move not only doesn't actualy move anything,
        // it doesn't even guarantee that the object it's casting will be eligible to be moved. The only
        // thing you know for sure about the result of applying std::move to an object it that it's an
        // rvalue.
        explicit Annotation(const std::string text) : value(std::move(text)) {}; // param to be copied
    private:
        std::string value;
    };

    class string {
    public:
        string(const string& rhs); // copy ctor
        string(string&& rhs); // move ctor
    };


    class Widget {
    public:
        //Widget(Widget&& rhs) : s(std::move(rhs.s)) {
        Widget(Widget&& rhs) : s(std::forward<std::string>(rhs.s)) {
            ++moveCtorCalls;
        }

        Widget(std::string value): s(std::move(value)) {
        }

        std::string gets() { return s;}

    private:
        static std::size_t moveCtorCalls;
        std::string s;
    };

    void process(const Widget& ) {
        std::cout << "Called lvalue process" << std::endl;
    }; // process lvalues
    void process(Widget&& ) {
        std::cout << "Called rvalue process" << std::endl;
    }; // process rvalues

    // template that passes param to process
    template<typename T>
    void logAndProcess(T&& param) {
        //auto now = std::chrono::system_clock::now();  // get current time
        // makeLogEntry("Calling 'process'", now);

        // std::forward is a conditional cast: it casts to an rvalue only if its argument
        // was initialized with an rvalue.
        process(std::forward<T>(param));
    }

}


int main() {
    using namespace item23;

    auto s = std::string("a");
    Widget w(s);
    logAndProcess(w);
    logAndProcess(std::move(w));

    std::cout << s << std::endl;
    std::cout << w.gets() << std::endl;

    return 0;
}
