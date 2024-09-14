#include "dansandu/glyph/internal/parsing_table.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/internal/grammar.hpp"

#include <vector>

using dansandu::glyph::internal::automaton::getAutomaton;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::parsing_table::Action;
using dansandu::glyph::internal::parsing_table::Cell;
using dansandu::glyph::internal::parsing_table::getClr1ParsingTable;

// clang-format off
TEST_CASE("Parsing table") {
    const auto grammar = Grammar{R"(
        Start    -> Sums
        Sums     -> Sums add Products
        Sums     -> Products
        Products -> Products multiply number
        Products -> number
    )"};

    const auto table = getClr1ParsingTable(grammar, getAutomaton(grammar));
    
    const auto reduce = Action::reduce,
               accept = Action::accept,
               shift = Action::shift,
               goTo = Action::goTo;
    
    REQUIRE(table == std::vector<std::vector<Cell>>{
        {         {},          {},          {},          {},          {},          {},          {},          {}},
        {{goTo,   1},          {},          {},          {},          {},          {},          {},          {}},
        {{goTo,   2},          {},          {},          {}, {goTo,   6},          {},          {},          {}},
        {         {}, {accept, 0}, {reduce, 2}, {reduce, 4},          {},          {}, {reduce, 1}, {reduce, 3}},
        {         {},          {},          {},          {},          {},          {},          {},          {}},
        {         {}, {shift,  4}, {reduce, 2}, {reduce, 4},          {},          {}, {reduce, 1}, {reduce, 3}},
        {         {},          {}, {shift,  5}, {reduce, 4},          {},          {}, {shift,  5}, {reduce, 3}},
        {{shift,  3},          {},          {},          {}, {shift,  3}, {shift,  7},          {},          {}}
    });
}
// clang-format on
