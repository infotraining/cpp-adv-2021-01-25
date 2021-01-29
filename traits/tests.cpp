#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;


template <typename T>
constexpr T pi(3.1415926535897932385);

TEST_CASE("variable templates & pi")
{
    float r = 1.0;

    auto area = pi<float> * r * r;

    REQUIRE(area == ::Approx(3.14159));
}

// template <typename T>
// void print(const T& value)
// {
//     std::cout << value << "\n";
// } 

// TEST_CASE("unconstrained templates")
// {
//     print(42);

//     print(std::vector{1, 2, 3});
// }

int foo(int x)
{
    return 2 * x;
}

// meta-funkcja, która przyjmuje jako we stałą int -> wartosc int
template <int x>
struct Foo
{
    static constexpr int value = 2 * x;
};

// meta-funkcja, która przyjmuje typ jako parametr wejsciowy -> wartosc size_t
template<typename T>
struct TypeSize
{
    static constexpr std::size_t value = sizeof(T);
};

// meta-funkcja, która przyjmuje typ -> typ
template <typename TContainer>
struct ValueType
{
    using type = typename TContainer::value_type;
};

// specjalizacja czesciowa ValueType dla tablic
template <typename T, size_t N>
struct ValueType<T[N]>
{
    using type = T;
};

// alias, który upraszcza dostep do ::type
template<typename TContainer>
using ValueType_t = typename ValueType<TContainer>::type;


template <typename TContainer> 
void some_algorithm(const TContainer& container)
{
    //using ElementType = typename ValueType<TContainer>::type; // lepiej zastosowac alias
    using ElementType = ValueType_t<TContainer>;

    ElementType value{};

    std::cout << value << "\n";
}

TEST_CASE("functions vs. meta-functions")
{
    REQUIRE(foo(2) == 4);

    REQUIRE(Foo<2>::value == 4);

    REQUIRE(TypeSize<int>::value == 4);

    std::vector<int> vec;
    some_algorithm(vec); 

    int tab[10]   ;
    some_algorithm(tab);
}