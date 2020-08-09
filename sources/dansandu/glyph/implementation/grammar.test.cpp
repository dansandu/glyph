#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::rule::Rule;
using dansandu::glyph::symbol::Symbol;

static std::set<Symbol> set(const std::vector<Symbol>& l)
{
    return std::set<Symbol>{l.cbegin(), l.cend()};
}

static std::set<Symbol> set(std::initializer_list<Symbol> l)
{
    return std::set<Symbol>{l.begin(), l.end()};
}

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
        
        auto grammar = Grammar{text};

        auto start  = grammar.getSymbol("Start");
        auto s      = grammar.getSymbol("S");
        auto sum    = grammar.getSymbol("Sum");
        auto value  = grammar.getSymbol("Value");
        auto end    = grammar.getSymbol("$");
        auto empty  = grammar.getSymbol("");
        auto add    = grammar.getSymbol("add");
        auto number = grammar.getSymbol("number");
        auto id     = grammar.getSymbol("id");

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
            REQUIRE(set(grammar.getFirstSet(start)) == set({id, number, empty}));

            REQUIRE(set(grammar.getFirstSet(s)) == set({id, number, empty}));

            REQUIRE(set(grammar.getFirstSet(sum)) == set({id, number}));

            REQUIRE(set(grammar.getFirstSet(value)) == set({id, number}));

            REQUIRE(set(grammar.getFirstSet(number)) == set({number}));

            REQUIRE(set(grammar.getFirstSet(id)) == set({id}));

            REQUIRE(set(grammar.getFirstSet(empty)) == set({empty}));
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

        auto grammar = Grammar{text};

        auto Start   = grammar.getSymbol("Start");
        auto A       = grammar.getSymbol("A");
        auto B       = grammar.getSymbol("B");
        auto C       = grammar.getSymbol("C");
        auto D       = grammar.getSymbol("D");
        auto end     = grammar.getSymbol("$");
        auto empty   = grammar.getSymbol("");
        auto a       = grammar.getSymbol("a");
        auto b       = grammar.getSymbol("b");
        auto c       = grammar.getSymbol("c");
        auto d       = grammar.getSymbol("d");

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
            REQUIRE(set(grammar.getFirstSet(Start)) == set({a, b, c, d, empty}));

            REQUIRE(set(grammar.getFirstSet(A)) == set({a, empty}));

            REQUIRE(set(grammar.getFirstSet(B)) == set({b, c, d, a, empty}));

            REQUIRE(set(grammar.getFirstSet(C)) == set({c, empty}));

            REQUIRE(set(grammar.getFirstSet(D)) == set({d, a, empty}));

            REQUIRE(set(grammar.getFirstSet(a)) == set({a}));

            REQUIRE(set(grammar.getFirstSet(b)) == set({b}));

            REQUIRE(set(grammar.getFirstSet(c)) == set({c}));

            REQUIRE(set(grammar.getFirstSet(d)) == set({d}));

            REQUIRE(set(grammar.getFirstSet(empty)) == set({empty}));
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

        auto grammar = Grammar{text};

        auto Start = grammar.getSymbol("Start");
        auto A     = grammar.getSymbol("A");
        auto B     = grammar.getSymbol("B");
        auto end   = grammar.getSymbol("$");
        auto empty = grammar.getSymbol("");
        auto a     = grammar.getSymbol("a");
        auto b     = grammar.getSymbol("b");

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
            REQUIRE(set(grammar.getFirstSet(Start)) == set({b, a}));
        
            REQUIRE(set(grammar.getFirstSet(A)) == set({b, a}));

            REQUIRE(set(grammar.getFirstSet(B)) == set({b, a, empty}));

            REQUIRE(set(grammar.getFirstSet(a)) == set({a}));

            REQUIRE(set(grammar.getFirstSet(b)) == set({b}));

            REQUIRE(set(grammar.getFirstSet(empty)) == set({empty}));
        }
    }

    SECTION("grammar #4")
    {
        auto text = R"(
            Start -> X
            X -> A1
            X -> C1
            A1 -> A2
            A1 -> B1
            A2 -> A3
            A2 -> a
            A3 -> A1
            A3 -> E
            B1 -> B2 B3
            B2 -> B3
            B2 -> b
            B2 ->
            B3 -> B4
            B3 -> B1
            B3 -> F
            B4 -> B1
            B4 -> D2
            C1 -> C2
            C1 -> c
            C1 ->
            C2 -> C1
            C2 -> D1
            D1 -> D2
            D1 -> D3
            D2 -> D1
            D2 -> D3
            D3 -> D1
            D3 -> D2
            D3 -> G
            D3 -> d
            E -> e
            F -> f
            G -> g
        )";

        auto grammar = Grammar{text};

        auto Start = grammar.getSymbol("Start");
        auto X = grammar.getSymbol("X");
        auto A1 = grammar.getSymbol("A1");
        auto A2 = grammar.getSymbol("A2");
        auto A3 = grammar.getSymbol("A3");
        auto B1 = grammar.getSymbol("B1");
        auto B2 = grammar.getSymbol("B2");
        auto B3 = grammar.getSymbol("B3");
        auto B4 = grammar.getSymbol("B4");
        auto C1 = grammar.getSymbol("C1");
        auto C2 = grammar.getSymbol("C2");
        auto D1 = grammar.getSymbol("D1");
        auto D2 = grammar.getSymbol("D2");
        auto D3 = grammar.getSymbol("D3");
        auto E = grammar.getSymbol("E");
        auto F = grammar.getSymbol("F");
        auto G = grammar.getSymbol("G");
        auto empty = grammar.getSymbol("");
        auto a = grammar.getSymbol("a");
        auto b = grammar.getSymbol("b");
        auto c = grammar.getSymbol("c");
        auto d = grammar.getSymbol("d");
        auto e = grammar.getSymbol("e");
        auto f = grammar.getSymbol("f");
        auto g = grammar.getSymbol("g");

        REQUIRE(set(grammar.getFirstSet(Start)) == set({b, f, g, c, a, e, d, empty}));

        REQUIRE(set(grammar.getFirstSet(G)) == set({g}));

        REQUIRE(set(grammar.getFirstSet(D2)) == set({g, d}));

        REQUIRE(set(grammar.getFirstSet(D3)) == set({g, d}));

        REQUIRE(set(grammar.getFirstSet(D1)) == set({g, d}));

        REQUIRE(set(grammar.getFirstSet(C2)) == set({c, empty, d, g}));

        REQUIRE(set(grammar.getFirstSet(C1)) == set({c, empty, d, g}));

        REQUIRE(set(grammar.getFirstSet(X)) == set({b, empty, f, g, c, a, e, d}));

        REQUIRE(set(grammar.getFirstSet(F)) == set({f}));

        REQUIRE(set(grammar.getFirstSet(B3)) == set({f, g, d, b}));

        REQUIRE(set(grammar.getFirstSet(B1)) == set({f, g, d, b}));

        REQUIRE(set(grammar.getFirstSet(B2)) == set({empty, f, d, b, g}));

        REQUIRE(set(grammar.getFirstSet(B4)) == set({f, g, d, b}));

        REQUIRE(set(grammar.getFirstSet(E)) == set({e}));

        REQUIRE(set(grammar.getFirstSet(A3)) == set({b, d, f, a, e, g}));

        REQUIRE(set(grammar.getFirstSet(A1)) == set({b, d, f, a, e, g}));

        REQUIRE(set(grammar.getFirstSet(A2)) == set({b, d, f, a, e, g}));

        REQUIRE(set(grammar.getFirstSet(a)) == set({a}));

        REQUIRE(set(grammar.getFirstSet(b)) == set({b}));

        REQUIRE(set(grammar.getFirstSet(c)) == set({c}));

        REQUIRE(set(grammar.getFirstSet(d)) == set({d}));

        REQUIRE(set(grammar.getFirstSet(e)) == set({e}));

        REQUIRE(set(grammar.getFirstSet(f)) == set({f}));

        REQUIRE(set(grammar.getFirstSet(g)) == set({g}));

        REQUIRE(set(grammar.getFirstSet(empty)) == set({empty}));
    }
}
// clang-format on
