#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"
#include "utils.hpp"

using namespace std;
using namespace Utils;

TEST_CASE("test")
{
    std::vector<Gadget> vg;

    vg.push_back(Gadget{1, "ipad1"});
    vg.push_back(Gadget{2, "ipad2"});
    vg.push_back(Gadget{3, "ipad3"});
}