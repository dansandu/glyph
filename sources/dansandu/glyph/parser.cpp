#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parsing.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"

#include <string>
#include <string_view>
#include <vector>

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::parsing_table::getCanonicalLeftToRightParsingTable;
using dansandu::glyph::implementation::parsing_table::ParsingTable;

namespace dansandu::glyph::parser
{

struct ParserImplementation
{
    ParserImplementation(const std::string& grm)
        : grammar{grm}, parsingTable{getCanonicalLeftToRightParsingTable(grammar, getAutomaton(grammar))}
    {
    }

    Grammar grammar;
    ParsingTable parsingTable;
};

Parser::Parser(const std::string& grammar) : implementation_{std::make_unique<ParserImplementation>(grammar)}
{
}

Parser::~Parser()
{
}

dansandu::glyph::node::Node Parser::parse(const std::vector<dansandu::glyph::token::Token>& tokens) const
{
    return dansandu::glyph::implementation::parsing::parse(tokens, implementation_->parsingTable,
                                                           implementation_->grammar.getRules());
}

}
