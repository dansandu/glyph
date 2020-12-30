#include "dansandu/glyph/internal/parsing_table.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/internal/grammar.hpp"

#include <ostream>
#include <stdexcept>
#include <vector>

using dansandu::glyph::internal::automaton::Automaton;
using dansandu::glyph::internal::grammar::Grammar;

namespace dansandu::glyph::internal::parsing_table
{

std::ostream& operator<<(std::ostream& stream, const Action action)
{
    switch (action)
    {
    case Action::shift:
        return stream << "shift";
    case Action::reduce:
        return stream << "reduce";
    case Action::goTo:
        return stream << "goTo";
    case Action::accept:
        return stream << "accept";
    case Action::error:
        return stream << "error";
    default:
        THROW(std::logic_error, "unrecognized cell action");
    }
}

std::ostream& operator<<(std::ostream& stream, const Cell cell)
{
    return stream << "Cell(" << cell.action << ", " << cell.parameter << ")";
}

std::vector<std::vector<Cell>> getClr1ParsingTable(const Grammar& grammar, const Automaton& automaton)
{
    auto table = std::vector<std::vector<Cell>>(grammar.getIdentifiersCount());
    for (auto& row : table)
    {
        row = std::vector<Cell>{automaton.states.size()};
    }
    for (const auto& transition : automaton.transitions)
    {
        const auto action = grammar.isTerminal(transition.symbol) ? Action::shift : Action::goTo;
        table[transition.symbol.getIdentifierIndex()][transition.from] = Cell{action, transition.to};
    }
    const auto& rules = grammar.getRules();
    for (auto stateIndex = 0U; stateIndex < automaton.states.size(); ++stateIndex)
    {
        for (const auto& item : automaton.states[stateIndex])
        {
            if (item.position == static_cast<int>(rules[item.ruleIndex].rightSide.size()))
            {
                auto& cell = table[item.lookahead.getIdentifierIndex()][stateIndex];
                if (cell.action != Action::error)
                {
                    THROW(std::logic_error, "grammar cannot be parsed using a CLR(1) parser due to ", cell.action,
                          "/reduce conflict");
                }
                cell = Cell{Action::reduce, item.ruleIndex};
            }
        }
    }
    table[grammar.getEndOfStringSymbol().getIdentifierIndex()][automaton.finalStateIndex] =
        Cell{Action::accept, grammar.getStartRuleIndex()};
    return table;
}

}
