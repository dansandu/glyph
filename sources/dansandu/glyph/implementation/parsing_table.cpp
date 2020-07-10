#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using dansandu::glyph::error::ParsingError;
using dansandu::glyph::implementation::automaton::Automaton;
using dansandu::glyph::implementation::automaton::Item;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::Grammar;

namespace dansandu::glyph::implementation::parsing_table
{

std::ostream& operator<<(std::ostream& stream, Action action)
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
        THROW(std::invalid_argument, "unrecognized cell action");
    }
}

bool operator==(Cell left, Cell right)
{
    return left.action == right.action && left.parameter == right.parameter;
}

bool operator!=(Cell left, Cell right)
{
    return !(left == right);
}

std::ostream& operator<<(std::ostream& stream, Cell cell)
{
    return stream << "Cell(" << cell.action << ", " << cell.parameter << ")";
}

ParsingTable getCanonicalLeftToRightParsingTable(const Grammar& grammar, const Automaton& automaton)
{
    auto table = std::map<std::string, std::vector<Cell>>{};
    auto stateCount = static_cast<int>(automaton.states.size());
    for (const auto& transition : automaton.transitions)
    {
        auto rowPosition = table.find(transition.symbol);
        if (rowPosition == table.end())
            rowPosition = table.emplace(transition.symbol, std::vector<Cell>(stateCount)).first;
        auto isTerminal = std::find(grammar.getNonterminals().cbegin(), grammar.getNonterminals().cend(),
                                    transition.symbol) == grammar.getNonterminals().cend();
        rowPosition->second.at(transition.from) = Cell{isTerminal ? Action::shift : Action::goTo, transition.to};
    }
    auto& endOfStringRow = table.emplace(endOfString, std::vector<Cell>(stateCount)).first->second;
    for (auto stateIndex = 0; stateIndex < stateCount; ++stateIndex)
        for (const auto& item : automaton.states[stateIndex])
            if (item.position == static_cast<int>(grammar.getRules().at(item.ruleIndex).rightSide.size()))
            {
                auto& cell = table.at(item.lookahead).at(stateIndex);
                if (cell.action != Action::error)
                    THROW(ParsingError, "grammar cannot be parsed using a CLR(1) parser due to ", cell.action,
                          "/reduce conflict");
                cell = Cell{Action::reduce, item.ruleIndex};
            }
    endOfStringRow.at(automaton.finalStateIndex) = Cell{Action::accept, automaton.startRuleIndex};
    return {automaton.startRuleIndex, std::move(table)};
}

}
