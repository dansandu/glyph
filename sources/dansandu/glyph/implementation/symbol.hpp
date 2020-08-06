#pragma once

#include "dansandu/ballotin/relation.hpp"

#include <ostream>

namespace dansandu::glyph::implementation::symbol
{

class Symbol : dansandu::ballotin::relation::total_order<Symbol>
{
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

inline bool operator<(Symbol a, Symbol b)
{
    return a.getIdentifierIndex() < b.getIdentifierIndex();
}

std::ostream& operator<<(std::ostream& stream, Symbol symbol);

}
