#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/parsing.hpp"
#include "dansandu/glyph/internal/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <string_view>
#include <vector>

using dansandu::glyph::internal::automaton::Automaton;
using dansandu::glyph::internal::automaton::getAutomaton;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::parsing::parse;
using dansandu::glyph::internal::parsing_table::Cell;
using dansandu::glyph::internal::parsing_table::getClr1ParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::parser
{

struct ParserImplementation
{
    ParserImplementation(const std::string_view grm)
        : grammar{grm}, parsingTable{getClr1ParsingTable(grammar, getAutomaton(grammar))}
    {
    }

    void dump(std::ostream& stream) const;

    Grammar grammar;
    std::vector<std::vector<Cell>> parsingTable;
};

void ParserImplementation::dump(std::ostream& stream) const
{
    stream << std::endl << "Rules:" << std::endl;
    for (const auto& rule : grammar.getRules())
    {
        stream << '\t' << grammar.getIdentifier(rule.leftSide) << " ->";
        for (const auto symbol : rule.rightSide)
        {
            stream << " " << grammar.getIdentifier(symbol);
        }
        stream << std::endl;
    }

    stream << std::endl << "First table:" << std::endl;
    for (auto i = 0; i < grammar.getIdentifiersCount(); ++i)
    {
        const auto symbol = Symbol{i};
        auto firstPrint = true;
        stream << "\t'" << grammar.getIdentifier(symbol) << "': [";
        for (const auto firstSymbol : grammar.getFirstSet(symbol))
        {
            stream << (firstPrint ? "'" : ", '") << grammar.getIdentifier(firstSymbol) << "'";
            firstPrint = false;
        }
        stream << ']' << std::endl;
    }

    stream << std::endl << "Automaton:" << std::endl;
    stream << "\tStates:" << std::endl;
    const auto automaton = getAutomaton(grammar);
    for (auto i = 0; i < static_cast<int>(automaton.states.size()); ++i)
    {
        const auto& state = automaton.states[i];
        stream << "\t\tState #" << i << ":\n";
        auto collapsedItems = std::map<std::pair<int, int>, std::set<Symbol>>{};
        for (const auto& item : state)
        {
            collapsedItems[{item.ruleIndex, item.position}].insert(item.lookahead);
        }
        for (const auto& entry : collapsedItems)
        {
            const auto& rule = grammar.getRules()[entry.first.first];
            stream << "\t\t\t" << grammar.getIdentifier(rule.leftSide) << " ->";
            for (auto j = 0; j < static_cast<int>(rule.rightSide.size()); ++j)
            {
                const auto symbol = rule.rightSide[j];
                stream << (entry.first.second == j ? " ." : " ") << grammar.getIdentifier(symbol);
            }
            if (entry.first.second == static_cast<int>(rule.rightSide.size()))
            {
                stream << " .";
            }
            stream << " ,";
            for (const auto lookahead : entry.second)
            {
                stream << " " << grammar.getIdentifier(lookahead);
            }
            stream << std::endl;
        }
        stream << std::endl;
    }
}

Parser::Parser(const std::string_view grammar) : implementation_{std::make_unique<ParserImplementation>(grammar)}
{
}

Parser::~Parser()
{
}

Symbol Parser::getTerminalSymbol(const std::string_view identifier) const
{
    return implementation_->grammar.getTerminalSymbol(identifier);
}

Symbol Parser::getDiscardedSymbolPlaceholder() const
{
    return implementation_->grammar.getDiscardedSymbolPlaceholder();
}

void Parser::parse(const std::vector<Token>& tokens, const std::function<void(const Node&)>& visitor) const
{
    ::parse(tokens, implementation_->parsingTable, implementation_->grammar, visitor);
}

void Parser::dump(std::ostream& stream) const
{
    implementation_->dump(stream);
}

}
