#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <map>

#include "catch.hpp"

//https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

using namespace std;
using namespace Utils;

void use_gadget(Gadget* g);

void use_gadget(Gadget* g)
{
    if (g)
        std::cout << "Using " << g->name() << "\n";
}

std::unique_ptr<Gadget> use(std::unique_ptr<Gadget> g);

std::unique_ptr<Gadget> use(std::unique_ptr<Gadget> g)
{
    if (g)
        std::cout << "Using " << g->name() << "\n";

    return g;
} // g is destroyed -> deletes gadget

namespace legacy_code
{
    Gadget* effect_factory(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
        // static Gadget uber_pedal(1, "Fractal");
        // return &uber_pedal;
    }

    void use_and_destroy(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }
}

namespace modern_cpp
{
    std::unique_ptr<Gadget> effect_factory(const std::string& name)
    {
        static int id = 665;
        return std::make_unique<Gadget>(++id, name);
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
}

TEST_CASE("smart pointers - unique_ptr")
{
    std::unique_ptr<Gadget> reverb = std::make_unique<Gadget>(1, "reverb");

    REQUIRE(reverb.get() != nullptr);

    use(std::make_unique<Gadget>(2, "noleak"));

    auto used_reverb = use(std::move(reverb));

    std::cout << "1-------------\n";

    used_reverb.reset(); // deletes reverb

    std::cout << "2-------------\n";

    legacy_code::use_and_destroy(legacy_code::effect_factory("delay"));

    std::unique_ptr<Gadget> fractal = modern_cpp::effect_factory("fractal");

    //modern_cpp::use(std::move(fractal));

    legacy_code::use_and_destroy(fractal.release());
}

TEST_CASE("shared_ptrs")
{
    std::map<std::string, std::shared_ptr<Gadget>> effect_chain;

    std::weak_ptr<Gadget> weak_overdrive_effect;

    {
        //std::shared_ptr<Gadget> o(new Gadget(1, "boss overdrive"));

        auto overdrive = std::make_shared<Gadget>(1, "boss overdrive"); // RC: 1

        auto another_sp = overdrive; // copy of shared_ptr -> RC: 2

        weak_overdrive_effect = overdrive; // asignment to weak_ptr: RC: 2

        REQUIRE(overdrive.use_count() == 2);

        effect_chain.emplace("1st", overdrive); // copy of shared_ptr to map -> RC: 3

        REQUIRE(overdrive.use_count() == 3);
    } // overdrive & another_sp are destroyed decrementing ref counter -> RC: 1

    REQUIRE(effect_chain["1st"].use_count() == 1);
    REQUIRE(effect_chain["1st"]->name() == "boss overdrive");

    shared_ptr<Gadget> temp_effect = weak_overdrive_effect.lock(); // lock() on existing object (when RC > 0) -> RC: 2
    if (temp_effect)
        std::cout << "Using effect: " <<  temp_effect->name() << "\n";
    
    temp_effect.reset(); // RC: 1
    effect_chain.clear(); // RC: 0

    temp_effect = weak_overdrive_effect.lock(); // lock() on destroyed object returns empty shared_ptr
    REQUIRE(temp_effect == nullptr);

    std::cout << "--------------\n";
}

//////////////////////////////////////////////
// custom deallocators

FILE* open_file(const char* filename)
{
    FILE* f = fopen(filename, "w");
    std::cout << "Opening a file: " << filename << " - " << reinterpret_cast<void*>(f) << "\n";
    return f;
}

void close_file(FILE* file)
{
    std::cout << "Closing a file: " << reinterpret_cast<void*>(file) << "\n";
    fclose(file);
}

TEST_CASE("custom dealloc")
{
    {
        std::unique_ptr<Gadget> g(new Gadget(1, "reverb"));

        //                    typ dealokatora - wsk na funkcje                 adres funkcji dealokujacej
        std::unique_ptr<FILE, void(*)(FILE*)> safe_file(open_file("test.dat"), &close_file); // 

        auto custom_deallocator = [](FILE* f) { close_file(f); };

        //                    deduction of closure type 
        std::unique_ptr<FILE, decltype(custom_deallocator)> another_file(open_file("text.dat"), custom_deallocator);
    }
}