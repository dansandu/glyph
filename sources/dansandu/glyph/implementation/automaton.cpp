#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

using dansandu::ballotin::container::setInsert;
using dansandu::ballotin::string::format;
using dansandu::glyph::implementation::grammar::GrammarError;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::grammar::startSymbol;

namespace dansandu::glyph::implementation::automaton {

bool operator==(Item left, Item right) { return left.ruleIndex == right.ruleIndex && left.position == right.position; }

bool operator!=(Item left, Item right) { return !(left == right); }

bool operator<(Item left, Item right) {
    return left.ruleIndex < right.ruleIndex || (left.ruleIndex == right.ruleIndex && left.position < right.position);
}

std::ostream& operator<<(std::ostream& stream, Item item) {
    return stream << "Item(" << item.ruleIndex << ", " << item.position << ")";
}

bool operator==(const Transition& left, const Transition& right) {
    return left.symbol == right.symbol && left.from == right.from && left.to == right.to;
}

bool operator!=(const Transition& left, const Transition& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Transition& transition) {
    return stream << "Transition(" << transition.symbol << ", " << transition.from << ", " << transition.to << ")";
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

std::vector<Item> getClosure(std::vector<Item> items, const std::vector<Rule>& rules) {
    for (auto itemIndex = 0; itemIndex < static_cast<int>(items.size()); ++itemIndex) {
        auto item = items[itemIndex];
        if (item.position < static_cast<int>(rules.at(item.ruleIndex).rightSide.size())) {
            const auto& symbol = rules[item.ruleIndex].rightSide[item.position];
            for (auto ruleIndex = 0; ruleIndex < static_cast<int>(rules.size()); ++ruleIndex) {
                auto newItem = Item{ruleIndex, 0};
                if (symbol == rules[ruleIndex].leftSide &&
                    std::find(items.cbegin(), items.cend(), newItem) == items.end())
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
            setInsert(transitions[symbol], Item{item.ruleIndex, item.position + 1});
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

Automaton getAutomaton(const std::vector<Rule>& rules) {
    auto startRuleIndex = getStartRuleIndex(rules);
    auto finalStateIndex = -1;
    auto states = std::vector<std::vector<Item>>{{getClosure({Item{startRuleIndex, 0}}, rules)}};
    auto transitions = std::vector<Transition>{};
    for (auto stateIndex = 0; stateIndex < static_cast<int>(states.size()); ++stateIndex) {
        for (auto& transition : getTransitions(states[stateIndex], rules)) {
            auto newState = getClosure(std::move(transition.second), rules);
            auto newStatePosition = std::find(states.cbegin(), states.cend(), newState);
            auto newStateIndex = static_cast<int>(newStatePosition - states.cbegin());
            if (newStatePosition == states.cend())
                states.push_back(std::move(newState));
            transitions.push_back({transition.first, stateIndex, newStateIndex});
        }
        if (isFinalState(states[stateIndex], rules, startRuleIndex)) {
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
