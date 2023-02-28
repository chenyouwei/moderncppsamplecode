#include <iostream>
#include <set>
#include <vector>
#include <memory>


// ITEM 30: Familiarize yourself with perfect forwarding failure cases.

/*
 *  Things to remember
 *
 *  # Perfect forwarding fails when template type deduction fails or when it
 *    deduces the wrong type.
 *
 *  # The kinds of arguments that lead to perfect forwarding failure are braced
 *    initializers, null pointers expressed as 0 and NULL, declration-only integral
 *    const static data members, template and overloaded function names, and bitfields.
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
 *  vector instaniation for it to be part of, and the type of that instantiation fully determines
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
        return "";
    };  // return name corresponding to idx


    // overload logAndAdd
    void logAndAdd(int idx) {
        std::cout << "Int logAndAdd Called" << std::endl;
        //auto now = std::chrono::system_clock::now(); // get current time
        names.emplace(nameFromIdx(idx));
    }


    class Person {
    public:
        //template<typename T>
        //explicit Person(T&& n): // perfect forwarding ctor;
        //    name(std::forward<T>(n)) { std::cout << "univeral called" << std::endl; };

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

namespace item27 {
    using item26::nameFromIdx;

    // Pass by value
    class Person_PBV {
    public:
        explicit Person_PBV(std::string n) // replace T&& ctor
            :name(std::move(n)) {}     // see Item 41 for use of std::move

        explicit Person_PBV(int idx)
            : name(nameFromIdx(idx)) {}
    private:
        std::string name;
    };

    // Use tag dispacth
    std::multiset<std::string> names;


    // non-integral argument: add it to global data structure.
    template<typename T>
    void logAndAddImpl(T&& name, std::false_type) {
        names.emplace(std::forward<T>(name));
    }


    template<typename T>
    void logAndAdd(T&& name) {
        //auto now = std::chrono::system_clock::now();
        //names.emplace(std::forward<T>(name));
        //logAndAddImpl(std::forward<T>(name), std::is_integral<typename std::remove_reference<T>::type>());

        // C++ 14
        logAndAddImpl(std::forward<T>(name), std::is_integral<std::remove_reference<T>>());
    }

    void logAndAddImpl(int idx, std::true_type) {
        logAndAdd(nameFromIdx(idx));
    }

    // Constraining templates that take universal references
    class Person {
    public:
        template<typename T, typename = typename std::enable_if<!std::is_same<Person, typename std::decay<T>::type>::value>::type>
        explicit Person(T&& n): // perfect forwarding ctor;
            name(std::forward<T>(n)) { std::cout << "univeral called" << std::endl; };

        explicit Person(int idx): // int ctor
           name(nameFromIdx(idx)) { std::cout << "int called" << std::endl; }; // int ctor

        //Person(const Person& rhs); // copy ctor
        //                           // (compiler-generated)
        //Person(Person&& rhs);      // move ctor
        //                           // (compiler-generated)

    private:
        std::string name;
    };
}


namespace item28 {
    // the reference collapse to a single reference according to this rule:
    // =====================================================================================
    // = If either reference is an lvalue reference, the result is an lvalue reference.    =
    // = Otherwise(i.e., if both are rvalue references) the result is an rvalue reference. =
    // =====================================================================================

    template<typename T>
    T&& forward(typename std::remove_reference<T>::type& param) {
        return static_cast<T&&>(param);
    }

    // Four contexts that reference collapse happens.
    // 1. template instantiation
    // 2. auto type deduction
    // 3. typedefs
    // 4. alias declarations.
}

namespace item29 {
    /*
     *  Consider std::array, a new container in C++11. std::array is essentially a built-in array
     *  with a STL interface. This is fundamentally different from the other standard containers,
     *  each of which stores its contents on the heap. Objects of such container types hold (as data
     *  member), conceptually, only a pointer to the heap memory storing the contents of the container.
     *  The existence of this pointer makes it possible to move the contents of an entire container in
     *  constant time: just copy the pointer to the container's contents from the source container to
     *  the target, and set the source's pointer to null
     * */

    class Widget {};

    // put data into vw1
    std::vector<Widget> vw1;

    // move vw1 into vw2. Runs in constant time.
    // Only ptrs in vw1 and vw2 are modified
    auto vw2 = std::move(vw1);

    /*
     *  std::array objects lack such a pointer, because the data for a std::array's contents are stored
     *  directly in the std::array object
     * */

    std::array<Widget, 10000> aw1;

    // move aw1 into aw2. Runs in linear time. All elements in aw1 are moved into aw2.
    auto aw2 = std::move(aw1);
}

namespace item30_braceinit {
    // Braced initializers
    void f(const std::vector<int>& ) {};

    // This is the form you'll see in, among other places, the standard containers'
    // emplacement functions (see Item 42) and the smart pointer factory functions,
    // std::make_shared and std::make_unique(see Item 21)
    template<typename... Ts>
    void fwd(Ts&&... params) {  // accept any arguments
        f(std::forward<Ts>(params)...);  // forward them to f
    }
}

namespace item30_declonlyint {
    // Declaration-only integral static const data members
    class Widget {
    public:
        static const std::size_t MinVals = 28;  // MinVal's declaration
    };

    // ... // no defn. for MinVals
    const std::size_t Widget::MinVals;

    void f(std::size_t ) {};

    template<typename... Ts>
    void fwd(Ts&&... params) {  // accept any arguments
        f(std::forward<Ts>(params)...);  // forward them to f
    }
}

namespace item30_overload {
    // Overloaded function names and template names
    void f(int (*)(int)) {};

    int processVal(int value) { return value; };

    int processVal(int value, int priority) { return value + priority; };

    template<typename T>
    T workOnVal(T param) { return param; } // template for processing values

    using ProcessFuncType = int (*)(int);

    template<typename... Ts>
    void fwd(Ts&&... params) {  // accept any arguments
        f(std::forward<Ts>(params)...);  // forward them to f
    }

}

namespace item30_bitfield {
    struct IPv4Header {
        std::uint32_t version:4,
                      IHL: 4,
                      DSCP: 6,
                      ECN: 2,
                      totalLength: 16;
    };

    void f(std::size_t ) {};

    template<typename... Ts>
    void fwd(Ts&&... params) {  // accept any arguments
        f(std::forward<Ts>(params)...);  // forward them to f
    }

}


int main() {
   item30_braceinit::f({1, 2, 3}); // fine, "{1, 2, 3}" implicitly converted to std::vector<int>

   // item30_braceinit::fwd({1, 2, 3}); // error! not compile

   item30_declonlyint::f(item30_declonlyint::Widget::MinVals); // fine, treated as "f(28)"

   item30_declonlyint::fwd(item30_declonlyint::Widget::MinVals); // error! shouldn't link

   item30_overload::f(item30_overload::processVal); // fine

   // item30_overload::fwd(item30_overload::processVal); // error! which processVal?

   // item30_overload::fwd(item30_overload::workOnVal); // error! which workOnval instantiation

   item30_overload::ProcessFuncType processValPtr = item30_overload::processVal;
   item30_overload::fwd(processValPtr);
   item30_overload::fwd(static_cast<item30_overload::ProcessFuncType>(item30_overload::workOnVal));


   item30_bitfield::IPv4Header h;

   item30_bitfield::f(h.totalLength);

   // A non-const reference shall not be bound to a bit-field
   //item30_bitfield::fwd(h.totalLength);

   auto length = static_cast<std::uint16_t>(h.totalLength);

   item30_bitfield::fwd(length);


   return 0;
}
