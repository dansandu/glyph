#pragma once

#include "dansandu/glyph/symbol.hpp"

#include <ostream>

namespace dansandu::glyph::token
{

class PRALINE_EXPORT Token
{
public:
    friend constexpr auto operator<=>(const Token& left, const Token& right) = default;

    constexpr Token(const dansandu::glyph::symbol::Symbol symbol, const int begin, const int end)
        : symbol_{symbol}, begin_{begin}, end_{end}
    {
    }

    constexpr dansandu::glyph::symbol::Symbol getSymbol() const
    {
        return symbol_;
    }

    constexpr int begin() const
    {
        return begin_;
    }

    constexpr int end() const
    {
        return end_;
    }

private:
    dansandu::glyph::symbol::Symbol symbol_;
    int begin_;
    int end_;
};

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, const Token& token);

}
