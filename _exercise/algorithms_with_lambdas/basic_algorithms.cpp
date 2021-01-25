#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

TEST_CASE("lambda exercise")
{
    using namespace Catch::Matchers;

    vector<int> data = {1, 6, 3, 5, 8, 9, 13, 12, 10, 45};

    SECTION("count even numbers")
    {
        auto evens_count = 0;

        // TODO
        
        REQUIRE(evens_count == 4);
    }

    SECTION("copy evens to vector")
    {
        vector<int> evens;
        
        // TODO

        REQUIRE_THAT(evens, Equals(vector<int>{6, 8, 12, 10}));
    }

    SECTION("create container with squares")
    {
        vector<int> squares;
        
        // TODO

        REQUIRE_THAT(squares, Equals(vector<int>{1, 36, 9, 25, 64, 81, 169, 144, 100, 2025}));
    }

    SECTION("remove from container items divisible by any number from a given array")
    {
        const array<int, 3> eliminators = {3, 5, 7};

        // TODO

        REQUIRE_THAT(data, Equals(vector<int>{1, 8, 13}));
    }

    SECTION("calculate average")
    {
        auto sum = 0.0;
        double avg;
            
        // TODO

        REQUIRE(avg == Approx(11.2));

        SECTION("create two containers - 1st with numbers less or equal to average & 2nd with numbers greater than average")
        {
            vector<int> less_equal_than_avg;
            vector<int> greater_than_avg;

            
            REQUIRE_THAT(less_equal_than_avg, Contains(vector<int>{1, 6, 3, 5, 8, 9, 10}));
            REQUIRE_THAT(greater_than_avg, Contains(vector<int>{13, 12, 45}));
        }
    }
}
