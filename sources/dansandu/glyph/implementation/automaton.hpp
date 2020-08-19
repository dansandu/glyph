#pragma once

#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/item.hpp"
#include "dansandu/glyph/implementation/rule.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <map>
#include <ostream>
#include <vector>

namespace dansandu::glyph::implementation::automaton
{

struct Transition
{
    dansandu::glyph::symbol::Symbol symbol;
    int from;
    int to;
};

bool operator==(const Transition& left, const Transition& right);

bool operator!=(const Transition& left, const Transition& right);

std::ostream& operator<<(std::ostream& stream, const Transition& transition);

std::vector<dansandu::glyph::symbol::Symbol>
getFollowSet(const dansandu::glyph::implementation::item::Item& item,
             const dansandu::glyph::implementation::grammar::Grammar& grammar);

std::vector<dansandu::glyph::implementation::item::Item>
getStateClosure(std::vector<dansandu::glyph::implementation::item::Item> state,
                const dansandu::glyph::implementation::grammar::Grammar& grammar);

std::map<dansandu::glyph::symbol::Symbol, std::vector<dansandu::glyph::implementation::item::Item>>
getStateTransitions(const std::vector<dansandu::glyph::implementation::item::Item>& state,
                    const dansandu::glyph::implementation::grammar::Grammar& grammar);

bool isFinalState(const std::vector<dansandu::glyph::implementation::item::Item>& state,
                  const dansandu::glyph::implementation::grammar::Grammar& grammar);

struct Automaton
{
    std::vector<std::vector<dansandu::glyph::implementation::item::Item>> states;
    std::vector<Transition> transitions;
    int finalStateIndex;
};

Automaton getAutomaton(const dansandu::glyph::implementation::grammar::Grammar& grammar);

}
