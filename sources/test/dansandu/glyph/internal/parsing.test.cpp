#include "dansandu/glyph/internal/parsing.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
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

TEST_CASE("Parsing")
{
    SECTION("grammar #1")
    {
        const auto grammar = Grammar{R"(
            /*0*/ Start       -> Sums
            /*1*/ Sums        -> Sums plus Products
            /*2*/ Sums        -> Products
            /*3*/ Products    -> Products multiply SignedValue
            /*4*/ Products    -> SignedValue
            /*5*/ SignedValue -> Value
            /*6*/ SignedValue -> plus Value
            /*7*/ Value       -> number
            /*8*/ Value       -> identifier
        )"};

        const auto identifier = grammar.getSymbol("identifier");
        const auto plus = grammar.getSymbol("plus");
        const auto multiply = grammar.getSymbol("multiply");
        const auto number = grammar.getSymbol("number");
        const auto discarded = grammar.getDiscardedSymbolPlaceholder();

        const auto tokenizer = RegexTokenizer{{{identifier, "[a-z]\\w*"},
                                               {number, "([1-9]\\d*|0)(\\.\\d+)?"},
                                               {plus, "\\+"},
                                               {multiply, "\\*"},
                                               {discarded, "\\s+"}}};

        const auto parsingTable = getClr1ParsingTable(grammar, getAutomaton(grammar));

        SECTION("successful parse")
        {
            const auto text = "a * b + 10";

            const auto nodes = parse(text, tokenizer.tokenize(text), parsingTable, grammar);

            const auto expected = std::vector<Node>{Node{Token{identifier, 0, 1}},
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
                                                    Node{0}};

            REQUIRE(nodes == expected);
        }

        SECTION("failed parse")
        {
            {
                const auto text = "a *";
                REQUIRE_THROWS_AS(parse(text, tokenizer.tokenize(text), parsingTable, grammar), SyntaxError);
            }

            {
                const auto text = "* 2";
                REQUIRE_THROWS_AS(parse(text, tokenizer.tokenize(text), parsingTable, grammar), SyntaxError);
            }

            {
                const auto text = "+ * a";
                REQUIRE_THROWS_AS(parse(text, tokenizer.tokenize(text), parsingTable, grammar), SyntaxError);
            }

            {
                const auto text = "x y";
                REQUIRE_THROWS_AS(parse(text, tokenizer.tokenize(text), parsingTable, grammar), SyntaxError);
            }
        }
    }

    SECTION("grammar #2")
    {
        const auto grammar = Grammar{R"(
            /*0*/ Start -> A
            /*1*/ A -> B a
            /*2*/ B -> A b
            /*3*/ B -> b
            /*4*/ B ->
        )"};

        const auto a = grammar.getSymbol("a");
        const auto b = grammar.getSymbol("b");

        const auto tokenizer = RegexTokenizer{{{a, "a"}, {b, "b"}}};

        const auto parsingTable = getClr1ParsingTable(grammar, getAutomaton(grammar));

        SECTION("successful parse")
        {
            const auto text = "ababa";

            const auto nodes = parse(text, tokenizer.tokenize(text), parsingTable, grammar);

            const auto a = grammar.getSymbol("a");
            const auto b = grammar.getSymbol("b");

            const auto expected = std::vector<Node>{Node{4}, Node{Token{a, 0, 1}}, Node{1}, Node{Token{b, 1, 2}},
                                                    Node{2}, Node{Token{a, 2, 3}}, Node{1}, Node{Token{b, 3, 4}},
                                                    Node{2}, Node{Token{a, 4, 5}}, Node{1}, Node{0}};

            REQUIRE(nodes == expected);
        }

        SECTION("failed parse")
        {
            {
                const auto text = "aa";
                REQUIRE_THROWS_AS(parse(text, tokenizer.tokenize(text), parsingTable, grammar), SyntaxError);
            }

            {
                const auto text = "aaba";
                REQUIRE_THROWS_AS(parse(text, tokenizer.tokenize(text), parsingTable, grammar), SyntaxError);
            }
        }
    }
}
