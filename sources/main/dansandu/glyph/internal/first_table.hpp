#pragma once

#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/symbol.hpp"

#include <vector>

namespace dansandu::glyph::internal::first_table
{

std::vector<std::vector<dansandu::glyph::symbol::Symbol>>
getFirstTable(const dansandu::glyph::internal::grammar::Grammar& grammar);

}
