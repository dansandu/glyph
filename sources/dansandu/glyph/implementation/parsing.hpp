#pragma once

#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::parsing
{

class SyntaxError : public std::runtime_error
{
    using runtime_error::runtime_error;
};

dansandu::glyph::node::Node parse(std::vector<dansandu::glyph::token::Token> tokens,
                                  const dansandu::glyph::implementation::parsing_table::ParsingTable& parsingTable,
                                  const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules);
}
