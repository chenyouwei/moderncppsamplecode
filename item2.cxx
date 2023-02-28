#include <iostream>
#include <vector>


// ITEM 2: Understand auto type deduction

/*
 *  Things to remember
 *
 *  # auto type deduction is usually the same as template type deduction, but auto type deduction
 *    assumes that a braced initializer represents a std::initializer_list, and template type
 *    deduction doesn't.
 *
 *  # auto in a function return type or a lambda parameter implies template type deduction, not
 *    auto type deduction.
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

namespace item2 {
    // When a variable is declared using auto, auto plays the role of T in the template,
    // and the type specifier for the variable acts as ParamType.

    auto x1 = 27;   // type is int, value is 27
    auto x2(27);    // ditto
    auto x3 = {27}; // type is std::initializer_list<int>, value is {27};
    auto x4{27};    // ditto

}


int main() {
    using namespace item2;

    std::vector<int> v{1, 2, 3};
    // auto resetV = [&v](const auto& newValue) { v = newValue; }; // C++ 14

    // resetV({1, 2, 3}) // error! can't deduced type for {1, 2, 3}
    //

    return 0;
}
