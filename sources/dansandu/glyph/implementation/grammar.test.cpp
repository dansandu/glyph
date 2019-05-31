#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <string>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::implementation::grammar::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::getFirstTable;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::grammar::GrammarError;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::grammar::SymbolTable;

// clang-format off
TEST_CASE("Grammar") {
    SECTION("invalid grammars") {
        REQUIRE_THROWS_AS(Grammar{"Start Sums -> Sums"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums Sums"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums -> Products Products -> Value"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums0 -> Value"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums -> Value$"}, GrammarError);

        REQUIRE_THROWS_AS(Grammar{"Sums -> Value + Value"}, GrammarError);
    }

    SECTION("one rule grammar") {
        auto grammar = Grammar{"Value -> identifier"};

        REQUIRE(grammar.asString() == "Value -> identifier");

        REQUIRE(grammar.getRules() == std::vector<Rule>{{Rule{"Value", {"identifier"}}}});

        REQUIRE(grammar.getNonterminals() == std::vector<std::string>{{"Value"}});

        REQUIRE(grammar.getTerminals() == std::vector<std::string>{{"identifier"}});
    }

    SECTION("multiple rule grammar") {
        auto grammar = Grammar{"Start -> Sums           \n"
                               "Sums  -> Sums add Value \n"
                               "Sums  -> Value          \n"
                               "Value -> identifier     \n"
                               "Value -> number"};

        REQUIRE(grammar.getRules() == std::vector<Rule>{{Rule{"Start", {"Sums"}},
                                                         Rule{"Sums", {"Sums", "add", "Value"}},
                                                         Rule{"Sums", {"Value"}},
                                                         Rule{"Value", {"identifier"}},
                                                         Rule{"Value", {"number"}}}});            
        
        REQUIRE(grammar.getNonterminals() == std::vector<std::string>{{"Start", "Sums", "Value"}});

        REQUIRE(grammar.getTerminals() == std::vector<std::string>{{"add", "identifier", "number"}});

        REQUIRE(getFirstTable(grammar) == SymbolTable{{
            {"Start", { "identifier", "number" }},
            {"Sums", { "identifier", "number" }},
            {"Value", { "identifier", "number" }},
            {"identifier", { "identifier" }},
            {"number", { "number" }},
            {"add", {"add"}}
        }});
    }

    SECTION("hard grammar") {
        auto grammar = Grammar{"Start -> A B C D \n"
                               "A -> a           \n"
                               "A ->             \n"
                               "B -> C D         \n"
                               "B -> b           \n"
                               "C -> c           \n"
                               "C ->             \n"
                               "D -> A a         \n"
                               "D -> d           \n"
                               "D ->"};
        
        REQUIRE(grammar.getNonterminals() == std::vector<std::string>{{"A", "B", "C", "D", "Start"}});

        REQUIRE(grammar.getTerminals() == std::vector<std::string>{{"", "a", "b", "c", "d"}});

        REQUIRE(getFirstTable(grammar) == SymbolTable{{
            {"A", {"", "a"}},
            {"C", {"", "c"}},
            {"D", {"", "a", "d"}},
            {"B", {"", "a", "b", "c", "d"}},
            {"Start", {"", "a", "b", "c", "d"}},
            {"a", {"a"}},
            {"b", {"b"}},
            {"c", {"c"}},
            {"d", {"d"}},
            {"", {""}}
        }});
    }
}

TEST_CASE("ReallyHardGrammar", "[!mayfail]") {
    auto grammar = Grammar{"Start -> A \n"
                           "A -> B a   \n"
                           "B -> A b   \n"
                           "B -> b     \n"
                           "B ->"};
    
    REQUIRE(getFirstTable(grammar) == SymbolTable{{
        {"A", {"", "a", "b"}},
        {"B", {"", "a", "b"}},
        {"Start", {"", "a", "b"}},
        {"a", {"a"}},
        {"b", {"b"}},
        {"", {""}}
    }});
}
// clang-format on
