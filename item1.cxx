#include <iostream>


// ITEM 1: Understand template type deduction

/*
 *  Things to remember
 *
 *  # During template type deduction, arguments that are references are treated as
 *    non-references, i.e., their reference-ness is ignored.
 *
 *  # When deducing types for universal reference parameters, lvalue arguments get
 *    special treatment.
 *
 *  # When deducing types for by-value parameters, const and/or volatile arguments
 *    are treated as non-const and non-volatile.
 *
 *  # During template type deduction, arugments that are array or function names decay
 *    to pointers, unless they're used to initialize references.
 *
 * */


namespace item1 {

    // pseudocode
    /*
     *  template<typename T>
     *  void f(ParamType param);
     *
     *  A call can look like this:
     *
     *      f(expr);        // call f with some expression
     *
     *  During compliation, compliers use expr to deduce two types: one for T and one for ParamType.
     *  These types are frequently different, because ParamType often contains adornments, e.g., const
     *  or reference qualifiers. For example, if the template is declared like this,
     *
     *      template<typename T>
     *      void f(const T& param);     // ParamType is const T&
     *
     *  and We have this call:
     *
     *      int x = 0;
     *      f(x);   // call f with an int
     *
     *  T is deduduced to be int, but ParamType is deduced to be const int&
     *
     *  The type deduced for T is dependent not just on the type expr, but also on the form of ParamType.
     *  There are three cases:
     *
     *  1. ParamType is a pointer or reference, but not a universal reference.
     *  2. ParamType is a universal reference.
     *  3. ParamType is neither a pointer nor a reference.
     * */


    // A helper function
    template<typename T>
    void f(T&& param) {
        std::cout << std::boolalpha;
        std::cout << "T Type: " << "\n";
        std::cout << "Reference: " << std::is_reference_v<T> << "\n";
        std::cout << "Const: " << std::is_const_v<T> << "\n";
        std::cout << "Lvalue Reference: " << std::is_lvalue_reference_v<T> << "\n";
        std::cout << "Rvalue Reference: " << std::is_rvalue_reference_v<T> << "\n\n";

        using paramType = decltype(param);
        std::cout << std::boolalpha;
        std::cout << "ParamType Type: " << "\n";
        std::cout << "Reference: " << std::is_reference_v<paramType> << "\n";
        std::cout << "Const: " << std::is_const_v<paramType> << "\n";
        std::cout << "Lvalue Reference: " << std::is_lvalue_reference_v<paramType> << "\n";
        std::cout << "Rvalue Reference: " << std::is_rvalue_reference_v<paramType> << "\n\n";
    }

    template<typename T, std::size_t N>
    constexpr std::size_t arraySize(T (&)[N]) noexcept {
        return N;
    }

}


int main() {
    using namespace item1;

    // Case 1. ParamType is a Reference or Pointer, but not a Universal Reference
    // type deduction works like this:
    //  1. If expr's type is a reference, ignore the reference part.
    //  2. Then pattern-match expr's type against ParamType to determine T.

    // template<typename T>
    // void f(T& param)

    int x = 27; // x is an int
    const int cx = x;   // cx is a const int
    const int& rx = x;  // rx is a reference to x as a const int

    f(x);   // T is int, param's type is int&
    f(cx);  // T is const int, param's type in const int &
    f(rx);  // T is const int, param's type is const int &

    // template<typename T>
    // void f(const T& param);  // param is now a ref-to-const

    // Note: change f's paramType to see the difference

    f(x);   // T is int, param's type in const int &
    f(cx);   // T is int, param's type in const int &
    f(rx);   // T is int, param's type in const int &

    // Because we're assuming that param is a reference-to-const, there's no longer
    // a need for const to be deduced as part of T:

    // Case 2. ParamType is a Universal Reference.
    // 1. If expr is an lvalue, both T and ParamType are deduced to be lvalue references.
    // 2. If expr is an rvalue, the "normal" (Case 1) rules apply

    // template<typename T>
    // void f(T&& param);   // param is now a universal reference

    f(x);  // x is lvalue, so T is int&, param's type is also int&
    f(cx); // cx is lvalue, so T is const int&, param's type is also const int&
    f(rx); // rx is lvalue, so T is const int&, param's type is also const int&

    f(27); // 27 is rvalue, so T is int, param's type is therefor int&&

    // Case 3: ParamType is Neither a Pointer nor a Reference
    // template<typename T>
    // void f(T param)

    f(x);   // T's and param's types are both int
    f(cx);  // T's and param's types are again both int
    f(rx);  // T's and param's types are still both int


    const char* const ptr = "Fun with pointers";    // ptr is const pointer to const object

    f(ptr); // pass arg of type const char* const, the constness of ptr itself is ignored
            // when copying it to create the new pointer, param.


    const char name[] = "J. P. Biggs";
    const int arr[]{1, 2, 3};
    std::cout << arraySize(name) << "\n";
    std::cout << arraySize(arr) << "\n";



    return 0;
}
