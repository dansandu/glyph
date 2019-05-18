#include "dansandu/glyph/implementation/tokenization.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/token.hpp"

#include <algorithm>
#include <regex>
#include <sstream>

using dansandu::glyph::token::Token;
using dansandu::glyph::token::TokenDescriptor;

namespace dansandu::glyph::implementation::tokenization {

std::string getTerminalsPattern(const std::vector<TokenDescriptor>& tokenDescriptors) {
    std::stringstream stream;
    for (const auto& tokenDescriptor : tokenDescriptors)
        stream << "(" << tokenDescriptor.pattern << ")|";
    auto pattern = stream.str();
    if (!pattern.empty())
        pattern.pop_back();
    return pattern;
}

std::vector<Token> tokenize(std::string_view string, const std::vector<TokenDescriptor>& tokenDescriptors,
                            const std::vector<std::string>& discard) {
    auto tokens = std::vector<Token>{};
    auto position = string.cbegin();
    auto pattern = std::regex{getTerminalsPattern(tokenDescriptors)};
    auto match = std::cmatch{};
    auto flags = std::regex_constants::match_continuous;
    for (; std::regex_search(position, string.cend(), match, pattern, flags); position += match.length())
        for (auto group = 1, foundGroup = 0; group < static_cast<int>(match.size()); ++group)
            if (match[group].length()) {
                auto begin = static_cast<int>(match[group].first - string.cbegin());
                auto end = static_cast<int>(match[group].second - string.cbegin());
                if (!foundGroup) {
                    if (std::find(discard.cbegin(), discard.cend(), tokenDescriptors[group - 1].identifier) ==
                        discard.cend())
                        tokens.push_back({tokenDescriptors[group - 1].identifier, begin, end});
                    foundGroup = group;
                } else
                    THROW(TokenizationError, "ambiguous tokenization at position ", begin, ":\n", string, "\n",
                          std::string(begin, ' '), "^", std::string(begin - end - 1, '~'), "\nToken matches both '",
                          tokenDescriptors[foundGroup - 1].identifier, "' and '",
                          tokenDescriptors[group - 1].identifier, "' patterns");
            }
    if (position != string.cend()) {
        auto index = position - string.cbegin();
        THROW(TokenizationError, "no pattern matches symbol at position ", index, ":\n", string, "\n",
              std::string(index, ' '), "^");
    }
    return tokens;
}
}
