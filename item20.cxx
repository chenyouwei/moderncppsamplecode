#include <iostream>
#include <memory>   // include std::unique_ptr
#include <vector>
#include <unordered_map>



// ITEM 20: Use std::weak_ptr for std::shared_ptr like pointers that can dangle.

/*
 *  Things to remember
 *
 *  # Use std::weak_ptr for std::shared_ptr-like pointers that can dangle.
 *
 *  # Potential use cases for std::weak_ptr include caching, observer lists, and
 *    the prevention of std::shared_ptr cycles.
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

namespace item20 {
    // std::weak_ptr are typically created from std::shared_ptrs. They point to the
    // same place as the std::shared_ptrs initializing them, but they don't affect
    // the reference count the object they point to:
    class Widget {
    public:
        int x = 0;
    };

    template<typename WidgetID>
    std::shared_ptr<const Widget> loadWidget(WidgetID ) {
        return std::make_shared<const Widget>();
    }

    // caching version of loadWidget (Widget is read-only object):
    template<typename WidgetID>
    std::shared_ptr<const Widget> fastLoadWidget(WidgetID id) {
        static std::unordered_map<WidgetID, std::weak_ptr<const Widget>> cache;

        auto objPtr = cache[id].lock(); // objPtr is std::shared_ptr
                                        // to cached object (or null
                                        // if object's not in the cache)

        if (!objPtr) {
            objPtr = loadWidget(id);
            cache[id] = objPtr;
        }

        return objPtr;
    }


    // Observer design pattern. Each subject contains a data member holding
    // pointers to its observers. A reasonable design is for each subject to
    // hold a container of std::weak_ptrs to its observers.


    // shared_ptr cycles
    /*
     *      std::shared_ptr          std::shared_ptr
     *  A ------------------>  B  <--------------------- C
     *
     *      std::shared_ptr          std::shared_ptr
     *  A ------------------>  B  <--------------------- C
     *  ^
     *  |  std::weak_ptr       |
     *  |----------------------|
     *
     *
     * */

}


int main() {
    using namespace item20;

    auto spw = std::make_shared<Widget>();  // after spw is constructed, the pointed-to Widget's
                                            // ref count (RC) is 1

    std::weak_ptr<Widget> wpw(spw); // wpw points to same Widget as spw. RC remains 1

    spw = nullptr;  // RC goes to 0, and the Widget is destoryped. wpw now dangeles.

    if (wpw.expired()) {
        std::cout << "wpw has expired" << '\n';
    }


    // atomic operation that checks to see if the std::weak_ptr has expired and, if not,
    // gives you access to the object it points to. The operations comes in two forms:

    std::shared_ptr<Widget> spw1 = wpw.lock();  // if wpw's expired, spw1 is null

    auto spw2 = wpw.lock(); // same as above, but uses auto

    // other form:

    try {
        std::shared_ptr<Widget> spw3(wpw);  // if wpw's expired, throw std::bad_weak_ptr
    } catch (const std::bad_weak_ptr& e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}
