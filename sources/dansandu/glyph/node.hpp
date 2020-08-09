#pragma once

#include "dansandu/glyph/token.hpp"

#include <ostream>

namespace dansandu::glyph::node
{

class PRALINE_EXPORT Node
{
public:
    explicit Node(const dansandu::glyph::token::Token& token);

    explicit Node(int ruleIndex);

    bool isRule() const;

    bool isToken() const;

    const dansandu::glyph::token::Token& getToken() const;

    int getRuleIndex() const;

    bool equals(const Node& node) const;

private:
    int ruleIndex_;
    dansandu::glyph::token::Token token_;
};

PRALINE_EXPORT bool operator==(const Node& left, const Node& right);

PRALINE_EXPORT bool operator!=(const Node& left, const Node& right);

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, const Node& node);

}
