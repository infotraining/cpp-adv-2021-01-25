#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

int add(int a, int b)
{
    return a + b;
}

using PtrFun2Arg = int (*)(int, int);
//typedef int (*PtrFun2Arg)(int, int);

int scale_by_2(int x)
{
    static int call_counter = 0;
    ++call_counter;
    std::cout << "call_counter: " << call_counter << std::endl;

    return x * 2;
}

using PtrFun1Arg = int (*)(int);

namespace v1
{
    auto calculate(const std::vector<int>& data, PtrFun1Arg ptr_f)
    {
        auto result = data; // copy of vector

        for (auto& item : result)
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
    REQUIRE(result_vec == std::vector {2, 4, 6});
}

/////////////////////////////////////////////
// Functors (function objects)

class ScaleBy
{
    int counter_ = 0;
    int factor_;

public:
    ScaleBy(int factor)
        : factor_ {factor}
    {
    }

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

    void operator()(const std::string& text)
    {
        ++counter_;
        std::cout << "String: " << text << "\n";
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

        for (auto& item : result)
            item = f(item); // function call using ptr_f

        return result;
    }
}

TEST_CASE("functors")
{
    ScaleBy scaler_by_2 {2};
    REQUIRE(scale_by_2(3) == 6);

    std::vector vec = {1, 2, 3};

    auto result_vec = v2::calculate(vec, scaler_by_2);
    REQUIRE(result_vec == std::vector {2, 4, 6});

    result_vec = v2::calculate(vec, ScaleBy {3});
    REQUIRE(result_vec == std::vector {3, 6, 9});

    result_vec = v2::calculate(vec, &scale_by_2);
    REQUIRE(result_vec == std::vector {2, 4, 6});

    SECTION("std::for_each & functors")
    {
        Printer used_printer = std::for_each(std::begin(vec), std::end(vec), Printer {});
        std::cout << "Printer used " << used_printer.counter() << " times\n";
    }
}

namespace explain
{
    template <typename Iter, typename UnPred>
    Iter find_if(Iter first, Iter last, UnPred pred)
    {
        for (Iter it = first; it != last; ++it)
        {
            if (pred(*it))
                return it;
        }

        return last;
    }
}

class GreaterThan
{
    double avg;

public:
    GreaterThan(double a)
        : avg {a}
    {
    }

    bool operator()(double value) const
    {
        return value > avg;
    }
};

TEST_CASE("exercise")
{
    std::vector vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    double avg = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
    std::cout << "avg: " << avg << "\n";

    GreaterThan gt_than_avg {avg};
    REQUIRE(gt_than_avg(11));
    REQUIRE(gt_than_avg(1) == false);

    auto pos_gt_than_avg = std::find_if(vec.begin(), vec.end(), GreaterThan {avg});
    std::cout << "Found: " << *pos_gt_than_avg << "\n";
}

TEST_CASE("std functors")
{
    std::greater<int> gt_int;

    REQUIRE(gt_int(1, 2) == false);
    REQUIRE(gt_int(2, 1));

    std::vector vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::sort(vec.begin(), vec.end(), std::greater<int> {});
    std::for_each(vec.begin(), vec.end(), Printer {});
}

////////////////////////////////////////////////////////
// lambdy

class Lambda_342763547623547
{
public:
    auto operator()(int a) const
    {
        std::cout << "Print from lambda: " << a << "\n";
    }
};

namespace c_style
{
    int get_current_temp()
    {
        return 13;
    }

    void callback(void (*call_me)(int))
    {
        int temp = get_current_temp();
        call_me(temp);
    }
}

TEST_CASE("the simplest lambda")
{
    auto printer = [](int a) { std::cout << "Print from lambda: " << a << "\n"; };

    printer(1);
    printer(2);

    SECTION("is interpreted as")
    {
        auto printer = Lambda_342763547623547 {};
    }

    std::vector vec = {112, 212, 312};
    vec = v2::calculate(vec, [](int a) { return a * 2; });
    std::for_each(vec.begin(), vec.end(), printer);

    std::vector<int*> vec_ptrs = {new int(12), new int(665), new int(1)};

    std::sort(vec_ptrs.begin(), vec_ptrs.end(), [](int* pa, int* pb) { return *pa < *pb; });

    std::for_each(vec_ptrs.begin(), vec_ptrs.end(), [](int* ptr) { std::cout << (*ptr) << " "; });
    std::cout << "\n";

    auto log = [](int value) { std::cout << "logging: " << value << "\n"; };
    c_style::callback(log);
}

class Lambda_87364273548276345
{
    const double avg_;

public:
    Lambda_87364273548276345(double avg)
        : avg_ {avg}
    {
    }
    auto operator()(int x) const { return x > avg_; }
};

TEST_CASE("captures")
{
    std::vector vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    double avg = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
    std::cout << "avg: " << avg << "\n";

    SECTION("capture by value")
    {
        auto pos_gt_than_avg = std::find_if(vec.begin(), vec.end(), [avg](int x) { return x > avg; });
        REQUIRE(*pos_gt_than_avg == 6);
    }

    SECTION("capture by reference")
    {
        int sum = 0;
        std::for_each(vec.begin(), vec.end(), [&sum](int x) { sum += x; });

        REQUIRE(sum == 55);
    }

    SECTION("capture by value & reference")
    {
        int sum = 0;
        int factor = 2;
        std::for_each(vec.begin(), vec.end(), [&sum, factor](int x) { sum += factor * x; });

        REQUIRE(sum == 110);
    }
}

TEST_CASE("capture using = and &")
{
    int val1 = 1;
    int val2 = 2;

    auto l1 = [=] { return val1 * val2; };
    auto l2 = [&] { ++val1; ++val2; };
    auto l3 = [=, &val2] { ++val2; return val1 + 1; };
    auto l4 = [&, val2] { ++val1; return val2 + 1; };
}

auto create_generator(int start)
{
    return [start]() mutable { return start++; };
}

TEST_CASE("generator")
{
    auto gen = create_generator(100);
    REQUIRE(gen() == 100);
    REQUIRE(gen() == 101);
    REQUIRE(gen() == 102);

    std::vector<int> vec(10);
    std::generate_n(vec.begin(), 10, create_generator(13));
    std::for_each_n(vec.begin(), 5, [](int n) { std::cout << n << " "; });
    std::cout << "\n";
}

class Data
{
    const std::vector<int> data_;
    double avg_;

public:
    Data(std::vector<int> data)
        : data_ {std::move(data)}
    {
        avg_ = std::accumulate(data_.begin(), data_.end(), 0.0) / data_.size();
    }

    void print_gt_than_avg()
    {
        std::for_each(data_.begin(), data_.end(), [this](int x) { if (x > avg_) std::cout << x << " "; });
        std::cout << "\n";
    }
};

TEST_CASE("capturing this")
{
    Data data {{1, 2, 3, 5, 6, 7, 8, 9, 10}};
    data.print_gt_than_avg();
}

TEST_CASE("lambda expressions with ->")
{
    auto describe = [](int n) -> std::string { // -> only for C++11 compilers
        if (n % 2 == 2)
            return "even";
        else
            return "odd";
    };
}

class GenericPrinter
{
    int counter_ = 0;

public:
    template <typename T>
    void operator()(const T& item)
    {
        ++counter_;
        std::cout << "Item: " << item << "\n";
    }

    int counter() const
    {
        return counter_;
    }
};

class Lambda_928364782356235
{
public:
    template <typename T>
    void operator()(const T& item) const
    {
        std::cout << "Item: " << item << "\n";
    }
};

TEST_CASE("generic lambda expressions - C++14")
{
    auto printer = [](const auto& item) {
        std::cout << "Item: " << item << "\n";
    };

    std::vector numbers = {1, 2, 3};
    std::for_each(numbers.begin(), numbers.end(), printer);

    std::vector words = {"one"s, "two"s, "three"s};
    std::for_each(words.begin(), words.end(), printer);
}

class Gadget
{
    std::string name_;

public:
    Gadget(std::string name)
        : name_ {std::move(name)}
    {
        std::cout << "Gadget(" << name_ << ")\n";
    }

    ~Gadget()
    {
        std::cout << "~Gadget(" << name_ << ")\n";
    }

    void use() const
    {
        std::cout << "using Gadget(" << name_ << ")\n";
    }
};

TEST_CASE("init expressions in capture clause")
{
    int x = 10;

    auto l1 = [lx = x] { return lx; }; // capture by value makes copy when closure is created

    x = 11;

    REQUIRE(l1() == 10);

    SECTION("lambda + move semantics")
    {
        std::unique_ptr<Gadget> ptr_gadget = std::make_unique<Gadget>("ipad");
        std::unique_ptr<Gadget> other_ptr = std::move(ptr_gadget);
        REQUIRE(ptr_gadget.get() == nullptr);

        auto use_gadget = [ptr = std::move(other_ptr)] { ptr->use(); };
        REQUIRE(other_ptr.get() == nullptr);
    }
}

TEST_CASE("std algorithms + lambda expressions")
{
    std::vector<std::string> words = {"zero", "sixty_six", "one", "two", "three", "four"};

    auto compare_by_length = [](const auto& a, const auto& b) { return a.size() > b.size(); };

    std::sort(words.begin(), words.end(), compare_by_length);
}