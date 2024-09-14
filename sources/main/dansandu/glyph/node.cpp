#include "dansandu/glyph/node.hpp"

#include <ostream>

namespace dansandu::glyph::node
{

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    if (node.isToken())
    {
        return stream << node.getToken();
    }
    return stream << "Node(" << node.getRuleIndex() << ")";
}

}
