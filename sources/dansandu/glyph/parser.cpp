#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/implementation/automaton.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parsing.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <string_view>
#include <vector>

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::parsing_table::Cell;
using dansandu::glyph::implementation::parsing_table::getCanonicalLeftToRightParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::parser
{

struct ParserImplementation
{
    ParserImplementation(std::string_view grm)
        : grammar{grm}, parsingTable{getCanonicalLeftToRightParsingTable(grammar, getAutomaton(grammar))}
    {
    }

    Grammar grammar;
    std::vector<std::vector<Cell>> parsingTable;
};

Parser::Parser(std::string_view grammar) : implementation_{std::make_unique<ParserImplementation>(grammar)}
{
}

Parser::~Parser()
{
}

Symbol Parser::getSymbol(std::string_view identifier) const
{
    return implementation_->grammar.getSymbol(identifier);
}

void Parser::parse(const std::vector<Token>& tokens, const std::function<void(const Node&)>& visitor) const
{
    dansandu::glyph::implementation::parsing::parse(tokens, implementation_->parsingTable, implementation_->grammar,
                                                    visitor);
}

}
