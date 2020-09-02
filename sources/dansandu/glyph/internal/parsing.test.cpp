#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/parsing.hpp"
#include "dansandu/glyph/internal/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/glyph/token.hpp"

#include <vector>

using dansandu::glyph::error::SyntaxError;
using dansandu::glyph::internal::automaton::getAutomaton;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::parsing::parse;
using dansandu::glyph::internal::parsing_table::getClr1ParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::regex_tokenizer::RegexTokenizer;
using dansandu::glyph::token::Token;

// clang-format off
TEST_CASE("Parsing")
{
    const auto doNothing = [](const auto&) { };

    SECTION("grammar #1")
    {
        const auto grammar = Grammar{/*0*/ "Start       -> Sums                          \n"
                                     /*1*/ "Sums        -> Sums plus Products            \n"
                                     /*2*/ "Sums        -> Products                      \n"
                                     /*3*/ "Products    -> Products multiply SignedValue \n"
                                     /*4*/ "Products    -> SignedValue                   \n"
                                     /*5*/ "SignedValue -> Value                         \n"
                                     /*6*/ "SignedValue -> plus Value                    \n"
                                     /*7*/ "Value       -> number                        \n"
                                     /*8*/ "Value       -> identifier"};

        const auto identifier = grammar.getSymbol("identifier");
        const auto plus       = grammar.getSymbol("plus");
        const auto multiply   = grammar.getSymbol("multiply");
        const auto number     = grammar.getSymbol("number");
        const auto discarded  = grammar.getDiscardedSymbolPlaceholder();

        const auto tokenizer = RegexTokenizer{{{identifier, "[a-z]\\w*"},
                                               {number,     "([1-9]\\d*|0)(\\.\\d+)?"},
                                               {plus,       "\\+"},
                                               {multiply,   "\\*"},
                                               {discarded,  "\\s+"}}};

        const auto parsingTable = getClr1ParsingTable(grammar, getAutomaton(grammar));

        SECTION("successful parse")
        {
            auto actualTree = std::vector<Node>{};

            const auto visitor = [&actualTree](const Node& node) { actualTree.push_back(node); };

            parse(tokenizer("a * b + 10"), parsingTable, grammar, visitor);

            const auto expectedTree = std::vector<Node>{
                Node{Token{identifier, 0, 1}},
                Node{8},
                Node{5},
                Node{4},
                Node{Token{multiply, 2, 3}},
                Node{Token{identifier, 4, 5}},
                Node{8},
                Node{5},
                Node{3},
                Node{2},
                Node{Token{plus, 6, 7}},
                Node{Token{number, 8, 10}},
                Node{7},
                Node{5},
                Node{4},
                Node{1},
                Node{0}
            };

            REQUIRE(actualTree == expectedTree);
        }

        SECTION("failed parse")
        {
            REQUIRE_THROWS_AS(parse(tokenizer("a *"), parsingTable, grammar, doNothing), SyntaxError);

            REQUIRE_THROWS_AS(parse(tokenizer("* 2"), parsingTable, grammar, doNothing), SyntaxError);

            REQUIRE_THROWS_AS(parse(tokenizer("+ * a"), parsingTable, grammar, doNothing), SyntaxError);

            REQUIRE_THROWS_AS(parse(tokenizer("x y"), parsingTable, grammar, doNothing), SyntaxError);
        }
    }

    SECTION("grammar #2")
    {
        const auto grammar = Grammar{/*0*/ "Start -> A \n"
                                     /*1*/ "A -> B a   \n"
                                     /*2*/ "B -> A b   \n"
                                     /*3*/ "B -> b     \n"
                                     /*4*/ "B ->"};

        const auto a = grammar.getSymbol("a");
        const auto b = grammar.getSymbol("b");
        
        const auto tokenizer = RegexTokenizer{{{a, "a"}, {b, "b"}}};

        const auto parsingTable = getClr1ParsingTable(grammar, getAutomaton(grammar));

        SECTION("successful parse")
        {
            auto actualTree = std::vector<Node>{};

            const auto visitor = [&actualTree](const Node& node) { actualTree.push_back(node); };

            parse(tokenizer("ababa"), parsingTable, grammar, visitor);

            const auto a = grammar.getSymbol("a");
            const auto b = grammar.getSymbol("b");

            const auto expectedTree = std::vector<Node>{
                Node{4},
                Node{Token{a, 0, 1}},
                Node{1},
                Node{Token{b, 1, 2}},
                Node{2},
                Node{Token{a, 2, 3}},
                Node{1},
                Node{Token{b, 3, 4}},
                Node{2},
                Node{Token{a, 4, 5}},
                Node{1},
                Node{0}
            };

            REQUIRE(actualTree == expectedTree);
        }

        SECTION("failed parse")
        {
            REQUIRE_THROWS_AS(parse(tokenizer("aa"), parsingTable, grammar, doNothing), SyntaxError);

            REQUIRE_THROWS_AS(parse(tokenizer("aaba"), parsingTable, grammar, doNothing), SyntaxError);
        }
    }
}
// clang-format on
