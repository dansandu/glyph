#pragma once

#include <compare>
#include <ostream>

namespace dansandu::glyph::symbol
{

class PRALINE_EXPORT Symbol
{
public:
    friend constexpr auto operator<=>(const Symbol& left, const Symbol& right) = default;

    constexpr Symbol() : identifierIndex_{-1}
    {
    }

    constexpr explicit Symbol(const int identifierIndex) : identifierIndex_{identifierIndex}
    {
    }

    constexpr int getIdentifierIndex() const
    {
        return identifierIndex_;
    }

private:
    int identifierIndex_;
};

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, const Symbol symbol);

}
