#pragma once

#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/item.hpp"
#include "dansandu/glyph/internal/rule.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <map>
#include <ostream>
#include <vector>

namespace dansandu::glyph::internal::automaton
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

std::vector<dansandu::glyph::symbol::Symbol> getFollowSet(const dansandu::glyph::internal::item::Item& item,
                                                          const dansandu::glyph::internal::grammar::Grammar& grammar);

std::vector<dansandu::glyph::internal::item::Item>
getStateClosure(std::vector<dansandu::glyph::internal::item::Item> state,
                const dansandu::glyph::internal::grammar::Grammar& grammar);

std::map<dansandu::glyph::symbol::Symbol, std::vector<dansandu::glyph::internal::item::Item>>
getStateTransitions(const std::vector<dansandu::glyph::internal::item::Item>& state,
                    const dansandu::glyph::internal::grammar::Grammar& grammar);

bool isFinalState(const std::vector<dansandu::glyph::internal::item::Item>& state,
                  const dansandu::glyph::internal::grammar::Grammar& grammar);

struct Automaton
{
    std::vector<std::vector<dansandu::glyph::internal::item::Item>> states;
    std::vector<Transition> transitions;
    int finalStateIndex;
};

Automaton getAutomaton(const dansandu::glyph::internal::grammar::Grammar& grammar);

}
