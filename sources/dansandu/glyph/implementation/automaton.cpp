#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using dansandu::ballotin::container::setInsert;
using dansandu::ballotin::container::setUnion;
using dansandu::ballotin::string::format;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::getFirstTable;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::grammar::GrammarError;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::grammar::startSymbol;
using dansandu::glyph::implementation::grammar::SymbolTable;

namespace dansandu::glyph::implementation::automaton {

bool operator==(const Item& left, const Item& right) {
    return std::tie(left.ruleIndex, left.position, left.lookahead) ==
           std::tie(right.ruleIndex, right.position, right.lookahead);
}

bool operator!=(const Item& left, const Item& right) { return !(left == right); }

bool operator<(const Item& left, const Item& right) {
    return std::tie(left.ruleIndex, left.position, left.lookahead) <
           std::tie(right.ruleIndex, right.position, right.lookahead);
}

std::ostream& operator<<(std::ostream& stream, const Item& item) {
    return stream << "Item(" << item.ruleIndex << ", " << item.position << ", " << item.lookahead << ")";
}

bool operator==(const Transition& left, const Transition& right) {
    return left.symbol == right.symbol && left.from == right.from && left.to == right.to;
}

bool operator!=(const Transition& left, const Transition& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Transition& transition) {
    return stream << "Transition(" << transition.symbol << ", " << transition.from << ", " << transition.to << ")";
}

std::vector<std::string> getFollowSet(const Item& item, const std::vector<Rule>& rules, const SymbolTable& firstTable) {
    auto followSet = std::vector<std::string>{};
    const auto& rule = rules.at(item.ruleIndex);
    auto follow = item.position + 1;
    for (; follow < static_cast<int>(rule.rightSide.size()); ++follow) {
        const auto& firstSet = firstTable.at(rule.rightSide[follow]);
        if (auto empty = std::find(firstSet.cbegin(), firstSet.cend(), ""); empty != firstSet.cend()) {
            auto noEmpty = std::vector<std::string>{firstSet.cbegin(), empty};
            noEmpty.insert(noEmpty.end(), empty + 1, firstSet.cend());
            followSet = setUnion(followSet, noEmpty);
        } else {
            followSet = setUnion(followSet, firstSet);
            break;
        }
    }
    if (follow >= static_cast<int>(rule.rightSide.size()))
        followSet.push_back(item.lookahead);
    return followSet;
}

int getStartRuleIndex(const std::vector<Rule>& rules) {
    int startRuleIndex = -1;
    for (auto ruleIndex = 0; ruleIndex < static_cast<int>(rules.size()); ++ruleIndex)
        if (startSymbol == rules[ruleIndex].leftSide) {
            if (startRuleIndex == -1)
                startRuleIndex = ruleIndex;
            else
                THROW(GrammarError, "multiple start production rules with indices ", startRuleIndex, " and ", ruleIndex,
                      " were defined -- there can only be one");
        }
    if (startRuleIndex == -1)
        THROW(GrammarError, "no start production rule was found");
    return startRuleIndex;
}

std::vector<Item> getClosure(std::vector<Item> items, const std::vector<Rule>& rules, const SymbolTable& firstTable) {
    for (auto parentIndex = 0; parentIndex < static_cast<int>(items.size()); ++parentIndex) {
        auto parentItem = items[parentIndex];
        auto parentRule = rules.at(parentItem.ruleIndex);
        if (parentItem.position < static_cast<int>(parentRule.rightSide.size())) {
            const auto& symbol = parentRule.rightSide[parentItem.position];
            for (auto ruleIndex = 0; ruleIndex < static_cast<int>(rules.size()); ++ruleIndex)
                if (symbol == rules[ruleIndex].leftSide)
                    for (auto& follow : getFollowSet(items.at(parentIndex), rules, firstTable)) {
                        auto newItem = Item{ruleIndex, 0, std::move(follow)};
                        if (std::find(items.cbegin(), items.cend(), newItem) == items.end())
                            items.push_back(newItem);
                    }
        }
    }
    std::sort(items.begin(), items.end());
    return items;
}

std::map<std::string, std::vector<Item>> getTransitions(const std::vector<Item>& fromItems,
                                                        const std::vector<Rule>& rules) {
    auto transitions = std::map<std::string, std::vector<Item>>{};
    for (const auto& item : fromItems)
        if (item.position < static_cast<int>(rules.at(item.ruleIndex).rightSide.size())) {
            const auto& symbol = rules[item.ruleIndex].rightSide[item.position];
            setInsert(transitions[symbol], Item{item.ruleIndex, item.position + 1, item.lookahead});
        }
    return transitions;
}

bool isFinalState(const std::vector<Item>& items, const std::vector<Rule>& rules, int startRuleIndex) {
    for (const auto& item : items)
        if (item.ruleIndex == startRuleIndex &&
            item.position == static_cast<int>(rules.at(startRuleIndex).rightSide.size()))
            return true;
    return false;
}

Automaton getAutomaton(const Grammar& grammar) {
    auto startRuleIndex = getStartRuleIndex(grammar.getRules());
    auto finalStateIndex = -1;
    auto firstTable = getFirstTable(grammar);
    auto states = std::vector<std::vector<Item>>{
        {getClosure({Item{startRuleIndex, 0, endOfString}}, grammar.getRules(), firstTable)}};
    auto transitions = std::vector<Transition>{};
    for (auto stateIndex = 0; stateIndex < static_cast<int>(states.size()); ++stateIndex) {
        for (auto& transition : getTransitions(states[stateIndex], grammar.getRules())) {
            auto newState = getClosure(std::move(transition.second), grammar.getRules(), firstTable);
            auto newStatePosition = std::find(states.cbegin(), states.cend(), newState);
            auto newStateIndex = static_cast<int>(newStatePosition - states.cbegin());
            if (newStatePosition == states.cend())
                states.push_back(std::move(newState));
            transitions.push_back({transition.first, stateIndex, newStateIndex});
        }
        if (isFinalState(states[stateIndex], grammar.getRules(), startRuleIndex)) {
            if (finalStateIndex == -1)
                finalStateIndex = stateIndex;
            else
                THROW(GrammarError, "multiple final states found with indices ", stateIndex, " and ", finalStateIndex);
        }
    }
    if (finalStateIndex == -1)
        THROW(std::runtime_error, "no final state was found");
    return {startRuleIndex, finalStateIndex, std::move(states), std::move(transitions)};
}

}
