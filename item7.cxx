#include <iostream>
#include <memory>   // include std::unique_ptr
#include <functional>  // include std::function
#include <vector>



// ITEM 7: Distinguish between () and {} when creating objects.

/*
 *  Things to remember
 *
 *  # Braced intialization is the most widely usable initialization syntax, it prevents
 *    narrowing conversions, and it's immune to C++'s most vexing parse.
 *
 *  # During constructor overload resolution, braced initializers are matched to std::initializer_list
 *    parameters if at all possible, even if other constructors offer seemingly better macthes.
 *
 *  # An example of where the choice between parentheses and braces can make a significant
 *    is creating a std::vector<numeric type> with two arguments.
 *
 *  # Choosing between parentheses and braces for object creation inside templates can be chanllenging.
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

int main() {
    using namespace item7;

    // A novel feature of braced initialization is that is prohibits implicit
    // narrowing coversions among built-in types.

    //double x_ = 0;
    //double y_ = 0;
    //double z_ = 0;

    //int sum1{x_ + y_ + z_}; // error!

    //int sum2(x_ + y_ + z_); // okay(value of expression truncated to a int)

    // Braced initialzation is its immunity to C++'s most vexing parse.
    Widget w1(10);  // call Widget ctor with argument 10

    //Widget w2(); // most vexing parse! declares a function named w2 that
                 // returns a Widget!

    Widget w3{}; // calls Widget ctor with no args;

    // If there's any way for compiers to construct a call using a braced initliazer
    // to be a constructor taking a std::initializer_list, compilers empoly that
    // interpretation
    Widget w4{10, 5.0}; // uses braces, but now calls std::initializer_list ctor
                        // (10 and 5.0 convert to long double)

    Widget w5(w4);  // uses parens, calls copy ctor
    // Note: I used clang and it treated it as copy constuctor ( :(, TRY TO FIXME)
    Widget w6{w4};  // uses braces, calls std::initializer_list ctor
                    // (w4 converts to float, and float converts to long double)

    Widget w7(std::move(w4)); // uses parens, calls move ctor
    Widget w8{std::move(w4)}; // uses braces, calls std::initializer ctor
                              // (for same reason as w6)

    //Widget w{10, 5.0}; // try to call the constructor taking a std::initializer_list<bool>
                         // Calling that constructor would require converting an int(10) and
                         // a double (5.0) to bools. Both conversion would be narrowing (bool
                         // can't exactly represent either value), and narrowing convesions
                         // are prohibited inside braced initializers, so the call is invalid.



    return 0;
}
