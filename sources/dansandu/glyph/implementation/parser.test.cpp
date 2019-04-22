#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parser.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/implementation/tokenization.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

#include <map>
#include <utility>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::implementation::automaton::operator<<;
using dansandu::glyph::implementation::grammar::operator<<;
using dansandu::glyph::implementation::parsing_table::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::grammar::getFirstTable;
using dansandu::glyph::implementation::grammar::getFollowTable;
using dansandu::glyph::implementation::grammar::getRules;
using dansandu::glyph::implementation::grammar::getSymbols;
using dansandu::glyph::implementation::parser::parse;
using dansandu::glyph::implementation::parsing_table::getSimpleLeftToRightParsingTable;
using dansandu::glyph::implementation::tokenization::tokenize;
using dansandu::glyph::node::Node;
using dansandu::glyph::token::Token;

// clang-format off
TEST_CASE("Parser") {
    const auto terminals = std::vector<std::pair<std::string, std::string>>{{{"identifier", "[a-z]\\w*"},
                                                                             {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                                                             {"add", "\\+"},
                                                                             {"multiply", "\\*"},
                                                                             {"whitespace", "\\s+"}}};

    constexpr auto grammar = /*0*/"Start    -> Sums                    \n"
                             /*1*/"Sums     -> Sums add Products       \n"
                             /*2*/"Sums     -> Products                \n"
                             /*3*/"Products -> Products multiply Value \n"
                             /*4*/"Products -> Value                   \n"
                             /*5*/"Value    -> number                  \n"
                             /*6*/"Value    -> identifier";

    const auto rules = getRules(grammar);
    const auto automaton = getAutomaton(rules);
    const auto symbols = getSymbols(rules);
    const auto firstTable = getFirstTable(rules, symbols);
    const auto followTable = getFollowTable(rules, firstTable);
    const auto parsingTable = getSimpleLeftToRightParsingTable(rules, automaton, followTable, symbols.first);
    
    auto tokens = tokenize("a * b + 10", terminals, {"whitespace"});
    auto tree = parse(tokens, parsingTable, rules);

    REQUIRE(tree == Node{0, { Node{1, { Node{2, { Node{3, { Node{4, { Node{6, { Node{Token{"identifier", 0, 1}}}}}},
                                                            Node{Token{"multiply", 2, 3}},
                                                            Node{6, { Node{Token{"identifier", 4, 5}}}}}}}},
                                        Node{Token{"add", 6, 7}},
                                        Node{4, { Node{5, { Node{Token{"number", 8, 10}}}}}}}}}});
}
// clang-format on
