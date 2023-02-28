#include <iostream>
#include <future>
#include <regex>
#include <memory>
#include <list>


// ITEM 42: Consider emplacement instead of insertion

/*
 *  Things to remember
 *
 *  # In principle, emplacement function should sometimes be more efficient that
 *    their insertion counterparts, and they should never be less efficient.
 *
 *  # In practice, they're most likely to be fast than when (1) the value being
 *    added is constructed into container, not assigned; (2)
 *
 * */

namespace item41 {
    class Widget {
    public:
//        void addName(const std::string& newName) { // take lvalue, copy it
//            std::cout << "1" << "\n";
//            names.push_back(newName);
//        }
//
//        void addName(std::string&& newName) { // take rvalue, move it
//            std::cout << "2" << "\n";
//            names.push_back(std::move(newName));
//        }
//
//        template<typename T>
//        void addName(T&& newName) {
//            names.push_back(std::forward<T>(newName));
//        }

        // Should consider pass by value only for paramters that are always copied
        void addName(std::string newName) { // take lvalue or rvalue, move it
            std::cout << "3" << "\n";
            names.push_back(std::move(newName));
        }

        // std::unique_ptr is a move-only type, so the "overloading" approach
        // to its setter consists of a single function
        void setPtr(std::unique_ptr<std::string>&& ptr) {
             p = std::move(ptr);
        }

    private:
        std::vector<std::string> names;
        std::unique_ptr<std::string> p;
    };


    // assignment
    class Password {
    public:
        explicit Password(std::string pwd): text(std::move(pwd)) { // pass by value construct text
        }

        // two dynamic memory management actions (allocate, deallocate)
        void changeTo(std::string newPwd) { // pass by value assign text
            text = std::move(newPwd);
        }

        void changeTo(const std::string& newPwd) { // can reuse text's memory if
            text = newPwd;                         // text.capacity() >= newPwd.size()
        }

    private:
        std::string text;   // text of password
    };

    // Slicing Problem
    class SpecialWdiget: public Widget{
    public:
        int spw_a = 1;
    };

    void processWdiget(Widget ) {} // func for any kind of Widget
                                    // including derived types
                                    // suffers from slicing problem

}

namespace item42 {
    std::vector<std::string> vs;    // container of std::string

    // from the C++11 Standard
    template<class T, class Allocator = std::allocator<T>>
    class vector_example {
    public:
        void push_back(const T& x);     // insert lvalue
        void push_back(T&& x);          // insert rvalue
    };

}



int main() {
    using namespace item42;

    // 1. A temporary std::string object is created from the string literal "xyzzy". This
    //    object has no name; we'll call it temp. Construction of temp is the first std::string
    //    construction. Because it's a temporary object, temp is an rvalue.

    // 2. temp is passed to the rvalue overload for push_back, where it's bound to the rvalue reference
    //    parameter x. A copy of x is then constructed in the memory for the std::vector. This construction
    //    -the second one-is what actually creates a new object inside the std::vector. (The constructor
    //    that's used to copy x into the std::vector is the move constructor, because x, being an rvalue
    //    reference, gets cast to an rvalue before it's copied.)

    // 3. Immediately after push_back returns, temp is destroyed, thus calling the std::string destructor.

    vs.push_back("xyzzy");


    vs.push_back(std::string("xyzzy")); // create temp. std::string
                                        // and pass it to push_back



    vs.emplace_back("xyzzy");   // construct std::string inside
                                // vs directly from "xyzzy"



    // emplacement will almost certainly outperform insertion
    // 1. The value being added is constructed into the container, not assigned.
    //    node-based containers virtually always use construction to add new values.
    //    std::vector, std::deque, std::string are not node-based.
    // 2. The argument type(s) being passed differ from the type held by the container.
    // 3. The container is unlikely to reject the new value as a duplicate

    class Widget{};

    std::list<std::shared_ptr<Widget>> ptrs;


    // emplacement interaction with explict constructors
    // std::regex r1 = nullptr;

    std::regex r2(nullptr);

    // The syntax used to initializer r1(empolying the equal signs) corresponding
    // to what is known as copy initialization. In constract, the syntax used to
    // initialize r2(with the parentheses, although braces may be used instead)
    // yields what is called direct initialization. Copy initialization is not permitted
    // to use explict constructors. Direct initialization is. That's why the line r1
    // doesn't compile, but the line initlializing r2 does.

    std::vector<std::regex> regexes;

    regexes.emplace_back(nullptr);  // compiles. Direct init permits
                                    // use of explict std::regex
                                    // ctor taking a pointer

    // regexes.push_back(nullptr);     // error! copy init forbids
                                    // use of that ctor





    return 0;
}
