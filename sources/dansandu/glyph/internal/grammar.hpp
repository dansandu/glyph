#pragma once

#include "dansandu/glyph/internal/rule.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <ostream>
#include <string_view>
#include <vector>

namespace dansandu::glyph::internal::grammar
{

class Grammar
{
public:
    explicit Grammar(const std::string_view grammar);

    int getStartRuleIndex() const
    {
        return 0;
    }

    dansandu::glyph::symbol::Symbol getSymbol(const std::string_view identifier) const;

    dansandu::glyph::symbol::Symbol getTerminalSymbol(const std::string_view identifier) const;

    dansandu::glyph::symbol::Symbol getDiscardedSymbolPlaceholder() const
    {
        return dansandu::glyph::symbol::Symbol{};
    }

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

    const std::string& getIdentifier(const dansandu::glyph::symbol::Symbol symbol) const
    {
        return identifiers_[symbol.getIdentifierIndex()];
    }

    const std::vector<std::string>& getIdentifiers() const
    {
        return identifiers_;
    }

    bool isTerminal(const dansandu::glyph::symbol::Symbol symbol) const
    {
        return symbol.getIdentifierIndex() >= terminalBeginIndex_;
    }

    bool isNonTerminal(const dansandu::glyph::symbol::Symbol symbol) const
    {
        return symbol.getIdentifierIndex() < terminalBeginIndex_;
    }

    const std::vector<dansandu::glyph::internal::rule::Rule>& getRules() const
    {
        return rules_;
    }

private:
    int terminalBeginIndex_;
    std::vector<std::string> identifiers_;
    std::vector<dansandu::glyph::internal::rule::Rule> rules_;
};

}
