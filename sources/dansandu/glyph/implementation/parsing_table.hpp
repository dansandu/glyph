#pragma once

#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::parsing_table {

class ParsingError : public std::runtime_error {
    using runtime_error::runtime_error;
};

enum class Action { error, shift, goTo, reduce, accept };

std::ostream& operator<<(std::ostream& stream, Action action);

struct Cell {
    Cell() : action{Action::error}, parameter{0} {}
    Cell(Action action, int parameter) : action{action}, parameter{parameter} {}

    Action action;
    int parameter;
};

bool operator==(Cell left, Cell right);

bool operator!=(Cell left, Cell right);

std::ostream& operator<<(std::ostream& stream, Cell cell);

struct ParsingTable {
    int startRuleIndex;
    std::map<std::string, std::vector<Cell>> table;
};

std::vector<int> getReductionRuleIndices(const std::vector<dansandu::glyph::implementation::automaton::Item>& items,
                                         const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules,
                                         int startRuleIndex);

ParsingTable getSimpleLeftToRightParsingTable(const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules,
                                              const dansandu::glyph::implementation::automaton::Automaton& automaton,
                                              const dansandu::glyph::implementation::grammar::SymbolTable& followTable,
                                              const std::vector<std::string>& nonterminals);

}
