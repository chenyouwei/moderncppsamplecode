#include <iostream>
#include <memory>   // include std::unique_ptr
#include <functional>  // include std::function


using namespace std;

// ITEM 5: Prefer auto to explicit type declarations.

/*
 *  Things to remember
 *
 *  # auto variables must be initialized, are generally immune to type mismatches that can
 *    lead to portability or efficiency problems, can ease the process of refactoring, and
 *    typically require less typing than variables with explictly specified types.
 *
 *  # auto-typed variables are subject to the pitfalls described in Item 2 and 6.
 * */


namespace item5 {
    template<typename It>
    void dwim(It b, It e) {     // algorithm to dwin ("do what I mean")
        while (b != e) {        // for all elements in range from b to e
            //typename std::iterator_traits<It>::value_type currVaule = *b;
            auto currValue = *b;
        }
    }

    class Widget {
    };

    bool operator<(const Widget&, const Widget& ) {
        return false;
    }

}



int main() {
    using namespace item5;

    auto derefUPLess = [](const std::unique_ptr<Widget>&p1,
                          const std::unique_ptr<Widget>&p2)
                        { return *p1 < *p2; };

    // C++ 14
    auto derefLess = [](const auto& p1, const auto&p2)
                     { return *p1 < *p2; };

    /*
     *  std::function is a template in the C++11 Standard Library that generalizes
     *  the idea of a function pointer. Whereas function pointers can point only to
     *  functions, however, std::function objects can refer to any callable object,
     *  i.e., to anything that can be invoked like a function.
     * */

    std::function<bool(const std::unique_ptr<Widget>&, const std::unique_ptr<Widget>&)>
    derefUPLess_11 = [](const std::unique_ptr<Widget>&p1,
                        const std::unique_ptr<Widget>&p2)
                        { return *p1 < *p2; };


    return 0;
}
