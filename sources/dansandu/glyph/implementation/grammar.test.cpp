#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <string>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::grammar::Symbol;

// clang-format off
TEST_CASE("Grammar")
{
    SECTION("invalid grammars")
    {
        REQUIRE_THROWS_AS(Grammar{"Start Sums -> Sums"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums Sums"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums -> Products Products -> Value"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums0 -> Value"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums -> Value$"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums -> Value + Value"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Start -> S\nStart -> SS"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"S -> SS\nStart -> SS"}, GrammarError);
    }

    SECTION("grammar #1")
    {
        auto text = R"(
            Start -> S
            S     -> Sum
            S     ->
            Sum   -> Sum add Value
            Sum   -> Value
            Value -> number
            Value -> id
        )";

        auto start  = Symbol{0};
        auto s      = Symbol{1};
        auto sum    = Symbol{2};
        auto value  = Symbol{3};
        auto end    = Symbol{4};
        auto empty  = Symbol{5};
        auto add    = Symbol{6};
        auto number = Symbol{7};
        auto id     = Symbol{8};
        
        auto grammar = Grammar{text};

        REQUIRE(grammar.toString() == text);

        SECTION("rules")
        {
            auto rules = std::vector<Rule>{
                Rule{start, {s}},
                Rule{s,     {sum}},
                Rule{s,     {empty}},
                Rule{sum,   {sum, add, value}},
                Rule{sum,   {value}},
                Rule{value, {number}},
                Rule{value, {id}}
            };

            REQUIRE(grammar.getRules() == rules);
        }

        SECTION("special symbols")
        {
            REQUIRE(grammar.getStartSymbol() == start);

            REQUIRE(grammar.getEndOfStringSymbol() == end);

            REQUIRE(grammar.getEmptySymbol() == empty);
        }

        SECTION("identifiers")
        {
            REQUIRE(grammar.getIdentifier(start) == "Start");

            REQUIRE(grammar.getIdentifier(s) == "S");

            REQUIRE(grammar.getIdentifier(sum) == "Sum");

            REQUIRE(grammar.getIdentifier(add) == "add");

            REQUIRE(grammar.getIdentifier(value) == "Value");

            REQUIRE(grammar.getIdentifier(number) == "number");

            REQUIRE(grammar.getIdentifier(id) == "id");

            REQUIRE(grammar.getIdentifier(empty) == "");

            REQUIRE(grammar.getIdentifier(end) == "$");
        }

        SECTION("non-terminals")
        {
            REQUIRE(grammar.isNonTerminal(start));

            REQUIRE(grammar.isNonTerminal(sum));

            REQUIRE(grammar.isNonTerminal(value));

            REQUIRE(grammar.isNonTerminal(s));
        }

        SECTION("terminals")
        {
            REQUIRE(grammar.isTerminal(add));

            REQUIRE(grammar.isTerminal(number));

            REQUIRE(grammar.isTerminal(id));

            REQUIRE(grammar.isTerminal(end));

            REQUIRE(grammar.isTerminal(empty));
        }

        SECTION("first sets")
        {
            REQUIRE(grammar.getFirstSet(start) == std::vector<Symbol>{id, number, empty});

            REQUIRE(grammar.getFirstSet(s) == std::vector<Symbol>{id, number, empty});

            REQUIRE(grammar.getFirstSet(sum) == std::vector<Symbol>{id, number});

            REQUIRE(grammar.getFirstSet(value) == std::vector<Symbol>{id, number});

            REQUIRE(grammar.getFirstSet(number) == std::vector<Symbol>{number});

            REQUIRE(grammar.getFirstSet(id) == std::vector<Symbol>{id});

            REQUIRE(grammar.getFirstSet(empty) == std::vector<Symbol>{empty});
        }
    }

    SECTION("grammar #2")
    {
        auto text = R"(
            Start -> A B C D
            A -> a
            A ->
            B -> C D
            B -> b
            C -> c
            C ->
            D -> A a
            D -> d
            D ->
        )";

        auto Start   = Symbol{0};
        auto A       = Symbol{1};
        auto B       = Symbol{2};
        auto C       = Symbol{3};
        auto D       = Symbol{4};
        auto end     = Symbol{5};
        auto empty   = Symbol{6};
        auto a       = Symbol{7};
        auto b       = Symbol{8};
        auto c       = Symbol{9};
        auto d       = Symbol{10};

        auto grammar = Grammar{text};

        REQUIRE(grammar.toString() == text);

        SECTION("rules")
        {
            auto rules = std::vector<Rule>{
                Rule{Start, {A, B, C, D}},
                Rule{A,     {a}},
                Rule{A,     {empty}},
                Rule{B,     {C, D}},
                Rule{B,     {b}},
                Rule{C,     {c}},
                Rule{C,     {empty}},
                Rule{D,     {A, a}},
                Rule{D,     {d}},
                Rule{D,     {empty}}
            };

            REQUIRE(grammar.getRules() == rules);
        }

        SECTION("special symbols")
        {
            REQUIRE(grammar.getStartSymbol() == Start);

            REQUIRE(grammar.getEndOfStringSymbol() == end);

            REQUIRE(grammar.getEmptySymbol() == empty);
        }

        SECTION("non-terminals")
        {
            REQUIRE(grammar.isNonTerminal(Start));

            REQUIRE(grammar.isNonTerminal(A));

            REQUIRE(grammar.isNonTerminal(B));

            REQUIRE(grammar.isNonTerminal(C));

            REQUIRE(grammar.isNonTerminal(D));
        }

        SECTION("terminals")
        {
            REQUIRE(grammar.isTerminal(end));

            REQUIRE(grammar.isTerminal(empty));

            REQUIRE(grammar.isTerminal(a));

            REQUIRE(grammar.isTerminal(b));

            REQUIRE(grammar.isTerminal(c));

            REQUIRE(grammar.isTerminal(d));
        }

        SECTION("identifiers")
        {
            REQUIRE(grammar.getIdentifier(Start) == "Start");

            REQUIRE(grammar.getIdentifier(A) == "A");

            REQUIRE(grammar.getIdentifier(B) == "B");

            REQUIRE(grammar.getIdentifier(C) == "C");

            REQUIRE(grammar.getIdentifier(D) == "D");

            REQUIRE(grammar.getIdentifier(end) == "$");

            REQUIRE(grammar.getIdentifier(empty) == "");

            REQUIRE(grammar.getIdentifier(a) == "a");

            REQUIRE(grammar.getIdentifier(b) == "b");

            REQUIRE(grammar.getIdentifier(c) == "c");

            REQUIRE(grammar.getIdentifier(d) == "d");
        }

        SECTION("first sets")
        {
            REQUIRE(grammar.getFirstSet(Start) == std::vector<Symbol>{a, b, c, d, empty});

            REQUIRE(grammar.getFirstSet(A) == std::vector<Symbol>{a, empty});

            REQUIRE(grammar.getFirstSet(B) == std::vector<Symbol>{b, c, d, a, empty});

            REQUIRE(grammar.getFirstSet(C) == std::vector<Symbol>{c, empty});

            REQUIRE(grammar.getFirstSet(D) == std::vector<Symbol>{d, a, empty});

            REQUIRE(grammar.getFirstSet(a) == std::vector<Symbol>{a});

            REQUIRE(grammar.getFirstSet(b) == std::vector<Symbol>{b});

            REQUIRE(grammar.getFirstSet(c) == std::vector<Symbol>{c});

            REQUIRE(grammar.getFirstSet(d) == std::vector<Symbol>{d});

            REQUIRE(grammar.getFirstSet(empty) == std::vector<Symbol>{empty});
        }
    }

    SECTION("grammar #3")
    {
        auto text = R"(
            Start -> A
            A -> B a
            B -> A b
            B -> b
            B ->
        )";

        auto Start = Symbol{0};
        auto A     = Symbol{1};
        auto B     = Symbol{2};
        auto end   = Symbol{3};
        auto empty = Symbol{4};
        auto a     = Symbol{5};
        auto b     = Symbol{6};

        auto grammar = Grammar{text};

        SECTION("rules")
        {
            auto rules = std::vector<Rule>{
                Rule{Start, {A}},
                Rule{A,     {B, a}},
                Rule{B,     {A, b}},
                Rule{B,     {b}},
                Rule{B,     {empty}}
            };

            REQUIRE(grammar.getRules() == rules);
        }

        SECTION("non-terminals")
        {
            REQUIRE(grammar.isNonTerminal(Start));

            REQUIRE(grammar.isNonTerminal(A));

            REQUIRE(grammar.isNonTerminal(B));
        }

        SECTION("terminals")
        {
            REQUIRE(grammar.isTerminal(end));

            REQUIRE(grammar.isTerminal(empty));

            REQUIRE(grammar.isTerminal(a));

            REQUIRE(grammar.isTerminal(b));
        }

        SECTION("identifiers")
        {
            REQUIRE(grammar.getIdentifier(Start) == "Start");

            REQUIRE(grammar.getIdentifier(A) == "A");

            REQUIRE(grammar.getIdentifier(B) == "B");

            REQUIRE(grammar.getIdentifier(end) == "$");

            REQUIRE(grammar.getIdentifier(empty) == "");

            REQUIRE(grammar.getIdentifier(a) == "a");

            REQUIRE(grammar.getIdentifier(b) == "b");
        }

        SECTION("first sets")
        {
            REQUIRE(grammar.getFirstSet(Start) == std::vector<Symbol>{b, a});
        
            REQUIRE(grammar.getFirstSet(A) == std::vector<Symbol>{b, a});

            REQUIRE(grammar.getFirstSet(B) == std::vector<Symbol>{b, a, empty});

            REQUIRE(grammar.getFirstSet(a) == std::vector<Symbol>{a});

            REQUIRE(grammar.getFirstSet(b) == std::vector<Symbol>{b});

            REQUIRE(grammar.getFirstSet(empty) == std::vector<Symbol>{empty});
        }
    }
}
// clang-format on
