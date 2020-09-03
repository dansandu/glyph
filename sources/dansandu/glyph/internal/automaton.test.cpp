#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/internal/grammar.hpp"

#include <set>
#include <vector>

using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::automaton::getAutomaton;
using dansandu::glyph::internal::automaton::getFollowSet;
using dansandu::glyph::internal::automaton::getStateClosure;
using dansandu::glyph::internal::automaton::getStateTransitions;
using dansandu::glyph::internal::automaton::isFinalState;
using dansandu::glyph::internal::automaton::Transition;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::item::Item;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

using Items = std::vector<Item>;
using Transitions = std::map<Symbol, std::vector<Item>>;

static std::set<Symbol> set(const std::vector<Symbol>& l)
{
    return std::set<Symbol>{l.cbegin(), l.cend()};
}

static std::set<Symbol> set(std::initializer_list<Symbol> l)
{
    return std::set<Symbol>{l.begin(), l.end()};
}

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

    SECTION("follow set")
    {
        REQUIRE(set(getFollowSet(Item{0, 0, end}, grammar)) == set({end}));

        REQUIRE(set(getFollowSet(Item{1, 0, end}, grammar)) == set({add}));

        REQUIRE(set(getFollowSet(Item{2, 0, end}, grammar)) == set({end}));

        REQUIRE(set(getFollowSet(Item{3, 0, end}, grammar)) == set({multiply}));

        REQUIRE(set(getFollowSet(Item{4, 1, multiply}, grammar)) == set({multiply}));
    }

    SECTION("closure")
    {
        REQUIRE(getStateClosure({}, grammar).empty());

        REQUIRE(getStateClosure({Item{0, 0, end}}, grammar) == Items{{
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
        });

        REQUIRE(getStateClosure({Item{0, 1, end}}, grammar) == Items{Item{0, 1, end}});

        REQUIRE(getStateClosure({Item{1, 1, end}}, grammar) == Items{Item{1, 1, end}});
    }

    SECTION("transitions")
    {
        REQUIRE(getStateTransitions({}, grammar).empty());

        REQUIRE(getStateTransitions({Item{0, 1, end}}, grammar) == Transitions{});

        const auto state = Items{
            Item{0, 0, end},
            Item{1, 1, multiply},
            Item{2, 0, add},
            Item{3, 0, add}
        };

        const auto transitions = Transitions{
            {add,      {Item{1, 2, multiply}}},
            {Products, {Item{2, 1, add}, Item{3, 1, add}}},
            {Sums,     {Item{0, 1, end}}}
        };

        REQUIRE(getStateTransitions(state, grammar) == transitions);
    }

    SECTION("final state")
    {
        REQUIRE(isFinalState({Item{0, 1, end}, Item{1, 1, end}}, grammar));

        REQUIRE(!isFinalState({Item{0, 0, end}, Item{1, 1, end}, Item{2, 0, end}}, grammar));
    }

    SECTION("automaton") {
        const auto automaton = getAutomaton(grammar);

        REQUIRE(automaton.states == std::vector<Items>{
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
        });

        REQUIRE(automaton.transitions == std::vector<Transition>{
            Transition{Sums,     0, 1},
            Transition{Products, 0, 2},
            Transition{number,   0, 3},
            Transition{add,      1, 4},
            Transition{multiply, 2, 5},
            Transition{Products, 4, 6},
            Transition{number,   4, 3},
            Transition{number,   5, 7},
            Transition{multiply, 6, 5}
        });
    }
}
// clang-format on
