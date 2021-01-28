#include "catch.hpp"
#include "utils.hpp"

using namespace std;
using namespace Utils;

void have_fun(Gadget& g) // ver 1
{
    puts(__PRETTY_FUNCTION__);
    std::cout << "Having fun with " << g.name() << "\n";
}

void have_fun(const Gadget& g) // ver 2
{
    puts(__PRETTY_FUNCTION__);
    std::cout << "Having fun with " << g.name() << "\n";
}

void have_fun(Gadget&& g) // ver 3
{
    puts(__PRETTY_FUNCTION__);
    std::cout << "Having fun with " << g.name() << "\n";
}

// void use(Gadget& g)
// {
//     have_fun(g); // calls have_fun(Gadget&)
// }

// void use(const Gadget& g)
// {
//     have_fun(g); // calls have_fun(const Gadget&)
// }
 
// void use(Gadget&& g) /// g jest l-value (bo ma nazwę) typu r-value_reference do Gadget (Gadget&&)
// {   
//     //have_fun(g); // g jest l-value przesłanym do have_fun() -> ver 3 odpada, bo l-value g nie może zostać związane z Gadget&& -> dopasowane bedzie wywolanie have_fun(Gadget& g)
//     // -> have_fun(static_cast<Gadget&&>(g)) -> to zapewni wywołanie have_fun(Gadget&&)
//     have_fun(std::move(g)); // działa tak samo calls have_fun(Gadget&&)
// }

// TEST_CASE("simple case")
// {
//     Gadget g{1, "ipad"}; // g jest l-value typu Gadget
    
//     use(g);

//     use(Gadget{2, "ipod"}); // argument wywołania nie ma nazwy, więc jest to r-value Gadget // GOTO 35
// }

template <typename TGadget1, typename TGadget2>
void use(TGadget1&& g1, TGadget2&& g2)
{
    have_fun(std::forward<TGadget1>(g1));
    have_fun(std::forward<TGadget2>(g2));
}

TEST_CASE("perfect forwarding")
{
    Gadget g{1, "ipad"};
    const Gadget cg{2, "const ipad"};

    use(g, cg);
    use(cg, Gadget{5, "temp ipod"});
    use(Gadget{3, "temp ipad"}, Gadget{6, "temp key"});
}

///////////////////////////////////
// Reference collapsing

template <typename T>
void foo(T&& item)
{
    puts(__PRETTY_FUNCTION__);
}

// TEST_CASE("reference collapsing")
// {
//     int x = 4;
//     // foo(x);

//     foo(4);

//     auto&& ax1 = x; // int&
//     auto&& ax2 = 4; // int&&
// }


TEST_CASE("std::move does not move")
{
    const std::vector<int> vec = {1, 2, 3};

    std::vector<int> backup = vec; // copy
    std::vector<int> target = std::move(vec); // still copy 

    REQUIRE(target == std::vector{1, 2, 3});
    //REQUIRE(vec.empty()); // FAILS
}