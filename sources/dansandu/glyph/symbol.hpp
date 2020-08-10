#pragma once

#include "dansandu/ballotin/relation.hpp"

#include <ostream>

namespace dansandu::glyph::symbol
{

class PRALINE_EXPORT Symbol : dansandu::ballotin::relation::TotalOrder<Symbol>
{
    friend bool operator<(Symbol a, Symbol b)
    {
        return a.getIdentifierIndex() < b.getIdentifierIndex();
    }

public:
    explicit Symbol(int identifierIndex) : identifierIndex_{identifierIndex}
    {
    }

    int getIdentifierIndex() const
    {
        return identifierIndex_;
    }

private:
    int identifierIndex_;
};

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, Symbol symbol);

}
