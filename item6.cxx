#include <iostream>
#include <memory>   // include std::unique_ptr
#include <functional>  // include std::function
#include <vector>


using namespace std;

// ITEM 6: Uses the explicitly typed initializer idiom when auto deduces undesired types.

/*
 *  Things to remember
 *
 *  # "Invisible" proxy types can cause auto to deduce the "wrong" type for an intializing expression.
 *
 *  # The explicitly typed intializer idiom forces auto to deduce the type you want it to have.
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

namespace item6 {
    class Widget {};
    std::vector<bool> features(const Widget& ) {
        return std::vector<bool>{true, false, true, false, true, true};
    }

    void processWidget(Widget&, bool) {};

    namespace std_ {
        // primary template
        template<class T, class Allocator>
        class vec {
        public:
            T operator[](std::size_t) {};
        };

        // partial specialization
        template<class Allocator>
        class vec<bool, Allocator> {
        public:
            class reference {};

            reference operator[](std::size_t) {};
        };
    }


}


int main() {
    using namespace item6;

    // suppose that bit 5 indicates whether the Widget has high priority.
    // We can thus write code like this:
    Widget w;

    //bool highPriority = features(w)[5]; // is w high priority?
    auto highPriority = features(w)[5]; // is w high priority?

    // undefined behavior!
    processWidget(w, highPriority); // Process w in accord with is priority.
                                    // highProity contains dangling pointer!

    // operator[] for std::vector<bool> returns an object of type
    // std::vector<bool>::reference ( a class nested inside std::vector<bool>)
    // std::vector<bool> is specifed to represent its bool in packed form, one
    // bit per bool.

    // std::vector<bool>::reference is an exmaple of proxy class: a class that
    // exists for the purpose of emulating and augmenting the behavior of some
    // other type.


    // avoid code of this form:
    // auto someVar = expression of "invisible" proxy class type.

    // the explicitly typed initializer idiom
    auto highPriority_ = static_cast<bool>(features(w)[5]);

    return 0;
}
