#pragma once

#include "dansandu/glyph/implementation/symbol.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::implementation::rule
{

struct Rule
{
    dansandu::glyph::implementation::symbol::Symbol leftSide;
    std::vector<dansandu::glyph::implementation::symbol::Symbol> rightSide;
};

bool operator==(const Rule& left, const Rule& right);

bool operator!=(const Rule& left, const Rule& right);

std::ostream& operator<<(std::ostream& stream, const Rule& rule);

}
