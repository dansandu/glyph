#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/token.hpp"

#include <ostream>

namespace dansandu::glyph::node
{

class PRALINE_EXPORT Node
{
public:
    friend constexpr auto operator<=>(const Node& left, const Node& right) = default;

    constexpr explicit Node(const dansandu::glyph::token::Token& token) : ruleIndex_{-1}, token_{token}
    {
    }

    constexpr explicit Node(const int ruleIndex) : ruleIndex_{ruleIndex}, token_{{}, 0, 0}
    {
    }

    constexpr bool isRule() const
    {
        return ruleIndex_ != -1;
    }

    constexpr bool isToken() const
    {
        return !isRule();
    }

    constexpr const dansandu::glyph::token::Token& getToken() const
    {
        if (isToken())
        {
            return token_;
        }
        THROW(std::logic_error, "node doesn't hold a token");
    }

    constexpr int getRuleIndex() const
    {
        if (isRule())
        {
            return ruleIndex_;
        }
        THROW(std::logic_error, "node doesn't hold a rule");
    }

private:
    int ruleIndex_;
    dansandu::glyph::token::Token token_;
};

PRALINE_EXPORT std::ostream& operator<<(std::ostream& stream, const Node& node);

}
