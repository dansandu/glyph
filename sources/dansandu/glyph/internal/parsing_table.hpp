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

std::ostream& operator<<(std::ostream& stream, Action action);

struct Cell
{
    Cell() : action{Action::error}, parameter{0}
    {
    }

    Cell(Action action, int parameter) : action{action}, parameter{parameter}
    {
    }

    Action action;
    int parameter;
};

bool operator==(Cell left, Cell right);

bool operator!=(Cell left, Cell right);

std::ostream& operator<<(std::ostream& stream, Cell cell);

std::vector<std::vector<Cell>> getClr1ParsingTable(const dansandu::glyph::internal::grammar::Grammar& grammar,
                                                   const dansandu::glyph::internal::automaton::Automaton& automaton);

}
