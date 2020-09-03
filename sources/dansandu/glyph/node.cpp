#include "dansandu/glyph/node.hpp"
#include "dansandu/ballotin/exception.hpp"

#include <stdexcept>

using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::node
{

Node::Node(const Token& token) : ruleIndex_{-1}, token_{token}
{
}

Node::Node(const int ruleIndex) : ruleIndex_{ruleIndex}, token_{Symbol{}, 0, 0}
{
}

bool Node::isToken() const
{
    return !isRule();
}

bool Node::isRule() const
{
    return ruleIndex_ != -1;
}

const Token& Node::getToken() const
{
    if (isRule())
    {
        THROW(std::runtime_error, "node doesn't hold a token");
    }
    return token_;
}

int Node::getRuleIndex() const
{
    if (isToken())
    {
        THROW(std::runtime_error, "node doesn't hold a rule");
    }
    return ruleIndex_;
}

bool Node::equals(const Node& node) const
{
    return ruleIndex_ == node.ruleIndex_ && token_ == node.token_;
}

bool operator==(const Node& left, const Node& right)
{
    return left.equals(right);
}

bool operator!=(const Node& left, const Node& right)
{
    return !left.equals(right);
}

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    if (node.isToken())
    {
        return stream << node.getToken();
    }
    return stream << "Node(" << node.getRuleIndex() << ")";
}

}
