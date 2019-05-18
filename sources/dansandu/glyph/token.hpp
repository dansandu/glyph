#pragma once

#include <ostream>
#include <string>

namespace dansandu::glyph::token {

struct TokenDescriptor {
    std::string identifier;
    std::string pattern;
};

class Token {
public:
    Token(std::string identifier, int begin, int end) : identifier_{std::move(identifier)}, begin_{begin}, end_{end} {}

    const std::string& getIdentifier() const { return identifier_; }

    int begin() const { return begin_; }

    int end() const { return end_; }

private:
    std::string identifier_;
    int begin_;
    int end_;
};

bool operator==(const Token& left, const Token& right);

bool operator!=(const Token& left, const Token& right);

std::ostream& operator<<(std::ostream& stream, const Token& token);

}
