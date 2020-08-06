#pragma once

#include "dansandu/glyph/implementation/symbol.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::grammar
{

using dansandu::glyph::implementation::symbol::Symbol;

struct Rule
{
    Symbol leftSide;
    std::vector<Symbol> rightSide;
};

class Grammar
{
public:
    explicit Grammar(std::string grammar);

    Symbol getStartSymbol() const
    {
        return Symbol{0};
    }

    Symbol getEndOfStringSymbol() const
    {
        return Symbol{terminalBeginIndex_};
    }

    Symbol getEmptySymbol() const
    {
        return Symbol{terminalBeginIndex_ + 1};
    }

    const std::vector<Symbol>& getFirstSet(Symbol symbol) const
    {
        return firstTable_[symbol.getIdentifierIndex()];
    }

    const std::string& getIdentifier(Symbol symbol) const
    {
        return identifiers_[symbol.getIdentifierIndex()];
    }

    bool isTerminal(Symbol symbol) const
    {
        return symbol.getIdentifierIndex() >= terminalBeginIndex_;
    }

    bool isNonTerminal(Symbol symbol) const
    {
        return symbol.getIdentifierIndex() < terminalBeginIndex_;
    }

    const std::vector<Rule>& getRules() const
    {
        return rules_;
    }

    const std::string& toString() const
    {
        return grammar_;
    }

private:
    void generateFirstTable();

    std::vector<std::string> identifiers_;
    std::vector<Rule> rules_;
    std::vector<std::vector<Symbol>> firstTable_;
    std::string grammar_;
    int terminalBeginIndex_;
};

bool operator==(const Rule& left, const Rule& right);

bool operator!=(const Rule& left, const Rule& right);

std::ostream& operator<<(std::ostream& stream, const Rule& rule);

}
