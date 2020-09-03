#pragma once

#include "dansandu/ballotin/relation.hpp"

#include <ostream>

namespace dansandu::glyph::symbol
{

class PRALINE_EXPORT Symbol : dansandu::ballotin::relation::TotalOrder<Symbol>
{
public:
    Symbol() : identifierIndex_{-1}
    {
    }

    explicit Symbol(const int identifierIndex) : identifierIndex_{identifierIndex}
    {
    }

    int getIdentifierIndex() const
    {
        return identifierIndex_;
    }

private:
    int identifierIndex_;
};

inline bool operator<(const Symbol a, const Symbol b)
{
    return a.getIdentifierIndex() < b.getIdentifierIndex();
}

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, const Symbol symbol);

}
