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
using dansandu::glyph::implementation::automaton::Item;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::getFirstTable;
using dansandu::glyph::implementation::grammar::getFollowTable;
using dansandu::glyph::implementation::grammar::getRules;
using dansandu::glyph::implementation::grammar::getSymbols;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::parsing_table::Action;
using dansandu::glyph::implementation::parsing_table::Cell;
using dansandu::glyph::implementation::parsing_table::getReductionRuleIndices;
using dansandu::glyph::implementation::parsing_table::getSimpleLeftToRightParsingTable;

// clang-format off
TEST_CASE("Parsing table") {
    constexpr auto grammar = "Start    -> Sums                    \n"
                             "Sums     -> Sums add Products       \n"
                             "Sums     -> Products                \n"
                             "Products -> Products multiply Value \n"
                             "Products -> Value                   \n"
                             "Value    -> number                  \n"
                             "Value    -> identifier";
    constexpr auto startRuleIndex = 0;
    const auto rules = getRules(grammar);

    SECTION("reductions") {
        REQUIRE(getReductionRuleIndices({Item{0, 0}, Item{1, 0}, Item{2, 0}}, rules, startRuleIndex).empty());

        REQUIRE(getReductionRuleIndices({Item{0, 1}, Item{1, 1}}, rules, startRuleIndex).empty());

        REQUIRE(getReductionRuleIndices({Item{2, 1}, Item{3, 1}}, rules, startRuleIndex) == std::vector<int>{2});

        REQUIRE(getReductionRuleIndices({Item{1, 3}, Item{2, 0}, Item{3, 3}}, rules, startRuleIndex) == std::vector<int>{{1, 3}});
    }

    SECTION("parsing table") {
        constexpr auto reduce = Action::reduce,
                       accept = Action::accept,
                        shift = Action::shift,
                         goTo = Action::goTo;
        const auto automaton = getAutomaton(rules);
        const auto symbols = getSymbols(rules);
        const auto firstTable = getFirstTable(rules, symbols);
        const auto followTable = getFollowTable(rules, firstTable);
        const auto parsingTable = getSimpleLeftToRightParsingTable(rules, automaton, followTable, symbols.first);
        
        REQUIRE(parsingTable.table == std::map<std::string, std::vector<Cell>>{{
            {"add",        {         {}, {reduce, 2}, {shift,  7}, {reduce, 4}, {reduce, 6}, {reduce, 5},          {},          {}, {reduce, 3}, {reduce, 1}}},
            {"multiply",   {         {}, {shift,  6},          {}, {reduce, 4}, {reduce, 6}, {reduce, 5},          {},          {}, {reduce, 3}, {shift,  6}}},
            {"number",     {{shift,  5},          {},          {},          {},          {},          {}, {shift,  5}, {shift,  5},          {},          {}}},
            {"identifier", {{shift,  4},          {},          {},          {},          {},          {}, {shift,  4}, {shift,  4},          {},          {}}},
            {endOfString,  {         {}, {reduce, 2}, {accept, 0}, {reduce, 4}, {reduce, 6}, {reduce, 5},          {},          {}, {reduce, 3}, {reduce, 1}}},
            {"Products",   {{goTo  , 1},          {},          {},          {},          {},          {},          {}, {goTo,   9},          {},          {}}},
            {"Sums",       {{goTo  , 2},          {},          {},          {},          {},          {},          {},          {},          {},          {}}},
            {"Value",      {{goTo  , 3},          {},          {},          {},          {},          {}, {goTo,   8}, {goTo,   3},          {},          {}}}
        }});

        REQUIRE(parsingTable.startRuleIndex == automaton.startRuleIndex);
    }
}
// clang-format on
