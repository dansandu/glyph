#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parsing.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"

#include <string>
#include <string_view>
#include <vector>

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::parsing_table::getCanonicalLeftToRightParsingTable;

namespace dansandu::glyph::parser {

Parser::Parser(std::string grammar)
    : grammar_{std::move(grammar)},
      parsingTable_{getCanonicalLeftToRightParsingTable(grammar_, getAutomaton(grammar_))} {}

}
