#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

int add(int a, int b) 
{
    return a + b;
}

using PtrFun2Arg = int(*)(int, int);
//typedef int (*PtrFun2Arg)(int, int);

int scale_by_2(int x)
{
    static int call_counter = 0;
    ++call_counter;
    std::cout << "call_counter: " << call_counter << std::endl;

    return x * 2;
}

using PtrFun1Arg = int(*)(int);

namespace v1
{
    auto calculate(const std::vector<int>& data, PtrFun1Arg ptr_f)
    {
        auto result = data; // copy of vector

        for(auto& item : result)
            item = ptr_f(item); // function call using ptr_f

        return result;
    }
}

class Logger
{
    Logger() { std::cout << "Logger()" << std::endl; }
    ~Logger() { std::cout << "~Logger()" << std::endl; }
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& msg)
    {
        std::cout << "Logging: " << msg << std::endl;
    }

    static Logger& instance()
    {
        static Logger log_instance;
        return log_instance;
    }
};

TEST_CASE("singleton")
{
    std::cout << "---------------\n";
    Logger::instance().log("Start");
    Logger::instance().log("End");
}

TEST_CASE("functions & function pointers")
{
    int result = add(4, 5);
    REQUIRE(result == 9);

    PtrFun2Arg ptr_f = add;
    ptr_f = &add;

    REQUIRE(ptr_f(1, 2) == 3);
    REQUIRE((*ptr_f)(2, 2) == 4);

    std::vector vec = {1, 2, 3};

    auto result_vec = v1::calculate(vec, &scale_by_2);
    REQUIRE(result_vec == std::vector{2, 4, 6});
}

/////////////////////////////////////////////
// Functors (function objects)

class ScaleBy
{
    int counter_ = 0;
    int factor_;
public:
    ScaleBy(int factor) : factor_{factor}
    {}

    int operator()(int a)
    {
        ++counter_;
        return a * factor_;
    }

    int counter() const
    {
        return counter_;
    }
};

class Printer
{
    int counter_ = 0;
public:
    void operator()(int a)
    {
        ++counter_;
        std::cout << "Item: " << a << "\n";
    }
    
    int counter() const
    {
        return counter_;
    }
};

namespace v2
{
    template <typename Fun2Arg>
    auto calculate(const std::vector<int>& data, Fun2Arg f) // Fun2Arg is passed by value -> is copied
    {
        puts(__PRETTY_FUNCTION__);

        auto result = data; // copy of vector

        for(auto& item : result)
            item = f(item); // function call using ptr_f

        return result;
    }
}

TEST_CASE("functors")
{
    ScaleBy scaler_by_2{2};
    REQUIRE(scale_by_2(3) == 6);

    std::vector vec = {1, 2, 3};

    auto result_vec = v2::calculate(vec, scaler_by_2);
    REQUIRE(result_vec == std::vector{2, 4, 6});

    result_vec = v2::calculate(vec, ScaleBy{3});
    REQUIRE(result_vec == std::vector{3, 6, 9});

    result_vec = v2::calculate(vec, &scale_by_2);
    REQUIRE(result_vec == std::vector{2, 4, 6});

    SECTION("std::for_each & functors")
    {
        Printer used_printer = std::for_each(std::begin(vec), std::end(vec), Printer{});
        std::cout << "Printer used " << used_printer.counter() << " times\n";
    }
}