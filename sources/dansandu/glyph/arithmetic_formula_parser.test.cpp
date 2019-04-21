#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/arithmetic_formula_parser.hpp"

using Catch::Detail::Approx;
using dansandu::glyph::arithmetic_formula_parser::evaluate;

TEST_CASE("Arithmetic formula parser") {

    REQUIRE(evaluate("amplitude * sin(t^0.5) + 10", {{"amplitude", 10.0}, {"t", 0.5}}) == Approx(16.4963));
}