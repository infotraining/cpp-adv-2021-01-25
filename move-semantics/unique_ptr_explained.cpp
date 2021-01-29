#include "catch.hpp"
#include "utils.hpp"

using namespace Utils;
using namespace std::literals;

namespace explain
{
    template <typename T>
    class unique_ptr
    {
        T* ptr_ = nullptr;

    public:
        unique_ptr() = default;

        explicit unique_ptr(T* ptr)
            : ptr_ {ptr}
        {
        }

        unique_ptr(nullptr_t)
            : ptr_ {nullptr}
        {
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

        ~unique_ptr()
        {
            if (ptr_)
                delete ptr_;
        }

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        explicit operator bool() const
        {
            return ptr_ != nullptr;
        }
    };

    // template <typename T>
    // explain::unique_ptr<T> make_unique()
    // {
    //     return explain::unique_ptr<T>(new T());
    // }

    // template <typename T, typename TArg1>
    // explain::unique_ptr<T> make_unique(TArg1&& arg1)
    // {
    //     return explain::unique_ptr<T>(new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)));
    // }

    // template <typename T, typename TArg1, typename TArg2>
    // explain::unique_ptr<T> make_unique(TArg1&& arg1, TArg2&& arg2)
    // {
    //     return explain::unique_ptr<T>(new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)));
    // }

    // implementation with variadic templates
    template <typename T, typename... TArgs>
    explain::unique_ptr<T> make_unique(TArgs&&... args)
    {
        return explain::unique_ptr<T>(new 
        T(std::forward<TArgs>(args)...));
    }

    template <typename T>
    class unique_ptr<T[]>
    {
        T* ptr_ = nullptr;

    public:
        unique_ptr() = default;

        explicit unique_ptr(T* ptr)
            : ptr_ {ptr}
        {
        }

        unique_ptr(nullptr_t)
            : ptr_ {nullptr}
        {
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

        ~unique_ptr()
        {
            if (ptr_)
                delete[] ptr_;
        }

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        T& operator[](size_t index) const
        {
            return ptr_[index];
        }

        explicit operator bool() const
        {
            return ptr_ != nullptr;
        }
    };
}

TEST_CASE("unique_ptr & move semantics")
{
    explain::unique_ptr<Gadget> up0;

    //explain::unique_ptr<Gadget> up1 {new Gadget(1, "ipad")};
    explain::unique_ptr<Gadget> up1 = explain::make_unique<Gadget>(1, "ipad");

    REQUIRE((*up1).id() == 1);
    REQUIRE(up1->name() == "ipad"s);

    explain::unique_ptr<Gadget> up2 = std::move(up1); // move constructor

    explain::unique_ptr<Gadget> up3(explain::make_unique<Gadget>(2, "ipod"));

    up3 = std::move(up2);

    REQUIRE(up3->id() == 1);

    if (up3)
    {
        std::cout << up3->name() << "\n";
    }

    std::vector<explain::unique_ptr<Gadget>> gadgets;

    gadgets.push_back(std::move(up3));
    gadgets.push_back(explain::make_unique<Gadget>(3, "reverb effect"));   
    gadgets.push_back(explain::make_unique<Gadget>(5, "overdrive effect"));
    gadgets.push_back(explain::make_unique<Gadget>(42));
} // destructor Gadget

TEST_CASE("vector & perfect forwarding")
{
    std::cout << "*****************\n";

    std::vector<Gadget> gadgets;
    gadgets.reserve(100);

    Gadget g1{1, "chorus"};
    gadgets.push_back(g1);

    gadgets.push_back(Gadget{2, "hall reverb"}); // move constructor will be called

    gadgets.emplace_back(3, "flanger");

    std::cout << "*****************\n";
    std::cout << "*****************\n";
}

TEST_CASE("auto vs. decltype")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;

    auto a1 = x; // int
    auto a2 = cx; // int
    auto a3 = ref_x; // int
    auto a4 = cref_x; // int

    decltype(x) d1 = x;
    decltype(cx) d2 = cx;
    decltype(ref_x) d3 = x;
    decltype(cref_x) d4 = cx;
}

decltype(auto) get_item(std::vector<int>& data, size_t index)
{
    return data[index]; // jeśli auto to zadziała mechanizm dedukcji auto int& -> int 
}

TEST_CASE("auto vs. decltype(auto)")
{
    std::vector vec{1, 2, 3};

    REQUIRE(get_item(vec, 1) == 2);

    get_item(vec, 1) = 42;

    REQUIRE(vec[1] == 42);
}

TEST_CASE("partial specialization for unique_ptr for arrays")
{
    int* dynamic_tab = new int[1024];
    delete[] dynamic_tab;

    explain::unique_ptr<int[]> tab(new int[1024]);

    tab[1] = 13;

} // delete[] tab.ptr_