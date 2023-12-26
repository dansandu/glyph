#include "dansandu/glyph/internal/grammar.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <map>
#include <string>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::grammar::removeComments;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

TEST_CASE("Grammar")
{
    SECTION("comment removal")
    {
        REQUIRE(removeComments("") == "");

        REQUIRE(removeComments("Not a comment") == "Not a comment");

        REQUIRE(removeComments("/* comment */Not a comment") == "Not a comment");

        REQUIRE(removeComments("Not a comment/* comment */") == "Not a comment");

        REQUIRE(removeComments("Prefix/* comment */ */") == "Prefix */");

        REQUIRE(removeComments("/* /* comment */Suffix") == "Suffix");

        const auto text = R"(
            /*0*/ Start -> S      /* first production rule */
            /*1*/ S     -> SS     /* some explanation */
            /*2*/ SS    -> value  /* more details */
        )";

        const auto expected = R"(
             Start -> S      
             S     -> SS     
             SS    -> value  
        )";

        REQUIRE(removeComments(text) == expected);
    }

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

        const auto Start = grammar.getSymbol("Start");
        const auto S = grammar.getSymbol("S");
        const auto Sum = grammar.getSymbol("Sum");
        const auto Value = grammar.getSymbol("Value");
        const auto ParametersBegin = grammar.getSymbol("ParametersBegin");
        const auto Parameters = grammar.getSymbol("Parameters");
        const auto end = grammar.getSymbol("$");
        const auto empty = grammar.getSymbol("");
        const auto add = grammar.getSymbol("add");
        const auto number = grammar.getSymbol("number");
        const auto id = grammar.getSymbol("id");
        const auto parenthesesStart = grammar.getSymbol("parenthesesStart");
        const auto parenthesesEnd = grammar.getSymbol("parenthesesEnd");
        const auto comma = grammar.getSymbol("comma");

        SECTION("rules")
        {
            const auto expectedRules = std::vector<Rule>{
                Rule{Start, {S}},
                Rule{S, {Sum}},
                Rule{S, {}},
                Rule{Sum, {Sum, add, Value}},
                Rule{Sum, {Value}},
                Rule{Value, {number}},
                Rule{Value, {id}},
                Rule{Value, {id, parenthesesStart, ParametersBegin, Parameters, parenthesesEnd}},
                Rule{ParametersBegin, {}},
                Rule{Parameters, {Parameters, comma, Sum}},
                Rule{Parameters, {Sum}},
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
        const auto A = grammar.getSymbol("A");
        const auto B = grammar.getSymbol("B");
        const auto C = grammar.getSymbol("C");
        const auto D = grammar.getSymbol("D");
        const auto end = grammar.getSymbol("$");
        const auto empty = grammar.getSymbol("");
        const auto a = grammar.getSymbol("a");
        const auto b = grammar.getSymbol("b");
        const auto c = grammar.getSymbol("c");
        const auto d = grammar.getSymbol("d");

        SECTION("rules")
        {
            const auto expectedRules = std::vector<Rule>{Rule{Start, {A, B, C, D}},
                                                         Rule{A, {a}},
                                                         Rule{A, {}},
                                                         Rule{B, {C, D}},
                                                         Rule{B, {b}},
                                                         Rule{C, {c}},
                                                         Rule{C, {}},
                                                         Rule{D, {A, a}},
                                                         Rule{D, {d}},
                                                         Rule{D, {}}};

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
        const auto A = grammar.getSymbol("A");
        const auto B = grammar.getSymbol("B");
        const auto end = grammar.getSymbol("$");
        const auto empty = grammar.getSymbol("");
        const auto a = grammar.getSymbol("a");
        const auto b = grammar.getSymbol("b");

        SECTION("rules")
        {
            const auto expectedRules =
                std::vector<Rule>{Rule{Start, {A}}, Rule{A, {B, a}}, Rule{B, {A, b}}, Rule{B, {b}}, Rule{B, {}}};

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
    }
}
