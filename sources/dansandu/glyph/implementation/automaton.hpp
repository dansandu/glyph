#pragma once
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::automaton {

struct Item {
    int ruleIndex;
    int position;
    std::string lookahead;
};

bool operator==(const Item& left, const Item& right);

bool operator!=(const Item& left, const Item& right);

bool operator<(const Item& left, const Item& right);

std::ostream& operator<<(std::ostream& stream, const Item& item);

struct Transition {
    std::string symbol;
    int from;
    int to;
};

bool operator==(const Transition& left, const Transition& right);

bool operator!=(const Transition& left, const Transition& right);

std::ostream& operator<<(std::ostream& stream, const Transition& transition);

std::vector<std::string> getFollowSet(const Item& item,
                                      const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules,
                                      const dansandu::glyph::implementation::grammar::SymbolTable& firstTable);

int getStartRuleIndex(const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules);

std::vector<Item> getClosure(std::vector<Item> items,
                             const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules,
                             const dansandu::glyph::implementation::grammar::SymbolTable& firstTable);

std::map<std::string, std::vector<Item>>
getTransitions(const std::vector<Item>& items,
               const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules);

bool isFinalState(const std::vector<Item>& items,
                  const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules, int startRuleIndex);

struct Automaton {
    int startRuleIndex;
    int finalStateIndex;
    std::vector<std::vector<Item>> states;
    std::vector<Transition> transitions;
};

Automaton getAutomaton(const dansandu::glyph::implementation::grammar::Grammar& grammar);

}
