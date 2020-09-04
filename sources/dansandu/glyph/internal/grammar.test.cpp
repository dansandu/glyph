#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::rule::Rule;
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

        REQUIRE_THROWS_AS(Grammar{"Start -> Sums\nSums -> Start"}, GrammarError);
    }

    SECTION("grammar #1")
    {
        const auto text = R"(
            Start -> S
            S     -> Sum
            S     ->
            Sum   -> Sum add Value
            Sum   -> Value
            Value -> number
            Value -> id
            Value -> id parenthesesStart ParametersBegin Parameters parenthesesEnd
            ParametersBegin ->
            Parameters -> Parameters comma Sum
            Parameters -> Sum
        )";
        
        const auto grammar = Grammar{text};

        const auto Start            = grammar.getSymbol("Start");
        const auto S                = grammar.getSymbol("S");
        const auto Sum              = grammar.getSymbol("Sum");
        const auto Value            = grammar.getSymbol("Value");
        const auto ParametersBegin  = grammar.getSymbol("ParametersBegin");
        const auto Parameters       = grammar.getSymbol("Parameters");
        const auto end              = grammar.getSymbol("$");
        const auto empty            = grammar.getSymbol("");
        const auto add              = grammar.getSymbol("add");
        const auto number           = grammar.getSymbol("number");
        const auto id               = grammar.getSymbol("id");
        const auto parenthesesStart = grammar.getSymbol("parenthesesStart");
        const auto parenthesesEnd   = grammar.getSymbol("parenthesesEnd");
        const auto comma            = grammar.getSymbol("comma");

        REQUIRE(grammar.toString() == text);

        SECTION("rules")
        {
            const auto expectedRules = std::vector<Rule>{
                Rule{Start,           {S}},
                Rule{S,               {Sum}},
                Rule{S,               {}},
                Rule{Sum,             {Sum, add, Value}},
                Rule{Sum,             {Value}},
                Rule{Value,           {number}},
                Rule{Value,           {id}},
                Rule{Value,           {id, parenthesesStart, ParametersBegin, Parameters, parenthesesEnd}},
                Rule{ParametersBegin, {}},
                Rule{Parameters,      {Parameters, comma, Sum}},
                Rule{Parameters,      {Sum}},
            };

            REQUIRE(grammar.getRules() == expectedRules);
        }

        SECTION("special symbols")
        {
            REQUIRE(grammar.getStartSymbol() == Start);

            REQUIRE(grammar.getEndOfStringSymbol() == end);

            REQUIRE(grammar.getEmptySymbol() == empty);
        }

        SECTION("identifiers")
        {
            REQUIRE(grammar.getIdentifier(Start) == "Start");

            REQUIRE(grammar.getIdentifier(S) == "S");

            REQUIRE(grammar.getIdentifier(Sum) == "Sum");

            REQUIRE(grammar.getIdentifier(Value) == "Value");

            REQUIRE(grammar.getIdentifier(ParametersBegin) == "ParametersBegin");

            REQUIRE(grammar.getIdentifier(Parameters) == "Parameters");

            REQUIRE(grammar.getIdentifier(add) == "add");

            REQUIRE(grammar.getIdentifier(number) == "number");

            REQUIRE(grammar.getIdentifier(id) == "id");

            REQUIRE(grammar.getIdentifier(empty) == "");

            REQUIRE(grammar.getIdentifier(end) == "$");
            
            REQUIRE(grammar.getIdentifier(parenthesesStart) == "parenthesesStart");
            
            REQUIRE(grammar.getIdentifier(parenthesesEnd) == "parenthesesEnd");

            REQUIRE(grammar.getIdentifier(comma) == "comma");
        }

        SECTION("non-terminals")
        {
            REQUIRE(grammar.isNonTerminal(Start));

            REQUIRE(grammar.isNonTerminal(Sum));

            REQUIRE(grammar.isNonTerminal(Value));

            REQUIRE(grammar.isNonTerminal(S));

            REQUIRE(grammar.isNonTerminal(ParametersBegin));

            REQUIRE(grammar.isNonTerminal(Parameters));
        }

        SECTION("terminals")
        {
            REQUIRE(grammar.isTerminal(add));

            REQUIRE(grammar.isTerminal(number));

            REQUIRE(grammar.isTerminal(id));

            REQUIRE(grammar.isTerminal(end));

            REQUIRE(grammar.isTerminal(empty));

            REQUIRE(grammar.isTerminal(parenthesesStart));

            REQUIRE(grammar.isTerminal(parenthesesEnd));

            REQUIRE(grammar.isTerminal(comma));
        }

        SECTION("first sets")
        {
            REQUIRE(set(grammar.getFirstSet(Start)) == set({id, number, empty}));

            REQUIRE(set(grammar.getFirstSet(S)) == set({id, number, empty}));

            REQUIRE(set(grammar.getFirstSet(Sum)) == set({id, number}));

            REQUIRE(set(grammar.getFirstSet(Value)) == set({id, number}));
            
            REQUIRE(set(grammar.getFirstSet(ParametersBegin)) == set({empty}));

            REQUIRE(set(grammar.getFirstSet(Parameters)) == set({id, number}));

            REQUIRE(set(grammar.getFirstSet(number)) == set({number}));

            REQUIRE(set(grammar.getFirstSet(id)) == set({id}));

            REQUIRE(set(grammar.getFirstSet(empty)) == set({empty}));

            REQUIRE(set(grammar.getFirstSet(end)) == set({end}));
            
            REQUIRE(set(grammar.getFirstSet(parenthesesStart)) == set({parenthesesStart}));
            
            REQUIRE(set(grammar.getFirstSet(parenthesesEnd)) == set({parenthesesEnd}));
            
            REQUIRE(set(grammar.getFirstSet(comma)) == set({comma}));
        }
    }

    SECTION("grammar #2")
    {
        const auto text = R"(
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

        const auto grammar = Grammar{text};

        const auto Start   = grammar.getSymbol("Start");
        const auto A       = grammar.getSymbol("A");
        const auto B       = grammar.getSymbol("B");
        const auto C       = grammar.getSymbol("C");
        const auto D       = grammar.getSymbol("D");
        const auto end     = grammar.getSymbol("$");
        const auto empty   = grammar.getSymbol("");
        const auto a       = grammar.getSymbol("a");
        const auto b       = grammar.getSymbol("b");
        const auto c       = grammar.getSymbol("c");
        const auto d       = grammar.getSymbol("d");

        REQUIRE(grammar.toString() == text);

        SECTION("rules")
        {
            const auto expectedRules = std::vector<Rule>{
                Rule{Start, {A, B, C, D}},
                Rule{A,     {a}},
                Rule{A,     {}},
                Rule{B,     {C, D}},
                Rule{B,     {b}},
                Rule{C,     {c}},
                Rule{C,     {}},
                Rule{D,     {A, a}},
                Rule{D,     {d}},
                Rule{D,     {}}
            };

            REQUIRE(grammar.getRules() == expectedRules);
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

        SECTION("get terminal")
        {
            REQUIRE(grammar.getTerminalSymbol("a") == a);

            REQUIRE(grammar.getTerminalSymbol("b") == b);

            REQUIRE(grammar.getTerminalSymbol("c") == c);

            REQUIRE(grammar.getTerminalSymbol("d") == d);

            REQUIRE_THROWS_AS(grammar.getTerminalSymbol(""), GrammarError);

            REQUIRE_THROWS_AS(grammar.getTerminalSymbol("$"), GrammarError);
        }
    }

    SECTION("grammar #3")
    {
        const auto text = R"(
            Start -> A
            A -> B a
            B -> A b
            B -> b
            B ->
        )";

        const auto grammar = Grammar{text};

        const auto Start = grammar.getSymbol("Start");
        const auto A     = grammar.getSymbol("A");
        const auto B     = grammar.getSymbol("B");
        const auto end   = grammar.getSymbol("$");
        const auto empty = grammar.getSymbol("");
        const auto a     = grammar.getSymbol("a");
        const auto b     = grammar.getSymbol("b");

        SECTION("rules")
        {
            const auto expectedRules = std::vector<Rule>{
                Rule{Start, {A}},
                Rule{A,     {B, a}},
                Rule{B,     {A, b}},
                Rule{B,     {b}},
                Rule{B,     {}}
            };

            REQUIRE(grammar.getRules() == expectedRules);
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
        const auto text = R"(
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

        const auto grammar = Grammar{text};

        const auto Start = grammar.getSymbol("Start");
        const auto X = grammar.getSymbol("X");
        const auto A1 = grammar.getSymbol("A1");
        const auto A2 = grammar.getSymbol("A2");
        const auto A3 = grammar.getSymbol("A3");
        const auto B1 = grammar.getSymbol("B1");
        const auto B2 = grammar.getSymbol("B2");
        const auto B3 = grammar.getSymbol("B3");
        const auto B4 = grammar.getSymbol("B4");
        const auto C1 = grammar.getSymbol("C1");
        const auto C2 = grammar.getSymbol("C2");
        const auto D1 = grammar.getSymbol("D1");
        const auto D2 = grammar.getSymbol("D2");
        const auto D3 = grammar.getSymbol("D3");
        const auto E = grammar.getSymbol("E");
        const auto F = grammar.getSymbol("F");
        const auto G = grammar.getSymbol("G");
        const auto empty = grammar.getSymbol("");
        const auto a = grammar.getSymbol("a");
        const auto b = grammar.getSymbol("b");
        const auto c = grammar.getSymbol("c");
        const auto d = grammar.getSymbol("d");
        const auto e = grammar.getSymbol("e");
        const auto f = grammar.getSymbol("f");
        const auto g = grammar.getSymbol("g");

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
