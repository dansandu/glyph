#pragma once

#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace dansandu::glyph::regex_tokenizer
{

class PRALINE_EXPORT RegexTokenizer
{
public:
    explicit RegexTokenizer(const std::vector<std::pair<std::string, std::string>>& descriptors,
                            std::vector<std::string> discarded = {});

    std::vector<dansandu::glyph::token::Token>
    operator()(std::string_view string,
               const std::function<dansandu::glyph::symbol::Symbol(std::string_view)>& symbolMapper) const;

private:
    std::vector<std::pair<std::string, std::regex>> descriptors_;
    std::vector<std::string> discarded_;
};

}
