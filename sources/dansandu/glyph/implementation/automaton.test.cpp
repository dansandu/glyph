#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <string>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::implementation::automaton::operator<<;
using dansandu::glyph::implementation::grammar::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::automaton::getClosure;
using dansandu::glyph::implementation::automaton::getFollowSet;
using dansandu::glyph::implementation::automaton::getStartRuleIndex;
using dansandu::glyph::implementation::automaton::getTransitions;
using dansandu::glyph::implementation::automaton::isFinalState;
using dansandu::glyph::implementation::automaton::Item;
using dansandu::glyph::implementation::automaton::Transition;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::grammar::Rule;

using Items = std::vector<Item>;
using Transitions = std::map<std::string, std::vector<Item>>;

// clang-format off
TEST_CASE("Automaton") {
    constexpr auto startRuleIndex = 0;
    auto grammar = Grammar{"Start -> Sums                        \n"
                           "Sums -> Sums add Products            \n"
                           "Sums -> Products                     \n"
                           "Products -> Products multiply number \n"
                           "Products -> number"};    
    const auto& rules = grammar.getRules();
    auto firstTable = getFirstTable(grammar);

    SECTION("follow set") {
        REQUIRE(getFollowSet({1, 0, endOfString}, rules, firstTable) == std::vector<std::string>{"add"});

        REQUIRE(getFollowSet({2, 0, endOfString}, rules, firstTable) == std::vector<std::string>{endOfString});

        REQUIRE(getFollowSet({4, 1, "multiply"}, rules, firstTable) == std::vector<std::string>{"multiply"});
    }

    SECTION("start rule index") {
        REQUIRE(getStartRuleIndex(rules) == startRuleIndex);
        
        auto multipleStartRulesGrammar = Grammar{"Start -> number \n Start -> identifier"};
        REQUIRE_THROWS_AS(getStartRuleIndex(multipleStartRulesGrammar.getRules()), GrammarError);

        auto noStartRuleGrammar = Grammar{"S -> number \n S -> identifier"};
        REQUIRE_THROWS_AS(getStartRuleIndex(noStartRuleGrammar.getRules()), GrammarError);
    }

    SECTION("closure") {
        REQUIRE(getClosure({}, rules, firstTable).empty());

        REQUIRE(getClosure({ {0, 0, endOfString} }, rules, firstTable) == Items{{
            Item{0, 0, "$"},
            Item{1, 0, "$"},
            Item{1, 0, "add"},
            Item{2, 0, "$"},
            Item{2, 0, "add"},
            Item{3, 0, "$"},
            Item{3, 0, "add"},
            Item{3, 0, "multiply"},
            Item{4, 0, "$"},
            Item{4, 0, "add"},
            Item{4, 0, "multiply"}}
        });

        REQUIRE(getClosure({ {0, 1, endOfString} }, rules, firstTable) == Items{{ {0, 1, endOfString} }});

        REQUIRE(getClosure({ {1, 1, endOfString} }, rules, firstTable) == Items{{ {1, 1, endOfString} }});
    }

    SECTION("transitions") {
        REQUIRE(getTransitions({}, rules).empty());

        REQUIRE(getTransitions({ Item{0, 0, endOfString}, Item{1, 1, "multiply"}, Item{2, 0, "add"}, Item{3, 0, "add"} }, rules) == Transitions{{
            {"add", { Item{1, 2, "multiply"} }},
            {"Products", { Item{2, 1, "add"}, Item{3, 1, "add"} }},
            {"Sums", { Item{0, 1, endOfString} }}
        }});

        REQUIRE(getTransitions({ Item{0, 1, endOfString} }, rules) == Transitions{});
    }

    SECTION("final state") {
        REQUIRE(isFinalState({ {0, 1, endOfString}, {1, 1, endOfString} }, rules, startRuleIndex));

        REQUIRE(!isFinalState({ {0, 0, endOfString}, {1, 1, endOfString}, {2, 0, endOfString} }, rules, startRuleIndex));
    }

    SECTION("generate automaton") {
        auto automaton = getAutomaton(grammar);

        REQUIRE(automaton.states == std::vector<Items>{{
            Items{{Item{0, 0, "$"},
                   Item{1, 0, "$"},
                   Item{1, 0, "add"},
                   Item{2, 0, "$"},
                   Item{2, 0, "add"},
                   Item{3, 0, "$"},
                   Item{3, 0, "add"},
                   Item{3, 0, "multiply"},
                   Item{4, 0, "$"},
                   Item{4, 0, "add"},
                   Item{4, 0, "multiply"}}},

            Items{{Item{2, 1, "$"},
                   Item{2, 1, "add"},
                   Item{3, 1, "$"},
                   Item{3, 1, "add"},
                   Item{3, 1, "multiply"}}},
    
            Items{{Item{0, 1, "$"},
                   Item{1, 1, "$"},
                   Item{1, 1, "add"}}},
    
            Items{{Item{4, 1, "$"},
                   Item{4, 1, "add"},
                   Item{4, 1, "multiply"}}},
    
            Items{{Item{3, 2, "$"},
                   Item{3, 2, "add"},
                   Item{3, 2, "multiply"}}},
    
            Items{{Item{1, 2, "$"},
                   Item{1, 2, "add"},
                   Item{3, 0, "$"},
                   Item{3, 0, "add"},
                   Item{3, 0, "multiply"},
                   Item{4, 0, "$"},
                   Item{4, 0, "add"},
                   Item{4, 0, "multiply"}}},
    
            Items{{Item{3, 3, "$"},
                   Item{3, 3, "add"},
                   Item{3, 3, "multiply"}}},
    
            Items{{Item{1, 3, "$"},
                   Item{1, 3, "add"},
                   Item{3, 1, "$"},
                   Item{3, 1, "add"},
                   Item{3, 1, "multiply"}}}
        }});

        REQUIRE(automaton.transitions == std::vector<Transition>{{
            Transition{"Products", 0, 1},
            Transition{"Sums", 0, 2},
            Transition{"number", 0, 3},
            Transition{"multiply", 1, 4},
            Transition{"add", 2, 5},
            Transition{"number", 4, 6},
            Transition{"Products", 5, 7},
            Transition{"number", 5, 3},
            Transition{"multiply", 7, 4}
        }});
    }
}
// clang-format on
