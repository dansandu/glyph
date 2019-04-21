#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <string>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::implementation::automaton::operator<<;
using dansandu::glyph::implementation::grammar::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::automaton::getClosure;
using dansandu::glyph::implementation::automaton::getStartRuleIndex;
using dansandu::glyph::implementation::automaton::getTransitions;
using dansandu::glyph::implementation::automaton::isFinalState;
using dansandu::glyph::implementation::automaton::Item;
using dansandu::glyph::implementation::automaton::Transition;
using dansandu::glyph::implementation::grammar::getRules;
using dansandu::glyph::implementation::grammar::GrammarError;
using dansandu::glyph::implementation::grammar::Rule;

// clang-format off
TEST_CASE("Automaton") {
    constexpr auto grammar = "Start -> Sums                        \n"
                             "Sums -> Sums add Products            \n"
                             "Sums -> Products                     \n"
                             "Products -> Products multiply number \n"
                             "Products -> number";
    constexpr auto startRuleIndex = 0;
    const auto rules = getRules(grammar);

    SECTION("start item") {
        REQUIRE(getStartRuleIndex(rules) == startRuleIndex);
        
        REQUIRE_THROWS_AS(getStartRuleIndex(getRules("Start -> number \n Start -> identifier")), GrammarError);

        REQUIRE_THROWS_AS(getStartRuleIndex(getRules("S -> number \n S -> identifier")), GrammarError);
    }

    SECTION("closure") {
        using Items = std::vector<Item>;

        REQUIRE(getClosure({}, rules).empty());

        REQUIRE(getClosure({ {0, 0} }, rules) == Items{{ {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0} }});

        REQUIRE(getClosure({ {0, 1} }, rules) == Items{{ {0, 1} }});

        REQUIRE(getClosure({ {1, 1} }, rules) == Items{{ {1, 1} }});

        REQUIRE(getClosure({ {1, 2} }, rules) == Items{{ {1, 2}, {3, 0}, {4, 0} }});

        REQUIRE(getClosure({ {2, 0}, {4, 0} }, rules) == Items{{ {2, 0}, {3, 0}, {4, 0} }});
    }

    SECTION("transitions") {
        using Transitions = std::map<std::string, std::vector<Item>>;
        
        REQUIRE(getTransitions({}, rules).empty());

        REQUIRE(getTransitions({ {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0} }, rules) == Transitions{{
            {"Sums", { {0, 1}, {1, 1} }},
            {"Products", { {2, 1}, {3, 1} }},
            {"number", { {4, 1} }}
        }});

        REQUIRE(getTransitions({ {0, 1}, {1, 1} }, rules) == Transitions{{
            {"add", { {1, 2} } }
        }});
    }

    SECTION("final state") {
        REQUIRE(isFinalState({ {0, 1}, {1, 1} }, rules, startRuleIndex));

        REQUIRE(!isFinalState({ {0, 0}, {1, 1}, {2, 0} }, rules, startRuleIndex));
    }

    SECTION("generate automaton") {
        auto automaton = getAutomaton(rules);

        REQUIRE(automaton.states == std::vector<std::vector<Item>>{{
            { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0} },
            { {2, 1}, {3, 1} },
            { {0, 1}, {1, 1} },
            { {4, 1} },
            { {3, 2} },
            { {1, 2}, {3, 0}, {4, 0} },
            { {3, 3} },
            { {1, 3}, {3, 1} }
        }});

        REQUIRE(automaton.transitions == std::vector<Transition>{{
            {"Products", 0, 1},
            {"Sums", 0, 2},
            {"number", 0, 3},
            {"multiply", 1, 4},
            {"add", 2, 5},
            {"number", 4, 6},
            {"Products", 5, 7},
            {"number", 5, 3},
            {"multiply", 7, 4}
        }});

        REQUIRE(automaton.startRuleIndex == startRuleIndex);
        
        REQUIRE(automaton.finalStateIndex == 2);
    }
}
// clang-format on
