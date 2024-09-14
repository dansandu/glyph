#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/text_location.hpp"

#include <regex>
#include <string_view>
#include <vector>

using dansandu::glyph::error::TokenizationError;
using dansandu::glyph::internal::text_location::getTextLocation;
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

std::vector<Token> RegexTokenizer::tokenize(const std::string_view text) const
{
    auto tokens = std::vector<Token>{};
    auto position = text.cbegin();
    auto match = std::match_results<decltype(position)>{};
    const auto flags = std::regex_constants::match_continuous;
    while (position != text.cend())
    {
        auto matchFound = false;
        for (const auto& descriptor : descriptors_)
        {
            if (matchFound = std::regex_search(position, text.cend(), match, descriptor.second, flags); matchFound)
            {
                const auto begin = static_cast<int>(match[0].first - text.cbegin());
                const auto end = static_cast<int>(match[0].second - text.cbegin());
                tokens.push_back({descriptor.first, begin, end});
                position += match.length();
                break;
            }
        }
        if (!matchFound)
        {
            const auto index = position - text.cbegin();
            const auto textLocation = getTextLocation(text, index, index);

            THROW(TokenizationError, "no pattern matches at line ", textLocation.lineNumber, " and column ",
                  textLocation.columnNumber, "\n", textLocation.highlight);
        }
    }
    return tokens;
}

}
