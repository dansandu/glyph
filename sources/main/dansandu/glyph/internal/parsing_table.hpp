#pragma once

#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/internal/grammar.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::internal::parsing_table
{

enum class Action
{
    error,
    shift,
    goTo,
    reduce,
    accept
};

std::ostream& operator<<(std::ostream& stream, const Action action);

struct Cell
{
    Cell() : action{Action::error}, parameter{0}
    {
    }

    Cell(const Action action, const int parameter) : action{action}, parameter{parameter}
    {
    }

    Action action;
    int parameter;
};

inline bool operator==(const Cell left, const Cell right)
{
    return (left.action == right.action) & (left.parameter == right.parameter);
}

inline bool operator!=(const Cell left, const Cell right)
{
    return !(left == right);
}

std::ostream& operator<<(std::ostream& stream, const Cell cell);

std::vector<std::vector<Cell>> getClr1ParsingTable(const dansandu::glyph::internal::grammar::Grammar& grammar,
                                                   const dansandu::glyph::internal::automaton::Automaton& automaton);

}
