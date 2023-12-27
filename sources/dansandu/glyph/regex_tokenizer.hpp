#pragma once

#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/tokenizer.hpp"

#include <regex>
#include <string_view>
#include <vector>

namespace dansandu::glyph::regex_tokenizer
{

class PRALINE_EXPORT RegexTokenizer : public dansandu::glyph::tokenizer::ITokenizer
{
public:
    explicit RegexTokenizer(
        const std::vector<std::pair<dansandu::glyph::symbol::Symbol, std::string_view>>& descriptors);

    std::vector<dansandu::glyph::token::Token> tokenize(const std::string_view text) const override;

private:
    std::vector<std::pair<dansandu::glyph::symbol::Symbol, std::regex>> descriptors_;
};

}
