#include "dansandu/glyph/implementation/tokenization.hpp"
#include "dansandu/ballotin/exception.hpp"

#include <algorithm>
#include <regex>
#include <sstream>

namespace dansandu::glyph::implementation::tokenization {

bool operator==(const Token& left, const Token& right) {
    return left.identifier == right.identifier && left.begin == right.begin && left.end == right.end;
}

bool operator!=(const Token& left, const Token& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    return stream << "Token(" << token.identifier << ", " << token.begin << ", " << token.end << ")";
}

std::string getTerminalsPattern(const std::vector<std::pair<std::string, std::string>>& terminals) {
    std::stringstream stream;
    for (const auto& terminal : terminals)
        stream << "(" << terminal.second << ")|";
    auto pattern = stream.str();
    if (!pattern.empty())
        pattern.pop_back();
    return pattern;
}

std::vector<Token> tokenize(std::string_view string, const std::vector<std::pair<std::string, std::string>>& terminals,
                            const std::vector<std::string>& discard) {
    auto tokens = std::vector<Token>{};
    auto position = string.cbegin();
    auto pattern = std::regex{getTerminalsPattern(terminals)};
    auto match = std::cmatch{};
    auto flags = std::regex_constants::match_continuous;
    for (; std::regex_search(position, string.cend(), match, pattern, flags); position += match.length())
        for (auto group = 1, foundGroup = 0; group < static_cast<int>(match.size()); ++group)
            if (match[group].length()) {
                auto begin = static_cast<int>(match[group].first - string.cbegin());
                auto end = static_cast<int>(match[group].second - string.cbegin());
                if (!foundGroup) {
                    if (std::find(discard.cbegin(), discard.cend(), terminals[group - 1].first) == discard.cend())
                        tokens.push_back({terminals[group - 1].first, begin, end});
                    foundGroup = group;
                } else
                    THROW(TokenizationError, "ambiguous tokenization at position ", begin, ":\n", string, "\n",
                          std::string(begin, ' '), "^", std::string(begin - end - 1, '~'), "\nToken matches both '",
                          terminals[foundGroup - 1].first, "' and '", terminals[group - 1].first, "' patterns");
            }
    if (position != string.cend()) {
        auto index = position - string.cbegin();
        THROW(TokenizationError, "no pattern matches symbol at position ", index, ":\n", string, "\n",
              std::string(index, ' '), "^");
    }
    return tokens;
}
}
