#include <iostream>
#include <memory>   // include std::unique_ptr



// ITEM 18: Use std::unique_ptr for exclusive-ownership resource management.

/*
 *  Things to remember
 *
 *  # std::unique_ptr is a small, fast, move-only smart pointer for managing resources
 *    with exclusive-ownership semantics
 *
 *  # By default, resouce destruction takes place via delete, but custom deleters can
 *    be specified. Stateful deleters and function pointers as deleters increase the
 *    size of std::unique_ptr objects.
 *
 *  # Converting a std::unique_ptr to a std::shared_ptr is easy.
 *
 * */

namespace item18 {
    // A common use for std::unique_ptr is as a factory function return type for objects
    // in a hierarchy. Suppose we have a hierarchy for types of investments (e.g., stocks,
    // bonds, real estate, etc.) with a base class Investment.
    class Investment {
    public:
        virtual ~Investment();
    };

    class Stock: public Investment {

    };

    class Bond: public Investment {

    };

    class RealEstate: public Investment {

    };

    // custom deleter as stateless lambda
    auto delInvmt = [](Investment* pInvestment) {
        // makeLogEntry(pInvestment);
        delete pInvestment;
    };

    // custom deleter as function
    // return type has size of Investment* plus at least size of function pointer!
    void delInvmt2(Investment* pInvestment) {
        // makeLogEntry(pInvestment);
        delete pInvestment;
    }

    template<typename... Ts>
    //std::unique_ptr<Investment, decltype(delInvmt)>
    auto makeInvestment(Ts&&... params) {
        std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);

        if ( true ) {
            pInv.reset(new Stock(std::forward<Ts>(params)...));
        }
        else if ( true ) {
            pInv.reset(new Bond(std::forward<Ts>(params)...));
        }
        else if ( true ) {
            pInv.reset(new RealEstate(std::forward<Ts>(params)...));
        }

        return pInv;
    }

}


int main() {
    using namespace item18;

    {
        auto pInvestment = makeInvestment(); // pInvestment is of type std::unique_ptr<Investment>
    }   // destroy *pInvestment

    std::shared_ptr<Investment> sp = makeInvestment(); // converts std::unique_ptr to std::shared_ptr

    return 0;
}
