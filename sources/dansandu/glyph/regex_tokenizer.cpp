#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"

#include <string>
#include <string_view>

using dansandu::glyph::error::TokenizationError;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::regex_tokenizer
{

RegexTokenizer::RegexTokenizer(const std::vector<std::pair<Symbol, std::string>>& descriptors,
                               std::vector<Symbol> discarded)
    : discarded_{std::move(discarded)}
{
    descriptors_.reserve(descriptors.size());
    for (const auto& descriptor : descriptors)
    {
        descriptors_.push_back({descriptor.first, std::regex{descriptor.second}});
    }
}

std::vector<Token> RegexTokenizer::operator()(std::string_view string) const
{
    auto tokens = std::vector<Token>{};
    auto position = string.cbegin();
    auto match = std::match_results<decltype(position)>{};
    auto flags = std::regex_constants::match_continuous;
    while (position != string.cend())
    {
        auto matchFound = false;
        for (const auto& descriptor : descriptors_)
        {
            if (matchFound = std::regex_search(position, string.cend(), match, descriptor.second, flags); matchFound)
            {
                auto begin = static_cast<int>(match[0].first - string.cbegin());
                auto end = static_cast<int>(match[0].second - string.cbegin());
                if (std::find(discarded_.cbegin(), discarded_.cend(), descriptor.first) == discarded_.cend())
                {
                    tokens.push_back({descriptor.first, begin, end});
                }
                position += match.length();
                break;
            }
        }
        if (!matchFound)
        {
            auto index = position - string.cbegin();
            THROW(TokenizationError, "no pattern matches symbol at position ", index, ":\n", string, "\n",
                  std::string(index, ' '), "^");
        }
    }
    return tokens;
}

}