#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"

using dansandu::ballotin::container::contains;
using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::item::Item;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

namespace dansandu::glyph::internal::automaton
{

std::ostream& operator<<(std::ostream& stream, const Transition& transition)
{
    return stream << "Transition(" << transition.symbol << ", " << transition.from << ", " << transition.to << ")";
}

std::vector<Symbol> getFollowSet(const Item& item, const Grammar& grammar)
{
    auto followSet = std::vector<Symbol>{};
    auto followIndex = item.position + 1;
    const auto& rule = grammar.getRules()[item.ruleIndex];
    while (followIndex < static_cast<int>(rule.rightSide.size()))
    {
        const auto& firstSet = grammar.getFirstSet(rule.rightSide[followIndex]);
        const auto emptyPosition = std::find(firstSet.cbegin(), firstSet.cend(), grammar.getEmptySymbol());
        for (auto symbolPosition = firstSet.cbegin(); symbolPosition != firstSet.cend(); ++symbolPosition)
        {
            if (symbolPosition != emptyPosition && !contains(followSet, *symbolPosition))
            {
                followSet.push_back(*symbolPosition);
            }
        }
        if (emptyPosition == firstSet.cend())
        {
            break;
        }
        ++followIndex;
    }
    if (followIndex >= static_cast<int>(rule.rightSide.size()))
    {
        followSet.push_back(item.lookahead);
    }
    return followSet;
}

std::vector<Item> getStateClosure(std::vector<Item> state, const Grammar& grammar)
{
    const auto& rules = grammar.getRules();
    for (auto parentItemIndex = 0; parentItemIndex < static_cast<int>(state.size()); ++parentItemIndex)
    {
        const auto parentItem = state[parentItemIndex];
        const auto parentRule = rules[parentItem.ruleIndex];
        const auto parentItemFollowSet = getFollowSet(parentItem, grammar);
        if (parentItem.position < static_cast<int>(parentRule.rightSide.size()))
        {
            const auto symbol = parentRule.rightSide[parentItem.position];
            for (auto ruleIndex = 0; ruleIndex < static_cast<int>(rules.size()); ++ruleIndex)
            {
                if (symbol == rules[ruleIndex].leftSide)
                {
                    for (const auto followSymbol : parentItemFollowSet)
                    {
                        const auto newItem = Item{ruleIndex, 0, followSymbol};
                        if (!contains(state, newItem))
                        {
                            state.push_back(newItem);
                        }
                    }
                }
            }
        }
    }
    std::sort(state.begin(), state.end());
    return state;
}

std::map<Symbol, std::vector<Item>> getStateTransitions(const std::vector<Item>& fromState, const Grammar& grammar)
{
    auto transitions = std::map<Symbol, std::vector<Item>>{};
    const auto& rules = grammar.getRules();
    for (const auto& item : fromState)
    {
        if (item.position < static_cast<int>(rules[item.ruleIndex].rightSide.size()))
        {
            const auto symbol = rules[item.ruleIndex].rightSide[item.position];
            const auto newItem = Item{item.ruleIndex, item.position + 1, item.lookahead};
            auto& toState = transitions[symbol];
            if (!contains(toState, newItem))
            {
                toState.push_back(newItem);
            }
        }
    }
    return transitions;
}

bool isFinalState(const std::vector<Item>& state, const Grammar& grammar)
{
    const auto startRuleIndex = grammar.getStartRuleIndex();
    const auto& startRule = grammar.getRules()[startRuleIndex];
    for (const auto& item : state)
    {
        if (item.ruleIndex == startRuleIndex && item.position == static_cast<int>(startRule.rightSide.size()))
        {
            return true;
        }
    }
    return false;
}

Automaton getAutomaton(const Grammar& grammar)
{
    const auto startRuleIndex = grammar.getStartRuleIndex();
    auto finalStateIndex = -1;
    auto states = std::vector<std::vector<Item>>{
        getStateClosure({Item{startRuleIndex, 0, grammar.getEndOfStringSymbol()}}, grammar)};
    auto transitions = std::vector<Transition>{};
    for (auto stateIndex = 0; stateIndex < static_cast<int>(states.size()); ++stateIndex)
    {
        for (auto& transition : getStateTransitions(states[stateIndex], grammar))
        {
            auto newState = getStateClosure(std::move(transition.second), grammar);
            const auto newStatePosition = std::find(states.cbegin(), states.cend(), newState);
            const auto newStateIndex = static_cast<int>(newStatePosition - states.cbegin());
            if (newStatePosition == states.cend())
            {
                states.push_back(std::move(newState));
            }
            transitions.push_back({transition.first, stateIndex, newStateIndex});
        }
        if (isFinalState(states[stateIndex], grammar))
        {
            if (finalStateIndex == -1)
            {
                finalStateIndex = stateIndex;
            }
            else
            {
                THROW(GrammarError, "multiple final states found with indices ", stateIndex, " and ", finalStateIndex);
            }
        }
    }
    if (finalStateIndex == -1)
    {
        THROW(GrammarError, "no final state was found");
    }
    return {std::move(states), std::move(transitions), finalStateIndex};
}

}
