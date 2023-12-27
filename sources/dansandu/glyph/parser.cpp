#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/internal/automaton.hpp"
#include "dansandu/glyph/internal/first_table.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/parsing.hpp"
#include "dansandu/glyph/internal/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string_view>
#include <vector>

using dansandu::glyph::internal::automaton::Automaton;
using dansandu::glyph::internal::automaton::getAutomaton;
using dansandu::glyph::internal::first_table::getFirstTable;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::parsing::parse;
using dansandu::glyph::internal::parsing_table::Cell;
using dansandu::glyph::internal::parsing_table::getClr1ParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;
using dansandu::glyph::tokenizer::ITokenizer;

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
    stream << "\nRules:\n";
    for (const auto& rule : grammar.getRules())
    {
        stream << '\t' << grammar.getIdentifier(rule.leftSide) << " ->";
        for (const auto& symbol : rule.rightSide)
        {
            stream << " " << grammar.getIdentifier(symbol);
        }
        stream << '\n';
    }

    stream << "\nFirst table:\n";
    const auto firstTable = getFirstTable(grammar);
    for (auto i = 0; i < static_cast<int>(grammar.getIdentifiers().size()); ++i)
    {
        const auto symbol = Symbol{i};
        auto firstPrint = true;
        stream << "\t'" << grammar.getIdentifier(symbol) << "': [";
        for (const auto& firstSymbol : firstTable[symbol.getIdentifierIndex()])
        {
            stream << (firstPrint ? "'" : ", '") << grammar.getIdentifier(firstSymbol) << "'";
            firstPrint = false;
        }
        stream << "]\n";
    }

    stream << "\nAutomaton:\n";
    stream << "\tStates:\n";
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
            for (const auto& lookahead : entry.second)
            {
                stream << " " << grammar.getIdentifier(lookahead);
            }
            stream << '\n';
        }
        stream << '\n';
    }
}

static const ParserImplementation* casted(const void* implementation)
{
    return static_cast<const ParserImplementation*>(implementation);
}

static void deleter(const void* implementation)
{
    delete static_cast<const ParserImplementation*>(implementation);
}

Parser::Parser(const std::string_view grammar) : implementation_{new ParserImplementation{grammar}, &deleter}
{
}

Symbol Parser::getTerminalSymbol(const std::string_view identifier) const
{
    return casted(implementation_.get())->grammar.getTerminalSymbol(identifier);
}

Symbol Parser::getDiscardedSymbolPlaceholder() const
{
    return casted(implementation_.get())->grammar.getDiscardedSymbolPlaceholder();
}

std::vector<Node> Parser::parse(const std::string_view text, const ITokenizer& tokenizer) const
{
    const auto tokens = tokenizer.tokenize(text);
    return ::parse(text, tokens, casted(implementation_.get())->parsingTable, casted(implementation_.get())->grammar);
}

void Parser::dump(std::ostream& stream) const
{
    casted(implementation_.get())->dump(stream);
}

}
