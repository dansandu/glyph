#pragma once

#include <ostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::token
{

class PRALINE_EXPORT Token
{
public:
    Token(std::string identifier, int begin, int end) : identifier_{std::move(identifier)}, begin_{begin}, end_{end}
    {
    }

    const std::string& getIdentifier() const
    {
        return identifier_;
    }

    int begin() const
    {
        return begin_;
    }

    int end() const
    {
        return end_;
    }

private:
    std::string identifier_;
    int begin_;
    int end_;
};

PRALINE_EXPORT bool operator==(const Token& left, const Token& right);

PRALINE_EXPORT bool operator!=(const Token& left, const Token& right);

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, const Token& token);

class PRALINE_EXPORT RegexTokenizer
{
public:
    struct Descriptor
    {
        std::string identifier;
        std::string pattern;
    };

    explicit RegexTokenizer(std::vector<Descriptor> descriptors, std::vector<std::string> discarded = {});

    std::vector<Token> operator()(std::string_view string) const;

private:
    std::vector<Descriptor> descriptors_;
    std::vector<std::string> discarded_;
    std::regex pattern_;
};

}
