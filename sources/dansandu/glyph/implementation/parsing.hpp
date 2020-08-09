#pragma once

#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

#include <vector>

namespace dansandu::glyph::implementation::parsing
{

std::vector<dansandu::glyph::node::Node>
parse(std::vector<dansandu::glyph::token::Token> tokens,
      const std::vector<std::vector<dansandu::glyph::implementation::parsing_table::Cell>>& parsingTable,
      const dansandu::glyph::implementation::grammar::Grammar& grammar);

}
