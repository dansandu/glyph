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
using dansandu::glyph::implementation::grammar::getFollowTable;
using dansandu::glyph::implementation::grammar::getRules;
using dansandu::glyph::implementation::grammar::getSymbols;
using dansandu::glyph::implementation::grammar::GrammarError;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::grammar::SymbolTable;

// clang-format off
TEST_CASE("Grammar") {
    SECTION("invalid grammars") {
        REQUIRE_THROWS_AS(getRules("Start Sums -> Sums"), GrammarError);

        REQUIRE_THROWS_AS(getRules("Sums Sums"), GrammarError);

        REQUIRE_THROWS_AS(getRules("Sums -> Products Products -> Value"), GrammarError);

        REQUIRE_THROWS_AS(getRules("Sums0 -> Value"), GrammarError);

        REQUIRE_THROWS_AS(getRules("Sums -> Value$"), GrammarError);

        REQUIRE_THROWS_AS(getRules("Sums -> Value + Value"), GrammarError);
    }

    SECTION("one rule grammar") {
        REQUIRE(getRules("Value -> identifier") == std::vector<Rule>{{Rule{"Value", {"identifier"}}}});
    }

    SECTION("multiple rule grammar") {
        constexpr auto grammar = "Start -> Sums           \n"
                                 "Sums  -> Sums add Value \n"
                                 "Sums  -> Value          \n"
                                 "Value -> identifier     \n"
                                 "Value -> number";

        const auto rules = getRules(grammar);
        SECTION("rules") {
            REQUIRE(rules == std::vector<Rule>{{Rule{"Start", {"Sums"}},
                                                Rule{"Sums", {"Sums", "add", "Value"}},
                                                Rule{"Sums", {"Value"}},
                                                Rule{"Value", {"identifier"}},
                                                Rule{"Value", {"number"}}}});            
        }
        
        auto symbols = getSymbols(rules);
        SECTION("symbols") {
            auto [nonterminals, terminals] = symbols;
        
            REQUIRE(nonterminals == std::vector<std::string>{{"Start", "Sums", "Value"}});

            REQUIRE(terminals == std::vector<std::string>{{"add", "identifier", "number"}});
        }

        SECTION("first and follow tables") {
            auto firstTable = getFirstTable(rules, symbols);

            REQUIRE(firstTable == SymbolTable{{
                {"Start", { "identifier", "number" }},
                {"Sums", { "identifier", "number" }},
                {"Value", { "identifier", "number" }},
                {"identifier", { "identifier" }},
                {"number", { "number" }},
                {"add", {"add"}}
            }});

            REQUIRE(getFollowTable(rules, firstTable) == SymbolTable{{
                {"Start", {endOfString}},
                {"Sums", {endOfString, "add"}},
                {"Value", {endOfString, "add"}},
                {"identifier", {endOfString, "add"}},
                {"number", {endOfString, "add"}},
                {"add", { "identifier", "number" }}
            }});
        }
    }

    SECTION("hard grammar") {
        constexpr auto grammar = "Start -> A B C D \n"
                                 "A -> a           \n"
                                 "A ->             \n"
                                 "B -> C D         \n"
                                 "B -> b           \n"
                                 "C -> c           \n"
                                 "C ->             \n"
                                 "D -> A a         \n"
                                 "D -> d           \n"
                                 "D ->";
        auto rules = getRules(grammar);
        auto symbols = getSymbols(rules);

        SECTION("symbols") {
            auto [nonterminals, terminals] = symbols;
        
            REQUIRE(nonterminals == std::vector<std::string>{{"A", "B", "C", "D", "Start"}});

            REQUIRE(terminals == std::vector<std::string>{{"", "a", "b", "c", "d"}});
        }

        auto firstTable = getFirstTable(rules, symbols);

        REQUIRE(firstTable == SymbolTable{{
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

        REQUIRE(getFollowTable(rules, firstTable) == SymbolTable{{
            {"Start", {endOfString}},
            {"A", {endOfString, "a", "b", "c", "d"}},
            {"B", {endOfString, "a", "c", "d"}},
            {"C", {endOfString, "a", "c", "d"}},
            {"D", {endOfString, "a", "c", "d"}},
            {"a", {endOfString, "a", "b", "c", "d"}},
            {"b", {endOfString, "a", "c", "d"}},
            {"c", {endOfString, "a", "c", "d"}},
            {"d", {endOfString, "a", "c", "d"}},
        }});
    }
}

TEST_CASE("Hard grammar", "[!mayfail]") {
    constexpr auto grammar = "Start -> A \n"
                                "A -> B a   \n"
                                "B -> A b   \n"
                                "B -> b     \n"
                                "B ->";
    auto rules = getRules(grammar);
    auto symbols = getSymbols(rules);
    auto firstTable = getFirstTable(rules, symbols);

    REQUIRE(firstTable == SymbolTable{{
        {"A", {"", "a", "b"}},
        {"B", {"", "a", "b"}},
        {"Start", {"", "a", "b"}},
        {"a", {"a"}},
        {"b", {"b"}},
        {"", {""}}
    }});

    REQUIRE(getFollowTable(rules, firstTable) == SymbolTable{{
        {"a", {endOfString, "b"}},
        {"b", {"a"}},
        {"B", {"a"}},
        {"A", {endOfString, "b"}},
        {"Start", {endOfString}}
    }});
}
// clang-format on
