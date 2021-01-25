#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

namespace explain
{
    template <typename InputIt, typename OutputIt>
    void copy(InputIt first, InputIt last, OutputIt target_first)
    {
        for (InputIt it = first; it != last; ++it)
        {
            *target_first++ = *it;
        }
    }
}

TEST_CASE("copy & output iterators")
{
    std::vector<int> vec = {1, 2, 3};

    std::vector<int> target;

    std::copy(vec.begin(), vec.end(), std::back_inserter(target));
}

TEST_CASE("lambda exercise")
{
    using namespace Catch::Matchers;

    vector<int> data = {1, 6, 3, 5, 8, 9, 13, 12, 10, 45};

    auto is_even = [](int value) { return value % 2 == 0; };

    SECTION("count even numbers")
    {
        auto evens_count = std::count_if(data.begin(), data.end(), is_even);

        REQUIRE(evens_count == 4);
    }

    SECTION("copy evens to vector")
    {
        vector<int> evens;

        // TODO - std::copy_if
        std::copy_if(data.begin(), data.end(), std::back_inserter(evens), is_even);

        REQUIRE_THAT(evens, Equals(vector<int> {6, 8, 12, 10}));
    }

    SECTION("create container with squares")
    {
        vector<int> squares; //jesli nie back_inserter to squares.resize(data.size)

        transform(data.begin(), data.end(), std::back_inserter(squares), [](int value) { return value * value; });

        REQUIRE_THAT(squares, Equals(vector<int> {1, 36, 9, 25, 64, 81, 169, 144, 100, 2025}));
    }

    SECTION("remove from container even items")
    {
        auto new_end = remove_if(data.begin(), data.end(), is_even);
        data.erase(new_end, data.end());

        REQUIRE(std::all_of(data.begin(), data.end(), [is_even](int n) { return !is_even(n); }));
    }

    SECTION("remove from container items divisible by any number from a given array")
    {
        const array<int, 3> eliminators = {3, 5, 7};

        auto new_end = remove_if(data.begin(), data.end(), [&eliminators](int dv) {
            return std::any_of(eliminators.begin(), eliminators.end(), [dv](int ev) { return dv % ev == 0; });
        });

        data.erase(new_end, data.end());

        REQUIRE_THAT(data, Equals(vector<int> {1, 8, 13}));
    }

    SECTION("calculate average")
    {
        double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

        REQUIRE(avg == ::Approx(11.2));

        SECTION("create two containers - 1st with numbers less or equal to average & 2nd with numbers greater than average")
        {
            vector<int> less_equal_than_avg;
            vector<int> greater_than_avg;

            std::partition_copy(data.begin(), data.end(), std::back_inserter(less_equal_than_avg), std::back_inserter(greater_than_avg), [&avg = std::as_const(avg)](int value) { return value <= avg; });

            REQUIRE_THAT(less_equal_than_avg, Contains(vector<int> {1, 6, 3, 5, 8, 9, 10}));
            REQUIRE_THAT(greater_than_avg, Contains(vector<int> {13, 12, 45}));
        }
    }
}
