#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parser.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
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
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::parser::parse;
using dansandu::glyph::implementation::parsing_table::getCanonicalLeftToRightParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::token::RegexTokenizer;
using dansandu::glyph::token::Token;

// clang-format off
TEST_CASE("Parser") {
    auto tokenizer = RegexTokenizer{{{"identifier", "[a-z]\\w*"},
                                     {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                     {"add", "\\+"},
                                     {"multiply", "\\*"},
                                     {"whitespace", "\\s+"}}, {"whitespace"}};

    auto grammar = Grammar{/*0*/"Start    -> Sums                    \n"
                           /*1*/"Sums     -> Sums add Products       \n"
                           /*2*/"Sums     -> Products                \n"
                           /*3*/"Products -> Products multiply Value \n"
                           /*4*/"Products -> Value                   \n"
                           /*5*/"Value    -> number                  \n"
                           /*6*/"Value    -> identifier"};

    auto parsingTable = getCanonicalLeftToRightParsingTable(grammar, getAutomaton(grammar));
    auto tree = parse(tokenizer("a * b + 10"), parsingTable, grammar.getRules());

    REQUIRE(tree == Node{0, { Node{1, { Node{2, { Node{3, { Node{4, { Node{6, { Node{Token{"identifier", 0, 1}}}}}},
                                                            Node{Token{"multiply", 2, 3}},
                                                            Node{6, { Node{Token{"identifier", 4, 5}}}}}}}},
                                        Node{Token{"add", 6, 7}},
                                        Node{4, { Node{5, { Node{Token{"number", 8, 10}}}}}}}}}});
}
// clang-format on
