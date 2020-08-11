#pragma once

#include "dansandu/glyph/implementation/rule.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <ostream>
#include <string_view>
#include <vector>

namespace dansandu::glyph::implementation::grammar
{

class Grammar
{
public:
    explicit Grammar(std::string_view grammar);

    int getStartRuleIndex() const
    {
        return 0;
    }

    dansandu::glyph::symbol::Symbol getSymbol(std::string_view identifier) const;

    dansandu::glyph::symbol::Symbol getTerminalSymbol(std::string_view identifier) const;

    dansandu::glyph::symbol::Symbol getStartSymbol() const
    {
        return dansandu::glyph::symbol::Symbol{0};
    }

    dansandu::glyph::symbol::Symbol getEndOfStringSymbol() const
    {
        return dansandu::glyph::symbol::Symbol{terminalBeginIndex_};
    }

    dansandu::glyph::symbol::Symbol getEmptySymbol() const
    {
        return dansandu::glyph::symbol::Symbol{terminalBeginIndex_ + 1};
    }

    const std::vector<dansandu::glyph::symbol::Symbol>& getFirstSet(dansandu::glyph::symbol::Symbol symbol) const
    {
        return firstTable_[symbol.getIdentifierIndex()];
    }

    const std::string& getIdentifier(dansandu::glyph::symbol::Symbol symbol) const
    {
        return identifiers_[symbol.getIdentifierIndex()];
    }

    int getIdentifiersCount() const
    {
        return identifiers_.size();
    }

    bool isTerminal(dansandu::glyph::symbol::Symbol symbol) const
    {
        return symbol.getIdentifierIndex() >= terminalBeginIndex_;
    }

    bool isNonTerminal(dansandu::glyph::symbol::Symbol symbol) const
    {
        return symbol.getIdentifierIndex() < terminalBeginIndex_;
    }

    const std::vector<dansandu::glyph::implementation::rule::Rule>& getRules() const
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
    std::vector<dansandu::glyph::implementation::rule::Rule> rules_;
    std::vector<std::vector<dansandu::glyph::symbol::Symbol>> firstTable_;
    std::string grammar_;
    int terminalBeginIndex_;
};

}
