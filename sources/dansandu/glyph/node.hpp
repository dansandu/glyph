#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/token.hpp"

#include <ostream>

namespace dansandu::glyph::node
{

class PRALINE_EXPORT Node
{
    friend bool operator==(const Node& left, const Node& right)
    {
        return left.ruleIndex_ == right.ruleIndex_ && left.token_ == right.token_;
    }

public:
    explicit Node(const dansandu::glyph::token::Token& token) : ruleIndex_{-1}, token_{token}
    {
    }

    explicit Node(const int ruleIndex) : ruleIndex_{ruleIndex}, token_{{}, 0, 0}
    {
    }

    bool isRule() const
    {
        return ruleIndex_ != -1;
    }

    bool isToken() const
    {
        return !isRule();
    }

    const dansandu::glyph::token::Token& getToken() const
    {
        if (isToken())
        {
            return token_;
        }
        THROW(std::logic_error, "node doesn't hold a token");
    }

    int getRuleIndex() const
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

inline bool operator!=(const Node& left, const Node& right)
{
    return !(left == right);
}

inline std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    if (node.isToken())
    {
        return stream << node.getToken();
    }
    return stream << "Node(" << node.getRuleIndex() << ")";
}

}
