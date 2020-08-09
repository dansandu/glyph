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
    struct Descriptor
    {
        dansandu::glyph::symbol::Symbol symbol;
        std::regex pattern;
    };

    explicit RegexTokenizer(std::vector<Descriptor> descriptors,
                            std::vector<dansandu::glyph::symbol::Symbol> discarded = {});

    std::vector<dansandu::glyph::token::Token> operator()(std::string_view string) const;

private:
    std::vector<Descriptor> descriptors_;
    std::vector<dansandu::glyph::symbol::Symbol> discarded_;
};

}
