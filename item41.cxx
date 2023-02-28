#include <iostream>
#include <future>
#include <random>


// ITEM 41: Consider pass by value for copyable parameters that are cheap
//          to move and always copied.

/*
 *  Things to remember
 *
 *  # For copyable, cheap-to-move parameters that are always copied, pass by value
 *    may be nearly as efficient as pass by reference, it's easier to implement, and
 *    it can generate less object code.
 *
 *  # Copying parameters via construction may be significantly more expensive than
 *    copying them assignment.
 *
 *  # Pass by value is subject to the slicing problem, so it's typically inappropriate
 *    for base class parameter types.
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



int main() {

    using namespace item41;

    std::string name("Bart");

    Widget w;

    w.addName(name);

    w.addName(name + "Jenne");


    SpecialWdiget sw;

    //processWdiget(sw); // processWidget sees a Widet, not a SpecialWidget!



    return 0;
}
