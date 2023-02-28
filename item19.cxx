#include <iostream>
#include <memory>   // include std::unique_ptr
#include <vector>



// ITEM 19: Use std::share_ptr for shared_ownership resouce management.

/*
 *  Things to remember
 *
 *  # std::shared_ptrs offer convience approaching that of garbage collection
 *    for the shared lifetime management of arbitrary resouces.
 *
 *  # Compared to std::unique_ptr, std::shared_ptr objects are typically twice
 *    as big, incur overhead for control blocks, and require atomic reference
 *    count manipulations.
 *
 *  # Default resouce destruction is via delete, but custom deleters are supported.
 *    The type of the deleter has no effect on the type of the std::shared_ptr.
 *
 *  # Avoid creating std::shared_ptrs from variables of raw pointer type.
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

namespace item19 {
    // A std::shared_ptr can tell whether it's the last one pointing to a resource
    // by consulting the resouce's reference count, a value associated with the
    // resource that keeps track of how many std::shared_ptrs point to it.

    // The existence of the reference count has performance implications:
    // 1. std::shared_ptrs are twice the size of a raw pointer.
    // 2. Memory for the reference count must be dynamically allocated.
    // 3. Increments and decrements of the reference count must be atomic.

    // custom deleters
    class Widget: public std::enable_shared_from_this<Widget> {
    public:
        void process();

        // factory function that perfect-forward args
        // to a private ctor
        template<typename... Ts>
        static std::shared_ptr<Widget> create(Ts&&... ) { return std::make_shared<Widget>(new Widget);  }
    private:
    };

    auto loggingDel = [](Widget *pw) {
        // makeLogEntry(pw);
        delete pw;
    };

    std::unique_ptr<Widget, decltype(loggingDel)>   // deleter type is part
        upw(new Widget, loggingDel);                // of ptr type

    std::shared_ptr<Widget>                         // deleter type is not
        spw(new Widget, loggingDel);                // part of ptr type

    auto customDeleter1 = [](Widget* pw) { delete pw; };    // custom deleters each
    auto customDeleter2 = [](Widget* pw) { delete pw; };    // with a different type

    std::shared_ptr<Widget> pw1(new Widget, customDeleter1);
    std::shared_ptr<Widget> pw2(new Widget, customDeleter2);
    // Because pw1 and pw2 have the same type, they can be placed in a cotainer of
    // that type, and aloso could pass to a function taking a parameter of type
    // std::shared_ptr<Widget>. None of these things can be down with std::unique_ptrs
    // that differ in the types of their custom deleters.
    std::vector<std::shared_ptr<Widget>> vpw{pw1, pw2};

    // Specifying a custom deleter doesn't change the size of a std::shared_ptr

    // Control block
    // There's a control block for each object
    //
    /*
     *   std::shared_ptr<T>
     *   ------------------------
     *   _    Ptr to T          -   --------->   T Object
     *   ------------------------
     *   - Ptr to Control Block -   --          Control Block
     *   ------------------------     --       ----------------------
     *                                   -->   - Reference Count    -
     *                                         ----------------------
     *                                         -   Weak Count       -
     *                                         ----------------------
     *                                         -   Other Data       -
     *                                         - (e.g., custom del- -
     *                                         -  eter, allocator,  -
     *                                         -  etc.)             -
     *                                         ----------------------
     * */

    // In general, it's impossible for a function creating a std::shared_ptr
    // to an object to know whether some other std::shared_ptr already points
    // to that object, so the following rules for control block creation are
    // used:
    //
    // 1. std::make_shared always creates a control block. It manufactures a new
    //    object to point to, so there is certainly no control block for that
    //    object at the time std::make_shared is called.
    // 2. A control block is created when a std::shared_ptr is constructed from
    //    a unique-ownership pointer (i.e., a std::unique_ptr a std::auto_ptr).
    // 3. When a std::shared_ptr constructor is called with a raw pointer, it
    //    creates a control block. If you wanted to create a std::shared_ptr from
    //    an object that already had a control block, you'd presumably pass a
    //    std::shared_ptr or std::weak_ptr as a constructor argument, not a
    //    raw pointer. std::shared_ptr constructors taking std::shared_ptrs or
    //    std::weak_ptrs as constructor arguments don't create new control blocks,
    //    because they can rely on the smart pointers passed to them to point to
    //    any necessary control blocks.


    std::vector<std::shared_ptr<Widget>> processWidgets;

    void Widget::process() {
        // this is raw pointer, enable_shared_from_this works.
        // CRTP: The Curiously Recurring Template Pattern (CRTP)
        processWidgets.emplace_back(shared_from_this()); // add it to the list of processed Widgets.
    }

}


int main() {
    using namespace item19;

    return 0;
}
