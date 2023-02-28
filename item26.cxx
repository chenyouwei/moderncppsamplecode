#include <iostream>
#include <set>
#include <memory>


// ITEM 26: Avoid overloading on unversial references.

/*
 *  Things to remember
 *
 *  # Overloading on universal references almost always leads to the universal references overload
 *    being called more frequently than expected.
 *
 *  # Perfect-forwarding constructors are especially problematic, because they're typically better
 *    matches than copy constructos for non-const lvalues, and they can hijack derived class class
 *    to base class copy and move constuctors.
 *
 * */



namespace item22 {
    // C++ 11 basic version
    template<typename T, typename... Ts>
    std::unique_ptr<T> make_unique(Ts&&... params) {
        return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
    }
}

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

namespace item24 {
    /*
     *  Universal references arise in two contexts. The most common is function template parameters:
     *
     *      template<typename T>
     *      void f(T&& param);      // param is a universal reference
     *
     *  The second context is auto declarations:
     *
     *      auto&& var2 = var1;
     *
     *  these contexts have in common is the presence of type deduction. If you see "T&&" without
     *  type deduction, you're looking at an rvalue reference.
     *
     *
     *  Consider this push_back member function in std::vector:
     *
     *      template<class T, class Allocator = allocator<T>>
     *      class vector{
     *      public:
     *          void push_back(T&& x);      // from C++ Standards
     *      }
     *
     *  push_back's paramter certainly has the right form for a univeral reference, but there's no
     *  type deduction in this case. That's because push_back can't exist withhout a particular
     *  vector instantiation for it to be part of, and the type of that instantiation fully determines
     *  the declaration for push_back. That is, saying
     *
     *      std::vector<Widget> v;
     *
     *  causes the std::vector template to be instanitaited as follows:
     *
     *      class vector<Widget, allocator<Widget>> {
     *      public:
     *          void push_back(Widget&& x); // rvalue reference
     *      }
     *
     *  In contrast, emplace_back member function in std::vector does employ type deduction:
     *
     *      template<class T, class Allocator = allocator<T>>
     *      class vector {
     *      public:
     *          template<class... Args>
     *          void emplace_back(Args&&... args);
     *      }
     *
     *  Here, the type paramters Args is independent of vector's type parameter T, so Args must be deduced each
     *  time emplace_back is called.
     *
     * */
}

namespace item25 {
    class Widget {
    public:
        // rhs is a rvalue reference
        Widget(Widget&& rhs) : name(std::move(rhs.name)), p(std::move(rhs.p)) {};

        template<typename T>
        void setName(T&& newName) {
            name = std::forward<T>(newName); // newName is universal reference
        }

    private:
        std::string name;
        std::shared_ptr<int> p;
    };

    struct Fraction {
        void reduce();
    };

    template<typename T>        // by-value return, universal reference param
    Fraction reduceAndCopy(T&& frac) {
        frac.reduce();
        return std::forward<T>(frac);   // move rvalue into return value, copy lvalue
    }
}

namespace item26 {
    std::multiset<std::string> names;   // global data structure

    template<typename T>
    void logAndAdd(T&& name) {
        std::cout << "Universal logAnddAdd Called" << std::endl;
        //auto now = std::chrono::system_clock::now(); // get current time
        names.emplace(std::forward<T>(name));        // add name to global data structure; see Item42 for info on emplace
    }

    std::string nameFromIdx(int ) {
        return "a";
    };  // return name corresponding to idx


    // overload logAndAdd
    void logAndAdd(int idx) {
        std::cout << "Int logAndAdd Called" << std::endl;
        //auto now = std::chrono::system_clock::now(); // get current time
        names.emplace(nameFromIdx(idx));
    }


    class Person {
    public:
        template<typename T>
        explicit Person(T&& n): // perfect forwarding ctor;
            name(std::forward<Person&>(n)) { std::cout << "univeral called" << std::endl; };

        explicit Person(int idx): // int ctor
           name(nameFromIdx(idx)) { std::cout << "int called" << std::endl; }; // int ctor

        //Person(const Person& rhs); // copy ctor
        //                           // (compiler-generated)
        //Person(Person&& rhs);      // move ctor
        //                           // (compiler-generated)

    private:
        std::string name;
    };

    class SpecialPerson: public Person {
    public:
        SpecialPerson(const SpecialPerson& rhs): Person(rhs) {}  // copy ctor; calls
                                                                 // base class
                                                                 // forwarding ctor!
        SpecialPerson(SpecialPerson&& rhs): Person(std::move(rhs)) {} // move ctor; calls
                                                                      // base class
                                                                      // forwarding ctor!
    };

}


int main() {
    using namespace item26;
    std::string petName("Darla");


    // these calls all invoke the T&& overload
    logAndAdd(petName);
    logAndAdd(std::string("Persephone"));
    logAndAdd("Patty Dog");

    logAndAdd(22); // calls int overload

    //short nameIdx = 1;
    //logAndAdd(nameIdx); // error!

    Person p("Nancy");

    // Compilers reason: cloneOfP is being initialized with non-const lvalue(p)
    // that means that the templatized constructor can be initialized to take
    // an non-const lvalue of type Person.
    //auto cloneOfP(p);  // crate new Person from p;
    //                   // this won't compile

    const Person cp("Nancy");
    auto cloneofP(cp); // calls copy constructor!


  return 0;
}
