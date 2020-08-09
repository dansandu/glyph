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

RegexTokenizer::RegexTokenizer(std::vector<Descriptor> descriptors, std::vector<Symbol> discarded)
    : descriptors_{std::move(descriptors)}, discarded_{std::move(discarded)}
{
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
            if (matchFound = std::regex_search(position, string.cend(), match, descriptor.pattern, flags); matchFound)
            {
                auto begin = static_cast<int>(match[0].first - string.cbegin());
                auto end = static_cast<int>(match[0].second - string.cbegin());
                if (std::find(discarded_.cbegin(), discarded_.cend(), descriptor.symbol) == discarded_.cend())
                {
                    tokens.push_back({descriptor.symbol, begin, end});
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
