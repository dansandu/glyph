#pragma once
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace dansandu::glyph::implementation::tokenization {

class TokenizationError : public std::runtime_error {
    using runtime_error::runtime_error;
};

struct Token {
    std::string identifier;
    int begin;
    int end;
};

bool operator==(const Token& left, const Token& right);

bool operator!=(const Token& left, const Token& right);

std::ostream& operator<<(std::ostream& stream, const Token& token);

std::string getTerminalsPattern(const std::vector<std::pair<std::string, std::string>>& terminals);

std::vector<Token> tokenize(std::string_view string, const std::vector<std::pair<std::string, std::string>>& terminals,
                            const std::vector<std::string>& discard = {});
}
