#include "dansandu/glyph/node.hpp"
#include "dansandu/ballotin/exception.hpp"

using dansandu::glyph::token::Token;

namespace dansandu::glyph::node
{

constexpr auto ruleSentinel = -1;

Node::Node(Token token) : ruleIndex_{ruleSentinel}, token_{std::move(token)}
{
}

Node::Node(int ruleIndex, std::vector<Node> children)
    : ruleIndex_{ruleIndex}, token_{"", 0, 0}, children_{std::move(children)}
{
}

bool Node::isRule() const
{
    return ruleIndex_ != ruleSentinel;
}

bool Node::isToken() const
{
    return !isRule();
}

int Node::getRuleIndex() const
{
    if (isToken())
        THROW(std::runtime_error, "node doesn't hold a rule");
    return ruleIndex_;
}

const Token& Node::getToken() const
{
    if (isRule())
        THROW(std::runtime_error, "node doesn't hold a token");
    return token_;
}

const Node& Node::getChild(int index) const
{
    return children_.at(index);
}

int Node::getChildrenCount() const
{
    return static_cast<int>(children_.size());
}

bool Node::equals(const Node& node) const
{
    return ruleIndex_ == node.ruleIndex_ && token_ == node.token_ && children_ == node.children_;
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
        return stream << node.getToken();
    stream << "Node(" << node.getRuleIndex();
    for (auto i = 0; i < node.getChildrenCount(); ++i)
        stream << ", " << node.getChild(i);
    return stream << ")";
}

}
