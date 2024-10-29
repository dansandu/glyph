#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/first_table.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <map>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::automaton::getAutomaton;
using dansandu::glyph::internal::automaton::getFollowSet;
using dansandu::glyph::internal::automaton::getStateClosure;
using dansandu::glyph::internal::automaton::getStateTransitions;
using dansandu::glyph::internal::automaton::isFinalState;
using dansandu::glyph::internal::automaton::Transition;
using dansandu::glyph::internal::first_table::getFirstTable;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::item::Item;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

using Items = std::vector<Item>;
using Transitions = std::map<Symbol, std::vector<Item>>;

// clang-format off
TEST_CASE("Automaton")
{
    const auto grammar = Grammar{R"(
        Start    -> Sums
        Sums     -> Sums add Products
        Sums     -> Products
        Products -> Products multiply number
        Products -> number
    )"};

    const auto Sums     = grammar.getSymbol("Sums");
    const auto Products = grammar.getSymbol("Products");
    const auto end      = grammar.getSymbol("$");
    const auto add      = grammar.getSymbol("add");
    const auto multiply = grammar.getSymbol("multiply");
    const auto number   = grammar.getSymbol("number");

    const auto firstTable = getFirstTable(grammar);

    SECTION("closure")
    {
        SECTION("closure case #1")
        {
            const auto closure = getStateClosure({}, grammar, firstTable);

            REQUIRE(closure.empty());
        }

        SECTION("closure case #2")
        {
            const auto closure = getStateClosure({Item{0, 0, end}}, grammar, firstTable);

            const auto expectedClosure = Items{{
                Item{0, 0, end},
                Item{1, 0, end},
                Item{1, 0, add},
                Item{2, 0, end},
                Item{2, 0, add},
                Item{3, 0, end},
                Item{3, 0, add},
                Item{3, 0, multiply},
                Item{4, 0, end},
                Item{4, 0, add},
                Item{4, 0, multiply}}
            };

            REQUIRE(closure == expectedClosure);
        }

        SECTION("closure case #3")
        {
            const auto closure = getStateClosure({Item{0, 1, end}}, grammar, firstTable);

            const auto expectedClosure = Items{Item{0, 1, end}};

            REQUIRE(closure == expectedClosure);
        }

        SECTION("closure case #4")
        {
            const auto closure = getStateClosure({Item{1, 1, end}}, grammar, firstTable);

            const auto expectedClosure = Items{Item{1, 1, end}};

            REQUIRE(closure == expectedClosure);
        }
    }

    SECTION("transitions")
    {
        SECTION("transitions case #1")
        {
            const auto transitions = getStateTransitions({}, grammar);
            
            REQUIRE(transitions.empty());
        }

        SECTION("transitions case #2")
        {
            const auto transitions = getStateTransitions({Item{0, 1, end}}, grammar);

            REQUIRE(transitions.empty());
        }

        SECTION("transitions case #3")
        {
            const auto state = Items{
                Item{0, 0, end},
                Item{1, 1, multiply},
                Item{2, 0, add},
                Item{3, 0, add}
            };

            const auto expectedTransitions = Transitions{
                {add,      {Item{1, 2, multiply}}},
                {Products, {Item{2, 1, add}, Item{3, 1, add}}},
                {Sums,     {Item{0, 1, end}}}
            };

            const auto transitions = getStateTransitions(state, grammar);

            REQUIRE(transitions == expectedTransitions);
        }
    }

    SECTION("final state")
    {
        SECTION("final state case #3")
        {
            const auto finalState = isFinalState({Item{0, 1, end}, Item{1, 1, end}}, grammar);

            REQUIRE(finalState);
        }

        SECTION("final state case #3")
        {
            const auto finalState = !isFinalState({Item{0, 0, end}, Item{1, 1, end}, Item{2, 0, end}}, grammar);

            REQUIRE(finalState);
        }
    }

    SECTION("automaton") 
    {
        const auto automaton = getAutomaton(grammar);

        const auto expectedStates = std::vector<Items>{
            Items{Item{0, 0, end},
                  Item{1, 0, end},
                  Item{1, 0, add},
                  Item{2, 0, end},
                  Item{2, 0, add},
                  Item{3, 0, end},
                  Item{3, 0, add},
                  Item{3, 0, multiply},
                  Item{4, 0, end},
                  Item{4, 0, add},
                  Item{4, 0, multiply}},

            Items{Item{0, 1, end},
                  Item{1, 1, end},
                  Item{1, 1, add}},
    
            Items{Item{2, 1, end},
                  Item{2, 1, add},
                  Item{3, 1, end},
                  Item{3, 1, add},
                  Item{3, 1, multiply}},
    
            Items{Item{4, 1, end},
                  Item{4, 1, add},
                  Item{4, 1, multiply}},
        
            Items{Item{1, 2, end},
                  Item{1, 2, add},
                  Item{3, 0, end},
                  Item{3, 0, add},
                  Item{3, 0, multiply},
                  Item{4, 0, end},
                  Item{4, 0, add},
                  Item{4, 0, multiply}},

            Items{Item{3, 2, end},
                  Item{3, 2, add},
                  Item{3, 2, multiply}},
    
            Items{Item{1, 3, end},
                  Item{1, 3, add},
                  Item{3, 1, end},
                  Item{3, 1, add},
                  Item{3, 1, multiply}},

            Items{Item{3, 3, end},
                  Item{3, 3, add},
                  Item{3, 3, multiply}}
        };

        REQUIRE(automaton.states == expectedStates);

        const auto expectedTransitions = std::vector<Transition>{
            Transition{Sums,     0, 1},
            Transition{Products, 0, 2},
            Transition{number,   0, 3},
            Transition{add,      1, 4},
            Transition{multiply, 2, 5},
            Transition{Products, 4, 6},
            Transition{number,   4, 3},
            Transition{number,   5, 7},
            Transition{multiply, 6, 5}
        };

        REQUIRE(automaton.transitions == expectedTransitions);
    }
}
// clang-format on
