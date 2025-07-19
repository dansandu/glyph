#pragma once

#include "dansandu/ballotin/type_traits.hpp"
#include "dansandu/glyph/token.hpp"

#include <vector>

namespace dansandu::glyph::tokenizer
{

class PRALINE_EXPORT ITokenizer
{
public:
    virtual ~ITokenizer() noexcept;

    virtual std::vector<dansandu::glyph::token::Token> tokenize(const std::string_view text) const = 0;
};

}
