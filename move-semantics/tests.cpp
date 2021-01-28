#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"
#include "utils.hpp"

using namespace std;
using namespace Utils;

void use(Gadget g)
{
    std::cout << "Using gadget " << g.id() << " - " << g.name() << std::endl;
}

TEST_CASE("move semantics")
{
    std::vector<Gadget> gadgets;
    gadgets.reserve(100);

    Gadget g {0, "ipad0"};
    gadgets.push_back(g);
    gadgets.push_back(Gadget {1, "ipad1"});
    gadgets.push_back(2);
    gadgets.push_back(std::move(g));

    std::cout << "--------------\n";

    std::string s1 = "text";

    std::string s2 = s1;
}

std::string full_name(const std::string& first, const std::string& last)
{
    return first + " " + last;
}

TEST_CASE("reference binding")
{
    std::string first_name = "Jan";

    SECTION("C++98")
    {
        std::string& ref_name = first_name;

        const std::string& ref_full_name = full_name(first_name, "Kowalski");
        //ref_full_name[0] = 'P';
    }

    SECTION("C++11")
    {
        // all rules above

        std::string&& ref_full_name = full_name(first_name, "Kowalski");
        ref_full_name[0] = 'P';

        REQUIRE(ref_full_name == "Pan Kowalski"s);

        //std::string&& ref_first_name = first_name;
    }
}

namespace explain
{
    template <typename T>
    class vector
    {
    public:
        void push_back(const T& item) // makes copy of item to vector
        {
            std::cout << "makes copy of " << item << " to vector\n";
        }

        void push_back(T&& item) // moves item to vector
        {
            std::cout << "moves " << item << " to vector\n";
        }
    };
}

TEST_CASE("vector + push_backs")
{
    explain::vector<std::string> names;

    std::string first_name = "Jan";

    names.push_back(first_name);
    names.push_back(full_name(first_name, "Kowalski"));
    names.push_back(std::move(first_name));
}

struct Member
{
    std::string value;

    Member() = default;
};

struct Person
{
    int id;
    std::string name;
    Member member;

    Person() = default;

    Person(int id, std::string n)
        : id {id}
        , name {std::move(n)}
    {
    }
};

TEST_CASE("special functions")
{
    Person p(1, "Adam");

    Person p1 = p;
    REQUIRE(p1.id == 1);
    REQUIRE(p1.name == "Adam"s);
    REQUIRE(p.id == 1);
    REQUIRE(p.name == "Adam"s);

    Person p2 = std::move(p); // p is expiring value - x-value
    REQUIRE(p2.id == 1);
    REQUIRE(p2.name == "Adam"s);
    REQUIRE(p.id == 1);
    REQUIRE(p.name == ""s);
}

struct Data
{
    std::vector<int> data;

    Data(std::initializer_list<int> il)
        : data(il)
    {
    }

    Data(const Data&) = default;
    Data& operator=(const Data&) = default;
    Data(Data&&) = default;
    Data& operator=(Data&&) = default;

    ~Data() { data.clear(); }
};

TEST_CASE("data")
{
    Data d1 = {1, 2, 3};

    Data backup = d1; // copy
    Data target = std::move(d1); // move

    REQUIRE(d1.data.size() == 0);
}

// struct Modern
// {
//     std::vector<int> vec;
//     std::string name;
//     explain::unique_ptr<Gadget> gadget;
// };

// TEST_CASE("modern C++ classes")
// {
//     Modern m1{std::vector{1, 2, 3}, "text"s, explain::unique_ptr<Gadget>(new Gadget(1, "ipad"))};

//     Modern m2 = std::move(m1);
// }