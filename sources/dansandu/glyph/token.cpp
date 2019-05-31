#include "dansandu/glyph/token.hpp"
#include "dansandu/ballotin/exception.hpp"

#include <ostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::token {

bool operator==(const Token& left, const Token& right) {
    return left.getIdentifier() == right.getIdentifier() && left.begin() == right.begin() && left.end() == right.end();
}

bool operator!=(const Token& left, const Token& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    return stream << "Token(" << token.getIdentifier() << ", " << token.begin() << ", " << token.end() << ")";
}

RegexTokenizer::RegexTokenizer(std::vector<Descriptor> descriptors, std::vector<std::string> discarded)
    : descriptors_{std::move(descriptors)}, discarded_{std::move(discarded)} {
    std::stringstream stream;
    for (const auto& descriptor : descriptors_)
        stream << "(" << descriptor.pattern << ")|";
    auto pattern = stream.str();
    if (!pattern.empty())
        pattern.pop_back();
    pattern_ = std::regex{pattern};
}

std::vector<Token> RegexTokenizer::operator()(std::string_view string) const {
    auto tokens = std::vector<Token>{};
    auto position = string.cbegin();
    auto match = std::cmatch{};
    auto flags = std::regex_constants::match_continuous;
    for (; std::regex_search(position, string.cend(), match, pattern_, flags); position += match.length())
        for (auto group = 1, foundGroup = 0; group < static_cast<int>(match.size()); ++group)
            if (match[group].length()) {
                auto begin = static_cast<int>(match[group].first - string.cbegin());
                auto end = static_cast<int>(match[group].second - string.cbegin());
                if (!foundGroup) {
                    if (std::find(discarded_.cbegin(), discarded_.cend(), descriptors_[group - 1].identifier) ==
                        discarded_.cend())
                        tokens.push_back({descriptors_[group - 1].identifier, begin, end});
                    foundGroup = group;
                } else
                    THROW(TokenizationError, "ambiguous tokenization at position ", begin, ":\n", string, "\n",
                          std::string(begin, ' '), "^", std::string(begin - end - 1, '~'), "\nToken matches both '",
                          descriptors_[foundGroup - 1].identifier, "' and '", descriptors_[group - 1].identifier,
                          "' patterns");
            }
    if (position != string.cend()) {
        auto index = position - string.cbegin();
        THROW(TokenizationError, "no pattern matches symbol at position ", index, ":\n", string, "\n",
              std::string(index, ' '), "^");
    }
    return tokens;
}

}
