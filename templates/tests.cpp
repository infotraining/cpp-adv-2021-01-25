#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "catch.hpp"

using namespace std;

template <typename T>
void print(T t)
{
    std::cout << t << "\n";
}

// template <>
// void print<int>(int x)
// {
//     std::cout << "INT: " << x << "\n";
// }

void print(int x)
{
    std::cout << "INT: " << x << "\n";
}

template <typename T>
void print(T* ptr)
{
    std::cout << *ptr << "\n"; 
}

TEST_CASE("function templates")
{
    int x = 10;
    
    print(x);
    print(&x); // print<int*> -> GOTO 28

    void(*ptr_fun)(double) = &print<double>; // print<double>

    REQUIRE(1 == 1);
}

template <typename T, typename Container>
class StackA
{   
    Container container_;
public:
    using value_type = T;

    void push(const T& item)
    {
        container_.push_back(item); // insert a copy of item on stack
    }

    void push(T&& item) // tu nie ma uniwersalnej referencji, bo push(T&&) nie jest funkcją szablonowa
    {
        container_.push_back(std::move(item)); // moves item in stack
    }

    T pop()
    {
        T top_value = std::move(container_.back());
        container_.pop_back();
        return top_value;
    }
};

template <
    typename T, // 1st paramter - type
    template <typename, typename> class Container, // 2nd parameter - template<T1, T2>
    typename TAllocator = std::allocator<T>> // 3rd param - type; has default value std::allocator<T>
class StackB
{   
    Container<T, TAllocator> container_;
public:
    using value_type = T;
    
    template <typename TArg>
    void push(TArg&& item) // TArg&& jest uniwersalna referencja, bo push() jest funkcja szablonowa
    {
        container_.push_back(std::forward<TArg>(item)); // moves item in stack
    }

    T pop()
    {
        T top_value = std::move(container_.back());
        container_.pop_back();
        return top_value;
    }
};

TEST_CASE("class templates")
{
    std::vector<int> vec1 = {1, 2, 3};
    std::vector vec2 = {1, 2, 3}; // std::vector<int>

    //          parameter must be a type
    StackA<int, std::vector<int>> s1; 
    s1.push(42);

    decltype(s1)::value_type item = s1.pop();
    REQUIRE(item == 42);


    //          parameter is a template
    StackB<int, std::vector> s2;
}


template <typename T> 
void dependent_name(const T& container)
{
    using TValue = typename T::value_type; // typename podpowiada kompilatorowi, ze value_type to typ zdefiniowany wewnatrz T
};

TEST_CASE("dependent name")
{
    StackB<int, std::vector> s;

    dependent_name(s);
}

/////////////////////////////////////////
// template aliases - C++11

template <typename T>
using StringKeyMap = std::map<std::string, T>;

template <typename T1, typename T2>
using ReversedMap = std::map<T2, T1>;

StringKeyMap<int> dict;
ReversedMap<int, string> rev_map = { {"one"s, 1}, {"two"s, 2} };

/////////////////////////////////////////
// variable templates - C++14

template <typename T>
constexpr T pi{3.1415926535897932385};

TEST_CASE("variable templates & pi")
{
    float r = 8.0;

    auto area = pi<int> * r * r;
}