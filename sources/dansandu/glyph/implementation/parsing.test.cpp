#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parsing.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/glyph/token.hpp"

#include <regex>
#include <vector>

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::parsing::parse;
using dansandu::glyph::implementation::parsing_table::getCanonicalLeftToRightParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::regex_tokenizer::RegexTokenizer;
using dansandu::glyph::token::Token;

// clang-format off
TEST_CASE("Parsing")
{
    auto grammar = Grammar{/*0*/"Start    -> Sums                    \n"
                           /*1*/"Sums     -> Sums add Products       \n"
                           /*2*/"Sums     -> Products                \n"
                           /*3*/"Products -> Products multiply Value \n"
                           /*4*/"Products -> Value                   \n"
                           /*5*/"Value    -> number                  \n"
                           /*6*/"Value    -> identifier"};

    auto add        = grammar.getSymbol("add");
    auto multiply   = grammar.getSymbol("multiply");
    auto number     = grammar.getSymbol("number");
    auto identifier = grammar.getSymbol("identifier");
    auto whitespace = grammar.getEmptySymbol();
    
    auto tokenizer = RegexTokenizer{{{identifier, std::regex{"[a-z]\\w*"}},
                                     {number,     std::regex{"(?:[1-9]\\d*|0)(?:\\.\\d+)?"}},
                                     {add,        std::regex{"\\+"}},
                                     {multiply,   std::regex{"\\*"}},
                                     {whitespace, std::regex{"\\s+"}}},
                                    {whitespace}};

    auto parsingTable = getCanonicalLeftToRightParsingTable(grammar, getAutomaton(grammar));
    auto tree = parse(tokenizer("a * b + 10"), parsingTable, grammar);

    auto expectedTree = std::vector<Node>{
        Node{Token{identifier, 0, 1}},
        Node{6},
        Node{4},
        Node{Token{multiply, 2, 3}},
        Node{Token{identifier, 4, 5}},
        Node{6},
        Node{3},
        Node{2},
        Node{Token{add, 6, 7}},
        Node{Token{number, 8, 10}},
        Node{5},
        Node{4},
        Node{1},
        Node{0}
    };

    REQUIRE(tree == expectedTree);
}
// clang-format on
