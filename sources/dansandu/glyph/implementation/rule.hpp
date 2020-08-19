#pragma once

#include "dansandu/glyph/symbol.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::implementation::rule
{

struct Rule
{
    dansandu::glyph::symbol::Symbol leftSide;
    std::vector<dansandu::glyph::symbol::Symbol> rightSide;
};

bool operator==(const Rule& left, const Rule& right);

bool operator!=(const Rule& left, const Rule& right);

std::ostream& operator<<(std::ostream& stream, const Rule& rule);

}
