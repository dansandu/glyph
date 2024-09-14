#pragma once

#include "dansandu/glyph/symbol.hpp"

#include <compare>
#include <ostream>

namespace dansandu::glyph::internal::item
{

struct Item
{
    friend constexpr auto operator<=>(const Item& left, const Item& right) = default;

    Item(const int ruleIndex, const int position, const dansandu::glyph::symbol::Symbol lookahead)
        : ruleIndex{ruleIndex}, position{position}, lookahead{lookahead}
    {
    }

    int ruleIndex;
    int position;
    dansandu::glyph::symbol::Symbol lookahead;
};

std::ostream& operator<<(std::ostream& stream, const Item& item);

}
