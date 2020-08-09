#pragma once

#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <regex>
#include <string_view>
#include <vector>

namespace dansandu::glyph::regex_tokenizer
{

class PRALINE_EXPORT RegexTokenizer
{
public:
    explicit RegexTokenizer(const std::vector<std::pair<dansandu::glyph::symbol::Symbol, std::string>>& descriptors,
                            std::vector<dansandu::glyph::symbol::Symbol> discarded = {});

    std::vector<dansandu::glyph::token::Token> operator()(std::string_view string) const;

private:
    std::vector<std::pair<dansandu::glyph::symbol::Symbol, std::regex>> descriptors_;
    std::vector<dansandu::glyph::symbol::Symbol> discarded_;
};

}
