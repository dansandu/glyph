#pragma once

#include "dansandu/ballotin/relation.hpp"
#include "dansandu/glyph/symbol.hpp"

namespace dansandu::glyph::implementation::item
{

struct Item : dansandu::ballotin::relation::TotalOrder<Item>
{
    Item(int ruleIndex, int position, dansandu::glyph::symbol::Symbol lookahead)
        : ruleIndex{ruleIndex}, position{position}, lookahead{lookahead}
    {
    }

    int ruleIndex;
    int position;
    dansandu::glyph::symbol::Symbol lookahead;
};

bool operator<(const Item& left, const Item& right);

std::ostream& operator<<(std::ostream& stream, const Item& item);

}
