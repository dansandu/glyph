#include "dansandu/glyph/implementation/rule.hpp"
#include "dansandu/ballotin/string.hpp"

using dansandu::ballotin::string::join;

namespace dansandu::glyph::implementation::rule
{

bool operator==(const Rule& left, const Rule& right)
{
    return left.leftSide == right.leftSide && left.rightSide == right.rightSide;
}

bool operator!=(const Rule& left, const Rule& right)
{
    return !(left == right);
}

std::ostream& operator<<(std::ostream& stream, const Rule& rule)
{
    return stream << rule.leftSide << " -> " << join(rule.rightSide, " ");
}

}
