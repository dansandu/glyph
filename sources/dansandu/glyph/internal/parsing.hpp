#pragma once

#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

#include <vector>

namespace dansandu::glyph::internal::parsing
{

std::vector<dansandu::glyph::node::Node>
parse(const std::string_view text, const std::vector<dansandu::glyph::token::Token>& tokens,
      const std::vector<std::vector<dansandu::glyph::internal::parsing_table::Cell>>& parsingTable,
      const dansandu::glyph::internal::grammar::Grammar& grammar);

}
