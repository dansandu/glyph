#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using dansandu::glyph::implementation::automaton::Automaton;
using dansandu::glyph::implementation::automaton::Item;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::grammar::SymbolTable;

namespace dansandu::glyph::implementation::parsing_table {

std::ostream& operator<<(std::ostream& stream, Action action) {
    switch (action) {
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

bool operator==(Cell left, Cell right) { return left.action == right.action && left.parameter == right.parameter; }

bool operator!=(Cell left, Cell right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, Cell cell) {
    return stream << "Cell(" << cell.action << ", " << cell.parameter << ")";
}

std::vector<int> getReductionRuleIndices(const std::vector<Item>& items, const std::vector<Rule>& rules,
                                         int startRuleIndex) {
    auto reductions = std::vector<int>{};
    for (const auto& item : items)
        if (item.ruleIndex != startRuleIndex &&
            item.position == static_cast<int>(rules.at(item.ruleIndex).rightSide.size()))
            reductions.push_back(item.ruleIndex);
    return reductions;
}

ParsingTable getSimpleLeftToRightParsingTable(const std::vector<Rule>& rules, const Automaton& automaton,
                                              const SymbolTable& followTable,
                                              const std::vector<std::string>& nonterminals) {
    auto table = std::map<std::string, std::vector<Cell>>{};
    auto stateCount = static_cast<int>(automaton.states.size());
    for (const auto& transition : automaton.transitions) {
        auto rowPosition = table.find(transition.symbol);
        if (rowPosition == table.end())
            rowPosition = table.emplace(transition.symbol, std::vector<Cell>(stateCount)).first;
        auto isTerminal =
            std::find(nonterminals.cbegin(), nonterminals.cend(), transition.symbol) == nonterminals.cend();
        rowPosition->second.at(transition.from) = Cell{isTerminal ? Action::shift : Action::goTo, transition.to};
    }
    auto& endOfStringRow = table.emplace(endOfString, std::vector<Cell>(stateCount)).first->second;
    endOfStringRow.at(automaton.finalStateIndex) = Cell{Action::accept, automaton.startRuleIndex};
    for (auto stateIndex = 0; stateIndex < stateCount; ++stateIndex)
        for (auto ruleIndex : getReductionRuleIndices(automaton.states[stateIndex], rules, automaton.startRuleIndex))
            for (const auto& symbol : followTable.at(rules.at(ruleIndex).leftSide)) {
                auto& cell = table.at(symbol).at(stateIndex);
                if (cell.action != Action::error)
                    THROW(ParsingError, "grammar cannot be parsed using a SLR(1) parser due to ", cell.action,
                          "/reduce conflict");
                cell = Cell{Action::reduce, ruleIndex};
            }
    return {automaton.startRuleIndex, std::move(table)};
}

}
