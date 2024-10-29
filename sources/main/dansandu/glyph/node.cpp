#include "dansandu/glyph/node.hpp"

#include <sstream>

namespace dansandu::glyph::node
{

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    if (node.isToken())
    {
        return stream << "Node(" << node.getToken() << ")";
    }
    return stream << "Node(Rule(" << node.getRuleIndex() << "))";
}

std::string Node::toString() const
{
    auto stream = std::ostringstream{};

    stream << *this;

    return stream.str();
}

}
