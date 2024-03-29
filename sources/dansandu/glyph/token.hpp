#pragma once

#include "dansandu/glyph/symbol.hpp"

#include <ostream>

namespace dansandu::glyph::token
{

class PRALINE_EXPORT Token
{
public:
    Token(const dansandu::glyph::symbol::Symbol symbol, const int begin, const int end)
        : symbol_{symbol}, begin_{begin}, end_{end}
    {
    }

    dansandu::glyph::symbol::Symbol getSymbol() const
    {
        return symbol_;
    }

    int begin() const
    {
        return begin_;
    }

    int end() const
    {
        return end_;
    }

private:
    dansandu::glyph::symbol::Symbol symbol_;
    int begin_;
    int end_;
};

inline bool operator==(const Token& left, const Token& right)
{
    return left.getSymbol() == right.getSymbol() && left.begin() == right.begin() && left.end() == right.end();
}

inline bool operator!=(const Token& left, const Token& right)
{
    return !(left == right);
}

inline std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    return stream << "Token(" << token.getSymbol() << ", " << token.begin() << ", " << token.end() << ")";
}

}
