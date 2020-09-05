#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"

#include <regex>
#include <string_view>
#include <vector>

using dansandu::glyph::error::TokenizationError;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::regex_tokenizer
{

RegexTokenizer::RegexTokenizer(const std::vector<std::pair<Symbol, std::string_view>>& descriptors)
{
    descriptors_.reserve(descriptors.size());
    for (const auto& descriptor : descriptors)
    {
        descriptors_.push_back({descriptor.first, std::regex{descriptor.second.cbegin(), descriptor.second.cend()}});
    }
}

std::vector<Token> RegexTokenizer::operator()(const std::string_view string) const
{
    auto tokens = std::vector<Token>{};
    auto position = string.cbegin();
    auto match = std::match_results<decltype(position)>{};
    const auto flags = std::regex_constants::match_continuous;
    while (position != string.cend())
    {
        auto matchFound = false;
        for (const auto& descriptor : descriptors_)
        {
            if (matchFound = std::regex_search(position, string.cend(), match, descriptor.second, flags); matchFound)
            {
                const auto begin = static_cast<int>(match[0].first - string.cbegin());
                const auto end = static_cast<int>(match[0].second - string.cbegin());
                tokens.push_back({descriptor.first, begin, end});
                position += match.length();
                break;
            }
        }
        if (!matchFound)
        {
            const auto index = position - string.cbegin();
            THROW(TokenizationError, "no pattern matches symbol at position ", index + 1, ":\n", string, "\n",
                  std::string(index, ' '), "^");
        }
    }
    return tokens;
}

}
