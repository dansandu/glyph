#pragma once

#include "dansandu/glyph/implementation/rule.hpp"
#include "dansandu/glyph/implementation/symbol.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::grammar
{

class Grammar
{
public:
    explicit Grammar(std::string grammar);

    int getStartRuleIndex() const
    {
        return 0;
    }

    dansandu::glyph::implementation::symbol::Symbol getStartSymbol() const
    {
        return dansandu::glyph::implementation::symbol::Symbol{0};
    }

    dansandu::glyph::implementation::symbol::Symbol getEndOfStringSymbol() const
    {
        return dansandu::glyph::implementation::symbol::Symbol{terminalBeginIndex_};
    }

    dansandu::glyph::implementation::symbol::Symbol getEmptySymbol() const
    {
        return dansandu::glyph::implementation::symbol::Symbol{terminalBeginIndex_ + 1};
    }

    const std::vector<dansandu::glyph::implementation::symbol::Symbol>&
    getFirstSet(dansandu::glyph::implementation::symbol::Symbol symbol) const
    {
        return firstTable_[symbol.getIdentifierIndex()];
    }

    const std::string& getIdentifier(dansandu::glyph::implementation::symbol::Symbol symbol) const
    {
        return identifiers_[symbol.getIdentifierIndex()];
    }

    bool isTerminal(dansandu::glyph::implementation::symbol::Symbol symbol) const
    {
        return symbol.getIdentifierIndex() >= terminalBeginIndex_;
    }

    bool isNonTerminal(dansandu::glyph::implementation::symbol::Symbol symbol) const
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
    std::vector<std::vector<dansandu::glyph::implementation::symbol::Symbol>> firstTable_;
    std::string grammar_;
    int terminalBeginIndex_;
};

}
