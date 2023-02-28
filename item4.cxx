#include <iostream>
#include <vector>

#include <boost/type_index.hpp>


// ITEM 4: Know how to view deduced types.

/*
 *  Things to remember
 *
 *  # Deduced types can often be seen using IDE editors, compiler error messages,
 *    and the Boost TypeIndex library
 *
 *  # The results of some tools may be neither helpful nor accurate, so an understanding
 *    of C++'s type deduction rules remain essential.
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

namespace item3 {
    // C++11's trailing return type syntax
    template<typename Container, typename Index>
    auto authAndAccess(Container& c, Index i) -> decltype(c[i]) {
        // authenticateUser();
        return c[i];
    }

    // C++ 14
    template<typename Container, typename Index>
    decltype(auto) authAndAccess_14(Container&& c, Index i) {
        // authenticateUser();
        return std::forward<Container>(c)[i];
    }


    class Widget {};

}

namespace item4 {
    template<typename T>
    class TD;   // declaration only for TD;
                // TD = "Type Displayer"

    class Widget {};

    template<typename T>
    void f(const T& param) {
        using namespace std;
        cout << "T =    " << typeid(T).name() << "\n";
        cout << "param =    " << typeid(param).name() << "\n";

        using boost::typeindex::type_id_with_cvr;

        cout << "T =    " << type_id_with_cvr<T>().pretty_name() << "\n";
        cout << "param =    " << type_id_with_cvr<decltype(param)>().pretty_name() << "\n";
    };

    std::vector<Widget> createVec() { return std::vector<Widget>{Widget()}; }
}

int main() {
    using namespace item4;

    const int theAnswer = 42;

    auto x = theAnswer;
    auto y = &theAnswer;

    // Compiler Diagnostics
    //TD<decltype(x)> xType;
    //TD<decltype(y)> yType;

    // Runtime Output
    std::cout << typeid(x).name() << "\n";
    std::cout << typeid(y).name() << "\n";

    const auto vw = createVec();

    if (!vw.empty()) {
        f(&vw[0]);
    }


    return 0;
}
