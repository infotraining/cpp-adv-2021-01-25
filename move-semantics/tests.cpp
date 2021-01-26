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

    Gadget g {0, "ipad0"};
    gadgets.push_back(g);
    gadgets.push_back(Gadget {1, "ipad1"});
    gadgets.push_back(2);

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

namespace explain
{
    template <typename T>
    class unique_ptr
    {
        T* ptr_;

    public:
        explicit unique_ptr(T* ptr)
            : ptr_ {ptr}
        {
        }

        unique_ptr(nullptr_t)
            : ptr_ {nullptr}
        {
        }

        explicit operator bool() const
        {
            return ptr_ != nullptr;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        // move constructor
        unique_ptr(unique_ptr&& other)
            : ptr_ {other.ptr_}
        {
            other.ptr_ = nullptr;
        }

        unique_ptr& operator=(unique_ptr&& other) // move assignment
        {
            if (this != &other) // avoiding self-assignment
            {
                delete ptr_;

                ptr_ = other.ptr_;
                other.ptr_ = nullptr;
            }

            return *this;
        }

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        ~unique_ptr()
        {
            if (ptr_)
                delete ptr_;
        }
    };
}

TEST_CASE("unique_ptr & move semantics")
{
    explain::unique_ptr<Gadget> up1 {new Gadget(1, "ipad")};

    REQUIRE((*up1).id() == 1);
    REQUIRE(up1->name() == "ipad"s);

    explain::unique_ptr<Gadget> up2 = std::move(up1); // move constructor

    explain::unique_ptr<Gadget> up3(new Gadget(2, "ipod"));

    up3 = std::move(up2);

    REQUIRE(up3->id() == 1);

    if (up3)
    {
        std::cout << up3->name() << "\n";
    }

} // destructor Gadget