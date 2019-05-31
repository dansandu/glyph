#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"

#include <map>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::implementation::automaton::operator<<;
using dansandu::glyph::implementation::grammar::operator<<;
using dansandu::glyph::implementation::parsing_table::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::parsing_table::Action;
using dansandu::glyph::implementation::parsing_table::Cell;
using dansandu::glyph::implementation::parsing_table::getCanonicalLeftToRightParsingTable;

// clang-format off
TEST_CASE("Parsing table") {
    auto grammar = Grammar{"Start    -> Sums                     \n"
                           "Sums     -> Sums add Products        \n"
                           "Sums     -> Products                 \n"
                           "Products -> Products multiply number \n"
                           "Products -> number"};

    const auto parsingTable = getCanonicalLeftToRightParsingTable(grammar, getAutomaton(grammar));
    
    constexpr auto reduce = Action::reduce,
                   accept = Action::accept,
                   shift = Action::shift,
                   goTo = Action::goTo;
    
    REQUIRE(parsingTable.table == std::map<std::string, std::vector<Cell>>{{
        {"add",        {         {}, {reduce, 2}, {shift,  5}, {reduce, 4},          {},          {}, {reduce, 3}, {reduce, 1}}},
        {"multiply",   {         {}, {shift,  4},          {}, {reduce, 4},          {},          {}, {reduce, 3}, {shift,  4}}},
        {"number",     {{shift,  3},          {},          {},          {}, {shift,  6}, {shift,  3},          {},          {}}},
        {endOfString,  {         {}, {reduce, 2}, {accept, 0}, {reduce, 4},          {},          {}, {reduce, 3}, {reduce, 1}}},
        {"Products",   {{goTo,   1},          {},          {},          {},          {}, {goTo,   7},          {},          {}}},
        {"Sums",       {{goTo,   2},          {},          {},          {},          {},          {},          {},          {}}},
    }});

    REQUIRE(parsingTable.startRuleIndex == 0);
}
// clang-format on
