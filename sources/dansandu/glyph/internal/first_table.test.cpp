#include "dansandu/glyph/internal/first_table.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <set>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::first_table::getFirstTable;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

static std::set<Symbol> set(std::initializer_list<Symbol> list)
{
    return std::set<Symbol>{list.begin(), list.end()};
}

// clang-format off
TEST_CASE("first_table")
{
    auto firstTable = std::vector<std::vector<Symbol>>{};

    const auto getFirstSet = [&firstTable](const auto symbol)
    {
        const auto& row = firstTable[symbol.getIdentifierIndex()];
        return std::set<Symbol>{row.cbegin(), row.cend()};
    };

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

        firstTable = getFirstTable(grammar);

        REQUIRE(getFirstSet(Start) == set({id, number, empty}));

        REQUIRE(getFirstSet(S) == set({id, number, empty}));

        REQUIRE(getFirstSet(Sum) == set({id, number}));

        REQUIRE(getFirstSet(Value) == set({id, number}));

        REQUIRE(getFirstSet(ParametersBegin) == set({empty}));

        REQUIRE(getFirstSet(Parameters) == set({id, number}));

        REQUIRE(getFirstSet(number) == set({number}));

        REQUIRE(getFirstSet(add) == set({add}));

        REQUIRE(getFirstSet(empty) == set({empty}));

        REQUIRE(getFirstSet(id) == set({id}));

        REQUIRE(getFirstSet(end) == set({end}));

        REQUIRE(getFirstSet(parenthesesStart) == set({parenthesesStart}));

        REQUIRE(getFirstSet(parenthesesEnd) == set({parenthesesEnd}));

        REQUIRE(getFirstSet(comma) == set({comma}));
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

        const auto Start = grammar.getSymbol("Start");
        const auto A     = grammar.getSymbol("A");
        const auto B     = grammar.getSymbol("B");
        const auto C     = grammar.getSymbol("C");
        const auto D     = grammar.getSymbol("D");
        const auto end   = grammar.getSymbol("$");
        const auto empty = grammar.getSymbol("");
        const auto a     = grammar.getSymbol("a");
        const auto b     = grammar.getSymbol("b");
        const auto c     = grammar.getSymbol("c");
        const auto d     = grammar.getSymbol("d");

        firstTable = getFirstTable(grammar);

        REQUIRE(getFirstSet(Start) == set({a, b, c, d, empty}));

        REQUIRE(getFirstSet(A) == set({a, empty}));

        REQUIRE(getFirstSet(B) == set({b, c, d, a, empty}));

        REQUIRE(getFirstSet(C) == set({c, empty}));

        REQUIRE(getFirstSet(D) == set({d, a, empty}));

        REQUIRE(getFirstSet(a) == set({a}));

        REQUIRE(getFirstSet(b) == set({b}));

        REQUIRE(getFirstSet(c) == set({c}));

        REQUIRE(getFirstSet(d) == set({d}));

        REQUIRE(getFirstSet(empty) == set({empty}));

        REQUIRE(getFirstSet(end) == set({end}));
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

        firstTable = getFirstTable(grammar);

        REQUIRE(getFirstSet(Start) == set({b, a}));

        REQUIRE(getFirstSet(A) == set({b, a}));

        REQUIRE(getFirstSet(B) == set({b, a, empty}));

        REQUIRE(getFirstSet(a) == set({a}));

        REQUIRE(getFirstSet(b) == set({b}));

        REQUIRE(getFirstSet(empty) == set({empty}));

        REQUIRE(getFirstSet(end) == set({end}));
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
        const auto X     = grammar.getSymbol("X");
        const auto A1    = grammar.getSymbol("A1");
        const auto A2    = grammar.getSymbol("A2");
        const auto A3    = grammar.getSymbol("A3");
        const auto B1    = grammar.getSymbol("B1");
        const auto B2    = grammar.getSymbol("B2");
        const auto B3    = grammar.getSymbol("B3");
        const auto B4    = grammar.getSymbol("B4");
        const auto C1    = grammar.getSymbol("C1");
        const auto C2    = grammar.getSymbol("C2");
        const auto D1    = grammar.getSymbol("D1");
        const auto D2    = grammar.getSymbol("D2");
        const auto D3    = grammar.getSymbol("D3");
        const auto E     = grammar.getSymbol("E");
        const auto F     = grammar.getSymbol("F");
        const auto G     = grammar.getSymbol("G");
        const auto empty = grammar.getSymbol("");
        const auto end   = grammar.getSymbol("$");
        const auto a     = grammar.getSymbol("a");
        const auto b     = grammar.getSymbol("b");
        const auto c     = grammar.getSymbol("c");
        const auto d     = grammar.getSymbol("d");
        const auto e     = grammar.getSymbol("e");
        const auto f     = grammar.getSymbol("f");
        const auto g     = grammar.getSymbol("g");

        firstTable = getFirstTable(grammar);

        REQUIRE(getFirstSet(Start) == set({b, f, g, c, a, e, d, empty}));

        REQUIRE(getFirstSet(G) == set({g}));

        REQUIRE(getFirstSet(D2) == set({g, d}));

        REQUIRE(getFirstSet(D3) == set({g, d}));

        REQUIRE(getFirstSet(D1) == set({g, d}));

        REQUIRE(getFirstSet(C2) == set({c, empty, d, g}));

        REQUIRE(getFirstSet(C1) == set({c, empty, d, g}));

        REQUIRE(getFirstSet(X) == set({b, empty, f, g, c, a, e, d}));

        REQUIRE(getFirstSet(F) == set({f}));

        REQUIRE(getFirstSet(B3) == set({f, g, d, b}));

        REQUIRE(getFirstSet(B1) == set({f, g, d, b}));

        REQUIRE(getFirstSet(B2) == set({empty, f, d, b, g}));

        REQUIRE(getFirstSet(B4) == set({f, g, d, b}));

        REQUIRE(getFirstSet(E) == set({e}));

        REQUIRE(getFirstSet(A3) == set({b, d, f, a, e, g}));

        REQUIRE(getFirstSet(A1) == set({b, d, f, a, e, g}));

        REQUIRE(getFirstSet(A2) == set({b, d, f, a, e, g}));

        REQUIRE(getFirstSet(a) == set({a}));

        REQUIRE(getFirstSet(b) == set({b}));

        REQUIRE(getFirstSet(c) == set({c}));

        REQUIRE(getFirstSet(d) == set({d}));

        REQUIRE(getFirstSet(e) == set({e}));

        REQUIRE(getFirstSet(f) == set({f}));

        REQUIRE(getFirstSet(g) == set({g}));

        REQUIRE(getFirstSet(empty) == set({empty}));

        REQUIRE(getFirstSet(end) == set({end}));
    }
}
// clang-format on
