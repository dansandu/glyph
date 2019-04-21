#include "dansandu/glyph/arithmetic_formula_parser.hpp"

#include <iostream>
#include <string_view>

using dansandu::glyph::arithmetic_formula_parser::evaluate;

int main(int argc, char** argv) {
    auto x = 0.0;
    auto formula = argc > 1 ? argv[1] : "x";
    std::cout << evaluate(formula, {{"x", x}}) << std::endl;
    return 0;
}
