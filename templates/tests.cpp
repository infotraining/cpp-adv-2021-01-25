#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

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

    void (*ptr_fun)(double) = &print<double>; // print<double>

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
ReversedMap<int, string> rev_map = {{"one"s, 1}, {"two"s, 2}};

/////////////////////////////////////////
// variable templates - C++14

template <typename T>
constexpr T pi(3.1415926535897932385);

TEST_CASE("variable templates & pi")
{
    float r = 8.0;

    auto area = pi<int> * r * r;
}

namespace explain
{
    template <typename T>
    class allocator
    {
    public:
        T* allocate(size_t n)
        {
            void* ptr = malloc(n * sizeof(T));
            std::cout << "Allocation T[" << n << "] at " << ptr << "\n";
            return static_cast<T*>(ptr);
        }

        void destroy(T* obj)
        {
            obj->~T();
        }

        void deallocate(T* ptr, size_t n)
        {
            std::cout << "Deallocation T[" << n << "] at " << ptr << "\n";
            free(ptr);
        }
    };

    template <typename T, typename TAllocator = std::allocator<T>>
    class vector
    {
        T* items_; // wskaźnik na tablice alokowaną na stercie
        size_t capacity_;
        size_t size_;
        TAllocator alloc {};

    public:
        using iterator = T*;
        using const_iterator = const T*;

        vector()
            : items_ {nullptr}
            , capacity_ {0}
            , size_ {0}
        {
        }

        vector(size_t size)
            : items_ {alloc.allocate(size)}
            , capacity_ {size}
            , size_ {size}
        {
            std::fill_n(items_, size_, T {});
        }

        vector(std::initializer_list<T> lst)
            : items_ {alloc.allocate(lst.size())}
            , capacity_ {lst.size()}
            , size_ {lst.size()}
        {
            std::copy(lst.begin(), lst.end(), items_);
        }

        // copy semantics
        // TODO

        // move semantics
        // TODO

        ~vector()
        {
            for (T* it = items_; it != items_ + size_; ++it)
                alloc.destroy(it); // wywołanie destruktora

            alloc.deallocate(items_, capacity_);
        }

        size_t size() const
        {
            return size_;
        }

        T& operator[](size_t index)
        {
            return items_[index];
        }

        const T& operator[](size_t index) const
        {
            return items_[index];
        }

        iterator begin()
        {
            return items_;
        }

        const_iterator begin() const
        {
            return items_;
        }
    };
}

TEST_CASE("explain vector")
{
    SECTION("empty vector")
    {
        explain::vector<int> vec0;

        REQUIRE(vec0.size() == 0);
    }

    SECTION("vector with items")
    {
        explain::vector<int> vec(10);

        REQUIRE(vec.size() == 10);

        REQUIRE(vec[5] == 0);
    }

    SECTION("const vector")
    {
        const explain::vector<int, explain::allocator<int>> vec = {1, 2, 3};

        REQUIRE(vec[2] == 3);
    }
}

namespace dynamic_polimorphism
{
    class Pickup
    {
    public:
        virtual void give_sound() = 0;
        virtual ~Pickup() = default;
    };

    class SingleCoil : public Pickup
    {
    public:
        void give_sound() override
        {
            std::cout << "Humming sound of single coil...\n";
        }
    };

    class Humbucker : public Pickup
    {
    public:
        void give_sound() override
        {
            std::cout << "Strong sound without noise...\n";
        }
    };

    class Guitar
    {
        std::unique_ptr<Pickup> bridge_pickup_;

    public:
        Guitar(std::unique_ptr<Pickup> pickup)
            : bridge_pickup_ {std::move(pickup)}
        {
        }

        void play()
        {
            bridge_pickup_->give_sound();
        }

        void reset_pickup(std::unique_ptr<Pickup> new_pickup)
        {
            bridge_pickup_ = std::move(new_pickup);
        }
    };
}

namespace static_polymorphism
{
    class SingleCoil 
    {
    public:
        void give_sound()
        {
            std::cout << "Humming sound of single coil...\n";
        }
    };

    class Humbucker 
    {
    public:
        void give_sound()
        {
            std::cout << "Strong sound without noise...\n";
        }
    };

    template <typename Pickup = SingleCoil>
    class Guitar
    {
        Pickup bridge_pickup_;

    public:
        void play()
        {
            bridge_pickup_.give_sound();
        }
    };
}

TEST_CASE("dynamic polimorphism")
{
    using namespace dynamic_polimorphism;

    Guitar fender {std::make_unique<SingleCoil>()};
    fender.play();

    fender.reset_pickup(std::make_unique<Humbucker>());
    fender.play();

    Guitar les_paul {std::make_unique<Humbucker>()};
    les_paul.play();
}

TEST_CASE("static polimorphism")
{
    using namespace static_polymorphism;

    Guitar<SingleCoil> fender;
    fender.play();

    Guitar<Humbucker> les_paul;
    les_paul.play();

    Guitar g;
    g.play();
}