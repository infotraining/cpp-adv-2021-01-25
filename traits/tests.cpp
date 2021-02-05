#include "catch.hpp"
#include <array>
#include <iostream>
#include <list>
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
template <typename T>
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
template <typename TContainer>
using ValueType_t = typename ValueType<TContainer>::type;

template <typename TContainer>
void some_algorithm(const TContainer& container)
{
    //using ElementType = typename ValueType<TContainer>::type; // lepiej zastosowac alias
    using ElementType = ValueType_t<TContainer>;

    ElementType value {};

    std::cout << value << "\n";
}

TEST_CASE("functions vs. meta-functions")
{
    REQUIRE(foo(2) == 4);

    REQUIRE(Foo<2>::value == 4);

    REQUIRE(TypeSize<int>::value == 4);

    std::vector<int> vec;
    some_algorithm(vec);

    int tab[10];
    some_algorithm(tab);
}

//////////////////////////////////////////////
// traits in standard library

struct Gibson
{
    void play()
    {
        std::cout << "Classic gibson sound - humbuckers...\n";
    }
};

struct Start
{
    void play()
    {
        std::cout << "Classic fender sound - single coil hum...\n";
    }
};

struct Telecaster
{
    void play()
    {
        std::cout << "Classic twang...\n";
    }
};

namespace explain
{
    template <typename T_, T_ value_>
    struct integral_constant
    {
        static constexpr T_ value = value_;
    };

    // alias bool_constant
    template <bool value_>
    using bool_constant = integral_constant<bool, value_>;

    // alias for true
    using true_type = bool_constant<true>; // true_type::value == true
    using false_type = bool_constant<false>; // false_type::value == false

    // is_fender - generic implementation - assumes false
    template <typename TGuitar>
    struct is_fender : false_type
    {
        //static constexpr bool value = false; // is inherited from false_type
    };

    // is_fender - specialization for Strat only
    template <>
    struct is_fender<Start> : true_type
    {
        //static constexpr bool value = true; // is inherited from true_type
    };

    // is_fender - specialization for Telecaster only
    template <>
    struct is_fender<Telecaster> : true_type
    {
        //static constexpr bool value = true; // is inherited from true_type
    };

    // template variable _v
    template <typename TGuitar>
    constexpr bool is_fender_v = is_fender<TGuitar>::value;
}

template <typename TGuitar>
void guitar_player(TGuitar guitar)
{
    static_assert(explain::is_fender<TGuitar>::value);
    static_assert(explain::is_fender_v<TGuitar>); // prosciej niz linia wyzej

    guitar.play();
}

TEST_CASE("using traits")
{
    static_assert(explain::integral_constant<int, 2>::value == 2);
    static_assert(explain::integral_constant<int, 3>::value == 3);

    //static_assert(std::is_same<explain::integral_constant<int, 2>, explain::integral_constant<int, 2>>::value);

    static_assert(explain::bool_constant<true>::value == true);
    static_assert(std::bool_constant<false>::value == false);

    Start strat;
    guitar_player(strat);

    Telecaster telecaster;
    guitar_player(telecaster);

    // Gibson les_paul;
    // guitar_player(les_paul);
}

template <typename T>
void print(T value)
{
    if constexpr (std::is_pointer_v<T>)
    {
        std::cout << *value << "\n";
    }
    else
    {
        std::cout << value << "\n";
    }
}

TEST_CASE("is_pointer")
{
    int x = 42;
    print(x);
    print(&x);
}

namespace ver_2_0
{
    struct Fender
    {
        virtual void play() = 0;
        virtual ~Fender() = default;
    };

    struct Strat : Fender
    {
        void play() override
        {
            std::cout << "Classic fender sound - single coil hum...\n";
        }
    };

    struct Tele : Fender
    {
        void play() override
        {
            std::cout << "Classic twang...\n";
        }
    };

    template <typename TGuitar>
    void guitar_player(TGuitar guitar)
    {
        static_assert(std::is_base_of_v<Fender, TGuitar>);

        guitar.play();
    }
}

TEST_CASE("ver_2_0")
{
    ver_2_0::Strat start;
    ver_2_0::guitar_player(start);

    ver_2_0::Tele tele;
    ver_2_0::guitar_player(tele);

    //ver_2_0::guitar_player(Gibson{});
}

namespace explain
{
    // meta-funkcja zwracajaca typ
    template <typename T_>
    struct remove_reference
    {
        using type = T_; // konwencja aliasu type
    };

    // specjalizacja dla typow l-value ref -> usuwa &
    template <typename T_>
    struct remove_reference<T_&>
    {
        using type = T_;
    };

    // specjalizacja dla typow r-value ref -> usuwa &&
    template <typename T_>
    struct remove_reference<T_&&>
    {
        using type = T_;
    };

    // alias _t
    template <typename T_>
    using remove_reference_t = typename remove_reference<T_>::type;
}

TEST_CASE("traits - type transformation")
{
    static_assert(std::is_same_v<explain::remove_reference_t<int&>, int>);

    auto get_head = [](const auto& container) {
        using ContainerT = explain::remove_reference_t<decltype(container)>;
        using ValueT = typename ContainerT::value_type;
        std::array<ValueT, 3> head;

        assert(std::size(container) >= 3);

        std::copy_n(std::begin(container), 3, std::begin(head));

        return head;
    };

    std::vector vec = {1, 2, 3, 4, 5};

    auto head = get_head(vec);

    REQUIRE(head == std::array {1, 2, 3});
}

namespace explain
{
    template <class T>
    constexpr remove_reference_t<T>&& move(T&& arg) noexcept
    { // forward a as movable
        return static_cast<remove_reference_t<T>&&>(arg);
    }
}

TEST_CASE("remove_ref & std::move")
{
    std::string str = "text";

    std::string target = explain::move(str);
}

namespace explain
{
    template <bool Condition, typename T = void>
    struct enable_if
    {
        using type = T;
    };

    template <typename T>
    struct enable_if<false, T>
    {};

    template <bool Condition, typename T = void>
    using enable_if_t = typename enable_if<Condition, T>::type;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void calculate(T x)
{
    puts(__PRETTY_FUNCTION__);
    std::cout << "calculate(int: " << x << ")\n";
}

template <typename T, typename = void, typename = std::enable_if_t<std::is_floating_point_v<T>>>
void calculate(T x)
{
    std::cout << "calculate(double: " << x << ")\n";
}

TEST_CASE("enable_if")
{
    calculate(42);

    short s = 3;
    calculate(s);

    calculate(3.14);

    float f = 3.14;
    calculate(f);
}
